/* Floating point unit */

#ifndef vminst
#  define vminst(n, c...)
#endif


/* Load and store instructions. */

vminst(FLDI, {
  fp_reg[rA(i)] = (int64_t)reg[rB(i)];
  inext();
})

vminst(FLDU, {
  fp_reg[rA(i)] = (uint64_t)reg[rB(i)];
  inext();
})

vminst(FLDS, {
  fp_reg[rA(i)] = reinterp_cast(float,uint64_t,reg[rB(i)]);
  inext();
})

vminst(FLDD, {
  fp_reg[rA(i)] = reinterp_cast(double,uint64_t,reg[rB(i)]);
  inext();
})

vminst(FSTI, {
  reg[rA(i)] = (int64_t)fp_reg[rB(i)];
  inext();
})

vminst(FSTU, {
  reg[rA(i)] = (uint64_t)fp_reg[rB(i)];
  inext();
})

vminst(FSTS, {
  reg[rA(i)] = reinterp_cast(uint64_t,float,fp_reg[rB(i)]);
  inext();
})

vminst(FSTD, {
  reg[rA(i)] = reinterp_cast(uint64_t,double,fp_reg[rB(i)]);
  inext();
})

/* Constants */

vminst(FLCINF, {
  fp_reg[rA(i)] = INFINITY;
  inext();
})

vminst(FLCNGF, {
  fp_reg[rA(i)] = -INFINITY;
  inext();
})

vminst(FLCNAN, {
  fp_reg[rA(i)] = NAN;
  inext();
})

vminst(FLCNZR, {
  fp_reg[rA(i)] = 0.0;
  inext();
})

vminst(FLCNOE, {
  fp_reg[rA(i)] = 1.0;
  inext();
})

vminst(FLCPI, {
  fp_reg[rA(i)] = M_PI;
  inext();
})

vminst(FLCE, {
  fp_reg[rA(i)] = M_E;
  inext();
})

vminst(FLCLG2E, {
  fp_reg[rA(i)] = M_LOG2E;
  inext();
})

vminst(FLCLG10E, {
  fp_reg[rA(i)] = M_LOG10E;
  inext();
})

vminst(FLCLN2, {
  fp_reg[rA(i)] = M_LN2;
  inext();
})

vminst(FLCLN10, {
  fp_reg[rA(i)] = M_LN10;
  inext();
})

vminst(FLCPI2, {
  fp_reg[rA(i)] = M_PI_2;
  inext();
})

vminst(FLCPI4, {
  fp_reg[rA(i)] = M_PI_4;
  inext();
})

vminst(FLCSQ2, {
  fp_reg[rA(i)] = M_SQRT2;
  inext();
})

vminst(FLCRSQ2, {
  fp_reg[rA(i)] = M_SQRT1_2;
  inext();
})

/* Arithmetic */

vminst(FADD, {
  fp_reg[rA(i)] = fp_reg[rB(i)] + fp_reg[rC(i)];
  inext();
})

vminst(FSUB, {
  fp_reg[rA(i)] = fp_reg[rB(i)] - fp_reg[rC(i)];
  inext();
})

vminst(FMUL, {
  fp_reg[rA(i)] = fp_reg[rB(i)] * fp_reg[rC(i)];
  inext();
})

vminst(FDIV, {
  fp_reg[rA(i)] = fp_reg[rB(i)] / fp_reg[rC(i)];
  inext();
})

vminst(FMOD, {
  fp_reg[rA(i)] = fmod(fp_reg[rB(i)], fp_reg[rC(i)]);
  inext();
})

vminst(FNEG, {
  fp_reg[rA(i)] = -fp_reg[rB(i)];
  inext();
})

vminst(FABS, {
  fp_reg[rA(i)] = fabs(fp_reg[rB(i)]);
  inext();
})

vminst(FREC, {
  fp_reg[rA(i)] = 1.0 / fp_reg[rB(i)];
  inext();
})

/* Elementary functions */

vminst(FSIN, {
  fp_reg[rA(i)] = sin(fp_reg[rB(i)]);
  inext();
})

vminst(FCOS, {
  fp_reg[rA(i)] = cos(fp_reg[rB(i)]);
  inext();
})

vminst(FTAN, {
  fp_reg[rA(i)] = tan(fp_reg[rB(i)]);
  inext();
})

vminst(FASIN, {
  fp_reg[rA(i)] = asin(fp_reg[rB(i)]);
  inext();
})

vminst(FACOS, {
  fp_reg[rA(i)] = acos(fp_reg[rB(i)]);
  inext();
})

vminst(FATAN, {
  fp_reg[rA(i)] = atan(fp_reg[rB(i)]);
  inext();
})

vminst(FATAN2, {
  fp_reg[rA(i)] = atan2(fp_reg[rB(i)], fp_reg[rC(i)]);
  inext();
})

vminst(FSINH, {
  fp_reg[rA(i)] = sinh(fp_reg[rB(i)]);
  inext();
})

vminst(FCOSH, {
  fp_reg[rA(i)] = cosh(fp_reg[rB(i)]);
  inext();
})

vminst(FTANH, {
  fp_reg[rA(i)] = tanh(fp_reg[rB(i)]);
  inext();
})

vminst(FASINH, {
  fp_reg[rA(i)] = asinh(fp_reg[rB(i)]);
  inext();
})

vminst(FACOSH, {
  fp_reg[rA(i)] = acosh(fp_reg[rB(i)]);
  inext();
})

vminst(FATANH, {
  fp_reg[rA(i)] = atanh(fp_reg[rB(i)]);
  inext();
})

/* Exponents and logarithms */

vminst(FEXP, {
  fp_reg[rA(i)] = exp(fp_reg[rB(i)]);
  inext();
})

vminst(FEXP2, {
  fp_reg[rA(i)] = exp2(fp_reg[rB(i)]);
  inext();
})

vminst(FLN, {
  fp_reg[rA(i)] = log(fp_reg[rB(i)]);
  inext();
})

vminst(FLG2, {
  fp_reg[rA(i)] = log2(fp_reg[rB(i)]);
  inext();
})

vminst(FLG10, {
  fp_reg[rA(i)] = log10(fp_reg[rB(i)]);
  inext();
})

vminst(FPOW, {
  fp_reg[rA(i)] = pow(fp_reg[rB(i)], fp_reg[rC(i)]);
  inext();
})

vminst(FSQRT, {
  fp_reg[rA(i)] = sqrt(fp_reg[rB(i)]);
  inext();
})

vminst(FCBRT, {
  fp_reg[rA(i)] = cbrt(fp_reg[rB(i)]);
  inext();
})

/* Other functions */

vminst(FCEIL, {
  fp_reg[rA(i)] = ceil(fp_reg[rB(i)]);
  inext();
})

vminst(FFLR, {
  fp_reg[rA(i)] = floor(fp_reg[rB(i)]);
  inext();
})

vminst(FRND, {
  fp_reg[rA(i)] = round(fp_reg[rB(i)]);
  inext();
})

vminst(FTRUNC, {
  fp_reg[rA(i)] = trunc(fp_reg[rB(i)]);
  inext();
})

vminst(FSGN, {
  fp_reg[rA(i)] = fp_reg[rB(i)] < 0.0 ? -1.0 : 1.0;
  inext();
})

vminst(FCPSG, {
  fp_reg[rA(i)] = copysign(fp_reg[rB(i)], fp_reg[rC(i)]);
  inext();
})

/* Error functions */

vminst(FGMA, {
  fp_reg[rA(i)] = gamma(fp_reg[rB(i)]);
  inext();
})

vminst(FLGMA, {
  fp_reg[rA(i)] = lgamma(fp_reg[rB(i)]);
  inext();
})

vminst(FTGMA, {
  fp_reg[rA(i)] = tgamma(fp_reg[rB(i)]);
  inext();
})

vminst(FERF, {
  fp_reg[rA(i)] = erf(fp_reg[rB(i)]);
  inext();
})

vminst(FERFC, {
  fp_reg[rA(i)] = erfc(fp_reg[rB(i)]);
  inext();
})

/* Casts and Misc */

vminst(FNEXT, {
  fp_reg[rA(i)] = nextafter(fp_reg[rB(i)], fp_reg[rC(i)]);
  inext();
})

vminst(FCI2F, {
  reg[rA(i)] = reinterp_cast(uint64_t,float,(int64_t)reg[rB(i)]);
  inext();
})

vminst(FCI2D, {
  reg[rA(i)] = reinterp_cast(uint64_t,double,(int64_t)reg[rB(i)]);
  inext();
})

vminst(FCU2F, {
  reg[rA(i)] = reinterp_cast(uint64_t,float,(uint64_t)reg[rB(i)]);
  inext();
})

vminst(FCU2D, {
  reg[rA(i)] = reinterp_cast(uint64_t,double,(uint64_t)reg[rB(i)]);
  inext();
})

vminst(FCF2I, {
  reg[rA(i)] = (int64_t)reinterp_cast(float,uint64_t,reg[rB(i)]);
  inext();
})

vminst(FCF2U, {
  reg[rA(i)] = (uint64_t)reinterp_cast(float,uint64_t,reg[rB(i)]);
  inext();
})

vminst(FCF2D, {
  reg[rA(i)] = reinterp_cast(uint64_t,double,
                 reinterp_cast(float,uint64_t,reg[rB(i)]));
  inext();
})

vminst(FCD2I, {
  reg[rA(i)] = (int64_t)reinterp_cast(double,uint64_t,reg[rB(i)]);
  inext();
})

vminst(FCD2U, {
  reg[rA(i)] = (uint64_t)reinterp_cast(double,uint64_t,reg[rB(i)]);
  inext();
})

vminst(FCD2F, {
  reg[rA(i)] = reinterp_cast(uint64_t,float,
                 reinterp_cast(double,uint64_t,reg[rB(i)]));
  inext();
})

vminst(FCMP, {
  double res = fp_reg[rA(i)] - fp_reg[rB(i)];
  fl = 0;
  if (res == 0) {
    setf(FZ);
  }
  else if (res < 0.0) {
    setf(FS);
    setf(FL);
    setf(FB);
  }
  else {
    setf(FG);
    setf(FA);
  }
  inext();
})

vminst(FISNAN, {
  if (isnan(fp_reg[rA(i)]))
    setf(FZ);
  else
    clrf(FZ);
  inext();
})

vminst(FISINF, {
  if (isinf(fp_reg[rA(i)]))
    setf(FZ);
  else
    clrf(FZ);
  inext();
})
