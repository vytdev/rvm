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

#ifndef RVM_UTILS_H_
#define RVM_UTILS_H_  1

#include "config.h"

#define RVM_STR1(x)   #x
#define RVM_STR2(x)   RVM_STR1(x)

#define RVM_SIGNEDMIN(n)   (-(1<<((n)-1)))   /* - (2^(n-1)) */
#define RVM_SIGNEDMAX(n)   ((1<<((n)-1))-1)  /* 2^(n-1) - 1 */
#define RVM_UNSIGNEDMAX(n) ((1<<(n))-1)      /* 2^n - 1 */

#define RVM_SGXTD(x, b) (((x)^(1<<((b)-1)))-(1<<((b)-1)))

#define RVM_BSWAP32(v) \
  (((((v) & 0xff000000) >> 24) & 0x000000ff) | \
   ((((v) & 0x00ff0000) >> 8 ) & 0x0000ff00) | \
   ((((v) & 0x0000ff00) << 8 ) & 0x00ff0000) | \
   ((((v) & 0x000000ff) << 24) & 0xff000000))

/* Only bswap on big endian sys */
#if RVM_BORD == RVM_BORD_BIG
#define RVM_BSWP32BE(v)  RVM_BSWAP32((v))
#else
#define RVM_BSWP32BE(v)  (v)
#endif

#if defined(__GNUC__)
#define RVM_LIKELY(x)    (__builtin_expect(((x) != 0), 1))
#define RVM_UNLIKELY(x)  (__builtin_expect(((x) != 0), 0))
#else
#define RVM_LIKELY(x)    (x)
#define RVM_UNLIKELY(x)  (x)
#endif

#endif /* utils.h */
