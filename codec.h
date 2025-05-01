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

#ifndef RVM_CODEC_H_
#define RVM_CODEC_H_  1

#include "config.h"
#include "ints.h"

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

#endif /* codec.h */
