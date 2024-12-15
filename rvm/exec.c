#include "mach.h"
#include "codec.h"
#include "rvmbits.h"
#include "util.h"
#include <stdint.h>
#include <stdlib.h>

/* Prototype for the vm__interpreter() function. */
static statcd vm__interpreter(void);

/* Macro to check if a const rel index is valid. */
#define check_k(n) do { \
    if (len < 8 || reg[RPC] + (n) > len - 8)  \
      return S_OOB;     \
  } while (0)
/* Macro to read a const. Must be preceded by check_k(). */
#define gconst(n) (read64(src + reg[RPC] + (n)))


void vmexec(void) {
  /* vmexec() is a wrapper to vm__interpreter(). */
  benchmark_init();
  statcd s = vm__interpreter();
  benchmark_tag();

  /* Interpreter complete. Do some additional state checks. */
  if (s != S_OK)
    show_err(s);

  if (tid == 0) /* Only dump benchmarks from the main thread. */
    dump_benchmark();

  if (s != S_OK) {
    rlog("Error is unrecoverable. Aborting...\n");
    exit(-1);
  }
}


static statcd vm__interpreter(void) {
  interp_start:

  /* Make sure we're still reading within the bytecode. */
  if (len < 8 || reg[RPC] > len - 8)
    return S_ILL;

  /* For benchmarking. */
  benchmark_incr();

  /* Fetch the next instruction. */
  uint64_t i = read64(src + reg[RPC]);
  reg[RPC] += 8;

/* Macros for opcode implementation. */
switch ((opcode)op(i)) {
#define vminst(n) case (OP_ ## n):
#define vmbrk()   goto interp_start

/* Miscellaneous. */

vminst(NOP) {
  vmbrk();
}

vminst(IVC) {
  statcd s = vmcall(im(i) & 0xffff);
  if (s != S_OK)
    return s;
  vmbrk();
}

vminst(HLT) {
  return S_TERM;
}

/* Data manipulation. */

vminst(MOV) {
  reg[rA(i)] = reg[rB(i)];
  vmbrk();
}

vminst(MOVI) {
  reg[rA(i)] = im(i);
  vmbrk();
}

vminst(MOVK) {
  check_k(im(i));
  reg[rA(i)] = gconst(im(i));
  vmbrk();
}

vminst(LOD) {
  u64 idx = im(i);
  if (idx >= datalen)
    return S_OOB;
  reg[rA(i)] = data[idx];
  vmbrk();
}

vminst(LODS) {
  u64 idx = im(i);
  if (idx >= reg[RSP] - reg[RBP] || stack_len <= reg[RBP] + idx)
    return S_OOB;
  reg[rA(i)] = stack[reg[RBP] + idx];
  vmbrk();
}

vminst(LODA) {
  u64 idx = reg[RBP] - 3 - im(i);
  if (idx > reg[RBP] || idx < stack[reg[RBP] - 1])
    return S_OOB;
  reg[rA(i)] = stack[idx];
  vmbrk();
}

vminst(LODAR) {
  u64 idx = reg[RBP] - 3 - reg[rB(i)];
  if (idx > reg[RBP] || idx < stack[reg[RBP] - 1])
    return S_OOB;
  reg[rA(i)] = stack[idx];
  vmbrk();
}

vminst(STR) {
  u64 idx = im(i);
  if (idx >= datalen)
    return S_OOB;
  data[idx] = reg[rA(i)];
  vmbrk();
}

vminst(STRS) {
  u64 idx = im(i);
  if (idx >= reg[RSP] - reg[RBP] || stack_len <= reg[RBP] + idx)
    return S_OOB;
  stack[reg[RBP] + idx] = reg[rA(i)];
  vmbrk();
}

vminst(STRA) {
  u64 idx = reg[RBP] - 3 - im(i);
  if (idx > reg[RBP] || idx < stack[reg[RBP] - 1])
    return S_OOB;
  stack[idx] = reg[rA(i)];
  vmbrk();
}

vminst(STRAR) {
  u64 idx = reg[RBP] - 3 - reg[rB(i)];
  if (idx > reg[RBP] || idx < stack[reg[RBP] - 1])
    return S_OOB;
  stack[idx] = reg[rA(i)];
  vmbrk();
}

vminst(SWP) {
  u64 tmp = reg[rA(i)];
  reg[rA(i)] = reg[rB(i)];
  reg[rB(i)] = tmp;
  vmbrk();
}

vminst(PUSH) {
  statcd s = vpush(reg[rA(i)]);
  if (s != S_OK)
    return s;
  vmbrk();
}

vminst(PUSHI) {
  statcd s = vpush(im(i));
  if (s != S_OK)
    return s;
  vmbrk();
}

vminst(PUSHK) {
  check_k(im(i));
  statcd s = vpush(gconst(im(i)));
  if (s != S_OK)
    return s;
  vmbrk();
}

vminst(POP) {
  if (reg[RSP] <= reg[RBP])
    return S_STUND;
  statcd s = vpop(&reg[rA(i)]);
  if (s != S_OK)
    return s;
  vmbrk();
}

/* Integer arithmetic. */

vminst(ADD) {
  reg[rA(i)] = reg[rB(i)] + reg[rC(i)];
  vmbrk();
}

vminst(ADDI) {
  reg[rA(i)] = reg[rB(i)] + im(i);
  vmbrk();
}

vminst(ADDK) {
  check_k(im(i));
  reg[rA(i)] = reg[rB(i)] + gconst(im(i));
  vmbrk();
}

vminst(SUB) {
  reg[rA(i)] = reg[rB(i)] - reg[rC(i)];
  vmbrk();
}

vminst(SUBI) {
  reg[rA(i)] = reg[rB(i)] - im(i);
  vmbrk();
}

vminst(SUBIR) {
  reg[rA(i)] = im(i) - reg[rB(i)];
  vmbrk();
}

vminst(SUBK) {
  check_k(im(i));
  reg[rA(i)] = reg[rB(i)] - gconst(im(i));
  vmbrk();
}

vminst(SUBKR) {
  check_k(im(i));
  reg[rA(i)] = gconst(im(i)) - reg[rB(i)];
  vmbrk();
}

vminst(MUL) {
  reg[rA(i)] = reg[rB(i)] * reg[rC(i)];
  vmbrk();
}

vminst(MULI) {
  reg[rA(i)] = reg[rB(i)] * im(i);
  vmbrk();
}

vminst(MULK) {
  check_k(im(i));
  reg[rA(i)] = reg[rB(i)] * gconst(im(i));
  vmbrk();
}

vminst(IMUL) {
  reg[rA(i)] = (uint64_t)(
      (int64_t)reg[rB(i)] *
      (int64_t)reg[rC(i)]);
  vmbrk();
}

vminst(IMULI) {
  reg[rA(i)] = (uint64_t)(
      (int64_t)reg[rB(i)] *
      (int64_t)im(i));
  vmbrk();
}

vminst(IMULK) {
  check_k(im(i));
  reg[rA(i)] = (uint64_t)(
      (int64_t)reg[rB(i)] *
      (int64_t)gconst(im(i)));
  vmbrk();
}

vminst(DIV) {
  reg[rA(i)] = reg[rB(i)] / reg[rC(i)];
  vmbrk();
}

vminst(DIVI) {
  reg[rA(i)] = reg[rB(i)] / im(i);
  vmbrk();
}

vminst(DIVIR) {
  reg[rA(i)] = im(i) / reg[rB(i)];
  vmbrk();
}

vminst(DIVK) {
  check_k(im(i));
  reg[rA(i)] = reg[rB(i)] / gconst(im(i));
  vmbrk();
}

vminst(DIVKR) {
  check_k(im(i));
  reg[rA(i)] = gconst(im(i)) / reg[rB(i)];
  vmbrk();
}

vminst(IDIV) {
  reg[rA(i)] = (uint64_t)(
      (int64_t)reg[rB(i)] /
      (int64_t)reg[rC(i)]);
  vmbrk();
}

vminst(IDIVI) {
  reg[rA(i)] = (uint64_t)(
      (int64_t)reg[rB(i)] /
      (int64_t)im(i));
  vmbrk();
}

vminst(IDIVIR) {
  reg[rA(i)] = (uint64_t)(
      (int64_t)im(i) /
      (int64_t)reg[rB(i)]);
  vmbrk();
}

vminst(IDIVK) {
  check_k(im(i));
  reg[rA(i)] = (uint64_t)(
      (int64_t)reg[rB(i)] /
      (int64_t)gconst(im(i)));
  vmbrk();
}

vminst(IDIVKR) {
  check_k(im(i));
  reg[rA(i)] = (uint64_t)(
      (int64_t)gconst(im(i)) /
      (int64_t)reg[rB(i)]);
  vmbrk();
}

vminst(MOD) {
  reg[rA(i)] = reg[rB(i)] % reg[rC(i)];
  vmbrk();
}

vminst(MODI) {
  reg[rA(i)] = reg[rB(i)] % im(i);
  vmbrk();
}

vminst(MODIR) {
  reg[rA(i)] = im(i) % reg[rB(i)];
  vmbrk();
}

vminst(MODK) {
  check_k(im(i));
  reg[rA(i)] = reg[rB(i)] % gconst(im(i));
  vmbrk();
}

vminst(MODKR) {
  check_k(im(i));
  reg[rA(i)] = gconst(im(i)) % reg[rB(i)];
  vmbrk();
}

vminst(IMOD) {
  reg[rA(i)] = (uint64_t)(
      (int64_t)reg[rB(i)] %
      (int64_t)reg[rC(i)]);
  vmbrk();
}

vminst(IMODI) {
  reg[rA(i)] = (uint64_t)(
      (int64_t)reg[rB(i)] %
      (int64_t)im(i));
  vmbrk();
}

vminst(IMODIR) {
  reg[rA(i)] = (uint64_t)(
      (int64_t)im(i) %
      (int64_t)reg[rB(i)]);
  vmbrk();
}

vminst(IMODK) {
  check_k(im(i));
  reg[rA(i)] = (uint64_t)(
      (int64_t)reg[rB(i)] %
      (int64_t)gconst(im(i)));
  vmbrk();
}

vminst(IMODKR) {
  check_k(im(i));
  reg[rA(i)] = (uint64_t)(
      (int64_t)gconst(im(i)) %
      (int64_t)reg[rB(i)]);
  vmbrk();
}

vminst(INC) {
  reg[rA(i)]++;
  vmbrk();
}

vminst(DEC) {
  reg[rA(i)]--;
  vmbrk();
}

vminst(NEG) {
  reg[rA(i)] = -reg[rB(i)];
  vmbrk();
}

/* Bitwise ops. */

vminst(AND) {
  reg[rA(i)] = reg[rB(i)] & reg[rC(i)];
  vmbrk();
}

vminst(ANDI) {
  reg[rA(i)] = reg[rB(i)] & im(i);
  vmbrk();
}

vminst(ANDK) {
  check_k(im(i));
  reg[rA(i)] = reg[rB(i)] & gconst(im(i));
  vmbrk();
}

vminst(IOR) {
  reg[rA(i)] = reg[rB(i)] | reg[rC(i)];
  vmbrk();
}

vminst(IORI) {
  reg[rA(i)] = reg[rB(i)] | im(i);
  vmbrk();
}

vminst(IORK) {
  check_k(im(i));
  reg[rA(i)] = reg[rB(i)] | gconst(im(i));
  vmbrk();
}

vminst(XOR) {
  reg[rA(i)] = reg[rB(i)] ^ reg[rC(i)];
  vmbrk();
}

vminst(XORI) {
  reg[rA(i)] = reg[rB(i)] ^ im(i);
  vmbrk();
}

vminst(XORK) {
  check_k(im(i));
  reg[rA(i)] = reg[rB(i)] ^ gconst(im(i));
  vmbrk();
}

vminst(NOT) {
  reg[rA(i)] = ~reg[rB(i)];
  vmbrk();
}

#define do_shift(op, a, b) (reg[rA(i)] = ((b) >= 64) ? 0 : ((a) op (b)))

vminst(SHL) {
  do_shift(<<,
      reg[rB(i)],
      reg[rC(i)]);
  vmbrk();
}

vminst(SHLI) {
  do_shift(<<,
      reg[rB(i)],
      im(i));
  vmbrk();
}

vminst(SHLIR) {
  do_shift(<<,
      im(i),
      reg[rB(i)]);
  vmbrk();
}

vminst(SHR) {
  do_shift(>>,
      reg[rB(i)],
      reg[rC(i)]);
  vmbrk();
}

vminst(SHRI) {
  do_shift(>>,
      reg[rB(i)],
      im(i));
  vmbrk();
}

vminst(SHRIR) {
  do_shift(>>,
      im(i),
      reg[rB(i)]);
  vmbrk();
}

#undef do_shift

vminst(ROL) {
  u64 v = reg[rB(i)];
  u64 c = mod64(reg[rC(i)]);
  reg[rA(i)] = rol64(v, c);
  vmbrk();
}

vminst(ROLI) {
  u64 v = reg[rB(i)];
  u64 c = mod64(im(i));
  reg[rA(i)] = rol64(v, c);
  vmbrk();
}

vminst(ROLIR) {
  u64 v = im(i);
  u64 c = mod64(reg[rB(i)]);
  reg[rA(i)] = rol64(v, c);
  vmbrk();
}

vminst(ROR) {
  u64 v = reg[rB(i)];
  u64 c = mod64(reg[rC(i)]);
  reg[rA(i)] = ror64(v, c);
  vmbrk();
}

vminst(RORI) {
  u64 v = reg[rB(i)];
  u64 c = mod64(im(i));
  reg[rA(i)] = ror64(v, c);
  vmbrk();
}

vminst(RORIR) {
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

vminst(BT) {
  bt_imm();
  vmbrk();
}

vminst(BTG) {
  bt_reg();
  vmbrk();
}

vminst(BTS) {
  bt_imm();
  reg[rA(i)] = bit_set(reg[rA(i)], bval_imm());
  vmbrk();
}

vminst(BTSG) {
  bt_reg();
  reg[rA(i)] = bit_set(reg[rA(i)], bval_reg());
  vmbrk();
}

vminst(BTR) {
  bt_imm();
  reg[rA(i)] = bit_clr(reg[rA(i)], bval_imm());
  vmbrk();
}

vminst(BTRG) {
  bt_reg();
  reg[rA(i)] = bit_clr(reg[rA(i)], bval_reg());
  vmbrk();
}

vminst(BTC) {
  bt_imm();
  reg[rA(i)] = bit_cml(reg[rA(i)], bval_imm());
  vmbrk();
}

vminst(BTCG) {
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


vminst(CMP) {
  do_cmp(reg[rA(i)], reg[rB(i)]);
  vmbrk();
}

vminst(CMPI) {
  do_cmp(reg[rA(i)], im(i));
  vmbrk();
}

vminst(CMPIR) {
  do_cmp(im(i), reg[rA(i)]);
  vmbrk();
}

vminst(CMPK) {
  check_k(im(i));
  u64 k = gconst(im(i));
  do_cmp(reg[rA(i)], k);
  vmbrk();
}

vminst(CMPKR) {
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

vminst(TEST) {
  do_test(reg[rA(i)], reg[rB(i)]);
  vmbrk();
}

vminst(TESTI) {
  do_test(reg[rA(i)], im(i));
  vmbrk();
}

vminst(TESTK) {
  check_k(im(i));
  u64 k = gconst(im(i));
  do_test(reg[rA(i)], k);
  vmbrk();
}

#undef do_test
#undef sbit

/* Set flags */

vminst(STC) {
  setf(FC);
  vmbrk();
}

vminst(STO) {
  setf(FO);
  vmbrk();
}

vminst(STS) {
  setf(FS);
  vmbrk();
}

vminst(STZ) {
  setf(FZ);
  vmbrk();
}

vminst(STE) {
  setf(FE);
  vmbrk();
}

vminst(STG) {
  setf(FG);
  vmbrk();
}

vminst(STL) {
  setf(FL);
  vmbrk();
}

vminst(STA) {
  setf(FA);
  vmbrk();
}

vminst(STB) {
  setf(FB);
  vmbrk();
}

vminst(STQ) {
  setf(FQ);
  vmbrk();
}

/* Clear flags */

vminst(CLC) {
  clrf(FC);
  vmbrk();
}

vminst(CLO) {
  clrf(FO);
  vmbrk();
}

vminst(CLS) {
  clrf(FS);
  vmbrk();
}

vminst(CLZ) {
  clrf(FZ);
  vmbrk();
}

vminst(CLE) {
  clrf(FE);
  vmbrk();
}

vminst(CLG) {
  clrf(FG);
  vmbrk();
}

vminst(CLL) {
  clrf(FL);
  vmbrk();
}

vminst(CLA) {
  clrf(FA);
  vmbrk();
}

vminst(CLB) {
  clrf(FB);
  vmbrk();
}

vminst(CLQ) {
  clrf(FQ);
  vmbrk();
}

/* Complement flags */

vminst(CMC) {
  cmlf(FC);
  vmbrk();
}

vminst(CMO) {
  cmlf(FO);
  vmbrk();
}

vminst(CMS) {
  cmlf(FS);
  vmbrk();
}

vminst(CMZ) {
  cmlf(FZ);
  vmbrk();
}

vminst(CME) {
  cmlf(FE);
  vmbrk();
}

vminst(CMG) {
  cmlf(FG);
  vmbrk();
}

vminst(CML) {
  cmlf(FL);
  vmbrk();
}

vminst(CMA) {
  cmlf(FA);
  vmbrk();
}

vminst(CMB) {
  cmlf(FB);
  vmbrk();
}

vminst(CMQ) {
  cmlf(FQ);
  vmbrk();
}

/* Branching and flow control. */

#define br_abs()  (reg[RPC]  = im(i))
#define br_rel()  (reg[RPC] += im(i))

vminst(JMP) {
  br_abs();
  vmbrk();
}

vminst(JMPN) {
  br_rel();
  vmbrk();
}

vminst(JE) {
  if (getf(FQ))
    br_rel();
  vmbrk();
}

vminst(JNE) {
  if (!getf(FQ))
    br_rel();
  vmbrk();
}

vminst(JG) {
  if (getf(FG))
    br_rel();
  vmbrk();
}

vminst(JGE) {
  if (getf(FG) || getf(FQ))
    br_rel();
  vmbrk();
}

vminst(JL) {
  if (getf(FL))
    br_rel();
  vmbrk();
}

vminst(JLE) {
  if (getf(FL) || getf(FQ))
    br_rel();
  vmbrk();
}

vminst(JA) {
  if (getf(FA))
    br_rel();
  vmbrk();
}

vminst(JAE) {
  if (getf(FA) || getf(FQ))
    br_rel();
  vmbrk();
}

vminst(JB) {
  if (getf(FB))
    br_rel();
  vmbrk();
}

vminst(JBE) {
  if (getf(FB) || getf(FQ))
    br_rel();
  vmbrk();
}

vminst(JC) {
  if (getf(FC))
    br_rel();
  vmbrk();
}

vminst(JNC) {
  if (!getf(FC))
    br_rel();
  vmbrk();
}

vminst(JO) {
  if (getf(FO))
    br_rel();
  vmbrk();
}

vminst(JNO) {
  if (!getf(FO))
    br_rel();
  vmbrk();
}

vminst(JS) {
  if (getf(FS))
    br_rel();
  vmbrk();
}

vminst(JNS) {
  if (!getf(FS))
    br_rel();
  vmbrk();
}

vminst(JZ) {
  if (getf(FZ))
    br_rel();
  vmbrk();
}

vminst(JNZ) {
  if (!getf(FZ))
    br_rel();
  vmbrk();
}

vminst(JX) {
  if (getf(FE))
    br_rel();
  vmbrk();
}

vminst(JNX) {
  if (!getf(FE))
    br_rel();
  vmbrk();
}

vminst(LOOP) {
  if (--reg[rA(i)] != 0)
    reg[RPC] += im(i);
  vmbrk();
}

vminst(CALL) {
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

vminst(CALLR) {
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

vminst(RET) {
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

vminst(THR) {
  setf(FE);
  goto subroutine_ret;
}

#undef br_abs
#undef br_rel

#undef vminst
#undef vmbrk
    default: return S_ILL;
  }
}
