#include "mach.h"
#include "rvmbits.h"
#include "codec.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>


char     *src        = NULL;
uint64_t len         = 0;
uint64_t reg[16];
uint64_t *stack      = NULL;
uint32_t stack_len   = 0;
char     vmstate     = V_INAC;
char     exitcode    = 0;


bool checkmagic(char *src, uint64_t len) {
  if (!src || len < 4 ||
      src[0] != 0x7f ||
      src[1] != 0x52 ||
      src[2] != 0x56 ||
      src[3] != 0x4d)
    return false;
  return true;
}


bool parse_rvmhdr(char *src, uint64_t len, rvmhdr *out) {
  if (!src || len < 15 || !out || !checkmagic(src, len))
    return false;
  out->magic[0] = 0x7f;
  out->magic[1] = 0x52;
  out->magic[2] = 0x56;
  out->magic[3] = 0x4d;
  out->abi_ver  = read16(src+4);
  out->type     = read8(src+6);
  out->entry    = read64(src+7);
  return true;
}


const char *statcd_msg(statcd n) {
  switch (n) {
    case S_OK:      return "Ok";
    case S_ERR:     return "Internal error";
    case S_ILL:     return "Illegal instruction";
    case S_INVC:    return "Invalid VM call";
    case S_STOVF:   return "Stack overflow";
    case S_STUND:   return "Stack underflow";
    default:        return "Unknown status";
  }
}


bool vload(char *prog, uint64_t sz) {
  if (!prog || sz == 0 || vmstate != V_PROV)
    return false;
  rvmhdr hdr;
  if (!parse_rvmhdr(prog, sz, &hdr))
    return false;
  if (hdr.abi_ver != RVM_VER)
    return false;
  if (hdr.type != RTYP_EXEC)
    return false;
  reg[RPC] = hdr.entry;
  src = prog;
  len = sz;
  return true;
}


bool vth_init(uint32_t stlen) {
  if (vmstate != V_RUNN)
    return false;
  if (stlen == 0)
    stlen = 1;
  stack = (uint64_t*)malloc(sizeof(uint64_t) * stlen);
  if (!stack)
    return false;
  stack_len = stlen;
  return true;
}


bool vth_free(void) {
  if (vmstate == V_PROV)
    return false;
  if (stack)
    free(stack);
  stack = NULL;
  stack_len = 0;
  for (int i = 0; i < 16; i++)
    reg[i] = 0;
  return true;
}


statcd vpush(uint64_t v) {
  if (!stack || stack_len == 0)
    return S_ERR;
  if (reg[RSP] >= stack_len)
    return S_STOVF;
  stack[reg[RSP]++] = v;
  return S_OK;
}


statcd vpop(uint64_t *o) {
  if (!stack || stack_len == 0 || !o)
    return S_ERR;
  if (reg[RSP] == 0)
    return S_STUND;
  *o = stack[--reg[RSP]];
  return S_OK;
}


statcd vmexec(void) {
  if (len - reg[RPC] < 8)
    return S_ILL;
  uint64_t i = read64(src + reg[RPC]);
  reg[RPC] += 8;

  #define vmcase(n) case (n):
  #define vmbrk()   break
  switch (op(i)) {

    vmcase(OP_NOP) {
      vmbrk();
    }

    vmcase(OP_IVC) {
      return vmcall(m0(i) & 0xffff);
    }

    vmcase(OP_MOV) {
      reg[rA(i)] = reg[rB(i)];
      vmbrk();
    }

    vmcase(OP_MOVI) {
      reg[rA(i)] = m1(i);
      vmbrk();
    }

    default:
      return S_ILL;
  }
  #undef vmcase
  #undef vmbrk

  return S_OK;
}


statcd vmcall(uint16_t ndx) {
  #define dcase(n) case (n):
  #define dbrk()   break
  switch (ndx) {

    dcase(VM_EXIT) {
      exitcode = reg[R0] & 0xff;
      vmstate = V_INAC;
      dbrk();
    }

    default:
      return S_INVC;
  }
  #undef dcase
  #undef dbrk

  return S_OK;
}
