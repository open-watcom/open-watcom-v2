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
* Description:  Table translating generic cg "assembler" to MIPS
*               specific instructions.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "pattern.h"
#include "regset.h"
#include "model.h"
#include "tables.h"

opcode_entry    StubUnary[] = {
/*************************/
/*      op1   op2   res            verify      gen           reg            fu */
_Un(    ANY,  ANY,  NONE ),        V_NO,       G_NO,         RG_DWORD,      FU_NO,
};


opcode_entry    StubBinary[] = {
/************************/
/*      op1   op2   res   eq       verify      gen           reg            fu */
_Bin(   ANY,  ANY,  ANY,  NONE ),  V_NO,       G_NO,         RG_DWORD,      FU_NO,
};

opcode_entry    StubSide[] = {
/******************************/
/*      op1   op2               verify         gen           reg            fu */
_Side(  ANY,  ANY ),            V_NO,          G_NO,         RG_DWORD,      FU_NO,
};

#define LOAD_TABLE( name, reg ) \
static  opcode_entry    name[] = {                                                      \
/********************************/                                                      \
/*      op    res   eq          verify         gen             reg             fu */    \
_UnPP(  M,    R,    NONE ),     V_NO,          G_LOAD_ADDR,    RG_##reg,       FU_ALU,  \
_Un(    M,    M,    NONE ),     V_NO,          R_MOVRESTEMP,   RG_##reg,       FU_NO,   \
_Un(    M,    ANY,  NONE ),     V_NO,          G_UNKNOWN,      RG_##reg##_NEED,FU_NO,   \
_Un(    ANY,  ANY,  NONE ),     V_NO,          R_FORCEOP1MEM,  RG_##reg,       FU_NO,   \
};

LOAD_TABLE( LoadAddr2, WORD );
LOAD_TABLE( LoadAddr8, QWORD );

static  opcode_entry    LoadAddr4[] = {
/*************************************/
/*      op    res   eq          verify         gen             reg             fu */
_Un(    M,    ANY,  NONE ),     V_OFFSETZERO,  R_MOVEINDEX,    RG_DWORD,       FU_NO,
_UnPP(  M,    R,    NONE ),     V_NO,          G_LOAD_ADDR,    RG_DWORD,       FU_ALU,
_Un(    M,    M,    NONE ),     V_NO,          R_MOVRESTEMP,   RG_DWORD,       FU_NO,
_Un(    M,    ANY,  NONE ),     V_NO,          G_UNKNOWN,      RG_DWORD_NEED,  FU_NO,
_Un(    ANY,  ANY,  NONE ),     V_NO,          R_FORCEOP1MEM,  RG_DWORD,       FU_NO,
};

opcode_entry    Conv[] = {
/************************/
/*       op1   res  eq             verify      gen           reg            fu */
_Un(     ANY,  ANY, NONE ),        V_NO,       R_DOCVT,      RG_,           FU_NO,
};

opcode_entry    LoadUnaligned[] = {
/*********************************/
/*       op1   res   eq            verify      gen           reg            fu */
_Un(     M,    R,    NONE ),       V_NO,       G_LOAD_UA,    RG_DWORD,      FU_NO,
_Un(     M,    M,    NONE ),       V_NO,       R_MOVRESTEMP, RG_DWORD,      FU_NO,
_Un(     U,    ANY,  NONE ),       V_NO,       R_FORCEOP1MEM,RG_DWORD,      FU_NO,
_Un(     ANY,  ANY,  NONE ),       V_NO,       G_UNKNOWN,    RG_DWORD_NEED, FU_NO,
};

opcode_entry    StoreUnaligned[] = {
/**********************************/
/*       op1   res   eq            verify      gen           reg            fu */
_Un(     R,    M,    NONE ),       V_NO,       G_STORE_UA,   RG_DWORD,      FU_NO,
_Un(     M,    M,    NONE ),       V_NO,       R_MOVOP1TEMP, RG_DWORD,      FU_NO,
_Un(     ANY,  U,    NONE ),       V_NO,       R_FORCERESMEM,RG_DWORD,      FU_NO,
_Un(     ANY,  ANY,  NONE ),       V_NO,       G_UNKNOWN,    RG_DWORD_NEED, FU_NO,
};

opcode_entry    NegF[] = {
/************************/
/*       op1   res   eq            verify      gen           reg            fu */
_Un(    R,    R,    NONE ),        V_NO,       G_UNARY,      RG_FLOAT,      FU_NO,
_Un(    C|M,  ANY,  NONE ),        V_NO,       R_MOVOP1TEMP, RG_FLOAT,      FU_NO,
_Un(    ANY,  M,    NONE ),        V_NO,       R_MOVRESTEMP, RG_FLOAT,      FU_NO,
_Un(    ANY,  ANY,  NONE ),        V_NO,       G_UNKNOWN,    RG_FLOAT_NEED, FU_NO,
};

opcode_entry    MoveXX[] = {
/**************************/
/*      op    res   eq             verify          gen           reg            fu */
// _Un(    ANY,  ANY,  NONE  ),    V_REG_SIZE,     R_CHANGETYPE, RG_,           FU_NO,
_Un(    M|U,  M|U,  EQ_R1 ),       NVI(V_NO),      G_NO,         RG_,           FU_NO,
_UnPP(  M,    M,    NONE  ),       V_SAME_LOCN,    G_NO,         RG_,           FU_NO,
_Un(    U,    ANY,  NONE ),        V_NO,           R_FORCEOP1MEM,RG_,           FU_NO,
_Un(    ANY,  U,    NONE ),        V_NO,           R_FORCERESMEM,RG_,           FU_NO,
//_Un(    ANY,  ANY,  NONE ),        V_OP1_RES_AL8,  R_MOVEXX_8,   RG_QWORD_NEED, FU_NO,
_Un(    ANY,  ANY,  NONE ),        V_OP1_RES_AL4,  R_MOVEXX_4,   RG_DWORD_NEED, FU_NO,
_Un(    ANY,  ANY,  NONE ),        V_NO,           R_MOVEXX,     RG_DWORD_NEED, FU_NO,
};

// TODO: 1- and 2- byte loads ought to be conversions, not moves
opcode_entry    Move1[] = {
/**************************/
/*       op1   res   eq            verify          gen           reg      fu */
_Un(     ANY,  ANY,  EQ_R1 ),      NVI(V_NO),      G_NO,         RG_,     FU_NO,
_UnPP(   M,    M,    NONE  ),      NVI(V_SAME_LOCN),G_NO,        RG_,     FU_NO,
_Un(     R,    R,    NONE ),       V_NO,           G_MOVE,       RG_BYTE, FU_NO,
_Un(     R,    M,    NONE ),       V_NO,           G_STORE,      RG_BYTE, FU_MEM,
_Un(     C,    M,    NONE ),       V_NO,           R_MOVOP1TEMP, RG_BYTE, FU_NO,
_Un(     M,    R,    NONE ),       V_NO,           G_LOAD,       RG_BYTE, FU_MEM,
_Un(     C,    R,    NONE ),       V_HALFWORDCONST1,G_LEA,       RG_BYTE, FU_NO,
_Un(     C,    R,    NONE ),       V_UHALFWORDCONST1,G_MOVE_UI,  RG_BYTE, FU_NO,
_Un(     C,    R,    NONE ),       V_NO,           R_CONSTLOAD,  RG_BYTE, FU_NO,
_Un(     M,    M,    NONE ),       V_NO,           R_MOVOP1TEMP, RG_,     FU_NO,
_Un(     ANY,  ANY,  NONE ),       V_NO,           G_UNKNOWN,    RG_BYTE_NEED,FU_NO,
};

opcode_entry    Move2[] = {
/**************************/
/*       op1   res   eq            verify          gen           reg      fu */
_Un(     ANY,  ANY,  EQ_R1 ),      NVI(V_NO),      G_NO,         RG_,     FU_NO,
_UnPP(   M,    M,    NONE  ),      NVI(V_SAME_LOCN),G_NO,        RG_,     FU_NO,
_Un(     R,    R,    NONE ),       V_NO,           G_MOVE,       RG_WORD, FU_NO,
_Un(     R,    M,    NONE ),       V_RES_AL2,      G_STORE,      RG_WORD, FU_MEM,
_Un(     R,    M,    NONE ),       V_NO,           G_STORE_UA,   RG_WORD, FU_MEM,
_Un(     C,    M,    NONE ),       V_NO,           R_MOVOP1TEMP, RG_WORD, FU_NO,
_Un(     M,    R,    NONE ),       V_OP1_AL4,      G_LOAD,       RG_WORD, FU_MEM,
_Un(     M,    R,    NONE ),       V_NO,           G_LOAD_UA,    RG_WORD, FU_MEM,
_Un(     C,    R,    NONE ),       V_OP1HIGHADDR,  G_LEA_HIGH,   RG_WORD, FU_NO,
_Un(     C,    R,    NONE ),       V_HALFWORDCONST1,G_LEA,       RG_WORD, FU_NO,
_Un(     C,    R,    NONE ),       V_UHALFWORDCONST1,G_MOVE_UI,  RG_WORD, FU_NO,
_Un(     C,    R,    NONE ),       V_NO,           R_CONSTLOAD,  RG_WORD, FU_NO,
_Un(     M,    M,    NONE ),       V_NO,           R_MOVOP1TEMP, RG_,     FU_NO,
_Un(     ANY,  ANY,  NONE ),       V_NO,           G_UNKNOWN,    RG_WORD_NEED,FU_NO,
};

opcode_entry    Move4[] = {
/**************************/
/*       op1   res   eq            verify          gen           reg      fu */
_Un(     ANY,  ANY,  EQ_R1 ),      NVI(V_NO),      G_NO,         RG_,     FU_NO,
_UnPP(   M,    M,    NONE  ),      NVI(V_SAME_LOCN),G_NO,        RG_,     FU_NO,
_Un(     R,    R,    NONE ),       V_NO,           G_MOVE,       RG_DWORD,FU_NO,
_Un(     R,    M,    NONE ),       V_RES_AL4,      G_STORE,      RG_DWORD,FU_MEM,
_Un(     R,    M,    NONE ),       V_NO,           G_STORE_UA,   RG_DWORD,FU_MEM,
_Un(     C,    M,    NONE ),       V_NO,           R_MOVOP1TEMP, RG_DWORD,FU_NO,
#if 0
_Un(     M,    R,    NONE ),       V_OP1_AL4,      G_LOAD,       RG_DWORD,FU_MEM,
_Un(     M,    R,    NONE ),       V_NO,           R_LOAD_4U,    RG_DWORD,FU_MEM,
#else
_Un(     M,    R,    NONE ),       V_NO,           G_LOAD,       RG_DWORD,FU_MEM,
#endif
_Un(     C,    R,    NONE ),       V_OP1HIGHADDR,  G_LEA_HIGH,   RG_DWORD,FU_NO,
_Un(     C,    R,    NONE ),       V_HALFWORDCONST1,G_LEA,       RG_DWORD,FU_NO,
_Un(     C,    R,    NONE ),       V_UHALFWORDCONST1,G_MOVE_UI,  RG_DWORD,FU_NO,
_Un(     C,    R,    NONE ),       V_NO,           R_CONSTLOAD,  RG_DWORD,FU_NO,
_Un(     M,    M,    NONE ),       V_NO,           R_MOVOP1TEMP, RG_,     FU_NO,
_Un(     ANY,  ANY,  NONE ),       V_NO,           G_UNKNOWN,    RG_DWORD_NEED,FU_NO,
};

opcode_entry    Move8[] = {
/**************************/
/*       op1   res   eq            verify          gen           reg      fu */
_Un(     ANY,  ANY,  EQ_R1 ),      NVI(V_NO),      G_NO,         RG_,     FU_NO,
_UnPP(   M,    M,    NONE  ),      NVI(V_SAME_LOCN),G_NO,        RG_,     FU_NO,
_Un(     ANY,  ANY,  NONE ),       V_NO,           R_SPLITMOVE,  RG_,     FU_NO,
};

#define BINARY_TABLE( name, reg, const_verify ) \
opcode_entry    name[] = {                                                              \
/***************************/                                                           \
/*       op1   op2   res   eq      verify          gen           reg            fu */   \
_Bin(    R,    R,    R,    NONE ), V_NO,           G_BINARY,     RG_##reg,      FU_ALU, \
_Bin(    R,    C,    R,    NONE ), const_verify,   G_BINARY_IMM, RG_##reg,      FU_ALU, \
_Bin(    R,    C,    R,    NONE ), V_NO,           R_MOVOP2TEMP, RG_##reg,      FU_NO,  \
_Bin(    C,    C,    R,    NONE ), V_NO,           R_MOVOP1TEMP, RG_##reg,      FU_NO,  \
_Bin(    C,    ANY,  R,    NONE ), V_SYMMETRIC,    R_SWAPOPS,    RG_##reg,      FU_NO,  \
_Bin(    C,    ANY,  R,    NONE ), V_NO,           R_MOVOP1TEMP, RG_##reg,      FU_NO,  \
_Bin(    M,    ANY,  ANY,  NONE ), V_NO,           R_MOVOP1REG,  RG_##reg,      FU_NO,  \
_Bin(    ANY,  M,    ANY,  NONE ), V_NO,           R_MOVOP2REG,  RG_##reg,      FU_NO,  \
_Bin(    ANY,  ANY,  M,    NONE ), V_NO,           R_MOVRESTEMP, RG_##reg,      FU_NO,  \
_Bin(    ANY,  ANY,  ANY,  NONE ), V_NO,           G_UNKNOWN,    RG_##reg##_NEED, FU_NO,\
};

/* Why three binary operator reduction tables? Some instructions are
 * hardcoded to take signed immediate operands and others only take
 * unsigned. Separate tables were deemed less error prone than having
 * to check for operand signedness in the verification code. Finally
 * some operators (mul/div/mod) can't take immediate operands at all.
 */
BINARY_TABLE( BinaryUC1, BYTE,  V_UHALFWORDCONST2 );
BINARY_TABLE( BinaryUC2, WORD,  V_UHALFWORDCONST2 );
BINARY_TABLE( BinaryUC4, DWORD, V_UHALFWORDCONST2 );

BINARY_TABLE( Binary1, BYTE,  V_HALFWORDCONST2 );
BINARY_TABLE( Binary2, WORD,  V_HALFWORDCONST2 );
BINARY_TABLE( Binary4, DWORD, V_HALFWORDCONST2 );

// Instead of V_OP2ZERO this should just forbid the reduction completely
BINARY_TABLE( BinaryNI1, BYTE,  V_OP2ZERO );
BINARY_TABLE( BinaryNI2, WORD,  V_OP2ZERO );
BINARY_TABLE( BinaryNI4, DWORD, V_OP2ZERO );

static  opcode_entry    Binary8[] = {
/***********************************/
/*       op1   op2   res  eq      verify      gen           reg         fu*/
_Bin(   ANY,  ANY,  ANY, NONE ),  V_NO,       R_SPLITOP,    RG_QWORD,   FU_NO,
};

opcode_entry    Push[] = {
/************************/
/*       op1   op2   res           verify     gen           reg            fu */
_Un(     ANY,  ANY,  NONE ),       V_NO,      R_PUSHTOMOV,  RG_DWORD,      FU_NO,
};

opcode_entry    Pop[] = {
/***********************/
/*       op1   op2   res           verify     gen           reg            fu */
_Un(     ANY,  ANY,  NONE ),       V_NO,      R_POPTOMOV,   RG_DWORD,      FU_NO,
};

opcode_entry    Un1[] = {
/***********************/
/*      op1   res   eq             verify     gen           reg            fu */
_Un(    R,    R,    NONE ),        V_NO,      G_UNARY,      RG_BYTE,       FU_ALU,
_Un(    C|M,  ANY,  NONE ),        V_NO,      R_MOVOP1TEMP, RG_BYTE,       FU_NO,
_Un(    ANY,  M,    NONE ),        V_NO,      R_MOVRESTEMP, RG_BYTE,       FU_NO,
_Un(    ANY,  ANY,  NONE ),        V_NO,      G_UNKNOWN,    RG_BYTE_NEED,  FU_NO,
};

opcode_entry    Un2[] = {
/***********************/
/*      op1   res   eq             verify     gen           reg            fu */
_Un(    R,    R,    NONE ),        V_NO,      G_UNARY,      RG_WORD,       FU_ALU,
_Un(    C|M,  ANY,  NONE ),        V_NO,      R_MOVOP1TEMP, RG_WORD,       FU_NO,
_Un(    ANY,  M,    NONE ),        V_NO,      R_MOVRESTEMP, RG_WORD,       FU_NO,
_Un(    ANY,  ANY,  NONE ),        V_NO,      G_UNKNOWN,    RG_WORD_NEED,  FU_NO,
};

opcode_entry    Alloca4[] = {
/***************************/
/*      op1   res   eq             verify     gen           reg            fu */
_Un(    ANY,  ANY,  NONE ),        V_NO,      R_ALLOCA,     RG_DWORD,      FU_NO,
};

opcode_entry    Un4[] = {
/***********************/
/*      op1   res   eq             verify     gen           reg            fu */
_Un(    R,    R,    NONE ),        V_NO,      G_UNARY,      RG_DWORD,      FU_ALU,
_Un(    C|M,  ANY,  NONE ),        V_NO,      R_MOVOP1TEMP, RG_DWORD,      FU_NO,
_Un(    ANY,  M,    NONE ),        V_NO,      R_MOVRESTEMP, RG_DWORD,      FU_NO,
_Un(    ANY,  ANY,  NONE ),        V_NO,      G_UNKNOWN,    RG_DWORD_NEED, FU_NO,
};

opcode_entry    Un8[] = {
/***********************/
/*      op1   res   eq             verify     gen           reg            fu */
_Un(    ANY,  ANY,  NONE ),        V_NO,      R_SPLITUNARY, RG_QWORD,      FU_NO,
};

opcode_entry    Neg8[] = {
/***********************/
/*      op1   res   eq             verify     gen           reg            fu */
_Un(    ANY,  ANY,  NONE ),        V_NO,      R_SPLITNEG,   RG_QWORD,      FU_NO,
};

opcode_entry    MoveF[] = {
/*************************/
/*       op1   res   eq            verify          gen           reg            fu */
_Un(     ANY,  ANY,  EQ_R1 ),      NVI(V_NO),      G_NO,         RG_,           FU_NO,
_Un(     M,    M,    NONE ),       V_SAME_LOCN,    G_NO,         RG_,           FU_NO,
_Un(     R,    R,    NONE ),       V_NO,           G_MOVE_FP,    RG_FLOAT,      FU_NO,
_Un(     R,    M,    NONE ),       V_NO,           G_STORE,      RG_FLOAT,      FU_NO,
_Un(     M,    R,    NONE ),       V_NO,           G_LOAD,       RG_FLOAT,      FU_NO,
_Un(     C,    U,    NONE ),       V_RESCONSTTEMP, G_UNKNOWN,    RG_FLOAT_NEED, FU_NO,
_Un(     C,    ANY,  NONE ),       V_NO,           R_FORCEOP1CMEM,RG_,          FU_NO,
_Un(     M,    M,    NONE ),       V_NO,           R_MOVOP1TEMP, RG_,           FU_NO,
_Un(     ANY,  ANY,  NONE ),       V_NO,           G_UNKNOWN,    RG_FLOAT_NEED, FU_NO,
};

opcode_entry    FloatBinary[] = {
/*******************************/
/*       op1   op2   res   eq      verify     gen           reg            fu */
_Bin(    R,    R,    R,    NONE ), V_NO,      G_BINARY_FP,  RG_FLOAT,      FU_ALU,
_Bin(    C,    ANY,  ANY,  NONE ), V_NO,      R_FORCEOP1CMEM,RG_FLOAT,     FU_NO,
_Bin(    ANY,  C,    ANY,  NONE ), V_NO,      R_FORCEOP2CMEM,RG_FLOAT,     FU_NO,
_Bin(    M,    ANY,  ANY,  NONE ), V_NO,      R_MOVOP1REG,  RG_FLOAT,      FU_NO,
_Bin(    ANY,  M,    ANY,  NONE ), V_NO,      R_MOVOP2REG,  RG_FLOAT,      FU_NO,
_Bin(    ANY,  ANY,  M,    NONE ), V_NO,      R_MOVRESTEMP, RG_FLOAT,      FU_NO,
_Bin(    ANY,  ANY,  ANY,  NONE ), V_NO,      G_UNKNOWN,    RG_FLOAT_NEED, FU_NO,
};


opcode_entry    DoNop[] = {
/*************************/
/*       op1   op2   res   eq      verify     gen           reg         fu */
_BinPP( ANY,  ANY,  ANY,  NONE ),  V_NO,      G_NO,         RG_,        FU_NO,
};

opcode_entry    Set4[] = {
/************************/
/*       op1   op2   res   eq      verify     gen           reg         fu */
_Bin(    ANY,  ANY,  ANY,  NONE ), V_NO,      R_DOSET,      RG_,        FU_ALU,
};

opcode_entry    Test4[] = {
/*************************/
/*       op1   op2   res   eq      verify     gen           reg         fu */
_Bin(    ANY,  ANY,  ANY,  NONE ), V_NO,      R_DOTEST,     RG_,        FU_ALU,
};

/* V_MIPSBRANCH is true if condition is OP_CMP_NOT_EQUAL or OP_CMP_EQUAL.
 * Such condition can be encoded as beq/bne using registers as operands.
 */
opcode_entry    Cmp4[] = {
/************************/
/*      op1   op2       verify          gen             reg             fu */
_Side(  R,    R ),      V_MIPSBRANCH,   G_CONDBR,       RG_DWORD,       FU_NO,
_Side(  R,    R ),      V_NO,           R_M_SIMPCMP,    RG_DWORD,       FU_NO,
_Side(  R,    C ),      V_OP2ZERO,      G_CONDBR,       RG_DWORD,       FU_NO,
_Side(  R,    C ),      V_MIPSBRANCH,   R_MOVOP2TEMP,   RG_DWORD,       FU_NO,
_Side(  R,    C ),      V_NO,           R_M_SIMPCMP,    RG_DWORD,       FU_NO,
_Side(  C,    C ),      V_NO,           R_MOVOP1TEMP,   RG_DWORD,       FU_NO,
_Side(  C,    R ),      V_NO,           R_SWAPCMP,      RG_DWORD,       FU_NO,
_Side(  M,    ANY ),    V_NO,           R_MOVOP1REG,    RG_DWORD,       FU_NO,
_Side(  ANY,  M ),      V_NO,           R_MOVOP2REG,   RG_DWORD,       FU_NO,
_Side(  ANY,  ANY ),    V_NO,           G_UNKNOWN,      RG_DWORD_NEED,  FU_NO,
};

opcode_entry    Cmp8[] = {
/************************/
/*      op1   op2       verify          gen             reg             fu */
_Side(  ANY,  ANY ),    V_NO,           R_SPLITCMP,     RG_QWORD,       FU_NO,
};

opcode_entry    CmpF[] = {
/************************/
/*      op1   op2       verify          gen             reg             fu */
_Side(  R,    R ),      V_NO,           R_SIMPCMP,      RG_FLOAT,       FU_NO,
_Side(  R,    C ),      V_OP2ZERO,      G_CONDBR,       RG_FLOAT,       FU_NO,
_Side(  R,    C ),      V_NO,           R_FORCEOP2CMEM, RG_FLOAT,       FU_NO,
_Side(  C,    R ),      V_NO,           R_SWAPCMP,      RG_FLOAT,       FU_NO,
_Side(  C,    C ),      V_NO,           R_FORCEOP1CMEM, RG_FLOAT,       FU_NO,
_Side(  M,    ANY ),    V_NO,           R_MOVOP1REG,    RG_FLOAT,       FU_NO,
_Side(  ANY,  M ),      V_NO,           R_MOVOP2REG,    RG_FLOAT,       FU_NO,
_Side(  ANY,  ANY ),    V_NO,           G_UNKNOWN,      RG_FLOAT_NEED,  FU_NO,
};

opcode_entry    Call[] = {
/************************/
/*      op    op2,  res   eq          verify      gen             reg  fu */
_Bin(   ANY,  ANY,  ANY,  NONE ),     V_NO,       G_CALL,         RG_, FU_NO,
};

opcode_entry    CallI[] = {
/*************************/
/*      op    op2,  res   eq          verify      gen             reg  fu */
_Bin(   ANY,  ANY,  ANY,  NONE ),     V_NO,       G_CALLI,        RG_, FU_NO,
};

opcode_entry    Rtn[] = {
/***********************/
/*      op    op2,  res   eq          verify      gen             reg  fu */
_Bin(   ANY,  ANY,  ANY,  NONE ),     V_NO,       R_MAKECALL,     RG_, FU_NO,
};

static  opcode_entry    UnaryRtn[] = {
/************************************/
/*      op    op2,  res               verify      gen             reg  fu */
_Un(    ANY,  ANY,  NONE   ),         V_NO,       R_MAKECALL,     RG_, FU_NO,
};


opcode_entry    Promote[] = {
/***************************/
/*      op    op2,  res   eq          verify      gen             reg  fu */
_Bin(   ANY,  ANY,  ANY,  NONE ),     V_NO,       R_BIN2INT,      RG_, FU_NO,
};

opcode_entry    Promote8[] = {
/****************************/
/*      op    op2,  res   eq           verify     gen             reg  fu */
_Bin(   ANY,  ANY,  ANY,  NONE ),      V_NO,      R_BIN2QUAD,     RG_, FU_NO,
};

static  opcode_entry    *OpcodeList[] = {
    #define pick(enum,opcode)  opcode,
    #include "_tables.h"
    #undef pick
};

extern  opcode_entry    *OpcodeTable( table_def i )
/***************************************************/
{
    return( OpcodeList[i] );
}
