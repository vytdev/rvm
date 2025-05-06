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

#if defined(DEF)

DEF(nop,   0)   /* [*] No-op */
DEF(mov,   1)   /* [M] Move across regs */
DEF(trap,  2)   /* [J] Trigger an exception */
DEF(li,    3)   /* [M] Load fn19 to rgA, sign extended */
DEF(j,     4)   /* [J] Uncond. pc-rel jump */
DEF(cmp,   5)   /* [I] Compares two regs. */
DEF(cmpi,  6)   /* [M] Compares a reg and a 19-bit imm. */
DEF(add,   7)   /* [R] rgA = rgB + rgC */
DEF(addi,  8)   /* [I] rgA = rgB + zxt(func15) */
DEF(sub,   9)   /* [R] rgA = rgB - rgC */
DEF(subi, 10)   /* [I] rgA = rgB - zxt(func15) */

#endif /* opcodes.h */
