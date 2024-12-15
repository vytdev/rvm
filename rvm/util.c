#include "util.h"
#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

int    main_argc;
char **main_argv;


void *util_malloc1(uvar size) {
  return malloc(size ? size : 1);
}


uvar util_bfread(char *buf, uvar size, FILE *fp) {
  if (!buf || size == 0 || !fp)
    return 0;
  uvar pos = 0;
  while (pos < size) {
    uvar needed = (size - pos < 4096) ? (size - pos) : 4096;
    uvar bread = fread(buf + pos, 1, needed, fp);
    pos += bread;
    if (needed != bread)
      return pos;
  }
  return pos;
}


uvar util_bfwrite(char *buf, uvar size, FILE *fp) {
  if (!buf || size == 0 || !fp)
    return 0;
  uvar pos = 0;
  while (pos < size) {
    uvar pending = (size - pos < 4096) ? (size - pos) : 4096;
    uvar bwrite = fwrite(buf + pos, 1, pending, fp);
    pos += bwrite;
    if (pending != bwrite)
      return pos;
  }
  return pos;
}


char *util_readbin(char *path, uvar *size_out) {
  if (!path || !size_out)
    return NULL;
  /* Open the file. */
  FILE *fp = fopen(path, "rb");
  if (!fp)
    return NULL;
  /* Get the file size. */
  fseek(fp, 0, SEEK_END);
  uvar size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  /* Allocate buffer. */
  char *buff = (char*)util_malloc1(size);
  if (!buff) {
    fclose(fp);
    return NULL;
  }
  /* Read the file. */
  if (util_bfread(buff, size, fp) != size) {
    fclose(fp);
    free(buff);
    return NULL;
  }
  fclose(fp);
  *size_out = size;
  return buff;
}


bool util_writebin(char *path, char *buff, uvar size) {
  if (!path || !buff || size == 0)
    return false;
  /* Open the file for writing. */
  FILE *fp = fopen(path, "wb");
  if (!fp)
    return NULL;
  /* Write the buffer. */
  if (util_bfwrite(buff, size, fp) != size) {
    fclose(fp);
    return false;
  }
  fclose(fp);
  return true;
}


void rlog(const char *fmt, ...) {
  if (main_argv)
    fprintf(stderr, "%s: ", main_argv[0]);
  else
    fprintf(stderr, "?: ");
  va_list arg;
  va_start(arg, fmt);
  vfprintf(stderr, fmt, arg);
  va_end(arg);
}


i64 p64s(char *str, int base, char **endptr) {
  if (!str)
    return 0;
  u64 result = 0;
  int sign = 1;
  /* Parse sign. */
  if (*str == '+') {
    str++;
  }
  else if (*str == '-') {
    str++;
    sign = -1;
  }
  /* Auto-detect base. */
  if (base == 0) {
    if (*str == '0') {
      str++;
      if (*str == 'b') {
        str++;
        base = 2;
      }
      else if (*str == 'x') {
        str++;
        base = 16;
      }
      else
        base = 8;
    }
    else
      base = 10;
  }
  /* Validate base. */
  if (base < 2 || base > 36)
    return 0;
  /* Parse the string. */
  while (*str != '\0') {
    char c = *str;
    char val = 0;
    if (c >= '0' && c <= '9')
      val = c - '0';
    else if (c >= 'a' && c <= 'z')
      val = c - 'a' + 10;
    else if (c >= 'A' && c <= 'Z')
      val = c - 'A' + 10;
    else
      break;
    if (val >= base)
      break;
    result = result * base + val;
    str++;
  }
  /* Some finalisation. */
  if (endptr)
    *endptr = str;
  return result * sign;
}


u64 pdatasz(char *str) {
  if (!str)
    return 0;
  char *end = str;
  u64 val = p64s(str, 10, &end);
  /* Process data size magnitudes. */
  u64 factor = 1;
  #define mag(c, f) \
    case (c): {     \
      factor = U64C(f); \
      end++; \
      break; \
    }
  switch (*end) {
    mag('k', 1000);          /* 10^3  */
    mag('m', 1000000);       /* 10^6  */
    mag('g', 1000000000);    /* 10^9  */
    mag('t', 1000000000000); /* 10^12 */
    mag('K', 1024);          /* 2^10  */
    mag('M', 1048576);       /* 2^20  */
    mag('G', 1073741824);    /* 2^30  */
    mag('T', 1099511627776); /* 2^40  */
  }
  #undef mag
  if (*end == 'B')
    end++;
  if (*end != '\0')
    return MAX_U64;
  return val * factor;
}
