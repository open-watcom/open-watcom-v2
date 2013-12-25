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


#include "cgstd.h"
#include "coderep.h"
#include "model.h"
#include "pattern.h"
#include "regset.h"


opcode_entry    Move87L[] = {
/*           from  to    eq            verify          reg           gen             fu  */
_OE( _UnPP(  R|M|U,R|M|U,EQ_R1 ),      V_NO,           RG_,          G_NO,           FU_NO ),
_OE( _UnPP(  ANY,  ANY,  NONE ),       V_NO,           RG_8087_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

opcode_entry    Move87D[] = {
/***********************************/
/*           from  to    eq            verify          reg           gen             fu  */
_OE( _UnPP(  R|M|U,R|M|U,EQ_R1 ),      V_NO,           RG_,          G_NO,           FU_NO ),
_OE( _UnPP(  M,    M,    NONE  ),      V_SAME_LOCN,    RG_,          G_NO,           FU_NO ),
#if _TARGET & _TARG_IAPX86
_OE( _UnPP(  M,    M,    NONE  ),      V_SAME_TYPE,    RG_,          R_SPLIT8,       FU_NO ),
_OE( _UnPP(  M|C,  R,    NONE  ),      V_NO,           RG_8,         R_SPLIT8,       FU_NO ),
_OE( _UnPP(  R,    M,    NONE  ),      V_NO,           RG_8,         R_SPLIT8,       FU_NO ),
_OE( _UnPP(  C,    M,    NONE  ),      V_SAME_TYPE,    RG_8,         R_SPLIT8,       FU_NO ),
#else
_OE( _UnPP(  M,    M,    NONE  ),      V_SAME_TYPE,    RG_,          R_SPLITMOVE,    FU_NO ),
_OE( _UnPP(  M|C,  R,    NONE  ),      V_NO,           RG_8,         R_SPLITMOVE,    FU_NO ),
_OE( _UnPP(  R,    M,    NONE  ),      V_NO,           RG_8,         R_SPLITMOVE,    FU_NO ),
_OE( _UnPP(  C,    M,    NONE  ),      V_SAME_TYPE,    RG_8,         R_SPLITMOVE,    FU_NO ),
#endif
_OE( _UnPP(  U,    M,    NONE ),       V_NO,           RG_8087_NEED, G_UNKNOWN,      FU_NO ),
_OE( _UnPP(  R,    M,    NONE ),       V_NO,           RG_ST0_STI,   G_UNKNOWN,      FU_NO ),
_OE( _UnPP(  M|C,  U,    NONE ),       V_NO,           RG_8087_NEED, G_UNKNOWN,      FU_NO ),
_OE( _UnPP(  M,    R,    NONE ),       V_NO,           RG_STI_ST0,   G_UNKNOWN,      FU_NO ),
_OE( _UnPP(  R,    ANY,  NONE ),       V_NO,           RG_8_8087,    R_MOVOP1MEM,    FU_NO ),
_OE( _UnPP(  ANY,  R,    NONE ),       V_NO,           RG_8087_8,    R_MOVRESMEM,    FU_NO ),
_OE( _UnPP(  C,    ANY,  NONE ),       V_OP1ZERO,      RG_8087,      G_UNKNOWN,      FU_NO ),
_OE( _UnPP(  C,    ANY,  NONE ),       V_OP1ONE,       RG_8087,      G_UNKNOWN,      FU_NO ),
_OE( _UnPP(  C,    ANY,  NONE ),       V_NO,           RG_8087,      R_FORCEOP1CMEM, FU_NO ),
_OE( _UnPP(  M,    R,    NONE ),       V_NO,           RG_8087,      G_UNKNOWN,      FU_NO ),
_OE( _UnPP(  R,    M,    NONE ),       V_NO,           RG_8087,      G_UNKNOWN,      FU_NO ),
_OE( _UnPP(  R|M,  ANY,  NONE ),       V_NO,           RG_8087_NEED, G_UNKNOWN,      FU_NO ),
_OE( _UnPP(  R,    R,    NONE ),       V_NO,           RG_ST0_STI,   G_UNKNOWN,      FU_NO ),
_OE( _UnPP(  ANY,  ANY,  NONE ),       V_NO,           RG_8087_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

opcode_entry    Move87S[] = {
/***********************************/
/*           from  to    eq            verify          reg           gen             fu  */
_OE( _UnPP(  R|M|U,R|M|U,EQ_R1 ),      V_NO,           RG_,          G_NO,           FU_NO ),
_OE( _UnPP(  M,    M,    NONE  ),      V_SAME_LOCN,    RG_,          G_NO,           FU_NO ),
#if _TARGET & _TARG_IAPX86
_OE( _UnPP(  M,    M,    NONE  ),      V_SAME_TYPE,    RG_DOUBLE,    R_SPLITUNARY,   FU_NO ),
_OE( _UnPP(  M|C,  R,    NONE  ),      V_NO,           RG_DOUBLE,    R_SPLITUNARY,   FU_NO ),
_OE( _UnPP(  R,    M,    NONE  ),      V_NO,           RG_DOUBLE,    R_SPLITUNARY,   FU_NO ),
_OE( _UnPP(  C,    M,    NONE  ),      V_SAME_TYPE,    RG_DOUBLE,    R_SPLITUNARY,   FU_NO ),
#else
_OE( _UnPP(  M,    M,    NONE  ),      V_SAME_TYPE,    RG_DBL,       R_MOVOP1REG,    FU_NO ),
_OE( _UnPP(  M|C,  R,    NONE  ),      V_NO,           RG_DBL,       R_EXT_PUSHC,    FU_NO ),
_OE( _UnPP(  R|C,  M,    NONE  ),      V_NO,           RG_DBL,       R_EXT_PUSHC,    FU_NO ),
#endif
_OE( _UnPP(  U,    M,    NONE ),       V_NO,           RG_8087_NEED, G_UNKNOWN,      FU_NO ),
_OE( _UnPP(  R,    M,    NONE ),       V_NO,           RG_ST0_STI,   G_UNKNOWN,      FU_NO ),
_OE( _UnPP(  M|C,  U,    NONE ),       V_NO,           RG_8087_NEED, G_UNKNOWN,      FU_NO ),
_OE( _UnPP(  M,    R,    NONE ),       V_NO,           RG_STI_ST0,   G_UNKNOWN,      FU_NO ),
_OE( _UnPP(  R,    ANY,  NONE ),       V_NO,           RG_DBL_8087,  R_MOVOP1MEM,    FU_NO ),
_OE( _UnPP(  ANY,  R,    NONE ),       V_NO,           RG_8087_DBL,  R_MOVRESMEM,    FU_NO ),
_OE( _UnPP(  C,    ANY,  NONE ),       V_OP1ZERO,      RG_8087,      G_UNKNOWN,      FU_NO ),
_OE( _UnPP(  C,    ANY,  NONE ),       V_OP1ONE,       RG_8087,      G_UNKNOWN,      FU_NO ),
_OE( _UnPP(  C,    ANY,  NONE ),       V_NO,           RG_8087,      R_FORCEOP1CMEM, FU_NO ),
_OE( _UnPP(  M,    R,    NONE ),       V_NO,           RG_8087,      G_UNKNOWN,      FU_NO ),
_OE( _UnPP(  R,    M,    NONE ),       V_NO,           RG_8087,      G_UNKNOWN,      FU_NO ),
_OE( _UnPP(  R|M,  ANY,  NONE ),       V_NO,           RG_8087_NEED, G_UNKNOWN,      FU_NO ),
_OE( _UnPP(  R,    R,    NONE ),       V_NO,           RG_ST0_STI,   G_UNKNOWN,      FU_NO ),
_OE( _UnPP(  ANY,  ANY,  NONE ),       V_NO,           RG_8087_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

opcode_entry    Un87[] = {
/***************************/
/*           from  to    eq            verify          reg           gen             fu  */
_OE( _UnPP(  M,    M,    EQ_R1 ),      V_NEGATE,       RG_8087_ZAP_ACC,R_FLIPSIGN,   FU_NO ),
_OE( _UnPP(  C,    ANY,  NONE ),       V_OP1ZERO,      RG_8087,      G_UNKNOWN,      FU_NO ),
_OE( _UnPP(  C,    ANY,  NONE ),       V_OP1ONE,       RG_8087,      G_UNKNOWN,      FU_NO ),
_OE( _UnPP(  C,    ANY,  NONE ),       V_NO,           RG_8087,      R_FORCEOP1CMEM, FU_NO ),
_OE( _UnPP(  ANY,  ANY,  NONE ),       V_NO,           RG_8087,      G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

opcode_entry    Un87Func[] = {
/****************************/
/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    C,    ANY,  NONE ),       V_OP1ZERO,      RG_8087_ZAP_ACC,G_UNKNOWN,    FU_NO ),
_OE( _Un(    C,    ANY,  NONE ),       V_OP1ONE,       RG_8087_ZAP_ACC,G_UNKNOWN,    FU_NO ),
_OE( _Un(    C,    ANY,  NONE ),       V_NO,           RG_8087_ZAP_ACC,R_FORCEOP1CMEM,FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_8087_ZAP_ACC,G_UNKNOWN,    FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

opcode_entry    Push87S[] = {
/*****************************/
/*           from  to    eq            verify          reg           gen             fu  */
#if _TARGET & _TARG_IAPX86
_OE( _Un(    R|M|C,ANY,  NONE ),       V_NO,           RG_DBL_OR_PTR,R_SPLITUNARY,   FU_NO ),
#else
_OE( _UnPP(  R,    ANY,  NONE ),       V_NO,           RG_DBL,       G_WORDR1,       FU_FOP ),
_OE( _UnPP(  M,    ANY,  NONE ),       V_NO,           RG_,          G_M1,           FU_FOP ),
_OE( _UnPP(  C,    ANY,  NONE ),       V_NO,           RG_,          G_PUSHFS,       FU_FOP ),
#endif
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_8087,      G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

opcode_entry    Push87D[] = {
/*****************************/
/*           from  to    eq            verify          reg           gen             fu  */
#if _TARGET & _TARG_IAPX86
_OE( _Un(    R|M|C,ANY,  NONE ),       V_NO,           RG_8,         R_SPLIT8,       FU_NO ),
#else
_OE( _Un(    R|M|C,ANY,  NONE ),       V_NO,           RG_8,         R_SPLITUNARY,   FU_NO ),
#endif
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_8087,      G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

opcode_entry    Bin87[] = {
/***************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _BinPP( C,    R|M|U,ANY,  NONE ), V_OP1ONE,       RG_8087,      G_UNKNOWN,      FU_NO ),
_OE( _BinPP( C,    R|M|U,ANY,  NONE ), V_OP1ZERO,      RG_8087,      G_UNKNOWN,      FU_NO ),
_OE( _BinPP( C,    ANY,  ANY,  NONE ), V_NO,           RG_8087,      R_FORCEOP1CMEM, FU_NO ),
_OE( _BinPP( ANY,  C,    ANY,  NONE ), V_OP2ONE,       RG_8087,      G_UNKNOWN,      FU_NO ),
_OE( _BinPP( ANY,  C,    ANY,  NONE ), V_OP2ZERO,      RG_8087,      G_UNKNOWN,      FU_NO ),
_OE( _BinPP( ANY,  C,    ANY,  NONE ), V_NO,           RG_8087,      R_FORCEOP2CMEM, FU_NO ),
_OE( _BinPP( ANY,  ANY,  ANY,  NONE ), V_NO,           RG_8087,      G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

opcode_entry    Bin87Func[] = {
/*****************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   C,    R|M|U,ANY,  NONE ), V_OP1ONE,       RG_8087_ZAP_ACC,G_UNKNOWN,    FU_NO ),
_OE( _Bin(   C,    R|M|U,ANY,  NONE ), V_OP1ZERO,      RG_8087_ZAP_ACC,G_UNKNOWN,    FU_NO ),
_OE( _Bin(   C,    ANY,  ANY,  NONE ), V_NO,           RG_8087_ZAP_ACC,R_FORCEOP1CMEM,FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  NONE ), V_OP2ONE,       RG_8087_ZAP_ACC,G_UNKNOWN,    FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  NONE ), V_OP2ZERO,      RG_8087_ZAP_ACC,G_UNKNOWN,    FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  NONE ), V_NO,           RG_8087_ZAP_ACC,R_FORCEOP2CMEM,FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_8087_ZAP_ACC,G_UNKNOWN,    FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

opcode_entry    Cmp87[] = {
/***************************/
/*           op1   op2                 verify          reg           gen             fu  */
_OE( _SidCC( C,    ANY  ),             V_OP1ONE,       RG_8087_ZAP_ACC,G_UNKNOWN,    FU_NO ),
_OE( _SidCC( C,    ANY  ),             V_OP1ZERO,      RG_8087_ZAP_ACC,G_UNKNOWN,    FU_NO ),
_OE( _SidCC( C,    ANY  ),             V_NO,           RG_8087_ZAP_ACC,R_FORCEOP1CMEM,FU_NO ),
_OE( _SidCC( M,    C    ),             V_INTCOMP,      RG_8087_ZAP_ACC,R_INTCOMP,    FU_NO ),
_OE( _SidCC( ANY,  C    ),             V_OP2ONE,       RG_8087_ZAP_ACC,G_UNKNOWN,    FU_NO ),
_OE( _SidCC( ANY,  C    ),             V_OP2ZERO,      RG_8087_ZAP_ACC,G_UNKNOWN,    FU_NO ),
_OE( _SidCC( ANY,  C    ),             V_NO,           RG_8087_ZAP_ACC,R_FORCEOP2CMEM,FU_NO ),
_OE( _SidCC( ANY,  ANY  ),             V_NO,           RG_8087_ZAP_ACC,G_UNKNOWN,    FU_NO ),
_OE( _Side(  ANY,  ANY  ),             V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};
