#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "../rvm.h"


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

  char *mem = (char*)malloc(sz);
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
    *memsz = sz;
  return mem;
}


static void dump_state(struct rvm *ctx, signed stat)
{
  printf("vm fault [%d]: %s\n", stat, rvm_strstat(stat));
  printf("  memsz      %zu bytes\n", ctx->memsz);
  printf("  exec opt   0x%04x\n",    ctx->exec_opts);
  printf("  arch ver   %u\n",    RVM_ARCH);
  printf("  impl ver   %u\n",    RVM_IMPL);
  printf("  ver num    %lu\n",   RVM_VERSION);
# define prnt_reg(name, rg) \
  printf("  %-5s  0x%016"PRIx64"  % "PRIi64"\n", #name, rg, rg);
  prnt_reg(<pc>,  ctx->pc << 2);
  prnt_reg(<cf>,  (rvm_reg_t)ctx->cf);
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


int main(int argc, char **argv)
{
  if (argc != 2) {
    fprintf(stderr, ""
      "usage: %s path\n"
      RVM_LABEL ": Loads and executes the p-code image at 'path'.\n"
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

  signed stat = RVM_EOK;
  while (stat == RVM_EOK)
    stat = rvm_exec(&ctx);

  dump_state(&ctx, stat);
  return stat;
}
