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
 * iM       | i(12) |                M(52)            |
 * iAM      | i(12) | A(4) |            M(48)         |
 * iABM     | i(12) | A(4) | B(4) |        M(44)      |
 * iABCM    | i(12) | A(4) | B(4) | C(4) |    M(40)   |
 *
 * - i     = opcode (12-bits)
 * - A/B/C = register operand (4-bits)
 * - M     = immediate (varies)
 */

/* Instruction decoding */
#define op(n) ((n) & 0xfff)
#define rA(n) (((n) >> 12) & 0xf)
#define rB(n) (((n) >> 16) & 0xf)
#define rC(n) (((n) >> 20) & 0xf)
#define m0(n) ((n) >> 12)
#define m1(n) ((n) >> 16)
#define m2(n) ((n) >> 20)
#define m3(n) ((n) >> 24)

/* Sign extension functions for the immediates, if needed */
#define sgx0(n) ((n) | (((n) >> 51) ? (U64C(0xfff   ) << 52) : 0)) /* 52-bit imm */
#define sgx1(n) ((n) | (((n) >> 47) ? (U64C(0xffff  ) << 48) : 0)) /* 48-bit imm */
#define sgx2(n) ((n) | (((n) >> 43) ? (U64C(0xfffff ) << 44) : 0)) /* 44-bit imm */
#define sgx3(n) ((n) | (((n) >> 39) ? (U64C(0xffffff) << 40) : 0)) /* 40-bit imm */

/* Instruction encoding */
#define iM(o,m) (((o) & 0xfff) | ((m)<<12))
#define iAM(o,a,m) iM(o, ((a)&0xf) | ((m)<<4))
#define iABM(o,a,b,m) iAM(o,a, ((b)0xf) | ((m)<<4))
#define iABCM(o,a,b,c,m) iABM(o,a,b, ((c)&0xf) | ((m)<<4))

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
#define FG  (1<<5) /* greater */
#define FL  (1<<6) /* less */
#define FQ  (1<<7) /* equal */

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
enum {
OP_NOP = 0,        /* no-op */
OP_IVC,            /* [iM]    invoke vm call */
/* Data manipulation */
OP_MOV,            /* [iABM]  copy reg B to reg A */
OP_MOVI,           /* [iAM]   copy M to reg A */
OP_MOVK,           /* [iAM]   load a pc-rel-const into reg A */
OP_LOD,            /* [iAM]   load from data pool (data[M]) into reg A */
OP_LODS,           /* [iAM]   load from stack (%bp + M) into reg A */
OP_LODA,           /* [iAM]   load from arg stack */
OP_STR,            /* [iAM]   store reg A into data pool (data[M]) */
OP_STRS,           /* [iAM]   store reg A into stack (%bp + M) */
OP_STRA,           /* [iAM]   store reg A into arg stack */
OP_SWP,            /* [iABM]  swap reg A and reg B */
OP_PUSH,           /* [iAM]   push reg A onto stack */
OP_PUSHI,          /* [iM]    push M onto stack */
OP_PUSHK,          /* [iM]    push a pc-rel-const onto stack */
OP_POP,            /* [iAM]   pop the stack and store the top-item into reg A */
/* Integer arithmetic */
OP_ADD,            /* [iABCM] A = B + C */
OP_ADDK,           /* [iABM]  A = B + K */
OP_SUB,            /* [iABCM] A = B - C */
OP_SUBK,           /* [iABM]  A = B - K */
OP_SUBKR,          /* [iABM]  A = K - B */
OP_MUL,            /* [iABCM] A = B * C */
OP_MULK,           /* [iABM]  A = B * K */
OP_IMUL,           /* [iABCM] A = iB * iC */
OP_IMULK,          /* [iABM]  A = iB * iK */
OP_DIV,            /* [iABCM] A = B / C */
OP_DIVK,           /* [iABM]  A = B / K */
OP_DIVKR,          /* [iABM]  A = K / B */
OP_IDIV,           /* [iABCM] A = iB / iC */
OP_IDIVK,          /* [iABM]  A = iB / iK */
OP_IDIVKR,         /* [iABM]  A = iK / iB */
OP_MOD,            /* [iABCM] A = B mod C */
OP_MODK,           /* [iABM]  A = B mod K */
OP_MODKR,          /* [iABM]  A = K mod B */
OP_IMOD,           /* [iABCM] A = iB mod iC */
OP_IMODK,          /* [iABM]  A = iB mod iK */
OP_IMODKR,         /* [iABM]  A = iK mod iB */
OP_INC,            /* [iAM]   A++ */
OP_DEC,            /* [iAM]   A-- */
OP_NEG,            /* [iABM]  A = -B */
/* Bitwise ops */
OP_AND,            /* [iABCM] A = B & C */
OP_ANDK,           /* [iABM]  A = B & K */
OP_IOR,            /* [iABCM] A = B | C */
OP_IORK,           /* [iABM]  A = B | K */
OP_XOR,            /* [iABCM] A = B ^ C */
OP_XORK,           /* [iABM]  A = B ^ K */
OP_NOT,            /* [iAM]   A = ~B */
OP_SHL,            /* [iABCM] A = B << C */
OP_SHLK,           /* [iABM]  A = B << K */
OP_SHLKR,          /* [iABM]  A = K << B */
OP_SHR,            /* [iABCM] A = B >> C */
OP_SHRK,           /* [iABM]  A = B >> K */
OP_SHRKR,          /* [iABM]  A = K >> B */
OP_ROL,            /* [iABCM] A = B rol C */
OP_ROLK,           /* [iABM]  A = B rol K */
OP_ROLKR,          /* [iABM]  A = K rol B */
OP_ROR,            /* [iABCM] A = B ror C */
OP_RORK,           /* [iABM]  A = B ror K */
OP_RORKR,          /* [iABM]  A = K ror B */
/* Flags and conditionals */
OP_CMP,            /* [iABM]  (*) A - B */
OP_CMPK,           /* [iAM]   (*) A - K */
OP_CMPKR,          /* [iAM]   (*) K - A */
OP_TEST,           /* [iABM]  (*) A & B */
OP_TESTK,          /* [iAM]   (*) A & K */
OP_STC,            /* [iM]    set the carry flag */
OP_STO,            /* [iM]    set the overflow flag */
OP_STS,            /* [iM]    set the sign flag */
OP_STZ,            /* [iM]    set the zero flag */
OP_STE,            /* [iM]    set the error flag */
OP_STG,            /* [iM]    set the greater flag */
OP_STL,            /* [iM]    set the lesser flag */
OP_STQ,            /* [iM]    set the equal flag */
OP_CLC,            /* [iM]    clear the carry flag */
OP_CLO,            /* [iM]    clear the overflow flag */
OP_CLS,            /* [iM]    clear the sign flag */
OP_CLZ,            /* [iM]    clear the zero flag */
OP_CLE,            /* [iM]    clear the error flag */
OP_CLG,            /* [iM]    clear the greater flag */
OP_CLL,            /* [iM]    clear the lesser flag */
OP_CLQ,            /* [iM]    clear the equal flag */
/* Branching and flow control */
OP_JMP,            /* [iM]    unconditional (abs) jump to pc M */
OP_JE,             /* [iM]    (rel) jump if FQ */
OP_JNE,            /* [iM]    (rel) jump if not FQ */
OP_JG,             /* [iM]    (rel) jump if FG */
OP_JGE,            /* [iM]    (rel) jump if FG or FQ */
OP_JL,             /* [iM]    (rel) jump if FL */
OP_JLE,            /* [iM]    (rel) jump if FL or FQ */
OP_JC,             /* [iM]    (rel) jump if FC */
OP_JNC,            /* [iM]    (rel) jump if not FC */
OP_JO,             /* [iM]    (rel) jump if FO */
OP_JNO,            /* [iM]    (rel) jump if not FO */
OP_JS,             /* [iM]    (rel) jump if FS */
OP_JNS,            /* [iM]    (rel) jump if not FS */
OP_JZ,             /* [iM]    (rel) jump if FZ */
OP_JNZ,            /* [iM]    (rel) jump if not FZ */
OP_JX,             /* [iM]    (rel) jump if FE */
OP_JNX,            /* [iM]    (rel) jump if not FE */
OP_CALL,           /* [iM]    call a subroutine */
OP_CALLR,          /* [iAM]   setup a new frame and set reg A to %pc */
OP_RET,            /* [iM]    return to the caller */
OP_THR,            /* [iM]    set FE, then return to the caller */
};

/* VM calls (vmcalls use R0-R9 as their argument) */
enum {
VM_EXIT = 0,       /* (1) exit */
};

#endif // RVMBITS_H_
