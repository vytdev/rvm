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

#include "config.h"
#include "defs.h"
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
  default:            return "<reserved>";
  }
}

const char *rvm_stropc(int opc)
{
  switch (opc) {
# define DEF(op, idx) case (RVM_OP##op): return (#op);
# include "opcodes.h"
# undef DEF
  default: return "<unknown>";
  }
}

signed rvm_exec(struct rvm *RVM_RESTRICT ctx)
{
  rvm_reg_t    pc;
  rvm_inst_t  *code;
  rvm_uint     codesz;
  rvm_inst_t   inst;

  pc = ctx->pc;
  code = (rvm_inst_t*)(void*)ctx->mem;
  codesz = ctx->memsz / RVM_INLN;

  /* TODO: implementation. */

  return RVM_EOK;
}
