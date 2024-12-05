#include "util.h"
#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

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
