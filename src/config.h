#ifndef RVM_CONFIG_H_
#define RVM_CONFIG_H_
#include <stdint.h>
#include <stddef.h>

/* Check if the system is 64-bit. */
#if defined(__LP64__) || defined(_WIN64)
#  define RVM64
#endif

/* Determine the OS. */
#if defined(_WIN32) || defined(_WIN64)
#  define OS_WIN
#else  /* Not Windows, let's assume this is a unix-like OS. */
#  define OS_UNIX
#endif

/* We only support linux yet. */
#ifndef __linux__
#  error "Unsupported OS."
#endif

/* Determine the byte order of the system. */
#if defined(__BYTE_ORDER__)
#  if defined(__ORDER_LITTLE_ENDIAN__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#    define BO_LE
#  else
#    define BO_BE
#  endif
#elif defined(OS_UNIX) && (defined(__clang__) || (defined(__GNUC__) && (__GNUC__ >= 5)))
#  if __has_include(<sys/endian.h>) || __has_include(<endian.h>)
#    if __has_include(<endian.h>)
#      include <endian.h>
#    elif __has_include(<sys/endian.h>)
#      include <sys/endian.h>
#    endif
#    if _BYTE_ORDER == _LITTLE_ENDIAN
#      define BO_LE
#    else
#      define BO_BE
#    endif
#  else
#    error "endian.h not found."
#  endif
#else
#  error "Unable to determine system endianess."
#endif

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float    f32;
typedef double   f64;

#ifdef RVM64
#  define I64C(x) x ## L
#  define U64C(x) x ## UL
#  define V64S "l"
#else
#  define I64C(x) x ## LL
#  define U64C(x) x ## ULL
#  define V64S "ll"
#endif

#define MIN_I8   (-128)                         /* -2^(8-1)  */
#define MIN_I16  (-32768)                       /* -2^(16-1) */
#define MIN_I32  (-2147483648)                  /* -2^(32-1) */
#define MIN_I64  (I64C(-9223372036854775808))   /* -2^(64-1) */
#define MIN_U8   0                              /* 0 */
#define MIN_U16  0                              /* 0 */
#define MIN_U32  0                              /* 0 */
#define MIN_U64  U64C(0)                        /* 0 */
#define MAX_I8   127                            /* 2^(8-1)-1  */
#define MAX_I16  32767                          /* 2^(16-1)-1 */
#define MAX_I32  2147483647                     /* 2^(32-1)-1 */
#define MAX_I64  I64C(9223372036854775807)      /* 2^(64-1)-1 */
#define MAX_U8   255                            /* 2^8-1  */
#define MAX_U16  65535                          /* 2^16-1 */
#define MAX_U32  4294967295                     /* 2^32-1 */
#define MAX_U64  U64C(18446744073709551615)     /* 2^64-1 */

/* variable integer */
#ifdef RVM64
   typedef int64_t  ivar;
   typedef uint64_t uvar;
#  define MIN_IVAR MIN_I64
#  define MAX_IVAR MAX_I64
#  define MIN_UVAR MIN_U64
#  define MAX_UVAR MAX_U64
#  define VVARS "l"
#else
   typedef int32_t  ivar;
   typedef uint32_t uvar;
#  define MIN_IVAR MIN_I32
#  define MAX_IVAR MAX_I32
#  define MIN_UVAR MIN_U32
#  define MAX_UVAR MAX_U32
#  define VVARS
#endif

/* bit utils */
#define setBit(val, bit) ((val) |  (U64C(1) << (bit)))
#define clrBit(val, bit) ((val) & ~(U64C(1) << (bit)))
#define getBit(val, bit) (((val) >> (bit)) & U64C(1))

/* Reinterpret cast */
#define reinterp_cast(t,s,n) \
    (((union {    \
      s src;      \
      t trg;      \
    }){           \
      .src = (n), \
    }).trg)

#endif // RVM_CONFIG_H_
