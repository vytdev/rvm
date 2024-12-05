#ifndef RVM_CODEC_H_
#define RVM_CODEC_H_
#include "config.h"

/* read 8 (1 byte) */
#define read8(s)  ((i8)((s)[0]))

/* read 16 (2 bytes) */
#define read16(s) ((i16)( \
    read8((s)) |          \
    ((s)[1] << 8)))

/* read 32 (4 bytes) */
#define read32(s) ((i32)( \
    read16((s))    |      \
    ((s)[2] << 16) |      \
    ((s)[3] << 24)))

/* read 64 (8 bytes) */
#define read64(s) ((i64)(  \
    read32((s))         |  \
    ((i64)(s)[4] << 32) |  \
    ((i64)(s)[5] << 40) |  \
    ((i64)(s)[6] << 48) |  \
    ((i64)(s)[7] << 56)))

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
    (s)[4] = ((v) >> 32) & 0xff; \
    (s)[5] = ((v) >> 40) & 0xff; \
    (s)[6] = ((v) >> 48) & 0xff; \
    (s)[7] = ((v) >> 56) & 0xff; \
  } while (0)

#endif // RVM_CODEC_H_
