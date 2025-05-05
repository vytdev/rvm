#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "../rvm.h"

#if defined(__linux__) || defined(__unix) || defined(__unix__)
#  include <unistd.h>
#  if RVM_CFG_COUNT_INSTRS_EXEC && (_POSIX_VERSION >= 200112L)
#    define BENCH_
#    include <time.h>
#    if defined(CLOCK_MONOTONIC)
#      define CLOCK_USED  CLOCK_MONOTONIC
#    else
#      define CLOCK_USED  CLOCK_REALTIME
#    endif
#    define BILLION (1000 * 1000 * 1000)

typedef rvm_u64 tstamp;
static inline tstamp get_tstamp(void)
{
  struct timespec ts;
  clock_gettime(CLOCK_USED, &ts);
  return (tstamp)ts.tv_sec * BILLION + ts.tv_nsec;
}

static void dump_benchmark(struct rvm *ctx, tstamp elapsed)
{
  double avg_tpi = (double)elapsed / ctx->inst_cnt;
  double instr_rate = BILLION / avg_tpi;
  printf("[benchmarking metrics]\n");
  printf("  elapsed time:    %" PRIu64 " ns\n",    elapsed);
  printf("  instr count:     %" PRIu64 " insts\n", ctx->inst_cnt);
  printf("  avg tpi:         %.3lf ns\n",        avg_tpi);
  printf("  instr rate:      %.3lf ips\n",       instr_rate);
}

#  endif
#endif


static void dump_state(struct rvm *ctx, signed stat)
{
  printf("vm ended [%d]: %s\n", stat, rvm_strstat(stat));
  printf("  memsz      %zu bytes\n", ctx->memsz);
  printf("  exec opt   0x%04x\n",    ctx->exec_opts);
  printf("  arch ver   %u\n",    RVM_ARCH);
  printf("  impl ver   %u\n",    RVM_IMPL);
  printf("  ver num    %lu\n",   RVM_VERSION);
# define prnt_reg(name, rg) \
  printf("  %-5s  0x%016" PRIx64 "  % " PRIi64 "\n", #name, rg, rg)
  /* cf */
  printf("  <cf>  ");
  if (ctx->cf & RVM_FEQ) printf(" EQ");
  if (ctx->cf & RVM_FAB) printf(" AB");
  if (ctx->cf & RVM_FGT) printf(" GT");
  if (!ctx->cf) printf(" NOFLAGS");
  putc('\n', stdout);
  /* other regs */
  prnt_reg(<pc>,  ctx->pc);
  prnt_reg(r0,  ctx->reg[RVM_R0]);
  prnt_reg(r1,  ctx->reg[RVM_R1]);
  prnt_reg(r2,  ctx->reg[RVM_R2]);
  prnt_reg(r3,  ctx->reg[RVM_R3]);
  prnt_reg(r4,  ctx->reg[RVM_R4]);
  prnt_reg(r5,  ctx->reg[RVM_R5]);
  prnt_reg(r6,  ctx->reg[RVM_R6]);
  prnt_reg(r7,  ctx->reg[RVM_R7]);
  prnt_reg(r8,  ctx->reg[RVM_R8]);
  prnt_reg(r9,  ctx->reg[RVM_R9]);
  prnt_reg(r10, ctx->reg[RVM_R10]);
  prnt_reg(r11, ctx->reg[RVM_R11]);
  prnt_reg(r12, ctx->reg[RVM_R12]);
  prnt_reg(r13, ctx->reg[RVM_R13]);
  prnt_reg(r14, ctx->reg[RVM_R14]);
  prnt_reg(r15, ctx->reg[RVM_R15]);
# undef prnt_reg
  puts(RVM_LABEL);
}


static char *load_file(const char *path, rvm_uint *memsz,
                       const char *progname)
{
  FILE *fp = fopen(path, "rb");
  if (!fp) {
    fprintf(stderr,  ""
        "%s: Could not open file: %s\n",
        progname, path);
    return NULL;
  }

  fseek(fp, 0, SEEK_END);
  rvm_uint sz = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  rvm_uint reqmem = rvm_calcfmem(sz);

  char *mem = (char*)malloc(reqmem);
  if (!mem) {
    fprintf(stderr, ""
       "%s: Failed to allocate memory with size: %zu bytes\n",
       progname, sz);
    fclose(fp);
    return NULL;
  }

  #define buff_sz 4096
  rvm_uint pos = 0, rdneed = 0, rdlen = 0;

  /* Do a buffered read. */
  while (pos < sz) {
    rdneed = sz - pos > buff_sz ? buff_sz : sz - pos;
    rdlen = fread(mem + pos, 1, rdneed, fp);

    if (rdlen != rdneed) {
      fprintf(stderr, ""
        "%s: Didn't finished file read (pos=%zu, rdneed=%zu, rdlen=%zu)",
        progname, pos, rdneed, rdlen);
      free(mem);
      fclose(fp);
      return NULL;
    }

    pos += rdlen;
  }

  fclose(fp);
  if (memsz)
    *memsz = reqmem;
  return mem;
}


int main(int argc, char **argv)
{
  if (argc != 2) {
    fprintf(stderr, ""
      "usage: %s path\n"
      RVM_LABEL ": Loads and executes 'path'.\n"
      RVM_COPYRIGHT "\n"
      , argv[0]
    );
    return 1;
  }

  rvm_uint memsz = 0;
  char *mem = load_file(argv[1], &memsz, argv[0]);
  if (!mem)
    return 1;
  struct rvm ctx = rvm_new(mem, memsz);
  ctx.pc = 0;

  #if defined(BENCH_)
  tstamp start = get_tstamp();
  #endif

  signed stat = rvm_exec(&ctx);

  #if defined(BENCH_)
  dump_benchmark(&ctx, get_tstamp() - start);
  #endif
  dump_state(&ctx, stat);

  return stat;
}
