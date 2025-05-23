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

DEF(nop,     0)   /* [*] No-op */
DEF(mov,     1)   /* [M] Move across regs */
DEF(trap,    2)   /* [J] Trigger an exception */
DEF(li,      3)   /* [M] Load fn19 to rgA, sign extended */
DEF(j,       4)   /* [J] Uncond. pc-rel jump */
DEF(cmp,     5)   /* [I] Compares two regs. */
DEF(cmpi,    6)   /* [M] Compares a reg and a 19-bit imm. */
DEF(add,     7)   /* [R] rgA = rgB + rgC */
DEF(addi,    8)   /* [I] rgA = rgB + imm15u */
DEF(sub,     9)   /* [R] rgA = rgB - rgC */
DEF(subi,   10)   /* [I] rgA = rgB - imm15u */
DEF(mul,    11)   /* [R] rgA = rgB * rgC */
DEF(muli,   12)   /* [I] rgA = rgB * imm15u */
DEF(and,    13)   /* [R] rgA = rgB & rgC */
DEF(andi,   14)   /* [I] rgA = rgB & imm15u */
DEF(orr,    15)   /* [R] rgA = rgB | rgC */
DEF(orri,   16)   /* [I] rgA = rgB | imm15u */
DEF(xor,    17)   /* [R] rgA = rgB ^ rgC */
DEF(xori,   18)   /* [I] rgA = rgB ^ imm15u */
DEF(div,    19)   /* [R] rgA = rgB / rgC */
DEF(divi,   20)   /* [I] rgA = rgB / imm15u */
DEF(mod,    21)   /* [R] rgA = rgB % rgC */
DEF(modi,   22)   /* [I] rgA = rgB % imm15u */
DEF(muls,   23)   /* [R] rgA = s(rgB) * s(rgC) */
DEF(mulsi,  24)   /* [I] rgA = s(rgB) * imm15s */
DEF(divs,   25)   /* [R] rgA = s(rgB) / s(rgC) */
DEF(divsi,  26)   /* [I] rgA = s(rgB) / imm15s */
DEF(shl,    27)   /* [R] rgA = rgB << rgC */
DEF(shli,   28)   /* [I] rgA = rgB << (fnc & 63) */
DEF(shr,    29)   /* [R] rgA = rgB >> rgC */
DEF(shri,   30)   /* [I] rgA = rgB >> (fnc & 63) */
DEF(cpl,    31)   /* [I] rgA = ~rgB */
DEF(neg,    32)   /* [I] rgA = -rgB */
DEF(inc,    33)   /* [M] rgA = rgA + 1 */
DEF(dec,    34)   /* [M] rgA = rgA - 1 */
DEF(swp,    35)   /* [I] rgA, rgB = rgB, rgA */
DEF(adr,    36)   /* [M] rgA = pc + imm19s */
DEF(jr,     37)   /* [M] pc = rgA */
DEF(loop,   38)   /* [M] if (rgA-- != 0) pc += imm19s */
DEF(je,     39)   /* [J] jump if eq */
DEF(jne,    40)   /* [J] jump if not-eq */
DEF(jg,     41)   /* [J] jump if gt */
DEF(ja,     42)   /* [J] jump if ab */
DEF(jl,     43)   /* [J] jump if not-gt and not-eq */
DEF(jb,     44)   /* [J] jump if not-ab and not-eq */
DEF(jge,    45)   /* [J] jump if gt or eq */
DEF(jae,    46)   /* [J] jump if ab or eq */
DEF(jle,    47)   /* [J] jump if not-gt */
DEF(jbe,    48)   /* [J] jump if not-ab */
DEF(rd8,    49)   /* [I] 1B: rgA = mem[rgB + imm15s] */
DEF(wr8,    50)   /* [I] 1B: mem[rgB + imm15s] = rgA */
DEF(rd16,   51)   /* [I] 2B: rgA = mem[rgB + imm15s] */
DEF(wr16,   52)   /* [I] 2B: mem[rgB + imm15s] = rgA */
DEF(rd32,   53)   /* [I] 4B: rgA = mem[rgB + imm15s] */
DEF(wr32,   54)   /* [I] 4B: mem[rgB + imm15s] = rgA */
DEF(rd64,   55)   /* [I] 8B: rgA = mem[rgB + imm15s] */
DEF(wr64,   56)   /* [I] 8B: mem[rgB + imm15s] = rgA */
DEF(call,   57)   /* [J] call a subroutine */
DEF(callr,  58)   /* [M] call a subroutine from reg addr */
DEF(ret,    59)   /* [J] return to caller */

#endif /* opcodes.h */
