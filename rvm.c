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

rvm_stat_t rvm_exec(struct rvm *RVM_RESTRICT ctx)
{
  rvm_reg_t    pc;
  rvm_inst_t  *code;
  rvm_uint     codesz;
  rvm_inst_t   inst;

  pc = ctx->pc;
  code = (rvm_inst_t*)(void*)ctx->mem;
  codesz = ctx->memsz / RVM_INLN;

#define VMFETCH() (inst = (codesz > pc ? code[pc++] : 0))

  /* TODO: use a goto dispatch when available. */
  while (1) {
    switch (VMFETCH()) {
    case 0x0: return -(RVM_FNC(inst) & 0xff);
    default:  return RVM_EOK;
    }
  }

  return RVM_EOK;
}
