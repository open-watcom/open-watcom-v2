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
* Description:  Reduction tables for x87 instructions.
*
****************************************************************************/


#include "standard.h"
#include "coderep.h"
#include "model.h"
#include "opcodes.h"
#include "pattern.h"
#include "regset.h"
#include "vergen.h"
#include "funits.h"


opcode_entry    Move87L[] = {
/*       op    res   eq        verify       gen            reg          fu*/
{_UnPP(  R|M|U,R|M|U,EQ_R1 ),  V_NO,        G_NO,          RG_,         FU_NO},
{_UnPP(  ANY,  ANY,  NONE ),   V_NO,        G_UNKNOWN,     RG_8087_NEED,FU_NO},
};

opcode_entry    Move87D[] = {
/***********************************/
/*       op    res   eq        verify       gen            reg          fu*/
{_UnPP(  R|M|U,R|M|U,EQ_R1 ),  V_NO,        G_NO,          RG_,         FU_NO},
{_UnPP(  M,    M,    NONE  ),  V_SAME_LOCN, G_NO,          RG_,         FU_NO},
#if _TARGET & _TARG_80386
{_UnPP(  M,    M,    NONE  ),  V_SAME_TYPE, R_SPLITMOVE,   RG_,         FU_NO},
{_UnPP(  M|C,  R,    NONE  ),  V_NO,        R_SPLITMOVE,   RG_8,        FU_NO},
{_UnPP(  R,    M,    NONE  ),  V_NO,        R_SPLITMOVE,   RG_8,        FU_NO},
{_UnPP(  C,    M,    NONE  ),  V_SAME_TYPE, R_SPLITMOVE,   RG_8,        FU_NO},
#else
{_UnPP(  M,    M,    NONE  ),  V_SAME_TYPE, R_SPLIT8,      RG_,         FU_NO},
{_UnPP(  M|C,  R,    NONE  ),  V_NO,        R_SPLIT8,      RG_8,        FU_NO},
{_UnPP(  R,    M,    NONE  ),  V_NO,        R_SPLIT8,      RG_8,        FU_NO},
{_UnPP(  C,    M,    NONE  ),  V_SAME_TYPE, R_SPLIT8,      RG_8,        FU_NO},
#endif
{_UnPP(  U,    M,    NONE ),   V_NO,        G_UNKNOWN,     RG_8087_NEED,FU_NO},
{_UnPP(  R,    M,    NONE ),   V_NO,        G_UNKNOWN,     RG_ST0_STI,  FU_NO},
{_UnPP(  M|C,  U,    NONE ),   V_NO,        G_UNKNOWN,     RG_8087_NEED,FU_NO},
{_UnPP(  M,    R,    NONE ),   V_NO,        G_UNKNOWN,     RG_STI_ST0,  FU_NO},
{_UnPP(  R,    ANY,  NONE ),   V_NO,        R_MOVOP1MEM,   RG_8_8087,   FU_NO},
{_UnPP(  ANY,  R,    NONE ),   V_NO,        R_MOVRESMEM,   RG_8087_8,   FU_NO},
{_UnPP(  C,    ANY,  NONE ),   V_OP1ZERO,   G_UNKNOWN,     RG_8087,     FU_NO},
{_UnPP(  C,    ANY,  NONE ),   V_OP1ONE,    G_UNKNOWN,     RG_8087,     FU_NO},
{_UnPP(  C,    ANY,  NONE ),   V_NO,        R_FORCEOP1CMEM,RG_8087,     FU_NO},
{_UnPP(  M,    R,    NONE ),   V_NO,        G_UNKNOWN,     RG_8087,     FU_NO},
{_UnPP(  R,    M,    NONE ),   V_NO,        G_UNKNOWN,     RG_8087,     FU_NO},
{_UnPP(  R|M,  ANY,  NONE ),   V_NO,        G_UNKNOWN,     RG_8087_NEED,FU_NO},
{_UnPP(  R,    R,    NONE ),   V_NO,        G_UNKNOWN,     RG_ST0_STI,  FU_NO},
{_UnPP(  ANY,  ANY,  NONE ),   V_NO,        G_UNKNOWN,     RG_8087_NEED,FU_NO},
};

opcode_entry    Move87S[] = {
/***********************************/
/*       op    res   eq        verify       gen            reg          fu*/
{_UnPP(  R|M|U,R|M|U,EQ_R1 ),  V_NO,        G_NO,          RG_,         FU_NO},
{_UnPP(  M,    M,    NONE  ),  V_SAME_LOCN, G_NO,          RG_,         FU_NO},
#if _TARGET & _TARG_80386
{_UnPP(  M,    M,    NONE  ),  V_SAME_TYPE, R_MOVOP1REG,   RG_DBL,      FU_NO},
{_UnPP(  M|C,  R,    NONE  ),  V_NO,        R_EXT_PUSHC,   RG_DBL,      FU_NO},
{_UnPP(  R|C,  M,    NONE  ),  V_NO,        R_EXT_PUSHC,   RG_DBL,      FU_NO},
#else
{_UnPP(  M,    M,    NONE  ),  V_SAME_TYPE, R_SPLITUNARY,  RG_DOUBLE,   FU_NO},
{_UnPP(  M|C,  R,    NONE  ),  V_NO,        R_SPLITUNARY,  RG_DOUBLE,   FU_NO},
{_UnPP(  R,    M,    NONE  ),  V_NO,        R_SPLITUNARY,  RG_DOUBLE,   FU_NO},
{_UnPP(  C,    M,    NONE  ),  V_SAME_TYPE, R_SPLITUNARY,  RG_DOUBLE,   FU_NO},
#endif
{_UnPP(  U,    M,    NONE ),   V_NO,        G_UNKNOWN,     RG_8087_NEED,FU_NO},
{_UnPP(  R,    M,    NONE ),   V_NO,        G_UNKNOWN,     RG_ST0_STI,  FU_NO},
{_UnPP(  M|C,  U,    NONE ),   V_NO,        G_UNKNOWN,     RG_8087_NEED,FU_NO},
{_UnPP(  M,    R,    NONE ),   V_NO,        G_UNKNOWN,     RG_STI_ST0,  FU_NO},
{_UnPP(  R,    ANY,  NONE ),   V_NO,        R_MOVOP1MEM,   RG_DBL_8087, FU_NO},
{_UnPP(  ANY,  R,    NONE ),   V_NO,        R_MOVRESMEM,   RG_8087_DBL, FU_NO},
{_UnPP(  C,    ANY,  NONE ),   V_OP1ZERO,   G_UNKNOWN,     RG_8087,     FU_NO},
{_UnPP(  C,    ANY,  NONE ),   V_OP1ONE,    G_UNKNOWN,     RG_8087,     FU_NO},
{_UnPP(  C,    ANY,  NONE ),   V_NO,        R_FORCEOP1CMEM,RG_8087,     FU_NO},
{_UnPP(  M,    R,    NONE ),   V_NO,        G_UNKNOWN,     RG_8087,     FU_NO},
{_UnPP(  R,    M,    NONE ),   V_NO,        G_UNKNOWN,     RG_8087,     FU_NO},
{_UnPP(  R|M,  ANY,  NONE ),   V_NO,        G_UNKNOWN,     RG_8087_NEED,FU_NO},
{_UnPP(  R,    R,    NONE ),   V_NO,        G_UNKNOWN,     RG_ST0_STI,  FU_NO},
{_UnPP(  ANY,  ANY,  NONE ),   V_NO,        G_UNKNOWN,     RG_8087_NEED,FU_NO},
};

opcode_entry    Un87[] = {
/***************************/
/*       op    res   eq        verify       gen            reg             fu*/
{_UnPP(  M,    M,    EQ_R1 ),  V_NEGATE,    R_FLIPSIGN,    RG_8087_ZAP_ACC,FU_NO},
{_UnPP(  C,    ANY,  NONE ),   V_OP1ZERO,   G_UNKNOWN,     RG_8087,        FU_NO},
{_UnPP(  C,    ANY,  NONE ),   V_OP1ONE,    G_UNKNOWN,     RG_8087,        FU_NO},
{_UnPP(  C,    ANY,  NONE ),   V_NO,        R_FORCEOP1CMEM,RG_8087,        FU_NO},
{_UnPP(  ANY,  ANY,  NONE ),   V_NO,        G_UNKNOWN,     RG_8087,        FU_NO},
};

opcode_entry    Un87Func[] = {
/****************************/
/*       op    res   eq        verify       gen            reg             fu*/
{_Un(    C,    ANY,  NONE ),   V_OP1ZERO,   G_UNKNOWN,     RG_8087_ZAP_ACC,FU_NO},
{_Un(    C,    ANY,  NONE ),   V_OP1ONE,    G_UNKNOWN,     RG_8087_ZAP_ACC,FU_NO},
{_Un(    C,    ANY,  NONE ),   V_NO,        R_FORCEOP1CMEM,RG_8087_ZAP_ACC,FU_NO},
{_Un(    ANY,  ANY,  NONE ),   V_NO,        G_UNKNOWN,     RG_8087_ZAP_ACC,FU_NO},
};

opcode_entry    Push87S[] = {
/*****************************/
/*       op    res   eq        verify       gen            reg             fu*/
#if _TARGET & _TARG_IAPX86
{_Un(    R|M|C,ANY,  NONE ),   V_NO,        R_SPLITUNARY,  RG_DBL_OR_PTR,  FU_NO},
#else
{_UnPP(  R,    ANY,  NONE ),   V_NO,        G_WORDR1,      RG_DBL,         FU_FOP},
{_UnPP(  M,    ANY,  NONE ),   V_NO,        G_M1,          RG_,            FU_FOP},
{_UnPP(  C,    ANY,  NONE ),   V_NO,        G_PUSHFS,      RG_,            FU_FOP},
#endif
{_Un(    ANY,  ANY,  NONE ),   V_NO,        G_UNKNOWN,     RG_8087,        FU_NO},
};

opcode_entry    Push87D[] = {
/*****************************/
/*       op    res   eq        verify       gen            reg       fu*/
#if _TARGET & _TARG_IAPX86
{_Un(    R|M|C,ANY,  NONE ),   V_NO,        R_SPLIT8,      RG_8,     FU_NO},
#else
{_Un(    R|M|C,ANY,  NONE ),   V_NO,        R_SPLITUNARY,  RG_8,     FU_NO},
#endif
{_Un(    ANY,  ANY,  NONE ),   V_NO,        G_UNKNOWN,     RG_8087,  FU_NO},
};

opcode_entry    Bin87[] = {
/***************************/
/*       op1   op2   res  eq        verify       gen               reg     fu*/
{_BinPP( C,    R|M|U,ANY, NONE ),   V_OP1ONE,    G_UNKNOWN,        RG_8087,FU_NO},
{_BinPP( C,    R|M|U,ANY, NONE ),   V_OP1ZERO,   G_UNKNOWN,        RG_8087,FU_NO},
{_BinPP( C,    ANY,  ANY, NONE ),   V_NO,        R_FORCEOP1CMEM,   RG_8087,FU_NO},
{_BinPP( ANY,  C,    ANY, NONE ),   V_OP2ONE,    G_UNKNOWN,        RG_8087,FU_NO},
{_BinPP( ANY,  C,    ANY, NONE ),   V_OP2ZERO,   G_UNKNOWN,        RG_8087,FU_NO},
{_BinPP( ANY,  C,    ANY, NONE ),   V_NO,        R_FORCEOP2CMEM,   RG_8087,FU_NO},
{_BinPP( ANY,  ANY,  ANY, NONE ),   V_NO,        G_UNKNOWN,        RG_8087,FU_NO},
};

opcode_entry    Bin87Func[] = {
/*****************************/
/*       op1   op2   res  eq      verify     gen               reg             fu*/
{_Bin(   C,    R|M|U,ANY, NONE ), V_OP1ONE,  G_UNKNOWN,        RG_8087_ZAP_ACC,FU_NO},
{_Bin(   C,    R|M|U,ANY, NONE ), V_OP1ZERO, G_UNKNOWN,        RG_8087_ZAP_ACC,FU_NO},
{_Bin(   C,    ANY,  ANY, NONE ), V_NO,      R_FORCEOP1CMEM,   RG_8087_ZAP_ACC,FU_NO},
{_Bin(   ANY,  C,    ANY, NONE ), V_OP2ONE,  G_UNKNOWN,        RG_8087_ZAP_ACC,FU_NO},
{_Bin(   ANY,  C,    ANY, NONE ), V_OP2ZERO, G_UNKNOWN,        RG_8087_ZAP_ACC,FU_NO},
{_Bin(   ANY,  C,    ANY, NONE ), V_NO,      R_FORCEOP2CMEM,   RG_8087_ZAP_ACC,FU_NO},
{_Bin(   ANY,  ANY,  ANY, NONE ), V_NO,      G_UNKNOWN,        RG_8087_ZAP_ACC,FU_NO},
};

opcode_entry    Cmp87[] = {
/***************************/
/*       op1   op2       verify       gen               reg                fu*/
{_SidCC( C,    ANY  ),   V_OP1ONE,    G_UNKNOWN,        RG_8087_ZAP_ACC,   FU_NO},
{_SidCC( C,    ANY  ),   V_OP1ZERO,   G_UNKNOWN,        RG_8087_ZAP_ACC,   FU_NO},
{_SidCC( C,    ANY  ),   V_NO,        R_FORCEOP1CMEM,   RG_8087_ZAP_ACC,   FU_NO},
{_SidCC( M,    C    ),   V_INTCOMP,   R_INTCOMP,        RG_8087_ZAP_ACC,   FU_NO},
{_SidCC( ANY,  C    ),   V_OP2ONE,    G_UNKNOWN,        RG_8087_ZAP_ACC,   FU_NO},
{_SidCC( ANY,  C    ),   V_OP2ZERO,   G_UNKNOWN,        RG_8087_ZAP_ACC,   FU_NO},
{_SidCC( ANY,  C    ),   V_NO,        R_FORCEOP2CMEM,   RG_8087_ZAP_ACC,   FU_NO},
{_SidCC( ANY,  ANY  ),   V_NO,        G_UNKNOWN,        RG_8087_ZAP_ACC,   FU_NO},
};
