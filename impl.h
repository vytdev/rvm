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

#define util_icmp(a, b) ( \
  (((a) == (b))                  << RVM_FEQBP) | \
  (((rvm_u64)(a) > (rvm_u64)(b)) << RVM_FABBP) | \
  (((rvm_i64)(a) > (rvm_i64)(b)) << RVM_FGTBP) );
#define util_checkpc() do {   \
    if (RVM_UNLIKELY(pc >= codesz)) { \
      inst = RVM_TRAP_EMEMV;  \
      __RVM_DISPATCH;         \
    } \
  } while (0)

DEF(nop) {
  vmnext;
}

DEF(mov) {
  rgA = rgB;
  vmnext;
}

DEF(trap) {
  vmbrk -(fnc & 0xff);
}

DEF(li) {
  rgA = imm19s;
  vmnext;
}

DEF(j) {
  pc += imm23s;
  util_checkpc();
  vmnext;
}

DEF(cmp) {
  cf = util_icmp(rgA, rgB);
  vmnext;
}

DEF(cmpi) {
  cf = util_icmp(rgA, imm19s);
  vmnext;
}

DEF(add) {
  rgA = rgB + rgC;
  vmnext;
}

DEF(addi) {
  rgA = rgB + imm15u;
  vmnext;
}

DEF(sub) {
  rgA = rgB - rgC;
  vmnext;
}

DEF(subi) {
  rgA = rgB - imm15u;
  vmnext;
}

DEF(mul) {
  rgA = rgB * rgC;
  vmnext;
}

DEF(muli) {
  rgA = rgB * imm15u;
  vmnext;
}

DEF(and) {
  rgA = rgB & rgC;
  vmnext;
}

DEF(andi) {
  rgA = rgB & imm15u;
  vmnext;
}

DEF(orr) {
  rgA = rgB | rgC;
  vmnext;
}

DEF(orri) {
  rgA = rgB | imm15u;
  vmnext;
}

DEF(xor) {
  rgA = rgB ^ rgC;
  vmnext;
}

DEF(xori) {
  rgA = rgB ^ imm15u;
  vmnext;
}

DEF(div) {
  if (RVM_UNLIKELY(rgC == 0)) {
    vmbrk -RVM_EDIVZ;
  }
  rgA = rgB / rgC;
  vmnext;
}

DEF(divi) {
  if (RVM_UNLIKELY(imm15u == 0)) {
    vmbrk -RVM_EDIVZ;
  }
  rgA = rgB / imm15u;
  vmnext;
}

DEF(mod) {
  if (RVM_UNLIKELY(rgC == 0)) {
    vmbrk -RVM_EDIVZ;
  }
  rgA = rgB % rgC;
  vmnext;
}

DEF(modi) {
  if (RVM_UNLIKELY(imm15u == 0)) {
    vmbrk -RVM_EDIVZ;
  }
  rgA = rgB % imm15u;
  vmnext;
}

DEF(muls) {
  rgA = (rvm_i64)rgB * (rvm_i64)rgC;
  vmnext;
}

DEF(mulsi) {
  rgA = (rvm_i64)rgB * (rvm_i64)imm15s;
  vmnext;
}

DEF(divs) {
  if (RVM_UNLIKELY(rgC == 0)) {
    vmbrk -RVM_EDIVZ;
  }
  rgA = (rvm_i64)rgB / (rvm_i64)rgC;
  vmnext;
}

DEF(divsi) {
  if (RVM_UNLIKELY(imm15u == 0)) {
    vmbrk -RVM_EDIVZ;
  }
  rgA = (rvm_i64)rgB / (rvm_i64)imm15s;
  vmnext;
}

DEF(shl) {
  rgA = rgB << (rgC & 63);
  vmnext;
}

DEF(shli) {
  rgA = rgB << (fnc & 63);
  vmnext;
}

DEF(shr) {
  rgA = rgB >> (rgC & 63);
  vmnext;
}

DEF(shri) {
  rgA = rgB >> (fnc & 63);
  vmnext;
}

DEF(cpl) {
  rgA = ~rgB;
  vmnext;
}

DEF(neg) {
  rgA = -rgB;
  vmnext;
}

DEF(inc) {
  rgA++;
  vmnext;
}

DEF(dec) {
  rgA--;
  vmnext;
}

DEF(swp) {
  rvm_reg_t tmp = rgA;
  rgA = rgB;
  rgB = tmp;
  vmnext;
}

DEF(adr) {
  rgA = (pc << 2) + imm19s;
  vmnext;
}

DEF(jr) {
  pc = rgA >> 2;
  util_checkpc();
  vmnext;
}

DEF(loop) {
  if (RVM_LIKELY(rgA != 0)) {
    rgA--;
    pc += imm19s;
    util_checkpc();
  }
  vmnext;
}

DEF(je) {
  if (hasf(RVM_FEQ)) {
    pc += imm23s;
    util_checkpc();
  }
  vmnext;
}

DEF(jne) {
  if (!hasf(RVM_FEQ)) {
    pc += imm23s;
    util_checkpc();
  }
  vmnext;
}

#endif /* impl.h */
