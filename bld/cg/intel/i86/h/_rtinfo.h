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
* Description:  Runtime support routines list for i86.
*
****************************************************************************/



/*    enum            name           op          class left      right     result */

/*  beginning of runtime routine */
PICK( RT_U4FS,        "__U4FS",      OP_CONVERT, U4,   RL_DX_AX, RL_,      RL_DX_AX )
PICK( RT_I4FS,        "__I4FS",      OP_CONVERT, I4,   RL_DX_AX, RL_,      RL_DX_AX )
PICK( RT_U4FD,        "__U4FD",      OP_CONVERT, U4,   RL_DX_AX, RL_,      RL_8 )
PICK( RT_I4FD,        "__I4FD",      OP_CONVERT, I4,   RL_DX_AX, RL_,      RL_8 )
PICK( RT_U8FS,        "__U8FS",      OP_CONVERT, U8,   RL_8,     RL_,      RL_DX_AX )
PICK( RT_I8FS,        "__I8FS",      OP_CONVERT, I8,   RL_8,     RL_,      RL_DX_AX )
PICK( RT_U8FD,        "__U8FD",      OP_CONVERT, U8,   RL_8,     RL_,      RL_8 )
PICK( RT_I8FD,        "__I8FD",      OP_CONVERT, I8,   RL_8,     RL_,      RL_8 )
PICK( RT_FSFD,        "__FSFD",      OP_CONVERT, FS,   RL_DX_AX, RL_,      RL_8 )

/* Following types have both truncation and rounding */
PICK( RT_FSI4,        "__FSI4",      OP_CONVERT, FS,   RL_DX_AX, RL_,      RL_DX_AX )
PICK( RT_RSI4,        "__RSI4",      OP_ROUND,   FS,   RL_DX_AX, RL_,      RL_DX_AX )
PICK( RT_FSU4,        "__FSU4",      OP_CONVERT, FS,   RL_DX_AX, RL_,      RL_DX_AX )
PICK( RT_RSU4,        "__RSU4",      OP_ROUND,   FS,   RL_DX_AX, RL_,      RL_DX_AX )
PICK( RT_FSI8,        "__FSI8",      OP_CONVERT, FS,   RL_DX_AX, RL_,      RL_8 )
PICK( RT_RSI8,        "__RSI8",      OP_ROUND,   FS,   RL_DX_AX, RL_,      RL_8 )
PICK( RT_FSU8,        "__FSU8",      OP_CONVERT, FS,   RL_DX_AX, RL_,      RL_8 )
PICK( RT_RSU8,        "__RSU8",      OP_ROUND,   FS,   RL_DX_AX, RL_,      RL_8 )
PICK( RT_FDI4,        "__FDI4",      OP_CONVERT, FD,   RL_8,     RL_,      RL_DX_AX )
PICK( RT_RDI4,        "__RDI4",      OP_ROUND,   FD,   RL_8,     RL_,      RL_DX_AX )
PICK( RT_FDU4,        "__FDU4",      OP_CONVERT, FD,   RL_8,     RL_,      RL_DX_AX )
PICK( RT_RDU4,        "__RDU4",      OP_ROUND,   FD,   RL_8,     RL_,      RL_DX_AX )
PICK( RT_FDI8,        "__FDI8",      OP_CONVERT, FD,   RL_8,     RL_,      RL_8 )
PICK( RT_RDI8,        "__RDI8",      OP_ROUND,   FD,   RL_8,     RL_,      RL_8 )
PICK( RT_FDU8,        "__FDU8",      OP_CONVERT, FD,   RL_8,     RL_,      RL_8 )
PICK( RT_RDU8,        "__RDU8",      OP_ROUND,   FD,   RL_8,     RL_,      RL_8 )

PICK( RT_U8FS7,       "__U8FS7",     OP_CONVERT, U8,   RL_8,     RL_,      RL_DX_AX )
PICK( RT_U8FD7,       "__U8FD7",     OP_CONVERT, U8,   RL_8,     RL_,      RL_8 )
PICK( RT_FSU87,       "__FSU87",     OP_CONVERT, FS,   RL_DX_AX, RL_,      RL_8 )
PICK( RT_FDU87,       "__FDU87",     OP_CONVERT, FD,   RL_8,     RL_,      RL_8 )

PICK( RT_FDFS,        "__FDFS",      OP_CONVERT, FD,   RL_8,     RL_,      RL_DX_AX )
PICK( RT_RDFS,        "__RDFS",      OP_ROUND,   FD,   RL_8,     RL_,      RL_DX_AX )

/* Following types are runtime routines to do arithmetic */
PICK( RT_FSA,         "__FSA",       OP_ADD,     FS,   RL_DX_AX, RL_CX_BX, RL_DX_AX )
PICK( RT_FSS,         "__FSS",       OP_SUB,     FS,   RL_DX_AX, RL_CX_BX, RL_DX_AX )
PICK( RT_FSM,         "__FSM",       OP_MUL,     FS,   RL_DX_AX, RL_CX_BX, RL_DX_AX )
PICK( RT_FSD,         "__FSD",       OP_DIV,     FS,   RL_DX_AX, RL_CX_BX, RL_DX_AX )
PICK( RT_FSC,         "__FSC",       OP_CMP,     FS,   RL_DX_AX, RL_CX_BX, RL_ )
PICK( RT_FSN,         "__FSN",       OP_NEGATE,  FS,   RL_DX_AX, RL_,      RL_DX_AX )
PICK( RT_FDA,         "__FDA",       OP_ADD,     FD,   RL_8,     RL_8,     RL_8 )
PICK( RT_EDA,         "__EDA",       OP_ADD,     FD,   RL_8,     RL_8,     RL_8 )
PICK( RT_FDS,         "__FDS",       OP_SUB,     FD,   RL_8,     RL_8,     RL_8 )
PICK( RT_EDS,         "__EDS",       OP_SUB,     FD,   RL_8,     RL_8,     RL_8 )
PICK( RT_FDM,         "__FDM",       OP_MUL,     FD,   RL_8,     RL_8,     RL_8 )
PICK( RT_EDM,         "__EDM",       OP_MUL,     FD,   RL_8,     RL_8,     RL_8 )
PICK( RT_FDD,         "__FDD",       OP_DIV,     FD,   RL_8,     RL_8,     RL_8 )
PICK( RT_EDD,         "__EDD",       OP_DIV,     FD,   RL_8,     RL_8,     RL_8 )
PICK( RT_FDC,         "__FDC",       OP_CMP,     FD,   RL_8,     RL_8,     RL_ )
PICK( RT_EDC,         "__EDC",       OP_CMP,     FD,   RL_8,     RL_8,     RL_ )
PICK( RT_FDN,         "__FDN",       OP_NEGATE,  FD,   RL_8,     RL_,      RL_8 )
PICK( RT_I4M,         "__I4M",       OP_MUL,     I4,   RL_DX_AX, RL_CX_BX, RL_DX_AX )
PICK( RT_I4D,         "__I4D",       OP_DIV,     I4,   RL_DX_AX, RL_CX_BX, RL_DX_AX )
PICK( RT_I4MOD,       "__I4D",       OP_MOD,     I4,   RL_DX_AX, RL_CX_BX, RL_CX_BX )
PICK( RT_I8M,         "__I8M",       OP_MUL,     I8,   RL_8,     RL_8,     RL_8 )
PICK( RT_I8ME,        "__I8ME",      OP_MUL,     I8,   RL_8,     RL_8,     RL_8 )
PICK( RT_I8D,         "__I8DQ",      OP_DIV,     I8,   RL_8,     RL_8,     RL_8 )
PICK( RT_I8DE,        "__I8DQE",     OP_DIV,     I8,   RL_8,     RL_8,     RL_8 )
PICK( RT_I8MOD,       "__I8DR",      OP_MOD,     I8,   RL_8,     RL_8,     RL_8 )
PICK( RT_I8MODE,      "__I8DRE",     OP_MOD,     I8,   RL_8,     RL_8,     RL_8 )
PICK( RT_U4M,         "__U4M",       OP_MUL,     U4,   RL_DX_AX, RL_CX_BX, RL_DX_AX )
PICK( RT_U4D,         "__U4D",       OP_DIV,     U4,   RL_DX_AX, RL_CX_BX, RL_DX_AX )
PICK( RT_U4MOD,       "__U4D",       OP_MOD,     U4,   RL_DX_AX, RL_CX_BX, RL_CX_BX )
PICK( RT_U8M,         "__U8M",       OP_MUL,     U8,   RL_8,     RL_8,     RL_8 )
PICK( RT_U8ME,        "__U8ME",      OP_MUL,     U8,   RL_8,     RL_8,     RL_8 )
PICK( RT_U8D,         "__U8DQ",      OP_DIV,     U8,   RL_8,     RL_8,     RL_8 )
PICK( RT_U8DE,        "__U8DQE",     OP_DIV,     U8,   RL_8,     RL_8,     RL_8 )
PICK( RT_U8MOD,       "__U8DR",      OP_MOD,     U8,   RL_8,     RL_8,     RL_8 )
PICK( RT_U8MODE,      "__U8DRE",     OP_MOD,     U8,   RL_8,     RL_8,     RL_8 )
PICK( RT_I8RS,        "__I8LS",      OP_LSHIFT,  I8,   RL_8,     RL_SI,    RL_8 )
PICK( RT_I8LS,        "__I8RS",      OP_RSHIFT,  I8,   RL_8,     RL_SI,    RL_8 )
PICK( RT_U8RS,        "__U8LS",      OP_LSHIFT,  U8,   RL_8,     RL_SI,    RL_8 )
PICK( RT_U8LS,        "__U8RS",      OP_RSHIFT,  U8,   RL_8,     RL_SI,    RL_8 )

PICK( RT_PTS,         "__PTS",       OP_SUB,     PT,   RL_DX_AX, RL_CX_BX, RL_DX_AX )
PICK( RT_PTC,         "__PTC",       OP_CMP,     PT,   RL_DX_AX, RL_CX_BX, RL_ )
PICK( RT_PCS,         "__PIS",       OP_SUB,     PT,   RL_DX_AX, RL_CX_BX, RL_DX_AX )
PICK( RT_PCA,         "__PIA",       OP_ADD,     PT,   RL_DX_AX, RL_CX_BX, RL_DX_AX )
PICK( RT_CHK,         "__STK",       OP_CALL,    0,    RL_,      RL_,      RL_ )
PICK( RT_CHOP,        "__CHP",       OP_CALL,    0,    RL_,      RL_,      RL_ )
PICK( RT_SCAN1,       "__SCN1",      OP_SELECT,  0,    RL_AL,    RL_,      RL_ )
PICK( RT_SCAN2,       "__SCN2",      OP_SELECT,  0,    RL_AX,    RL_,      RL_ )
PICK( RT_SCAN4,       "__SCN4",      OP_SELECT,  0,    RL_DX_AX, RL_,      RL_ )
PICK( RT_EPIHOOK,     "__EPI",       OP_CALL,    0,    RL_,      RL_,      RL_ )
PICK( RT_PROHOOK,     "__PRO",       OP_CALL,    0,    RL_,      RL_,      RL_ )

PICK( RT_DP5DIV,      "IF@DP5DIV",   OP_P5DIV,   FD,   RL_8,     RL_,      RL_8 )
PICK( RT_P5DIV,       "IF@P5DIV",    OP_P5DIV,   FS,   RL_DX_AX, RL_CX_BX, RL_DX_AX )

PICK( RT_DPOW,        "IF@DPOW",     OP_POW,     FD,   RL_8,     RL_,      RL_8 )
PICK( RT_DPOWI,       "IF@DPOWI",    OP_POW,     FD,   RL_8,     RL_,      RL_8 )
PICK( RT_POW,         "IF@POW",      OP_POW,     FS,   RL_DX_AX, RL_CX_BX, RL_DX_AX )
PICK( RT_POWI,        "IF@POWI",     OP_POW,     FS,   RL_DX_AX, RL_CX_BX, RL_DX_AX )
PICK( RT_IPOW,        "IF@IPOW",     OP_POW,     I4,   RL_DX_AX, RL_CX_BX, RL_DX_AX )

PICK( RT_DATAN2,      "IF@DATAN2",   OP_ATAN2,   FD,   RL_8,     RL_,      RL_8 )
PICK( RT_DFMOD,       "IF@DFMOD",    OP_FMOD,    FD,   RL_8,     RL_,      RL_8 )
PICK( RT_DLOG,        "IF@DLOG",     OP_LOG,     FD,   RL_8,     RL_,      RL_8 )
PICK( RT_DCOS,        "IF@DCOS",     OP_COS,     FD,   RL_8,     RL_,      RL_8 )
PICK( RT_DSIN,        "IF@DSIN",     OP_SIN,     FD,   RL_8,     RL_,      RL_8 )
PICK( RT_DTAN,        "IF@DTAN",     OP_TAN,     FD,   RL_8,     RL_,      RL_8 )
PICK( RT_DSQRT,       "IF@DSQRT",    OP_SQRT,    FD,   RL_8,     RL_,      RL_8 )
PICK( RT_DFABS,       "IF@DFABS",    OP_FABS,    FD,   RL_8,     RL_,      RL_8 )
PICK( RT_DACOS,       "IF@DACOS",    OP_ACOS,    FD,   RL_8,     RL_,      RL_8 )
PICK( RT_DASIN,       "IF@DASIN",    OP_ASIN,    FD,   RL_8,     RL_,      RL_8 )
PICK( RT_DATAN,       "IF@DATAN",    OP_ATAN,    FD,   RL_8,     RL_,      RL_8 )
PICK( RT_DCOSH,       "IF@DCOSH",    OP_COSH,    FD,   RL_8,     RL_,      RL_8 )
PICK( RT_DEXP,        "IF@DEXP",     OP_EXP,     FD,   RL_8,     RL_,      RL_8 )
PICK( RT_DLOG10,      "IF@DLOG10",   OP_LOG10,   FD,   RL_8,     RL_,      RL_8 )
PICK( RT_DSINH,       "IF@DSINH",    OP_SINH,    FD,   RL_8,     RL_,      RL_8 )
PICK( RT_DTANH,       "IF@DTANH",    OP_TANH,    FD,   RL_8,     RL_,      RL_8 )

PICK( RT_ATAN2,       "IF@ATAN2",    OP_ATAN2,   FS,   RL_DX_AX, RL_CX_BX, RL_DX_AX )
PICK( RT_FMOD,        "IF@FMOD",     OP_FMOD,    FS,   RL_DX_AX, RL_CX_BX, RL_DX_AX )
PICK( RT_LOG,         "IF@LOG",      OP_LOG,     FS,   RL_DX_AX, RL_,      RL_DX_AX )
PICK( RT_COS,         "IF@COS",      OP_COS,     FS,   RL_DX_AX, RL_,      RL_DX_AX )
PICK( RT_SIN,         "IF@SIN",      OP_SIN,     FS,   RL_DX_AX, RL_,      RL_DX_AX )
PICK( RT_TAN,         "IF@TAN",      OP_TAN,     FS,   RL_DX_AX, RL_,      RL_DX_AX )
PICK( RT_SQRT,        "IF@SQRT",     OP_SQRT,    FS,   RL_DX_AX, RL_,      RL_DX_AX )
PICK( RT_FABS,        "IF@FABS",     OP_FABS,    FS,   RL_DX_AX, RL_,      RL_DX_AX )
PICK( RT_ACOS,        "IF@ACOS",     OP_ACOS,    FS,   RL_DX_AX, RL_,      RL_DX_AX )
PICK( RT_ASIN,        "IF@ASIN",     OP_ASIN,    FS,   RL_DX_AX, RL_,      RL_DX_AX )
PICK( RT_ATAN,        "IF@ATAN",     OP_ATAN,    FS,   RL_DX_AX, RL_,      RL_DX_AX )
PICK( RT_COSH,        "IF@COSH",     OP_COSH,    FS,   RL_DX_AX, RL_,      RL_DX_AX )
PICK( RT_EXP,         "IF@EXP",      OP_EXP,     FS,   RL_DX_AX, RL_,      RL_DX_AX )
PICK( RT_LOG10,       "IF@LOG10",    OP_LOG10,   FS,   RL_DX_AX, RL_,      RL_DX_AX )
PICK( RT_SINH,        "IF@SINH",     OP_SINH,    FS,   RL_DX_AX, RL_,      RL_DX_AX )
PICK( RT_TANH,        "IF@TANH",     OP_TANH,    FS,   RL_DX_AX, RL_,      RL_DX_AX )

PICK( RT_BUGLIST,     "__chipbug",   OP_NOP,     0,    RL_,      RL_,      RL_ )
PICK( RT_FDIV_MEM32,  "__fdiv_m32",  OP_NOP,     0,    RL_,      RL_,      RL_ )
PICK( RT_FDIV_MEM64,  "__fdiv_m64",  OP_NOP,     0,    RL_,      RL_,      RL_ )
PICK( RT_FDIV_MEM32R, "__fdiv_m32r", OP_NOP,     0,    RL_,      RL_,      RL_ )
PICK( RT_FDIV_MEM64R, "__fdiv_m64r", OP_NOP,     0,    RL_,      RL_,      RL_ )
PICK( RT_FDIV_FPREG,  "__fdiv_fpr",  OP_NOP,     0,    RL_,      RL_,      RL_ )

PICK( RT_NOP,         "__NOP",       OP_NOP,     0,    RL_,      RL_,      RL_ )
