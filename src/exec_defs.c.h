/* This file should only be included by exec.c */


/* Some editors complain about vminst() missing. */
#ifndef RVM_INTERP_
#  define vminst(n, c...)
#endif


/* Miscellaneous. */

vminst(NOP, {
  inext();
})

vminst(TRAP, {
  stop(im(i) & 0xffff);
})

vminst(HLT, {
  stop(E_TERM);
})

/* Data manipulation. */

vminst(MOV, {
  reg[rA(i)] = reg[rB(i)];
  inext();
})

vminst(MOVI, {
  reg[rA(i)] = im(i);
  inext();
})

vminst(LOD, {
  u64 idx = im(i);
  if (idx >= datalen)
    stop(E_OOB);
  reg[rA(i)] = read64(data + idx);
  inext();
})

vminst(LODS, {
  u64 idx = im(i);
  if (idx >= sp - bp || stack_len <= bp + idx)
    stop(E_OOB);
  reg[rA(i)] = stack[bp + idx];
  inext();
})

vminst(LODA, {
  u64 idx = bp - 3 - im(i);
  if (idx > bp || idx < stack[bp - 1])
    stop(E_OOB);
  reg[rA(i)] = stack[idx];
  inext();
})

vminst(LODAR, {
  u64 idx = bp - 3 - reg[rB(i)];
  if (idx > bp || idx < stack[bp - 1])
    stop(E_OOB);
  reg[rA(i)] = stack[idx];
  inext();
})

vminst(STR, {
  u64 idx = im(i);
  if (idx >= datalen)
    stop(E_OOB);
  write64((char*)(void*)(data + idx), reg[rA(i)]);
  inext();
})

vminst(STRS, {
  u64 idx = im(i);
  if (idx >= sp - bp || stack_len <= bp + idx)
    stop(E_OOB);
  stack[bp + idx] = reg[rA(i)];
  inext();
})

vminst(STRA, {
  u64 idx = bp - 3 - im(i);
  if (idx > bp || idx < stack[bp - 1])
    stop(E_OOB);
  stack[idx] = reg[rA(i)];
  inext();
})

vminst(STRAR, {
  u64 idx = bp - 3 - reg[rB(i)];
  if (idx > bp || idx < stack[bp - 1])
    stop(E_OOB);
  stack[idx] = reg[rA(i)];
  inext();
})

vminst(SWP, {
  u64 tmp = reg[rA(i)];
  reg[rA(i)] = reg[rB(i)];
  reg[rB(i)] = tmp;
  inext();
})

vminst(PUSH, {
  push(reg[rA(i)]);
  inext();
})

vminst(PUSHI, {
  push(im(i));
  inext();
})

vminst(POP, {
  if (sp <= bp)
    stop(E_STUND);
  pop(reg[rA(i)]);
  inext();
})

/* Integer arithmetic. */

vminst(ADD, {
  reg[rA(i)] = reg[rB(i)] + reg[rC(i)];
  inext();
})

vminst(ADDI, {
  reg[rA(i)] = reg[rB(i)] + im(i);
  inext();
})

vminst(SUB, {
  reg[rA(i)] = reg[rB(i)] - reg[rC(i)];
  inext();
})

vminst(SUBI, {
  reg[rA(i)] = reg[rB(i)] - im(i);
  inext();
})

vminst(SUBIR, {
  reg[rA(i)] = im(i) - reg[rB(i)];
  inext();
})

vminst(MUL, {
  reg[rA(i)] = reg[rB(i)] * reg[rC(i)];
  inext();
})

vminst(MULI, {
  reg[rA(i)] = reg[rB(i)] * im(i);
  inext();
})

vminst(IMUL, {
  reg[rA(i)] = (uint64_t)(
      (int64_t)reg[rB(i)] *
      (int64_t)reg[rC(i)]);
  inext();
})

vminst(IMULI, {
  reg[rA(i)] = (uint64_t)(
      (int64_t)reg[rB(i)] *
      (int64_t)im(i));
  inext();
})

vminst(DIV, {
  reg[rA(i)] = reg[rB(i)] / reg[rC(i)];
  inext();
})

vminst(DIVI, {
  reg[rA(i)] = reg[rB(i)] / im(i);
  inext();
})

vminst(DIVIR, {
  reg[rA(i)] = im(i) / reg[rB(i)];
  inext();
})

vminst(IDIV, {
  reg[rA(i)] = (uint64_t)(
      (int64_t)reg[rB(i)] /
      (int64_t)reg[rC(i)]);
  inext();
})

vminst(IDIVI, {
  reg[rA(i)] = (uint64_t)(
      (int64_t)reg[rB(i)] /
      (int64_t)im(i));
  inext();
})

vminst(IDIVIR, {
  reg[rA(i)] = (uint64_t)(
      (int64_t)im(i) /
      (int64_t)reg[rB(i)]);
  inext();
})

vminst(MOD, {
  reg[rA(i)] = reg[rB(i)] % reg[rC(i)];
  inext();
})

vminst(MODI, {
  reg[rA(i)] = reg[rB(i)] % im(i);
  inext();
})

vminst(MODIR, {
  reg[rA(i)] = im(i) % reg[rB(i)];
  inext();
})

vminst(IMOD, {
  reg[rA(i)] = (uint64_t)(
      (int64_t)reg[rB(i)] %
      (int64_t)reg[rC(i)]);
  inext();
})

vminst(IMODI, {
  reg[rA(i)] = (uint64_t)(
      (int64_t)reg[rB(i)] %
      (int64_t)im(i));
  inext();
})

vminst(IMODIR, {
  reg[rA(i)] = (uint64_t)(
      (int64_t)im(i) %
      (int64_t)reg[rB(i)]);
  inext();
})

vminst(INC, {
  reg[rA(i)]++;
  inext();
})

vminst(DEC, {
  reg[rA(i)]--;
  inext();
})

vminst(NEG, {
  reg[rA(i)] = -reg[rB(i)];
  inext();
})

/* Bitwise ops. */

vminst(AND, {
  reg[rA(i)] = reg[rB(i)] & reg[rC(i)];
  inext();
})

vminst(ANDI, {
  reg[rA(i)] = reg[rB(i)] & im(i);
  inext();
})

vminst(IOR, {
  reg[rA(i)] = reg[rB(i)] | reg[rC(i)];
  inext();
})

vminst(IORI, {
  reg[rA(i)] = reg[rB(i)] | im(i);
  inext();
})

vminst(XOR, {
  reg[rA(i)] = reg[rB(i)] ^ reg[rC(i)];
  inext();
})

vminst(XORI, {
  reg[rA(i)] = reg[rB(i)] ^ im(i);
  inext();
})

vminst(NOT, {
  reg[rA(i)] = ~reg[rB(i)];
  inext();
})

#define do_shift(op, a, b) (reg[rA(i)] = ((b) >= 64) ? 0 : ((a) op (b)))

vminst(SHL, {
  do_shift(<<,
      reg[rB(i)],
      reg[rC(i)]);
  inext();
})

vminst(SHLI, {
  do_shift(<<,
      reg[rB(i)],
      im(i));
  inext();
})

vminst(SHLIR, {
  do_shift(<<,
      im(i),
      reg[rB(i)]);
  inext();
})

vminst(SHR, {
  do_shift(>>,
      reg[rB(i)],
      reg[rC(i)]);
  inext();
})

vminst(SHRI, {
  do_shift(>>,
      reg[rB(i)],
      im(i));
  inext();
})

vminst(SHRIR, {
  do_shift(>>,
      im(i),
      reg[rB(i)]);
  inext();
})

#undef do_shift

vminst(ROL, {
  u64 v = reg[rB(i)];
  u64 c = mod64(reg[rC(i)]);
  reg[rA(i)] = rol64(v, c);
  inext();
})

vminst(ROLI, {
  u64 v = reg[rB(i)];
  u64 c = mod64(im(i));
  reg[rA(i)] = rol64(v, c);
  inext();
})

vminst(ROLIR, {
  u64 v = im(i);
  u64 c = mod64(reg[rB(i)]);
  reg[rA(i)] = rol64(v, c);
  inext();
})

vminst(ROR, {
  u64 v = reg[rB(i)];
  u64 c = mod64(reg[rC(i)]);
  reg[rA(i)] = ror64(v, c);
  inext();
})

vminst(RORI, {
  u64 v = reg[rB(i)];
  u64 c = mod64(im(i));
  reg[rA(i)] = ror64(v, c);
  inext();
})

vminst(RORIR, {
  u64 v = im(i);
  u64 c = mod64(reg[rB(i)]);
  reg[rA(i)] = ror64(v, c);
  inext();
})

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

vminst(BT, {
  bt_imm();
  inext();
})

vminst(BTG, {
  bt_reg();
  inext();
})

vminst(BTS, {
  bt_imm();
  reg[rA(i)] = bit_set(reg[rA(i)], bval_imm());
  inext();
})

vminst(BTSG, {
  bt_reg();
  reg[rA(i)] = bit_set(reg[rA(i)], bval_reg());
  inext();
})

vminst(BTR, {
  bt_imm();
  reg[rA(i)] = bit_clr(reg[rA(i)], bval_imm());
  inext();
})

vminst(BTRG, {
  bt_reg();
  reg[rA(i)] = bit_clr(reg[rA(i)], bval_reg());
  inext();
})

vminst(BTC, {
  bt_imm();
  reg[rA(i)] = bit_cml(reg[rA(i)], bval_imm());
  inext();
})

vminst(BTCG, {
  bt_reg();
  reg[rA(i)] = bit_cml(reg[rA(i)], bval_reg());
  inext();
})

#undef bval_imm
#undef bval_reg
#undef bt_imm
#undef bt_reg

/* Flags and conditionals. */

#define sbit(x) ((x)>>63)
#define do_cmp(x,y) do { \
    u64 r = (x) - (y);   \
    fl = 0;              \
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


vminst(CMP, {
  do_cmp(reg[rA(i)], reg[rB(i)]);
  inext();
})

vminst(CMPI, {
  do_cmp(reg[rA(i)], im(i));
  inext();
})

vminst(CMPIR, {
  do_cmp(im(i), reg[rA(i)]);
  inext();
})

#undef do_cmp
#define do_test(x,y) do { \
    u64 r = (x) & (y);    \
    fl = 0;               \
    if (sbit(r))          \
      setf(FS);           \
    if (r == 0)           \
      setf(FZ);           \
    if ((x) == (y))       \
      setf(FQ);           \
  } while (0)

vminst(TEST, {
  do_test(reg[rA(i)], reg[rB(i)]);
  inext();
})

vminst(TESTI, {
  do_test(reg[rA(i)], im(i));
  inext();
})

#undef do_test
#undef sbit

/* Set flags */

vminst(STC, {
  setf(FC);
  inext();
})

vminst(STO, {
  setf(FO);
  inext();
})

vminst(STS, {
  setf(FS);
  inext();
})

vminst(STZ, {
  setf(FZ);
  inext();
})

vminst(STE, {
  setf(FE);
  inext();
})

vminst(STG, {
  setf(FG);
  inext();
})

vminst(STL, {
  setf(FL);
  inext();
})

vminst(STA, {
  setf(FA);
  inext();
})

vminst(STB, {
  setf(FB);
  inext();
})

vminst(STQ, {
  setf(FQ);
  inext();
})

/* Clear flags */

vminst(CLC, {
  clrf(FC);
  inext();
})

vminst(CLO, {
  clrf(FO);
  inext();
})

vminst(CLS, {
  clrf(FS);
  inext();
})

vminst(CLZ, {
  clrf(FZ);
  inext();
})

vminst(CLE, {
  clrf(FE);
  inext();
})

vminst(CLG, {
  clrf(FG);
  inext();
})

vminst(CLL, {
  clrf(FL);
  inext();
})

vminst(CLA, {
  clrf(FA);
  inext();
})

vminst(CLB, {
  clrf(FB);
  inext();
})

vminst(CLQ, {
  clrf(FQ);
  inext();
})

/* Complement flags */

vminst(CMC, {
  cmlf(FC);
  inext();
})

vminst(CMO, {
  cmlf(FO);
  inext();
})

vminst(CMS, {
  cmlf(FS);
  inext();
})

vminst(CMZ, {
  cmlf(FZ);
  inext();
})

vminst(CME, {
  cmlf(FE);
  inext();
})

vminst(CMG, {
  cmlf(FG);
  inext();
})

vminst(CML, {
  cmlf(FL);
  inext();
})

vminst(CMA, {
  cmlf(FA);
  inext();
})

vminst(CMB, {
  cmlf(FB);
  inext();
})

vminst(CMQ, {
  cmlf(FQ);
  inext();
})

/* Branching and flow control. */

#define br_abs()  (pc  = im(i))
#define br_rel()  (pc += im(i))

vminst(JMP, {
  br_abs();
  inext();
})

vminst(JMPN, {
  br_rel();
  inext();
})

vminst(JE, {
  if (getf(FQ))
    br_rel();
  inext();
})

vminst(JNE, {
  if (!getf(FQ))
    br_rel();
  inext();
})

vminst(JG, {
  if (getf(FG))
    br_rel();
  inext();
})

vminst(JGE, {
  if (getf(FG) || getf(FQ))
    br_rel();
  inext();
})

vminst(JL, {
  if (getf(FL))
    br_rel();
  inext();
})

vminst(JLE, {
  if (getf(FL) || getf(FQ))
    br_rel();
  inext();
})

vminst(JA, {
  if (getf(FA))
    br_rel();
  inext();
})

vminst(JAE, {
  if (getf(FA) || getf(FQ))
    br_rel();
  inext();
})

vminst(JB, {
  if (getf(FB))
    br_rel();
  inext();
})

vminst(JBE, {
  if (getf(FB) || getf(FQ))
    br_rel();
  inext();
})

vminst(JC, {
  if (getf(FC))
    br_rel();
  inext();
})

vminst(JNC, {
  if (!getf(FC))
    br_rel();
  inext();
})

vminst(JO, {
  if (getf(FO))
    br_rel();
  inext();
})

vminst(JNO, {
  if (!getf(FO))
    br_rel();
  inext();
})

vminst(JS, {
  if (getf(FS))
    br_rel();
  inext();
})

vminst(JNS, {
  if (!getf(FS))
    br_rel();
  inext();
})

vminst(JZ, {
  if (getf(FZ))
    br_rel();
  inext();
})

vminst(JNZ, {
  if (!getf(FZ))
    br_rel();
  inext();
})

vminst(JX, {
  if (getf(FE))
    br_rel();
  inext();
})

vminst(JNX, {
  if (!getf(FE))
    br_rel();
  inext();
})

vminst(LOOP, {
  if (--reg[rA(i)] != 0)
    br_rel();
  inext();
})

#undef br_abs
#undef br_rel

#define setup_call(n) do { \
     push(pc);     \
     push(bp);     \
     pc += (n);    \
     bp = sp;      \
   } while (0)

vminst(CALL, {
  setup_call(im(i));
  inext();
})

vminst(CALLR, {
  setup_call(reg[rA(i)]);
  inext();
})

#undef setup_call

vminst(RET, {
  subroutine_ret:
  sp = bp;
  pop(bp);
  pop(pc);
  inext();
})

vminst(THR, {
  setf(FE);
  goto subroutine_ret;
})

vminst(SAVE, {
  for (int i = 0; i < 16; i++)
    push(reg[i]);
  inext();
})

vminst(RSTR, {
  if (sp < bp || sp - bp < 16)
    stop(E_STUND);
  for (int i = 16 - 1; i >= 0; i--)
    pop(reg[i]);
  inext();
})

vminst(JR, {
  pc = reg[rA(i)];
  inext();
})

vminst(JRN, {
  pc += reg[rA(i)];
  inext();
})

vminst(SAL, {
  if (stack_len <= sp || stack_len - sp < im(i))
    stop(E_STOVF);
  sp += im(i);
  inext();
})

vminst(SALR, {
  if (stack_len <= sp || stack_len - sp < reg[rA(i)])
    stop(E_STOVF);
  sp += reg[rA(i)];
  inext();
})

vminst(SDL, {
  if (sp < bp || sp - bp < im(i))
    stop(E_STUND);
  sp -= im(i);
  inext();
})

vminst(SDLR, {
  if (sp < bp || sp - bp < reg[rA(i)])
    stop(E_STUND);
  sp -= reg[rA(i)];
  inext();
})

vminst(CFL, {
  fl = 0;
  inext();
})
