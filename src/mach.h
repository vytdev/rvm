#ifndef RVM_MACH_H_
#define RVM_MACH_H_
#include <stdint.h>
#include <stdbool.h>
#include "rvmbits.h"
#include "thread.h"


/* The context of the entire program */
extern char     *src;
extern uint64_t len;
extern uint64_t *data;
extern uint64_t datalen;
extern uint32_t default_stlen;

/* Per-thread context */
extern TLOCAL uint64_t reg[16];
extern TLOCAL uint64_t *stack;
extern TLOCAL uint32_t stack_len;

/* VM states */
extern char vmstate;
#define V_INAC 0 /* Inactive */
#define V_PROV 1 /* Provisioning */
#define V_RUNN 2 /* Running */
#define V_SUSP 3 /* Suspended */

/* Execution modes */
extern char exec_mode;
#define X_PRIV  0 /* Privileged mode */
#define X_USER  1 /* User mode (default) */
#define X_GUEST 2 /* Guest mode */
#define has_perm(lvl) (exec_mode <= (lvl))

/* Status codes */
typedef int statcd;
#define S_OK      0 /* Ok */
#define S_ERR     1 /* Internal error */
#define S_PERM    2 /* Permission denied */
#define S_ILL     3 /* Illegal instruction */
#define S_INVC    4 /* Invalid VM call */
#define S_STOVF   5 /* Stack overflow */
#define S_STUND   6 /* Stack underflow */
#define S_OOB     7 /* Out of bounds access */

/* Flag manipulation */
#define setf(f) (reg[RFL] |= (f))
#define cmlf(f) (reg[RFL] ^= (f))
#define clrf(f) (reg[RFL] &= ~(uint64_t)(f))
#define getf(f) (reg[RFL] & (f))


/* Check the magic number in src. */
bool checkmagic(char *src, uint64_t len);

/* Parse rvmhdr structure from src. */
bool parse_rvmhdr(char *src, uint64_t len, rvmhdr *out);

/* Return stat code strings. */
const char *statcd_msg(statcd n);

/* Dump the caller thread's registers into stderr. */
void dump_regs(void);

/* Load a program. The vm state must be in provisioning. */
bool vload(char *prog, uint64_t sz, uint64_t *main_pc);

/* Initialise thread ctx. The vm must be in running state to use this. */
bool vth_init(uint32_t stlen, uint64_t start_pc);

/* Free thread ctx. Not allowed in provisioning state. */
bool vth_free(void);

/* Stack push and pop funcs. */
statcd vpush(uint64_t v);
statcd vpop(uint64_t *o);

/* Print the caller thread's stats as a runtime error. */
void show_err(statcd s);

/* Load and run a bytecode image. [noreturn] */
void run_vm(int argc, char **argv);

/* Interpret the bytecode for the current thread. [noreturn if error] */
void vmexec(void);

/* VM Call. */
statcd vmcall(uint16_t ndx);


#ifdef BENCHMARK_
#  ifndef __linux__
#    error "Benchmark mode only compiles on linux."
#  endif

extern TLOCAL u64 benchmark_epoch;
extern TLOCAL u64 benchmark_break;
extern TLOCAL u64 benchmark_insts;

#define benchmark_incr() (benchmark_insts++)
#define benchmark_init() (benchmark_epoch = read_mclock())
#define benchmark_curr() (read_mclock() - benchmark_epoch)
#define benchmark_tag()  (benchmark_break = benchmark_curr())

/* Get nanosecond time from the monotonic clock. */
u64 read_mclock(void);

/* Dump the current benchmark. */
void dump_benchmark(void);

#else

#define benchmark_epoch
#define benchmark_break
#define benchmark_insts
#define benchmark_incr()
#define benchmark_init()
#define benchmark_curr()
#define benchmark_tag()
#define read_mclock() (U64C(0))
#define dump_benchmark()

#endif /* defined(BENCHMARK_) */

#endif // RVM_MACH_H_
