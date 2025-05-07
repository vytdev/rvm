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
    if (pc >= codesz) {       \
      inst = RVM_TRAP_EMEMV;  \
      __RVM_DISPATCH;         \
    } \
  } while (0)

#define gen_intop(n, op) \
  DEF(n) { \
    rgA = rgB op rgC; \
    vmnext; \
  } \
  DEF(n ## i) { \
    rgA = rgB op imm15u; \
    vmnext; \
  }


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

gen_intop(add, +);
gen_intop(sub, -);
gen_intop(mul, *);
gen_intop(and, &);
gen_intop(orr, |);
gen_intop(xor, ^);

DEF(div) {
  if (rgC == 0) {
    vmbrk -RVM_EDIVZ;
  }
  rgA = rgB / rgC;
  vmnext;
}

DEF(divi) {
  if (imm15u == 0) {
    vmbrk -RVM_EDIVZ;
  }
  rgA = rgB / imm15u;
  vmnext;
}

DEF(mod) {
  if (rgC == 0) {
    vmbrk -RVM_EDIVZ;
  }
  rgA = rgB % rgC;
  vmnext;
}

DEF(modi) {
  if (imm15u == 0) {
    vmbrk -RVM_EDIVZ;
  }
  rgA = rgB % imm15u;
  vmnext;
}

#endif /* impl.h */
