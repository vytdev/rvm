#include "mach.h"
#include "rvmbits.h"
#include "config.h"
#include "codec.h"
#include "bcode.h"
#include "util.h"
#include "thread.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/* Process context. */
char     *src             = NULL;
uint64_t len              = 0;
uint64_t *code            = NULL;
uint64_t codelen          = 0;
uint64_t *data            = NULL;
uint64_t datalen          = 0;
/* Options. */
uint32_t default_stlen    = 524288; /* 524288 * 8 = 4MB */

char     vmstate          = V_INAC;
char     exec_mode        = X_USER;

/* Thread context. */
TLOCAL uint32_t tid       = 0;
TLOCAL uint64_t reg[RCNT];
TLOCAL uint64_t *stack    = NULL;
TLOCAL uint32_t stack_len = 0;
TLOCAL uint64_t last_pc   = 0;
TLOCAL uint64_t last_sp   = 0;
TLOCAL uint64_t last_bp   = 0;
TLOCAL uint64_t last_fl   = 0;
TLOCAL double fp_reg[RCNT];


const char *excp_msg(excp e) {
  switch (e) {
    case E_OK:      return "Ok";
    case E_ERR:     return "Internal error";
    case E_TERM:    return "Terminated";
    case E_VMCALL:  return "Invoke VM call";
    case E_PERM:    return "Permission denied";
    case E_ILL:     return "Illegal instruction";
    case E_INVC:    return "Invalid VM call";
    case E_STOVF:   return "Stack overflow";
    case E_STUND:   return "Stack underflow";
    case E_OOB:     return "Out of bounds access";
    default:        return "Unknown exception";
  }
}


void dump_regs(void) {
  #define preg(n,s) \
    fprintf(stderr, "  %%" n "  0x%016"V64S"x  %+"V64S"d\n", reg[(s)], reg[(s)])
  preg("r0 ", R0);
  preg("r1 ", R1);
  preg("r2 ", R2);
  preg("r3 ", R3);
  preg("r4 ", R4);
  preg("r5 ", R5);
  preg("r6 ", R6);
  preg("r7 ", R7);
  preg("r8 ", R8);
  preg("r9 ", R9);
  preg("r10", R10);
  preg("r11", R11);
  preg("r12", R12);
  preg("r13", R13);
  preg("r14", R14);
  preg("r15", R15);
  #undef preg
}


void dump_fp_regs(void) {
  #define preg(n,s) \
    fprintf(stderr, "  %%" n "  %+.9lf\n", fp_reg[(s)])
  preg("f0 ", R0);
  preg("f1 ", R1);
  preg("f2 ", R2);
  preg("f3 ", R3);
  preg("f4 ", R4);
  preg("f5 ", R5);
  preg("f6 ", R6);
  preg("f7 ", R7);
  preg("f8 ", R8);
  preg("f9 ", R9);
  preg("f10", R10);
  preg("f11", R11);
  preg("f12", R12);
  preg("f13", R13);
  preg("f14", R14);
  preg("f15", R15);
  #undef preg
}


static inline bool load_code(ByteCode *bd) {
  if (!bd)
    return false;
  void *shdr_p = bc_getsect(bd, ".code");
  if (!shdr_p) {
    rlog("Could not locate section: .code\n");
    return false;
  }
  rvm_shdr shdr = parse_rvm_shdr(shdr_p);
  if (shdr.entcnt == 0 || shdr.size < shdr.entcnt * 8) {
    rlog("entcnt is either zero or exceeds the section size.\n");
    return false;
  }
  char *pload = bc_getpload(bd, shdr);
  if (!pload) {
    rlog("Unable to resolve the payload of the '.code' section.\n");
    return false;
  }
  code = (uint64_t*)pload;
  codelen = shdr.entcnt;
  return true;
}


static inline bool load_data(ByteCode *bd) {
  if (!bd)
    return false;
  void *shdr_p = bc_getsect(bd, ".data");
  if (!shdr_p)
    return true; /* .data section does not exist */
  rvm_shdr shdr = parse_rvm_shdr(shdr_p);
  if (shdr.entcnt == 0 || shdr.size < shdr.entcnt * 8) {
    rlog("entcnt is either zero or exceeds the section size.\n");
    return false;
  }
  char *pload = bc_getpload(bd, shdr);
  if (!pload) {
    rlog("Unable to resolve the payload of the '.data' section.\n");
    return false;
  }
  data = (uint64_t*)pload;
  datalen  = shdr.entcnt;
  return true;
}


bool vload(char *prog, uint64_t sz, uint64_t *main_pc) {
  if (!prog || sz < 64 || !main_pc || vmstate != V_PROV)
    return false;
  if (!check_magic(prog)) {
    rlog("Invalid magic number.\n");
    return false;
  }
  ByteCode *bd = bc_open(prog, sz);
  if (!bd) {
    rlog("Invalid image header or insufficient memory.\n");
    return false;
  }
  rvmhdr hdr = bc_gethdr(bd);
  if (hdr.abi_ver != RVM_VER) {
    rlog("Unsupported ABI version: %u\n", hdr.abi_ver);
    bc_close(bd);
    return false;
  }
  if (hdr.type != RHT_LOADABLE) {
    rlog("Cannot load a non-loadable image.\n");
    bc_close(bd);
    return false;
  }
  /* Some setup. */
  *main_pc = hdr.entryp;
  src = prog;
  len = sz;
  /* Load the essential sections. */
  if (!load_code(bd) || /* required */
      !load_data(bd)) { /* optional */
    /* Failure handling. */
    bc_close(bd);
    return false;
  }
  bc_close(bd);
  return true;
}


bool vth_init(uint32_t stlen) {
  if (vmstate != V_RUNN)
    return false;
  /* Initialise the stack. */
  if (stlen == 0)
    stlen = 1;
  stack = (uint64_t*)malloc(sizeof(uint64_t) * stlen);
  if (!stack)
    return false;
  stack_len = stlen;
  /* Initialise the registers. */
  for (int i = 0; i < RCNT; i++) {
    reg[i]    = 0;
    fp_reg[i] = 0.0;
  }
  return true;
}


bool vth_free(void) {
  if (vmstate == V_PROV)
    return false;
  if (stack)
    free(stack);
  stack = NULL;
  stack_len = 0;
  for (int i = 0; i < RCNT; i++) {
    reg[i]    = 0;
    fp_reg[i] = 0.0;
  }
  return true;
}


void show_err(excp s) {
  vmfmsg(s);
  /* Some useful stats. */
  fprintf(stderr, "  abi version: v%u\n", RVM_VER);
  fprintf(stderr, "  stack used:  %"V64S"u B\n", last_sp * 8);
  fprintf(stderr, "  stack size:  %"V64S"u B\n", (u64)stack_len * 8);
  /* Dump the contents of all registers. */
  fprintf(stderr, "  (program registers)\n");
  dump_regs();
  dump_fp_regs();
  /* Dump the internal states. */
  #define print_state(name, st) \
    fprintf(stderr, "  " name "  0x%016"V64S"x  %"V64S"u\n", (st), (st))
  fprintf(stderr, "  (interpreter state)\n");
  print_state("prog cntr:", last_pc);
  print_state("stack ptr:", last_sp);
  print_state("frame ptr:", last_bp);
  #undef print_state
  fprintf(stderr, "  flags reg: ");
  /* Print the contents of the %fl register. */
  #define print_if_set(f, txt) \
    (getf(f) ? fprintf(stderr, (" " txt)) : 0)
  print_if_set(FC, "CF");
  print_if_set(FO, "OF");
  print_if_set(FS, "SF");
  print_if_set(FZ, "ZF");
  print_if_set(FE, "EF");
  print_if_set(FG, "GF");
  print_if_set(FL, "LF");
  print_if_set(FA, "AF");
  print_if_set(FB, "BF");
  print_if_set(FQ, "QF");
  #undef print_if_set
  if (last_fl == 0)
    fprintf(stderr, " NOFLAG");
  putc('\n', stderr); /* line-feed for the "flags reg" line */
}


void run_vm(int argc, char **argv) {
  #define ret_fail() exit(-1)
  #define ret_succ() exit(0)
  if (argc == 0 || !argv) {
    rlog("Internal error.\n");
    ret_fail();
  }
  uvar sz = 0;
  char *bin = util_readbin(argv[0], &sz);
  if (!bin) {
    rlog("Failed to load file: %s\n", argv[0]);
    ret_fail();
  }
  /* The entry point. */
  uint64_t main_pc = 0;
  vmstate = V_PROV;
  if (!vload(bin, sz, &main_pc)) {
    rlog("Failed to load bytecode image.\n");
    ret_fail();
  }
  vmstate = V_RUNN;
  if (!vth_init(default_stlen)) {
    rlog("Failed to initialize thread context.\n");
    ret_fail();
  }
  if (exec_mode == X_PRIV)
    rlog("warning: Running in privileged mode.\n");
  /* Run the vm. */
  vmexec(main_pc);
  vth_free();
  free(bin);
  ret_succ();
  #undef ret_fail
  #undef ret_succ
}


/* For future use. */
typedef struct vm__threadOpts {
  uint32_t tid;
  uint32_t stlen;
  uint64_t start_pc;
} vm__threadOpts;

static THREAD_FUNC(vm__threadHandler) {
  #define fail() exit(-1)
  /* Get the thread options structure. */
  if (!arg) {
    vmfmsg(E_ERR);
    rlog("Thread initialization failed.\n");
    fail();
  }
  vm__threadOpts opts = *(vm__threadOpts*)arg;
  free(arg);
  arg = NULL;
  /* Initialise the thread stack and registers. */
  if (opts.tid == 0 || !vth_init(opts.stlen)) {
    vmfmsg(E_ERR);
    rlog("Failed to initialize thread context.\n");
    fail();
  }
  tid = opts.tid;
  /* Run the interpreter. */
  vmexec(opts.start_pc);
  vth_free();
  EXIT_THREAD(0);
  #undef fail
}


/* Code for bencmarking. */
#ifdef BENCHMARK_
#  include <time.h>

TLOCAL u64 benchmark_epoch = 0;
TLOCAL u64 benchmark_break = 0;
TLOCAL u64 benchmark_insts = 0;


u64 read_mclock(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec * BILLION + ts.tv_nsec;
}


void dump_benchmark(void) {
  u64 elapsed = benchmark_break;
  double avg_tpi = (double)elapsed / benchmark_insts;
  double instr_rate = BILLION / avg_tpi;
  fprintf(stderr, "[benchmarking metrics]\n");
  fprintf(stderr, "  elapsed time:    %"V64S"u ns\n",    elapsed);
  fprintf(stderr, "  instr count:     %"V64S"u insts\n", benchmark_insts);
  fprintf(stderr, "  avg tpi:         %.3lf ns\n",       avg_tpi);
  fprintf(stderr, "  instr rate:      %.3lf ips\n",      instr_rate);
}

#endif /* defined(BENCHMARK_) */
