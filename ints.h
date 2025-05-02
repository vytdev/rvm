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

#ifndef RVM_INTS_H_
#define RVM_INTS_H_  1

#include "config.h"


/* Integer types. */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#include <stdint.h>
typedef int64_t             rvm_i64;
typedef uint64_t            rvm_u64;
typedef int32_t             rvm_i32;
typedef uint32_t            rvm_u32;
typedef int16_t             rvm_i16;
typedef uint16_t            rvm_u16;
typedef int8_t              rvm_i8;
typedef uint8_t             rvm_u8;

#elif defined(__cplusplus) && __cplusplus >= 201103L
/* C++98/03 doesn't have cstdint. */
#include <cstdint>
typedef std::int64_t        rvm_i64;
typedef std::uint64_t       rvm_u64;
typedef std::int32_t        rvm_i32;
typedef std::uint32_t       rvm_u32;
typedef std::int16_t        rvm_i16;
typedef std::uint16_t       rvm_u16;
typedef std::int8_t         rvm_i8;
typedef std::uint8_t        rvm_u8;

#elif defined(_MSC_VER)
typedef          __int64    rvm_i64;
typedef unsigned __int64    rvm_u64;
typedef          __int32    rvm_i32;
typedef unsigned __int32    rvm_u32;
typedef          __int16    rvm_i16;
typedef unsigned __int16    rvm_u16;
typedef          __int8     rvm_i8;
typedef unsigned __int8     rvm_u8;

#else
/* Manually detect int types. */
#include <limits.h>

/* We couldn't just use (ULONG_MAX >> 63). That will not work in 32-bit or
   LLP archs. Assuming ULONG_MAX == (2^64)-1 ; 128-bit systems doesn't
   exist yet! */
#define RVM_ISLONG64   ((ULONG_MAX >> 31) > 1)

#define RVM_ISLONG32   ((ULONG_MAX >> 31) == 1)
#define RVM_ISINT32    ((UINT_MAX  >> 31) == 1)
#define RVM_ISSHRT16   ((USHRT_MAX >> 15) == 1)
#define RVM_ISCHAR8    (CHAR_BIT == 8)

#if RVM_ISLONG64
typedef signed   long       rvm_i64;
typedef unsigned long       rvm_u64;
#elif (defined(__GNUC__) && !defined(__STRICT_ANSI__)) || defined(__clang__)
/* ISO C99 states that 'long long' must be *at least* 64-bits. gcc/clang may
   allow us to use 'long long' in C89/C++98/03 via extensions. */
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wlong-long"
typedef signed   long long  rvm_i64;
typedef unsigned long long  rvm_u64;
#  pragma GCC diagnostic pop
#else
#  error "Unable to find a 64-bit int type."
#endif

#if RVM_ISINT32
typedef signed   int        rvm_i32;
typedef unsigned int        rvm_u32;
#elif RVM_ISLONG32
/* ANSI C says that 'long' must be *at least* 32-bits. Case for LLP archs. */
typedef signed   long       rvm_i32;
typedef unsigned long       rvm_u32;
#else
#  error "Unable to find a 32-bit int type."
#endif

typedef signed   short      rvm_i16;
typedef unsigned short      rvm_u16;
#if !RVM_ISSHRT16
#  error "Unable to find a 16-bit int type."
#endif

/* Most of the time, char is 8-bit. */
typedef signed   char       rvm_i8;
typedef unsigned char       rvm_u8;
#if !RVM_ISCHAR8
#  error "Unable to find an 8-bit int type."
#endif

#endif

/* Variable integer. */
#if defined(RVM_64)
typedef rvm_i64 rvm_sint;
typedef rvm_u64 rvm_uint;
#else
typedef rvm_i32 rvm_sint;
typedef rvm_u32 rvm_uint;
#endif

#endif /* ints.h */
