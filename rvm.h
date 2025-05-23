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

#ifndef RVM_H_
#define RVM_H_  1

#include "config.h"
#include "defs.h"
#include "ints.h"
#include "utils.h"

#define RVM_SHORTNAME  "RVM"
#define RVM_LONGNAME   "Redstone Virtual Machine"
#define RVM_COPYRIGHT  "Copyright (C) 2024-2025  Vincent Yanzee J. Tan"

#define RVM_ARCH       0     /* Architecture version. */
#define RVM_IMPL       2     /* Implementation patch number. */
#define RVM_VERSION    (RVM_ARCH * 1000L + RVM_IMPL)

#define RVM_VERSTR     RVM_STR2(RVM_ARCH) "." RVM_STR2(RVM_IMPL)
#define RVM_LABEL      RVM_SHORTNAME " " RVM_VERSTR

typedef rvm_u64 rvm_reg_t;   /* Type for a register. */
typedef rvm_u32 rvm_inst_t;  /* Type for an instruction. */

/*
 * rvm execution context.
 */
struct rvm {
  int         exec_opts;  /* Exec options. */
  void       *mem;     /* Ptr to contiguous mem. */
  rvm_uint    memsz;   /* How much mem we have. */
  int         cf;      /* Condition flags. */
  rvm_reg_t   pc;      /* Program counter. */
  rvm_reg_t   reg[RVM_REGCNT];    /* GPRs. */
#if RVM_CFG_COUNT_INSTRS_EXEC
  rvm_u64     inst_cnt;  /* Number of instructions. */
#endif
};


#if defined(__cplusplus)
extern "C" {
#endif

/*
 * Creates a new rvm exec context with a contiguous `mem` of size `memsz`.
 */
struct rvm rvm_new(void *mem, rvm_uint memsz);

/*
 * Calculate the final mem size needed for allocation.
 */
rvm_uint rvm_calcfmem(rvm_uint sz);

/*
 * Returns a C-string explaining the given stat code `e`.
 */
const char *rvm_strstat(signed e);

/*
 * Returns a mnemonic C-string for opcode `opc`.
 */
const char *rvm_stropc(int opc);

/*
 * Execute rvm ctx until an exception occurs.
 */
signed rvm_exec(struct rvm *RVM_RESTRICT ctx);

#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif /* rvm.h */
