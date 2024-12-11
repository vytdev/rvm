#ifndef RVMBITS_H_
#define RVMBITS_H_
#include <stdint.h>
#include "config.h"

typedef struct rvmhdr {
  uint8_t  magic[4];
  uint16_t abi_ver;
  uint8_t  type;
  uint64_t entry;
  uint64_t datoff;
  uint64_t datlen;
} rvmhdr;

/* ABI Version */
#define RVM_VER 1

/* Program types */
#define RTYP_EXEC 0 /* executable image */
#define RTYP_DLIB 1 /* dynamic library */
#define RTYP_RAWO 2 /* raw object */

/*
 * Instruction encoding:
 *
 * - opcode (12b)
 * - reg A  ( 4b)
 * - reg B  ( 4b)
 * - reg C  ( 4b)
 * - imm    (40b)
 */

/* Instruction decoding */
#define op(n) ((n) & 0xfff)
#define rA(n) (((n) >> 12) & 0xf)
#define rB(n) (((n) >> 16) & 0xf)
#define rC(n) (((n) >> 20) & 0xf)
#define im(n) (((n) >> 24) | (((n) >> 63) ? (U64C(0xffffff) << 40) : 0))

#define ienc(o,a,b,c,m) \
  (((o) & 0xfff)      | \
  (((a) & 0xf) << 12) | \
  (((b) & 0xf) << 16) | \
  (((c) & 0xf) << 20) | \
  ((m) << 24))

/* Registers */
#define R0  0
#define R1  1
#define R2  2
#define R3  3
#define R4  4
#define R5  5
#define R6  6
#define R7  7
#define R8  8
#define R9  9
#define RRV 10 /* return value */
#define RLR 11 /* link register */
#define RBP 12 /* base pointer */
#define RSP 13 /* stack pointer */
#define RPC 14 /* program counter */
#define RFL 15 /* flags register */

/* Flags */
#define FC  (1<<0) /* carry flag */
#define FO  (1<<1) /* overflow flag */
#define FS  (1<<2) /* sign flag */
#define FZ  (1<<3) /* zero flag */
#define FE  (1<<4) /* error flag */
#define FG  (1<<5) /* (s) greater */
#define FL  (1<<6) /* (s) less */
#define FA  (1<<7) /* (u) above */
#define FB  (1<<8) /* (u) below */
#define FQ  (1<<9) /* equal */

/*
 * The calling convention:
 *
 * <stack>:
 *   ##      <- caller's base pointer
 *   ...
 *   ##      <- arg2
 *   ##      <- arg1
 *   ##      <- saved caller's %lr (internal)
 *   ##      <- saved caller's %bp (internal)
 *   ##      <- callee's base pointer
 *   ...
 *
 * NOTES:
 *  + Arguments must be stacked in reverse order.
 *  + You can only access the args up to the caller's %bp.
 *  + Local variables in the stack is relative to %bp, and the index
 *    cannot exceed (%sp - %bp).
 *  + The stack frame is automatically setup internally each call
 *    to a subroutine.
 *  + arg(d) = %bp - 3 - d
 *  + var(d) = %bp + d
 *
 * Allocating local vars:
 *      add %sp, %sp, #n
 *      ; n is the var count
 *
 * Deallocating local vars:
 *      sub %sp, %sp, #n
 *      ; n is the var count
 */

/* Instruction set */
typedef enum {
OP_NOP = 0,        /* no-op */
OP_IVC,            /* [    M]   invoke vm call */
OP_HLT,            /* [     ]   halt the vm's execution and enter idle */
/* Data manipulation */
OP_MOV,            /* [AB   ]   copy reg B to reg A */
OP_MOVI,           /* [A   M]   copy M to reg A */
OP_MOVK,           /* [A   M]   load a pc-rel-const into reg A */
OP_LOD,            /* [A   M]   load from data pool (data[M]) into reg A */
OP_LODS,           /* [A   M]   load from stack (%bp + M) into reg A */
OP_LODA,           /* [A   M]   load from arg stack */
OP_LODAR,          /* [AB   ]   load from arg stack using reg B as index */
OP_STR,            /* [A   M]   store reg A into data pool (data[M]) */
OP_STRS,           /* [A   M]   store reg A into stack (%bp + M) */
OP_STRA,           /* [A   M]   store reg A into arg stack */
OP_STRAR,          /* [AB   ]   store to arg stack using reg B as index */
OP_SWP,            /* [AB   ]   swap reg A and reg B */
OP_PUSH,           /* [A    ]   push reg A onto stack */
OP_PUSHI,          /* [    M]   push M onto stack */
OP_PUSHK,          /* [    M]   push a pc-rel-const onto stack */
OP_POP,            /* [A    ]   pop the stack and store the top-item into reg A */
/* Integer arithmetic */
OP_ADD,            /* [ABC  ]   A = B + C */
OP_ADDK,           /* [AB  M]   A = B + K */
OP_SUB,            /* [ABC  ]   A = B - C */
OP_SUBK,           /* [AB  M]   A = B - K */
OP_SUBKR,          /* [AB  M]   A = K - B */
OP_MUL,            /* [ABC  ]   A = B * C */
OP_MULK,           /* [AB  M]   A = B * K */
OP_IMUL,           /* [ABC  ]   A = iB * iC */
OP_IMULK,          /* [AB  M]   A = iB * iK */
OP_DIV,            /* [ABC  ]   A = B / C */
OP_DIVK,           /* [AB  M]   A = B / K */
OP_DIVKR,          /* [AB  M]   A = K / B */
OP_IDIV,           /* [ABC  ]   A = iB / iC */
OP_IDIVK,          /* [AB  M]   A = iB / iK */
OP_IDIVKR,         /* [AB  M]   A = iK / iB */
OP_MOD,            /* [ABC  ]   A = B mod C */
OP_MODK,           /* [AB  M]   A = B mod K */
OP_MODKR,          /* [AB  M]   A = K mod B */
OP_IMOD,           /* [ABC  ]   A = iB mod iC */
OP_IMODK,          /* [AB  M]   A = iB mod iK */
OP_IMODKR,         /* [AB  M]   A = iK mod iB */
OP_INC,            /* [A    ]   A++ */
OP_DEC,            /* [A    ]   A-- */
OP_NEG,            /* [AB   ]   A = -B */
/* Bitwise ops */
OP_AND,            /* [ABC  ]   A = B & C */
OP_ANDK,           /* [AB  M]   A = B & K */
OP_IOR,            /* [ABC  ]   A = B | C */
OP_IORK,           /* [AB  M]   A = B | K */
OP_XOR,            /* [ABC  ]   A = B ^ C */
OP_XORK,           /* [AB  M]   A = B ^ K */
OP_NOT,            /* [AB   ]   A = ~B */
OP_SHL,            /* [ABC  ]   A = B << C */
OP_SHLK,           /* [AB  M]   A = B << K */
OP_SHLKR,          /* [AB  M]   A = K << B */
OP_SHR,            /* [ABC  ]   A = B >> C */
OP_SHRK,           /* [AB  M]   A = B >> K */
OP_SHRKR,          /* [AB  M]   A = K >> B */
OP_ROL,            /* [ABC  ]   A = B rol C */
OP_ROLK,           /* [AB  M]   A = B rol K */
OP_ROLKR,          /* [AB  M]   A = K rol B */
OP_ROR,            /* [ABC  ]   A = B ror C */
OP_RORK,           /* [AB  M]   A = B ror K */
OP_RORKR,          /* [AB  M]   A = K ror B */
/* Flags and conditionals */
OP_CMP,            /* [AB   ]   (*) A - B */
OP_CMPK,           /* [A   M]   (*) A - K */
OP_CMPKR,          /* [A   M]   (*) K - A */
OP_TEST,           /* [AB   ]   (*) A & B */
OP_TESTK,          /* [A   M]   (*) A & K */
OP_STC,            /* [     ]   set the carry flag */
OP_STO,            /* [     ]   set the overflow flag */
OP_STS,            /* [     ]   set the sign flag */
OP_STZ,            /* [     ]   set the zero flag */
OP_STE,            /* [     ]   set the error flag */
OP_STG,            /* [     ]   set the greater flag */
OP_STL,            /* [     ]   set the lesser flag */
OP_STA,            /* [     ]   set the above flag */
OP_STB,            /* [     ]   set the below flag */
OP_STQ,            /* [     ]   set the equal flag */
OP_CLC,            /* [     ]   clear the carry flag */
OP_CLO,            /* [     ]   clear the overflow flag */
OP_CLS,            /* [     ]   clear the sign flag */
OP_CLZ,            /* [     ]   clear the zero flag */
OP_CLE,            /* [     ]   clear the error flag */
OP_CLG,            /* [     ]   clear the greater flag */
OP_CLL,            /* [     ]   clear the lesser flag */
OP_CLA,            /* [     ]   clear the above flag */
OP_CLB,            /* [     ]   clear the below flag */
OP_CLQ,            /* [     ]   clear the equal flag */
/* Branching and flow control */
OP_JMP,            /* [    M]   unconditional (abs) jump to pc M */
OP_JE,             /* [    M]   (rel) jump if FQ */
OP_JNE,            /* [    M]   (rel) jump if not FQ */
OP_JG,             /* [    M]   (rel) jump if FG */
OP_JGE,            /* [    M]   (rel) jump if FG or FQ */
OP_JL,             /* [    M]   (rel) jump if FL */
OP_JLE,            /* [    M]   (rel) jump if FL or FQ */
OP_JA,             /* [    M]   (rel) jump if FA */
OP_JAE,            /* [    M]   (rel) jump if FA or FQ */
OP_JB,             /* [    M]   (rel) jump if FB */
OP_JBE,            /* [    M]   (rel) jump if FB or FQ */
OP_JC,             /* [    M]   (rel) jump if FC */
OP_JNC,            /* [    M]   (rel) jump if not FC */
OP_JO,             /* [    M]   (rel) jump if FO */
OP_JNO,            /* [    M]   (rel) jump if not FO */
OP_JS,             /* [    M]   (rel) jump if FS */
OP_JNS,            /* [    M]   (rel) jump if not FS */
OP_JZ,             /* [    M]   (rel) jump if FZ */
OP_JNZ,            /* [    M]   (rel) jump if not FZ */
OP_JX,             /* [    M]   (rel) jump if FE */
OP_JNX,            /* [    M]   (rel) jump if not FE */
OP_LOOP,           /* [A   M]   decrement reg A and (rel) jump if non-zero */
OP_CALL,           /* [    M]   call a subroutine */
OP_CALLR,          /* [A    ]   setup a new frame and set reg A to %pc */
OP_RET,            /* [     ]   return to the caller */
OP_THR,            /* [     ]   set FE, then return to the caller */
} opcode;

/* VM calls (vmcalls use R0-R9 as their argument) */
enum {
VM_EXIT = 0,       /* (1) exit */
};

#endif // RVMBITS_H_
