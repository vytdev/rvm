#include "../rvm/util.h"
#include "../rvm/config.h"
#include <stdio.h>
#include <stdlib.h>

char *util_readtext(char *path, uvar *size_out) {
  if (!path || !size_out)
    return NULL;
  /* Open the file. */
  FILE *fp = fopen(path, "r");
  if (!fp)
    return NULL;
  /* Get the file size. */
  fseek(fp, 0, SEEK_END);
  uvar size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  /* Allocate buffer. */
  char *buff = (char*)util_malloc1(size+1);
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
  buff[size] = '\0';
  return buff;
}
