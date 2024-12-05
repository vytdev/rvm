#ifndef RVM_UTIL_H_
#define RVM_UTIL_H_
#include "config.h"
#include <stdio.h>

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

/* Print a log with the executable name. */
void rlog(const char *fmt, ...);

#endif // RVM_UTIL_H_
