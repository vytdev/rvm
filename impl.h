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
  rgA = RVM_SGXTD(fnc & RVM_F19MASK, 19);
  vmnext;
}

DEF(j) {
  pc += RVM_SGXTD(fnc, 23);
  util_checkpc();
  vmnext;
}

DEF(cmp) {
  cf = util_icmp(rgA, rgB);
  vmnext;
}

DEF(cmpi) {
  rvm_reg_t imm = RVM_SGXTD(fnc & RVM_F19MASK, 19);
  cf = util_icmp(rgA, imm);
  vmnext;
}

DEF(add) {
  rgA = rgB + rgC;
  vmnext;
}

DEF(addi) {
  rgA = rgB + RVM_ZRXTD(fnc & RVM_F15MASK, 15);
  vmnext;
}

DEF(sub) {
  rgA = rgB - rgC;
  vmnext;
}

DEF(subi) {
  rgA = rgB - RVM_ZRXTD(fnc & RVM_F15MASK, 15);
  vmnext;
}

#endif /* impl.h */
