#include "mach.h"
#include "codec.h"
#include "rvmbits.h"
#include "util.h"
#include <stdint.h>
#include <stdio.h>

/* Macro to check if a const rel index is valid. */
#define check_k(n) do { \
    if (len < 8 || reg[RPC] + (n) > len - 8)  \
      return S_OOB;     \
  } while (0)
/* Macro to read a const. Must be preceded by check_k(). */
#define gconst(n) (read64(src + reg[RPC] + (n)))


statcd vmexec(void) {
  if (len < 8 || reg[RPC] > len - 8)
    return S_ILL;

  /* For benchmarking. */
  #ifdef BENCHMARK_
  benchmark_insts++;
  #endif

  /* Fetch the next instruction. */
  uint64_t i = read64(src + reg[RPC]);
  reg[RPC] += 8;

/* Macros for opcode implementation. */
switch ((opcode)op(i)) {
#define vminst(n) case (n):
#define vmbrk()   return S_OK

/* Miscellaneous. */

vminst(OP_NOP) {
  vmbrk();
}

vminst(OP_IVC) {
  return vmcall(im(i) & 0xffff);
}

vminst(OP_HLT) {
  vmstate = V_SUSP;
  rlog("Execution suspended.\n");
  fprintf(stderr,
     "The VM is currently in an idle state.\n"
     "Press ENTER to reset and resume execution.\n"
     "%%pc is 0x%"V64S"x\n",
     reg[RPC]);
  getchar(); /* Wait for ENTER. */
  vmstate = V_RUNN;
  vmbrk();
}

/* Data manipulation. */

vminst(OP_MOV) {
  reg[rA(i)] = reg[rB(i)];
  vmbrk();
}

vminst(OP_MOVI) {
  reg[rA(i)] = im(i);
  vmbrk();
}

vminst(OP_MOVK) {
  check_k(im(i));
  reg[rA(i)] = gconst(im(i));
  vmbrk();
}

vminst(OP_LOD) {
  u64 idx = im(i);
  if (idx >= datalen)
    return S_OOB;
  reg[rA(i)] = data[idx];
  vmbrk();
}

vminst(OP_LODS) {
  u64 idx = im(i);
  if (idx >= reg[RSP] - reg[RBP] || stack_len <= reg[RBP] + idx)
    return S_OOB;
  reg[rA(i)] = stack[reg[RBP] + idx];
  vmbrk();
}

vminst(OP_LODA) {
  u64 idx = reg[RBP] - 3 - im(i);
  if (idx > reg[RBP] || idx < stack[reg[RBP] - 1])
    return S_OOB;
  reg[rA(i)] = stack[idx];
  vmbrk();
}

vminst(OP_LODAR) {
  u64 idx = reg[RBP] - 3 - reg[rB(i)];
  if (idx > reg[RBP] || idx < stack[reg[RBP] - 1])
    return S_OOB;
  reg[rA(i)] = stack[idx];
  vmbrk();
}

vminst(OP_STR) {
  u64 idx = im(i);
  if (idx >= datalen)
    return S_OOB;
  data[idx] = reg[rA(i)];
  vmbrk();
}

vminst(OP_STRS) {
  u64 idx = im(i);
  if (idx >= reg[RSP] - reg[RBP] || stack_len <= reg[RBP] + idx)
    return S_OOB;
  stack[reg[RBP] + idx] = reg[rA(i)];
  vmbrk();
}

vminst(OP_STRA) {
  u64 idx = reg[RBP] - 3 - im(i);
  if (idx > reg[RBP] || idx < stack[reg[RBP] - 1])
    return S_OOB;
  stack[idx] = reg[rA(i)];
  vmbrk();
}

vminst(OP_STRAR) {
  u64 idx = reg[RBP] - 3 - reg[rB(i)];
  if (idx > reg[RBP] || idx < stack[reg[RBP] - 1])
    return S_OOB;
  stack[idx] = reg[rA(i)];
  vmbrk();
}

vminst(OP_SWP) {
  u64 tmp = reg[rA(i)];
  reg[rA(i)] = reg[rB(i)];
  reg[rB(i)] = tmp;
  vmbrk();
}

vminst(OP_PUSH) {
  statcd s = vpush(reg[rA(i)]);
  if (s != S_OK)
    return s;
  vmbrk();
}

vminst(OP_PUSHI) {
  statcd s = vpush(im(i));
  if (s != S_OK)
    return s;
  vmbrk();
}

vminst(OP_PUSHK) {
  check_k(im(i));
  statcd s = vpush(gconst(im(i)));
  if (s != S_OK)
    return s;
  vmbrk();
}

vminst(OP_POP) {
  if (reg[RSP] <= reg[RBP])
    return S_STUND;
  statcd s = vpop(&reg[rA(i)]);
  if (s != S_OK)
    return s;
  vmbrk();
}

/* Integer arithmetic. */

vminst(OP_ADD) {
  reg[rA(i)] = reg[rB(i)] + reg[rC(i)];
  vmbrk();
}

vminst(OP_ADDI) {
  reg[rA(i)] = reg[rB(i)] + im(i);
  vmbrk();
}

vminst(OP_ADDK) {
  check_k(im(i));
  reg[rA(i)] = reg[rB(i)] + gconst(im(i));
  vmbrk();
}

vminst(OP_SUB) {
  reg[rA(i)] = reg[rB(i)] - reg[rC(i)];
  vmbrk();
}

vminst(OP_SUBI) {
  reg[rA(i)] = reg[rB(i)] - im(i);
  vmbrk();
}

vminst(OP_SUBIR) {
  reg[rA(i)] = im(i) - reg[rB(i)];
  vmbrk();
}

vminst(OP_SUBK) {
  check_k(im(i));
  reg[rA(i)] = reg[rB(i)] - gconst(im(i));
  vmbrk();
}

vminst(OP_SUBKR) {
  check_k(im(i));
  reg[rA(i)] = gconst(im(i)) - reg[rB(i)];
  vmbrk();
}

vminst(OP_MUL) {
  reg[rA(i)] = reg[rB(i)] * reg[rC(i)];
  vmbrk();
}

vminst(OP_MULI) {
  reg[rA(i)] = reg[rB(i)] * im(i);
  vmbrk();
}

vminst(OP_MULK) {
  check_k(im(i));
  reg[rA(i)] = reg[rB(i)] * gconst(im(i));
  vmbrk();
}

vminst(OP_IMUL) {
  reg[rA(i)] = (uint64_t)(
      (int64_t)reg[rB(i)] *
      (int64_t)reg[rC(i)]);
  vmbrk();
}

vminst(OP_IMULI) {
  reg[rA(i)] = (uint64_t)(
      (int64_t)reg[rB(i)] *
      (int64_t)im(i));
  vmbrk();
}

vminst(OP_IMULK) {
  check_k(im(i));
  reg[rA(i)] = (uint64_t)(
      (int64_t)reg[rB(i)] *
      (int64_t)gconst(im(i)));
  vmbrk();
}

vminst(OP_DIV) {
  reg[rA(i)] = reg[rB(i)] / reg[rC(i)];
  vmbrk();
}

vminst(OP_DIVI) {
  reg[rA(i)] = reg[rB(i)] / im(i);
  vmbrk();
}

vminst(OP_DIVIR) {
  reg[rA(i)] = im(i) / reg[rB(i)];
  vmbrk();
}

vminst(OP_DIVK) {
  check_k(im(i));
  reg[rA(i)] = reg[rB(i)] / gconst(im(i));
  vmbrk();
}

vminst(OP_DIVKR) {
  check_k(im(i));
  reg[rA(i)] = gconst(im(i)) / reg[rB(i)];
  vmbrk();
}

vminst(OP_IDIV) {
  reg[rA(i)] = (uint64_t)(
      (int64_t)reg[rB(i)] /
      (int64_t)reg[rC(i)]);
  vmbrk();
}

vminst(OP_IDIVI) {
  reg[rA(i)] = (uint64_t)(
      (int64_t)reg[rB(i)] /
      (int64_t)im(i));
  vmbrk();
}

vminst(OP_IDIVIR) {
  reg[rA(i)] = (uint64_t)(
      (int64_t)im(i) /
      (int64_t)reg[rB(i)]);
  vmbrk();
}

vminst(OP_IDIVK) {
  check_k(im(i));
  reg[rA(i)] = (uint64_t)(
      (int64_t)reg[rB(i)] /
      (int64_t)gconst(im(i)));
  vmbrk();
}

vminst(OP_IDIVKR) {
  check_k(im(i));
  reg[rA(i)] = (uint64_t)(
      (int64_t)gconst(im(i)) /
      (int64_t)reg[rB(i)]);
  vmbrk();
}

vminst(OP_MOD) {
  reg[rA(i)] = reg[rB(i)] % reg[rC(i)];
  vmbrk();
}

vminst(OP_MODI) {
  reg[rA(i)] = reg[rB(i)] % im(i);
  vmbrk();
}

vminst(OP_MODIR) {
  reg[rA(i)] = im(i) % reg[rB(i)];
  vmbrk();
}

vminst(OP_MODK) {
  check_k(im(i));
  reg[rA(i)] = reg[rB(i)] % gconst(im(i));
  vmbrk();
}

vminst(OP_MODKR) {
  check_k(im(i));
  reg[rA(i)] = gconst(im(i)) % reg[rB(i)];
  vmbrk();
}

vminst(OP_IMOD) {
  reg[rA(i)] = (uint64_t)(
      (int64_t)reg[rB(i)] %
      (int64_t)reg[rC(i)]);
  vmbrk();
}

vminst(OP_IMODI) {
  reg[rA(i)] = (uint64_t)(
      (int64_t)reg[rB(i)] %
      (int64_t)im(i));
  vmbrk();
}

vminst(OP_IMODIR) {
  reg[rA(i)] = (uint64_t)(
      (int64_t)im(i) %
      (int64_t)reg[rB(i)]);
  vmbrk();
}

vminst(OP_IMODK) {
  check_k(im(i));
  reg[rA(i)] = (uint64_t)(
      (int64_t)reg[rB(i)] %
      (int64_t)gconst(im(i)));
  vmbrk();
}

vminst(OP_IMODKR) {
  check_k(im(i));
  reg[rA(i)] = (uint64_t)(
      (int64_t)gconst(im(i)) %
      (int64_t)reg[rB(i)]);
  vmbrk();
}

vminst(OP_INC) {
  reg[rA(i)]++;
  vmbrk();
}

vminst(OP_DEC) {
  reg[rA(i)]--;
  vmbrk();
}

vminst(OP_NEG) {
  reg[rA(i)] = -reg[rB(i)];
  vmbrk();
}

/* Bitwise ops. */

vminst(OP_AND) {
  reg[rA(i)] = reg[rB(i)] & reg[rC(i)];
  vmbrk();
}

vminst(OP_ANDI) {
  reg[rA(i)] = reg[rB(i)] & im(i);
  vmbrk();
}

vminst(OP_ANDK) {
  check_k(im(i));
  reg[rA(i)] = reg[rB(i)] & gconst(im(i));
  vmbrk();
}

vminst(OP_IOR) {
  reg[rA(i)] = reg[rB(i)] | reg[rC(i)];
  vmbrk();
}

vminst(OP_IORI) {
  reg[rA(i)] = reg[rB(i)] | im(i);
  vmbrk();
}

vminst(OP_IORK) {
  check_k(im(i));
  reg[rA(i)] = reg[rB(i)] | gconst(im(i));
  vmbrk();
}

vminst(OP_XOR) {
  reg[rA(i)] = reg[rB(i)] ^ reg[rC(i)];
  vmbrk();
}

vminst(OP_XORI) {
  reg[rA(i)] = reg[rB(i)] ^ im(i);
  vmbrk();
}

vminst(OP_XORK) {
  check_k(im(i));
  reg[rA(i)] = reg[rB(i)] ^ gconst(im(i));
  vmbrk();
}

vminst(OP_NOT) {
  reg[rA(i)] = ~reg[rB(i)];
  vmbrk();
}

#define do_shift(op, a, b) (reg[rA(i)] = ((b) >= 64) ? 0 : ((a) op (b)))

vminst(OP_SHL) {
  do_shift(<<,
      reg[rB(i)],
      reg[rC(i)]);
  vmbrk();
}

vminst(OP_SHLI) {
  do_shift(<<,
      reg[rB(i)],
      im(i));
  vmbrk();
}

vminst(OP_SHLIR) {
  do_shift(<<,
      im(i),
      reg[rB(i)]);
  vmbrk();
}

vminst(OP_SHR) {
  do_shift(>>,
      reg[rB(i)],
      reg[rC(i)]);
  vmbrk();
}

vminst(OP_SHRI) {
  do_shift(>>,
      reg[rB(i)],
      im(i));
  vmbrk();
}

vminst(OP_SHRIR) {
  do_shift(>>,
      im(i),
      reg[rB(i)]);
  vmbrk();
}

#undef do_shift

vminst(OP_ROL) {
  u64 v = reg[rB(i)];
  u64 c = mod64(reg[rC(i)]);
  reg[rA(i)] = rol64(v, c);
  vmbrk();
}

vminst(OP_ROLI) {
  u64 v = reg[rB(i)];
  u64 c = mod64(im(i));
  reg[rA(i)] = rol64(v, c);
  vmbrk();
}

vminst(OP_ROLIR) {
  u64 v = im(i);
  u64 c = mod64(reg[rB(i)]);
  reg[rA(i)] = rol64(v, c);
  vmbrk();
}

vminst(OP_ROR) {
  u64 v = reg[rB(i)];
  u64 c = mod64(reg[rC(i)]);
  reg[rA(i)] = ror64(v, c);
  vmbrk();
}

vminst(OP_RORI) {
  u64 v = reg[rB(i)];
  u64 c = mod64(im(i));
  reg[rA(i)] = ror64(v, c);
  vmbrk();
}

vminst(OP_RORIR) {
  u64 v = im(i);
  u64 c = mod64(reg[rB(i)]);
  reg[rA(i)] = ror64(v, c);
  vmbrk();
}

/* Bit test */

#define bval_imm() (mod64(im(i)))
#define bval_reg() (mod64(reg[rB(i)]))
#define bt_imm() do { \
    clrf(FZ);   \
    if (bit_tst(reg[rA(i)], bval_imm())) \
      setf(FZ); \
  } while (0)
#define bt_reg() do { \
    clrf(FZ);   \
    if (bit_tst(reg[rA(i)], bval_reg())) \
      setf(FZ); \
  } while (0)

vminst(OP_BT) {
  bt_imm();
  vmbrk();
}

vminst(OP_BTG) {
  bt_reg();
  vmbrk();
}

vminst(OP_BTS) {
  bt_imm();
  reg[rA(i)] = bit_set(reg[rA(i)], bval_imm());
  vmbrk();
}

vminst(OP_BTSG) {
  bt_reg();
  reg[rA(i)] = bit_set(reg[rA(i)], bval_reg());
  vmbrk();
}

vminst(OP_BTR) {
  bt_imm();
  reg[rA(i)] = bit_clr(reg[rA(i)], bval_imm());
  vmbrk();
}

vminst(OP_BTRG) {
  bt_reg();
  reg[rA(i)] = bit_clr(reg[rA(i)], bval_reg());
  vmbrk();
}

vminst(OP_BTC) {
  bt_imm();
  reg[rA(i)] = bit_cml(reg[rA(i)], bval_imm());
  vmbrk();
}

vminst(OP_BTCG) {
  bt_reg();
  reg[rA(i)] = bit_cml(reg[rA(i)], bval_reg());
  vmbrk();
}

#undef bval_imm
#undef bval_reg
#undef bt_imm
#undef bt_reg

/* Flags and conditionals. */

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
    if ((i64)(x) > (i64)(y)) \
      setf(FG);          \
    if ((i64)(x) < (i64)(y)) \
      setf(FL);          \
    if ((x) > (y))       \
      setf(FA);          \
    if ((x) < (y))       \
      setf(FB);          \
    if ((x) == (y))      \
      setf(FQ);          \
  } while (0)


vminst(OP_CMP) {
  do_cmp(reg[rA(i)], reg[rB(i)]);
  vmbrk();
}

vminst(OP_CMPI) {
  do_cmp(reg[rA(i)], im(i));
  vmbrk();
}

vminst(OP_CMPIR) {
  do_cmp(im(i), reg[rA(i)]);
  vmbrk();
}

vminst(OP_CMPK) {
  check_k(im(i));
  u64 k = gconst(im(i));
  do_cmp(reg[rA(i)], k);
  vmbrk();
}

vminst(OP_CMPKR) {
  check_k(im(i));
  u64 k = gconst(im(i));
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

vminst(OP_TEST) {
  do_test(reg[rA(i)], reg[rB(i)]);
  vmbrk();
}

vminst(OP_TESTI) {
  do_test(reg[rA(i)], im(i));
  vmbrk();
}

vminst(OP_TESTK) {
  check_k(im(i));
  u64 k = gconst(im(i));
  do_test(reg[rA(i)], k);
  vmbrk();
}

#undef do_test
#undef sbit

/* Set flags */

vminst(OP_STC) {
  setf(FC);
  vmbrk();
}

vminst(OP_STO) {
  setf(FO);
  vmbrk();
}

vminst(OP_STS) {
  setf(FS);
  vmbrk();
}

vminst(OP_STZ) {
  setf(FZ);
  vmbrk();
}

vminst(OP_STE) {
  setf(FE);
  vmbrk();
}

vminst(OP_STG) {
  setf(FG);
  vmbrk();
}

vminst(OP_STL) {
  setf(FL);
  vmbrk();
}

vminst(OP_STA) {
  setf(FA);
  vmbrk();
}

vminst(OP_STB) {
  setf(FB);
  vmbrk();
}

vminst(OP_STQ) {
  setf(FQ);
  vmbrk();
}

/* Clear flags */

vminst(OP_CLC) {
  clrf(FC);
  vmbrk();
}

vminst(OP_CLO) {
  clrf(FO);
  vmbrk();
}

vminst(OP_CLS) {
  clrf(FS);
  vmbrk();
}

vminst(OP_CLZ) {
  clrf(FZ);
  vmbrk();
}

vminst(OP_CLE) {
  clrf(FE);
  vmbrk();
}

vminst(OP_CLG) {
  clrf(FG);
  vmbrk();
}

vminst(OP_CLL) {
  clrf(FL);
  vmbrk();
}

vminst(OP_CLA) {
  clrf(FA);
  vmbrk();
}

vminst(OP_CLB) {
  clrf(FB);
  vmbrk();
}

vminst(OP_CLQ) {
  clrf(FQ);
  vmbrk();
}

/* Complement flags */

vminst(OP_CMC) {
  cmlf(FC);
  vmbrk();
}

vminst(OP_CMO) {
  cmlf(FO);
  vmbrk();
}

vminst(OP_CMS) {
  cmlf(FS);
  vmbrk();
}

vminst(OP_CMZ) {
  cmlf(FZ);
  vmbrk();
}

vminst(OP_CME) {
  cmlf(FE);
  vmbrk();
}

vminst(OP_CMG) {
  cmlf(FG);
  vmbrk();
}

vminst(OP_CML) {
  cmlf(FL);
  vmbrk();
}

vminst(OP_CMA) {
  cmlf(FA);
  vmbrk();
}

vminst(OP_CMB) {
  cmlf(FB);
  vmbrk();
}

vminst(OP_CMQ) {
  cmlf(FQ);
  vmbrk();
}

/* Branching and flow control. */

#define br_abs()  (reg[RPC]  = im(i))
#define br_rel()  (reg[RPC] += im(i))

vminst(OP_JMP) {
  br_abs();
  vmbrk();
}

vminst(OP_JMPN) {
  br_rel();
  vmbrk();
}

vminst(OP_JE) {
  if (getf(FQ))
    br_rel();
  vmbrk();
}

vminst(OP_JNE) {
  if (!getf(FQ))
    br_rel();
  vmbrk();
}

vminst(OP_JG) {
  if (getf(FG))
    br_rel();
  vmbrk();
}

vminst(OP_JGE) {
  if (getf(FG) || getf(FQ))
    br_rel();
  vmbrk();
}

vminst(OP_JL) {
  if (getf(FL))
    br_rel();
  vmbrk();
}

vminst(OP_JLE) {
  if (getf(FL) || getf(FQ))
    br_rel();
  vmbrk();
}

vminst(OP_JA) {
  if (getf(FA))
    br_rel();
  vmbrk();
}

vminst(OP_JAE) {
  if (getf(FA) || getf(FQ))
    br_rel();
  vmbrk();
}

vminst(OP_JB) {
  if (getf(FB))
    br_rel();
  vmbrk();
}

vminst(OP_JBE) {
  if (getf(FB) || getf(FQ))
    br_rel();
  vmbrk();
}

vminst(OP_JC) {
  if (getf(FC))
    br_rel();
  vmbrk();
}

vminst(OP_JNC) {
  if (!getf(FC))
    br_rel();
  vmbrk();
}

vminst(OP_JO) {
  if (getf(FO))
    br_rel();
  vmbrk();
}

vminst(OP_JNO) {
  if (!getf(FO))
    br_rel();
  vmbrk();
}

vminst(OP_JS) {
  if (getf(FS))
    br_rel();
  vmbrk();
}

vminst(OP_JNS) {
  if (!getf(FS))
    br_rel();
  vmbrk();
}

vminst(OP_JZ) {
  if (getf(FZ))
    br_rel();
  vmbrk();
}

vminst(OP_JNZ) {
  if (!getf(FZ))
    br_rel();
  vmbrk();
}

vminst(OP_JX) {
  if (getf(FE))
    br_rel();
  vmbrk();
}

vminst(OP_JNX) {
  if (!getf(FE))
    br_rel();
  vmbrk();
}

vminst(OP_LOOP) {
  if (--reg[rA(i)] != 0)
    reg[RPC] += im(i);
  vmbrk();
}

vminst(OP_CALL) {
  statcd s;
  s = vpush(reg[RLR]);
  if (s != S_OK)
    return s;
  s = vpush(reg[RBP]);
  if (s != S_OK)
    return s;
  reg[RLR] = reg[RPC];
  reg[RBP] = reg[RSP];
  reg[RPC] = im(i);
  vmbrk();
}

vminst(OP_CALLR) {
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

vminst(OP_RET) {
  subroutine_ret:
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

vminst(OP_THR) {
  setf(FE);
  goto subroutine_ret;
}

#undef br_abs
#undef br_rel

#undef vminst
#undef vmbrk
default: return S_ILL; }
}
