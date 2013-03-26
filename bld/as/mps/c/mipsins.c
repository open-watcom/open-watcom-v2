/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  MIPS instruction opcodes.
*
****************************************************************************/


#include "as.h"

static bool insErrFlag = FALSE;    // to tell whether we had problems or not

#define INS( a, b, c, d, e, f ) { a, b, c, d, e, NULL, f }

ins_table MIPSTable[] = {
 /* INS( name,      opcode, fncode, template,           method,     ISA level ), */
 // Memory Format Instructions
    INS( "ldl",     0x1a,   0x00,   IT_MEMORY_ALL,      ENUM_NONE,  MIPS_ISA3 ),
    INS( "ldr",     0x1b,   0x00,   IT_MEMORY_ALL,      ENUM_NONE,  MIPS_ISA3 ),
    INS( "lb",      0x20,   0x00,   IT_MEMORY_ALL,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "lh",      0x21,   0x00,   IT_MEMORY_ALL,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "lwl",     0x22,   0x00,   IT_MEMORY_ALL,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "lw",      0x23,   0x00,   IT_MEMORY_ALL,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "lbu",     0x24,   0x00,   IT_MEMORY_ALL,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "lhu",     0x25,   0x00,   IT_MEMORY_ALL,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "lwr",     0x26,   0x00,   IT_MEMORY_ALL,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "sb",      0x28,   0x00,   IT_MEMORY_ALL,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "sh",      0x29,   0x00,   IT_MEMORY_ALL,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "swl",     0x2a,   0x00,   IT_MEMORY_ALL,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "sw",      0x2b,   0x00,   IT_MEMORY_ALL,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "sdl",     0x2c,   0x00,   IT_MEMORY_ALL,      ENUM_NONE,  MIPS_ISA3 ),
    INS( "sdr",     0x2d,   0x00,   IT_MEMORY_ALL,      ENUM_NONE,  MIPS_ISA3 ),
    INS( "swr",     0x2e,   0x00,   IT_MEMORY_ALL,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "ll",      0x30,   0x00,   IT_MEMORY_ALL,      ENUM_NONE,  MIPS_ISA2 ),
    INS( "lld",     0x34,   0x00,   IT_MEMORY_ALL,      ENUM_NONE,  MIPS_ISA3 ),
    INS( "ld",      0x37,   0x00,   IT_MEMORY_ALL,      ENUM_NONE,  MIPS_ISA3 ),
    INS( "sc",      0x38,   0x00,   IT_MEMORY_ALL,      ENUM_NONE,  MIPS_ISA2 ),
    INS( "scd",     0x3c,   0x00,   IT_MEMORY_ALL,      ENUM_NONE,  MIPS_ISA3 ),
    INS( "sd",      0x3f,   0x00,   IT_MEMORY_ALL,      ENUM_NONE,  MIPS_ISA3 ),
  // Unaligned memory format pseudo-instructions
    INS( "ulh",     0x00,   0x02,   IT_MEMORY_ALL,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "ulhu",    0x00,   0x03,   IT_MEMORY_ALL,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "ulw",     0x00,   0x04,   IT_MEMORY_ALL,      ENUM_NONE,  MIPS_ISA1 ),
 // FPU Memory Format Instructions
    INS( "lwc1",    0x31,   0x00,   IT_FP_MEMORY_ALL,   ENUM_NONE,  MIPS_ISA1 ),
    INS( "ldc1",    0x35,   0x00,   IT_FP_MEMORY_ALL,   ENUM_NONE,  MIPS_ISA2 ),
    INS( "swc1",    0x39,   0x00,   IT_FP_MEMORY_ALL,   ENUM_NONE,  MIPS_ISA1 ),
    INS( "sdc1",    0x3d,   0x00,   IT_FP_MEMORY_ALL,   ENUM_NONE,  MIPS_ISA2 ),
 // Memory Format Instructions with a function code
    INS( "fetch",   0x18,   0x80,   IT_MEMORY_B,        ENUM_NONE,  MIPS_ISA1 ),
    INS( "fetch_m", 0x18,   0xA0,   IT_MEMORY_B,        ENUM_NONE,  MIPS_ISA1 ),
    INS( "mb",      0x18,   0x40,   IT_MEMORY_NONE,     ENUM_NONE,  MIPS_ISA1 ),
    INS( "wmb",     0x18,   0x44,   IT_MEMORY_NONE,     ENUM_NONE,  MIPS_ISA1 ),
    INS( "rc",      0x18,   0xE0,   IT_MEMORY_A,        ENUM_NONE,  MIPS_ISA1 ),
    INS( "rpcc",    0x18,   0xC0,   IT_MEMORY_A,        ENUM_NONE,  MIPS_ISA1 ),
    INS( "rs",      0x18,   0xF0,   IT_MEMORY_A,        ENUM_NONE,  MIPS_ISA1 ),
    INS( "trapb",   0x18,   0x00,   IT_MEMORY_NONE,     ENUM_NONE,  MIPS_ISA1 ),
    INS( "excb",    0x18,   0x04,   IT_MEMORY_NONE,     ENUM_NONE,  MIPS_ISA1 ),
 // Jump Instructions
    INS( "j",       0x02,   0x00,   IT_JUMP,            ENUM_NONE,  MIPS_ISA1 ),
    INS( "jal",     0x03,   0x00,   IT_JUMP,            ENUM_NONE,  MIPS_ISA1 ),
    INS( "jr",      0x00,   0x08,   IT_REG_JUMP,        ENUM_NONE,  MIPS_ISA1 ),
    INS( "jalr",    0x00,   0x09,   IT_REG_JUMP,        ENUM_NONE,  MIPS_ISA1 ),
 // Memory Branch Instructions
    INS( "ret",     0x1A,   0x02,   IT_RET,             ENUM_NONE,  MIPS_ISA1 ),
 // Branch Format Instructions
    INS( "beq",     0x04,   0x00,   IT_BRANCH_TWO,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "bne",     0x05,   0x00,   IT_BRANCH_TWO,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "blez",    0x06,   0x00,   IT_BRANCH_ZERO,     ENUM_NONE,  MIPS_ISA1 ),
    INS( "bgtz",    0x07,   0x00,   IT_BRANCH_ZERO,     ENUM_NONE,  MIPS_ISA1 ),
    INS( "bltz",    0x01,   0x00,   IT_BRANCH_ZERO,     ENUM_NONE,  MIPS_ISA1 ),
    INS( "bgez",    0x01,   0x01,   IT_BRANCH_ZERO,     ENUM_NONE,  MIPS_ISA1 ),
    INS( "bltzl",   0x01,   0x02,   IT_BRANCH_ZERO,     ENUM_NONE,  MIPS_ISA2 ),
    INS( "bgezl",   0x01,   0x03,   IT_BRANCH_ZERO,     ENUM_NONE,  MIPS_ISA2 ),
    INS( "bltzal",  0x01,   0x10,   IT_BRANCH_ZERO,     ENUM_NONE,  MIPS_ISA1 ),
    INS( "bgezal",  0x01,   0x11,   IT_BRANCH_ZERO,     ENUM_NONE,  MIPS_ISA1 ),
    INS( "bltzall", 0x01,   0x12,   IT_BRANCH_ZERO,     ENUM_NONE,  MIPS_ISA2 ),
    INS( "bgezall", 0x01,   0x13,   IT_BRANCH_ZERO,     ENUM_NONE,  MIPS_ISA2 ),
    INS( "beql",    0x14,   0x00,   IT_BRANCH_TWO,      ENUM_NONE,  MIPS_ISA2 ),
    INS( "bnel",    0x15,   0x00,   IT_BRANCH_TWO,      ENUM_NONE,  MIPS_ISA2 ),
    INS( "bgtzl",   0x17,   0x00,   IT_BRANCH_ZERO,     ENUM_NONE,  MIPS_ISA2 ),
    INS( "blezl",   0x16,   0x00,   IT_BRANCH_ZERO,     ENUM_NONE,  MIPS_ISA2 ),

    INS( "br",      0x30,   0x00,   IT_BR,              ENUM_NONE,  MIPS_ISA1 ),
    INS( "fbeq",    0x31,   0x00,   IT_FP_BRANCH,       ENUM_NONE,  MIPS_ISA1 ),
    INS( "blt",     0x3A,   0x00,   IT_BRANCH,          ENUM_NONE,  MIPS_ISA1 ),
    INS( "ble",     0x3B,   0x00,   IT_BRANCH,          ENUM_NONE,  MIPS_ISA1 ),
    INS( "bge",     0x3E,   0x00,   IT_BRANCH,          ENUM_NONE,  MIPS_ISA1 ),
    INS( "bgt",     0x3F,   0x00,   IT_BRANCH,          ENUM_NONE,  MIPS_ISA1 ),
 // Coprocessor branch instructions; high byte of opcode is coprocessor number
    INS( "bc0f",    0x008,  0x00,   IT_BRANCH_COP,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "bc0fl",   0x008,  0x02,   IT_BRANCH_COP,      ENUM_NONE,  MIPS_ISA2 ),
    INS( "bc0t",    0x008,  0x01,   IT_BRANCH_COP,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "bc0tl",   0x008,  0x03,   IT_BRANCH_COP,      ENUM_NONE,  MIPS_ISA2 ),
    INS( "bc1f",    0x108,  0x00,   IT_BRANCH_COP,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "bc1fl",   0x108,  0x02,   IT_BRANCH_COP,      ENUM_NONE,  MIPS_ISA2 ),
    INS( "bc1t",    0x108,  0x01,   IT_BRANCH_COP,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "bc1tl",   0x108,  0x03,   IT_BRANCH_COP,      ENUM_NONE,  MIPS_ISA2 ),
 // Coprocessor 0 special instructions
    INS( "tlbr",    0x01,   0x00,   IT_COP0_SPECIAL,    ENUM_NONE,  MIPS_ISA1 ),
    INS( "tlbwi",   0x02,   0x00,   IT_COP0_SPECIAL,    ENUM_NONE,  MIPS_ISA1 ),
    INS( "tlbwr",   0x06,   0x00,   IT_COP0_SPECIAL,    ENUM_NONE,  MIPS_ISA1 ),
    INS( "tlbp",    0x08,   0x00,   IT_COP0_SPECIAL,    ENUM_NONE,  MIPS_ISA1 ),
    INS( "rfe",     0x10,   0x00,   IT_COP0_SPECIAL,    ENUM_NONE,  MIPS_ISA1 ),
    INS( "eret",    0x18,   0x00,   IT_COP0_SPECIAL,    ENUM_NONE,  MIPS_ISA3 ),
 // Moves to/from Coprocessor
    INS( "mfc0",    0x10,   0x00,   IT_MOV_COP,         ENUM_NONE,  MIPS_ISA1 ),
    INS( "dmfc0",   0x10,   0x01,   IT_MOV_COP,         ENUM_NONE,  MIPS_ISA3 ),
    INS( "mtc0",    0x10,   0x04,   IT_MOV_COP,         ENUM_NONE,  MIPS_ISA1 ),
    INS( "dmtc0",   0x10,   0x05,   IT_MOV_COP,         ENUM_NONE,  MIPS_ISA3 ),
 // Moves to/from FPU
    INS( "mfc1",    0x11,   0x00,   IT_MOV_FP,          ENUM_NONE,  MIPS_ISA1 ),
    INS( "dmfc1",   0x11,   0x01,   IT_MOV_FP,          ENUM_NONE,  MIPS_ISA2 ),
    INS( "cfc1",    0x11,   0x02,   IT_MOV_COP,         ENUM_NONE,  MIPS_ISA1 ),
    INS( "mtc1",    0x11,   0x04,   IT_MOV_FP,          ENUM_NONE,  MIPS_ISA1 ),
    INS( "dmtc1",   0x11,   0x05,   IT_MOV_FP,          ENUM_NONE,  MIPS_ISA2 ),
    INS( "ctc1",    0x11,   0x06,   IT_MOV_COP,         ENUM_NONE,  MIPS_ISA1 ),
 // Trap instructions
    INS( "tge",     0x00,   0x30,   IT_TRAP,            ENUM_NONE,  MIPS_ISA2 ),
    INS( "tgeu",    0x00,   0x31,   IT_TRAP,            ENUM_NONE,  MIPS_ISA2 ),
    INS( "tlt",     0x00,   0x32,   IT_TRAP,            ENUM_NONE,  MIPS_ISA2 ),
    INS( "tltu",    0x00,   0x33,   IT_TRAP,            ENUM_NONE,  MIPS_ISA2 ),
    INS( "teq",     0x00,   0x34,   IT_TRAP,            ENUM_NONE,  MIPS_ISA2 ),
    INS( "tne",     0x00,   0x36,   IT_TRAP,            ENUM_NONE,  MIPS_ISA2 ),
    INS( "tgei",    0x01,   0x08,   IT_TRAP_IMM,        ENUM_NONE,  MIPS_ISA2 ),
    INS( "tgeiu",   0x01,   0x09,   IT_TRAP_IMM,        ENUM_NONE,  MIPS_ISA2 ),
    INS( "tlti",    0x01,   0x0a,   IT_TRAP_IMM,        ENUM_NONE,  MIPS_ISA2 ),
    INS( "tltiu",   0x01,   0x0b,   IT_TRAP_IMM,        ENUM_NONE,  MIPS_ISA2 ),
    INS( "teqi",    0x01,   0x0c,   IT_TRAP_IMM,        ENUM_NONE,  MIPS_ISA2 ),
    INS( "tnei",    0x01,   0x0e,   IT_TRAP_IMM,        ENUM_NONE,  MIPS_ISA2 ),
 // Operate Format Instructions
    INS( "add",     0x00,   0x20,   IT_OPERATE,         ENUM_OF_ADDL,  MIPS_ISA1 ),
    INS( "addu",    0x00,   0x21,   IT_OPERATE,         ENUM_OF_ADDL,  MIPS_ISA1 ),
    INS( "sub",     0x00,   0x22,   IT_OPERATE,         ENUM_OF_ADDL,  MIPS_ISA1 ),
    INS( "subu",    0x00,   0x23,   IT_OPERATE,         ENUM_OF_ADDL,  MIPS_ISA1 ),
    INS( "and",     0x00,   0x24,   IT_OPERATE,         ENUM_NONE,  MIPS_ISA1 ),
    INS( "or",      0x00,   0x25,   IT_OPERATE,         ENUM_NONE,  MIPS_ISA1 ),
    INS( "nor",     0x00,   0x27,   IT_OPERATE,         ENUM_NONE,  MIPS_ISA1 ),
    INS( "xor",     0x00,   0x26,   IT_OPERATE,         ENUM_NONE,  MIPS_ISA1 ),
    INS( "slt",     0x00,   0x2a,   IT_OPERATE,         ENUM_NONE,  MIPS_ISA1 ),
    INS( "sltu",    0x00,   0x2b,   IT_OPERATE,         ENUM_NONE,  MIPS_ISA1 ),
    INS( "dadd",    0x00,   0x2c,   IT_OPERATE,         ENUM_NONE,  MIPS_ISA3 ),
    INS( "daddu",   0x00,   0x2d,   IT_OPERATE,         ENUM_NONE,  MIPS_ISA3 ),
    INS( "dsub",    0x00,   0x2e,   IT_OPERATE,         ENUM_NONE,  MIPS_ISA3 ),
    INS( "dsubu",   0x00,   0x2f,   IT_OPERATE,         ENUM_NONE,  MIPS_ISA3 ),
    INS( "addi",    0x08,   0x00,   IT_OPERATE_IMM,     ENUM_NONE,  MIPS_ISA1 ),
    INS( "addiu",   0x09,   0x00,   IT_OPERATE_IMM,     ENUM_NONE,  MIPS_ISA1 ),
    INS( "slti",    0x0a,   0x00,   IT_OPERATE_IMM,     ENUM_NONE,  MIPS_ISA1 ),
    INS( "sltiu",   0x0b,   0x00,   IT_OPERATE_IMM,     ENUM_NONE,  MIPS_ISA1 ),
    INS( "andi",    0x0c,   0x00,   IT_OPERATE_IMM,     ENUM_NONE,  MIPS_ISA1 ),
    INS( "ori",     0x0d,   0x00,   IT_OPERATE_IMM,     ENUM_NONE,  MIPS_ISA1 ),
    INS( "xori",    0x0e,   0x00,   IT_OPERATE_IMM,     ENUM_NONE,  MIPS_ISA1 ),
    INS( "lui",     0x0f,   0x00,   IT_LOAD_UIMM,       ENUM_NONE,  MIPS_ISA1 ),
    INS( "daddi",   0x18,   0x00,   IT_OPERATE_IMM,     ENUM_NONE,  MIPS_ISA3 ),
    INS( "daddiu",  0x19,   0x00,   IT_OPERATE_IMM,     ENUM_NONE,  MIPS_ISA3 ),
    INS( "sll",     0x00,   0x00,   IT_OPERATE_SHF,     ENUM_NONE,  MIPS_ISA1 ),
    INS( "srl",     0x00,   0x02,   IT_OPERATE_SHF,     ENUM_NONE,  MIPS_ISA1 ),
    INS( "sra",     0x00,   0x03,   IT_OPERATE_SHF,     ENUM_NONE,  MIPS_ISA1 ),
    INS( "sllv",    0x00,   0x04,   IT_OPERATE,         ENUM_NONE,  MIPS_ISA1 ),
    INS( "srlv",    0x00,   0x06,   IT_OPERATE,         ENUM_NONE,  MIPS_ISA1 ),
    INS( "srav",    0x00,   0x07,   IT_OPERATE,         ENUM_NONE,  MIPS_ISA1 ),
    INS( "dsllv",   0x00,   0x14,   IT_OPERATE,         ENUM_NONE,  MIPS_ISA3 ),
    INS( "dsrlv",   0x00,   0x16,   IT_OPERATE,         ENUM_NONE,  MIPS_ISA3 ),
    INS( "dsrav",   0x00,   0x17,   IT_OPERATE,         ENUM_NONE,  MIPS_ISA3 ),
    INS( "dsll",    0x00,   0x38,   IT_OPERATE_SHF,     ENUM_NONE,  MIPS_ISA3 ),
    INS( "dsrl",    0x00,   0x3a,   IT_OPERATE_SHF,     ENUM_NONE,  MIPS_ISA3 ),
    INS( "dsra",    0x00,   0x3b,   IT_OPERATE_SHF,     ENUM_NONE,  MIPS_ISA3 ),
    INS( "dsll32",  0x00,   0x3c,   IT_OPERATE_SHF,     ENUM_NONE,  MIPS_ISA3 ),
    INS( "dsrl32",  0x00,   0x3e,   IT_OPERATE_SHF,     ENUM_NONE,  MIPS_ISA3 ),
    INS( "dsra32",  0x00,   0x3f,   IT_OPERATE_SHF,     ENUM_NONE,  MIPS_ISA3 ),
 // Two-operand operate instructions
    INS( "mult",    0x00,   0x18,   IT_MUL_DIV,         ENUM_NONE,  MIPS_ISA1 ),
    INS( "multu",   0x00,   0x19,   IT_MUL_DIV,         ENUM_NONE,  MIPS_ISA1 ),
    INS( "div",     0x00,   0x1a,   IT_MUL_DIV,         ENUM_NONE,  MIPS_ISA1 ),
    INS( "divu",    0x00,   0x1b,   IT_MUL_DIV,         ENUM_NONE,  MIPS_ISA1 ),
    INS( "dmult",   0x00,   0x1c,   IT_MUL_DIV,         ENUM_NONE,  MIPS_ISA3 ),
    INS( "dmultu",  0x00,   0x1d,   IT_MUL_DIV,         ENUM_NONE,  MIPS_ISA3 ),
    INS( "ddiv",    0x00,   0x1e,   IT_MUL_DIV,         ENUM_NONE,  MIPS_ISA3 ),
    INS( "ddivu",   0x00,   0x1f,   IT_MUL_DIV,         ENUM_NONE,  MIPS_ISA3 ),
 // Moves to/from special registers
    INS( "mfhi",    0x00,   0x10,   IT_MF_SPECIAL,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "mthi",    0x00,   0x11,   IT_MT_SPECIAL,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "mflo",    0x00,   0x12,   IT_MF_SPECIAL,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "mtlo",    0x00,   0x13,   IT_MT_SPECIAL,      ENUM_NONE,  MIPS_ISA1 ),

 // Floating-Point Operate Format Instructions - Data Type Independent
    INS( "cpys",    0x17,   0x20,   IT_FP_OPERATE,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "cpyse",   0x17,   0x22,   IT_FP_OPERATE,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "cpysn",   0x17,   0x21,   IT_FP_OPERATE,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "cvtlq",   0x17,   0x10,   IT_FP_CONVERT,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "cvtql",   0x17,   0x30,   IT_FP_CONVERT,      ENUM_DTI_CVTQL,  MIPS_ISA1 ),
    INS( "fcmoveq", 0x17,   0x2A,   IT_FP_OPERATE,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "fcmovge", 0x17,   0x2D,   IT_FP_OPERATE,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "fcmovgt", 0x17,   0x2F,   IT_FP_OPERATE,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "fcmovle", 0x17,   0x2E,   IT_FP_OPERATE,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "fcmovlt", 0x17,   0x2C,   IT_FP_OPERATE,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "fcmovne", 0x17,   0x2B,   IT_FP_OPERATE,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "mf_fpcr", 0x17,   0x25,   IT_MT_MF_FPCR,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "mt_fpcr", 0x17,   0x24,   IT_MT_MF_FPCR,      ENUM_NONE,  MIPS_ISA1 ),
 // Floating-Point Operate Format Instructions - IEEE
    INS( "adds",    0x16,   0x80,   IT_FP_OPERATE,      ENUM_IEEE_ADDS,  MIPS_ISA1 ),
    INS( "cmpteq",  0x16,   0xA5,   IT_FP_OPERATE,      ENUM_IEEE_CMPTEQ, MIPS_ISA1 ),
    INS( "cvtqs",   0x16,   0xBC,   IT_FP_CONVERT,      ENUM_IEEE_CVTQS,  MIPS_ISA1 ),
    INS( "cvttq",   0x16,   0xAF,   IT_FP_CONVERT,      ENUM_IEEE_CVTTQ,  MIPS_ISA1 ),

    INS( "mov.s",   0x10,   0x06,   IT_FP_CONVERT,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "mov.d",   0x11,   0x06,   IT_FP_CONVERT,      ENUM_NONE,  MIPS_ISA1 ),
 // Instruction with optional embedded code
    INS( "syscall", 0x00,   0x0c,   IT_SYSCODE,         ENUM_NONE,  MIPS_ISA1 ),
    INS( "break",   0x00,   0x0d,   IT_SYSCODE,         ENUM_NONE,  MIPS_ISA1 ),
    INS( "sync",    0x00,   0x0f,   IT_SYSCODE,         ENUM_NONE,  MIPS_ISA2 ),
 // Stylized Code Forms
    // nop is in directiv.c since it doesn't require much parsing
    INS( "clr",     0x11,   0x20,   IT_PSEUDO_CLR,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "fclr",    0x17,   0x20,   IT_PSEUDO_FCLR,     ENUM_NONE,  MIPS_ISA1 ),
    INS( "fmov",    0x17,   0x20,   IT_PSEUDO_FMOV,     ENUM_NONE,  MIPS_ISA1 ),
    INS( "fneg",    0x17,   0x21,   IT_PSEUDO_FNEG,     ENUM_NONE,  MIPS_ISA1 ),
    INS( "andnot",  0x11,   0x08,   IT_OPERATE,         ENUM_NONE,  MIPS_ISA1 ),
    INS( "xornot",  0x11,   0x48,   IT_OPERATE,         ENUM_NONE,  MIPS_ISA1 ),
    INS( "fabs",    0x17,   0x20,   IT_PSEUDO_NEGF,     ENUM_NONE,  MIPS_ISA1 ),
    INS( "sextl",   0x10,   0x00,   IT_PSEUDO_NOT,      ENUM_NONE,  MIPS_ISA1 ),

    INS( "move",    0x00,   0x25,   IT_PSEUDO_MOV,      ENUM_NONE,  MIPS_ISA1 ),

 // The following pseudo-instructions might emit multiple real instructions
    INS( "li",      0x00,   0x00,   IT_PSEUDO_LIMM,     ENUM_NONE,  MIPS_ISA1 ),
    INS( "la",      0x00,   0x00,   IT_PSEUDO_LADDR,    ENUM_NONE,  MIPS_ISA1 ),
    // abs pseudo ins (opcode & funccode are from subl/v, subq/v)
    INS( "absl",    0x10,   0x49,   IT_PSEUDO_ABS,      ENUM_NONE,  MIPS_ISA1 ),
    INS( "absq",    0x10,   0x69,   IT_PSEUDO_ABS,      ENUM_NONE,  MIPS_ISA1 ),
};

#define MAX_NAME_LEN    20  // maximum length of a MIPS instruction mnemonic (TODO)

static void addInstructionSymbol( qualifier_flags flags, ins_table *table_entry )
//*******************************************************************************
// Given an instruction name for which the optional bits in flags
// are turned on, add a symbol for it to the symbol table.
{
    sym_handle  sym;
    ins_symbol  *entry;
    char        buffer[MAX_NAME_LEN];

    strcpy( buffer, table_entry->name );
    if( flags ) {
        strcat( buffer, "/" );
        if( flags & QF_S ) strcat( buffer, "s" );

        if( flags & QF_U ) strcat( buffer, "u" );   // u & v are mutually
        if( flags & QF_V ) strcat( buffer, "v" );   // exclusive

        if( flags & QF_I ) strcat( buffer, "i" );

        if( flags & QF_C ) strcat( buffer, "c" );   // c, m, & d are
        if( flags & QF_M ) strcat( buffer, "m" );   // mutually exclusive
        if( flags & QF_D ) strcat( buffer, "d" );
    }

    entry = MemAlloc( sizeof( ins_symbol ) );
    entry->table_entry = table_entry;
    entry->flags = flags;

    // link it into our list of symbols for this table entry
    entry->next = table_entry->symbols;
    table_entry->symbols = entry;

    sym = SymAdd( buffer, SYM_INSTRUCTION );
    SymSetLink( sym, (void *)entry );
}

static void enum_NONE( ins_enum_method method, uint_32 mask, uint_8 level, void (*func)( qualifier_flags, ins_table * ), void *parm )
//***********************************************************************************************************************************
{
    method = method;
    level = level;
    mask = mask;
    func( QF_NONE, parm );
}

static void enum_OF_ADDL( ins_enum_method method, uint_32 mask, uint_8 level, void (*func)( qualifier_flags, ins_table * ), void *parm )
//**************************************************************************************************************************************
{
    method = method;
    level = level;
    mask = mask;
    func( QF_NONE, parm );
    func( QF_V, parm );
}

static void enum_DTI_CVTQL( ins_enum_method method, uint_32 mask, uint_8 level, void (*func)( qualifier_flags, ins_table * ), void *parm )
//****************************************************************************************************************************************
{
    method = method;
    level = level;
    mask = mask;
    func( QF_NONE, parm );
    func( QF_V, parm );
    func( QF_S | QF_V, parm );
}

static void enum_IEEE_CMPTEQ( ins_enum_method method, uint_32 mask, uint_8 level, void (*func)( qualifier_flags, ins_table * ), void *parm )
//******************************************************************************************************************************************
{
    method = method;
    level = level;
    mask = mask;
    func( QF_NONE, parm );
    func( QF_S | QF_U, parm );
}

static void enum_IEEE_CVTQS( ins_enum_method method, uint_32 mask, uint_8 level, void (*func)( qualifier_flags, ins_table * ), void *parm )
//*****************************************************************************************************************************************
{
    assert( level < 2 );
    switch( level ) {
    case 0:
        enum_IEEE_CVTQS( method, mask, level + 1, func, parm );
        mask |= ( QF_S | QF_U | QF_I );
        enum_IEEE_CVTQS( method, mask, level + 1, func, parm );
        break;
    case 1:
        func( mask, parm );
        mask |= QF_C;
        func( mask, parm );
        mask &= ~QF_C;
        mask |= QF_M;
        func( mask, parm );
        mask &= ~QF_M;
        mask |= QF_D;
        func( mask, parm );
        break;
    }
}

static void enum_IEEE_ADDS_or_CVTTQ( ins_enum_method method, uint_32 mask, uint_8 level, void (*func)( qualifier_flags, ins_table * ), void *parm )
//*************************************************************************************************************************************************
{
    assert( level < 4 );
    switch( level ) {
    case 0:
        enum_IEEE_ADDS_or_CVTTQ( method, mask, level + 1, func, parm );
        mask |= QF_S;
        enum_IEEE_ADDS_or_CVTTQ( method, mask, level + 1, func, parm );
        break;
    case 1:
        if( !mask ) {
            enum_IEEE_ADDS_or_CVTTQ( method, mask, level + 1, func, parm );
        }
        mask |= ( ( method == ENUM_IEEE_ADDS ) ? QF_U : QF_V );
        enum_IEEE_ADDS_or_CVTTQ( method, mask, level + 1, func, parm );
        break;
    case 2:
        enum_IEEE_ADDS_or_CVTTQ( method, mask, level + 1, func, parm );
        if( mask & QF_S ) {
            mask |= QF_I;
            enum_IEEE_ADDS_or_CVTTQ( method, mask, level + 1, func, parm );
        }
        break;
    case 3:
        func( mask, parm );
        mask |= QF_C;
        func( mask, parm );
        mask &= ~QF_C;
        mask |= QF_M;
        func( mask, parm );
        mask &= ~QF_M;
        mask |= QF_D;
        func( mask, parm );
        break;
    }
}

typedef void (*enumFunc_t)( ins_enum_method, uint_32, uint_8, void (*func)( qualifier_flags, ins_table * ), void * );

static enumFunc_t enumFunc[] = {
    #define PICK( a, b )    b,
    #include "insenum.inc"
    #undef PICK
};

static void enumInstructions( ins_enum_method method, void (*func)( qualifier_flags set, ins_table *parm ), void *parm )
//**********************************************************************************************************************
// Depending on which enum_method it belongs to, different instruction-
// enumeration functions will be called to generate all the possible
// instructions with the different qualifiers attached.
{
    enumFunc[method]( method, QF_NONE, 0, func, parm );
}

#ifdef _STANDALONE_
#ifdef AS_DEBUG_DUMP
static char *itStrings[] = {
    #define PICK( a, b, c, d, e ) #a,
    #include "mipsfmt.inc"
    #undef PICK
};

extern void DumpITString( ins_template template )
{
    printf( itStrings[template] );
}

static char *insEnumStrings[] = {
    #define PICK( a, b ) #a,
    #include "insenum.inc"
    #undef PICK
};

extern void DumpInsEnumMethod( ins_enum_method method )
//*****************************************************
{
    printf( insEnumStrings[method] );
}

extern void DumpInsTableEntry( ins_table *table_entry )
//*****************************************************
{
    ins_symbol  *symbol;

    printf( "%s: 0x%x(0x%x) ", table_entry->name, table_entry->opcode, table_entry->funccode );
    DumpITString( table_entry->template );
    printf( ", " );
    DumpInsEnumMethod( table_entry->method );
    printf( "\n\tSymbol entries: " );
    symbol = table_entry->symbols;
    while( symbol != NULL ) {
        printf( " %x", symbol );
        symbol = symbol->next;
    }
    printf( "\n" );
}

extern void DumpInsTables()
//*************************
{
    ins_table   *curr;
    int         i, n;

    n = sizeof( MIPSTable ) / sizeof( MIPSTable[0] );
    for( i = 0; i < n; i++ ) {
        curr = &MIPSTable[i];
        DumpInsTableEntry( curr );
    }
}
#endif
#endif

extern void InsInit()
//*******************
{
    ins_table   *curr;
    int         i, n;

    n = sizeof( MIPSTable ) / sizeof( MIPSTable[0] );
    for( i = 0; i < n; i++ ) {
        curr = &MIPSTable[i];
        /* for each possible symbol generated by this instruction, add a symbol table entry */
        enumInstructions( curr->method, addInstructionSymbol, curr );
    }
#ifdef AS_DEBUG_DUMP
    #ifdef _STANDALONE_
    if( _IsOption( DUMP_INS_TABLE ) ) DumpInsTables();
    #endif
#endif
}

extern instruction *InsCreate( sym_handle op_sym )
//************************************************
// Allocate an instruction and initialize it.
{
    instruction *ins;

    ins = MemAlloc( sizeof( instruction ) );
    ins->opcode_sym = op_sym;
    ins->format = SymGetLink( op_sym );
    ins->num_operands = 0;
    return( ins );
}

extern void InsAddOperand( instruction *ins, ins_operand *op )
//************************************************************
// Add an operand to the given instruction.
{
    if( ins->num_operands == MAX_OPERANDS ) {
        if( !insErrFlag ) {
            Error( MAX_NUMOP_EXCEEDED );
            insErrFlag = TRUE;
        }
        MemFree( op );
        return;
    }
    if( insErrFlag) insErrFlag = FALSE;
    ins->operands[ins->num_operands++] = op;
}

extern void InsEmit( instruction *ins )
//*************************************
// Check an instruction to make sure operands match
// and encode it. The encoded instruction is emitted
// to the current OWL section.
{
#ifdef AS_DEBUG_DUMP
    #ifdef _STANDALONE_
    if( _IsOption( DUMP_INSTRUCTIONS ) ) {
        DumpIns( ins );
    }
    #endif
#endif
    if( insErrFlag == FALSE && MIPSValidate( ins ) ) {
#ifdef _STANDALONE_
        MIPSEmit( CurrentSection, ins );
#else
        MIPSEmit( ins );
#endif
    }
}

extern void InsDestroy( instruction *ins )
//****************************************
// Free up an instruction and all operands which
// are hanging off of it.
{
    int         i;

    for( i = 0; i < ins->num_operands; i++ ) {
        MemFree( ins->operands[i] );
    }
    MemFree( ins );
}

extern void InsFini()
//*******************
{
    ins_table   *curr;
    ins_symbol  *next;
    ins_symbol  *entry;
    int         i, n;
    extern instruction *AsCurrIns; // from as.y

    if( AsCurrIns != NULL ) {
        InsDestroy( AsCurrIns );
        AsCurrIns = NULL;
    }
    n = sizeof( MIPSTable ) / sizeof( MIPSTable[0] );
    for( i = 0; i < n; i++ ) {
        curr = &MIPSTable[i];
        for( entry = curr->symbols; entry != NULL; entry = next ) {
            next = entry->next;
            MemFree( entry );
        }
        curr->symbols = NULL;   // need to reset this pointer
    }
}

#ifdef _STANDALONE_
#ifdef AS_DEBUG_DUMP
extern void DumpIns( instruction *ins )
//*************************************
{
    int         i;

    printf( "%-11s", SymName( ins->opcode_sym ) );
    for( i = 0; i < ins->num_operands; i++ ) {
        if( i != 0 ) printf( ", " );
        DumpOperand( ins->operands[i] );
    }
    printf( "\n" );
}
#endif
#endif
