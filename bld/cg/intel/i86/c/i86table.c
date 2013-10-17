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
* Description:  Table of possible instructions and reductions for each
*               opcode.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "pattern.h"
#include "regset.h"
#include "model.h"
#include "tables.h"

extern    opcode_entry  Cmp87[];
extern    opcode_entry  Un87[];
extern    opcode_entry  Un87Func[];
extern    opcode_entry  Bin87[];
extern    opcode_entry  Bin87Func[];
extern    opcode_entry  Move87S[];
extern    opcode_entry  Move87D[];
extern    opcode_entry  Push87S[];
extern    opcode_entry  Push87D[];

static  opcode_entry    Add1[] = {
/************************/

/* optimizing reductions*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2ZERO), RG_BYTE,      R_MAKEMOVE,     FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2NEG),  RG_BYTE,      R_MAKESUB,      FU_NO ),
_OE( _Bin(   R,    U,    R,    NONE ), V_CONSTTEMP,    RG_BYTE,      R_TEMP2CONST,   FU_NO ),

/* instructions that we can generate*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _BinSC( R,    C,    R,    EQ_R1 ),V_OP2ONE,       RG_BYTE,      G_R1,           FU_ALU1 ),
_OE( _BinSC( R,    C,    R,    EQ_R1 ),V_NO,           RG_BYTE_ACC,  G_AC,           FU_ALU1 ),
_OE( _BinSC( R,    C,    R,    EQ_R1 ),V_NO,           RG_BYTE,      G_RC,           FU_ALU1 ),
_OE( _BinSC( M,    C,    M,    EQ_R1 ),V_OP2ONE,       RG_,          G_M1,           FU_ALU1 ),
_OE( _BinSC( M,    C,    M,    EQ_R1 ),V_NO,           RG_,          G_MC,           FU_ALU1 ),
_OE( _BinSC( R,    R,    R,    EQ_R1 ),V_NO,           RG_BYTE,      G_RR2,          FU_ALU1 ),
_OE( _BinSC( R,    M,    R,    EQ_R1 ),V_NO,           RG_BYTE,      G_RM2,          FU_ALU1 ),
_OE( _BinSC( M,    R,    M,    EQ_R1 ),V_NO,           RG_BYTE,      G_MR2,          FU_ALU1 ),

/* simplifying reductions*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   C,    R|M|U,ANY,  NONE ), V_NO,           RG_BYTE,      R_SWAPOPS,      FU_NO ),
_OE( _Bin(   R|M,  R|M|C,R|M,  EQ_R1 ),V_NO,           RG_BYTE,      R_LOADOP2,      FU_NO ),
_OE( _Bin(   R|M|C,R|M,  R|M,  EQ_R2 ),V_NO,           RG_BYTE,      R_SWAPOPS,      FU_NO ),
_OE( _Bin(   R,    ANY,  ANY,  NONE ), V_SWAP_GOOD,    RG_BYTE,      R_SWAPOPS,      FU_NO ),
_OE( _Bin(   R|M|C,R|M|C,R|M,  NONE ), V_NO,           RG_BYTE,      R_USEREGISTER,  FU_NO ),
_OE( _Bin(   ANY,  R|C,  ANY,  EQ_R1), V_NO,           RG_BYTE,      G_UNKNOWN,      FU_NO ),
_OE( _Bin(   R|C,  ANY,  ANY,  EQ_R2), V_NO,           RG_BYTE,      G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_BYTE_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Add2[] = {
/************************/

/* optimizing reductions*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2ZERO), RG_WORD,      R_MAKEMOVE,     FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2NEG),  RG_WORD,      R_MAKESUB,      FU_NO ),
_OE( _Bin(   R|M|U,C,    R|M|U,EQ_R1), NVI(V_OP2LOW_B_ZERO),RG_TWOBYTE,R_CYPHIGH,    FU_NO ),
_OE( _Bin(   R,    U,    R,    NONE ), V_CONSTTEMP,    RG_WORD,      R_TEMP2CONST,   FU_NO ),

/* instructions we can generate*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _BinSC( R,    C,    R,    EQ_R1 ),V_OP1SP,        RG_WORD,      G_RC,           FU_ALU1 ),
_OE( _BinSC( R,    C,    R,    EQ_R1 ),V_OP2ONE,       RG_WORD,      G_WORDR1,       FU_ALU1 ),
_OE( _BinSC( R,    C,    R,    EQ_R1 ),V_OP2TWO_SIZE,  RG_WORD,      G_WORDR1,       FU_ALU1 ),
_OE( _BinSC( R,    C,    R,    EQ_R1 ),V_NO,           RG_WORD_ACC,  G_AC,           FU_ALU1 ),
_OE( _BinSC( R,    C,    R,    EQ_R1 ),V_NO,           RG_WORD,      G_RC,           FU_ALU1 ),
_OE( _BinPP( R,    C,    R,    NONE ), V_LEA,          RG_LEA,       G_LEA,          FU_ALU1 ),
_OE( _BinSC( M,    C,    M,    EQ_R1 ),V_OP2ONE,       RG_,          G_M1,           FU_ALU1 ),

/* Fall into AddExt table*/
/**** NB. AddExt points here ****/
/* opcode_entry    AddExt[] = { */
/**************************/

/* instructions that we can generate*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _BinSC( R,    C,    R,    EQ_R1 ),V_NO,           RG_WORD_ACC,  G_AC,           FU_ALU1 ),
_OE( _BinSC( R,    C,    R,    EQ_R1 ),V_NO,           RG_WORD,      G_RC,           FU_ALU1 ),
_OE( _BinSC( M,    C,    M,    EQ_R1 ),V_NO,           RG_,          G_MC,           FU_ALU1 ),
_OE( _BinSC( R,    R,    R,    EQ_R1 ),V_NO,           RG_WORD,      G_RR2,          FU_ALU1 ),
_OE( _BinSC( R,    M,    R,    EQ_R1 ),V_NO,           RG_WORD,      G_RM2,          FU_ALU1 ),
_OE( _BinSC( M,    R,    M,    EQ_R1 ),V_NO,           RG_WORD,      G_MR2,          FU_ALU1 ),

/* simplifying reductions*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   R|M,  R|M|C,R|M,  EQ_R1 ),V_NO,           RG_WORD,      R_LOADOP2,      FU_NO ),
_OE( _Bin(   R|M|C,R|M,  R|M,  EQ_R2 ),V_NO,           RG_WORD,      R_SWAPOPS,      FU_NO ),
_OE( _Bin(   R,    ANY,  ANY,  NONE ), V_SWAP_GOOD,    RG_WORD,      R_SWAPOPS,      FU_NO ),
_OE( _Bin(   R|M|C,R|M|C,R|M,  NONE ), V_NO,           RG_WORD,      R_USEREGISTER,  FU_NO ),
_OE( _Bin(   ANY,  R|C,  ANY,  EQ_R1), V_NO,           RG_WORD,      G_UNKNOWN,      FU_NO ),
_OE( _Bin(   R|C,  ANY,  ANY,  EQ_R2), V_NO,           RG_WORD,      G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_WORD_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

/* Point at where AddExt used to start */
/*************************/
#define AddExt &Add2[11]

static  opcode_entry    Add4[] = {
/************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_DOUBLE,    R_SPLITOP,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Add8[] = {
/************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_8,         R_SPLIT8BIN,    FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    AddCP[] = {
/*************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
//_OE( _Bin(   C,    R|M|U,ANY,  NONE ), V_NO,           RG_DBL_OR_PTR,R_SWAPOPS,      FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2ZERO), RG_DBL_OR_PTR,R_MAKEMOVE,     FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2NEG),  RG_DBL_OR_PTR,R_MAKESUB,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_DBL_OR_PTR,R_MAKEU2,       FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    AddPT[] = {
/*************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
//_OE( _Bin(   C,    R|M|U,ANY,  NONE ), V_NO,           RG_,          R_SWAPOPS,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          R_MAKECALL,     FU_NO ),
};


static  opcode_entry    Sub1[] = {
/************************/

/* optimizing reductions*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   C,    ANY,  ANY,  NONE ), NVI(V_OP1ZERO), RG_BYTE,      R_MAKENEG,      FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2ZERO), RG_BYTE,      R_MAKEMOVE,     FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2NEG),  RG_BYTE,      R_MAKEADD,      FU_NO ),
_OE( _Bin(   R,    U,    R,    NONE ), V_CONSTTEMP,    RG_BYTE,      R_TEMP2CONST,   FU_NO ),

/* instructions that we can generate*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _BinSC( R,    C,    R,    EQ_R1 ),V_OP2ONE,       RG_BYTE,      G_R1,           FU_ALU1 ),
_OE( _BinCC( R,    C,    R,    EQ_R1 ),V_NO,           RG_BYTE_ACC,  G_AC,           FU_ALU1 ),
_OE( _BinCC( R,    C,    R,    EQ_R1 ),V_NO,           RG_BYTE,      G_RC,           FU_ALU1 ),
_OE( _BinSC( M,    C,    M,    EQ_R1 ),V_OP2ONE,       RG_,          G_M1,           FU_ALU1 ),
_OE( _BinCC( M,    C,    M,    EQ_R1 ),V_NO,           RG_,          G_MC,           FU_ALU1 ),
_OE( _BinCC( R,    R,    R,    EQ_R1 ),V_NO,           RG_BYTE,      G_RR2,          FU_ALU1 ),
_OE( _BinCC( R,    M,    R,    EQ_R1 ),V_NO,           RG_BYTE,      G_RM2,          FU_ALU1 ),
_OE( _BinCC( M,    R,    M,    EQ_R1 ),V_NO,           RG_BYTE,      G_MR2,          FU_ALU1 ),

/* simplifying reductions*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   R|M,  R|M|C,R|M,  EQ_R1 ),V_NO,           RG_BYTE,      R_LOADOP2,      FU_NO ),
_OE( _Bin(   R|M|C,R|M|C,R|M,  NONE ), V_NO,           RG_BYTE,      R_USEREGISTER,  FU_NO ),
_OE( _Bin(   ANY,  R|C,  ANY,  EQ_R1), V_NO,           RG_BYTE,      G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_BYTE_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Sub2[] = {
/************************/

/* optimizing reductions*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   C,    ANY,  ANY,  NONE ), NVI(V_OP1ZERO), RG_WORD,      R_MAKENEG,      FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2ZERO), RG_WORD,      R_MAKEMOVE,     FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2NEG),  RG_WORD,      R_MAKEADD,      FU_NO ),
_OE( _Bin(   R,    U,    R,    NONE ), V_CONSTTEMP,    RG_WORD,      R_TEMP2CONST,   FU_NO ),

/* instructions we can generate*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _BinSC( R,    C,    R,    EQ_R1 ),V_OP1SP,        RG_WORD,      G_RC,           FU_ALU1 ),
_OE( _BinSC( R,    C,    R,    EQ_R1 ),V_OP2ONE,       RG_WORD,      G_WORDR1,       FU_ALU1 ),
_OE( _BinSC( R,    C,    R,    EQ_R1 ),V_OP2TWO_SIZE,  RG_WORD,      G_WORDR1,       FU_ALU1 ),
_OE( _BinCC( R,    C,    R,    EQ_R1 ),V_NO,           RG_WORD_ACC,  G_AC,           FU_ALU1 ),
_OE( _BinCC( R,    C,    R,    EQ_R1 ),V_NO,           RG_WORD,      G_RC,           FU_ALU1 ),
_OE( _BinPP( R,    C,    R,    NONE ), V_LEA,          RG_LEA,       G_LEA,          FU_ALU1 ),
_OE( _BinSC( M,    C,    M,    EQ_R1 ),V_OP2ONE,       RG_,          G_M1,           FU_ALU1 ),

/* cheap pointer subtractions come through here since the result type is I2*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_OP2PTR,       RG_,          R_CPSUB,        FU_NO ),

/* from here on down is identical to SubExt table except SubExt uses _BinSC*/

/* instructions that we can generate*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _BinCC( R,    C,    R,    EQ_R1 ),V_NO,           RG_WORD_ACC,  G_AC,           FU_ALU1 ),
_OE( _BinCC( R,    C,    R,    EQ_R1 ),V_NO,           RG_WORD,      G_RC,           FU_ALU1 ),
_OE( _BinCC( M,    C,    M,    EQ_R1 ),V_NO,           RG_,          G_MC,           FU_ALU1 ),
_OE( _BinCC( R,    R,    R,    EQ_R1 ),V_NO,           RG_WORD,      G_RR2,          FU_ALU1 ),
_OE( _BinCC( R,    M,    R,    EQ_R1 ),V_NO,           RG_WORD,      G_RM2,          FU_ALU1 ),
_OE( _BinCC( M,    R,    M,    EQ_R1 ),V_NO,           RG_WORD,      G_MR2,          FU_ALU1 ),

/* simplifying reductions*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   R|M,  R|M|C,R|M,  EQ_R1 ),V_NO,           RG_WORD,      R_LOADOP2,      FU_NO ),
_OE( _Bin(   R|M|C,R|M|C,R|M,  NONE ), V_NO,           RG_WORD,      R_USEREGISTER,  FU_NO ),
_OE( _Bin(   ANY,  R|C,  ANY,  EQ_R1), V_NO,           RG_WORD,      G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_WORD_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


opcode_entry    SubExt[] = {
/**************************/
/* instructions that we can generate*/
/* NOTE: SBB does NOT set conditions right in all cases*/
/*       consider SBB AX,DX when AX==0,DX==FFFF,CF==1 -> leaves carry set and a JB*/
/*       will take the jump!!!*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _BinSC( R,    C,    R,    EQ_R1 ),V_NO,           RG_WORD_ACC,  G_AC,           FU_ALU1 ),
_OE( _BinSC( R,    C,    R,    EQ_R1 ),V_NO,           RG_WORD,      G_RC,           FU_ALU1 ),
_OE( _BinSC( M,    C,    M,    EQ_R1 ),V_NO,           RG_,          G_MC,           FU_ALU1 ),
_OE( _BinSC( R,    R,    R,    EQ_R1 ),V_NO,           RG_WORD,      G_RR2,          FU_ALU1 ),
_OE( _BinSC( R,    M,    R,    EQ_R1 ),V_NO,           RG_WORD,      G_RM2,          FU_ALU1 ),
_OE( _BinSC( M,    R,    M,    EQ_R1 ),V_NO,           RG_WORD,      G_MR2,          FU_ALU1 ),

/* simplifying reductions*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   R|M,  R|M|C,R|M,  EQ_R1 ),V_NO,           RG_WORD,      R_LOADOP2,      FU_NO ),
_OE( _Bin(   R|M|C,R|M|C,R|M,  NONE ), V_NO,           RG_WORD,      R_USEREGISTER,  FU_NO ),
_OE( _Bin(   ANY,  R|C,  ANY,  EQ_R1), V_NO,           RG_WORD,      G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_WORD_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Sub4[] = {
/************************/

/* huge pointer subtractions come through here since the result type is I4*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_OP2PTR,       RG_,          R_PTSUB,        FU_NO ),
_OE( _Bin(   C,    ANY,  ANY,  NONE ), NVI(V_OP1ZERO), RG_DOUBLE,    R_MAKENEG,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_DOUBLE,    R_SPLITOP,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Sub8[] = {
/************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  ANY,  ANY, NONE ),  V_NO,           RG_8,         R_SPLIT8BIN,    FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    SubCP[] = {
/*************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          R_MAKEU2,       FU_NO ),
};


static  opcode_entry    SubPT[] = {
/*************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  C,    ANY,  NONE ), V_NO,           RG_,          R_MAKEADD,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          R_MAKECALL,     FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          R_NEGADD,       FU_NO ),
};


/* Binary Logical 8 bytes operations: AND, OR, XOR */
static  opcode_entry    Log8[] = {
/************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_8,         R_SPLIT8BIN,    FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Or1[] = {
/***********************/
/* WATCH OUT: Both OR and XOR come through this table!*/
/* optimizing reductions*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2ZERO), RG_BYTE,      R_MAKEMOVE,     FU_NO ),

/* instructions that we can generate*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _BinSC( R,    C,    R,    EQ_R1 ),V_NO,           RG_BYTE_ACC,  G_AC,           FU_ALU1 ),
_OE( _BinSC( R,    C,    R,    EQ_R1 ),V_NO,           RG_BYTE,      G_RC,           FU_ALU1 ),
_OE( _BinSC( M,    C,    M,    EQ_R1 ),V_NO,           RG_,          G_MC,           FU_ALU1 ),
_OE( _BinSC( R,    R,    R,    EQ_R1 ),V_NO,           RG_BYTE,      G_RR2,          FU_ALU1 ),
_OE( _BinSC( R,    M,    R,    EQ_R1 ),V_NO,           RG_BYTE,      G_RM2,          FU_ALU1 ),
_OE( _BinSC( M,    R,    M,    EQ_R1 ),V_NO,           RG_BYTE,      G_MR2,          FU_ALU1 ),

/* simplifying reductions*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   C,    R|M|U,ANY,  NONE ), V_NO,           RG_BYTE,      R_SWAPOPS,      FU_NO ),
_OE( _Bin(   R|M,  R|M|C,R|M,  EQ_R1 ),V_NO,           RG_BYTE,      R_LOADOP2,      FU_NO ),
_OE( _Bin(   R|M|C,R|M,  R|M,  EQ_R2 ),V_NO,           RG_BYTE,      R_SWAPOPS,      FU_NO ),
_OE( _Bin(   R,    ANY,  ANY,  NONE ), V_SWAP_GOOD,    RG_BYTE,      R_SWAPOPS,      FU_NO ),
_OE( _Bin(   R|M|C,R|M|C,R|M,  NONE ), V_NO,           RG_BYTE,      R_USEREGISTER,  FU_NO ),
_OE( _Bin(   R|C,  ANY,  ANY,  EQ_R2), V_NO,           RG_BYTE,      G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  R|C,  ANY,  EQ_R1), V_NO,           RG_BYTE,      G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_BYTE_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Or2[] = {
/***********************/
/* WATCH OUT: Both OR and XOR come through this table!*/
/* optimizing reductions*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2ZERO), RG_WORD,      R_MAKEMOVE,     FU_NO ),
_OE( _Bin(   R|M|U,C,    R|M|U,EQ_R1), NVI(V_OP2LOW_B_ZERO),RG_TWOBYTE,R_CYPHIGH,    FU_NO ),
_OE( _Bin(   R|M|U,C,    R|M|U,EQ_R1), NVI(V_OP2HIGH_B_ZERO),RG_TWOBYTE,R_CYPLOW,    FU_NO ),

/* instructions that we can generate*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _BinSC( R,    C,    R,    EQ_R1 ),V_NO,           RG_WORD_ACC,  G_AC,           FU_ALU1 ),
_OE( _BinSC( R,    C,    R,    EQ_R1 ),V_NO,           RG_WORD,      G_RC,           FU_ALU1 ),
_OE( _BinSC( M,    C,    M,    EQ_R1 ),V_NO,           RG_,          G_MC,           FU_ALU1 ),
_OE( _BinSC( R,    R,    R,    EQ_R1 ),V_NO,           RG_WORD,      G_RR2,          FU_ALU1 ),
_OE( _BinSC( R,    M,    R,    EQ_R1 ),V_NO,           RG_WORD,      G_RM2,          FU_ALU1 ),
_OE( _BinSC( M,    R,    M,    EQ_R1 ),V_NO,           RG_WORD,      G_MR2,          FU_ALU1 ),

/* simplifying reductions*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   C,    R|M|U,ANY,  NONE ), V_NO,           RG_WORD,      R_SWAPOPS,      FU_NO ),
_OE( _Bin(   R|M,  R|M|C,R|M,  EQ_R1 ),V_NO,           RG_WORD,      R_LOADOP2,      FU_NO ),
_OE( _Bin(   R|M|C,R|M,  R|M,  EQ_R2 ),V_NO,           RG_WORD,      R_SWAPOPS,      FU_NO ),
_OE( _Bin(   R,    ANY,  ANY,  NONE ), V_SWAP_GOOD,    RG_WORD,      R_SWAPOPS,      FU_NO ),
_OE( _Bin(   R|M|C,R|M|C,R|M,  NONE ), V_NO,           RG_WORD,      R_USEREGISTER,  FU_NO ),
_OE( _Bin(   ANY,  R|C,  ANY,  EQ_R1), V_NO,           RG_WORD,      G_UNKNOWN,      FU_NO ),
_OE( _Bin(   R|C,  ANY,  ANY,  EQ_R2), V_NO,           RG_WORD,      G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_WORD_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Or4[] = {
/***********************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
/* WATCH OUT: Both OR and XOR come through this table!*/
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_DOUBLE,    R_SPLITOP,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    And1[] = {
/************************/

/* optimizing reductions*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2ZERO), RG_BYTE,      R_MOVOP2,       FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2LOW_B_FF),RG_BYTE,   R_MAKEMOVE,     FU_NO ),

/* instructions that we can generate*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _BinSC( R,    C,    R,    EQ_R1 ),V_NO,           RG_BYTE_ACC,  G_AC,           FU_ALU1 ),
_OE( _BinSC( R,    C,    R,    EQ_R1 ),V_NO,           RG_BYTE,      G_RC,           FU_ALU1 ),
_OE( _BinSC( M,    C,    M,    EQ_R1 ),V_NO,           RG_,          G_MC,           FU_ALU1 ),
_OE( _BinSC( R,    R,    R,    EQ_R1 ),V_NO,           RG_BYTE,      G_RR2,          FU_ALU1 ),
_OE( _BinSC( R,    M,    R,    EQ_R1 ),V_NO,           RG_BYTE,      G_RM2,          FU_ALU1 ),
_OE( _BinSC( M,    R,    M,    EQ_R1 ),V_NO,           RG_BYTE,      G_MR2,          FU_ALU1 ),

/* simplifying reductions*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   C,    R|M|U,ANY,  NONE ), V_NO,           RG_BYTE,      R_SWAPOPS,      FU_NO ),
_OE( _Bin(   R|M,  R|M|C,R|M,  EQ_R1 ),V_NO,           RG_BYTE,      R_LOADOP2,      FU_NO ),
_OE( _Bin(   R|M|C,R|M,  R|M,  EQ_R2 ),V_NO,           RG_BYTE,      R_SWAPOPS,      FU_NO ),
_OE( _Bin(   R,    ANY,  ANY,  NONE ), V_SWAP_GOOD,    RG_BYTE,      R_SWAPOPS,      FU_NO ),
_OE( _Bin(   R|M|C,R|M|C,R|M,  NONE ), V_NO,           RG_BYTE,      R_USEREGISTER,  FU_NO ),
_OE( _Bin(   ANY,  R|C,  ANY,  EQ_R1), V_NO,           RG_BYTE,      G_UNKNOWN,      FU_NO ),
_OE( _Bin(   R|C,  ANY,  ANY,  EQ_R2), V_NO,           RG_BYTE,      G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_BYTE_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    And2[] = {
/************************/

/* optimizing reductions*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2ZERO), RG_WORD,      R_MOVOP2,       FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2LOW_W_FFFF),RG_WORD, R_MAKEMOVE,     FU_NO ),
_OE( _Bin(   R|M|U,C,    R|M|U,EQ_R1), NVI(V_OP2LOW_B_FF),RG_TWOBYTE,R_CYPHIGH,      FU_NO ),
_OE( _Bin(   R|M|U,C,    R|M|U,EQ_R1), NVI(V_OP2HIGH_B_FF),RG_TWOBYTE,R_CYPLOW,      FU_NO ),

/* instructions that we can generate*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _BinSC( R,    C,    R,    EQ_R1 ),V_NO,           RG_WORD_ACC,  G_AC,           FU_ALU1 ),
_OE( _BinSC( R,    C,    R,    EQ_R1 ),V_NO,           RG_WORD,      G_RC,           FU_ALU1 ),
_OE( _BinSC( M,    C,    M,    EQ_R1 ),V_NO,           RG_,          G_MC,           FU_ALU1 ),
_OE( _BinSC( R,    R,    R,    EQ_R1 ),V_NO,           RG_WORD,      G_RR2,          FU_ALU1 ),
_OE( _BinSC( R,    M,    R,    EQ_R1 ),V_NO,           RG_WORD,      G_RM2,          FU_ALU1 ),
_OE( _BinSC( M,    R,    M,    EQ_R1 ),V_NO,           RG_WORD,      G_MR2,          FU_ALU1 ),

/* simplifying reductions*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   C,    R|M|U,ANY,  NONE ), V_NO,           RG_WORD,      R_SWAPOPS,      FU_NO ),
_OE( _Bin(   R|M,  R|M|C,R|M,  EQ_R1 ),V_NO,           RG_WORD,      R_LOADOP2,      FU_NO ),
_OE( _Bin(   R|M|C,R|M,  R|M,  EQ_R2 ),V_NO,           RG_WORD,      R_SWAPOPS,      FU_NO ),
_OE( _Bin(   R,    ANY,  ANY,  NONE ), V_SWAP_GOOD,    RG_WORD,      R_SWAPOPS,      FU_NO ),
_OE( _Bin(   R|M|C,R|M|C,R|M,  NONE ), V_NO,           RG_WORD,      R_USEREGISTER,  FU_NO ),
_OE( _Bin(   ANY,  R|C,  ANY,  EQ_R1), V_NO,           RG_WORD,      G_UNKNOWN,      FU_NO ),
_OE( _Bin(   R|C,  ANY,  ANY,  EQ_R2), V_NO,           RG_WORD,      G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_WORD_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    And4[] = {
/************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   C,    R|M|U,ANY,  NONE ), V_NO,           RG_DOUBLE,    R_SWAPOPS,      FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  EQ_R1), NVI(V_OP2HIGH_W_FFFF),RG_DOUBLE,R_CYPLOW,     FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  EQ_R1), NVI(V_OP2LOW_W_FFFF),RG_DOUBLE,R_CYPHIGH,     FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_DOUBLE,    R_SPLITOP,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Rtn4C[] = {
/*************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   R,    R,    R,    BOTH_EQ ),V_NO,         RG_4CRTN,     R_MAKECALL,     FU_NO ),
_OE( _Bin(   ANY,  R,    R,    EQ_R2 ),V_NO,           RG_4CRTN,     R_SWAPOPS,      FU_NO ),

/* Fall into Rtn4 table*/
/**** NB. Rtn4 points here ****/
/* static  opcode_entry    Rtn4[] = { */
/************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          R_MAKECALL,     FU_NO ),
};

/* Point at where Rtn4 used to start */
/*************************/
#define Rtn4 &Rtn4C[2]

static  opcode_entry    Mul1[] = {
/************************/

/* instructions we can generate*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   R,    R,    R,    NONE ), V_NO,           RG_BYTE_MUL,  G_R2,           FU_IMUL ),
_OE( _Bin(   R,    M,    R,    NONE ), V_NO,           RG_BYTE_MUL,  G_M2,           FU_IMUL ),

/* simplifying reductions*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   R,    C,    R,    NONE ), V_NO,           RG_BYTE_MUL,  R_MOVOP2TEMP,   FU_NO ),
_OE( _Bin(   R,    U,    R,    NONE ), V_NO,           RG_BYTE_MUL,  G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_BYTE_MUL,  R_MULREGISTER,  FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Mul2[] = {
/************************/

/* instructions we can generate*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
//_OE( _Bin(   R,    C,    R,    NONE ), V_OP2ONE,       RG_WORD_MUL,  R_MULSAVEFACE,  FU_NO ),
_OE( _Bin(   R,    C,    R,    NONE ), V_80186,        RG_WORD,      G_186RMUL,      FU_IMUL ),
_OE( _Bin(   M,    C,    R,    NONE ), V_80186,        RG_WORD,      G_186MUL,       FU_IMUL ),

/* simplifying reductions*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   U,    C,    R,    NONE ), V_80186,        RG_WORD,      G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  C,    M,    NONE ), V_80186,        RG_WORD,      R_RESREG,       FU_NO ),
_OE( _Bin(   ANY,  C,    U|M,  NONE ), V_80186,        RG_WORD_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  C,    R,    NONE ), V_80186,        RG_WORD_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Bin(   C,    ANY,  ANY,  NONE ), V_80186,        RG_WORD,      R_SWAPOPS,      FU_NO ),

/* instructions we can generate*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   R,    R,    R,    EQ_R1 ),V_80386,        RG_WORD,      G_0FRR2,        FU_IMUL ),
_OE( _Bin(   R,    M,    R,    EQ_R1 ),V_80386,        RG_WORD,      G_0FRM2,        FU_IMUL ),

/* simplifying reductions*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   C,    R|M|U,ANY,  NONE ), V_80386,        RG_WORD,      R_SWAPOPS,      FU_NO ),
_OE( _Bin(   R|M|C,R,    R,    EQ_R2 ),V_80386,        RG_WORD,      R_SWAPOPS,      FU_NO ),
_OE( _Bin(   R,    ANY,  ANY,  NONE ), V_SWAP_GOOD_386,RG_WORD,      R_SWAPOPS,      FU_NO ),
_OE( _Bin(   R|M|C,R|M|C,R|M,  NONE ), V_80386,        RG_WORD,      R_USEREGISTER,  FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_80386,        RG_WORD_NEED, G_UNKNOWN,      FU_NO ),

/* instructions we can generate*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   R,    R,    R,    NONE ), V_NO,           RG_WORD_MUL,  G_R2,           FU_IMUL ),
_OE( _Bin(   R,    M,    R,    NONE ), V_NO,           RG_WORD_MUL,  G_M2,           FU_IMUL ),

/* simplifying reductions*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   R,    C,    R,    NONE ), V_NO,           RG_WORD_MUL,  R_MOVOP2TEMP,   FU_NO ),
_OE( _Bin(   R,    U,    R,    NONE ), V_NO,           RG_WORD_MUL,  G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_WORD_MUL,  R_MULREGISTER,  FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    ExtMul[] = {
/**************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          R_MAKECYPMUL,   FU_NO ),
};


static  opcode_entry    Div1[] = {
/************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   R,    C,    R,    NONE ), V_OP2TWO,       RG_BYTE_DIV,  G_DIV2,         FU_IDIV ),
_OE( _Bin(   R,    C,    R,    NONE ), V_OP2POW2_286,  RG_BYTE_DIV,  G_POW2DIV_286,  FU_IDIV ),
_OE( _Bin(   R,    C,    R,    NONE ), V_OP2POW2,      RG_BYTE_DIVPOW2,G_POW2DIV,    FU_IDIV ),
_OE( _Bin(   ANY,  C,    ANY,  NONE ), V_OP2POW2,      RG_BYTE_DIV,  R_DIVREGISTER,  FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  NONE ), V_NO,           RG_BYTE_DIV,  R_MOVOP2TEMP,   FU_NO ),
_OE( _Bin(   R,    R,    R,    NONE ), V_NO,           RG_BYTE_DIV,  G_R2,           FU_IDIV ),
_OE( _Bin(   R,    M,    R,    NONE ), V_NO,           RG_BYTE_DIV,  G_M2,           FU_IDIV ),
_OE( _Bin(   R,    U|M,  R,    NONE ), V_DIV_BUG,      RG_BYTE_DIV,  R_LOADOP2,      FU_NO ),
_OE( _Bin(   R,    U,    R,    NONE ), V_NO,           RG_BYTE_DIV,  G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_BYTE_DIV,  R_DIVREGISTER,  FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Div2[] = {
/************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
//_OE( _Bin(   R,    C,    R,    NONE ), V_OP2ONE,       RG_WORD_DIV,  R_SAVEFACE,     FU_NO ),
_OE( _Bin(   R,    C,    R,    NONE ), V_OP2TWO,       RG_WORD_DIV,  G_DIV2,         FU_IDIV ),
_OE( _Bin(   R,    C,    R,    NONE ), V_OP2POW2_286,  RG_WORD_DIV,  G_POW2DIV_286,  FU_IDIV ),
_OE( _Bin(   R,    C,    R,    NONE ), V_OP2POW2,      RG_WORD_DIVPOW2,G_POW2DIV,    FU_IDIV ),
_OE( _Bin(   ANY,  C,    ANY,  NONE ), V_OP2POW2,      RG_WORD_DIV,  R_DIVREGISTER,  FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  NONE ), V_NO,           RG_WORD_DIV,  R_MOVOP2TEMP,   FU_NO ),
_OE( _Bin(   R,    R,    R,    NONE ), V_NO,           RG_WORD_DIV,  G_R2,           FU_IDIV ),
_OE( _Bin(   R,    M,    R,    NONE ), V_NO,           RG_WORD_DIV,  G_M2,           FU_IDIV ),
_OE( _Bin(   R,    U|M,  R,    NONE ), V_DIV_BUG,      RG_WORD_DIV,  R_LOADOP2,      FU_NO ),
_OE( _Bin(   R,    U,    R,    NONE ), V_NO,           RG_WORD_DIV,  G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_WORD_DIV,  R_DIVREGISTER,  FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Mod1[] = {
/************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  C,    ANY,  NONE ), V_NO,           RG_BYTE_MOD,  R_MOVOP2TEMP,   FU_NO ),
_OE( _Bin(   R,    R,    R,    NONE ), V_NO,           RG_BYTE_MOD,  G_R2,           FU_IDIV ),
_OE( _Bin(   R,    M,    R,    NONE ), V_NO,           RG_BYTE_MOD,  G_M2,           FU_IDIV ),
_OE( _Bin(   R,    U|M,  R,    NONE ), V_DIV_BUG,      RG_BYTE_MOD,  R_LOADOP2,      FU_NO ),
_OE( _Bin(   R,    U,    R,    NONE ), V_NO,           RG_BYTE_MOD,  G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_BYTE_MOD,  R_DIVREGISTER,  FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Mod2[] = {
/************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
//_OE( _Bin(   R,    C,    R,    NONE ), V_OP2ONE,       RG_WORD_MOD,  R_SAVEFACE,     FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  NONE ), V_NO,           RG_WORD_MOD,  R_MOVOP2TEMP,   FU_NO ),
_OE( _Bin(   R,    R,    R,    NONE ), V_NO,           RG_WORD_MOD,  G_R2,           FU_IDIV ),
_OE( _Bin(   R,    M,    R,    NONE ), V_NO,           RG_WORD_MOD,  G_M2,           FU_IDIV ),
_OE( _Bin(   R,    U|M,  R,    NONE ), V_DIV_BUG,      RG_WORD_MOD,  R_LOADOP2,      FU_NO ),
_OE( _Bin(   R,    U,    R,    NONE ), V_NO,           RG_WORD_MOD,  G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_WORD_MOD,  R_DIVREGISTER,  FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Shft1[] = {
/*************************/

/* optimizing reductions*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  C,    ANY,  NONE ), V_OP2NEG,       RG_BYTE_SHIFT,R_CHANGESHIFT,  FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2ZERO), RG_BYTE,      R_MAKEMOVE,     FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  NONE ), V_SHIFT2BIG,    RG_BYTE_SHIFT,R_FIXSHIFT,     FU_NO ),

/* instructions we can generate*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   C,    C,    ANY,  EQ_R1 ),V_NO,           RG_BYTE,      R_MOVOP1TEMP,   FU_NO ),
_OE( _Bin(   R,    C,    R,    EQ_R1 ),V_LSHONE,       RG_BYTE,      R_ADDRR,        FU_NO ),
_OE( _Bin(   R,    C,    R,    EQ_R1 ),V_OP2ONE,       RG_BYTE,      G_R1SHIFT,      FU_ALU1 ),
_OE( _Bin(   M,    C,    M,    EQ_R1 ),V_OP2ONE,       RG_,          G_1SHIFT,       FU_ALU1 ),
_OE( _Bin(   R,    C,    R,    EQ_R1 ),V_80186,        RG_BYTE,      G_RNSHIFT,      FU_ALU1 ),
_OE( _Bin(   M,    C,    M,    EQ_R1 ),V_80186,        RG_,          G_NSHIFT,       FU_ALU1 ),
_OE( _Bin(   R,    C,    R,    EQ_R1 ),V_OP2TWO,       RG_BYTE,      R_DOUBLEHALF,   FU_NO ),
_OE( _Bin(   M,    C,    M,    EQ_R1 ),V_OP2TWO,       RG_,          R_DOUBLEHALF,   FU_NO ),
_OE( _Bin(   R,    R,    R,    EQ_R1 ),V_NO,           RG_BYTE_SHIFT,G_RCLSHIFT,     FU_ALU1 ),
_OE( _Bin(   M,    R,    M,    EQ_R1 ),V_NO,           RG_BYTE_SHIFT,G_CLSHIFT,      FU_ALU1 ),

/* simplifying reductions*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   R|M|C,C,    R|M,  NONE ), V_80186,        RG_BYTE_SHIFT,R_OP1RESTEMP,   FU_NO ),
_OE( _Bin(   R|M|C,C,    R|M,  NONE ), V_OP2ONE,       RG_BYTE_SHIFT,R_OP1RESTEMP,   FU_NO ),
_OE( _Bin(   R|M|C,C,    R|M,  NONE ), V_OP2TWO,       RG_BYTE_SHIFT,R_OP1RESTEMP,   FU_NO ),
_OE( _Bin(   R|M|C,R,    R|M,  NONE ), V_NO,           RG_BYTE_SHIFT,R_OP1RESTEMP,   FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  EQ_R1), V_OP2ONE,       RG_BYTE,      G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  NONE ), V_OP2ONE,       RG_BYTE_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  EQ_R1), V_OP2TWO,       RG_BYTE,      G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  NONE ), V_OP2TWO,       RG_BYTE_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  EQ_R1), V_80186,        RG_BYTE,      G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  NONE ), V_80186,        RG_BYTE_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  R,    ANY,  EQ_R1), V_NO,           RG_BYTE_SHIFT,G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  R,    ANY,  NONE ), V_NO,           RG_BYTE_SHIFT_NEED,G_UNKNOWN, FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          R_OP2CL,        FU_NO ),
};

static  opcode_entry    Shft2[] = {
/*************************/

/* optimizing reductions*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  C,    ANY,  NONE ), V_OP2NEG,       RG_WORD_SHIFT,R_CHANGESHIFT,  FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2ZERO), RG_WORD,      R_MAKEMOVE,     FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  NONE ), V_SHIFT2BIG,    RG_WORD_SHIFT,R_FIXSHIFT,     FU_NO ),
_OE( _Bin(   R,    C,    R,    NONE ), NVI(V_CYP2SHIFT),RG_TWOBYTE,  R_CYPSHIFT,     FU_NO ),
_OE( _Bin(   M|U,  C,    M|U,  NONE ), NVI(V_CYP2SHIFT),RG_WORD,     R_CYPSHIFT,     FU_NO ),

/* instructions we can generate*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   C,    C,    ANY,  EQ_R1 ),V_NO,           RG_WORD,      R_MOVOP1TEMP,   FU_NO ),
_OE( _Bin(   R,    C,    R,    EQ_R1 ),V_LSHONE,       RG_WORD,      R_ADDRR,        FU_NO ),
_OE( _Bin(   R,    C,    R,    EQ_R1 ),V_OP2ONE,       RG_WORD,      G_R1SHIFT,      FU_ALU1 ),
_OE( _Bin(   M,    C,    M,    EQ_R1 ),V_OP2ONE,       RG_,          G_1SHIFT,       FU_ALU1 ),
_OE( _Bin(   R,    C,    R,    EQ_R1 ),V_80186,        RG_WORD,      G_RNSHIFT,      FU_ALU1 ),
_OE( _Bin(   M,    C,    M,    EQ_R1 ),V_80186,        RG_,          G_NSHIFT,       FU_ALU1 ),
_OE( _Bin(   R,    C,    R,    EQ_R1 ),V_OP2TWO,       RG_WORD,      R_DOUBLEHALF,   FU_NO ),
_OE( _Bin(   M,    C,    M,    EQ_R1 ),V_OP2TWO,       RG_,          R_DOUBLEHALF,   FU_NO ),
_OE( _Bin(   R,    R,    R,    EQ_R1 ),V_NO,           RG_WORD_SHIFT,G_RCLSHIFT,     FU_ALU1 ),
_OE( _Bin(   M,    R,    M,    EQ_R1 ),V_NO,           RG_WORD_SHIFT,G_CLSHIFT,      FU_ALU1 ),

/* simplifying reductions*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   R|M|C,C,    R|M,  NONE ), V_80186,        RG_WORD_SHIFT,R_OP1RESTEMP,   FU_NO ),
_OE( _Bin(   R|M|C,C,    R|M,  NONE ), V_OP2ONE,       RG_WORD_SHIFT,R_OP1RESTEMP,   FU_NO ),
_OE( _Bin(   R|M|C,C,    R|M,  NONE ), V_OP2TWO,       RG_WORD_SHIFT,R_OP1RESTEMP,   FU_NO ),
_OE( _Bin(   R|M|C,R,    R|M,  NONE ), V_NO,           RG_WORD_SHIFT,R_OP1RESTEMP,   FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  EQ_R1), V_OP2ONE,       RG_WORD,      G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  NONE ), V_OP2ONE,       RG_WORD_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  EQ_R1), V_OP2TWO,       RG_WORD,      G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  NONE ), V_OP2TWO,       RG_WORD_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  EQ_R1), V_80186,        RG_WORD,      G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  NONE ), V_80186,        RG_WORD_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  R,    ANY,  EQ_R1), V_NO,           RG_WORD_SHIFT,G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  R,    ANY,  NONE ), V_NO,           RG_WORD_SHIFT_NEED,G_UNKNOWN, FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          R_OP2CL,        FU_NO ),
};


/**/
/* NEVER, NEVER touch this table unless you are feeling VERY brave (j.d)*/
static  opcode_entry    Shft4[] = {
/*************************/

/* optimizing reductions*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  C,    ANY,  NONE ), V_OP2NEG,       RG_DBL_SHIFT, R_CHANGESHIFT,  FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2ZERO), RG_DOUBLE,    R_MAKEMOVE,     FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  NONE ), NVI(V_CYP4SHIFT),RG_DOUBLE,   R_CYPSHIFT,     FU_NO ),

/* instructions we can generate*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   C,    C,    ANY,  EQ_R1 ),V_NO,           RG_DOUBLE,    R_MOVOP1TEMP,   FU_NO ),
_OE( _Bin(   R,    C,    R,    EQ_R1 ),V_LSHONE,       RG_DOUBLE,    R_ADDRR,        FU_NO ),
_OE( _Bin(   R,    C,    R,    EQ_R1 ),V_OP2ONE,       RG_DOUBLE,    G_4RSHIFT,      FU_ALU1 ),
_OE( _Bin(   M,    C,    M,    EQ_R1 ),V_OP2ONE,       RG_,          G_4SHIFT,       FU_ALU1 ),
_OE( _Bin(   R,    C,    R,    EQ_R1 ),V_OP2TWO,       RG_DOUBLE,    R_DOUBLEHALF,   FU_NO ),
_OE( _Bin(   M,    C,    M,    EQ_R1 ),V_OP2TWO,       RG_,          R_DOUBLEHALF,   FU_NO ),
_OE( _Bin(   R,    R,    R,    EQ_R1 ),V_NO,           RG_DBL_SHIFT, G_RCXSHIFT,     FU_ALU1 ),
_OE( _Bin(   R,    C,    R,    EQ_R1 ),V_NO,           RG_DBL_SHIFT_NEED,G_RCXSHIFT, FU_ALU1 ),
_OE( _Bin(   M,    R,    M,    EQ_R1 ),V_NO,           RG_DBL_SHIFT, G_CXSHIFT,      FU_ALU1 ),
_OE( _Bin(   M,    C,    M,    EQ_R1 ),V_NO,           RG_DBL_SHIFT_NEED,G_CXSHIFT,  FU_ALU1 ),

/* simplifying reductions*/

/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   R,    C,    R,    NONE ), V_BYTESHIFT,    RG_DBL_SHIFT, R_BYTESHIFT,    FU_NO ),
_OE( _Bin(   R,    C,    M,    NONE ), V_BYTESHIFT,    RG_DBL_SHIFT, R_BYTESHIFT,    FU_NO ),
_OE( _Bin(   M,    C,    R,    NONE ), V_BYTESHIFT,    RG_DBL_SHIFT, R_BYTESHIFT,    FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  EQ_R1 ),V_OP2ONE,       RG_DOUBLE_NEED,G_UNKNOWN,     FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  EQ_R1 ),V_OP2TWO,       RG_DOUBLE_NEED,G_UNKNOWN,     FU_NO ),
_OE( _Bin(   ANY,  R,    ANY,  EQ_R1 ),V_NO,           RG_DBL_SHIFT, G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  C,    ANY,  EQ_R1 ),V_NO,           RG_DBL_SHIFT_NEED,G_UNKNOWN,  FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  EQ_R1 ),V_NO,           RG_,          R_OP2CX,        FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_DBL_SHIFT, R_OP1RESTEMP,   FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    TestOrCmp1[] = {
/******************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   C,    C,    ANY,  EQ_R1 ),V_NO,           RG_DOUBLE,    R_MOVOP1TEMP,   FU_NO ),

/*           op1   op2                 verify          reg           gen             fu  */
_OE( _Side(  R,    U ),                V_CONSTTEMP,    RG_BYTE,      R_TEMP2CONST,   FU_NO ),
_OE( _Side(  U,    R ),                V_CONSTTEMP,    RG_BYTE,      R_TEMP2CONST,   FU_NO ),
_OE( _Side(  ANY,  ANY ),              NVI(V_CMPTRUE), RG_,          R_CMPTRUE,      FU_NO ),
_OE( _Side(  ANY,  ANY ),              NVI(V_CMPFALSE),RG_,          R_CMPFALSE,     FU_NO ),

/* instructions we can generate*/

/*           op1   op2                 verify          reg           gen             fu  */
_OE( _SidCC( R,    R ),                V_NO,           RG_BYTE,      G_RR2,          FU_ALU1 ),
_OE( _SidCC( R,    M ),                V_NO,           RG_BYTE,      G_RM2,          FU_ALU1 ),
_OE( _SidCC( R,    C ),                V_OP2ZERO,      RG_BYTE,      G_TEST,         FU_ALU1 ),
_OE( _SidCC( R,    C ),                V_NO,           RG_BYTE_ACC,  G_AC,           FU_ALU1 ),
_OE( _SidCC( R,    C ),                V_NO,           RG_BYTE,      G_RC,           FU_ALU1 ),
_OE( _SidCC( M,    C ),                V_NO,           RG_,          G_MC,           FU_ALU1 ),

/* simplifying reductions*/

/*           op1   op2                 verify          reg           gen             fu  */
_OE( _Side(  C,    C ),                V_NO,           RG_BYTE,      R_MOVOP1REG,    FU_NO ),
_OE( _Side(  C,    ANY ),              V_NO,           RG_BYTE,      R_SWAPCMP,      FU_NO ),
_OE( _Side(  M,    R ),                V_NO,           RG_BYTE,      R_SWAPCMP,      FU_NO ),
_OE( _Side(  M,    M ),                V_NO,           RG_BYTE,      R_MOVOP1REG,    FU_NO ),
_OE( _Side(  ANY,  R|C ),              V_NO,           RG_BYTE,      G_UNKNOWN,      FU_NO ),
_OE( _Side(  R|C,  ANY ),              V_NO,           RG_BYTE,      G_UNKNOWN,      FU_NO ),
_OE( _Side(  ANY,  ANY ),              V_NO,           RG_BYTE_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Test2[] = {
/*************************/

/* optimizing reductions*/

/*           op1   op2                 verify          reg           gen             fu  */
_OE( _Side(  R|M|U,C ),                NVI(V_OP2LOW_B_ZERO),RG_TWOBYTE,R_CYPHIGH,    FU_NO ),
_OE( _Side(  R|M|U,C ),                NVI(V_OP2HIGH_B_ZERO),RG_TWOBYTE,R_CYPLOW,    FU_NO ),
_OE( _Side(  R,    U ),                V_CONSTTEMP,    RG_WORD,      R_TEMP2CONST,   FU_NO ),
_OE( _Side(  U,    R ),                V_CONSTTEMP,    RG_WORD,      R_TEMP2CONST,   FU_NO ),
_OE( _Side(  ANY,  C ),                V_CMPTRUE,      RG_,          R_CMPTRUE,      FU_NO ),
_OE( _Side(  ANY,  C ),                V_CMPFALSE,     RG_,          R_CMPFALSE,     FU_NO ),

/* instructions we can generate*/

/*           op1   op2                 verify          reg           gen             fu  */
_OE( _SidCC( R,    R ),                V_NO,           RG_WORD,      G_RR2,          FU_ALU1 ),
_OE( _SidCC( R,    M ),                V_NO,           RG_WORD,      G_RM2,          FU_ALU1 ),
_OE( _SidCC( R,    C ),                V_NO,           RG_WORD_ACC,  G_AC,           FU_ALU1 ),
_OE( _SidCC( R,    C ),                V_NO,           RG_WORD,      G_RC,           FU_ALU1 ),
_OE( _SidCC( M,    C ),                V_NO,           RG_,          G_MC,           FU_ALU1 ),

/* simplifying reductions*/

/*           op1   op2                 verify          reg           gen             fu  */
_OE( _Side(  C,    C ),                V_NO,           RG_WORD,      R_MOVOP1REG,    FU_NO ),
_OE( _Side(  C,    ANY ),              V_NO,           RG_WORD,      R_SWAPCMP,      FU_NO ),
_OE( _Side(  M,    R ),                V_NO,           RG_WORD,      R_SWAPCMP,      FU_NO ),
_OE( _Side(  M,    M ),                V_NO,           RG_WORD,      R_MOVOP1REG,    FU_NO ),
_OE( _Side(  ANY,  R|C ),              V_NO,           RG_WORD,      G_UNKNOWN,      FU_NO ),
_OE( _Side(  R|C,  ANY ),              V_NO,           RG_WORD,      G_UNKNOWN,      FU_NO ),
_OE( _Side(  ANY,  ANY ),              V_NO,           RG_WORD_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Side(  ANY,  ANY ),              V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Test4[] = {
/*************************/
/*           op1   op2                 verify          reg           gen             fu  */
_OE( _Side(  ANY,  C   ),              NVI(V_OP2LOW_W_ZERO),RG_,     R_CYPHIGH,      FU_NO ),
_OE( _Side(  ANY,  C   ),              NVI(V_OP2HIGH_W_ZERO),RG_,    R_CYPLOW,       FU_NO ),
_OE( _Side(  C,    R|M|U ),            V_NO,           RG_,          R_SWAPCMP,      FU_NO ),
_OE( _Side(  ANY,  ANY ),              V_NO,           RG_,          R_SPLITCMP,     FU_NO ),
};


static  opcode_entry    Test8[] = {
/*************************/
/*           op1   op2                 verify          reg           gen             fu  */
_OE( _Side(  C,    R|M|U ),            V_NO,           RG_,          R_SWAPCMP,      FU_NO ),
_OE( _Side(  ANY,  ANY ),              V_NO,           RG_8,         R_SPLIT8TST,    FU_NO ),
_OE( _Side(  ANY,  ANY ),              V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Cmp2[] = {
/************************/
/*           op1   op2                 verify          reg           gen             fu  */
_OE( _Side(  R,    U ),                V_CONSTTEMP,    RG_WORD,      R_TEMP2CONST,   FU_NO ),
_OE( _Side(  U,    R ),                V_CONSTTEMP,    RG_WORD,      R_TEMP2CONST,   FU_NO ),
_OE( _Side(  ANY,  ANY ),              NVI(V_CMPTRUE), RG_,          R_CMPTRUE,      FU_NO ),
_OE( _Side(  ANY,  ANY ),              NVI(V_CMPFALSE),RG_,          R_CMPFALSE,     FU_NO ),

/* instructions we can generate*/

/*           op1   op2                 verify          reg           gen             fu  */
_OE( _SidCC( R,    C ),                V_OP2ZERO,      RG_WORD,      G_TEST,         FU_ALU1 ),
_OE( _SidCC( R,    R ),                V_NO,           RG_WORD,      G_RR2,          FU_ALU1 ),
_OE( _SidCC( R,    M ),                V_NO,           RG_WORD,      G_RM2,          FU_ALU1 ),
_OE( _SidCC( R,    C ),                V_NO,           RG_WORD_ACC,  G_AC,           FU_ALU1 ),
_OE( _SidCC( R,    C ),                V_NO,           RG_WORD,      G_RC,           FU_ALU1 ),
_OE( _SidCC( M,    C ),                V_NO,           RG_,          G_MC,           FU_ALU1 ),

/* simplifying reductions*/

/*           op1   op2                 verify          reg           gen             fu  */
_OE( _Side(  ANY,  R ),                V_NO,           RG_SEG_SEG,   R_MOVOP2TEMP,   FU_NO ),
_OE( _Side(  R,    ANY ),              V_NO,           RG_SEG_SEG,   R_MOVOP1TEMP,   FU_NO ),
_OE( _Side(  C,    C ),                V_NO,           RG_WORD,      R_MOVOP1REG,    FU_NO ),
_OE( _Side(  C,    ANY ),              V_NO,           RG_WORD,      R_SWAPCMP,      FU_NO ),
_OE( _Side(  M,    R ),                V_NO,           RG_WORD,      R_SWAPCMP,      FU_NO ),
_OE( _Side(  M,    M ),                V_NO,           RG_WORD,      R_MOVOP1REG,    FU_NO ),
_OE( _Side(  ANY,  R|C ),              V_NO,           RG_WORD,      G_UNKNOWN,      FU_NO ),
_OE( _Side(  R|C,  ANY ),              V_NO,           RG_WORD,      G_UNKNOWN,      FU_NO ),
_OE( _Side(  ANY,  ANY ),              V_NO,           RG_WORD_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Side(  ANY,  ANY ),              V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Cmp4[] = {
/************************/

/*           op1   op2                 verify          reg           gen             fu  */
// 2006-06-01 RomanT: It's not effective. Compare is DoNothing()'ed and
//                    operands are stuck unsplit, poisoning all analysis.
//_OE( _Side(  ANY,  C ),                V_CMPTRUE,      RG_,          R_CMPTRUE,      FU_NO ),
//_OE( _Side(  ANY,  C ),                V_CMPFALSE,     RG_,          R_CMPFALSE,     FU_NO ),
_OE( _Side(  ANY,  C   ),              V_U_TEST,       RG_DOUBLE,    R_U_TEST,       FU_NO ),
_OE( _Side(  C,    R|M|U ),            V_NO,           RG_DOUBLE,    R_SWAPCMP,      FU_NO ),
_OE( _Side(  ANY,  ANY ),              V_NO,           RG_DOUBLE,    R_SPLITCMP,     FU_NO ),
_OE( _Side(  ANY,  ANY ),              V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Cmp8[] = {
/************************/
/*           op1   op2                 verify          reg           gen             fu  */
_OE( _Side(  C,    R|M|U ),            V_NO,           RG_,          R_SWAPCMP,      FU_NO ),
_OE( _Side(  ANY,  ANY ),              V_NO,           RG_8,         R_SPLIT8CMP,    FU_NO ),
_OE( _Side(  ANY,  ANY ),              V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    CmpFS[] = {
/*************************/
/*           op1   op2                 verify          reg           gen             fu  */
_OE( _Side(  ANY,  C ),                V_OP2ZERO,      RG_DBL_OR_PTR,R_HIGHCMP,      FU_NO ),
_OE( _Side(  ANY,  C ),                V_NO,           RG_DBL_OR_PTR,R_FSCONSCMP,    FU_NO ),
_OE( _Side(  C,    R|M|U ),            V_NO,           RG_DBL_OR_PTR,R_SWAPCMP,      FU_NO ),
_OE( _Side(  ANY,  ANY ),              V_NO,           RG_DBL_OR_PTR,R_MAKECMPCALL,  FU_NO ),
_OE( _Side(  ANY,  ANY ),              V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    CmpFD[] = {
/*************************/
/*           op1   op2                 verify          reg           gen             fu  */
_OE( _Side(  ANY,  C ),                V_OP2ZERO,      RG_8,         R_HIGHCMP,      FU_NO ),
_OE( _Side(  ANY,  C ),                V_NO,           RG_8,         R_FDCONSCMP,    FU_NO ),
_OE( _Side(  C,    R|M|U ),            V_NO,           RG_8,         R_SWAPCMP,      FU_NO ),
_OE( _Side(  ANY,  ANY ),              V_NO,           RG_8,         R_MAKECMPCALL,  FU_NO ),
_OE( _Side(  ANY,  ANY ),              V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Move1[] = {
/*************************/

/* optimizing reductions*/

/*           from  to    eq            verify          reg           gen             fu  */
_OE( _UnPP(  ANY,  ANY,  EQ_R1 ),      NVI(V_NO),      RG_,          G_NO,           FU_NO ),
_OE( _UnPP(  M,    M,    NONE ),       V_SAME_LOCN,    RG_,          G_NO,           FU_NO ),

/* instructions we can generate*/

/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    C,    R,    NONE ),       V_OP1ZERO,      RG_BYTE,      R_MAKEXORRR,    FU_NO ),
_OE( _UnPP(  C,    R,    NONE ),       V_NO,           RG_BYTE,      G_MOVRC,        FU_ALU1 ),
_OE( _UnPP(  C,    M,    NONE ),       V_NO,           RG_,          G_MOVMC,        FU_ALU1 ),
_OE( _UnPP(  R,    M,    NONE ),       V_RESLOC,       RG_BYTE_ACC,  G_MOVMA,        FU_ALU1 ),
_OE( _UnPP(  M,    R,    NONE ),       V_OP1LOC,       RG_BYTE_ACC,  G_MOVAM,        FU_ALU1 ),
_OE( _UnPP(  R,    R,    NONE ),       V_NO,           RG_BYTE,      G_RR1,          FU_ALU1 ),
_OE( _UnPP(  M,    R,    NONE ),       V_NO,           RG_BYTE,      G_RM1,          FU_ALU1 ),
_OE( _UnPP(  R,    M,    NONE ),       V_NO,           RG_BYTE,      G_MR1,          FU_ALU1 ),

/* simplifying reductions*/

/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    M,    M,    NONE ),       V_NO,           RG_BYTE,      R_MOVOP1REG,    FU_NO ),
_OE( _Un(    ANY,  R,    NONE ),       V_NO,           RG_BYTE,      G_UNKNOWN,      FU_NO ),
_OE( _Un(    R|C,  ANY,  NONE ),       V_NO,           RG_BYTE,      G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_BYTE_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

static  opcode_entry    Move2CC[] = {
/***************************/

/* optimizing reductions*/

/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    C,    R,    NONE ),       V_OP1ZERO,      RG_WORD,      R_MAKEXORRR,    FU_NO ),

/* fall through into move2 table*/
/**** NB. Move2 points here ****/
/* opcode_entry    Move2[]; */
/*************************/

/* optimizing reductions*/

/*           from  to    eq            verify          reg           gen             fu  */
_OE( _UnPP(  ANY,  ANY,  EQ_R1 ),      NVI(V_NO),      RG_,          G_NO,           FU_NO ),
_OE( _UnPP(  M,    M,    NONE ),       V_SAME_LOCN,    RG_,          G_NO,           FU_NO ),

/* instructions we can generate*/

/*           from  to    eq            verify          reg           gen             fu  */
_OE( _UnPP(  C,    R,    NONE ),       V_NO,           RG_WORD,      G_MOVRC,        FU_ALU1 ),
_OE( _UnPP(  C,    M,    NONE ),       V_NO,           RG_,          G_MOVMC,        FU_ALU1 ),
_OE( _UnPP(  R,    M,    NONE ),       V_RESLOC,       RG_WORD_ACC,  G_MOVMA,        FU_ALU1 ),
_OE( _UnPP(  M,    R,    NONE ),       V_OP1LOC,       RG_WORD_ACC,  G_MOVAM,        FU_ALU1 ),
_OE( _UnPP(  R,    R,    NONE ),       V_NO,           RG_WORD,      G_RR1,          FU_ALU1 ),
_OE( _UnPP(  M,    R,    NONE ),       V_NO,           RG_WORD,      G_RM1,          FU_ALU1 ),
_OE( _UnPP(  R,    M,    NONE ),       V_NO,           RG_WORD,      G_MR1,          FU_ALU1 ),
_OE( _UnPP(  R,    R,    NONE ),       V_NO,           RG_WORD_SEG,  G_SR,           FU_ALU1 ),
_OE( _UnPP(  R,    R,    NONE ),       V_NO,           RG_SEG_WORD,  G_RS,           FU_ALU1 ),
_OE( _UnPP(  R,    M,    NONE ),       V_NO,           RG_SEG_WORD,  G_MS1,          FU_ALU1 ),
_OE( _UnPP(  M,    R,    NONE ),       V_NO,           RG_WORD_SEG,  G_SM1,          FU_ALU1 ),
_OE( _UnPP(  R,    R,    NONE ),       V_SIZE,         RG_SEG_SEG,   G_SEG_SEG,      FU_ALU1 ),
_OE( _UnPP(  R,    R,    NONE ),       V_WORDREG_AVAIL,RG_SEG_SEG,   R_SEG_SEG,      FU_NO ),
_OE( _UnPP(  R,    R,    NONE ),       V_NO,           RG_SEG_SEG,   G_SEG_SEG,      FU_ALU1 ),

/* simplifying reductions*/

/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    C,    R,    NONE ),       V_NO,           RG_WORD_SEG,  R_MOVOP1REG,    FU_NO ),
_OE( _Un(    M,    M,    NONE ),       V_NO,           RG_WORD,      R_MOVOP1REG,    FU_NO ),
_OE( _Un(    R|M|U,R,    NONE ),       V_NO,           RG_ANYWORD,   G_UNKNOWN,      FU_NO ),
_OE( _Un(    R,    ANY,  NONE ),       V_NO,           RG_ANYWORD,   G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_ANYWORD_NEED,G_UNKNOWN,    FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

/* Point at where Move2 used to start */
/*************************/
opcode_entry   *Move2 = &Move2CC[1]; /* used from intel/c/i86split.c */

static opcode_entry    Move4op[] = {
/*************************/
/*           from  to    eq            verify          reg           gen             fu  */
//_OE( _Un(    C,    ANY,  NONE ),       NVI(V_HIGHEQLOW),RG_DOUBLE,   R_HIGHLOWMOVE,  FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_DOUBLE,    R_SPLITMOVE,    FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

/* Pointer to Move4 table (required for symmetry with 386table.c) */
/*************************/
opcode_entry    *Move4 = Move4op; /* used from intel/c/i86split.c */

static  opcode_entry    Move8[] = {
/*************************/
/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    R|M,  R|M,  EQ_R1 ),      NVI(V_NO),      RG_8,         G_NO,           FU_NO ),
_OE( _Un(    M,    M,    NONE ),       V_SAME_LOCN,    RG_,          G_NO,           FU_NO ),
_OE( _Un(    M,    M,    NONE ),       V_NO,           RG_,          R_MAYBSTRMOVE,  FU_NO ),
//_OE( _Un(    R|M|C,R|M,  NONE ),       V_NO,           RG_8,         R_SPLIT8,       FU_NO ),
//_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_8_NEED_WORD,G_UNKNOWN,     FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          R_SPLIT8,       FU_NO ),
};


static  opcode_entry    MoveXX[] = {
/**************************/
/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    M|U,  M|U,  EQ_R1 ),      NVI(V_NO),      RG_,          G_NO,           FU_NO ),
_OE( _UnPP(  M,    M,    NONE ),       V_SAME_LOCN,    RG_,          G_NO,           FU_NO ),
_OE( _Un(    U,    ANY,  NONE ),       V_NO,           RG_,          R_FORCEOP1MEM,  FU_NO ),
_OE( _Un(    ANY,  U,    NONE ),       V_NO,           RG_,          R_FORCERESMEM,  FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          R_MAKESTRMOVE,  FU_NO ),
};


static  opcode_entry    CmpXX[] = {
/*************************/
/*           op1   op2                 verify          reg           gen             fu  */
_OE( _Side(  U,    ANY ),              V_NO,           RG_,          R_FORCEOP1MEM,  FU_NO ),
_OE( _Side(  ANY,  U   ),              V_NO,           RG_,          R_FORCEOP2MEM,  FU_NO ),
_OE( _Side(  ANY,  ANY ),              V_NO,           RG_,          R_MAKESTRCMP,   FU_NO ),
};


opcode_entry    String[] = {
/**************************/
/*           op1   op2                 verify          reg           gen             fu  */
_OE( _Side(  ANY,  ANY ),              V_NO,           RG_,          G_REPOP,        FU_ALU1 ),
};


static  opcode_entry    LoadA4[] = {
/**************************/
/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    M,    ANY,  NONE ),       V_NO,           RG_DBL_OR_PTR,R_LOADLONGADDR, FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_DBL_OR_PTR,R_FORCEOP1MEM,  FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    LoadA[] = {
/*************************/

/* optimizing reductions*/

/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    M,    ANY,  NONE ),       V_OFFSETZERO,   RG_WORD,      R_MOVEINDEX,    FU_NO ),

/* instructions we can generate*/

/*           from  to    eq            verify          reg           gen             fu  */
_OE( _UnPP(  M,    M,    NONE ),       V_OP1ADDR,      RG_,          G_MADDR,        FU_ALU1 ),
_OE( _UnPP(  M,    R,    NONE ),       V_OP1ADDR,      RG_WORD,      G_RADDR,        FU_ALU1 ),
_OE( _UnPP(  M,    R,    NONE ),       V_NO,           RG_WORD,      G_LOADADDR,     FU_ALU1 ),

/* simplifying reductions*/

/*           from  to    eq            verify          reg           gen             fu  */
_OE( _Un(    M,    ANY,  NONE ),       V_OP1ADDR,      RG_WORD,      G_UNKNOWN,      FU_NO ),
_OE( _Un(    M,    M,    NONE ),       V_NO,           RG_WORD,      R_RESREG,       FU_NO ),
_OE( _Un(    M,    ANY,  NONE ),       V_NO,           RG_WORD_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_WORD,      R_FORCEOP1MEM,  FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Not1[] = {
/************************/

/* instructions we can generate*/

/*           op1   op2   eq            verify          reg           gen             fu  */
_OE( _UnPP(  R,    R,    EQ_R1 ),      V_NO,           RG_BYTE,      G_R1,           FU_ALU1 ),
_OE( _UnPP(  M,    M,    EQ_R1 ),      V_NO,           RG_,          G_M1,           FU_ALU1 ),

/*       Reduction routines*/

/*           op1   op2   eq            verify          reg           gen             fu  */
_OE( _Un(    R|M|C,R,    NONE ),       V_NO,           RG_BYTE,      R_MOVOP1RES,    FU_NO ),
_OE( _Un(    R|M|C,M,    NONE ),       V_NO,           RG_BYTE,      R_MOVOP1RES,    FU_NO ),
_OE( _Un(    ANY,  R,    NONE ),       V_NO,           RG_BYTE,      G_UNKNOWN,      FU_NO ),
_OE( _Un(    R,    ANY,  NONE ),       V_NO,           RG_BYTE,      G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  EQ_R1 ),      V_NO,           RG_BYTE,      G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_BYTE_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Not2[] = {
/************************/

/* instructions we can generate*/

/*           op1   op2   eq            verify          reg           gen             fu  */
_OE( _UnPP(  R,    R,    EQ_R1 ),      V_NO,           RG_WORD,      G_R1,           FU_ALU1 ),
_OE( _UnPP(  M,    M,    EQ_R1 ),      V_NO,           RG_,          G_M1,           FU_ALU1 ),

/*       Reduction routines*/

/*           op1   op2   eq            verify          reg           gen             fu  */
_OE( _Un(    R|M|C,R,    NONE ),       V_NO,           RG_WORD,      R_MOVOP1RES,    FU_NO ),
_OE( _Un(    R|M|C,M,    NONE ),       V_NO,           RG_WORD,      R_MOVOP1RES,    FU_NO ),
_OE( _Un(    ANY,  R,    NONE ),       V_NO,           RG_WORD,      G_UNKNOWN,      FU_NO ),
_OE( _Un(    R,    ANY,  NONE ),       V_NO,           RG_WORD,      G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  EQ_R1 ),      V_NO,           RG_WORD,      G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_WORD_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Not4[] = {
/************************/
/*           op1   op2   eq            verify          reg           gen             fu  */
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_DOUBLE,    R_SPLITUNARY,   FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Not8[] = {
/************************/
/*           op1   op2   eq            verify          reg           gen             fu  */
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_8,         R_SPLIT8,       FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Neg1[] = {
/************************/

/* instructions we can generate*/

/*           op1   op2   eq            verify          reg           gen             fu  */
_OE( _Un(    R,    R,    EQ_R1 ),      V_NO,           RG_BYTE,      G_R1,           FU_ALU1 ),
_OE( _Un(    M,    M,    EQ_R1 ),      V_NO,           RG_,          G_M1,           FU_ALU1 ),

/*       Reduction routines*/

/*           op1   op2   eq            verify          reg           gen             fu  */
_OE( _Un(    R|M|C,R,    NONE ),       V_NO,           RG_BYTE,      R_MOVOP1RES,    FU_NO ),
_OE( _Un(    R|M|C,M,    NONE ),       V_NO,           RG_BYTE,      R_MOVOP1RES,    FU_NO ),
_OE( _Un(    ANY,  R,    NONE ),       V_NO,           RG_BYTE,      G_UNKNOWN,      FU_NO ),
_OE( _Un(    R,    ANY,  NONE ),       V_NO,           RG_BYTE,      G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  EQ_R1 ),      V_NO,           RG_BYTE,      G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_BYTE_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Neg2[] = {
/************************/

/* instructions we can generate*/

/*           op1   op2   eq            verify          reg           gen             fu  */
_OE( _Un(    R,    R,    EQ_R1 ),      V_NO,           RG_WORD,      G_R1,           FU_ALU1 ),
_OE( _Un(    M,    M,    EQ_R1 ),      V_NO,           RG_,          G_M1,           FU_ALU1 ),

/*       Reduction routines*/

/*           op1   op2   eq            verify          reg           gen             fu  */
_OE( _Un(    R|M|C,R,    NONE ),       V_NO,           RG_WORD,      R_MOVOP1RES,    FU_NO ),
_OE( _Un(    R|M|C,M,    NONE ),       V_NO,           RG_WORD,      R_MOVOP1RES,    FU_NO ),
_OE( _Un(    ANY,  R,    NONE ),       V_NO,           RG_WORD,      G_UNKNOWN,      FU_NO ),
_OE( _Un(    R,    ANY,  NONE ),       V_NO,           RG_WORD,      G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  EQ_R1 ),      V_NO,           RG_WORD,      G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_WORD_NEED, G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Neg4[] = {
/************************/
/*           op1   op2   eq            verify          reg           gen             fu  */
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          R_SPLITNEG,     FU_NO ),
};


static  opcode_entry    Neg8[] = {
/************************/
/*           op1   op2   eq            verify          reg           gen             fu  */
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_8,         R_SPLIT8NEG,    FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    NegF[] = {
/************************/
/*           op1   op2   eq            verify          reg           gen             fu  */
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          R_MAKEFNEG,     FU_NO ),
};


static  opcode_entry    RTCall[] = {
/************************/
/*           op1   op2   eq            verify          reg           gen             fu  */
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          R_MAKECALL,     FU_NO ),
};


static  opcode_entry    Push1[] = {
/*************************/

/* instructions we can generate*/

/*           op1   op2   eq            verify          reg           gen             fu  */
_OE( _Un(    C,    ANY,  NONE ),       V_NO,           RG_,          R_EXT_PUSHC,    FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          R_EXT_PUSH1,    FU_NO ),
};


static  opcode_entry    Push2[] = {
/*************************/

/* instructions we can generate*/

/*           op1   op2   eq            verify          reg           gen             fu  */
_OE( _UnPP(  R,    ANY,  NONE ),       V_NO,           RG_WORD,      G_WORDR1,       FU_ALU1 ),
_OE( _UnPP(  R,    ANY,  NONE ),       V_NO,           RG_SEG_WORD,  G_SEGR1,        FU_ALU1 ),
_OE( _UnPP(  M,    ANY,  NONE ),       V_NO,           RG_,          G_M1,           FU_ALU1 ),
_OE( _UnPP(  C,    ANY,  NONE ),       V_80186,        RG_,          G_C1,           FU_ALU1 ),

/* simplifying reductions*/

/*           op1   op2   eq            verify          reg           gen             fu  */
_OE( _UnPP(  C,    ANY,  NONE ),       V_NO,           RG_WORD,      R_MOVOP1TEMP,   FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_ANYWORD,   G_UNKNOWN,      FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Pop2[] = {
/********************************/

/* instructions we can generate*/

/*           op1   op2   eq            verify          reg           gen             fu  */
_OE( _UnPP(  ANY,  R,    NONE ),       V_NO,           RG_WORD,      G_WORDR1,       FU_ALU1 ),
_OE( _UnPP(  ANY,  R,    NONE ),       V_NO,           RG_WORD_SEG,  G_SEGR1,        FU_ALU1 ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Push4[] = {
/*************************/
/*           op1   op2   eq            verify          reg           gen             fu  */
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_DOUBLE,    R_SPLITUNARY,   FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Push8[] = {
/*************************/
/*           op1   op2   eq            verify          reg           gen             fu  */
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_DOUBLE,    R_SPLIT8,       FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};

static  opcode_entry    PushXX[] = {
/**************************/
/*           op1   op2   eq            verify          reg           gen             fu  */
_OE( _Un(    U,    ANY,  NONE ),       V_NO,           RG_,          R_FORCEOP1MEM,  FU_NO ),
_OE( _Un(    M,    ANY,  NONE ),       V_NO,           RG_,          R_DOLONGPUSH,   FU_NO ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


/*   Calls always have their return value put in a register name*/

static  opcode_entry    Call[] = {
/************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_CALL,         FU_CALL ),
};


static  opcode_entry    CallI[] = {
/*************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_ICALL,        FU_CALL ),
};


static  opcode_entry    SJump[] = {
/*************************/
/*           op1   op2   eq            verify          reg           gen             fu  */
_OE( _Un(    R|U,  ANY,  NONE ),       V_NO,           RG_WORD,      G_RJMP,         FU_CALL ),
_OE( _Un(    M,    ANY,  NONE ),       V_NO,           RG_,          G_MJMP,         FU_CALL ),
_OE( _Un(    C,    ANY,  NONE ),       V_NO,           RG_,          G_CJMP,         FU_CALL ),
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Rtn8[] = {
/************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_8,         R_MAKECALL,     FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};



static  opcode_entry    Rtn8C[] = {
/*************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_8,         R_MAKECALL,     FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};



static  opcode_entry    Parm[] = {
/************************/
/*           op1   op2   eq            verify          reg           gen             fu  */
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          G_NO,           FU_NO ),
};


/*  If a value is returned, op1 will be the proper register name*/

static  opcode_entry    CmpPT[] = {
/*************************/
/*           op1   op2                 verify          reg           gen             fu  */
_OE( _Side(  ANY,  C ),                V_OP2ZERO,      RG_DBL_OR_PTR,R_MAKEU4,       FU_NO ),
_OE( _Side(  C,    R|M|U ),            V_NO,           RG_DBL_OR_PTR,R_SWAPCMP,      FU_NO ),
_OE( _Side(  ANY,  ANY ),              V_NO,           RG_DBL_OR_PTR,R_MAKECALL,     FU_NO ),
_OE( _Side(  ANY,  ANY ),              V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    BitCP[] = {
/*************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_DBL_OR_PTR,R_MAKEU2,       FU_NO ),
_OE( _Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    CmpCP[] = {
/*************************/
/*           op1   op2                 verify          reg           gen             fu  */
_OE( _Side(  ANY,  C ),                V_OP2ZERO,      RG_DBL_OR_PTR,R_CMPCP,        FU_NO ),
_OE( _Side(  C,    R|M|U ),            V_NO,           RG_DBL_OR_PTR,R_SWAPCMP,      FU_NO ),
_OE( _Side(  ANY,  ANY ),              V_CMPEQ,        RG_DBL_OR_PTR,R_MAKEU4,       FU_NO ),
_OE( _Side(  ANY,  ANY ),              V_NO,           RG_DBL_OR_PTR,R_MAKEU2,       FU_NO ),
_OE( _Side(  ANY,  ANY ),              V_NO,           RG_,          G_UNKNOWN,      FU_NO ),
};


static  opcode_entry    Cvt[] = {
/***********************/
/*           op1   op2   eq            verify          reg           gen             fu  */
_OE( _Un(    ANY,  ANY,  NONE ),       V_NO,           RG_,          R_DOCVT,        FU_NO ),
};

opcode_entry    DoNop[] = {
/*************************/
/*           op1   op2   res   eq      verify          reg           gen             fu  */
_OE( _BinPP( ANY,  ANY,  ANY,  NONE ), V_NO,           RG_,          G_NO,           FU_NO ),
};

static  opcode_entry    *OpcodeList[] = {
    #define pick(enum,opcode,fpopcode)  opcode,
    #include "_tables.h"
    #undef pick
};

static  opcode_entry    *FPOpcodeList[] = {
    #define pick(enum,opcode,fpopcode)  fpopcode,
    #include "_tables.h"
    #undef pick
};


extern  opcode_entry    *OpcodeTable( table_def i )
/**************************************************
    return the address of the appropriate generate table given an index "i"
*/
{
    if( _FPULevel( FPU_87 ) ) {
        return( FPOpcodeList[ i ] );
    } else {
        return( OpcodeList[ i ] );
    }
}
