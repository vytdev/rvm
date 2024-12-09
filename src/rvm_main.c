#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "mach.h"
#include "config.h"
#include "util.h"

/* Code for bencmarking. */
#ifdef BENCHMARK_
#  ifndef __linux__
#    error "Benchmark mode only compiles on linux."
#  endif
#  include <time.h>
#  define BILLION 1000000000
static inline u64 read_mclock(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec * BILLION + ts.tv_nsec;
}
#endif

/* Print help to stderr. */
void show_help(void);

/* Load and run a bytecode image. */
int run_vm(int argc, char **argv);


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
      arg += 2;
      #define ifcase(n) if (strcmp(arg, (n)) == 0)

      ifcase("help") {
        opt_help = true;
        continue;
      }
      else ifcase("version") {
        opt_version = true;
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
    printf("Redstone Virtual Machine (rvm)\n");
    printf("Compiled on %s at %s\n", __DATE__, __TIME__);
    printf("ABI version %u\n", RVM_VER);

    #if defined(DEBUG_)
    printf("Build type: debug\n");
    #elif defined(PERF_)
    printf("Build type: perf\n");
    #else
    printf("Build type: release\n");
    #endif

    #ifdef BENCHMARK_
    printf("Compiled for benchmarking.\n");
    #endif

    return 0;
  }

  if (argc - i == 0) {
    rlog("Please provide a bytecode file.\n");
    return 1;
  }

  /* Run the VM. */
  return run_vm(argc - i, argv + i);
}


void show_help(void) {
  fprintf(stderr,
    "Usage: %s [options] file [args...]\n"
    "\n"
    "Options:\n"
    "        --            Stop parsing options.\n"
    "    -h, --help        Show this help and exit.\n"
    "    -v, --version     Show version and build info.\n"
    "\n"
    "Arguments:\n"
    "    file              The bytecode image file.\n"
    "    args...           Program args to pass.\n"
    "\n", main_argv[0]);

  #ifdef PERF_
  fprintf(stderr,
    "Performance mode is enabled to enhance speed.\n"
    "Note: Undefined behaviour may occur, though it is rare and unlikely\n"
    "to cause issues.\n"
    "\n");
  #endif

  fprintf(stderr, "RVM Version: %u\n", RVM_VER);
  fprintf(stderr, "Copyright (c) 2024 Vincent Yanzee J. Tan <vytdev>\n");
}


int run_vm(int argc, char **argv) {
  uvar sz = 0;
  char *bin = util_readbin(argv[0], &sz);
  if (!bin) {
    rlog("Failed to load file: %s\n", argv[0]);
    return 1;
  }

  /* The entry point. */
  uint64_t main_pc = 0;

  vmstate = V_PROV;
  if (!vload(bin, sz, &main_pc)) {
    rlog("Failed to load executable image.\n");
    return 1;
  }

  vmstate = V_RUNN;
  if (!vth_init(524288, main_pc)) {
    rlog("Failed to initialize thread context.\n");
    return 1;
  }

  #ifdef BENCHMARK_
  register u64 inst = 0;
  u64 start = read_mclock();
  #endif

  /* The execution loop. */
  statcd s = S_OK;
  while (vmstate == V_RUNN) {
    s = vmexec();
    if (s != S_OK) {
      vmstate = V_ERRR;
      rlog("%s\n\n", statcd_msg(s));
      dump_regs();
      putc('\n', stderr);
      exitcode = -1;
      break;
    }
    #ifdef BENCHMARK_
    inst++;
    #endif
  }

  #ifdef BENCHMARK_
  u64 end = read_mclock();
  u64 elapsed = end - start;
  printf("BENCHMARK RESULTS:\n");
  printf("elapsed time:       %"V64S"u ns\n",         elapsed);
  printf("avg time per instr: %"V64S"u ns\n",         elapsed / inst);
  printf("instr count:        %"V64S"u insts\n",      inst);
  printf("instr rate:         %"V64S"u inst / sec\n", BILLION / (elapsed / inst));
  #endif

  vth_free();
  free(bin);
  return exitcode;
}
