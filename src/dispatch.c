#include "mach.h"
#include "codec.h"
#include "rvmbits.h"
#include <stdint.h>

/* Macro to check if a const rel index is valid. */
#define check_k(n) do {            \
    if (len - reg[RPC] - (n) < 8)  \
      return S_OOB;                \
  } while (0)
/* Macro to read a const. Must be preceded by check_k(). */
#define gconst(n) (read64(src + reg[RPC] + (n)))


statcd vmexec(void) {
  if (len < reg[RPC] + 8)
    return S_ILL;

  register uint64_t i = read64(src + reg[RPC]);
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

    vmcase(OP_MOVK) {
      check_k(m1(i));
      reg[rA(i)] = gconst(m1(i));
      vmbrk();
    }

    vmcase(OP_LOD) {
      u64 idx = m1(i);
      if (idx >= datalen)
        return S_OOB;
      reg[rA(i)] = data[idx];
      vmbrk();
    }

    vmcase(OP_LODS) {
      u64 idx = m1(i);
      if (idx >= reg[RSP] - reg[RBP] || stack_len <= reg[RBP] + idx)
        return S_OOB;
      reg[rA(i)] = stack[reg[RBP] + idx];
      vmbrk();
    }

    vmcase(OP_LODA) {
      u64 idx = reg[RBP] - 3 - m1(i);
      if (idx > reg[RBP] || idx < stack[reg[RBP] - 1])
        return S_OOB;
      reg[rA(i)] = stack[idx];
      vmbrk();
    }

    vmcase(OP_STR) {
      u64 idx = m1(i);
      if (idx >= datalen)
        return S_OOB;
      data[idx] = reg[rA(i)];
      vmbrk();
    }

    vmcase(OP_STRS) {
      u64 idx = m1(i);
      if (idx >= reg[RSP] - reg[RBP] || stack_len <= reg[RBP] + idx)
        return S_OOB;
      stack[reg[RBP] + idx] = reg[rA(i)];
      vmbrk();
    }

    vmcase(OP_STRA) {
      u64 idx = reg[RBP] - 3 - m1(i);
      if (idx > reg[RBP] || idx < stack[reg[RBP] - 1])
        return S_OOB;
      stack[idx] = reg[rA(i)];
      vmbrk();
    }

    vmcase(OP_SWP) {
      u64 tmp = reg[rA(i)];
      reg[rA(i)] = reg[rB(i)];
      reg[rB(i)] = tmp;
      vmbrk();
    }

    vmcase(OP_PUSH) {
      statcd s = vpush(reg[rA(i)]);
      if (s != S_OK)
        return s;
      vmbrk();
    }

    vmcase(OP_PUSHI) {
      statcd s = vpush(m0(i));
      if (s != S_OK)
        return s;
      vmbrk();
    }

    vmcase(OP_PUSHK) {
      check_k(m0(i));
      statcd s = vpush(gconst(m0(i)));
      if (s != S_OK)
        return s;
      vmbrk();
    }

    vmcase(OP_POP) {
      if (reg[RSP] <= reg[RBP])
        return S_STUND;
      statcd s = vpop(&reg[rA(i)]);
      if (s != S_OK)
        return s;
      vmbrk();
    }

    vmcase(OP_ADD) {
      reg[rA(i)] = reg[rB(i)] + reg[rC(i)];
      vmbrk();
    }

    vmcase(OP_ADDK) {
      check_k(m2(i));
      reg[rA(i)] = reg[rB(i)] + gconst(m2(i));
      vmbrk();
    }

    vmcase(OP_SUB) {
      reg[rA(i)] = reg[rB(i)] - reg[rC(i)];
      vmbrk();
    }

    vmcase(OP_SUBK) {
      check_k(m2(i));
      reg[rA(i)] = reg[rB(i)] - gconst(m2(i));
      vmbrk();
    }

    vmcase(OP_SUBKR) {
      check_k(m2(i));
      reg[rA(i)] = gconst(m2(i)) - reg[rB(i)];
      vmbrk();
    }

    vmcase(OP_MUL) {
      reg[rA(i)] = reg[rB(i)] * reg[rC(i)];
      vmbrk();
    }

    vmcase(OP_MULK) {
      check_k(m2(i));
      reg[rA(i)] = reg[rB(i)] * gconst(m2(i));
      vmbrk();
    }

    vmcase(OP_IMUL) {
      reg[rA(i)] = (uint64_t)(
          (int64_t)reg[rB(i)] *
          (int64_t)reg[rC(i)]);
      vmbrk();
    }

    vmcase(OP_IMULK) {
      check_k(m2(i));
      reg[rA(i)] = (uint64_t)(
          (int64_t)reg[rB(i)] *
          (int64_t)gconst(m2(i))
        );
      vmbrk();
    }

    vmcase(OP_DIV) {
      reg[rA(i)] = reg[rB(i)] / reg[rC(i)];
      vmbrk();
    }

    vmcase(OP_DIVK) {
      check_k(m2(i));
      reg[rA(i)] = reg[rB(i)] / gconst(m2(i));
      vmbrk();
    }

    vmcase(OP_DIVKR) {
      check_k(m2(i));
      reg[rA(i)] = gconst(m2(i)) / reg[rB(i)];
      vmbrk();
    }

    vmcase(OP_IDIV) {
      reg[rA(i)] = (uint64_t)(
          (int64_t)reg[rB(i)] /
          (int64_t)reg[rC(i)]);
      vmbrk();
    }

    vmcase(OP_IDIVK) {
      check_k(m2(i));
      reg[rA(i)] = (uint64_t)(
          (int64_t)reg[rB(i)] /
          (int64_t)gconst(m2(i)));
      vmbrk();
    }

    vmcase(OP_IDIVKR) {
      check_k(m2(i));
      reg[rA(i)] = (uint64_t)(
          (int64_t)gconst(m2(i)) /
          (int64_t)reg[rB(i)]);
      vmbrk();
    }

    vmcase(OP_MOD) {
      reg[rA(i)] = reg[rB(i)] % reg[rC(i)];
      vmbrk();
    }

    vmcase(OP_MODK) {
      check_k(m2(i));
      reg[rA(i)] = reg[rB(i)] % gconst(m2(i));
      vmbrk();
    }

    vmcase(OP_MODKR) {
      check_k(m2(i));
      reg[rA(i)] = gconst(m2(i)) % reg[rB(i)];
      vmbrk();
    }

    vmcase(OP_IMOD) {
      reg[rA(i)] = (uint64_t)(
          (int64_t)reg[rB(i)] %
          (int64_t)reg[rC(i)]);
      vmbrk();
    }

    vmcase(OP_IMODK) {
      check_k(m2(i));
      reg[rA(i)] = (uint64_t)(
          (int64_t)reg[rB(i)] %
          (int64_t)gconst(m2(i)));
      vmbrk();
    }

    vmcase(OP_IMODKR) {
      check_k(m2(i));
      reg[rA(i)] = (uint64_t)(
          (int64_t)gconst(m2(i)) %
          (int64_t)reg[rB(i)]);
      vmbrk();
    }

    vmcase(OP_INC) {
      reg[rA(i)]++;
      vmbrk();
    }

    vmcase(OP_DEC) {
      reg[rA(i)]--;
      vmbrk();
    }

    vmcase(OP_NEG) {
      reg[rA(i)] = -reg[rB(i)];
      vmbrk();
    }

    vmcase(OP_AND) {
      reg[rA(i)] = reg[rB(i)] & reg[rC(i)];
      vmbrk();
    }

    vmcase(OP_ANDK) {
      check_k(m2(i));
      reg[rA(i)] = reg[rB(i)] & gconst(m2(i));
      vmbrk();
    }

    vmcase(OP_IOR) {
      reg[rA(i)] = reg[rB(i)] | reg[rC(i)];
      vmbrk();
    }

    vmcase(OP_IORK) {
      check_k(m2(i));
      reg[rA(i)] = reg[rB(i)] | gconst(m2(i));
      vmbrk();
    }

    vmcase(OP_XOR) {
      reg[rA(i)] = reg[rB(i)] ^ reg[rC(i)];
      vmbrk();
    }

    vmcase(OP_XORK) {
      check_k(m2(i));
      reg[rA(i)] = reg[rB(i)] ^ gconst(m2(i));
      vmbrk();
    }

    vmcase(OP_NOT) {
      reg[rA(i)] = ~reg[rB(i)];
      vmbrk();
    }

    vmcase(OP_SHL) {
      reg[rA(i)] = reg[rB(i)] << reg[rC(i)];
      vmbrk();
    }

    vmcase(OP_SHLK) {
      check_k(m2(i));
      reg[rA(i)] = reg[rB(i)] << gconst(m2(i));
      vmbrk();
    }

    vmcase(OP_SHLKR) {
      check_k(m2(i));
      reg[rA(i)] = gconst(m2(i)) << reg[rB(i)];
      vmbrk();
    }

    vmcase(OP_SHR) {
      reg[rA(i)] = reg[rB(i)] >> reg[rC(i)];
      vmbrk();
    }

    vmcase(OP_SHRK) {
      check_k(m2(i));
      reg[rA(i)] = reg[rB(i)] >> gconst(m2(i));
      vmbrk();
    }

    vmcase(OP_SHRKR) {
      check_k(m2(i));
      reg[rA(i)] = gconst(m2(i)) >> reg[rB(i)];
      vmbrk();
    }

    vmcase(OP_ROL) {
      u64 v = reg[rB(i)];
      u64 c = reg[rC(i)] % 64;
      reg[rA(i)] = (v << c) | (v >> (64-c));
      vmbrk();
    }

    vmcase(OP_ROLK) {
      check_k(m2(i));
      u64 v = reg[rB(i)];
      u64 c = gconst(m2(i)) % 64;
      reg[rA(i)] = (v << c) | (v >> (64-c));
      vmbrk();
    }

    vmcase(OP_ROLKR) {
      check_k(m2(i));
      u64 v = gconst(m2(i));
      u64 c = reg[rB(i)] % 64;
      reg[rA(i)] = (v << c) | (v >> (64-c));
      vmbrk();
    }

    vmcase(OP_ROR) {
      u64 v = reg[rB(i)];
      u64 c = reg[rC(i)] % 64;
      reg[rA(i)] = (v >> c) | (v << (64-c));
      vmbrk();
    }

    vmcase(OP_RORK) {
      check_k(m2(i));
      u64 v = reg[rB(i)];
      u64 c = gconst(m2(i)) % 64;
      reg[rA(i)] = (v >> c) | (v << (64-c));
      vmbrk();
    }

    vmcase(OP_RORKR) {
      check_k(m2(i));
      u64 v = gconst(m2(i));
      u64 c = reg[rB(i)] % 64;
      reg[rA(i)] = (v >> c) | (v << (64-c));
      vmbrk();
    }

    #define sbit(x) ((x)>>63)
    #define do_cmp(x,y) do { \
        u64 r = (x) - (y);   \
        reg[RFL] = 0;        \
        if ((x) < (y))       \
          setf(FC);          \
        if (sbit(x) != sbit(y) && sbit(r) == sbit(y)) \
          setf(FO);          \
        if (sbit(r))         \
          setf(FS);          \
        if (r == 0)          \
          setf(FZ);          \
        if ((x) > (y))       \
          setf(FG);          \
        if ((x) < (y))       \
          setf(FL);          \
        if ((x) == (y))      \
          setf(FQ);          \
      } while (0)


    vmcase(OP_CMP) {
      do_cmp(reg[rA(i)], reg[rB(i)]);
      vmbrk();
    }

    vmcase(OP_CMPK) {
      check_k(m1(i));
      u64 k = gconst(m1(i));
      do_cmp(reg[rA(i)], k);
      vmbrk();
    }

    vmcase(OP_CMPKR) {
      check_k(m1(i));
      u64 k = gconst(m1(i));
      do_cmp(k, reg[rA(i)]);
      vmbrk();
    }

    #undef do_cmp
    #define do_test(x,y) do { \
        u64 r = (x) & (y);    \
        reg[RFL] = 0;         \
        if (sbit(r))          \
          setf(FS);           \
        if (r == 0)           \
          setf(FZ);           \
        if ((x) == (y))       \
          setf(FQ);           \
      } while (0)

    vmcase(OP_TEST) {
      do_test(reg[rA(i)], reg[rB(i)]);
      vmbrk();
    }

    vmcase(OP_TESTK) {
      check_k(m1(i));
      u64 k = gconst(m1(i));
      do_test(reg[rA(i)], k);
      vmbrk();
    }

    #undef do_test
    #undef sbit

    vmcase(OP_STC) {
      setf(FC);
      vmbrk();
    }

    vmcase(OP_STO) {
      setf(FO);
      vmbrk();
    }

    vmcase(OP_STS) {
      setf(FS);
      vmbrk();
    }

    vmcase(OP_STZ) {
      setf(FZ);
      vmbrk();
    }

    vmcase(OP_STE) {
      setf(FE);
      vmbrk();
    }

    vmcase(OP_STG) {
      setf(FG);
      vmbrk();
    }

    vmcase(OP_STL) {
      setf(FL);
      vmbrk();
    }

    vmcase(OP_STQ) {
      setf(FQ);
      vmbrk();
    }

    vmcase(OP_CLC) {
      clrf(FC);
      vmbrk();
    }

    vmcase(OP_CLO) {
      clrf(FO);
      vmbrk();
    }

    vmcase(OP_CLS) {
      clrf(FS);
      vmbrk();
    }

    vmcase(OP_CLZ) {
      clrf(FZ);
      vmbrk();
    }

    vmcase(OP_CLE) {
      clrf(FE);
      vmbrk();
    }

    vmcase(OP_CLG) {
      clrf(FG);
      vmbrk();
    }

    vmcase(OP_CLL) {
      clrf(FL);
      vmbrk();
    }

    vmcase(OP_CLQ) {
      clrf(FQ);
      vmbrk();
    }

    #define br_abs()  (reg[RPC]  = m0(i))
    #define br_rel()  (reg[RPC] += sgx0(m0(i)))

    vmcase(OP_JMP) {
      br_abs();
      vmbrk();
    }

    vmcase(OP_JE) {
      if (getf(FQ))
        br_rel();
      vmbrk();
    }

    vmcase(OP_JNE) {
      if (!getf(FQ))
        br_rel();
      vmbrk();
    }

    vmcase(OP_JG) {
      if (getf(FG))
        br_rel();
      vmbrk();
    }

    vmcase(OP_JGE) {
      if (getf(FG) || getf(FQ))
        br_rel();
      vmbrk();
    }

    vmcase(OP_JL) {
      if (getf(FL))
        br_rel();
      vmbrk();
    }

    vmcase(OP_JLE) {
      if (getf(FL) || getf(FQ))
        br_rel();
      vmbrk();
    }

    vmcase(OP_JC) {
      if (getf(FC))
        br_rel();
      vmbrk();
    }

    vmcase(OP_JNC) {
      if (!getf(FC))
        br_rel();
      vmbrk();
    }

    vmcase(OP_JO) {
      if (getf(FO))
        br_rel();
      vmbrk();
    }

    vmcase(OP_JNO) {
      if (!getf(FO))
        br_rel();
      vmbrk();
    }

    vmcase(OP_JS) {
      if (getf(FS))
        br_rel();
      vmbrk();
    }

    vmcase(OP_JNS) {
      if (!getf(FS))
        br_rel();
      vmbrk();
    }

    vmcase(OP_JZ) {
      if (getf(FZ))
        br_rel();
      vmbrk();
    }

    vmcase(OP_JNZ) {
      if (!getf(FZ))
        br_rel();
      vmbrk();
    }

    vmcase(OP_JX) {
      if (getf(FE))
        br_rel();
      vmbrk();
    }

    vmcase(OP_JNX) {
      if (!getf(FE))
        br_rel();
      vmbrk();
    }

    vmcase(OP_CALL) {
      statcd s;
      s = vpush(reg[RLR]);
      if (s != S_OK)
        return s;
      s = vpush(reg[RBP]);
      if (s != S_OK)
        return s;
      reg[RLR] = reg[RPC];
      reg[RBP] = reg[RSP];
      reg[RPC] = m0(i);
      vmbrk();
    }

    vmcase(OP_CALLR) {
      statcd s;
      s = vpush(reg[RLR]);
      if (s != S_OK)
        return s;
      s = vpush(reg[RBP]);
      if (s != S_OK)
        return s;
      reg[RLR] = reg[RPC];
      reg[RBP] = reg[RSP];
      reg[RPC] = reg[rA(i)];
      vmbrk();
    }

    vmcase(OP_RET) {
      op_ret:
      reg[RSP] = reg[RBP];
      reg[RPC] = reg[RLR];
      statcd s;
      s = vpop(&reg[RBP]);
      if (s != S_OK)
        return s;
      s = vpop(&reg[RLR]);
      if (s != S_OK)
        return s;
      vmbrk();
    }

    vmcase(OP_THR) {
      setf(FE);
      goto op_ret;
    }

    #undef br_abs
    #undef br_rel

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
