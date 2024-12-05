#ifndef RVMBITS_H_
#define RVMBITS_H_
#include <stdint.h>

typedef struct rvmhdr {
  uint8_t  magic[4];
  uint16_t abi_ver;
  uint8_t  type;
  uint64_t entry;
} rvmhdr;

/* ABI Version */
#define RVM_VER 1

/* Program types */
#define RTYP_EXEC 0 /* executable image */
#define RTYP_DLIB 1 /* dynamic library */
#define RTYP_RAWO 2 /* raw object */

/* Instruction decoding */
#define op(n) ((n) & 0xfff)
#define rA(n) (((n) >> 12) & 0xf)
#define rB(n) (((n) >> 16) & 0xf)
#define rC(n) (((n) >> 20) & 0xf)
#define m0(n) ((n) >> 12)
#define m1(n) ((n) >> 16)
#define m2(n) ((n) >> 20)
#define m3(n) ((n) >> 24)

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

/* Instruction set */
enum {
OP_NOP = 0,        /* no-op */
OP_IVC,            /* [iM]    invoke vm call */
OP_MOV,            /* [iABM]  copy reg B to reg A */
OP_MOVI,           /* [iAM]   copy M to reg A */
};

/* VM calls (vmcalls use R0-R9 as their argument) */
enum {
VM_EXIT = 0,       /* (1) exit */
};

#endif // RVMBITS_H_
