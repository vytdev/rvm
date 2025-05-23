/*
 *  rvm -- A virtual machine.
 *  Copyright (C) 2024-2025  Vincent Yanzee J. Tan
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "codec.h"
#include "config.h"
#include "defs.h"
#include "ints.h"
#include "rvm.h"
#include "utils.h"


struct rvm rvm_new(void *mem, rvm_uint memsz)
{
  struct rvm ctx;
  int i;
  char *m;
  /* Patch the mem. */
  m = (char*)mem;
  memsz &= ~(RVM_INLN-1);
  memsz -= RVM_INLN;
  RVM_ENC32(RVM_TRAP_EMEMV, &m[memsz]);
  /* Setup ctx. */
  ctx.exec_opts = 0;
  ctx.mem = mem;
  ctx.memsz = memsz;
  ctx.cf = 0;
  ctx.pc = 0;
  for (i = 0; i < RVM_REGCNT; i++)
    ctx.reg[i] = 0;
# if RVM_CFG_COUNT_INSTRS_EXEC
  ctx.inst_cnt = 0;
# endif
  return ctx;
}

rvm_uint rvm_calcfmem(rvm_uint sz)
{
  /* align up + trap instruction + extra */
  return ((sz + (RVM_INLN << 1) - 1) & ~(RVM_INLN - 1)) + 32;
}

const char *rvm_strstat(signed e)
{
  if (e < 0)
    e = -e;
  if (e >= RVM_EHOST)
    return "<host>";
  /* RVM defined exception codes. */
  switch (e) {
  case RVM_EOK:       return "Ok";
  case RVM_ERR:       return "Error";
  case RVM_EUINST:    return "Illegal instruction";
  case RVM_EMEMV:     return "Memory fault";
  case RVM_EDIVZ:     return "Division by zero";
  default:            return "<reserved>";
  }
}

const char *rvm_stropc(int opc)
{
  switch (opc) {
# define DEF(op, idx) case (RVM_OP_##op): return (#op);
# include "opcodes.h"
# undef DEF
  default: return "<unknown>";
  }
}



/* Convinience shortcuts. */
#define rgA reg[RVM_RGA(inst)]
#define rgB reg[RVM_RGB(inst)]
#define rgC reg[RVM_RGC(inst)]
#define fnc     RVM_FNC(inst)

#define imm11s  RVM_SGXTD(fnc & RVM_F11MASK, 11)
#define imm11u  RVM_ZRXTD(fnc & RVM_F11MASK, 11)

#define imm15s  RVM_SGXTD(fnc & RVM_F15MASK, 15)
#define imm15u  RVM_ZRXTD(fnc & RVM_F15MASK, 15)

#define imm19s  RVM_SGXTD(fnc & RVM_F19MASK, 19)
#define imm19u  RVM_ZRXTD(fnc & RVM_F19MASK, 19)

#define imm23s  RVM_SGXTD(fnc & RVM_F23MASK, 23)
#define imm23u  RVM_ZRXTD(fnc & RVM_F23MASK, 23)

#define setf(v)  (cf |= (v))
#define hasf(v)  ((cf & (v)) == (v))
#define clrf()   (cf = 0)

/* Saving vm state. */
#if RVM_CFG_COUNT_INSTRS_EXEC
#  define __RVM_SAVEICNT()   (ctx->inst_cnt = icnt)
#else
#  define __RVM_SAVEICNT()   (0)
#endif
#define vmsave() (     \
    ctx->cf = cf,      \
    ctx->pc = pc << 2, \
    __RVM_SAVEICNT())

#define vmfetch()  (inst = RVM_DEC32(&mem[(pc++) << 2]))

#if RVM_CFG_COUNT_INSTRS_EXEC
#  define __RVM_JMPNEXT   icnt++; goto
#else
#  define __RVM_JMPNEXT   goto
#endif
#define vmnext     vmfetch(); __RVM_DISPATCH
#define vmbrk      vmsave(); return


signed rvm_exec(struct rvm *RVM_RESTRICT ctx)
{
  int                  const opts    = ctx->exec_opts;
  char               * const mem     = (char * const)ctx->mem;
  rvm_uint             const memsz   = ctx->memsz;
  rvm_reg_t          * const reg     = ctx->reg;
  rvm_uint             const codesz  = memsz >> 2;
  int                        cf      = ctx->cf;
  rvm_reg_t                  pc      = ctx->pc >> 2;
  rvm_inst_t                 inst;
# if RVM_CFG_COUNT_INSTRS_EXEC
  rvm_u64                    icnt    = ctx->inst_cnt;
# endif

  RVM_UNUSED(opts);

/* Use computed gotos if available. */
#if RVM_CFG_PREFER_COMP_GOTOS && (defined(__GNUC__) && \
    !defined(__STRICT_ANSI__))
# define __RVM_DISPATCH __RVM_JMPNEXT *_disptab[RVM_OPC(inst)]

  static void * RVM_RESTRICT _disptab[RVM_OPNUM];
  { /* enclose `i` */
    int i;
    for (i = 0; i < RVM_OPNUM; i++)
      _disptab[i] = &&_notimpl;
  }

  # define DEF(op, idx) _disptab[(idx)] = (&&_H_##op);
  # include "opcodes.h"
  # undef DEF

  vmnext;

  /* Implementation. */
  #define DEF(op) _H_##op :
  #include "impl.h"
  #undef DEF

  vmbrk -RVM_ERR;
  _notimpl:
  vmbrk -RVM_EUINST;


/* Fallback to switch dispatch. */
#else
# define __RVM_DISPATCH __RVM_JMPNEXT _loop

  vmnext;
  _loop:
  switch (RVM_OPC(inst)) {
  # define DEF(op) case (RVM_OP_##op):
  # include "impl.h"
  # undef DEF
  default: vmbrk -RVM_EUINST;
  }
  vmbrk -RVM_ERR;

#endif
}
