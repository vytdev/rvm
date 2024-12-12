#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "mach.h"
#include "config.h"
#include "util.h"

/* Print help to stderr. */
void show_help(void);

/* Print version info. */
void show_version(void);


int main(int argc, char **argv) {
  main_argc = argc;
  main_argv = argv;

  if (argc == 1) {
    rlog("Type '%s -h' for more info.\n", argv[0]);
    return 1;
  }

  int i;
  bool opt_help = false;
  bool opt_version = false;

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
      #define lopt(n) if (strcmp(arg+2, (n)) == 0)

      lopt("help") {
        opt_help = true;
        continue;
      }

      lopt("version") {
        opt_version = true;
        continue;
      }

      rlog("Unrecognized option: %s\n", arg);
      return 1;

      #undef loppt
    }

    /* Single-dash options. */
    #define opt(n) if (strcmp(arg+1, (n)) == 0)

    opt("p0") {
      exec_mode = X_PRIV;
      continue;
    }

    opt("p1") {
      exec_mode = X_USER;
      continue;
    }

    opt("p2") {
      exec_mode = X_GUEST;
      continue;
    }

    #undef opt

    /* For short options. */
    for (int j = 1; j < len; j++) switch (arg[j]) {
      case 'h':
        opt_help = true;
        break;
      case 'v':
        opt_version = true;
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

  /* Show version and exit. */
  if (opt_version) {
    show_version();
    return 0;
  }

  if (argc - i == 0) {
    rlog("Please provide a bytecode file.\n");
    return 1;
  }

  /* Run the VM. */
  run_vm(argc - i, argv + i);
}


void show_help(void) {
  fprintf(stderr,
    "Usage: %s [options] file [args...]\n"
    "\n"
    "Options:\n"
    "        --            Stop parsing options.\n"
    "    -h, --help        Show this help and exit.\n"
    "    -v, --version     Show version and build info.\n"
    "        -p0           Run in priviledged mode (unsafe).\n"
    "        -p1           Run in user mode (default).\n"
    "        -p2           Run in guest mode (restricted).\n"
    "\n"
    "Arguments:\n"
    "    file              The bytecode image file.\n"
    "    args...           Program args to pass.\n"
    "\n", main_argv[0]);

  #ifdef PERF_
  fprintf(stderr, "Fast mode is enabled.\n");
  #endif

  fprintf(stderr, "Base ABI version: %u\n", RVM_VER);
  fprintf(stderr, "Copyright (c) 2024 Vincent Yanzee J. Tan <vytdev>\n");
}


void show_version(void) {
  printf("Redstone Abstract Virtual Machine (rvm)\n");
  printf("Compiled on %s at %s\n", __DATE__, __TIME__);
  printf("Base ABI version: %u\n", RVM_VER);

  /* Build type. */
  #if defined(DEBUG_)
  printf("Build type: debug\n");
  #elif defined(PERF_)
  printf("Build type: perf\n");
  #else
  printf("Build type: release\n");
  #endif

  /* Benchmarking note. */
  #ifdef BENCHMARK_
  printf("Compiled for benchmarking.\n");
  #endif
}
