#include "mach.h"
#include "rvmbits.h"
#include "codec.h"
#include "config.h"
#include "util.h"
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#define RVM_INTERP_

/* Prototype for the vm__interpreter() function. */
static excp vm__interpreter(uint64_t start_pc);

/* Some interpreter macros. */
#ifdef BO_LE
#  define fetch() (code[pc++])
#else
#  define fetch() (read64((char*)(void*)(code + pc++)))
#endif

#define inext()   goto interp_start

/* Raise VM exception. */
#define __rvm_stop(e) do { \
    last_pc = pc; \
    last_bp = bp; \
    last_sp = sp; \
    last_fl = fl; \
    return (e);   \
  } while (0)

#ifdef BENCHMARK_
#  define stop(e) do { \
     benchmark_insts += icnt; \
     __rvm_stop((e));  \
   } while (0)
#else
#  define stop(e) __rvm_stop((e))
#endif

/* Replace the flag manipulation macros. */
#ifdef setf
#  undef setf
#endif
#define setf(f) (fl |= (f))

#ifdef cmlf
#  undef cmlf
#endif
#define cmlf(f) (fl ^= (f))

#ifdef clrf
#  undef clrf
#endif
#define clrf(f) (fl &= ~(uint64_t)(f))

#ifdef getf
#  undef getf
#endif
#define getf(f) (fl & (f))

/* Macro to push into stack. */
#define push(v) do {     \
    if (sp >= stack_len) \
      stop(E_STOVF);     \
    stack[sp++] = (v);   \
  } while (0)

/* Macro to pop from stack. */
#define pop(v) do {     \
    if (sp == 0)        \
      stop(E_STUND);    \
    (v) = stack[--sp];  \
  } while (0)

/* Detect if we can use direct threading dispatch. */
#if defined(__GNUC__) || defined(__clang__)
#  define USE_THREADED_DISPATCH
#endif


void vmexec(uint64_t start_pc) {
  last_pc = start_pc;
  excp e = E_OK;

  benchmark_init();
  while (1) {
    if (e == E_OK) {
      e = vm__interpreter(last_pc);
      continue;
    }
    if (e == E_VMCALL) {
      e = vmcall();
      continue;
    }
    break;
  }
  benchmark_tag();

  /* Interpreter complete. Do some additional state checks. */
  if (e != E_OK)
    show_err(e);

  if (tid == 0) /* Only dump benchmarks from the main thread. */
    dump_benchmark();

  if (e != E_OK) {
    rlog("Error is unrecoverable. Aborting...\n");
    exit(-1);
  }
}


static excp vm__interpreter(uint64_t start_pc) {
  register uint64_t pc = start_pc;
  register uint64_t bp = last_bp;
  register uint64_t sp = last_sp;
  register uint64_t fl = last_fl;
  #ifdef BENCHMARK_
  register uint64_t icnt = 0;
  #endif

  /* Create the dispatch table. */
  #ifdef USE_THREADED_DISPATCH
  void * const disptab[opcnt] = {
    #define vminst(n, c...) [OP_##n] = &&D_##n,
    #include "exec_defs.c.h"
    #include "exec_fpu.c.h"
    #undef vminst
  };
  #endif

  interp_start:

  /* Make sure we're still reading within the bytecode. */
  if (pc >= codelen)
    stop(E_ILL);

  /* For benchmarking. */
  #ifdef BENCHMARK_
  icnt++;
  #endif

  /* Fetch the next instruction. */
  uint64_t i = fetch();

  #ifdef USE_THREADED_DISPATCH

    if (op(i) < opcnt)
      goto *disptab[op(i)];
    stop(E_ILL);

  #else /* if !defined(USE_THREADED_DISPATCH) */

    /* Trick to the branch predictor. */
    if (op(i) == 0)
      inext();

    switch ((opcode)op(i)) {
      #define vminst(n, c...) case (OP_##n): goto D_##n ;
      #include "exec_defs.c.h"
      #include "exec_fpu.c.h"
      #undef vminst
      default: stop(E_ILL);
    }

  #endif /* defined(USE_THREADED_DISPATCH) */

  /* Macros for opcode implementation. */
  #define vminst(n, c...) D_##n : c
  #include "exec_defs.c.h"
  #include "exec_fpu.c.h"
  #undef vminst

  /* This is unreachable. */
  stop(E_ERR);
}
