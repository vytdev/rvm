/*
 *  rvm -- A virtual machine.
 *  Copyright (C) 2024-2025  Vincent Yanzee J. Tan
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef RVM_DEFS_H_
#define RVM_DEFS_H_  1

/* General purpose registers. */
#define RVM_R0   0
#define RVM_R1   1
#define RVM_R2   2
#define RVM_R3   3
#define RVM_R4   4
#define RVM_R5   5
#define RVM_R6   6
#define RVM_R7   7
#define RVM_R8   8
#define RVM_R9   9
#define RVM_R10  10
#define RVM_R11  11
#define RVM_R12  12
#define RVM_R13  13
#define RVM_R14  14
#define RVM_R15  15
#define RVM_RSP  RVM_R15

#define RVM_REGCNT   16      /* rvm register count. Must match RVM_RGNUM. */

/* Condition flags. */
#define RVM_FEQ  (1<<0)  /* Equal flag. */
#define RVM_FAB  (1<<1)  /* Above (unsigned >) flag. */
#define RVM_FGT  (1<<2)  /* Greater (signed >) flag. */


/* rvm instruction format constants. */
#define RVM_INWD     32   /* Width of an rvm instruction. */
#define RVM_INLN     (RVM_INWD / 8) /* Byte length of an rvm inst. */

#define RVM_OPWD     9    /* Width of the opcode. */
#define RVM_OPNUM    (1<<RVM_OPWD)  /* No. of possible opcodes. */
#define RVM_OPMASK   (RVM_OPNUM-1)  /* To extract the opcode. */
#define RVM_OPPOS    0              /* Bit pos of opcode. */

#define RVM_RGWD     4    /* Width of an rvm register. */
#define RVM_RGNUM    (1<<RVM_RGWD)         /* No. of registers. */
#define RVM_RGMASK   (RVM_RGNUM-1)         /* To extract the reg idx. */
#define RVM_RGAPOS   (RVM_INWD  -RVM_RGWD) /* Register A bit pos. */
#define RVM_RGBPOS   (RVM_RGAPOS-RVM_RGWD) /* Register B bit pos. */
#define RVM_RGCPOS   (RVM_RGBPOS-RVM_RGWD) /* Register C bit pos. */

#define RVM_FNPOS    RVM_OPWD   /* Start of func bits. */
#define RVM_F11MASK  ((1<<11)-1)  /* Mask for func11. */
#define RVM_F15MASK  ((1<<15)-1)  /* Mask for func15. */
#define RVM_F19MASK  ((1<<19)-1)  /* Mask for func19. */
#define RVM_F23MASK  ((1<<23)-1)  /* Mask for func23. */

/* rvm instruction decoding helpers. */
#define RVM_OPC(i) ((i) & RVM_OPMASK)                 /* Extract opcode. */
#define RVM_RGA(i) (((i) >> RVM_RGAPOS) & RVM_RGMASK) /* Extract regA. */
#define RVM_RGB(i) (((i) >> RVM_RGBPOS) & RVM_RGMASK) /* Extract regB. */
#define RVM_RGC(i) (((i) >> RVM_RGCPOS) & RVM_RGMASK) /* Extract regC. */
#define RVM_FNC(i) ((i) >> RVM_FNPOS)                 /* Extract func bits. */

/* rvm instruction encoding helpers. */
#define RVM_ETYPJ(op, f23)        \
  ((op) | ((f23) << RVM_FNPOS))     /* Encodes J-type instructions. */
#define RVM_ETYPM(op, f19, rgA)   \
  ((op) | ((f19) << RVM_FNPOS)    \
        | ((rgA) << RVM_RGAPOS))    /* Encodes M-type instructions. */
#define RVM_ETYPI(op, f15, rgA, rgB) \
  ((op) | ((f15) << RVM_FNPOS)    \
        | ((rgA) << RVM_RGAPOS)   \
        | ((rgB) << RVM_RGBPOS))    /* Encodes I-type instructions. */
#define RVM_ETYPR(op, f11, rgA, rgB, rgC) \
  ((op) | ((f11) << RVM_FNPOS)    \
        | ((rgA) << RVM_RGAPOS)   \
        | ((rgB) << RVM_RGBPOS)   \
        | ((rgC) << RVM_RGCPOS))    /* Encodes R-type instructions. */

/* rvm status codes. */
#define RVM_EOK          0    /* Ok */
#define RVM_ERR          1    /* Error */

#endif /* defs.h */
