#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "mach.h"
#include "config.h"
#include "util.h"


void show_help(void) {
  fprintf(stderr,
    "Usage: %s [options] file [arg...]\n"
    "\n"
    "Options:\n"
    "      --            Stop parsing options.\n"
    "  -h, --help        Show this help and exit\n"
    "\n"
    "Arguments:\n"
    "  file              The bytecode image file.\n"
    "  arg...            Program args to pass.\n"
    "\n"
    "Read https://github.com/vytdev/rvm for more info.\n"
    "Copyright (c) 2024 Vincent Yanzee J. Tan\n"
  ,main_argv[0]);
}


int run_vm(int argc, char **argv) {
  uvar sz = 0;
  char *bin = util_readbin(argv[0], &sz);
  if (!bin) {
    rlog("Failed to load file: %s\n", argv[0]);
    return 1;
  }

  vmstate = V_PROV;
  if (!vload(bin, sz)) {
    rlog("Failed to load executable image.\n");
    return 1;
  }

  vmstate = V_RUNN;
  if (!vth_init(524288)) {
    rlog("Failed to initialize thread context.\n");
    return 1;
  }

  statcd s = S_OK;
  while (vmstate == V_RUNN) {
    s = vmexec();
    if (s != S_OK) {
      rlog("%s\n", statcd_msg(s));
      free(bin);
      return -1;
    }
  }

  vth_free();
  free(bin);
  return exitcode;
}


int main(int argc, char **argv) {
  main_argc = argc;
  main_argv = argv;

  if (argc == 1) {
    rlog("Type '%s -h' for more info.\n", argv[0]);
    return 1;
  }

  int i;
  bool opt_help = false;

  for (i = 1; i < argc; i++) {
    char *arg = argv[i];
    uvar len = strlen(arg);

    /* Only check for options. */
    if (len == 0 || arg[0] != '-')
      break;

    /* '--' delimeter. */
    if (len == 2 && arg[1] == '-') {
      i++;
      break;
    }

    /* For long options. */
    if (len > 2 && arg[1] == '-') {
      arg += 2;
      #define ifcase(n) if (strcmp(arg, (n)) == 0)

      ifcase("help") {
        opt_help = true;
        continue;
      }
      else {
        rlog("Unrecognized option: --%s\n", arg);
        return 1;
      }

      #undef ifcase
      continue;
    }

    /* For short options. */
    for (int j = 1; j < len; j++) switch (arg[j]) {
      case 'h':
        opt_help = true;
        break;
      default:
        rlog("Unknown option: -%c\n", arg[j]);
        return 1;
    }
  }

  /* Show help and exit. */
  if (opt_help) {
    show_help();
    return 1;
  }

  if (argc - i == 0) {
    rlog("Please provide a bytecode file.\n");
    return 1;
  }

  /* Run the VM. */
  return run_vm(argc - i, argv + i);
}
