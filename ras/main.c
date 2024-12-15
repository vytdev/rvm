#include <stdio.h>
#include "util.h"
#include "../rvm/config.h"

int main(int argc, char **argv) {
  main_argc = argc;
  main_argv = argv;

  if (argc < 2) {
    rlog("Please provide a file.\n");
    return 1;
  }

  uvar sz = 0;
  char *txt = util_readtext(argv[1], &sz);

  if (!txt) {
    rlog("Failed to load file: %s\n", argv[1]);
    return 1;
  }

  printf("%s\n", txt);
  return 0;
}
