#ifndef RVM_UTIL_H_
#define RVM_UTIL_H_
#include "config.h"
#include <stdio.h>
#include <stdbool.h>

/* Arguments passed to this vm. */
extern int    main_argc;
extern char **main_argv;

/* Malloc atleast 1 byte. */
void *util_malloc1(uvar size);

/* Buffered fread. */
uvar util_bfread(char *buf, uvar size, FILE *fp);

/* Buffered fwrite. */
uvar util_bfwrite(char *buf, uvar size, FILE *fp);

/* Reads a binary file. */
char *util_readbin(char *path, uvar *size_out);

/* Writes to a binary file. */
bool util_writebin(char *path, char *buff, uvar size);

/* Print a log with the executable name. */
void rlog(const char *fmt, ...);

/* Parse 64-bit integer from a C-str. Similar to strtoll. */
i64 p64s(char *str, int base, char **endptr);

/* Parse data sizes. */
u64 pdatasz(char *str);

/* Bit utils */
#define bit_tst(x,p)  (((x) >> (p)) & 0x1)
#define bit_set(x,p)  ((x) |  (U64C(1) << (p)))
#define bit_clr(x,p)  ((x) & ~(U64C(1) << (p)))
#define bit_cml(x,p)  ((x) ^  (U64C(1) << (p)))
#define rol64(v, c) (((v) << (c)) | ((v) >> (64-(c))))
#define ror64(v, c) (((v) >> (c)) | ((v) << (64-(c))))
#define mod64(x) ((x) & 0x3f) /* x % 64 */

/* Reinterpret cast */
#define reinterp_cast(t,s,n) \
    (((union {    \
      s src;      \
      t trg;      \
    }){           \
      .src = (n), \
    }).trg)

#endif // RVM_UTIL_H_
