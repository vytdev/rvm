#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "mach.h"
#include "config.h"
#include "util.h"
#include "rvmbits.h"

/* Print help to stderr. */
void show_help(void);

/* Print version info. */
void show_version(void);


int main(int argc, char **argv) {
  main_argc = argc;
  main_argv = argv;

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

    /* Long options. */
    #define opt(n) \
      if (strcmp(arg, (n)) == 0)
    /* Extensible long options. */
    #define xopt(n) \
      if (strncmp(arg, (n), strlen((n))) == 0)

    opt("--help") {
      opt_help = true;
      continue;
    }

    opt("--version") {
      opt_version = true;
      continue;
    }

    opt("-p0") {
      exec_mode = X_PRIV;
      continue;
    }

    opt("-p1") {
      exec_mode = X_USER;
      continue;
    }

    opt("-p2") {
      exec_mode = X_GUEST;
      continue;
    }

    xopt("-mss") {
      u64 val = pdatasz(arg+4);
      if (val == MAX_U64) {
        rlog("Invalid value: %s\n", arg+4);
        return 1;
      }
      default_stlen = val / 8;
      continue;
    }

    #undef opt
    #undef xopt

    /* For short options. */
    for (int j = 1; j < len; j++) switch (arg[j]) {
      case 'h':
      case '?':
        opt_help = true;
        break;
      case 'v':
        opt_version = true;
        break;
      default:
        rlog("Unknown option: %s\n", arg);
        rlog("Type '%s -?' for more info.\n", argv[0]);
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
    rlog("No input bytecode file.\n");
    rlog("Type '%s -?' for more info.\n", argv[0]);
    return 1;
  }

  /* Run the VM. */
  run_vm(argc - i, argv + i);
}


void show_help(void) {
  fprintf(stderr,
    "Usage: %s [options] file [args...]\n"
    "\n"
    "    --                Stop parsing options.\n"
    "    -h, -?, --help    Show this help and exit.\n"
    "    -v, --version     Show version and build info.\n"
    "\n"
    "  VM execution modes:\n"
    "    -p0               Run in privileged mode (unsafe).\n"
    "    -p1               Run in user mode (default).\n"
    "    -p2               Run in guest mode (restricted).\n"
    "\n"
    "  Machine tuning:\n"
    "    -mss<size>        Set the default thread stack size.\n"
    "\n"
    "Positional arguments:\n"
    "    file              The bytecode image file.\n"
    "    args...           Program args to pass.\n"
    "\n", main_argv[0]);

  fprintf(stderr, "Base ABI version: v%u\n", RVM_VER);
  fprintf(stderr, "Copyright (c) 2024-2025 Vincent Yanzee J. Tan <vytdev>\n");
}


void show_version(void) {
  printf("Redstone Abstract Virtual Machine (rvm)\n");
  printf("Copyright (c) 2024-2025 Vincent Yanzee J. Tan <vytdev>\n");
  printf("Built on %s at %s\n", __DATE__, __TIME__);
  printf("Base ABI version: v%u\n", RVM_VER);
  printf("Opcode count: %u ops\n", opcnt);

  /* Build type. */
  #if defined(DEBUG_)
  printf("Build type: debug\n");
  #else
  printf("Build type: release\n");
  #endif

  /* Benchmarking note. */
  #ifdef BENCHMARK_
  printf("Compiled for benchmarking.\n");
  #endif
}
