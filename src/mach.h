#ifndef RVM_MACH_H_
#define RVM_MACH_H_
#include <stdint.h>
#include <stdbool.h>
#include "rvmbits.h"
#include "thread.h"


/* The context of the entire program */
extern char     *src;
extern uint64_t len;
extern uint64_t *code;
extern uint64_t codelen;
extern uint64_t *data;
extern uint64_t datalen;
/* Program options */
extern uint32_t default_stlen;

/* Per-thread context */
extern TLOCAL uint32_t tid;
extern TLOCAL uint64_t reg[RCNT];
extern TLOCAL uint64_t *stack;
extern TLOCAL uint32_t stack_len;
extern TLOCAL uint64_t last_pc;
extern TLOCAL uint64_t last_sp;
extern TLOCAL uint64_t last_bp;
extern TLOCAL uint64_t last_fl;
extern TLOCAL double fp_reg[RCNT];

/* VM states */
extern char vmstate;
#define V_INAC 0 /* Inactive */
#define V_PROV 1 /* Provisioning */
#define V_RUNN 2 /* Running */

/* Execution modes */
extern char exec_mode;
#define X_PRIV  0 /* Privileged mode */
#define X_USER  1 /* User mode (default) */
#define X_GUEST 2 /* Guest mode */
#define has_perm(lvl) (exec_mode <= (lvl))

/* Flag manipulation */
#define setf(f) (last_fl |= (f))
#define cmlf(f) (last_fl ^= (f))
#define clrf(f) (last_fl &= ~(uint64_t)(f))
#define getf(f) (last_fl & (f))

#define vmfmsg(s) \
  (rlog("vm fault [%d]: %s\n", (s), excp_msg((s))))


/* Return exception code strings. */
const char *excp_msg(excp e);

/* Dump the caller thread's registers into stderr. */
void dump_regs(void);

/* Dump the floating-point registers into stderr. */
void dump_fp_regs(void);

/* Load a program. The vm state must be in provisioning. */
bool vload(char *prog, uint64_t sz, uint64_t *main_pc);

/* Initialise thread ctx. The vm must be in running state to use this. */
bool vth_init(uint32_t stlen);

/* Free thread ctx. Not allowed in provisioning state. */
bool vth_free(void);

/* Print the caller thread's stats as a runtime error. */
void show_err(excp e);

/* Load and run a bytecode image. [noreturn] */
void run_vm(int argc, char **argv);

/* Interpret the bytecode for the current thread. [noreturn if error] */
void vmexec(uint64_t start_pc);

/* VM Call. */
excp vmcall(void);


#ifdef BENCHMARK_
#  ifndef __linux__
#    error "Benchmark mode only compiles on linux."
#  endif

extern TLOCAL u64 benchmark_epoch;
extern TLOCAL u64 benchmark_break;
extern TLOCAL u64 benchmark_insts;

#define benchmark_init() (benchmark_epoch = read_mclock())
#define benchmark_curr() (read_mclock() - benchmark_epoch)
#define benchmark_tag()  (benchmark_break = benchmark_curr())

/* Get nanosecond time from the monotonic clock. */
u64 read_mclock(void);

/* Dump the current benchmark. */
void dump_benchmark(void);

#else

#define benchmark_epoch 0
#define benchmark_break 0
#define benchmark_insts 0

#define benchmark_init() 0
#define benchmark_curr() 0
#define benchmark_tag() 0

#define read_mclock() (U64C(0))
#define dump_benchmark()

#endif /* defined(BENCHMARK_) */

#endif // RVM_MACH_H_
