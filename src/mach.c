#include "mach.h"
#include "rvmbits.h"
#include "codec.h"
#include "config.h"
#include "util.h"
#include "thread.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

char     *src             = NULL;
uint64_t len              = 0;
uint64_t *data            = NULL;
uint64_t datalen          = 0;
uint64_t TLOCAL reg[16];
uint64_t TLOCAL *stack    = NULL;
uint32_t TLOCAL stack_len = 0;
char     vmstate          = V_INAC;
char     exec_mode        = X_USER;


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
  if (!src || len < 31 || !out || !checkmagic(src, len))
    return false;
  out->magic[0] = 0x7f;
  out->magic[1] = 0x52;
  out->magic[2] = 0x56;
  out->magic[3] = 0x4d;
  out->abi_ver  = read16(src+4);
  out->type     = read8(src+6);
  out->entry    = read64(src+7);
  out->datoff   = read64(src+15);
  out->datlen   = read64(src+23);
  return true;
}


const char *statcd_msg(statcd n) {
  switch (n) {
    case S_OK:      return "Ok";
    case S_ERR:     return "Internal error";
    case S_PERM:    return "Permission denied";
    case S_ILL:     return "Illegal instruction";
    case S_INVC:    return "Invalid VM call";
    case S_STOVF:   return "Stack overflow";
    case S_STUND:   return "Stack underflow";
    case S_OOB:     return "Out of bounds access";
    default:        return "Unknown status";
  }
}


void dump_regs(void) {
  #define preg(n,s) \
    fprintf(stderr, "  %%" n "  0x%016"V64S"x  %"V64S"d\n", reg[(s)], reg[(s)])
  preg("r0", R0);
  preg("r1", R1);
  preg("r2", R2);
  preg("r3", R3);
  preg("r4", R4);
  preg("r5", R5);
  preg("r6", R6);
  preg("r7", R7);
  preg("r8", R8);
  preg("r9", R9);
  preg("rv", RRV);
  preg("lr", RLR);
  preg("bp", RBP);
  preg("sp", RSP);
  preg("pc", RPC);
  preg("fl", RFL);
  #undef preg
}


bool vload(char *prog, uint64_t sz, uint64_t *main_pc) {
  if (!prog || sz == 0 || !main_pc || vmstate != V_PROV)
    return false;
  rvmhdr hdr;
  if (!parse_rvmhdr(prog, sz, &hdr))
    return false;
  if (hdr.abi_ver != RVM_VER)
    return false;
  if (hdr.type != RTYP_EXEC)
    return false;
  if (hdr.datoff + hdr.datlen * 8 >= sz)
    return false;
  /* Some setup. */
  for (int i = 0; i < 16; i++)
    reg[i] = 0;
  *main_pc = hdr.entry;
  src = prog;
  len = sz;
  /* Load the program data. */
  if (hdr.datlen != 0) {
    data = (uint64_t*)malloc(hdr.datlen * sizeof(uint64_t));
    if (!data)
      return false;
    datalen = hdr.datlen;
    for (uint64_t i = 0; i < datalen; i++)
      data[i] = read64(prog + i * 8);
  }
  return true;
}


bool vth_init(uint32_t stlen, uint64_t start_pc) {
  if (vmstate != V_RUNN)
    return false;
  /* Initialise the stack. */
  if (stlen == 0)
    stlen = 1;
  stack = (uint64_t*)malloc(sizeof(uint64_t) * stlen);
  if (!stack)
    return false;
  stack_len = stlen;
  /* Initialise the registers. */
  for (int i = 0; i < 16; i++)
    reg[i] = 0;
  reg[RPC] = start_pc;
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


void interp_loop(void) {
  /* Immediate exit on error. */
  #define vm_err(s) do { \
      rlog("%s (%d)\n", statcd_msg((s)), (s)); \
      dump_regs();       \
      exit(-1);          \
    } while (0)
  /* For optimisation purposes only. */
  interp_start:
  /* Dispatch the current instruction. */
  if (vmstate == V_RUNN) {
    statcd s = vmexec();
    if (s != S_OK) {
      #ifdef BENCHMARK_
      dump_benchmark();
      #endif
      vm_err(s);
    }
    goto interp_start;
  }
  /* VM is suspended. */
  if (vmstate == V_SUSP)
    goto interp_start;
  #undef vm_err
}


bool run_vm(int argc, char **argv) {
  uvar sz = 0;
  char *bin = util_readbin(argv[0], &sz);
  if (!bin) {
    rlog("Failed to load file: %s\n", argv[0]);
    return false;
  }
  /* The entry point. */
  uint64_t main_pc = 0;
  vmstate = V_PROV;
  if (!vload(bin, sz, &main_pc)) {
    rlog("Failed to load executable image.\n");
    return false;
  }
  vmstate = V_RUNN;
  if (!vth_init(524288, main_pc)) {
    rlog("Failed to initialize thread context.\n");
    return false;
  }
  #ifdef BENCHMARK_
  benchmark_init();
  #endif
  interp_loop();
  #ifdef BENCHMARK_
  dump_benchmark();
  #endif
  vth_free();
  free(bin);
  return true;
}


/* Code for bencmarking. */
#ifdef BENCHMARK_
#  include <time.h>

TLOCAL u64 benchmark_epoch = 0;
TLOCAL u64 benchmark_insts = 0;


u64 read_mclock(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec * BILLION + ts.tv_nsec;
}


void dump_benchmark(void) {
  u64 elapsed = benchmark_curr();
  printf("BENCHMARK RESULTS:\n");
  printf("elapsed time:       %"V64S"u ns\n",    elapsed);
  printf("avg time per instr: %"V64S"u ns\n",    elapsed / benchmark_insts);
  printf("instr count:        %"V64S"u insts\n", benchmark_insts);
  printf("instr rate:         %"V64S"u inst / sec\n", BILLION / (elapsed / benchmark_insts));
}

#endif /* defined(BENCHMARK_) */
