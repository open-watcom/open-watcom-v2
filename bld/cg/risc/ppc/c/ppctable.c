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
* Description:  Table translating generic cg "assembler" to PowerPC
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
/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_DWORD,     G_NO,           FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


opcode_entry    StubBinary[] = {
/************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_DWORD,     G_NO,           FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

opcode_entry    StubSide[] = {
/******************************/
/*           op1   op2                 verify          reg           gen             fu  */
_OE( _Side(  ANY,  ANY ),              V_NO,           RG_DWORD,     G_NO,           FU_NO ),
_OE( _Side(  ANY,  ANY ),              V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

#define LOAD_TABLE( name, reg ) \
static  opcode_entry    name[] = {                                                             \
/********************************/                                                             \
/*           from  to    eq            verify          reg           gen             fu  */    \
_OE( _UnPP(  M,    R,    NONE ),       V_NO,           RG_##reg,     G_LOAD_ADDR,    FU_ALU ), \
_OE( _Un(    M,    M,    NONE ),       V_NO,           RG_##reg,     R_MOVRESTEMP,   FU_NO ),  \
_OE( _Un(    M,    ANY,  NONE ),       V_NO,           RG_##reg##_NEED,G_UNKNOWN,    FU_NO ),  \
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_##reg,     R_FORCEOP1MEM,  FU_NO ),  \
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),  \
};

LOAD_TABLE( LoadAddr2, WORD );
LOAD_TABLE( LoadAddr8, QWORD );

static  opcode_entry    LoadAddr4[] = {
/*************************************/
/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    M,    ANY,  NONE ),       V_OFFSETZERO,   RG_DWORD,     R_MOVEINDEX,    FU_NO ),
_OE( _UnPP(  M,    R,    NONE ),       V_NO,           RG_DWORD,     G_LOAD_ADDR,    FU_ALU ),
_OE( _Un(    M,    M,    NONE ),       V_NO,           RG_DWORD,     R_MOVRESTEMP,   FU_NO ),
_OE( _Un(    M,    ANY,  NONE ),       V_NO,           RG_DWORD_NEED,G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_DWORD,     R_FORCEOP1MEM,  FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

opcode_entry    Conv[] = {
/************************/
/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    ANY,  ANY, NONE ),        V_NO,           RG_,          R_DOCVT,        FU_NO ),
};

opcode_entry    NegF[] = {
/************************/
/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    R,    R,    NONE ),       V_NO,           RG_FLOAT,     G_UNARY,        FU_NO ),
_OE( _Un(    C|M,  ANY,  NONE ),       V_NO,           RG_FLOAT,     R_MOVOP1TEMP,   FU_NO ),
_OE( _Un(    ANY,  M,    NONE ),       V_NO,           RG_FLOAT,     R_MOVRESTEMP,   FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_FLOAT_NEED,G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

opcode_entry    MoveXX[] = {
/**************************/
/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    ANY,  ANY,  NONE  ),      V_REG_SIZE,     RG_,          R_CHANGETYPE,   FU_NO ),
_OE( _Un(    M|U,  M|U,  EQ_R1 ),      NVI(V_NO),      RG_,          G_NO,           FU_NO ),
_OE( _UnPP(  M,    M,    NONE  ),      NVI(V_SAME_LOCN),RG_,         G_NO,           FU_NO ),
_OE( _Un(    U,    ANY,  NONE ),       V_NO,           RG_,          R_FORCEOP1MEM,  FU_NO ),
_OE( _Un(    ANY,  U,    NONE ),       V_NO,           RG_,          R_FORCERESMEM,  FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_OP1_RES_AL4,  RG_DWORD_NEED,R_MOVEXX_4,     FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          R_MOVEXX,       FU_NO ),
};

#define MOVE_TABLE( t_name, reg, load, store ) \
opcode_entry    t_name[] = {                                                                   \
/**************************/                                                                   \
/*           from  to    eq            verify          reg           gen             fu  */    \
_OE( _Un(    ANY,  ANY,  EQ_R1 ),      NVI(V_NO),      RG_,          G_NO,           FU_NO ),  \
_OE( _UnPP(  M,    M,    NONE  ),      NVI(V_SAME_LOCN),RG_,         G_NO,           FU_NO ),  \
_OE( _Un(    R,    R,    NONE ),       V_NO,           RG_##reg,     G_MOVE,         FU_NO ),  \
_OE( _Un(    R,    M,    NONE ),       V_NO,           RG_##reg,     store,          FU_MEM ), \
_OE( _Un(    C,    M,    NONE ),       V_NO,           RG_##reg,     R_MOVOP1TEMP,   FU_NO ),  \
_OE( _Un(    M,    R,    NONE ),       V_NO,           RG_##reg,     load,           FU_MEM ), \
_OE( _Un(    C,    R,    NONE ),       V_OP1HIGHADDR,  RG_##reg,     G_LEA_HIGH,     FU_NO ),  \
_OE( _Un(    C,    R,    NONE ),       V_HALFWORDCONST1,RG_##reg,    G_LEA,          FU_NO ),  \
_OE( _Un(    C,    R,    NONE ),       V_UHALFWORDCONST1,RG_##reg,   G_MOVE_UI,      FU_NO ),  \
_OE( _Un(    C,    R,    NONE ),       V_NO,           RG_##reg,     R_CONSTLOAD,    FU_NO ),  \
_OE( _Un(    M,    M,    NONE ),       V_NO,           RG_,          R_MOVOP1TEMP,   FU_NO ),  \
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_##reg##_NEED,G_UNKNOWN,    FU_NO ),  \
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),  \
};

MOVE_TABLE( Move1, BYTE,  G_LOAD, G_STORE );
MOVE_TABLE( Move2, WORD,  G_LOAD, G_STORE );
MOVE_TABLE( Move4, DWORD, G_LOAD, G_STORE );

opcode_entry    Move8[] = {
/**************************/
/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    ANY,  ANY,  EQ_R1 ),      NVI(V_NO),      RG_,          G_NO,           FU_NO ),
_OE( _UnPP(  M,    M,    NONE  ),      NVI(V_SAME_LOCN),RG_,         G_NO,           FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          R_SPLITMOVE,    FU_NO ),
};

#define BINARY_TABLE( name, reg ) \
opcode_entry    name[] = {                                                                     \
/***************************/                                                                  \
/*           op1   op2   res   eq      verify          reg           gen             fu  */    \
_OE( _Bin(   R,    R,    R,    NONE ), V_NO,           RG_##reg,     G_BINARY,       FU_ALU ), \
_OE( _Bin(   R,    C,    R,    NONE ), V_HALFWORDCONST2,RG_##reg,    G_BINARY_IMM,   FU_ALU ), \
_OE( _Bin(   R,    C,    R,    NONE ), V_NO,           RG_##reg,     R_MOVOP2TEMP,   FU_NO ),  \
_OE( _Bin(   C,    C,    R,    NONE ), V_NO,           RG_##reg,     R_MOVOP1TEMP,   FU_NO ),  \
_OE( _Bin(   C,    ANY,  R,    NONE ), V_SYMMETRIC,    RG_##reg,     R_SWAPOPS,      FU_NO ),  \
_OE( _Bin(   C,    ANY,  R,    NONE ), V_NO,           RG_##reg,     R_MOVOP1TEMP,   FU_NO ),  \
_OE( _Bin(   M,    ANY,  ANY,  NONE ), V_NO,           RG_##reg,     R_MOVOP1TEMP,   FU_NO ),  \
_OE( _Bin(   ANY,  M,    ANY,  NONE ), V_NO,           RG_##reg,     R_MOVOP2TEMP,   FU_NO ),  \
_OE( _Bin(   ANY,  ANY,  M,    NONE ), V_NO,           RG_##reg,     R_MOVRESTEMP,   FU_NO ),  \
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_##reg##_NEED,G_UNKNOWN,    FU_NO ),  \
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),  \
};

BINARY_TABLE( Binary1, BYTE  );
BINARY_TABLE( Binary2, WORD  );
BINARY_TABLE( Binary4, DWORD );

#define U_BINARY_TABLE( name, reg ) \
opcode_entry    name[] = {                                                                     \
/***************************/                                                                  \
/*           op1   op2   res   eq      verify          reg           gen             fu  */    \
_OE( _Bin(   R,    R,    R,    NONE ), V_NO,           RG_##reg,     G_BINARYS,      FU_ALU ), \
_OE( _Bin(   R,    C,    R,    NONE ), V_UHALFWORDCONST2,RG_##reg,   G_BINARYS_IMM,  FU_ALU ), \
_OE( _Bin(   R,    C,    R,    NONE ), V_NO,           RG_##reg,     R_MOVOP2TEMP,   FU_NO ),  \
_OE( _Bin(   C,    C,    R,    NONE ), V_NO,           RG_##reg,     R_MOVOP1TEMP,   FU_NO ),  \
_OE( _Bin(   C,    ANY,  R,    NONE ), V_SYMMETRIC,    RG_##reg,     R_SWAPOPS,      FU_NO ),  \
_OE( _Bin(   C,    ANY,  R,    NONE ), V_NO,           RG_##reg,     R_MOVOP1TEMP,   FU_NO ),  \
_OE( _Bin(   M,    ANY,  ANY,  NONE ), V_NO,           RG_##reg,     R_MOVOP1TEMP,   FU_NO ),  \
_OE( _Bin(   ANY,  M,    ANY,  NONE ), V_NO,           RG_##reg,     R_MOVOP2TEMP,   FU_NO ),  \
_OE( _Bin(   ANY,  ANY,  M,    NONE ), V_NO,           RG_##reg,     R_MOVRESTEMP,   FU_NO ),  \
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_##reg##_NEED,G_UNKNOWN,    FU_NO ),  \
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),  \
};

U_BINARY_TABLE( UBinary1, BYTE  );
U_BINARY_TABLE( UBinary2, WORD  );
U_BINARY_TABLE( UBinary4, DWORD );

#define N_BINARY_TABLE( name, reg ) \
opcode_entry    name[] = {                                                                     \
/***************************/                                                                  \
/*           op1   op2   res   eq      verify          reg           gen             fu  */    \
_OE( _Bin(   R,    R,    R,    NONE ), V_NO,           RG_##reg,     G_BINARY,       FU_ALU ), \
_OE( _Bin(   R,    C,    R,    NONE ), V_NO,           RG_##reg,     R_MOVOP2TEMP,   FU_NO ),  \
_OE( _Bin(   C,    C,    R,    NONE ), V_NO,           RG_##reg,     R_MOVOP1TEMP,   FU_NO ),  \
_OE( _Bin(   C,    ANY,  R,    NONE ), V_SYMMETRIC,    RG_##reg,     R_SWAPOPS,      FU_NO ),  \
_OE( _Bin(   C,    ANY,  R,    NONE ), V_NO,           RG_##reg,     R_MOVOP1TEMP,   FU_NO ),  \
_OE( _Bin(   M,    ANY,  ANY,  NONE ), V_NO,           RG_##reg,     R_MOVOP1TEMP,   FU_NO ),  \
_OE( _Bin(   ANY,  M,    ANY,  NONE ), V_NO,           RG_##reg,     R_MOVOP2TEMP,   FU_NO ),  \
_OE( _Bin(   ANY,  ANY,  M,    NONE ), V_NO,           RG_##reg,     R_MOVRESTEMP,   FU_NO ),  \
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_##reg##_NEED,G_UNKNOWN,    FU_NO ),  \
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),  \
};

N_BINARY_TABLE( NBinary1, BYTE  );
N_BINARY_TABLE( NBinary2, WORD  );
N_BINARY_TABLE( NBinary4, DWORD );

static  opcode_entry    Binary8[] = {
/***********************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  ANY,  ANY, NONE ),  V_NO,           RG_QWORD,     R_SPLITOP,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY, NONE ),  V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

opcode_entry    Push[] = {
/************************/
/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_DWORD,     R_PUSHTOMOV,    FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

opcode_entry    Pop[] = {
/***********************/
/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_DWORD,     R_POPTOMOV,     FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

opcode_entry    Un1[] = {
/***********************/
/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    R,    R,    NONE ),       V_NO,           RG_BYTE,      G_UNARY,        FU_ALU ),
_OE( _Un(    C|M,  ANY,  NONE ),       V_NO,           RG_BYTE,      R_MOVOP1TEMP,   FU_NO ),
_OE( _Un(    ANY,  M,    NONE ),       V_NO,           RG_BYTE,      R_MOVRESTEMP,   FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_BYTE_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

opcode_entry    Un2[] = {
/***********************/
/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    R,    R,    NONE ),       V_NO,           RG_WORD,      G_UNARY,        FU_ALU ),
_OE( _Un(    C|M,  ANY,  NONE ),       V_NO,           RG_WORD,      R_MOVOP1TEMP,   FU_NO ),
_OE( _Un(    ANY,  M,    NONE ),       V_NO,           RG_WORD,      R_MOVRESTEMP,   FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_WORD_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

opcode_entry    Un4[] = {
/***********************/
/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    R,    R,    NONE ),       V_NO,           RG_DWORD,     G_UNARY,        FU_ALU ),
_OE( _Un(    C|M,  ANY,  NONE ),       V_NO,           RG_DWORD,     R_MOVOP1TEMP,   FU_NO ),
_OE( _Un(    ANY,  M,    NONE ),       V_NO,           RG_DWORD,     R_MOVRESTEMP,   FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_DWORD_NEED,G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

opcode_entry    VaStart[] = {
/***************************/
/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    R,    ANY,  NONE ),       V_NO,           RG_DWORD,     G_VASTART,      FU_ALU ),
_OE( _Un(    M,    ANY,  NONE ),       V_NO,           RG_DWORD,     R_MOVOP1TEMP,   FU_ALU ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_DWORD_NEED,G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

opcode_entry    MoveF[] = {
/*************************/
/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    ANY,  ANY,  EQ_R1 ),      NVI(V_NO),      RG_,          G_NO,           FU_NO ),
_OE( _Un(    M,    M,    NONE ),       V_SAME_LOCN,    RG_,          G_NO,           FU_NO ),
_OE( _Un(    R,    R,    NONE ),       V_NO,           RG_FLOAT,     G_MOVE_FP,      FU_NO ),
_OE( _Un(    R,    M,    NONE ),       V_NO,           RG_FLOAT,     G_STORE,        FU_NO ),
_OE( _Un(    M,    R,    NONE ),       V_NO,           RG_FLOAT,     G_LOAD,         FU_NO ),
_OE( _Un(    C,    ANY,  NONE ),       V_NO,           RG_,          R_FORCEOP1CMEM, FU_NO ),
_OE( _Un(    M,    M,    NONE ),       V_NO,           RG_,          R_MOVOP1TEMP,   FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_FLOAT_NEED,G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

opcode_entry    FloatBinary[] = {
/*******************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   R,    R,    R,    NONE ), V_NO,           RG_FLOAT,     G_BINARY_FP,    FU_ALU ),
_OE( _Bin(   C,    ANY,  ANY,  NONE ), V_NO,           RG_FLOAT,     R_FORCEOP1CMEM, FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  NONE ), V_NO,           RG_FLOAT,     R_FORCEOP2CMEM, FU_NO ),
_OE( _Bin(   M,    ANY,  ANY,  NONE ), V_NO,           RG_FLOAT,     R_MOVOP1TEMP,   FU_NO ),
_OE( _Bin(   ANY,  M,    ANY,  NONE ), V_NO,           RG_FLOAT,     R_MOVOP2TEMP,   FU_NO ),
_OE( _Bin(   ANY,  ANY,  M,    NONE ), V_NO,           RG_FLOAT,     R_MOVRESTEMP,   FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_FLOAT_NEED,G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


opcode_entry    DoNop[] = {
/*************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _BinPP( ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_NO,           FU_NO ),
};

opcode_entry    Set4[] = {
/************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          R_DOSET,        FU_ALU ),
};

opcode_entry    Test4[] = {
/*************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          R_DOTEST,       FU_ALU ),
};

opcode_entry    Cmp4[] = {
/************************/
/*           op1   op2                 verify          reg           gen             fu  */
_OE( _Side(  R,    R ),                V_NO,           RG_DWORD,     G_CMP,          FU_NO ),
_OE( _Side(  R,    C ),                V_HALFWORDCONST2,RG_DWORD,    G_CMP_I,        FU_NO ),
_OE( _Side(  R,    C ),                V_NO,           RG_DWORD,     R_MOVOP2TEMP,   FU_NO ),
_OE( _Side(  C,    C ),                V_NO,           RG_DWORD,     R_MOVOP1TEMP,   FU_NO ),
_OE( _Side(  C,    R ),                V_NO,           RG_DWORD,     R_SWAPCMP,      FU_NO ),
_OE( _Side(  M,    ANY ),              V_NO,           RG_DWORD,     R_MOVOP1TEMP,   FU_NO ),
_OE( _Side(  ANY,  M ),                V_NO,           RG_DWORD,     R_MOVOP2TEMP,   FU_NO ),
_OE( _Side(  ANY,  ANY ),              V_NO,           RG_DWORD_NEED,G_UNKNOWN,      FU_NO ),
_OE( _Side(  ANY,  ANY ),              V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

opcode_entry    CmpF[] = {
/************************/
/*           op1   op2                 verify          reg           gen             fu  */
_OE( _Side(  R,    R ),                V_NO,           RG_FLOAT,     G_CMP_FP,       FU_NO ),
_OE( _Side(  R,    C ),                V_NO,           RG_FLOAT,     R_FORCEOP2CMEM, FU_NO ),
_OE( _Side(  C,    R ),                V_NO,           RG_FLOAT,     R_SWAPCMP,      FU_NO ),
_OE( _Side(  M,    ANY ),              V_NO,           RG_FLOAT,     R_MOVOP1TEMP,   FU_NO ),
_OE( _Side(  ANY,  M ),                V_NO,           RG_FLOAT,     R_MOVOP2TEMP,   FU_NO ),
_OE( _Side(  ANY,  ANY ),              V_NO,           RG_FLOAT_NEED,G_UNKNOWN,      FU_NO ),
_OE( _Side(  ANY,  ANY ),              V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

opcode_entry    Call[] = {
/************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_CALL,         FU_NO ),
};

opcode_entry    CallI[] = {
/*************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_CALLI,        FU_NO ),
};

opcode_entry    Rtn[] = {
/***********************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          R_MAKECALL,     FU_NO ),
};

opcode_entry    Promote[] = {
/***************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          R_BIN2INT,      FU_NO ),
};

opcode_entry    Mod4[] = {
/************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          R_MOD2DIV,      FU_NO ),
};


static  opcode_entry    *OpcodeList[] = {
    #define pick(enum,opcode)  opcode,
    #include "_tables.h"
    #undef pick
};

extern  opcode_entry    *OpcodeTable( table_def i )
/*************************************************/
{
    return( OpcodeList[ i ] );
}
