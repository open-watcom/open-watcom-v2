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
* Description:  Table of MIPS opcodes and corresponding decode routines.
*
****************************************************************************/


#define o( p, s, f )        ((p##ul << 26) + (s##ul << 6) + (f##ul))
#define b( p, c )           ((p##ul << 26) + (c##ul << 16))
#define s( p, c )           ((p##ul << 26) + (c##ul))
#define d( p, c )           ((p##ul << 26) + (c##ul << 21))
#define cb( c, p, s )       (((0x10ul + c) << 26) + (p##ul << 21) + (s##ul << 16))
#define cop( c, o )         (((0x10ul + c) << 26) + (0x10ul << 21) + (o##ul))

/*
      Idx,              Name,           Opcode,             Mask,           Handler
*/
// Integer opcodes first
inspick( ADD,           "add",          o(0x00,0x00,0x20),  0xfc0007ff,     MIPSReg3 )
inspick( ADDI,          "addi",         0x20000000,         0xfc000000,     MIPSImmed2 )
inspick( ADDIU,         "addiu",        0x24000000,         0xfc000000,     MIPSImmed2 )
inspick( ADDU,          "addu",         o(0x00,0x00,0x21),  0xfc0007ff,     MIPSReg3 )
inspick( AND,           "and",          o(0x00,0x00,0x24),  0xfc0007ff,     MIPSReg3 )
inspick( ANDI,          "andi",         0x30000000,         0xfc000000,     MIPSImmed2U )
inspick( BC0F,          "bc0f",         cb(0,0x08,0x00),    0xffff0000,     MIPSBranchCop )
inspick( BC0FL,         "bc0fl",        cb(0,0x08,0x02),    0xffff0000,     MIPSBranchCop )
inspick( BC0T,          "bc0t",         cb(0,0x08,0x01),    0xffff0000,     MIPSBranchCop )
inspick( BC0TL,         "bc0tl",        cb(0,0x08,0x03),    0xffff0000,     MIPSBranchCop )
inspick( BEQ,           "beq",          0x10000000,         0xfc000000,     MIPSBranch2 )
inspick( BEQL,          "beql",         0x50000000,         0xfc000000,     MIPSBranch2 )
inspick( BGEZ,          "bgez",         b(0x01,0x01),       0xfc1f0000,     MIPSBranch1 )
inspick( BGEZAL,        "bgezal",       b(0x01,0x11),       0xfc1f0000,     MIPSBranch1 )
inspick( BGEZALL,       "bgezall",      b(0x01,0x13),       0xfc1f0000,     MIPSBranch1 )
inspick( BGEZL,         "bgezl",        b(0x01,0x03),       0xfc1f0000,     MIPSBranch1 )
inspick( BGTZ,          "bgtz",         b(0x07,0x00),       0xfc1f0000,     MIPSBranch3 )
inspick( BGTZL,         "bgtzl",        b(0x17,0x00),       0xfc1f0000,     MIPSBranch3 )
inspick( BLEZ,          "blez",         b(0x06,0x00),       0xfc1f0000,     MIPSBranch3 )
inspick( BLEZL,         "blezl",        b(0x16,0x00),       0xfc1f0000,     MIPSBranch3 )
inspick( BLTZ,          "bltz",         b(0x01,0x00),       0xfc1f0000,     MIPSBranch1 )
inspick( BLTZAL,        "bltzal",       b(0x01,0x10),       0xfc1f0000,     MIPSBranch1 )
inspick( BLTZALL,       "bltzall",      b(0x01,0x12),       0xfc1f0000,     MIPSBranch1 )
inspick( BLTZL,         "bltzl",        b(0x01,0x02),       0xfc1f0000,     MIPSBranch1 )
inspick( BNE,           "bne",          0x14000000,         0xfc000000,     MIPSBranch2 )
inspick( BNEL,          "bnel",         0x54000000,         0xfc000000,     MIPSBranch2 )
inspick( BREAK,         "break",        s(0x00,0x0d),       0xfc00003f,     MIPSCode )
inspick( CACHE,         "cache",        0xbc000000,         0xfc000000,     MIPSCache )
// CFCz
// COPz
// CTCz
inspick( DADD,          "dadd",         s(0x00,0x2c),       0xfc0007ff,     MIPSReg3 )
inspick( DADDI,         "daddi",        0x60000000,         0xfc000000,     MIPSImmed2 )
inspick( DADDIU,        "daddiu",       0x64000000,         0xfc000000,     MIPSImmed2 )
inspick( DADDU,         "daddu",        o(0x00,0x00,0x2d),  0xfc0007ff,     MIPSReg3 )
inspick( DDIV,          "ddiv",         s(0x00,0x1e),       0xfc00ffff,     MIPSMulDiv )
inspick( DDIVU,         "ddivu",        s(0x00,0x1f),       0xfc00ffff,     MIPSMulDiv )
inspick( DIV,           "div",          s(0x00,0x1a),       0xfc00ffff,     MIPSMulDiv )
inspick( DIVU,          "divu",         s(0x00,0x1b),       0xfc00ffff,     MIPSMulDiv )
inspick( DMFC0,         "dmfc0",        d(0x10,0x01),       0xffe007ff,     MIPSReg2 )
inspick( DMTC0,         "dmtc0",        d(0x10,0x05),       0xffe007ff,     MIPSReg2 )
inspick( DMULT,         "dmult",        s(0x00,0x1c),       0xfc00ffff,     MIPSMulDiv )
inspick( DMULTU,        "dmultu",       s(0x00,0x1d),       0xfc00ffff,     MIPSMulDiv )
inspick( DSLL,          "dsll",         s(0x00,0x38),       0xffe0003f,     MIPSShift )
inspick( DSLLV,         "dsllv",        s(0x00,0x14),       0xfc0007ff,     MIPSReg3 )
inspick( DSLL32,        "dsll32",       s(0x00,0x3c),       0xffe0003f,     MIPSShift )
inspick( DSRA,          "dsra",         s(0x00,0x3b),       0xffe0003f,     MIPSShift )
inspick( DSRAV,         "dsrav",        s(0x00,0x17),       0xfc0007ff,     MIPSReg3 )
inspick( DSRA32,        "dsra32",       s(0x00,0x3f),       0xffe0003f,     MIPSShift )
inspick( DSRL,          "dsrl",         s(0x00,0x3a),       0xffe0003f,     MIPSShift )
inspick( DSRLV,         "dsrlv",        s(0x00,0x16),       0xfc0007ff,     MIPSReg3 )
inspick( DSRL32,        "dsrl32",       s(0x00,0x3e),       0xffe0003f,     MIPSShift )
inspick( DSUB,          "dsub",         s(0x00,0x2e),       0xfc0007ff,     MIPSReg3 )
inspick( DSUBU,         "dsubu",        s(0x00,0x2f),       0xfc0007ff,     MIPSReg3 )
inspick( ERET,          "eret",         0x42000018,         0xffffffff,     MIPSNull )
inspick( J,             "j",            s(0x02,0x00),       0xfc000000,     MIPSJType )
inspick( JAL,           "jal",          s(0x03,0x00),       0xfc000000,     MIPSJType )
inspick( JALR,          "jalr",         s(0x00,0x09),       0xfc1f07ff,     MIPSJump2 )
inspick( JR,            "jr",           s(0x00,0x08),       0xfc1fffff,     MIPSJump1 )

// All load instructions bunched together
inspick( LB,            "lb",           s(0x20,0x00),       0xfc000000,     MIPSMemory )
inspick( LBU,           "lbu",          s(0x24,0x00),       0xfc000000,     MIPSMemory )
inspick( LD,            "ld",           s(0x37,0x00),       0xfc000000,     MIPSMemory )
inspick( LDL,           "ldl",          s(0x1a,0x00),       0xfc000000,     MIPSMemory )
inspick( LDR,           "ldr",          s(0x1b,0x00),       0xfc000000,     MIPSMemory )
inspick( LH,            "lh",           s(0x21,0x00),       0xfc000000,     MIPSMemory )
inspick( LHU,           "lhu",          s(0x25,0x00),       0xfc000000,     MIPSMemory )
inspick( LL,            "ll",           s(0x30,0x00),       0xfc000000,     MIPSMemory )
inspick( LLD,           "lld",          s(0x34,0x00),       0xfc000000,     MIPSMemory )
inspick( LW,            "lw",           s(0x23,0x00),       0xfc000000,     MIPSMemory )
inspick( LWL,           "lwl",          s(0x22,0x00),       0xfc000000,     MIPSMemory )
inspick( LWR,           "lwr",          s(0x26,0x00),       0xfc000000,     MIPSMemory )
inspick( LWU,           "lwu",          s(0x27,0x00),       0xfc000000,     MIPSMemory )
inspick( LDC1,          "ldc1",         s(0x35,0x00),       0xfc000000,     MIPSFPUMemory )
inspick( LWC1,          "lwc1",         s(0x31,0x00),       0xfc000000,     MIPSFPUMemory )
// Other LWCz/LDCz instructions
inspick( LUI,           "lui",          s(0x0f,0x00),       0xffe00000,     MIPSImmed1 )
inspick( MFC0,          "mfc0",         d(0x10,0x00),       0xffe007ff,     MIPSReg2 )
inspick( MFHI,          "mfhi",         s(0x00,0x10),       0xffff07ff,     MIPSRegD )
inspick( MFLO,          "mflo",         s(0x00,0x12),       0xffff07ff,     MIPSRegD )
inspick( MTC0,          "mtc0",         d(0x10,0x04),       0xffe007ff,     MIPSReg2 )
inspick( MTHI,          "mthi",         s(0x00,0x11),       0xfc1fffff,     MIPSRegS )
inspick( MTLO,          "mtlo",         s(0x00,0x13),       0xfc1fffff,     MIPSRegS )
inspick( MULT,          "mult",         s(0x00,0x18),       0xfc00ffff,     MIPSMulDiv )
inspick( MULTU,         "multu",        s(0x00,0x19),       0xfc00ffff,     MIPSMulDiv )
inspick( NOR,           "nor",          s(0x00,0x27),       0xfc0007ff,     MIPSReg3 )
inspick( OR,            "or",           s(0x00,0x25),       0xfc0007ff,     MIPSReg3 )
inspick( ORI,           "ori",          s(0x0d,0x00),       0xfc000000,     MIPSImmed2U )

// All store instructions
inspick( SB,            "sb",           s(0x28,0x00),       0xfc000000,     MIPSMemory )
inspick( SC,            "sc",           s(0x38,0x00),       0xfc000000,     MIPSMemory )
inspick( SCD,           "scd",          s(0x3c,0x00),       0xfc000000,     MIPSMemory )
inspick( SD,            "sd",           s(0x3f,0x00),       0xfc000000,     MIPSMemory )
inspick( SDL,           "sdl",          s(0x2c,0x00),       0xfc000000,     MIPSMemory )
inspick( SDR,           "sdr",          s(0x2d,0x00),       0xfc000000,     MIPSMemory )
inspick( SH,            "sh",           s(0x29,0x00),       0xfc000000,     MIPSMemory )
inspick( SW,            "sw",           s(0x2b,0x00),       0xfc000000,     MIPSMemory )
inspick( SWL,           "swl",          s(0x2a,0x00),       0xfc000000,     MIPSMemory )
inspick( SWR,           "swr",          s(0x2e,0x00),       0xfc000000,     MIPSMemory )
inspick( SDC1,          "sdc1",         s(0x3d,0x00),       0xfc000000,     MIPSFPUMemory )
inspick( SWC1,          "swc1",         s(0x39,0x00),       0xfc000000,     MIPSFPUMemory )
// Other SWCz/SDCz instructions

inspick( RFE,           "rfe",          0x42000010,         0xffffffff,     MIPSNull )
inspick( SLL,           "sll",          s(0x00,0x00),       0xffe0003f,     MIPSShift )
inspick( SLLV,          "sllv",         s(0x00,0x04),       0xfc0007ff,     MIPSReg3 )
inspick( SLT,           "slt",          s(0x00,0x2a),       0xfc0007ff,     MIPSReg3 )
inspick( SLTI,          "slti",         s(0x0a,0x00),       0xfc000000,     MIPSImmed2 )
inspick( SLTIU,         "sltiu",        s(0x0b,0x00),       0xfc000000,     MIPSImmed2 )
inspick( SLTU,          "sltu",         s(0x00,0x2b),       0xfc0007ff,     MIPSReg3 )
inspick( SRA,           "sra",          s(0x00,0x03),       0xffe0003f,     MIPSShift )
inspick( SRAV,          "srav",         s(0x00,0x07),       0xfc0007ff,     MIPSReg3 )
inspick( SRL,           "srl",          s(0x00,0x02),       0xffe0003f,     MIPSShift )
inspick( SRLV,          "srlv",         s(0x00,0x06),       0xfc0007ff,     MIPSReg3 )
inspick( SUB,           "sub",          s(0x00,0x22),       0xfc0007ff,     MIPSReg3 )
inspick( SUBU,          "subu",         s(0x00,0x23),       0xfc0007ff,     MIPSReg3 )
// Note extra spaces to get longer max instruction length (for FPU instructions)
inspick( SYNC,          "sync     ",    0x0000000f,         0xffffffff,     MIPSNull )
inspick( SYSCALL,       "syscall",      s(0x00,0x0c),       0xfc00003f,     MIPSCode )
inspick( TEQ,           "teq",          s(0x00,0x34),       0xfc00003f,     MIPSTrap2 )
inspick( TEQI,          "teqi",         b(0x01,0x0c),       0xfc1f0000,     MIPSTrap1 )
inspick( TGE,           "tge",          s(0x00,0x30),       0xfc00003f,     MIPSTrap2 )
inspick( TGEI,          "tgei",         b(0x01,0x08),       0xfc1f0000,     MIPSTrap1 )
inspick( TGEIU,         "tgeiu",        b(0x01,0x09),       0xfc1f0000,     MIPSTrap1 )
inspick( TGEU,          "tgeu",         s(0x00,0x31),       0xfc00003f,     MIPSTrap2 )
inspick( TLBP,          "tlbp",         0x42000008,         0xffffffff,     MIPSNull )
inspick( TLBR,          "tlbr",         0x42000001,         0xffffffff,     MIPSNull )
inspick( TLBWI,         "tlbwi",        0x42000002,         0xffffffff,     MIPSNull )
inspick( TLBWR,         "tlbwr",        0x42000006,         0xffffffff,     MIPSNull )
inspick( TLT,           "tlt",          s(0x00,0x32),       0xfc00003f,     MIPSTrap2 )
inspick( TLTI,          "tlti",         b(0x01,0x0a),       0xfc1f0000,     MIPSTrap1 )
inspick( TLTIU,         "tltiu",        b(0x01,0x0b),       0xfc1f0000,     MIPSTrap1 )
inspick( TLTU,          "tltu",         s(0x00,0x33),       0xfc00003f,     MIPSTrap2 )
inspick( TNE,           "tne",          s(0x00,0x36),       0xfc00003f,     MIPSTrap2 )
inspick( TNEI,          "tnei",         b(0x01,0x0e),       0xfc1f0000,     MIPSTrap1 )
inspick( XOR,           "xor",          s(0x00,0x26),       0xfc0007ff,     MIPSReg3 )
inspick( XORI,          "xori",         s(0x0e,0x00),       0xfc000000,     MIPSImmed2U )

// Floating point opcodes (ie. Coprocessor 1)
inspick( ABS_f,         "abs",          cop(1,0x05),        0xfe00003f,     MIPSFPUOp2 )
inspick( ADD_f,         "add",          cop(1,0x00),        0xfe00003f,     MIPSFPUOp3 )
inspick( BC1F,          "bc1f",         cb(1,0x08,0x00),    0xffff0000,     MIPSBranchCop )
inspick( BC1FL,         "bc1fl",        cb(1,0x08,0x02),    0xffff0000,     MIPSBranchCop )
inspick( BC1T,          "bc1t",         cb(1,0x08,0x01),    0xffff0000,     MIPSBranchCop )
inspick( BC1TL,         "bc1tl",        cb(1,0x08,0x03),    0xffff0000,     MIPSBranchCop )
inspick( C_F_f,         "c.f",          cop(1,0x30),        0xfe00003f,     MIPSFPUCmp )
inspick( C_UN_f,        "c.un",         cop(1,0x31),        0xfe00003f,     MIPSFPUCmp )
inspick( C_EQ_f,        "c.eq",         cop(1,0x32),        0xfe00003f,     MIPSFPUCmp )
inspick( C_UEQ_f,       "c.ueq",        cop(1,0x33),        0xfe00003f,     MIPSFPUCmp )
inspick( C_OLT_f,       "c.olt",        cop(1,0x34),        0xfe00003f,     MIPSFPUCmp )
inspick( C_ULT_f,       "c.ult",        cop(1,0x35),        0xfe00003f,     MIPSFPUCmp )
inspick( C_OLE_f,       "c.ole",        cop(1,0x36),        0xfe00003f,     MIPSFPUCmp )
inspick( C_ULE_f,       "c.ule",        cop(1,0x37),        0xfe00003f,     MIPSFPUCmp )
inspick( C_SF_f,        "c.sf",         cop(1,0x38),        0xfe00003f,     MIPSFPUCmp )
inspick( C_NGLE_f,      "c.ngle",       cop(1,0x39),        0xfe00003f,     MIPSFPUCmp )
inspick( C_SEQ_f,       "c.seq",        cop(1,0x3a),        0xfe00003f,     MIPSFPUCmp )
inspick( C_NGL_f,       "c.ngl",        cop(1,0x3b),        0xfe00003f,     MIPSFPUCmp )
inspick( C_LE_f,        "c.le",         cop(1,0x3c),        0xfe00003f,     MIPSFPUCmp )
inspick( C_LT_f,        "c.lt",         cop(1,0x3d),        0xfe00003f,     MIPSFPUCmp )
inspick( C_NGE_f,       "c.nge",        cop(1,0x3e),        0xfe00003f,     MIPSFPUCmp )
inspick( C_NGT_f,       "c.ngt",        cop(1,0x3f),        0xfe00003f,     MIPSFPUCmp )
inspick( CEIL_L_f,      "ceil.l",       cop(1,0x0a),        0xfe00003f,     MIPSFPUOp3 )
inspick( CEIL_W_f,      "ceil.w",       cop(1,0x0e),        0xfe00003f,     MIPSFPUOp3 )
inspick( CFC1,          "cfc1",         cb(1,0x02,0x00),    0xffe007ff,     MIPSFGMove )
inspick( CTC1,          "ctc1",         cb(1,0x06,0x00),    0xffe007ff,     MIPSFGMove )
inspick( CVT_D_f,       "cvt.d",        cop(1,0x21),        0xfe00003f,     MIPSFPUOp2 )
inspick( CVT_L_f,       "cvt.l",        cop(1,0x25),        0xfe00003f,     MIPSFPUOp2 )
inspick( CVT_S_f,       "cvt.s",        cop(1,0x20),        0xfe00003f,     MIPSFPUOp2 )
inspick( CVT_W_f,       "cvt.w",        cop(1,0x24),        0xfe00003f,     MIPSFPUOp2 )
inspick( DIV_f,         "div",          cop(1,0x03),        0xfe00003f,     MIPSFPUOp3 )
inspick( DMFC1,         "dmfc1",        d(0x11,0x01),       0xffe007ff,     MIPSReg2 )
inspick( DMTC1,         "dmtc1",        d(0x11,0x05),       0xffe007ff,     MIPSReg2 )
inspick( FLOOR_L_f,     "floor.l",      cop(1,0x0b),        0xfe00003f,     MIPSFPUOp2 )
inspick( FLOOR_W_f,     "floor.w",      cop(1,0x0f),        0xfe00003f,     MIPSFPUOp2 )
inspick( MFC1,          "mfc1",         cb(1,0x00,0x00),    0xffe007ff,     MIPSFGMove )
inspick( MOV_f,         "mov",          cop(1,0x06),        0xfe00003f,     MIPSFPUOp2 )
inspick( MTC1,          "mtc1",         cb(1,0x04,0x00),    0xffe007ff,     MIPSFGMove )
inspick( MUL_f,         "mul",          cop(1,0x02),        0xfe00003f,     MIPSFPUOp3 )
inspick( NEG_f,         "neg",          cop(1,0x07),        0xfe00003f,     MIPSFPUOp2 )
inspick( ROUND_L_f,     "round.l",      cop(1,0x08),        0xfe00003f,     MIPSFPUOp2 )
inspick( ROUND_W_f,     "round.w",      cop(1,0x0c),        0xfe00003f,     MIPSFPUOp2 )
inspick( SQRT_f,        "sqrt",         cop(1,0x04),        0xfe00003f,     MIPSFPUOp2 )
inspick( SUB_f,         "sub",          cop(1,0x01),        0xfe00003f,     MIPSFPUOp3 )
inspick( TRUNC_L_f,     "trunc.l",      cop(1,0x09),        0xfe00003f,     MIPSFPUOp2 )
inspick( TRUNC_W_f,     "trunc.w",      cop(1,0x0d),        0xfe00003f,     MIPSFPUOp2 )

#undef o
#undef b
#undef s
#undef d
#undef cb
#undef cop
