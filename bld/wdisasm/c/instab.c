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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "disasm.h"

op_desc  const                InsTab[] = {
    I_ADD,      OT_MOD_RM,              /* 00 */
    I_ADD,      OT_MOD_RM,              /* 01 */
    I_ADD,      OT_MOD_RM,              /* 02 */
    I_ADD,      OT_MOD_RM,              /* 03 */
    I_ADD,      OT_ACCUM_IMMED,         /* 04 */
    I_ADD,      OT_ACCUM_IMMED,         /* 05 */
    I_PUSH,     OT_SEG_OP,              /* 06 */
    I_POP,      OT_SEG_OP,              /* 07 */
    I_OR,       OT_MOD_RM,              /* 08 */
    I_OR,       OT_MOD_RM,              /* 09 */
    I_OR,       OT_MOD_RM,              /* 0a */
    I_OR,       OT_MOD_RM,              /* 0b */
    I_OR,       OT_ACCUM_IMMED,         /* 0c */
    I_OR,       OT_ACCUM_IMMED,         /* 0d */
    I_PUSH,     OT_SEG_OP,              /* 0e */
    I_286,      OT_286,                 /* 0f (286/386) */
    I_ADC,      OT_MOD_RM,              /* 10 */
    I_ADC,      OT_MOD_RM,              /* 11 */
    I_ADC,      OT_MOD_RM,              /* 12 */
    I_ADC,      OT_MOD_RM,              /* 13 */
    I_ADC,      OT_ACCUM_IMMED,         /* 14 */
    I_ADC,      OT_ACCUM_IMMED,         /* 15 */
    I_PUSH,     OT_SEG_OP,              /* 16 */
    I_POP,      OT_SEG_OP,              /* 17 */
    I_SBB,      OT_MOD_RM,              /* 18 */
    I_SBB,      OT_MOD_RM,              /* 19 */
    I_SBB,      OT_MOD_RM,              /* 1a */
    I_SBB,      OT_MOD_RM,              /* 1b */
    I_SBB,      OT_ACCUM_IMMED,         /* 1c */
    I_SBB,      OT_ACCUM_IMMED,         /* 1d */
    I_PUSH,     OT_SEG_OP,              /* 1e */
    I_POP,      OT_SEG_OP,              /* 1f */
    I_AND,      OT_MOD_RM,              /* 20 */
    I_AND,      OT_MOD_RM,              /* 21 */
    I_AND,      OT_MOD_RM,              /* 22 */
    I_AND,      OT_MOD_RM,              /* 23 */
    I_AND,      OT_ACCUM_IMMED,         /* 24 */
    I_AND,      OT_ACCUM_IMMED,         /* 25 */
    I_SEG,      OT_SEG_REG,             /* 26 */
    I_DAA,      OT_NULL,                /* 27 */
    I_SUB,      OT_MOD_RM,              /* 28 */
    I_SUB,      OT_MOD_RM,              /* 29 */
    I_SUB,      OT_MOD_RM,              /* 2a */
    I_SUB,      OT_MOD_RM,              /* 2b */
    I_SUB,      OT_ACCUM_IMMED,         /* 2c */
    I_SUB,      OT_ACCUM_IMMED,         /* 2d */
    I_SEG,      OT_SEG_REG,             /* 2e */
    I_DAS,      OT_NULL,                /* 2f */
    I_XOR,      OT_MOD_RM,              /* 30 */
    I_XOR,      OT_MOD_RM,              /* 31 */
    I_XOR,      OT_MOD_RM,              /* 32 */
    I_XOR,      OT_MOD_RM,              /* 33 */
    I_XOR,      OT_ACCUM_IMMED,         /* 34 */
    I_XOR,      OT_ACCUM_IMMED,         /* 35 */
    I_SEG,      OT_SEG_REG,             /* 36 */
    I_AAA,      OT_NULL,                /* 37 */
    I_CMP,      OT_MOD_RM,              /* 38 */
    I_CMP,      OT_MOD_RM,              /* 39 */
    I_CMP,      OT_MOD_RM,              /* 3a */
    I_CMP,      OT_MOD_RM,              /* 3b */
    I_CMP,      OT_ACCUM_IMMED,         /* 3c */
    I_CMP,      OT_ACCUM_IMMED,         /* 3d */
    I_SEG,      OT_SEG_REG,             /* 3e */
    I_AAS,      OT_NULL,                /* 3f */
    I_INC,      OT_REG16,               /* 40 */
    I_INC,      OT_REG16,               /* 41 */
    I_INC,      OT_REG16,               /* 42 */
    I_INC,      OT_REG16,               /* 43 */
    I_INC,      OT_REG16,               /* 44 */
    I_INC,      OT_REG16,               /* 45 */
    I_INC,      OT_REG16,               /* 46 */
    I_INC,      OT_REG16,               /* 47 */
    I_DEC,      OT_REG16,               /* 48 */
    I_DEC,      OT_REG16,               /* 49 */
    I_DEC,      OT_REG16,               /* 4a */
    I_DEC,      OT_REG16,               /* 4b */
    I_DEC,      OT_REG16,               /* 4c */
    I_DEC,      OT_REG16,               /* 4d */
    I_DEC,      OT_REG16,               /* 4e */
    I_DEC,      OT_REG16,               /* 4f */
    I_PUSH,     OT_REG16,               /* 50 */
    I_PUSH,     OT_REG16,               /* 51 */
    I_PUSH,     OT_REG16,               /* 52 */
    I_PUSH,     OT_REG16,               /* 53 */
    I_PUSH,     OT_REG16,               /* 54 */
    I_PUSH,     OT_REG16,               /* 55 */
    I_PUSH,     OT_REG16,               /* 56 */
    I_PUSH,     OT_REG16,               /* 57 */
    I_POP,      OT_REG16,               /* 58 */
    I_POP,      OT_REG16,               /* 59 */
    I_POP,      OT_REG16,               /* 5a */
    I_POP,      OT_REG16,               /* 5b */
    I_POP,      OT_REG16,               /* 5c */
    I_POP,      OT_REG16,               /* 5d */
    I_POP,      OT_REG16,               /* 5e */
    I_POP,      OT_REG16,               /* 5f */
    I_PUSHA,    OT_NULL,                /* 60 (186) */
    I_POPA,     OT_NULL,                /* 61 (186) */
    I_BOUND,    OT_MOD_RM,              /* 62 (186) */
    I_ARPL,     OT_MOD_RM,              /* 63 (286) */
    I_SEG,      OT_FS,                  /* 64 (FS: 386) */
    I_SEG,      OT_GS,                  /* 65 (GS: 386) */
    I_NULL,     OT_OPND_SIZE,           /* 66 (Opnd size prefix 386) */
    I_NULL,     OT_ADDR_SIZE,           /* 67 (Addr size prefix 386) */
    I_PUSH,     OT_IMMED16,             /* 68 (186) */
    I_IMUL,     OT_REG_MEM_IMMED16,     /* 69 (186) */
    I_PUSH,     OT_IMMED,               /* 6a (186) */
    I_IMUL,     OT_REG_MEM_IMMED8,      /* 6b (186) */
    I_INSB,     OT_NULL,                /* 6c (186) */
    I_INSW,     OT_NULL,                /* 6d (186) */
    I_OUTSB,    OT_NULL,                /* 6e (186) */
    I_OUTSW,    OT_NULL,                /* 6f (186) */
    I_JO,       OT_IP_INC8,             /* 70 */
    I_JNO,      OT_IP_INC8,             /* 71 */
    I_JB,       OT_IP_INC8,             /* 72 */
    I_JAE,      OT_IP_INC8,             /* 73 */
    I_JE,       OT_IP_INC8,             /* 74 */
    I_JNE,      OT_IP_INC8,             /* 75 */
    I_JBE,      OT_IP_INC8,             /* 76 */
    I_JA,       OT_IP_INC8,             /* 77 */
    I_JS,       OT_IP_INC8,             /* 78 */
    I_JNS,      OT_IP_INC8,             /* 79 */
    I_JP,       OT_IP_INC8,             /* 7a */
    I_JPO,      OT_IP_INC8,             /* 7b */
    I_JL,       OT_IP_INC8,             /* 7c */
    I_JGE,      OT_IP_INC8,             /* 7d */
    I_JLE,      OT_IP_INC8,             /* 7e */
    I_JG,       OT_IP_INC8,             /* 7f */
    I_IMMED,    OT_RM_IMMED,            /* 80 */
    I_IMMED,    OT_RM_IMMED,            /* 81 */
    I_IMMED,    OT_RM_IMMED,            /* 82 */
    I_IMMED,    OT_RM_IMMED,            /* 83 */
    I_TEST,     OT_MOD_RM,              /* 84 */
    I_TEST,     OT_MOD_RM,              /* 85 */
    I_XCHG,     OT_MOD_RM,              /* 86 */
    I_XCHG,     OT_MOD_RM,              /* 87 */
    I_MOV,      OT_MOD_RM,              /* 88 */
    I_MOV,      OT_MOD_RM,              /* 89 */
    I_MOV,      OT_MOD_RM,              /* 8a */
    I_MOV,      OT_MOD_RM,              /* 8b */
    I_MOV,      OT_SR_RM,               /* 8c */
    I_LEA,      OT_MOD_REG,             /* 8d */
    I_MOV,      OT_SR_RM,               /* 8e */
    I_POP,      OT_RM,                  /* 8f */
    I_NOP,      OT_NULL,                /* 90 */
    I_XCHG,     OT_ACCUM_REG16,         /* 91 */
    I_XCHG,     OT_ACCUM_REG16,         /* 92 */
    I_XCHG,     OT_ACCUM_REG16,         /* 93 */
    I_XCHG,     OT_ACCUM_REG16,         /* 94 */
    I_XCHG,     OT_ACCUM_REG16,         /* 95 */
    I_XCHG,     OT_ACCUM_REG16,         /* 96 */
    I_XCHG,     OT_ACCUM_REG16,         /* 97 */
    I_CBW,      OT_NULL,                /* 98 */
    I_CWD,      OT_NULL,                /* 99 */
    I_CALL_FAR, OT_SEG_ADDR,            /* 9a */
    I_WAIT,     OT_NULL,                /* 9b */
    I_PUSHF,    OT_NULL,                /* 9c */
    I_POPF,     OT_NULL,                /* 9d */
    I_SAHF,     OT_NULL,                /* 9e */
    I_LAHF,     OT_NULL,                /* 9f */
    I_MOV,      OT_ACCUM_MEM,           /* a0 */
    I_MOV,      OT_ACCUM_MEM,           /* a1 */
    I_MOV,      OT_ACCUM_MEM,           /* a2 */
    I_MOV,      OT_ACCUM_MEM,           /* a3 */
    I_MOVSB,    OT_DS_SI_ES_DI,         /* a4 */
    I_MOVSW,    OT_DS_SI_ES_DI,         /* a5 */
    I_CMPSB,    OT_DS_SI_ES_DI,         /* a6 */
    I_CMPSW,    OT_DS_SI_ES_DI,         /* a7 */
    I_TEST,     OT_ACCUM_IMMED,         /* a8 */
    I_TEST,     OT_ACCUM_IMMED,         /* a9 */
    I_STOSB,    OT_ES_DI,               /* aa */
    I_STOSW,    OT_ES_DI,               /* ab */
    I_LODSB,    OT_DS_SI,               /* ac */
    I_LODSW,    OT_DS_SI,               /* ad */
    I_SCASB,    OT_ES_DI,               /* ae */
    I_SCASW,    OT_ES_DI,               /* af */
    I_MOV,      OT_REG8_IMMED,          /* b0 */
    I_MOV,      OT_REG8_IMMED,          /* b1 */
    I_MOV,      OT_REG8_IMMED,          /* b2 */
    I_MOV,      OT_REG8_IMMED,          /* b3 */
    I_MOV,      OT_REG8_IMMED,          /* b4 */
    I_MOV,      OT_REG8_IMMED,          /* b5 */
    I_MOV,      OT_REG8_IMMED,          /* b6 */
    I_MOV,      OT_REG8_IMMED,          /* b7 */
    I_MOV,      OT_REG16_IMMED,         /* b8 */
    I_MOV,      OT_REG16_IMMED,         /* b9 */
    I_MOV,      OT_REG16_IMMED,         /* ba */
    I_MOV,      OT_REG16_IMMED,         /* bb */
    I_MOV,      OT_REG16_IMMED,         /* bc */
    I_MOV,      OT_REG16_IMMED,         /* bd */
    I_MOV,      OT_REG16_IMMED,         /* be */
    I_MOV,      OT_REG16_IMMED,         /* bf */
    I_SHIFT,    OT_MEM_IMMED8,          /* c0 (186) */
    I_SHIFT,    OT_MEM_IMMED8,          /* c1 (186) */
    I_RET,      OT_IMMED_WORD,          /* c2 */
    I_RET,      OT_NULL,                /* c3 */
    I_LES,      OT_MOD_REG,             /* c4 */
    I_LDS,      OT_MOD_REG,             /* c5 */
    I_MOV,      OT_MEM_IMMED8,          /* c6 */
    I_MOV,      OT_MEM_IMMED16,         /* c7 */
    I_ENTER,    OT_ENTER,               /* c8 (186) */
    I_LEAVE,    OT_NULL,                /* c9 (186) */
    I_RET_FAR,  OT_IMMED_WORD,          /* ca */
    I_RET_FAR,  OT_NULL,                /* cb */
    I_INT,      OT_THREE,               /* cc */
    I_INT,      OT_IMMED8,              /* cd */
    I_INTO,     OT_NULL,                /* ce */
    I_IRET,     OT_NULL,                /* cf */
    I_SHIFT,    OT_RM_1,                /* d0 */
    I_SHIFT,    OT_RM_1,                /* d1 */
    I_SHIFT,    OT_RM_CL,               /* d2 */
    I_SHIFT,    OT_RM_CL,               /* d3 */
    I_AAM,      OT_EXTRA,                /* d4 */
    I_AAD,      OT_EXTRA,                /* d5 */
    I_INVALID,  OT_NULL,                /* d6 */
    I_XLAT,     OT_NULL,                /* d7 */
    I_ESC,      OT_ESC,                 /* d8 */
    I_ESC,      OT_ESC,                 /* d9 */
    I_ESC,      OT_ESC,                 /* da */
    I_ESC,      OT_ESC,                 /* db */
    I_ESC,      OT_ESC,                 /* dc */
    I_ESC,      OT_ESC,                 /* dd */
    I_ESC,      OT_ESC,                 /* de */
    I_ESC,      OT_ESC,                 /* df */
    I_LOOPNE,   OT_IP_INC8,             /* e0 */
    I_LOOPE,    OT_IP_INC8,             /* e1 */
    I_LOOP,     OT_IP_INC8,             /* e2 */
    I_JCXZ,     OT_IP_INC8,             /* e3 */
    I_IN,       OT_ACCUM_IMMED8,        /* e4 */
    I_IN,       OT_ACCUM_IMMED8,        /* e5 */
    I_OUT,      OT_IMMED8_ACCUM,        /* e6 */
    I_OUT,      OT_IMMED8_ACCUM,        /* e7 */
    I_CALL,     OT_IP_INC16,            /* e8 */
    I_JMP,      OT_IP_INC16,            /* e9 */
    I_JMP_FAR,  OT_SEG_ADDR,            /* ea */
    I_JMP,      OT_IP_INC8,             /* eb */
    I_IN,       OT_ACCUM_DX,            /* ec */
    I_IN,       OT_ACCUM_DX,            /* ed */
    I_OUT,      OT_DX_ACCUM,            /* ee */
    I_OUT,      OT_DX_ACCUM,            /* ef */
    I_LOCK,     OT_PREFIX,              /* f0 */
    I_INVALID,  OT_NULL,                /* f1 */
    I_REPNE,    OT_PREFIX,              /* f2 */
    I_REPE,     OT_PREFIX,              /* f3 */
    I_HALT,     OT_NULL,                /* f4 */
    I_CMC,      OT_NULL,                /* f5 */
    I_GROUP1,   OT_RM_TEST,             /* f6 */
    I_GROUP1,   OT_RM_TEST,             /* f7 */
    I_CLC,      OT_NULL,                /* f8 */
    I_STC,      OT_NULL,                /* f9 */
    I_CLI,      OT_NULL,                /* fa */
    I_STI,      OT_NULL,                /* fb */
    I_CLD,      OT_NULL,                /* fc */
    I_STD,      OT_NULL,                /* fd */
    I_GROUP2,   OT_RM,                  /* fe */
    I_GROUP3,   OT_RM                   /* ff */
};

op_desc  const                S4xInsTab[] = {
    I_CMOVO,       OT_CC_MODRM,             /* 0 */
    I_CMOVNO,      OT_CC_MODRM,             /* 1 */
    I_CMOVB,       OT_CC_MODRM,             /* 2 */
    I_CMOVAE,      OT_CC_MODRM,             /* 3 */
    I_CMOVE,       OT_CC_MODRM,             /* 4 */
    I_CMOVNE,      OT_CC_MODRM,             /* 5 */
    I_CMOVBE,      OT_CC_MODRM,             /* 6 */
    I_CMOVA,       OT_CC_MODRM,             /* 7 */
    I_CMOVS,       OT_CC_MODRM,             /* 8 */
    I_CMOVNS,      OT_CC_MODRM,             /* 9 */
    I_CMOVP,       OT_CC_MODRM,             /* a */
    I_CMOVPO,      OT_CC_MODRM,             /* b */
    I_CMOVL,       OT_CC_MODRM,             /* c */
    I_CMOVGE,      OT_CC_MODRM,             /* d */
    I_CMOVLE,      OT_CC_MODRM,             /* e */
    I_CMOVG,       OT_CC_MODRM,             /* f */
};

op_desc  const                S6xInsTab[] = {
    I_PUNPCKLBW,        OT_MM_MODRM,    /* 0 */
    I_PUNPCKLWD,        OT_MM_MODRM,    /* 1 */
    I_PUNPCKLDQ,        OT_MM_MODRM,    /* 2 */
    I_PACKSSWB,         OT_MM_MODRM,    /* 3 */
    I_PCMPGTB,          OT_MM_MODRM,    /* 4 */
    I_PCMPGTW,          OT_MM_MODRM,    /* 5 */
    I_PCMPGTD,          OT_MM_MODRM,    /* 6 */
    I_PACKUSWB,         OT_MM_MODRM,    /* 7 */
    I_PUNPCKHBW,        OT_MM_MODRM,    /* 8 */
    I_PUNPCKHWD,        OT_MM_MODRM,    /* 9 */
    I_PUNPCKHDQ,        OT_MM_MODRM,    /* a */
    I_PACKSSDW,         OT_MM_MODRM,    /* b */
    I_INVALID,          OT_NULL,        /* c */
    I_INVALID,          OT_NULL,        /* d */
    I_MOVD,             OT_MM_MODRM,    /* e */
    I_MOVQ,             OT_MM_MODRM,    /* f */
};

op_desc  const                S7xInsTab[] = {
    I_INVALID,          OT_NULL,        /* 0 */
    I_PSLLW,            OT_MM_IMMED8,   /* 1 */ //NYI
    I_PSLLD,            OT_MM_IMMED8,   /* 2 */ //NYI
    I_PSLLQ,            OT_MM_IMMED8,   /* 3 */ //NYI
    I_PCMPEQB,          OT_MM_MODRM,    /* 4 */
    I_PCMPEQW,          OT_MM_MODRM,    /* 5 */
    I_PCMPEQD,          OT_MM_MODRM,    /* 6 */
    I_EMMS,             OT_NULL,        /* 7 */
    I_INVALID,          OT_NULL,        /* 8 */
    I_INVALID,          OT_NULL,        /* 9 */
    I_INVALID,          OT_NULL,        /* a */
    I_INVALID,          OT_NULL,        /* b */
    I_INVALID,          OT_NULL,        /* c */
    I_INVALID,          OT_NULL,        /* d */
    I_MOVD,             OT_MODRM_MM,    /* e */
    I_MOVQ,             OT_MODRM_MM,    /* f */
};

op_desc  const                SDxInsTab[] = {
    I_INVALID,          OT_NULL,        /* 0 */
    I_PSRLW,            OT_MM_MODRM,    /* 1 */
    I_PSRLD,            OT_MM_MODRM,    /* 2 */
    I_PSRLQ,            OT_MM_MODRM,    /* 3 */
    I_INVALID,          OT_NULL,        /* 4 */
    I_PMULLW,           OT_MM_MODRM,    /* 5 */
    I_INVALID,          OT_NULL,        /* 6 */
    I_INVALID,          OT_NULL,        /* 7 */
    I_PSUBUSB,          OT_MM_MODRM,    /* 8 */
    I_PSUBUSW,          OT_MM_MODRM,    /* 9 */
    I_INVALID,          OT_NULL,        /* a */
    I_PAND,             OT_MM_MODRM,    /* b */
    I_PADDUSB,          OT_MM_MODRM,    /* c */
    I_PADDUSW,          OT_MM_MODRM,    /* d */
    I_INVALID,          OT_NULL,        /* e */
    I_PANDN,            OT_MM_MODRM,    /* f */
};

op_desc  const                SExInsTab[] = {
    I_INVALID,          OT_NULL,        /* 0 */
    I_PSRAW,            OT_MM_MODRM,    /* 1 */
    I_PSRAD,            OT_MM_MODRM,    /* 2 */
    I_INVALID,          OT_NULL,        /* 3 */
    I_INVALID,          OT_NULL,        /* 4 */
    I_PMULHW,           OT_MM_MODRM,    /* 5 */
    I_INVALID,          OT_NULL,        /* 6 */
    I_INVALID,          OT_NULL,        /* 7 */
    I_PSUBSB,           OT_MM_MODRM,    /* 8 */
    I_PSUBSW,           OT_MM_MODRM,    /* 9 */
    I_INVALID,          OT_NULL,        /* a */
    I_POR,              OT_MM_MODRM,    /* b */
    I_PADDSB,           OT_MM_MODRM,    /* c */
    I_PADDSW,           OT_MM_MODRM,    /* d */
    I_INVALID,          OT_NULL,        /* e */
    I_PXOR,             OT_MM_MODRM,    /* f */
};

op_desc  const                SFxInsTab[] = {
    I_INVALID,          OT_NULL,        /* 0 */
    I_PSLLW,            OT_MM_MODRM,    /* 1 */
    I_PSLLD,            OT_MM_MODRM,    /* 2 */
    I_PSLLQ,            OT_MM_MODRM,    /* 3 */
    I_INVALID,          OT_NULL,        /* 4 */
    I_PMADDWD,          OT_MM_MODRM,    /* 5 */
    I_INVALID,          OT_NULL,        /* 6 */
    I_INVALID,          OT_NULL,        /* 7 */
    I_PSUBB,            OT_MM_MODRM,    /* 8 */
    I_PSUBW,            OT_MM_MODRM,    /* 9 */
    I_PSUBD,            OT_MM_MODRM,    /* a */
    I_INVALID,          OT_NULL,        /* b */
    I_PADDB,            OT_MM_MODRM,    /* c */
    I_PADDW,            OT_MM_MODRM,    /* d */
    I_PADDD,            OT_MM_MODRM,    /* e */
    I_INVALID,          OT_NULL,        /* f */
};
