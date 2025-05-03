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


struct rvm rvm_new(char *mem, rvm_uint memsz)
{
  struct rvm ctx;
  int i;
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


/* Instruction that triggers a memory fault. */
#define __RVM_TRAP_EMEMV    RVM_INENC(RVM_OP_trap, RVM_EMEMV, 0, 0, 0)
#define __RVM_FETCH()       (RVM_LIKELY(codesz > pc) \
    ? (pc++, RVM_DEC32(&mem[(pc-1) << 2]))  \
    : __RVM_TRAP_EMEMV)

#define __RVM_SAVECF()     (ctx->cf = cf)
#define __RVM_SAVEPC()     (ctx->pc = pc << 2)
#if RVM_CFG_COUNT_INSTRS_EXEC
#  define __RVM_SAVEICNT()   (ctx->inst_cnt = icnt)
#else
#  define __RVM_SAVEICNT()   (0)
#endif

#if RVM_CFG_COUNT_INSTRS_EXEC
#  define __RVM_JMPNEXT   icnt++; goto
#else
#  define __RVM_JMPNEXT   goto
#endif

/* Convinience shortcuts. */
#define rgA reg[RVM_RGA(inst)]
#define rgB reg[RVM_RGB(inst)]
#define rgC reg[RVM_RGC(inst)]
#define fnc     RVM_FNC(inst)

/* Execution helper macros. */
#define vmsave() (    \
    __RVM_SAVECF(),   \
    __RVM_SAVEPC(),   \
    __RVM_SAVEICNT())
#define vmbrk      vmsave(); return

/* Do byte-swap on big endian systems. */
#define vmfetch()  (inst = __RVM_FETCH())


signed rvm_exec(struct rvm *RVM_RESTRICT ctx)
{
  int                  const opts    = ctx->exec_opts;
  char               * const mem     = ctx->mem;
  rvm_uint             const memsz   = ctx->memsz;
  rvm_reg_t          * const reg     = ctx->reg;
  rvm_uint             const codesz  = memsz >> 2;
  int                        cf      = ctx->cf;
  rvm_reg_t                  pc      = ctx->pc >> 2;
  rvm_inst_t                 inst;
# if RVM_CFG_COUNT_INSTRS_EXEC
  rvm_u64                    icnt    = ctx->inst_cnt;
# endif


/* Use computed gotos if available. */
#if RVM_CFG_PREFER_COMP_GOTOS && (defined(__GNUC__) && \
    !defined(__STRICT_ANSI__))
# define vmnext __RVM_JMPNEXT *_disptab[RVM_OPC(vmfetch())]

  static void * RVM_RESTRICT _disptab[RVM_OPNUM];
  for (int i = 0; i < RVM_OPNUM; i++)
    _disptab[i] = &&_notimpl;

  # define DEF(op, idx) _disptab[(idx)] = (&&_H_##op);
  # include "opcodes.h"
  # undef DEF

  vmnext;

  /* Implementation. */
  #define DEF(op) _H_##op :
  #include "impl.h"
  #undef DEF

  return -RVM_ERR;
  _notimpl:
  return -RVM_EUINST;


/* Fallback to switch dispatch. */
#else
# define vmnext __RVM_JMPNEXT _loop

  _loop:
  switch (RVM_OPC(vmfetch())) {
  # define DEF(op) case (RVM_OP_##op):
  # include "impl.h"
  # undef DEF
  default: return -RVM_EUINST;
  }
  return -RVM_ERR;

#endif
}
