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


#include "cgstd.h"
#include "coderep.h"
#include "vergen.h"
#include "pattern.h"
#include "funits.h"
#include "regset.h"
#include "model.h"
#include "tables.h"

#include "s37table.def"

/**************************************************************************/
/**************************************************************************/
/*                             ARITH                                      */
/**************************************************************************/
/**************************************************************************/

#define _BinCommutes( type ) \
_Bin(   R,    ANY,  R,    EQ_R1 ), V_NO,        G_UNKNOWN,      RG_##type,FU_NO,\
_Bin(   R|M|C,R,    R,    EQ_R2 ), V_NO,        R_SWAPOPS,      RG_##type,FU_NO,\
_Bin(   R,    ANY,  ANY,  NONE ),  V_SWAP_GOOD, R_SWAPOPS,      RG_##type,FU_NO,\
_Bin(   M,    C,    R|M,  NONE ),  V_NO,        R_SWAPOPS,      RG_##type,FU_NO,\
_Bin(   R|M|C,R|M|C,R|M,  NONE ),  V_NO,        R_USEREGISTER,  RG_##type,FU_NO,\
_Bin(   ANY,  ANY,  ANY,  NONE ),  V_NO,        G_UNKNOWN,      RG_##type##_NEED,FU_NO

#define _BinNoCommute( type ) \
_Bin(   R,    C,    R,    EQ_R1 ), V_NO,        R_LOADOP2,      RG_##type,FU_NO,\
_Bin(   R|M|C,R|M|C,R|M,  NONE ),  V_NO,        R_USEREGISTER,  RG_##type,FU_NO,\
_Bin(   ANY,  ANY,  ANY,  NONE ),  V_NO,        G_UNKNOWN,      RG_##type##_NEED,FU_NO

#define _GenBinary( op, type, cc ) \
_Bin##cc( R,  R,    R,    EQ_R1),  V_NO,        G_##op##R,       RG_##type,FU_NO,\
_Bin##cc( R,  M|C,  R,    EQ_R1),  V_NO,        G_##op,          RG_##type,FU_NO

#define _GenHalf( op ) \
_BinCC( R,    M,    R,    EQ_R1),  V_OP2I2,     G_##op##H,       RG_WORDOP1,FU_NO,\
_BinCC( R,    C,    R,    EQ_R1),  V_OP2I2CON,  G_##op##H,       RG_WORD,FU_NO

#define _MoveOp1IfOp2Zero \
_Bin(   ANY,  C,    ANY,  NONE),   V_OP2ZERO,   R_MAKEMOVE,      RG_,FU_NO

#define _MoveOp1IfOp2One \
_Bin(   ANY,  C,    ANY,  NONE),   V_OP2ONE,    R_MAKEMOVE,      RG_,FU_NO

#define _MoveZeroIfOp2Zero \
_Bin(   ANY,  C,    ANY,  NONE),   V_OP2ZERO,   R_MOVOP2,        RG_,FU_NO

#define _NegIfOp1Zero \
_Bin(   C,    ANY,  ANY,  NONE),   V_OP1ZERO,   R_MAKENEG,       RG_,FU_NO

#define _SubIfOp2Neg \
_Bin(   ANY,  C,    ANY,  NONE),   V_OP2NEG,    R_MAKESUB,       RG_,FU_NO

#define _AddIfOp2Neg \
_Bin(   ANY,  C,    ANY,  NONE),   V_OP2NEG,    R_MAKEADD,       RG_,FU_NO

/**************************************************************************/
/*                             ADD                                        */
/**************************************************************************/

#define _OptimizeAdd \
    _SubIfOp2Neg,\
    _MoveOp1IfOp2Zero

#define _AddUnsigned \
    _OptimizeAdd,\
    _GenBinary( AL, WORD, CC ),\
    _BinCommutes( WORD )

static  opcode_entry    AddPT[] = {
_Bin(   R,    C,    R,    NONE  ), V_LA2,      G_LA2,        RG_WORD,FU_NO,
_AddUnsigned
};

static  opcode_entry    AddU4[] = {
/************************/
_AddUnsigned
};

static  opcode_entry    AddI4[] = {
/************************/
_OptimizeAdd,
_Bin(   ANY,  M|U,  ANY,  NONE ),  V_OP2I2,        R_PREPI4I2, RG_,FU_NO,
_Bin(   ANY,  C,    ANY,  NONE ),  V_OP2I2CON,     R_PREPI4I2, RG_,FU_NO,
_GenBinary( A, WORD, CC ),
_BinCommutes( WORD )
};

extern  opcode_entry    Add4I2G[] = {
/************************/
_BinPP( R,    M,    R,    EQ_R1),  V_NO,           G_AH,         RG_WORDRES,FU_NO,
_BinPP( R,    C,    R,    EQ_R1),  V_NO,           G_AH,         RG_WORDRES,FU_NO,
_Bin(   R|M|C,M|C,  R|M,  NONE ),  V_NO,           R_USEREGISTER,RG_WORDRES,FU_NO,
_Bin(   ANY,  ANY,  ANY,  NONE ),  V_NO,           G_UNKNOWN,    RG_WORDRES_NEED,FU_NO
};

static  opcode_entry    AddFS[] = {
/************************/
_OptimizeAdd,
_GenBinary( AE, SINGLE, CC ),
_BinCommutes( SINGLE )
};

static  opcode_entry    AddFD[] = {
/************************/
_OptimizeAdd,
_GenBinary( AD, DOUBLE, CC ),
_BinCommutes( DOUBLE )
};

/**************************************************************************/
/*                             SUB                                        */
/**************************************************************************/

#define _OptimizeSub \
    _AddIfOp2Neg,\
    _MoveOp1IfOp2Zero,\
    _NegIfOp1Zero

static  opcode_entry    SubI4[] = {
/************************/
_OptimizeSub,
_BinPP( R,    C,    R,    EQ_R1 ),  V_OP2ONE,      G_BCTR,       RG_WORD,FU_NO,
_Bin(   ANY,  M|U,  ANY,  NONE ),  V_OP2I2,        R_PREPI4I2, RG_,FU_NO,
_Bin(   ANY,  C,    ANY,  NONE ),  V_OP2I2CON,     R_PREPI4I2, RG_,FU_NO,
_GenBinary( S, WORD, CC ),
_BinNoCommute( WORD )
};

extern  opcode_entry    Sub4I2G[] = {
/************************/
_BinPP( R,    M,    R,    EQ_R1),  V_NO,           G_SH,         RG_WORDRES,FU_NO,
_BinPP( R,    C,    R,    EQ_R1),  V_NO,           G_SH,         RG_WORDRES,FU_NO,
_Bin(   R|M|C,M|C,  R|M,  NONE ),  V_NO,           R_USEREGISTER,RG_WORDRES,FU_NO,
_Bin(   ANY,  ANY,  ANY,  NONE ),  V_NO,           G_UNKNOWN,    RG_WORDRES_NEED,FU_NO
};

static  opcode_entry    SubU4[] = {
/************************/
_OptimizeSub,
_BinPP( R,    C,    R,    EQ_R1 ),  V_OP2ONE,      G_BCTR,       RG_WORD,FU_NO,
_GenBinary( SL, WORD, CC ),
_BinNoCommute( WORD )
};

static  opcode_entry    SubFS[] = {
/************************/
_OptimizeSub,
_GenBinary( SE, SINGLE, CC ),
_BinNoCommute( SINGLE )
};

static  opcode_entry    SubFD[] = {
/************************/
_OptimizeSub,
_GenBinary( SD, DOUBLE, CC ),
_BinNoCommute( DOUBLE )
};

/**************************************************************************/
/*                             MUL                                        */
/**************************************************************************/

#define _OptimizeMul \
    _MoveOp1IfOp2One,\
    _MoveZeroIfOp2Zero

static  opcode_entry    Mul4[] = {
/************************/
/*       op1   op2   res   eq      verify          gen           reg*/
_OptimizeMul,
_Bin(   ANY,  M|U,  ANY,  NONE ),  V_OP2I2,        R_PREPI4I2, RG_,FU_NO,
_Bin(   ANY,  C,    ANY,  NONE ),  V_OP2I2CON,     R_PREPI4I2, RG_,FU_NO,
_Bin(   ANY,  ANY,  ANY,  NONE ),  V_NO,           R_PREPMUL,    RG_,FU_NO
};

extern opcode_entry    Mul4I2G[] = {
/************************/
/*       op1   op2   res   eq      verify          gen           reg*/
_BinPP( R,    M,    R,    EQ_R1),  V_NO,           G_MH,         RG_WORDRES,FU_NO,
_BinPP( R,    C,    R,    EQ_R1),  V_NO,           G_MH,         RG_WORDRES,FU_NO,
_Bin(   R|M|C,M|C,  R|M,  NONE ),  V_NO,           R_USEREGISTER,RG_WORDRES,FU_NO,
_Bin(   ANY,  ANY,  ANY,  NONE ),  V_NO,           G_UNKNOWN,    RG_WORDRES_NEED,FU_NO
};

opcode_entry    Mul4G[] = {
/************************/
/*       op1   op2   res   eq      verify          gen           reg*/
_OptimizeMul,
_BinPP( R,    R,    R,    NONE ),  V_MULPAIR,      G_MR,         RG_MUL,FU_NO,
_BinPP( R,    M,    R,    NONE ),  V_MULPAIR,      G_M,          RG_MUL,FU_NO,
_Bin(   R,    C,    R,    NONE ),  V_MULPAIR,      R_LOADOP2,    RG_MUL,FU_NO,
_Bin(   ANY,  ANY,  ANY,  NONE ),  V_NO,           G_UNKNOWN,    RG_MUL_NEED,FU_NO
};

static  opcode_entry    MulFS[] = {
/************************/
/*       op1   op2   res   eq      verify          gen           reg*/
_OptimizeMul,
_GenBinary( ME, SINGLE, PP ),
_BinCommutes( SINGLE )
};

static  opcode_entry    MulFD[] = {
/************************/
/*       op1   op2   res   eq      verify          gen           reg*/
_OptimizeMul,
_GenBinary( MD, DOUBLE, PP ),
_BinCommutes( DOUBLE )
};


/**************************************************************************/
/*                             MOD/DIV                                    */
/**************************************************************************/

static  opcode_entry    Mod4[] = {
/************************/
/*       op1   op2   res   eq      verify          gen           reg*/
_Bin(   ANY,  ANY,  ANY,  NONE ),  V_NO,           R_PREPMOD,    RG_,FU_NO
};

static  opcode_entry    Div4[] = {
/************************/
/*       op1   op2   res   eq      verify          gen           reg*/
_MoveOp1IfOp2One,
_Bin(   ANY,  ANY,  ANY,  NONE ),  V_NO,           R_PREPDIV,    RG_,FU_NO
};

#define _DivMod4G \
_BinPP( R,    R,    R,    EQ_R1 ), V_NO,           G_DR,         RG_DIV,FU_NO,\
_BinPP( R,    M,    R,    EQ_R1 ), V_NO,           G_D,          RG_DIV,FU_NO,\
_Bin(   R,    C,    R,    EQ_R1 ), V_NO,           R_LOADOP2,    RG_DIV,FU_NO,\
_Bin(   ANY,  ANY,  ANY,  NONE ),  V_NO,           G_UNKNOWN,    RG_DIV_NEED,FU_NO

opcode_entry    Div4G[] = {
/************************/
/*       op1   op2   res   eq      verify          gen           reg*/
_Bin(   R,    C,    R,    EQ_R1 ), V_OP2POW2,      G_DIVPOW2,    RG_DIV,FU_NO,\
_DivMod4G,
};

opcode_entry    Mod4G[] = {
/************************/
/*       op1   op2   res   eq      verify          gen           reg*/
_DivMod4G,
};

static  opcode_entry    DivFS[] = {
/************************/
_MoveOp1IfOp2One,
_BinPP(   R,    C,    R,    NONE ), V_OP2TWO,      G_HER,  RG_SINGLE,FU_NO,\
_GenBinary( DE, SINGLE, PP ),
_BinNoCommute( SINGLE )
};

static  opcode_entry    DivFD[] = {
/************************/
_MoveOp1IfOp2One,
_BinPP(   R,    C,    R,    NONE ), V_OP2TWO,      G_HDR,  RG_DOUBLE,FU_NO,\
_GenBinary( DD, DOUBLE, PP ),
_BinNoCommute( DOUBLE )
};

/**************************************************************************/
/**************************************************************************/
/*                             SHIFTS                                     */
/**************************************************************************/
/**************************************************************************/

#define _Shift( op, cc ) \
_Bin##cc( R,  C,    R,    EQ_R1),  V_OP2SMALL,  G_##op,          RG_SHIFT,FU_NO,\
_Bin##cc( R,  R,    R,    EQ_R1),  V_NO,        G_##op,          RG_SHIFT,FU_NO,\
_Bin(   M,    R,    M,    EQ_R1 ), V_NO,        R_USEREGISTER,   RG_SHIFT,FU_NO,\
_Bin(   M,    C,    M,    EQ_R1 ), V_OP2SMALL,  R_USEREGISTER,   RG_SHIFT,FU_NO,\
_Bin(   ANY,  C,    ANY,  EQ_R1 ), V_OP2SMALL,  G_UNKNOWN,       RG_SHIFT_NEED,FU_NO,\
_Bin(   ANY,  R,    ANY,  EQ_R1 ), V_NO,        G_UNKNOWN,       RG_SHIFT_NEED,FU_NO,\
_Bin(   ANY,  ANY,  ANY,  EQ_R1 ), V_NO,        R_LOADOP2,       RG_SHIFT,FU_NO,\
_Bin(   R|M|C,R|M|C,R|M,  NONE ),  V_NO,        R_USEREGISTER,   RG_SHIFT,FU_NO,\
_Bin(   ANY,  ANY,  ANY,  NONE ),  V_NO,        G_UNKNOWN,       RG_SHIFT_NEED,FU_NO

static  opcode_entry    RShiftU4[] = {
/********************************/
_Shift( SRL, PP )
};

static  opcode_entry    RShiftI4[] = {
/********************************/
_Shift( SRA, CC )
};

static  opcode_entry    LShiftU4[] = {
/********************************/
_Shift( SLL, PP )
};

static  opcode_entry    LShiftI4[] = {
/********************************/
_Shift( SLL, PP )
};


/**************************************************************************/
/**************************************************************************/
/*                             BITWISE                                    */
/**************************************************************************/
/**************************************************************************/

#define _BitWise1( op ) \
/*       op1   op2   res   eq      verify          gen        reg*/\
_Bin(   M,    M,    M,    EQ_R1),  V_NO,           R_XC,      RG_,FU_NO,\
_BinSC( M,    C,    M,    EQ_R1),  V_NO,           G_##op##I, RG_,FU_NO,\
_Bin(   M|U,  M|C|U,M|U,  EQ_R1),  V_NO,           G_UNKNOWN, RG__NEED_WORD,FU_NO,\
_Bin(   ANY,  ANY,  ANY,  NONE ),  V_NO,           R_BIN2INT, RG_,FU_NO

#define _BitWise2( op ) \
/*       op1   op2   res   eq      verify          gen        reg*/\
_Bin(   M,    M,    M,    EQ_R1),  V_NO,           R_XC,      RG_,FU_NO,\
_Bin(   M|U,  M|U,  M|U,  EQ_R1),  V_NO,           G_UNKNOWN, RG__NEED_WORD,FU_NO,\
_Bin(   ANY,  ANY,  ANY,  NONE ),  V_NO,           R_BIN2INT, RG_,FU_NO

#define _BitWise4( op ) \
/*       op1   op2   res   eq      verify          gen        reg*/\
_Bin(   M,    M,    M,    EQ_R1),  V_NO,           R_XC,      RG_,FU_NO,\
_BinSC( R,    R,    R,    EQ_R1),  V_NO,           G_##op##R, RG_WORD,FU_NO,\
_BinSC( R,    M|C,  R,    EQ_R1),  V_NO,           G_##op,    RG_WORD,FU_NO,\
_BinSC( M,    M|C,  M,    EQ_R1),  V_NO,           G_##op##C, RG_WORD,FU_NO,\
_BinCommutes( WORD )

static  opcode_entry    And1[] = {
/********************************/
_BitWise1( N )
};

static  opcode_entry    And2[] = {
/********************************/
_BitWise2( N )
};

static  opcode_entry    And4[] = {
/********************************/
_BitWise4( N )
};

opcode_entry    NC[] = { /* this is a kludge to reserve an extra index reg */
/*       op1   op2   res   eq      verify          gen        reg*/\
_BinSC(  M,    M,    M,    EQ_R1),  V_NO,          G_NC,     RG_INDEX_NEED,FU_NO,\
_Bin(    ANY,  ANY,  ANY,  NONE ),  V_NO,          G_UNKNOWN,RG_,FU_NO
};

static  opcode_entry    Or1[] =  {
/********************************/
_BitWise1( O )
};

static  opcode_entry    Or2[] =  {
/********************************/
_BitWise2( O )
};

static  opcode_entry    Or4[] =  {
/********************************/
_BitWise4( O )
};

opcode_entry    OC[] = { /* this is a kludge to reserve an extra index reg */
/***********************/
/*       op1   op2   res   eq      verify          gen        reg*/\
_BinSC(  M,    M,    M,    EQ_R1),  V_NO,          G_OC,     RG_INDEX_NEED,FU_NO,\
_Bin(    ANY,  ANY,  ANY,  NONE ),  V_NO,          G_UNKNOWN,RG_,FU_NO
};

static  opcode_entry    Xor1[] = {
/********************************/
_BitWise1( X )
};

static  opcode_entry    Xor2[] = {
/********************************/
_BitWise2( X )
};

static  opcode_entry    Xor4[] = {
/********************************/
_BitWise4( X )
};

opcode_entry    XC[] = { /* this is a kludge to reserve an extra index reg */
/***********************/
/*       op1   op2   res   eq      verify          gen        reg*/\
_BinSC(  M,    M,    M,    EQ_R1),  V_NO,          G_XC,     RG_INDEX_NEED,FU_NO,\
_Bin(    ANY,  ANY,  ANY,  NONE ),  V_NO,          G_UNKNOWN,RG_,FU_NO
};

/**************************************************************************/
/*                             CMP                                        */
/**************************************************************************/

static  opcode_entry    CmpU1[] = {
/*********************************/
/*       op1   op2      verify          gen             reg*/
_SidCC( C,    M    ),   V_NO,           R_SWAPCMP,      RG_,FU_NO,
_SidCC( M,    C    ),   V_NO,           G_CLI,          RG_,FU_NO,
_Side(  M,    M    ),   V_NO,           R_CLC,          RG_,FU_NO,
_Side(  ANY,  ANY   ),  V_NO,           G_UNKNOWN,      RG_,FU_NO
};

static  opcode_entry    CmpU4[] = {
/*********************************/
/*       op1   op2      verify          gen             reg*/
_SidCC( R,    C    ),   V_CMPEQ_OP2ZERO,G_LTZ,          RG_WORD,FU_NO,
_SidCC( R,    R    ),   V_NO,           G_CLR,          RG_WORD,FU_NO,
_SidCC( R,    M    ),   V_NO,           G_CL,           RG_WORD,FU_NO,
_SidCC( R,    C    ),   V_NO,           G_CL,           RG_WORD,FU_NO,
_Side(  M,    M    ),   V_NO,           R_CLC,          RG_,FU_NO,
_Side(  C,    R|M  ),   V_NO,           R_SWAPCMP,      RG_WORD,FU_NO,
_Side(  R|M|C,R|M|C),   V_NO,           R_MOVOP1REG,    RG_WORD,FU_NO,
_Side(  ANY,  ANY   ),  V_NO,           G_UNKNOWN,      RG_WORD_NEED,FU_NO
};

static  opcode_entry    CmpI4[] = {
/*********************************/
/*       op1   op2      verify          gen             reg*/
_SidCC( R,    C    ),   V_OP2ZERO,      G_LTZ,          RG_WORD,FU_NO,
_SidCC( R,    R    ),   V_NO,           G_CR,           RG_WORD,FU_NO,
_SidCC( R,    M    ),   V_OP2I2,        G_CH,           RG_WORD,FU_NO,
_SidCC( R,    C    ),   V_OP2I2CON,     G_CH,           RG_WORD,FU_NO,
_SidCC( R,    M    ),   V_NO,           G_C,            RG_WORD,FU_NO,
_SidCC( R,    C    ),   V_NO,           G_C,            RG_WORD,FU_NO,
_SidCC( M,    M    ),   V_CMPEQ,        R_CLC,          RG_,FU_NO,
_Side(  C,    R|M  ),   V_NO,           R_SWAPCMP,      RG_WORD,FU_NO,
_Side(  R|M|C,R|M|C),   V_NO,           R_MOVOP1REG,    RG_WORD,FU_NO,
_Side(  ANY,  ANY   ),  V_NO,           G_UNKNOWN,      RG_WORD_NEED,FU_NO
};

#define _FloatCompare( op, type ) \
/*       op1   op2      verify          gen             reg*/ \
_SidCC( R,    C    ),   V_OP2ZERO,      G_LT##op##Z,    RG_##type,FU_NO,\
_SidCC( R,    R    ),   V_NO,           G_C##op##R,     RG_##type,FU_NO,\
_SidCC( R,    M|C  ),   V_NO,           G_C##op,        RG_##type,FU_NO,\
_SidCC( M,    M    ),   V_CMPEQ,        R_CLC,          RG_,FU_NO,\
_Side(  C,    R|M  ),   V_NO,           R_SWAPCMP,      RG_##type,FU_NO,\
_Side(  R|M|C,R|M|C),   V_NO,           R_MOVOP1REG,    RG_##type,FU_NO,\
_Side(  ANY,  ANY   ),  V_NO,           G_UNKNOWN,      RG_##type##_NEED,FU_NO

static  opcode_entry    CmpFS[] = {
/*********************************/
_FloatCompare( E, SINGLE )
};

static  opcode_entry    CmpFD[] = {
/*********************************/
_FloatCompare( D, DOUBLE )
};

opcode_entry    CLC[] = { /* this is a kludge to reserve an extra index reg */
/***********************/
/*      op1    op2      verify          gen           reg*/
_SidCC(  M,    M ),     V_NO,           G_CLC,        RG_INDEX_NEED,FU_NO,
_Side(   ANY,  ANY),    V_NO,           G_UNKNOWN,    RG_,FU_NO,
};

/**************************************************************************/
/*                             TEST                                       */
/**************************************************************************/

static  opcode_entry    Test1[] = {
/*********************************/
/*       op1   op2      verify          gen             reg*/
_SidCC( M,    C    ),   V_NO,           G_TM,           RG_,FU_NO,
_SidCC( R|M|C,R|M|C),   V_NO,           R_MAKEAND,      RG_,FU_NO,
_SidCC( ANY,  ANY  ),   V_NO,           G_UNKNOWN,      RG__NEED_WORD,FU_NO
};

static  opcode_entry    Test2[] = {
/*********************************/
/*       op1   op2      verify          gen             reg*/
_SidCC( M,    C    ),   V_OP2BYTE2CONS, R_TEST1,        RG_,FU_NO,
_SidCC( R|M|C,R|M|C),   V_NO,           R_MAKEAND,      RG_,FU_NO,
_SidCC( ANY,  ANY  ),   V_NO,           G_UNKNOWN,      RG__NEED_WORD,FU_NO,
};

static  opcode_entry    Test4[] = {
/*********************************/
/*       op1   op2      verify          gen             reg*/
_SidCC( M,    C    ),   V_OP2BYTE4CONS, R_TEST1,        RG_,FU_NO,
_SidCC( R|M|C,R|M|C),   V_NO,           R_MAKEAND,      RG_WORD,FU_NO,
_SidCC( ANY,  ANY  ),   V_NO,           G_UNKNOWN,      RG_WORD_NEED,FU_NO,
};

/**************************************************************************/
/*                             MOV                                        */
/**************************************************************************/

static  opcode_entry    Move1[] = {
/*************************/
/*       op1   op2                 verify          gen           reg*/
_UnPP(  M,    M,    NONE  ),       V_SAME_LOCN,    G_NO,         RG_,FU_NO,
_UnPP(  M,    M,    NONE ),        V_NO,           R_MVC,        RG_,FU_NO,
_UnPP(  C,    M,    NONE ),        V_NO,           G_MVI,        RG_,FU_NO,
_Un(    ANY,  ANY,  NONE ),        V_NO,           G_UNKNOWN,    RG_,FU_NO
};

#define _MoveP1( tipe, l, lr, st ) \
_UnPP(  ANY,  ANY,  EQ_R1 ),       V_NO,           G_NO,         RG_,FU_NO,\
_UnPP(  M,    M,    NONE  ),       V_SAME_LOCN,    G_NO,         RG_,FU_NO,\
_Un(    C,    R,    NONE ),        V_OP1ZERO,      G_CLRR,       RG_##tipe,FU_NO,

#define _MoveP2( tipe, l, lr, st ) \
_UnPP(  C,    M,    NONE ),        V_NO,           G_MVC,        RG_##tipe,FU_NO,\
_UnPP(  C,    R,    NONE ),        V_NO,           G_##l,        RG_##tipe,FU_NO,\
_UnPP(  R,    R,    NONE ),        V_NO,           G_##lr,       RG_##tipe,FU_NO,\
_UnPP(  R,    M,    NONE ),        V_NO,           G_##st,       RG_##tipe,FU_NO,\
_UnPP(  M,    R,    NONE ),        V_NO,           G_##l,        RG_##tipe,FU_NO,\
_UnPP(  M,    M,    NONE ),        V_NO,           R_MVC,        RG_,FU_NO,\
_Un(    ANY,  ANY,  NONE ),        V_NO,           G_UNKNOWN,    RG_##tipe,FU_NO

#define _MoveFlt( tipe, l, lr, st ) \
_MoveP1( tipe, l, lr, st ) \
_MoveP2( tipe, l, lr, st )

#define _MoveInt( tipe, l, lr, st ) \
_MoveP1( tipe, l, lr, st ) \
_UnPP(  C,    R,    NONE ),        V_OP1SMALL,     G_LA1,        RG_WORD,FU_NO,\
_UnPP(  C,    M,    NONE ),        V_OP1SMALL,     R_MOVOP1REG,  RG_WORD,FU_NO,\
_MoveP2( tipe, l, lr, st )

opcode_entry    Move2[] = {
/*************************/
_UnPP(  M,    M,    NONE  ),       V_SAME_LOCN,    G_NO,         RG_,FU_NO,
_UnPP(  C,    M,    NONE ),        V_NO,           G_MVC,        RG_,FU_NO,
_UnPP(  M,    M,    NONE ),        V_NO,           R_MVC,        RG_,FU_NO,
_Un(    ANY,  ANY,  NONE ),        V_NO,           G_UNKNOWN,    RG_,FU_NO
};

opcode_entry    Move4[] = {
/*************************/
_MoveInt( WORD, L, LR, ST )
};

opcode_entry    MoveFS[] = {
/**************************/
_MoveFlt( SINGLE, LE, LER, STE )
};

opcode_entry    MoveFD[] = {
/**************************/
_MoveFlt( DOUBLE, LD, LDR, STD )
};

opcode_entry    MVCL[] = {
/*       op1   res                 verify          gen           reg*/
_Un(    ANY,  ANY,  NONE ),        V_NO,           G_MVCL,       RG_MVCL,FU_NO,
_Un(    ANY,  ANY,  NONE ),        V_NO,           G_UNKNOWN,    RG_,FU_NO,
};

opcode_entry    MVC[] = { /* this is a kludge to reserve an extra index reg */
/***********************/
/*      op1   res                  verify          gen           reg*/
_UnPP(  M,    M,    NONE ),        V_NO,           G_MVC,        RG_INDEX_NEED,FU_NO,
_Un(    ANY,  ANY,  NONE ),        V_NO,           G_UNKNOWN,    RG_,FU_NO,
};

opcode_entry    MoveXX[] = {
/**************************/
/*       op1   res                 verify          gen           reg*/
_UnPP(  M,    M,    NONE ),        V_SIZE_SMALL,   R_MVC,        RG_,FU_NO,
_Un(    M,    M,    NONE ),        V_NO,           R_MVCL,       RG_,FU_NO,
_Un(    ANY,  M,    NONE ),        V_NO,           R_FORCEOP1MEM,RG_,FU_NO,
_Un(    ANY,  ANY,  NONE ),        V_NO,           R_FORCERESMEM,RG_,FU_NO,
};

/**************************************************************************/
/*                             UNARY                                      */
/**************************************************************************/

#define _Neg( op, type ) \
_UnCC(  R,    R,    EQ_R1 ),       V_NO,           G_##op,       RG_##type,FU_NO,\
_Un(    R|M|C,R|M,  NONE ),        V_NO,           R_USEREGISTER,RG_##type,FU_NO,\
_Un(    ANY,  ANY,  NONE  ),       V_NO,           G_UNKNOWN,  RG_##type##_NEED,FU_NO


opcode_entry    Neg4[] = {
/**************************/
_Neg( LCR, WORD )
};

opcode_entry    NegFS[] = {
/**************************/
_Neg( LCER, SINGLE )
};

opcode_entry    NegFD[] = {
/**************************/
_Neg( LCDR, DOUBLE )
};

opcode_entry    Not4[] = {
/**************************/
/*       op1   res                 verify          gen          reg*/
_UnCC(  R,     ANY,  NONE ),       V_NO,           R_NEGDEC,    RG_WORD,FU_NO,
_UnCC(  ANY,   R,    NONE ),       V_NO,           R_NEGDEC,    RG_WORD,FU_NO,
_UnCC(  M,     M,    EQ_R1 ),      V_NO,           R_NOT2XOR,   RG_,FU_NO,
_UnCC(  M,     M,    NONE ),       V_NO,           R_NEGDEC,    RG_,FU_NO,
_UnCC(  ANY,   ANY,  NONE ),       V_NO,           G_UNKNOWN,   RG_WORD_NEED,FU_NO
};


/**************************************************************************/
/*                             CVT                                        */
/**************************************************************************/

static  opcode_entry    Cvt[] = {
/***********************/
/*       op1   op2   eq            verify          gen           reg*/
_Un(    ANY,  ANY,  NONE ),        V_NO,           R_DOCVT,      RG_,FU_NO
};

/**************************************************************************/
/*                             LEA                                        */
/**************************************************************************/

static  opcode_entry    LoadA4[] = {
/**************************/
/*       op    res   eq            verify          gen           reg*/
_Un(    M,    ANY,  NONE ),        V_OFFSETZERO,   R_MOVEINDEX,  RG_WORD,FU_NO,
_Un(    M,    R,    NONE ),        V_OP1ADDR,      R_MOVEADDR,   RG_WORD,FU_NO,
_Un(    M,    R,    NONE ),        V_NO,           G_LA,         RG_WORD,FU_NO,
_Un(    M,    M,    NONE ),        V_NO,           R_MOVRESREG,  RG_WORD,FU_NO,
_Un(    ANY,  ANY,  NONE ),        V_NO,           G_UNKNOWN,    RG_WORD,FU_NO
};

/**************************************************************************/
/*                             CALL                                       */
/**************************************************************************/

static  opcode_entry    Parm[] = {
/************************/
/*       op    res   eq            verify          gen             reg*/
_Un(    ANY,  ANY,  NONE ),        V_NO,           G_NO,           RG_,FU_NO
};
/*   Calls always have their return value put in a register name*/

static  opcode_entry    Call[] = {
/************************/
/*       op    op2,  res   eq      verify          gen             reg*/
_Bin(   ANY,  ANY,  ANY,  NONE ),  V_NO,           G_CALL,         RG_,FU_NO
};

static  opcode_entry    CallI[] = {
/*************************/
/*       op1   op2   res   eq      verify          gen             reg*/
_Bin(   ANY,  ANY,  ANY,  NONE ),  V_NO,           G_ICALL,        RG_,FU_NO
};

/**************************************************************************/
/*                             NOP                                        */
/**************************************************************************/

opcode_entry    DoNop[] = {
/*************************/
/*       op1   op2   res   eq      verify          gen           reg*/
_BinPP( ANY,  ANY,  ANY,  NONE ),  V_NO,           G_NO,         RG_,FU_NO
};

/**************************************************************************/
/*                             CONVERT                                    */
/**************************************************************************/

static  opcode_entry    BinToInt[] = {
/************************/
/*       op1   op2   res   eq      verify          gen           reg*/
_Bin(   ANY,  ANY,  ANY,  NONE ),  V_NO,           R_BIN2INT,    RG_,FU_NO
};

static  opcode_entry    UnToInt[] = {
/************************/
/*       op1  res   eq             verify          gen           reg*/
_Un(    ANY,  ANY,  NONE ),        V_NO,           R_UN2INT,     RG_,FU_NO
};

static  opcode_entry    SideToInt[] = {
/************************/
/*       op1  op2          verify          gen           reg*/
_Side(  ANY,  ANY ),       V_NO,           R_SIDE2INT,   RG_,FU_NO
};

/**************************************************************************/
/*                             Select                                     */
/**************************************************************************/

static  opcode_entry    Select[] = {
_Bin(   ANY, R,  R, NONE ), V_NO,       G_SELECT, RG_INDEX,FU_NO,      /* NYI */
_Bin(   ANY, ANY, ANY, NONE ), V_NO,    G_UNKNOWN,RG_INDEX_NEED,FU_NO  /* NYI */
};

static  opcode_entry    *OpcodeList[] = {
        NULL,                   /* NO*/
        &BinToInt,              /* ADD1*/
        &BinToInt,              /* ADD2*/
        &AddU4,                 /* ADDU4*/
        &AddI4,                 /* ADDI4*/
        &AddPT,                 /* ADDPT*/
        &AddFS,                 /* ADDFS */
        &AddFD,                 /* ADDFD*/
        &BinToInt,              /* SUB1*/
        &BinToInt,              /* SUB2*/
        &SubU4,                 /* SUBU4*/
        &SubI4,                 /* SUBI4*/
        &SubU4,                 /* SUBPT*/
        &SubFS,                 /* SUBFS */
        &SubFD,                 /* SUBFD */
        &Or1,                   /* OR1 */
        &Or2,                   /* OR2 */
        &Or4,                   /* OR4 */
        &Xor1,                  /* XOR1 */
        &Xor2,                  /* XOR2 */
        &Xor4,                  /* XOR4 */
        &And1,                  /* AND1 */
        &And2,                  /* AND2 */
        &And4,                  /* AND4 */
        &BinToInt,              /* MUL1*/
        &BinToInt,              /* MUL2*/
        &Mul4,                  /* MUL4*/
        &MulFS,                 /* MULFS */
        &MulFD,                 /* MULFD */
        &BinToInt,              /* DIV1*/
        &BinToInt,              /* DIV2*/
        &Div4,                  /* DIV4*/
        &DivFS,                 /* DIVFS */
        &DivFD,                 /* DIVFD */
        &BinToInt,              /* MOD1*/
        &BinToInt,              /* MOD2*/
        &Mod4,                  /* MOD4 */
        &BinToInt,              /* SHFT1 */
        &BinToInt,              /* SHFT2 */
        &RShiftU4,              /* RSHU4 */
        &RShiftI4,              /* RSHI4 */
        &LShiftU4,              /* RSHU4 */
        &LShiftI4,              /* RSHI4 */
        &Test1,                 /* TEST1 */
        &Test2,                 /* TEST2 */
        &Test4,                 /* TEST4 */
        &Test4,                 /* TESTPT */
        &CmpU1,                 /* CMPU1*/
        &SideToInt,             /* CMPI1*/
        &SideToInt,             /* CMPU2*/
        &SideToInt,             /* CMPI2*/
        &CmpU4,                 /* CMPU4*/
        &CmpI4,                 /* CMPI4*/
        &CmpU4,                 /* CMPPT */
        &CmpFS,                 /* CMPFS */
        &CmpFD,                 /* CMPFD */
        &Move1,                 /* MOV1*/
        &Move2,                 /* MOV2*/
        &Move4,                 /* MOV4*/
        &Move4,                 /* MOVPT*/
        &MoveFS,                /* MOVFS*/
        &MoveFD,                /* MOVFD*/
        &MoveXX,                /* MOVX */
        &LoadA4,                /* LA*/
        &LoadA4,                /* LAPT*/
        &UnToInt,               /* NOT1 */
        &UnToInt,               /* NOT2 */
        &Not4,                  /* NOT4 */
        &UnToInt,               /* NEG1 */
        &UnToInt,               /* NEG2 */
        &Neg4,                  /* NEG4 */
        &NegFS,                 /* NEGFS */
        &NegFD,                 /* NEGFD */
        &Call,                  /* CALL*/
        &CallI,                 /* CALLI*/
        &Select,                /* SJUMP */
        &Parm,                  /* DPARM*/
        &Cvt,                   /* CVT*/
        &DoNop,                 /* DONOTHING*/
        NULL };                 /* BAD*/

extern  opcode_entry    *OpcodeTable( table_def i ) {
/****************************************************
    return the address of the appropriate generate table given an index "i"
*/

    return( OpcodeList[ i ] );
}
