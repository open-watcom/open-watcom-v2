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
* Description:  Table translating generic cg "assembler" to Alpha AXP
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

#define LOAD_TABLE( name, reg )                                                                \
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
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          R_DOCVT,        FU_NO ),
};

opcode_entry    LoadUnaligned[] = {
/*********************************/
/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    M,    R,    NONE ),       V_NO,           RG_QWORD,     G_LDQ_U,        FU_NO ),
_OE( _Un(    M,    M,    NONE ),       V_NO,           RG_QWORD,     R_MOVRESTEMP,   FU_NO ),
_OE( _Un(    U,    ANY,  NONE ),       V_NO,           RG_QWORD,     R_FORCEOP1MEM,  FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_QWORD_NEED,G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

opcode_entry    StoreUnaligned[] = {
/**********************************/
/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    R,    M,    NONE ),       V_NO,           RG_QWORD,     G_STQ_U,        FU_NO ),
_OE( _Un(    M,    M,    NONE ),       V_NO,           RG_QWORD,     R_MOVOP1TEMP,   FU_NO ),
_OE( _Un(    ANY,  U,    NONE ),       V_NO,           RG_QWORD,     R_FORCERESMEM,  FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_QWORD_NEED,G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
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
//_OE( _Un(    ANY,  ANY,  NONE ),       V_REG_SIZE,     RG_,          R_CHANGETYPE, FU_NO ),
_OE( _Un(    M|U,  M|U,  EQ_R1 ),      NVI(V_NO),      RG_,          G_NO,           FU_NO ),
_OE( _UnPP(  M,    M,    NONE ),       V_SAME_LOCN,    RG_,          G_NO,           FU_NO ),
_OE( _Un(    U,    ANY,  NONE ),       V_NO,           RG_,          R_FORCEOP1MEM,  FU_NO ),
_OE( _Un(    ANY,  U,    NONE ),       V_NO,           RG_,          R_FORCERESMEM,  FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_OP1_RES_AL8,  RG_QWORD_NEED,R_MOVEXX_8,     FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_OP1_RES_AL4,  RG_QWORD_NEED,R_MOVEXX_4,     FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_QWORD_NEED,R_MOVEXX,       FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

opcode_entry    Move1[] = {
/*************************/
/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    ANY,  ANY,  EQ_R1 ),      NVI(V_NO),      RG_,          G_NO,           FU_NO ),
_OE( _UnPP(  M,    M,    NONE ),       V_SAME_LOCN,    RG_,          G_NO,           FU_NO ),
_OE( _Un(    R,    R,    NONE ),       V_NO,           RG_BYTE,      G_MOVE,         FU_NO ),
_OE( _Un(    R,    M,    NONE ),       V_RES_TEMP,     RG_BYTE,      G_STORE,        FU_MEM ),
_OE( _Un(    R,    M,    NONE ),       V_NO,           RG_BYTE,      R_STORE_1,      FU_MEM ),
_OE( _Un(    C,    M,    NONE ),       V_NO,           RG_BYTE,      R_MOVOP1TEMP,   FU_NO ),
_OE( _Un(    M,    R,    NONE ),       V_OP1_AL4,      RG_BYTE,      G_LOAD,         FU_MEM ),
_OE( _Un(    M,    R,    NONE ),       V_NO,           RG_BYTE,      R_LOAD_1,       FU_MEM ),
_OE( _Un(    C,    R,    NONE ),       V_UNSIGNED,     RG_BYTE,      G_BYTE_CONST,   FU_NO ),
_OE( _Un(    C,    R,    NONE ),       V_NO,           RG_BYTE,      G_LEA,          FU_NO ),
_OE( _Un(    M,    M,    NONE ),       V_NO,           RG_,          R_MOVOP1TEMP,   FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_BYTE_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

opcode_entry    Move2[] = {
/*************************/
/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    ANY,  ANY,  EQ_R1 ),      NVI(V_NO),      RG_,          G_NO,           FU_NO ),
_OE( _UnPP(  M,    M,    NONE ),       V_SAME_LOCN,    RG_,          G_NO,           FU_NO ),
_OE( _Un(    R,    R,    NONE ),       V_NO,           RG_WORD,      G_MOVE,         FU_NO ),
_OE( _Un(    R,    M,    NONE ),       V_RES_TEMP,     RG_WORD,      G_STORE,        FU_MEM ),
_OE( _Un(    R,    M,    NONE ),       V_RES_AL2,      RG_WORD,      R_STORE_2,      FU_MEM ),
_OE( _Un(    R,    M,    NONE ),       V_NO,           RG_WORD,      R_STORE_2U,     FU_MEM ),
_OE( _Un(    C,    M,    NONE ),       V_NO,           RG_WORD,      R_MOVOP1TEMP,   FU_NO ),
_OE( _Un(    M,    R,    NONE ),       V_OP1_AL4,      RG_WORD,      G_LOAD,         FU_MEM ),
_OE( _Un(    M,    R,    NONE ),       V_OP1_AL2,      RG_WORD,      R_LOAD_2,       FU_MEM ),
_OE( _Un(    M,    R,    NONE ),       V_NO,           RG_WORD,      R_LOAD_2U,      FU_MEM ),
_OE( _Un(    C,    R,    NONE ),       V_OP1HIGHADDR,  RG_WORD,      G_LEA_HIGH,     FU_NO ),
_OE( _Un(    C,    R,    NONE ),       V_HALFWORDCONST1,RG_WORD,     G_LEA,          FU_NO ),
_OE( _Un(    C,    R,    NONE ),       V_NO,           RG_WORD,      R_CONSTLOAD,    FU_NO ),
_OE( _Un(    M,    M,    NONE ),       V_NO,           RG_,          R_MOVOP1TEMP,   FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_WORD_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

opcode_entry    Move4[] = {
/*************************/
/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    ANY,  ANY,  EQ_R1 ),      NVI(V_NO),      RG_,          G_NO,           FU_NO ),
_OE( _UnPP(  M,    M,    NONE ),       V_SAME_LOCN,    RG_,          G_NO,           FU_NO ),
_OE( _Un(    R,    R,    NONE ),       V_NO,           RG_DWORD,     G_MOVE,         FU_NO ),
_OE( _Un(    R,    M,    NONE ),       V_RES_AL4,      RG_DWORD,     G_STORE,        FU_MEM ),
_OE( _Un(    R,    M,    NONE ),       V_NO,           RG_DWORD,     R_STORE_4U,     FU_MEM ),
_OE( _Un(    C,    M,    NONE ),       V_NO,           RG_DWORD,     R_MOVOP1TEMP,   FU_NO ),
_OE( _Un(    M,    R,    NONE ),       V_OP1_AL4,      RG_DWORD,     G_LOAD,         FU_MEM ),
_OE( _Un(    M,    R,    NONE ),       V_NO,           RG_DWORD,     R_LOAD_4U,      FU_MEM ),
_OE( _Un(    C,    R,    NONE ),       V_OP1HIGHADDR,  RG_DWORD,     G_LEA_HIGH,     FU_NO ),
_OE( _Un(    C,    R,    NONE ),       V_HALFWORDCONST1,RG_DWORD,    G_LEA,          FU_NO ),
_OE( _Un(    C,    R,    NONE ),       V_NO,           RG_DWORD,     R_CONSTLOAD,    FU_NO ),
_OE( _Un(    M,    M,    NONE ),       V_NO,           RG_,          R_MOVOP1TEMP,   FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_DWORD_NEED,G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

opcode_entry    Move8[] = {
/*************************/
/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    ANY,  ANY,  EQ_R1 ),      NVI(V_NO),      RG_,          G_NO,           FU_NO ),
_OE( _UnPP(  M,    M,    NONE ),       V_SAME_LOCN,    RG_,          G_NO,           FU_NO ),
_OE( _Un(    C,    ANY,  NONE ),       V_OP164BITCONST,RG_,          R_FORCEOP1CMEM, FU_NO ),
_OE( _Un(    R,    R,    NONE ),       V_NO,           RG_QWORD,     G_MOVE,         FU_NO ),
//_OE( _Un(    R,    M,    NONE ),       V_RES_AL8,      RG_QWORD,     G_STORE,        FU_MEM ),
//_OE( _Un(    R,    M,    NONE ),       V_NO,           RG_QWORD,     R_STORE_8U,     FU_MEM ),
_OE( _Un(    R,    M,    NONE ),       V_NO,           RG_QWORD,     G_STORE,        FU_MEM ),
_OE( _Un(    C,    M,    NONE ),       V_NO,           RG_QWORD,     R_MOVOP1TEMP,   FU_NO ),
//_OE( _Un(    M,    R,    NONE ),       V_OP1_AL8,      RG_QWORD,     G_LOAD,         FU_MEM ),
//_OE( _Un(    M,    R,    NONE ),       V_NO,           RG_QWORD,     R_LOAD_8U,      FU_MEM ),
_OE( _Un(    M,    R,    NONE ),       V_NO,           RG_QWORD,     G_LOAD,         FU_MEM ),
_OE( _Un(    C,    R,    NONE ),       V_OP1HIGHADDR,  RG_QWORD,     G_LEA_HIGH,     FU_NO ),
_OE( _Un(    C,    R,    NONE ),       V_HALFWORDCONST1,RG_QWORD,    G_LEA,          FU_NO ),
_OE( _Un(    C,    R,    NONE ),       V_NO,           RG_QWORD,     R_CONSTLOAD,    FU_NO ),
_OE( _Un(    M,    M,    NONE ),       V_NO,           RG_,          R_MOVOP1TEMP,   FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_QWORD_NEED,G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

#define BINARY_TABLE( name, reg )                                                              \
opcode_entry    name[] = {                                                                     \
/***************************/                                                                  \
/*           op1   op2   res   eq      verify          reg           gen             fu  */    \
_OE( _Bin(   R,    R,    R,    NONE ), V_NO,           RG_##reg,     G_BINARY,       FU_ALU ), \
_OE( _Bin(   R,    C,    R,    NONE ), V_BYTECONST2,   RG_##reg,     G_BINARY_IMM,   FU_ALU ), \
_OE( _Bin(   R,    C,    R,    NONE ), V_NO,           RG_##reg,     R_MOVOP2TEMP,   FU_NO ),  \
_OE( _Bin(   C,    C,    R,    NONE ), V_NO,           RG_##reg,     R_MOVOP1TEMP,   FU_NO ),  \
_OE( _Bin(   C,    ANY,  R,    NONE ), V_SYMMETRIC,    RG_##reg,     R_SWAPOPS,      FU_NO ),  \
_OE( _Bin(   C,    ANY,  R,    NONE ), V_NO,           RG_##reg,     R_MOVOP1TEMP,   FU_NO ),  \
_OE( _Bin(   M,    ANY,  ANY,  NONE ), V_NO,           RG_##reg,     R_MOVOP1REG,    FU_NO ),  \
_OE( _Bin(   ANY,  M,    ANY,  NONE ), V_NO,           RG_##reg,     R_MOVOP2REG,    FU_NO ),  \
_OE( _Bin(   ANY,  ANY,  M,    NONE ), V_NO,           RG_##reg,     R_MOVRESTEMP,   FU_NO ),  \
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_##reg##_NEED,G_UNKNOWN,    FU_NO ),  \
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),  \
};

BINARY_TABLE( Binary1, BYTE  );
BINARY_TABLE( Binary2, WORD  );
BINARY_TABLE( Binary4, DWORD );
BINARY_TABLE( Binary8, QWORD );

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

opcode_entry    Alloca4[] = {
/***************************/
/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_DWORD,     R_ALLOCA,       FU_NO ),
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

opcode_entry    Un8[] = {
/***********************/
/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    R,    R,    NONE ),       V_NO,           RG_QWORD,     G_UNARY,        FU_ALU ),
_OE( _Un(    C|M,  ANY,  NONE ),       V_NO,           RG_QWORD,     R_MOVOP1TEMP,   FU_NO ),
_OE( _Un(    ANY,  M,    NONE ),       V_NO,           RG_QWORD,     R_MOVRESTEMP,   FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_QWORD_NEED,G_UNKNOWN,      FU_NO ),
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
_OE( _Un(    C,    U,    NONE ),       V_RESCONSTTEMP, RG_FLOAT_NEED,G_UNKNOWN,      FU_NO ),
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
_OE( _Bin(   M,    ANY,  ANY,  NONE ), V_NO,           RG_FLOAT,     R_MOVOP1REG,    FU_NO ),
_OE( _Bin(   ANY,  M,    ANY,  NONE ), V_NO,           RG_FLOAT,     R_MOVOP2REG,    FU_NO ),
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

opcode_entry    Cmp8[] = {
/************************/
/*           op1   op2                 verify          reg           gen             fu  */
_OE( _Side(  R,    R ),                V_NO,           RG_QWORD,     R_SIMPCMP,      FU_NO ),
_OE( _Side(  R,    C ),                V_OP2ZERO,      RG_QWORD,     G_CONDBR,       FU_NO ),
_OE( _Side(  R,    C ),                V_NO,           RG_QWORD,     R_SIMPCMP,      FU_NO ),
_OE( _Side(  C,    C ),                V_NO,           RG_QWORD,     R_MOVOP1TEMP,   FU_NO ),
_OE( _Side(  C,    R ),                V_NO,           RG_QWORD,     R_SWAPCMP,      FU_NO ),
_OE( _Side(  M,    ANY ),              V_NO,           RG_QWORD,     R_MOVOP1REG,    FU_NO ),
_OE( _Side(  ANY,  M ),                V_NO,           RG_QWORD,     R_MOVOP2REG,    FU_NO ),
_OE( _Side(  ANY,  ANY ),              V_NO,           RG_QWORD_NEED,G_UNKNOWN,      FU_NO ),
_OE( _Side(  ANY,  ANY ),              V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

opcode_entry    CmpF[] = {
/************************/
/*           op1   op2                 verify          reg           gen             fu  */
_OE( _Side(  R,    R ),                V_NO,           RG_FLOAT,     R_SIMPCMP,      FU_NO ),
_OE( _Side(  R,    C ),                V_OP2ZERO,      RG_FLOAT,     G_CONDBR,       FU_NO ),
_OE( _Side(  R,    C ),                V_NO,           RG_FLOAT,     R_FORCEOP2CMEM, FU_NO ),
_OE( _Side(  C,    R ),                V_NO,           RG_FLOAT,     R_SWAPCMP,      FU_NO ),
_OE( _Side(  C,    C ),                V_NO,           RG_FLOAT,     R_FORCEOP1CMEM, FU_NO ),
_OE( _Side(  M,    ANY ),              V_NO,           RG_FLOAT,     R_MOVOP1REG,    FU_NO ),
_OE( _Side(  ANY,  M ),                V_NO,           RG_FLOAT,     R_MOVOP2REG,    FU_NO ),
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

static  opcode_entry    UnaryRtn[] = {
/************************************/
/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    ANY,  ANY,  NONE   ),     V_NO,           RG_,          R_MAKECALL,     FU_NO ),
};


opcode_entry    Promote[] = {
/***************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          R_BIN2INT,      FU_NO ),
};

opcode_entry    Promote8[] = {
/****************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          R_BIN2QUAD,     FU_NO ),
};

static  opcode_entry    *OpcodeList[] = {
    #define pick(enum,opcode)  opcode,
    #include "_tables.h"
    #undef pick
};

extern  opcode_entry    *OpcodeTable( table_def i ) {
/***************************************************/

    return( OpcodeList[ i ] );
}
