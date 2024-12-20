#ifndef RVM_CODEC_H_
#define RVM_CODEC_H_
#include "config.h"


/* This header contains deserialisers and
   serialisers for primitives. */


/* Big-endian compatibility. */
#if defined(BO_BE)

/* read 8 (1 byte) */
#define read8(s)  ((u8)((s)[0]))

/* read 16 (2 bytes) */
#define read16(s) ((u16)( \
    read8((s)) |          \
    ((s)[1] << 8)))

/* read 32 (4 bytes) */
#define read32(s) ((u32)( \
    read16((s))    |      \
    ((s)[2] << 16) |      \
    ((s)[3] << 24)))

/* read 64 (8 bytes) */
#define read64(s) ((u64)(  \
    read32((s))         |  \
    ((u64)(s)[4] << 32) |  \
    ((u64)(s)[5] << 40) |  \
    ((u64)(s)[6] << 48) |  \
    ((u64)(s)[7] << 56)))

/* write 8 (1 byte) */
#define write8(s,v) do {         \
    (s)[0] = (v) & 0xff;         \
  } while (0)

/* write 16 (2 bytes) */
#define write16(s,v) do {        \
    write8((s),(v));             \
    (s)[1] = ((v) >> 8) & 0xff;  \
  } while (0)

/* write 32 (4 bytes) */
#define write32(s,v) do {        \
    write16((s),(v));            \
    (s)[2] = ((v) >> 16) & 0xff; \
    (s)[3] = ((v) >> 24) & 0xff; \
  } while (0)

/* write 64 (8 bytes) */
#define write64(s,v) do {        \
    write32((s),(v));            \
    (s)[4] = ((u64)(v) >> 32) & 0xff; \
    (s)[5] = ((u64)(v) >> 40) & 0xff; \
    (s)[6] = ((u64)(v) >> 48) & 0xff; \
    (s)[7] = ((u64)(v) >> 56) & 0xff; \
  } while (0)

#else /* if defined(BO_LE) */

/* Assuming that unaligned access will not cause
   major faults on the little-endian processors
   running this program. */

#define read8(s)  ((s)[0])
#define read16(s) (*(u16*)(void*)&(s)[0])
#define read32(s) (*(u32*)(void*)&(s)[0])
#define read64(s) (*(u64*)(void*)&(s)[0])

#define write8(s,v)  ((s)[0] = (v))
#define write16(s,v) (read16(s) = (v))
#define write32(s,v) (read32(s) = (v))
#define write64(s,v) (read64(s) = (v))

#endif

#endif // RVM_CODEC_H_
