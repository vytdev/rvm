#ifndef RVMBITS_H_
#define RVMBITS_H_
#include <stdint.h>
#include "config.h"

/* ABI Version */
#define RVM_VER 1


/* (rvmhdr.ident) The magic number. */
#define RHM_NI    4
#define RHM_0     0  /* ident[0] */
#define RHM_1     1  /* ident[1] */
#define RHM_2     2  /* ident[2] */
#define RHM_3     3  /* ident[3] */
#define RMAG0    0x7f
#define RMAG1    'R'   /* 0x52 */
#define RMAG2    'V'   /* 0x56 */
#define RMAG3    'M'   /* 0x4d */
#define RMAGIC "\x7fRVM"

/* (rvmhdr.type) Bytecode types */
#define RHT_LOADABLE  0  /* Can be loaded and ran. */
#define RHT_RAWOBJ    1  /* Not runnable. */

/* (rvmhdr.flags) Program flags */
#define RHF_DEPENDENT  (1<<0)  /* Has dynamic dependencies. */
#define RHF_EXPORTS    (1<<1)  /* Does export global symbols. */

/* (rvm_symb.type) Legal values for symbol types. */
#define RST_NOTYPE    0  /* No type. This is not allocated in any sections. */
#define RST_CODE      1  /* Allocated in the code section. */
#define RST_DATA      2  /* Allocated in the data section. */

/* (rvm_symb.bind) Legal values for symbol bindings. */
#define RSB_GLOBAL    0  /* Global symbol. Redifinitions are not allowed. */
#define RSB_INTERNAL  1  /* Just like global, but the symbol is not exported. */
#define RSB_LOCAL     2  /* This shadows the global symbols. Discarded after link. */

/* (rvm_reloc.nsrc) Legal values for the relocation source segment. */
#define RRS_CODE      0  /* Code segment. */
#define RRS_DATA      1  /* Data segment. */

/* (rvm_reloc.stype) Legal values for the relocation substitution types. */
#define RRT_ABS64     0  /* Substitute absolute 64-bit. */
#define RRT_IMM40     1  /* Substitute 40-bit absolute immediate. */
#define RRT_IMM40PC   2  /* Substitute 40-bit pc-relative immediate. */

typedef struct rvmhdr {
  uint8_t  ident[RHM_NI];
  uint16_t abi_ver;
  uint8_t  type;
  uint8_t  flags;
  uint64_t entryp;
  uint64_t shoff;   /* Offset to the section header table. */
  uint64_t shnum;   /* Number of entry sections. */
  uint64_t stroff;  /* Offset to the string table. */
} rvmhdr;

typedef struct rvm_shdr {
  uint32_t name;     /* strndx */
  uint64_t offset;   /* Offset to the section payload. */
  uint64_t size;     /* Size of the section in file. */
  uint64_t entcnt;   /* Number of array entries (optional). */
} rvm_shdr;

typedef struct rvm_symb {
  uint32_t name;     /* strndx */
  uint8_t  type;     /* Type of the symbol. */
  uint8_t  bind;     /* Binding of the symbol. */
  uint64_t value;    /* Value of the symbol. */
} rvm_symb;

typedef struct rvm_reloc {
  uint64_t offset;   /* Offset to the source segment. */
  uint64_t addend;   /* Add to the resolved symbol value. */
  uint32_t name;     /* strndx */
  uint8_t  nsrc;     /* The source segment. */
  uint8_t  stype;    /* Substitution type. */
} rvm_reloc;


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
OP_HLT,            /* [     ]   terminates the vm */
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
OP_ADDI,           /* [AB  M]   A = B + M */
OP_ADDK,           /* [AB  M]   A = B + K */
OP_SUB,            /* [ABC  ]   A = B - C */
OP_SUBI,           /* [AB  M]   A = B - M */
OP_SUBIR,          /* [AB  M]   A = M - B */
OP_SUBK,           /* [AB  M]   A = B - K */
OP_SUBKR,          /* [AB  M]   A = K - B */
OP_MUL,            /* [ABC  ]   A = B * C */
OP_MULI,           /* [AB  M]   A = B * M */
OP_MULK,           /* [AB  M]   A = B * K */
OP_IMUL,           /* [ABC  ]   A = iB * iC */
OP_IMULI,          /* [AB  M]   A = iB * iM */
OP_IMULK,          /* [AB  M]   A = iB * iK */
OP_DIV,            /* [ABC  ]   A = B / C */
OP_DIVI,           /* [AB  M]   A = B / M */
OP_DIVIR,          /* [AB  M]   A = M / B */
OP_DIVK,           /* [AB  M]   A = B / K */
OP_DIVKR,          /* [AB  M]   A = K / B */
OP_IDIV,           /* [ABC  ]   A = iB / iC */
OP_IDIVI,          /* [AB  M]   A = iB / iM */
OP_IDIVIR,         /* [AB  M]   A = iM / iB */
OP_IDIVK,          /* [AB  M]   A = iB / iK */
OP_IDIVKR,         /* [AB  M]   A = iK / iB */
OP_MOD,            /* [ABC  ]   A = B mod C */
OP_MODI,           /* [AB  M]   A = B mod M */
OP_MODIR,          /* [AB  M]   A = M mod B */
OP_MODK,           /* [AB  M]   A = B mod K */
OP_MODKR,          /* [AB  M]   A = K mod B */
OP_IMOD,           /* [ABC  ]   A = iB mod iC */
OP_IMODI,          /* [AB  M]   A = iB mod iM */
OP_IMODIR,         /* [AB  M]   A = iM mod iB */
OP_IMODK,          /* [AB  M]   A = iB mod iK */
OP_IMODKR,         /* [AB  M]   A = iK mod iB */
OP_INC,            /* [A    ]   A++ */
OP_DEC,            /* [A    ]   A-- */
OP_NEG,            /* [AB   ]   A = -B */
/* Bitwise ops */
OP_AND,            /* [ABC  ]   A = B & C */
OP_ANDI,           /* [AB  M]   A = B & M */
OP_ANDK,           /* [AB  M]   A = B & K */
OP_IOR,            /* [ABC  ]   A = B | C */
OP_IORI,           /* [AB  M]   A = B | M */
OP_IORK,           /* [AB  M]   A = B | K */
OP_XOR,            /* [ABC  ]   A = B ^ C */
OP_XORI,           /* [AB  M]   A = B ^ M */
OP_XORK,           /* [AB  M]   A = B ^ K */
OP_NOT,            /* [AB   ]   A = ~B */
OP_SHL,            /* [ABC  ]   A = B << C */
OP_SHLI,           /* [AB  M]   A = B << M */
OP_SHLIR,          /* [AB  M]   A = M << B */
OP_SHR,            /* [ABC  ]   A = B >> C */
OP_SHRI,           /* [AB  M]   A = B >> M */
OP_SHRIR,          /* [AB  M]   A = M >> B */
OP_ROL,            /* [ABC  ]   A = B rol C */
OP_ROLI,           /* [AB  M]   A = B rol M */
OP_ROLIR,          /* [AB  M]   A = M rol B */
OP_ROR,            /* [ABC  ]   A = B ror C */
OP_RORI,           /* [AB  M]   A = B ror M */
OP_RORIR,          /* [AB  M]   A = M ror B */
/* Bit test instructions */
OP_BT,             /* [A   M]   test bit M of reg A */
OP_BTG,            /* [AB   ]   test bit B of reg A */
OP_BTS,            /* [A   M]   test bit M of reg A then set it */
OP_BTSG,           /* [AB   ]   test bit B of reg A then set it */
OP_BTR,            /* [A   M]   test bit M of reg A then reset it */
OP_BTRG,           /* [AB   ]   test bit B of reg A then reset it */
OP_BTC,            /* [A   M]   test bit M of reg A then complement it */
OP_BTCG,           /* [AB   ]   test bit B of reg A then complement it */
/* Flags and conditionals */
OP_CMP,            /* [AB   ]   (*) A - B */
OP_CMPI,           /* [A   M]   (*) A - M */
OP_CMPIR,          /* [A   M]   (*) M - A */
OP_CMPK,           /* [A   M]   (*) A - K */
OP_CMPKR,          /* [A   M]   (*) K - A */
OP_TEST,           /* [AB   ]   (*) A & B */
OP_TESTI,          /* [A   M]   (*) A & M */
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
OP_CMC,            /* [     ]   complement the carry flag */
OP_CMO,            /* [     ]   complement the overflow flag */
OP_CMS,            /* [     ]   complement the sign flag */
OP_CMZ,            /* [     ]   complement the zero flag */
OP_CME,            /* [     ]   complement the error flag */
OP_CMG,            /* [     ]   complement the greater flag */
OP_CML,            /* [     ]   complement the lesser flag */
OP_CMA,            /* [     ]   complement the above flag */
OP_CMB,            /* [     ]   complement the below flag */
OP_CMQ,            /* [     ]   complement the equal flag */
/* Branching and flow control */
OP_JMP,            /* [    M]   unconditional (abs) jump to pc M */
OP_JMPN,           /* [    M]   unconditional (rel) jump by offset M */
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
opcnt,    /* internal: The number of opcodes. */
} opcode;

/* VM calls (vmcalls use R0-R9 as their argument) */
enum {
VM_EXIT = 0,       /* (1) exit */
};

#endif // RVMBITS_H_
