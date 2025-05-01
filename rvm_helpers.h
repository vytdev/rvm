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

#ifndef RVM_HELPERS_H_
#define RVM_HELPERS_H_  1

/* Config values. */
#define RVM_TRUE   (1)
#define RVM_FALSE  (0)
#define RVM_AUTO  (-1) /* Auto-detection. */

#define RVM_BORD_LIL 0
#define RVM_BORD_BIG 1

/*
 * If you have an unforgiving CPU that doesn't allow unaligned access, set
 * this to RVM_TRUE. Allowed vals: RVM_TRUE and RVM_FALSE.
 */
#define RVM_CFG_FORCE_ALIGNED_ACCESS    RVM_FALSE

/*
 * Specify your system's endianess when auto detection doesn't work. Can be
 * RVM_BORD_LIL, RVM_BORD_BIG, or RVM_AUTO.
 */
#define RVM_CFG_SPECIFY_ENDIANESS       RVM_AUTO

/*
 * Specify whether your system's wordsize is 64-bit. Can be RVM_TRUE,
 * RVM_FALSE, or RVM_AUTO.
 */
#define RVM_CFG_SPECIFY_64_BIT_SYS      RVM_AUTO


/* Detect whether we're on a 64-bit system. */
#if RVM_CFG_SPECIFY_64_BIT_SYS == RVM_TRUE || \
    (RVM_CFG_SPECIFY_64_BIT_SYS == RVM_AUTO && \
       (defined(__LP64__) || defined(_WIN64)))
#define RVM_64
#endif

/* Determine the endianess of the target system. */
#if RVM_CFG_SPECIFY_ENDIANESS != RVM_AUTO
#  define RVM_BORD RVM_CFG_SPECIFY_ENDIANESS
#elif defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && \
    defined(__ORDER_LITTLE_ENDIAN__)
#  if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#    define RVM_BORD RVM_BORD_BIG
#  elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#    define RVM_BORD RVM_BORD_LIL
#  else
#    error "Unknown endianess."
#  endif
#elif defined(_MSC_VER)
#  define RVM_BORD RVM_BORD_LIL /* All MSVC targets are little-endian. */
#else
#  error "Unable to detect system endianess."
#endif

/* Restrict keyword. */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#  define RVM_RESTRICT   restrict
#elif defined(__GNUC__)
#  define RVM_RESTRICT   __restrict__
#elif defined(_MSC_VER)
#  define RVM_RESTRICT   __restrict
#else
#  define RVM_RESTRICT
#endif

/* inline keyword. */
#if defined(__cplusplus) || (defined(__STDC_VERSION__) && \
    __STDC_VERSION__ >= 199901L)
#  define RVM_INLINE inline
#else
#  define RVM_INLINE
#endif

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

#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wlong-long"

/* gcc/clang may support 'long long' in C89 and C++98/03, via extensions. */
typedef signed   long long  rvm_i64;
typedef unsigned long long  rvm_u64;
typedef signed   int        rvm_i32;
typedef unsigned int        rvm_u32;
typedef signed   short      rvm_i16;
typedef unsigned short      rvm_u16;
typedef signed   char       rvm_i8;
typedef unsigned char       rvm_u8;

#pragma GCC diagnostic pop

#else
#  error "Unable to determine integer types."
#endif

/* Variable integer. */
#if defined(RVM_64)
typedef rvm_i64 rvm_sint;
typedef rvm_u64 rvm_uint;
#else
typedef rvm_i32 rvm_sint;
typedef rvm_u32 rvm_uint;
#endif

#define RVM__STR1(x)   #x
#define RVM__STR2(x)   RVM_STR1(x)

#define RVM_SIGNEDMIN(n)   (-(1<<((n)-1)))   /* - (2^(n-1)) */
#define RVM_SIGNEDMAX(n)   ((1<<((n)-1))-1)  /* 2^(n-1) - 1 */
#define RVM_UNSIGNEDMAX(n) ((1<<(n))-1)      /* 2^n - 1 */
#define RVM_SGXTD(x, b) (((x)^(1<<((b)-1)))-(1<<((b)-1)))

/*
 * For archs that require aligned memory access, or for big
 * endian systems. We need to manually encode and decode,
 * byte-wise.
 */
#if RVM_CFG_FORCE_ALIGNED_ACCESS || RVM_BORD == RVM_BORD_BIG
#define C(x) ((rvm_u64)(x))

#define RVM_DEC8(b)  (                   (b)[0])
#define RVM_DEC16(b) (RVM_DEC8((b))  |   ((b)[1]  << 8))
#define RVM_DEC32(b) (RVM_DEC16((b)) |   ((b)[2]  << 16) \
                                     |   ((b)[3]  << 24))
#define RVM_DEC64(b) (RVM_DEC32((b)) | (C((b)[4]) << 32) \
                                     | (C((b)[5]) << 40) \
                                     | (C((b)[6]) << 48) \
                                     | (C((b)[7]) << 56))

#define RVM_ENC8(x,b)  (                    (b)[0] =    (x)         & 0xff)
#define RVM_ENC16(x,b) (RVM_ENC8((x),(b)),  (b)[1] =   ((x)  >> 8)  & 0xff)
#define RVM_ENC32(x,b) (RVM_ENC16((x),(b)), (b)[2] =   ((x)  >> 16) & 0xff, \
                                            (b)[3] =   ((x)  >> 24) & 0xff)
#define RVM_ENC64(x,b) (RVM_ENC32((x),(b)), (b)[4] = (C((x)) >> 32) & 0xff, \
                                            (b)[5] = (C((x)) >> 40) & 0xff, \
                                            (b)[6] = (C((x)) >> 48) & 0xff, \
                                            (b)[7] = (C((x)) >> 56) & 0xff)

#undef C
#else

/*
 * If we're on a little-endian system that doesn't crash on
 * unaligned memory access, let's take advantage of single-fetch
 * speed. Even though unaligned access is slow, it is still
 * faster than doing it per-byte.
 */

#define RVM_DEC8(b)  (*(rvm_u8 *)(void*)&(b)[0])
#define RVM_DEC16(b) (*(rvm_u16*)(void*)&(b)[0])
#define RVM_DEC32(b) (*(rvm_u32*)(void*)&(b)[0])
#define RVM_DEC64(b) (*(rvm_u64*)(void*)&(b)[0])

#define RVM_ENC8(x,b)  ( RVM_DEC8((b)) = (x))
#define RVM_ENC16(x,b) (RVM_DEC16((b)) = (x))
#define RVM_ENC32(x,b) (RVM_DEC32((b)) = (x))
#define RVM_ENC64(x,b) (RVM_DEC64((b)) = (x))

#endif /* RVM_CFG_FORCE_ALIGNED_ACCESS || RVM_BORD == RVM_BORD_BIG */

#endif /* rvm_helpers.h */
