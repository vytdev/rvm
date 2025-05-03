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

#ifndef RVM_CONFIG_H_
#define RVM_CONFIG_H_  1

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

/*
 * Specify whether to prefer using a threaded dispatch if the compiler
 * supports. Allowed vals: RVM_TRUE, RVM_FALSE.
 */
#define RVM_CFG_PREFER_COMP_GOTOS       RVM_TRUE

/*
 * Enable counting of the instructions executed. Allowed vals: RVM_TRUE,
 * RVM_FALSE.
 */
#define RVM_CFG_COUNT_INSTRS_EXEC       RVM_TRUE


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
#elif defined(__GNUC__) && !defined(__STRICT_ANSI__)
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

#endif /* config.h */
