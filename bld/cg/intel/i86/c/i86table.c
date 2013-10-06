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
/*       op1   op2   res   eq      verify          gen           reg fu*/

/* optimizing reductions*/

{_Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2ZERO),   R_MAKEMOVE, RG_BYTE,FU_NO},
{_Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2NEG),    R_MAKESUB,  RG_BYTE,FU_NO},
{_Bin(   R,    U,    R,    NONE ), V_CONSTTEMP,    R_TEMP2CONST, RG_BYTE,FU_NO},

/* instructions that we can generate*/

{_BinSC( R,    C,    R,    EQ_R1 ),V_OP2ONE,       G_R1,         RG_BYTE,FU_ALU1},
{_BinSC( R,    C,    R,    EQ_R1 ),V_NO,           G_AC,         RG_BYTE_ACC,FU_ALU1},
{_BinSC( R,    C,    R,    EQ_R1 ),V_NO,           G_RC,         RG_BYTE,FU_ALU1},
{_BinSC( M,    C,    M,    EQ_R1 ),V_OP2ONE,       G_M1,         RG_,FU_ALU1},
{_BinSC( M,    C,    M,    EQ_R1 ),V_NO,           G_MC,         RG_,FU_ALU1},
{_BinSC( R,    R,    R,    EQ_R1 ),V_NO,           G_RR2,        RG_BYTE,FU_ALU1},
{_BinSC( R,    M,    R,    EQ_R1 ),V_NO,           G_RM2,        RG_BYTE,FU_ALU1},
{_BinSC( M,    R,    M,    EQ_R1 ),V_NO,           G_MR2,        RG_BYTE,FU_ALU1},

/* simplifying reductions*/

{_Bin(   C,    R|M|U,ANY,  NONE ), V_NO,           R_SWAPOPS,    RG_BYTE,FU_NO},
{_Bin(   R|M,  R|M|C,R|M,  EQ_R1 ),V_NO,           R_LOADOP2,    RG_BYTE,FU_NO},
{_Bin(   R|M|C,R|M,  R|M,  EQ_R2 ),V_NO,           R_SWAPOPS,    RG_BYTE,FU_NO},
{_Bin(   R,    ANY,  ANY,  NONE ), V_SWAP_GOOD,    R_SWAPOPS,    RG_BYTE,FU_NO},
{_Bin(   R|M|C,R|M|C,R|M,  NONE ), V_NO,           R_USEREGISTER,RG_BYTE,FU_NO},
{_Bin(   ANY,  R|C,  ANY,  EQ_R1), V_NO,           G_UNKNOWN,    RG_BYTE,FU_NO},
{_Bin(   R|C,  ANY,  ANY,  EQ_R2), V_NO,           G_UNKNOWN,    RG_BYTE,FU_NO},
{_Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           G_UNKNOWN,    RG_BYTE_NEED,FU_NO},
};


static  opcode_entry    Add2[] = {
/************************/
/*       op1   op2   res   eq      verify          gen           reg fu*/

/* optimizing reductions*/

{_Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2ZERO),   R_MAKEMOVE, RG_WORD,FU_NO},
{_Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2NEG),    R_MAKESUB,  RG_WORD,FU_NO},
{_Bin(   R|M|U,C,    R|M|U,EQ_R1), NVI(V_OP2LOW_B_ZERO),R_CYPHIGH,RG_TWOBYTE,FU_NO},
{_Bin(   R,    U,    R,    NONE ), V_CONSTTEMP,    R_TEMP2CONST, RG_WORD,FU_NO},

/* instructions we can generate*/

{_BinSC( R,    C,    R,    EQ_R1 ),V_OP1SP,        G_RC,         RG_WORD,FU_ALU1},
{_BinSC( R,    C,    R,    EQ_R1 ),V_OP2ONE,       G_WORDR1,     RG_WORD,FU_ALU1},
{_BinSC( R,    C,    R,    EQ_R1 ),V_OP2TWO_SIZE,  G_WORDR1,     RG_WORD,FU_ALU1},
{_BinSC( R,    C,    R,    EQ_R1 ),V_NO,           G_AC,         RG_WORD_ACC,FU_ALU1},
{_BinSC( R,    C,    R,    EQ_R1 ),V_NO,           G_RC,         RG_WORD,FU_ALU1},
{_BinPP( R,    C,    R,    NONE ), V_LEA,          G_LEA,        RG_LEA,FU_ALU1},
{_BinSC( M,    C,    M,    EQ_R1 ),V_OP2ONE,       G_M1,         RG_,FU_ALU1},

/* Fall into AddExt table*/
/**** NB. AddExt points here ****/
/* opcode_entry    AddExt[] = { */
/**************************/
/*       op1   op2   res   eq      verify          gen*/

/* instructions that we can generate*/

{_BinSC( R,    C,    R,    EQ_R1 ),V_NO,           G_AC,         RG_WORD_ACC,FU_ALU1},
{_BinSC( R,    C,    R,    EQ_R1 ),V_NO,           G_RC,         RG_WORD,FU_ALU1},
{_BinSC( M,    C,    M,    EQ_R1 ),V_NO,           G_MC,         RG_,FU_ALU1},
{_BinSC( R,    R,    R,    EQ_R1 ),V_NO,           G_RR2,        RG_WORD,FU_ALU1},
{_BinSC( R,    M,    R,    EQ_R1 ),V_NO,           G_RM2,        RG_WORD,FU_ALU1},
{_BinSC( M,    R,    M,    EQ_R1 ),V_NO,           G_MR2,        RG_WORD,FU_ALU1},

/* simplifying reductions*/

{_Bin(   R|M,  R|M|C,R|M,  EQ_R1 ),V_NO,           R_LOADOP2,    RG_WORD,FU_NO},
{_Bin(   R|M|C,R|M,  R|M,  EQ_R2 ),V_NO,           R_SWAPOPS,    RG_WORD,FU_NO},
{_Bin(   R,    ANY,  ANY,  NONE ), V_SWAP_GOOD,    R_SWAPOPS,    RG_WORD,FU_NO},
{_Bin(   R|M|C,R|M|C,R|M,  NONE ), V_NO,           R_USEREGISTER,RG_WORD,FU_NO},
{_Bin(   ANY,  R|C,  ANY,  EQ_R1), V_NO,           G_UNKNOWN,    RG_WORD,FU_NO},
{_Bin(   R|C,  ANY,  ANY,  EQ_R2), V_NO,           G_UNKNOWN,    RG_WORD,FU_NO},
{_Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           G_UNKNOWN,    RG_WORD_NEED,FU_NO},
};

/* Point at where AddExt used to start */
/*************************/
#define AddExt &Add2[11]

static  opcode_entry    Add4[] = {
/************************/
/*       op1   op2   res  eq      verify          gen           reg fu*/
{_Bin(   ANY,  ANY,  ANY, NONE ), V_NO,           R_SPLITOP,    RG_DOUBLE,FU_NO},
};


static  opcode_entry    Add8[] = {
/************************/
/*       op1   op2   res  eq      verify          gen           reg fu*/
{_Bin(   ANY,  ANY,  ANY, NONE ), V_NO,           R_SPLIT8BIN,   RG_8,FU_NO},
};


static  opcode_entry    AddCP[] = {
/*************************/
/*       op1   op2   res   eq      verify          gen           reg fu*/
/*Bin(   C,    R|M|U,ANY,  NONE  ),V_NO,           R_SWAPOPS,    RG_DBL_OR_PTR,FU_NO,*/
{_Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2ZERO), R_MAKEMOVE,   RG_DBL_OR_PTR,FU_NO},
{_Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2NEG),  R_MAKESUB,    RG_DBL_OR_PTR,FU_NO},
{_Bin(   ANY,  ANY,  ANY,  NONE  ),V_NO,           R_MAKEU2,     RG_DBL_OR_PTR,FU_NO},
};


static  opcode_entry    AddPT[] = {
/*************************/
/*       op1   op2   res   eq      verify          gen           reg fu*/
/*Bin(   C,    R|M|U,ANY,  NONE  ),V_NO,           R_SWAPOPS,    RG_,FU_NO,*/
{_Bin(   ANY,  ANY,  ANY,  NONE  ),V_NO,           R_MAKECALL,   RG_,FU_NO},
};


static  opcode_entry    Sub1[] = {
/************************/
/*       op1   op2   res   eq      verify          gen           reg fu*/

/* optimizing reductions*/

{_Bin(   C,    ANY,  ANY,  NONE ), NVI(V_OP1ZERO),  R_MAKENEG,   RG_BYTE,FU_NO},
{_Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2ZERO),  R_MAKEMOVE,  RG_BYTE,FU_NO},
{_Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2NEG),   R_MAKEADD,   RG_BYTE,FU_NO},
{_Bin(   R,    U,    R,    NONE ), V_CONSTTEMP,    R_TEMP2CONST, RG_BYTE,FU_NO},

/* instructions that we can generate*/

{_BinSC( R,    C,    R,    EQ_R1 ),V_OP2ONE,       G_R1,         RG_BYTE,FU_ALU1},
{_BinCC( R,    C,    R,    EQ_R1 ),V_NO,           G_AC,         RG_BYTE_ACC,FU_ALU1},
{_BinCC( R,    C,    R,    EQ_R1 ),V_NO,           G_RC,         RG_BYTE,FU_ALU1},
{_BinSC( M,    C,    M,    EQ_R1 ),V_OP2ONE,       G_M1,         RG_,FU_ALU1},
{_BinCC( M,    C,    M,    EQ_R1 ),V_NO,           G_MC,         RG_,FU_ALU1},
{_BinCC( R,    R,    R,    EQ_R1 ),V_NO,           G_RR2,        RG_BYTE,FU_ALU1},
{_BinCC( R,    M,    R,    EQ_R1 ),V_NO,           G_RM2,        RG_BYTE,FU_ALU1},
{_BinCC( M,    R,    M,    EQ_R1 ),V_NO,           G_MR2,        RG_BYTE,FU_ALU1},

/* simplifying reductions*/

{_Bin(   R|M,  R|M|C,R|M,  EQ_R1 ),V_NO,           R_LOADOP2,    RG_BYTE,FU_NO},
{_Bin(   R|M|C,R|M|C,R|M,  NONE ), V_NO,           R_USEREGISTER,RG_BYTE,FU_NO},
{_Bin(   ANY,  R|C,  ANY,  EQ_R1), V_NO,           G_UNKNOWN,    RG_BYTE,FU_NO},
{_Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           G_UNKNOWN,    RG_BYTE_NEED,FU_NO},
};


static  opcode_entry    Sub2[] = {
/************************/
/*       op1   op2   res   eq      verify          gen           reg fu*/

/* optimizing reductions*/

{_Bin(   C,    ANY,  ANY,  NONE ), NVI(V_OP1ZERO),  R_MAKENEG,   RG_WORD,FU_NO},
{_Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2ZERO),  R_MAKEMOVE,  RG_WORD,FU_NO},
{_Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2NEG),   R_MAKEADD,   RG_WORD,FU_NO},
{_Bin(   R,    U,    R,    NONE ), V_CONSTTEMP,    R_TEMP2CONST, RG_WORD,FU_NO},

/* instructions we can generate*/

{_BinSC( R,    C,    R,    EQ_R1 ),V_OP1SP,        G_RC,         RG_WORD,FU_ALU1},
{_BinSC( R,    C,    R,    EQ_R1 ),V_OP2ONE,       G_WORDR1,     RG_WORD,FU_ALU1},
{_BinSC( R,    C,    R,    EQ_R1 ),V_OP2TWO_SIZE,  G_WORDR1,     RG_WORD,FU_ALU1},
{_BinCC( R,    C,    R,    EQ_R1 ),V_NO,           G_AC,         RG_WORD_ACC,FU_ALU1},
{_BinCC( R,    C,    R,    EQ_R1 ),V_NO,           G_RC,         RG_WORD,FU_ALU1},
{_BinPP( R,    C,    R,    NONE ), V_LEA,          G_LEA,        RG_LEA,FU_ALU1},
{_BinSC( M,    C,    M,    EQ_R1 ),V_OP2ONE,       G_M1,         RG_,FU_ALU1},

/* cheap pointer subtractions come through here since the result type is I2*/

{_Bin(   ANY,  ANY,  ANY,  NONE  ),V_OP2PTR,       R_CPSUB,      RG_,FU_NO},

/* from here on down is identical to SubExt table except SubExt uses _BinSC*/

/* instructions that we can generate*/

{_BinCC( R,    C,    R,    EQ_R1 ),V_NO,           G_AC,         RG_WORD_ACC,FU_ALU1},
{_BinCC( R,    C,    R,    EQ_R1 ),V_NO,           G_RC,         RG_WORD,FU_ALU1},
{_BinCC( M,    C,    M,    EQ_R1 ),V_NO,           G_MC,         RG_,FU_ALU1},
{_BinCC( R,    R,    R,    EQ_R1 ),V_NO,           G_RR2,        RG_WORD,FU_ALU1},
{_BinCC( R,    M,    R,    EQ_R1 ),V_NO,           G_RM2,        RG_WORD,FU_ALU1},
{_BinCC( M,    R,    M,    EQ_R1 ),V_NO,           G_MR2,        RG_WORD,FU_ALU1},

/* simplifying reductions*/

{_Bin(   R|M,  R|M|C,R|M,  EQ_R1 ),V_NO,           R_LOADOP2,    RG_WORD,FU_NO},
{_Bin(   R|M|C,R|M|C,R|M,  NONE ), V_NO,           R_USEREGISTER,RG_WORD,FU_NO},
{_Bin(   ANY,  R|C,  ANY,  EQ_R1), V_NO,           G_UNKNOWN,    RG_WORD,FU_NO},
{_Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           G_UNKNOWN,    RG_WORD_NEED,FU_NO},
};


opcode_entry    SubExt[] = {
/**************************/
/*       op1   op2   res   eq      verify          gen*/

/* instructions that we can generate*/
/* NOTE: SBB does NOT set conditions right in all cases*/
/*       consider SBB AX,DX when AX==0,DX==FFFF,CF==1 -> leaves carry set and a JB*/
/*       will take the jump!!!*/

{_BinSC( R,    C,    R,    EQ_R1 ),V_NO,           G_AC,         RG_WORD_ACC,FU_ALU1},
{_BinSC( R,    C,    R,    EQ_R1 ),V_NO,           G_RC,         RG_WORD,FU_ALU1},
{_BinSC( M,    C,    M,    EQ_R1 ),V_NO,           G_MC,         RG_,FU_ALU1},
{_BinSC( R,    R,    R,    EQ_R1 ),V_NO,           G_RR2,        RG_WORD,FU_ALU1},
{_BinSC( R,    M,    R,    EQ_R1 ),V_NO,           G_RM2,        RG_WORD,FU_ALU1},
{_BinSC( M,    R,    M,    EQ_R1 ),V_NO,           G_MR2,        RG_WORD,FU_ALU1},

/* simplifying reductions*/

{_Bin(   R|M,  R|M|C,R|M,  EQ_R1 ),V_NO,           R_LOADOP2,    RG_WORD,FU_NO},
{_Bin(   R|M|C,R|M|C,R|M,  NONE ), V_NO,           R_USEREGISTER,RG_WORD,FU_NO},
{_Bin(   ANY,  R|C,  ANY,  EQ_R1), V_NO,           G_UNKNOWN,    RG_WORD,FU_NO},
{_Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           G_UNKNOWN,    RG_WORD_NEED,FU_NO},
};


static  opcode_entry    Sub4[] = {
/************************/
/*       op1   op2   res  eq      verify          gen           reg fu*/

/* huge pointer subtractions come through here since the result type is I4*/

{_Bin(   ANY,  ANY,  ANY, NONE ), V_OP2PTR,       R_PTSUB,      RG_,FU_NO},

{_Bin(   C,    ANY,  ANY, NONE ), NVI(V_OP1ZERO), R_MAKENEG,    RG_DOUBLE,FU_NO},
{_Bin(   ANY,  ANY,  ANY, NONE ), V_NO,           R_SPLITOP,    RG_DOUBLE,FU_NO},
};


static  opcode_entry    Sub8[] = {
/************************/
/*       op1   op2   res  eq      verify          gen           reg fu*/
{_Bin(   ANY,  ANY,  ANY, NONE ), V_NO,           R_SPLIT8BIN,   RG_8,FU_NO},
};


static  opcode_entry    SubCP[] = {
/*************************/
/*       op1   op2   res   eq      verify          gen           reg fu*/
{_Bin(   ANY,  ANY,  ANY,  NONE  ),V_NO,           R_MAKEU2,     RG_,FU_NO},
};


static  opcode_entry    SubPT[] = {
/*************************/
/*       op1   op2   res   eq      verify          gen           reg fu*/
{_Bin(   ANY,  C,    ANY,  NONE  ),V_NO,           R_MAKEADD,    RG_,FU_NO},
{_Bin(   ANY,  ANY,  ANY,  NONE  ),V_NO,           R_MAKECALL, RG_,FU_NO},
{_Bin(   ANY,  ANY,  ANY,  NONE  ),V_NO,           R_NEGADD,     RG_,FU_NO},
};


/* Binary Logical 8 bytes operations: AND, OR, XOR */
static  opcode_entry    Log8[] = {
/************************/
/*       op1   op2   res  eq      verify          gen           reg fu*/
{_Bin(   ANY,  ANY,  ANY, NONE ), V_NO,           R_SPLIT8BIN,   RG_8,FU_NO},
};


static  opcode_entry    Or1[] = {
/***********************/
/*       op1   op2   res   eq      verify          gen           reg fu*/

/* WATCH OUT: Both OR and XOR come through this table!*/
/* optimizing reductions*/

{_Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2ZERO), R_MAKEMOVE,   RG_BYTE,FU_NO},

/* instructions that we can generate*/

{_BinSC( R,    C,    R,    EQ_R1 ),V_NO,           G_AC,         RG_BYTE_ACC,FU_ALU1},
{_BinSC( R,    C,    R,    EQ_R1 ),V_NO,           G_RC,         RG_BYTE,FU_ALU1},
{_BinSC( M,    C,    M,    EQ_R1 ),V_NO,           G_MC,         RG_,FU_ALU1},
{_BinSC( R,    R,    R,    EQ_R1 ),V_NO,           G_RR2,        RG_BYTE,FU_ALU1},
{_BinSC( R,    M,    R,    EQ_R1 ),V_NO,           G_RM2,        RG_BYTE,FU_ALU1},
{_BinSC( M,    R,    M,    EQ_R1 ),V_NO,           G_MR2,        RG_BYTE,FU_ALU1},

/* simplifying reductions*/

{_Bin(   C,    R|M|U,ANY,  NONE ), V_NO,           R_SWAPOPS,    RG_BYTE,FU_NO},
{_Bin(   R|M,  R|M|C,R|M,  EQ_R1 ),V_NO,           R_LOADOP2,    RG_BYTE,FU_NO},
{_Bin(   R|M|C,R|M,  R|M,  EQ_R2 ),V_NO,           R_SWAPOPS,    RG_BYTE,FU_NO},
{_Bin(   R,    ANY,  ANY,  NONE ), V_SWAP_GOOD,    R_SWAPOPS,    RG_BYTE,FU_NO},
{_Bin(   R|M|C,R|M|C,R|M,  NONE ), V_NO,           R_USEREGISTER,RG_BYTE,FU_NO},
{_Bin(   R|C,  ANY,  ANY,  EQ_R2), V_NO,           G_UNKNOWN,    RG_BYTE,FU_NO},
{_Bin(   ANY,  R|C,  ANY,  EQ_R1), V_NO,           G_UNKNOWN,    RG_BYTE,FU_NO},
{_Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           G_UNKNOWN,    RG_BYTE_NEED,FU_NO},
};


static  opcode_entry    Or2[] = {
/***********************/
/*       op1   op2   res   eq      verify          gen           reg fu*/

/* WATCH OUT: Both OR and XOR come through this table!*/
/* optimizing reductions*/

{_Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2ZERO), R_MAKEMOVE,   RG_WORD,FU_NO},
{_Bin(   R|M|U,C,    R|M|U,EQ_R1), NVI(V_OP2LOW_B_ZERO),R_CYPHIGH,RG_TWOBYTE,FU_NO},
{_Bin(   R|M|U,C,    R|M|U,EQ_R1), NVI(V_OP2HIGH_B_ZERO),R_CYPLOW,RG_TWOBYTE,FU_NO},

/* instructions that we can generate*/

{_BinSC( R,    C,    R,    EQ_R1 ),V_NO,           G_AC,         RG_WORD_ACC,FU_ALU1},
{_BinSC( R,    C,    R,    EQ_R1 ),V_NO,           G_RC,         RG_WORD,FU_ALU1},
{_BinSC( M,    C,    M,    EQ_R1 ),V_NO,           G_MC,         RG_,FU_ALU1},
{_BinSC( R,    R,    R,    EQ_R1 ),V_NO,           G_RR2,        RG_WORD,FU_ALU1},
{_BinSC( R,    M,    R,    EQ_R1 ),V_NO,           G_RM2,        RG_WORD,FU_ALU1},
{_BinSC( M,    R,    M,    EQ_R1 ),V_NO,           G_MR2,        RG_WORD,FU_ALU1},

/* simplifying reductions*/

{_Bin(   C,    R|M|U,ANY,  NONE ), V_NO,           R_SWAPOPS,    RG_WORD,FU_NO},
{_Bin(   R|M,  R|M|C,R|M,  EQ_R1 ),V_NO,           R_LOADOP2,    RG_WORD,FU_NO},
{_Bin(   R|M|C,R|M,  R|M,  EQ_R2 ),V_NO,           R_SWAPOPS,    RG_WORD,FU_NO},
{_Bin(   R,    ANY,  ANY,  NONE ), V_SWAP_GOOD,    R_SWAPOPS,    RG_WORD,FU_NO},
{_Bin(   R|M|C,R|M|C,R|M,  NONE ), V_NO,           R_USEREGISTER,RG_WORD,FU_NO},
{_Bin(   ANY,  R|C,  ANY,  EQ_R1), V_NO,           G_UNKNOWN,    RG_WORD,FU_NO},
{_Bin(   R|C,  ANY,  ANY,  EQ_R2), V_NO,           G_UNKNOWN,    RG_WORD,FU_NO},
{_Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           G_UNKNOWN,    RG_WORD_NEED,FU_NO},
};


static  opcode_entry    Or4[] = {
/***********************/
/*       op1   op2   res   eq      verify          gen           reg fu*/
/* WATCH OUT: Both OR and XOR come through this table!*/
{_Bin(   ANY,  ANY,  ANY, NONE ), V_NO,           R_SPLITOP,    RG_DOUBLE,FU_NO},
};


static  opcode_entry    And1[] = {
/************************/
/*       op1   op2   res   eq      verify          gen           reg fu*/

/* optimizing reductions*/

{_Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2ZERO),R_MOVOP2,      RG_BYTE,FU_NO},
{_Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2LOW_B_FF),R_MAKEMOVE,RG_BYTE,FU_NO},

/* instructions that we can generate*/

{_BinSC( R,    C,    R,    EQ_R1 ),V_NO,           G_AC,         RG_BYTE_ACC,FU_ALU1},
{_BinSC( R,    C,    R,    EQ_R1 ),V_NO,           G_RC,         RG_BYTE,FU_ALU1},
{_BinSC( M,    C,    M,    EQ_R1 ),V_NO,           G_MC,         RG_,FU_ALU1},
{_BinSC( R,    R,    R,    EQ_R1 ),V_NO,           G_RR2,        RG_BYTE,FU_ALU1},
{_BinSC( R,    M,    R,    EQ_R1 ),V_NO,           G_RM2,        RG_BYTE,FU_ALU1},
{_BinSC( M,    R,    M,    EQ_R1 ),V_NO,           G_MR2,        RG_BYTE,FU_ALU1},

/* simplifying reductions*/

{_Bin(   C,    R|M|U,ANY,  NONE ), V_NO,           R_SWAPOPS,    RG_BYTE,FU_NO},
{_Bin(   R|M,  R|M|C,R|M,  EQ_R1 ),V_NO,           R_LOADOP2,    RG_BYTE,FU_NO},
{_Bin(   R|M|C,R|M,  R|M,  EQ_R2 ),V_NO,           R_SWAPOPS,    RG_BYTE,FU_NO},
{_Bin(   R,    ANY,  ANY,  NONE ), V_SWAP_GOOD,    R_SWAPOPS,    RG_BYTE,FU_NO},
{_Bin(   R|M|C,R|M|C,R|M,  NONE ), V_NO,           R_USEREGISTER,RG_BYTE,FU_NO},
{_Bin(   ANY,  R|C,  ANY,  EQ_R1), V_NO,           G_UNKNOWN,    RG_BYTE,FU_NO},
{_Bin(   R|C,  ANY,  ANY,  EQ_R2), V_NO,           G_UNKNOWN,    RG_BYTE,FU_NO},
{_Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           G_UNKNOWN,    RG_BYTE_NEED,FU_NO},
};


static  opcode_entry    And2[] = {
/************************/
/*       op1   op2   res   eq      verify          gen           reg fu*/

/* optimizing reductions*/

{_Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2ZERO), R_MOVOP2,    RG_WORD,FU_NO},
{_Bin(   ANY,  C,    ANY,  NONE ), NVI(V_OP2LOW_W_FFFF),R_MAKEMOVE,RG_WORD,FU_NO},
{_Bin(   R|M|U,C,    R|M|U,EQ_R1), NVI(V_OP2LOW_B_FF),R_CYPHIGH,RG_TWOBYTE,FU_NO},
{_Bin(   R|M|U,C,    R|M|U,EQ_R1), NVI(V_OP2HIGH_B_FF),R_CYPLOW,RG_TWOBYTE,FU_NO},

/* instructions that we can generate*/

{_BinSC( R,    C,    R,    EQ_R1 ),V_NO,           G_AC,         RG_WORD_ACC,FU_ALU1},
{_BinSC( R,    C,    R,    EQ_R1 ),V_NO,           G_RC,         RG_WORD,FU_ALU1},
{_BinSC( M,    C,    M,    EQ_R1 ),V_NO,           G_MC,         RG_,FU_ALU1},
{_BinSC( R,    R,    R,    EQ_R1 ),V_NO,           G_RR2,        RG_WORD,FU_ALU1},
{_BinSC( R,    M,    R,    EQ_R1 ),V_NO,           G_RM2,        RG_WORD,FU_ALU1},
{_BinSC( M,    R,    M,    EQ_R1 ),V_NO,           G_MR2,        RG_WORD,FU_ALU1},

/* simplifying reductions*/

{_Bin(   C,    R|M|U,ANY,  NONE ), V_NO,           R_SWAPOPS,    RG_WORD,FU_NO},
{_Bin(   R|M,  R|M|C,R|M,  EQ_R1 ),V_NO,           R_LOADOP2,    RG_WORD,FU_NO},
{_Bin(   R|M|C,R|M,  R|M,  EQ_R2 ),V_NO,           R_SWAPOPS,    RG_WORD,FU_NO},
{_Bin(   R,    ANY,  ANY,  NONE ), V_SWAP_GOOD,    R_SWAPOPS,    RG_WORD,FU_NO},
{_Bin(   R|M|C,R|M|C,R|M,  NONE ), V_NO,           R_USEREGISTER,RG_WORD,FU_NO},
{_Bin(   ANY,  R|C,  ANY,  EQ_R1), V_NO,           G_UNKNOWN,    RG_WORD,FU_NO},
{_Bin(   R|C,  ANY,  ANY,  EQ_R2), V_NO,           G_UNKNOWN,    RG_WORD,FU_NO},
{_Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           G_UNKNOWN,    RG_WORD_NEED,FU_NO},
};


static  opcode_entry    And4[] = {
/************************/
/*       op1   op2   res   eq      verify          gen           reg fu*/
{_Bin(   C,    R|M|U,ANY, NONE ), V_NO,           R_SWAPOPS,    RG_DOUBLE,FU_NO},
{_Bin(   ANY,  C,    ANY, EQ_R1), NVI(V_OP2HIGH_W_FFFF),R_CYPLOW,RG_DOUBLE,FU_NO},
{_Bin(   ANY,  C,    ANY, EQ_R1), NVI(V_OP2LOW_W_FFFF),R_CYPHIGH,RG_DOUBLE,FU_NO},
{_Bin(   ANY,  ANY,  ANY, NONE ), V_NO,           R_SPLITOP,    RG_DOUBLE,FU_NO},
};


static  opcode_entry    Rtn4C[] = {
/*************************/
/*       op1   op2   res   eq      verify          gen           reg fu*/
{_Bin(   R,    R,    R,    BOTH_EQ ),V_NO,         R_MAKECALL,   RG_4CRTN,FU_NO},
{_Bin(   ANY,  R,    R,    EQ_R2 ),V_NO,           R_SWAPOPS,    RG_4CRTN,FU_NO},

/* Fall into Rtn4 table*/
/**** NB. Rtn4 points here ****/
/* static  opcode_entry    Rtn4[] = { */
/************************/
/*       op1   op2   res   eq      verify          gen*/
{_Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           R_MAKECALL,   RG_,FU_NO},
};

/* Point at where Rtn4 used to start */
/*************************/
#define Rtn4 &Rtn4C[2]

static  opcode_entry    Mul1[] = {
/************************/
/*       op1   op2   res   eq      verify          gen           reg fu*/

/* instructions we can generate*/

{_Bin(   R,    R,    R,    NONE ), V_NO,           G_R2,         RG_BYTE_MUL,FU_IMUL},
{_Bin(   R,    M,    R,    NONE ), V_NO,           G_M2,         RG_BYTE_MUL,FU_IMUL},

/* simplifying reductions*/

{_Bin(   R,    C,    R,    NONE ), V_NO,           R_MOVOP2TEMP,RG_BYTE_MUL,FU_NO},
{_Bin(   R,    U,    R,    NONE ), V_NO,           G_UNKNOWN,    RG_BYTE_MUL,FU_NO},
{_Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           R_MULREGISTER,RG_BYTE_MUL,FU_NO},
};


static  opcode_entry    Mul2[] = {
/************************/
/*       op1   op2   res   eq      verify          gen           reg fu*/

/* instructions we can generate*/

// _Bin(        R,    C,    R,    NONE ),V_OP2ONE,        R_MULSAVEFACE,RG_WORD_MUL,FU_NO},
{_Bin(   R,    C,    R,    NONE ),V_80186,         G_186RMUL,    RG_WORD,FU_IMUL},
{_Bin(   M,    C,    R,    NONE ),V_80186,         G_186MUL,     RG_WORD,FU_IMUL},

/* simplifying reductions*/

{_Bin(   U,    C,    R,    NONE ),V_80186,         G_UNKNOWN,    RG_WORD,FU_NO},
{_Bin(   ANY,  C,    M,    NONE ),V_80186,         R_RESREG,     RG_WORD,FU_NO},
{_Bin(   ANY,  C,    U|M,  NONE ),V_80186,         G_UNKNOWN,    RG_WORD_NEED,FU_NO},
{_Bin(   ANY,  C,    R,    NONE ),V_80186,         G_UNKNOWN,    RG_WORD_NEED,FU_NO},
{_Bin(   C,    ANY,  ANY,  NONE ),V_80186,         R_SWAPOPS,    RG_WORD,FU_NO},

/* instructions we can generate*/

{_Bin(   R,    R,    R,    EQ_R1 ),V_80386,        G_0FRR2,      RG_WORD,FU_IMUL},
{_Bin(   R,    M,    R,    EQ_R1 ),V_80386,        G_0FRM2,      RG_WORD,FU_IMUL},

/* simplifying reductions*/

{_Bin(   C,    R|M|U,ANY,  NONE ), V_80386,        R_SWAPOPS,    RG_WORD,FU_NO},
{_Bin(   R|M|C,R,    R,    EQ_R2 ),V_80386,        R_SWAPOPS,    RG_WORD,FU_NO},
{_Bin(   R,    ANY,  ANY,  NONE ), V_SWAP_GOOD_386,R_SWAPOPS,    RG_WORD,FU_NO},
{_Bin(   R|M|C,R|M|C,R|M,  NONE ), V_80386,        R_USEREGISTER,RG_WORD,FU_NO},
{_Bin(   ANY,  ANY,  ANY,  NONE ), V_80386,        G_UNKNOWN,    RG_WORD_NEED,FU_NO},

/* instructions we can generate*/

{_Bin(   R,    R,    R,    NONE ), V_NO,           G_R2,         RG_WORD_MUL,FU_IMUL},
{_Bin(   R,    M,    R,    NONE ), V_NO,           G_M2,         RG_WORD_MUL,FU_IMUL},

/* simplifying reductions*/

{_Bin(   R,    C,    R,    NONE ), V_NO,           R_MOVOP2TEMP,RG_WORD_MUL,FU_NO},
{_Bin(   R,    U,    R,    NONE ), V_NO,           G_UNKNOWN,    RG_WORD_MUL,FU_NO},
{_Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           R_MULREGISTER,RG_WORD_MUL,FU_NO},
};


static  opcode_entry    ExtMul[] = {
/**************************/
/*       op1   op2   res   eq      verify          gen           reg fu*/
{_Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           R_MAKECYPMUL, RG_,FU_NO},
};


static  opcode_entry    Div1[] = {
/************************/
/*       op1   op2   res   eq      verify          gen           reg fu*/
{_Bin(   R,    C,    R,    NONE ), V_OP2TWO,       G_DIV2,       RG_BYTE_DIV,FU_IDIV},
{_Bin(   R,    C,    R,    NONE ), V_OP2POW2_286,  G_POW2DIV_286,RG_BYTE_DIV,FU_IDIV},
{_Bin(   R,    C,    R,    NONE ), V_OP2POW2,      G_POW2DIV,    RG_BYTE_DIVPOW2,FU_IDIV},
{_Bin(   ANY,  C,    ANY,  NONE ), V_OP2POW2,      R_DIVREGISTER,RG_BYTE_DIV,FU_NO},
{_Bin(   ANY,  C,    ANY,  NONE ), V_NO,           R_MOVOP2TEMP, RG_BYTE_DIV,FU_NO},
{_Bin(   R,    R,    R,    NONE ), V_NO,           G_R2,         RG_BYTE_DIV,FU_IDIV},
{_Bin(   R,    M,    R,    NONE ), V_NO,           G_M2,         RG_BYTE_DIV,FU_IDIV},
{_Bin(   R,    U|M,  R,    NONE ), V_DIV_BUG,      R_LOADOP2,    RG_BYTE_DIV,FU_NO},
{_Bin(   R,    U,    R,    NONE ), V_NO,           G_UNKNOWN,    RG_BYTE_DIV,FU_NO},
{_Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           R_DIVREGISTER,RG_BYTE_DIV,FU_NO},
};


static  opcode_entry    Div2[] = {
/************************/
/*       op1   op2   res   eq      verify          gen           reg fu*/
// _Bin(        R,    C,    R,    NONE ), V_OP2ONE,       R_SAVEFACE,   RG_WORD_DIV,FU_NO},
{_Bin(   R,    C,    R,    NONE ), V_OP2TWO,       G_DIV2,       RG_WORD_DIV,FU_IDIV},
{_Bin(   R,    C,    R,    NONE ), V_OP2POW2_286,  G_POW2DIV_286,RG_WORD_DIV,FU_IDIV},
{_Bin(   R,    C,    R,    NONE ), V_OP2POW2,      G_POW2DIV,    RG_WORD_DIVPOW2,FU_IDIV},
{_Bin(   ANY,  C,    ANY,  NONE ), V_OP2POW2,      R_DIVREGISTER,RG_WORD_DIV,FU_NO},
{_Bin(   ANY,  C,    ANY,  NONE ), V_NO,           R_MOVOP2TEMP, RG_WORD_DIV,FU_NO},
{_Bin(   R,    R,    R,    NONE ), V_NO,           G_R2,         RG_WORD_DIV,FU_IDIV},
{_Bin(   R,    M,    R,    NONE ), V_NO,           G_M2,         RG_WORD_DIV,FU_IDIV},
{_Bin(   R,    U|M,  R,    NONE ), V_DIV_BUG,      R_LOADOP2,    RG_WORD_DIV,FU_NO},
{_Bin(   R,    U,    R,    NONE ), V_NO,           G_UNKNOWN,    RG_WORD_DIV,FU_NO},
{_Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           R_DIVREGISTER,RG_WORD_DIV,FU_NO},
};


static  opcode_entry    Mod1[] = {
/************************/
/*       op1   op2   res   eq      verify          gen           reg fu*/
{_Bin(   ANY,  C,    ANY,  NONE ), V_NO,           R_MOVOP2TEMP, RG_BYTE_MOD,FU_NO},
{_Bin(   R,    R,    R,    NONE ), V_NO,           G_R2,         RG_BYTE_MOD,FU_IDIV},
{_Bin(   R,    M,    R,    NONE ), V_NO,           G_M2,         RG_BYTE_MOD,FU_IDIV},
{_Bin(   R,    U|M,  R,    NONE ), V_DIV_BUG,      R_LOADOP2,    RG_BYTE_MOD,FU_NO},
{_Bin(   R,    U,    R,    NONE ), V_NO,           G_UNKNOWN,    RG_BYTE_MOD,FU_NO},
{_Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           R_DIVREGISTER,RG_BYTE_MOD,FU_NO},
};


static  opcode_entry    Mod2[] = {
/************************/
/*       op1   op2   res   eq      verify          gen           reg fu*/
// _Bin(        R,    C,    R,    NONE ), V_OP2ONE,       R_SAVEFACE,   RG_WORD_MOD,FU_NO},
{_Bin(   ANY,  C,    ANY,  NONE ), V_NO,           R_MOVOP2TEMP, RG_WORD_MOD,FU_NO},
{_Bin(   R,    R,    R,    NONE ), V_NO,           G_R2,         RG_WORD_MOD,FU_IDIV},
{_Bin(   R,    M,    R,    NONE ), V_NO,           G_M2,         RG_WORD_MOD,FU_IDIV},
{_Bin(   R,    U|M,  R,    NONE ), V_DIV_BUG,      R_LOADOP2,    RG_WORD_MOD,FU_NO},
{_Bin(   R,    U,    R,    NONE ), V_NO,           G_UNKNOWN,    RG_WORD_MOD,FU_NO},
{_Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,           R_DIVREGISTER,RG_WORD_MOD,FU_NO},
};


static  opcode_entry    Shft1[] = {
/*************************/
/*       op1   op2   res   eq      verify     gen           reg fu*/

/* optimizing reductions*/

{_Bin(   ANY,  C,    ANY,  NONE  ),V_OP2NEG,  R_CHANGESHIFT,RG_BYTE_SHIFT,FU_NO},
{_Bin(   ANY,  C,    ANY,  NONE  ),NVI(V_OP2ZERO),R_MAKEMOVE,RG_BYTE,FU_NO},
{_Bin(   ANY,  C,    ANY,  NONE  ),V_SHIFT2BIG,R_FIXSHIFT,  RG_BYTE_SHIFT,FU_NO},

/* instructions we can generate*/

{_Bin(   C,    C,    ANY,  EQ_R1 ),V_NO,      R_MOVOP1TEMP, RG_BYTE,FU_NO},
{_Bin(   R,    C,    R,    EQ_R1 ),V_LSHONE,  R_ADDRR,      RG_BYTE,FU_NO},
{_Bin(   R,    C,    R,    EQ_R1 ),V_OP2ONE,  G_R1SHIFT,    RG_BYTE,FU_ALU1},
{_Bin(   M,    C,    M,    EQ_R1 ),V_OP2ONE,  G_1SHIFT,     RG_,FU_ALU1},
{_Bin(   R,    C,    R,    EQ_R1 ),V_80186,   G_RNSHIFT,    RG_BYTE,FU_ALU1},
{_Bin(   M,    C,    M,    EQ_R1 ),V_80186,   G_NSHIFT,     RG_,FU_ALU1},
{_Bin(   R,    C,    R,    EQ_R1 ),V_OP2TWO,  R_DOUBLEHALF, RG_BYTE,FU_NO},
{_Bin(   M,    C,    M,    EQ_R1 ),V_OP2TWO,  R_DOUBLEHALF, RG_,FU_NO},
{_Bin(   R,    R,    R,    EQ_R1 ),V_NO,      G_RCLSHIFT,   RG_BYTE_SHIFT,FU_ALU1},
{_Bin(   M,    R,    M,    EQ_R1 ),V_NO,      G_CLSHIFT,    RG_BYTE_SHIFT,FU_ALU1},

/* simplifying reductions*/

{_Bin(   R|M|C,C,    R|M,  NONE ), V_80186,   R_OP1RESTEMP,RG_BYTE_SHIFT,FU_NO},
{_Bin(   R|M|C,C,    R|M,  NONE ), V_OP2ONE,  R_OP1RESTEMP,RG_BYTE_SHIFT,FU_NO},
{_Bin(   R|M|C,C,    R|M,  NONE ), V_OP2TWO,  R_OP1RESTEMP,RG_BYTE_SHIFT,FU_NO},
{_Bin(   R|M|C,R,    R|M,  NONE ), V_NO,      R_OP1RESTEMP,RG_BYTE_SHIFT,FU_NO},
{_Bin(   ANY,  C,    ANY,  EQ_R1), V_OP2ONE,  G_UNKNOWN,   RG_BYTE,FU_NO},
{_Bin(   ANY,  C,    ANY,  NONE ), V_OP2ONE,  G_UNKNOWN,   RG_BYTE_NEED,FU_NO},
{_Bin(   ANY,  C,    ANY,  EQ_R1), V_OP2TWO,  G_UNKNOWN,   RG_BYTE,FU_NO},
{_Bin(   ANY,  C,    ANY,  NONE ), V_OP2TWO,  G_UNKNOWN,   RG_BYTE_NEED,FU_NO},
{_Bin(   ANY,  C,    ANY,  EQ_R1), V_80186,   G_UNKNOWN,   RG_BYTE,FU_NO},
{_Bin(   ANY,  C,    ANY,  NONE ), V_80186,   G_UNKNOWN,   RG_BYTE_NEED,FU_NO},
{_Bin(   ANY,  R,    ANY,  EQ_R1), V_NO,      G_UNKNOWN,   RG_BYTE_SHIFT,FU_NO},
{_Bin(   ANY,  R,    ANY,  NONE ), V_NO,      G_UNKNOWN,   RG_BYTE_SHIFT_NEED,FU_NO},
{_Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,      R_OP2CL,     RG_,FU_NO},
};

static  opcode_entry    Shft2[] = {
/*************************/
/*       op1   op2   res   eq      verify     gen           reg fu*/

/* optimizing reductions*/

{_Bin(   ANY,  C,    ANY,  NONE  ),V_OP2NEG,  R_CHANGESHIFT,RG_WORD_SHIFT,FU_NO},
{_Bin(   ANY,  C,    ANY,  NONE  ),NVI(V_OP2ZERO),R_MAKEMOVE,RG_WORD,FU_NO},
{_Bin(   ANY,  C,    ANY,  NONE  ),V_SHIFT2BIG,R_FIXSHIFT,  RG_WORD_SHIFT,FU_NO},
{_Bin(   R,    C,    R,    NONE  ),NVI(V_CYP2SHIFT),R_CYPSHIFT,RG_TWOBYTE,FU_NO},
{_Bin(   M|U,  C,    M|U,  NONE  ),NVI(V_CYP2SHIFT),R_CYPSHIFT,RG_WORD,FU_NO},

/* instructions we can generate*/

{_Bin(   C,    C,    ANY,  EQ_R1 ),V_NO,      R_MOVOP1TEMP, RG_WORD,FU_NO},
{_Bin(   R,    C,    R,    EQ_R1 ),V_LSHONE,  R_ADDRR,      RG_WORD,FU_NO},
{_Bin(   R,    C,    R,    EQ_R1 ),V_OP2ONE,  G_R1SHIFT,    RG_WORD,FU_ALU1},
{_Bin(   M,    C,    M,    EQ_R1 ),V_OP2ONE,  G_1SHIFT,     RG_,FU_ALU1},
{_Bin(   R,    C,    R,    EQ_R1 ),V_80186,   G_RNSHIFT,    RG_WORD,FU_ALU1},
{_Bin(   M,    C,    M,    EQ_R1 ),V_80186,   G_NSHIFT,     RG_,FU_ALU1},
{_Bin(   R,    C,    R,    EQ_R1 ),V_OP2TWO,  R_DOUBLEHALF, RG_WORD,FU_NO},
{_Bin(   M,    C,    M,    EQ_R1 ),V_OP2TWO,  R_DOUBLEHALF, RG_,FU_NO},
{_Bin(   R,    R,    R,    EQ_R1 ),V_NO,      G_RCLSHIFT,   RG_WORD_SHIFT,FU_ALU1},
{_Bin(   M,    R,    M,    EQ_R1 ),V_NO,      G_CLSHIFT,    RG_WORD_SHIFT,FU_ALU1},

/* simplifying reductions*/

{_Bin(   R|M|C,C,    R|M,  NONE ), V_80186,   R_OP1RESTEMP,RG_WORD_SHIFT,FU_NO},
{_Bin(   R|M|C,C,    R|M,  NONE ), V_OP2ONE,  R_OP1RESTEMP,RG_WORD_SHIFT,FU_NO},
{_Bin(   R|M|C,C,    R|M,  NONE ), V_OP2TWO,  R_OP1RESTEMP,RG_WORD_SHIFT,FU_NO},
{_Bin(   R|M|C,R,    R|M,  NONE ), V_NO,      R_OP1RESTEMP,RG_WORD_SHIFT,FU_NO},
{_Bin(   ANY,  C,    ANY,  EQ_R1), V_OP2ONE,  G_UNKNOWN,   RG_WORD,FU_NO},
{_Bin(   ANY,  C,    ANY,  NONE ), V_OP2ONE,  G_UNKNOWN,   RG_WORD_NEED,FU_NO},
{_Bin(   ANY,  C,    ANY,  EQ_R1), V_OP2TWO,  G_UNKNOWN,   RG_WORD,FU_NO},
{_Bin(   ANY,  C,    ANY,  NONE ), V_OP2TWO,  G_UNKNOWN,   RG_WORD_NEED,FU_NO},
{_Bin(   ANY,  C,    ANY,  EQ_R1), V_80186,   G_UNKNOWN,   RG_WORD,FU_NO},
{_Bin(   ANY,  C,    ANY,  NONE ), V_80186,   G_UNKNOWN,   RG_WORD_NEED,FU_NO},
{_Bin(   ANY,  R,    ANY,  EQ_R1), V_NO,      G_UNKNOWN,   RG_WORD_SHIFT,FU_NO},
{_Bin(   ANY,  R,    ANY,  NONE ), V_NO,      G_UNKNOWN,   RG_WORD_SHIFT_NEED,FU_NO},
{_Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,      R_OP2CL,     RG_,FU_NO},
};


/**/
/* NEVER, NEVER touch this table unless you are feeling VERY brave (j.d)*/
static  opcode_entry    Shft4[] = {
/*************************/
/*       op1   op2   res   eq      verify     gen           reg fu*/

/* optimizing reductions*/

{_Bin(   ANY,  C,    ANY,  NONE  ),V_OP2NEG,  R_CHANGESHIFT,RG_DBL_SHIFT,FU_NO},
{_Bin(   ANY,  C,    ANY,  NONE  ),NVI(V_OP2ZERO), R_MAKEMOVE,   RG_DOUBLE,FU_NO},
{_Bin(   ANY,  C,    ANY,  NONE  ),NVI(V_CYP4SHIFT),R_CYPSHIFT,  RG_DOUBLE,FU_NO},

/* instructions we can generate*/

{_Bin(   C,    C,    ANY,  EQ_R1 ),V_NO,      R_MOVOP1TEMP, RG_DOUBLE,FU_NO},
{_Bin(   R,    C,    R,    EQ_R1 ),V_LSHONE,  R_ADDRR,      RG_DOUBLE,FU_NO},
{_Bin(   R,    C,    R,    EQ_R1 ),V_OP2ONE,  G_4RSHIFT,    RG_DOUBLE,FU_ALU1},
{_Bin(   M,    C,    M,    EQ_R1 ),V_OP2ONE,  G_4SHIFT,     RG_,FU_ALU1},
{_Bin(   R,    C,    R,    EQ_R1 ),V_OP2TWO,  R_DOUBLEHALF, RG_DOUBLE,FU_NO},
{_Bin(   M,    C,    M,    EQ_R1 ),V_OP2TWO,  R_DOUBLEHALF, RG_,FU_NO},
{_Bin(   R,    R,    R,    EQ_R1 ),V_NO,      G_RCXSHIFT,   RG_DBL_SHIFT,FU_ALU1},
{_Bin(   R,    C,    R,    EQ_R1 ),V_NO,      G_RCXSHIFT,   RG_DBL_SHIFT_NEED,FU_ALU1},
{_Bin(   M,    R,    M,    EQ_R1 ),V_NO,      G_CXSHIFT,    RG_DBL_SHIFT,FU_ALU1},
{_Bin(   M,    C,    M,    EQ_R1 ),V_NO,      G_CXSHIFT,    RG_DBL_SHIFT_NEED,FU_ALU1},

/* simplifying reductions*/

{_Bin(   R,    C,    R,    NONE ),V_BYTESHIFT,R_BYTESHIFT, RG_DBL_SHIFT,FU_NO},
{_Bin(   R,    C,    M,    NONE ),V_BYTESHIFT,R_BYTESHIFT, RG_DBL_SHIFT,FU_NO},
{_Bin(   M,    C,    R,    NONE ),V_BYTESHIFT,R_BYTESHIFT, RG_DBL_SHIFT,FU_NO},


{_Bin(   ANY,  C,    ANY,  EQ_R1 ), V_OP2ONE,  G_UNKNOWN,   RG_DOUBLE_NEED,FU_NO},
{_Bin(   ANY,  C,    ANY,  EQ_R1 ), V_OP2TWO,  G_UNKNOWN,   RG_DOUBLE_NEED,FU_NO},
{_Bin(   ANY,  R,    ANY,  EQ_R1 ), V_NO,      G_UNKNOWN,   RG_DBL_SHIFT,FU_NO},
{_Bin(   ANY,  C,    ANY,  EQ_R1 ), V_NO,      G_UNKNOWN,   RG_DBL_SHIFT_NEED,FU_NO},
{_Bin(   ANY,  ANY,  ANY,  EQ_R1 ), V_NO,      R_OP2CX,     RG_,FU_NO},
{_Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,       R_OP1RESTEMP,RG_DBL_SHIFT,FU_NO},
};


static  opcode_entry    TestOrCmp1[] = {
/******************************/
/*       op1   op2       verify          gen             reg fu*/
{_Bin(   C,    C,    ANY,  EQ_R1 ),V_NO,      R_MOVOP1TEMP, RG_DOUBLE,FU_NO},
{_Side(  R,    U ),      V_CONSTTEMP,    R_TEMP2CONST,   RG_BYTE,FU_NO},
{_Side(  U,    R ),      V_CONSTTEMP,    R_TEMP2CONST,   RG_BYTE,FU_NO},
{_Side(  ANY,  ANY ),    NVI(V_CMPTRUE), R_CMPTRUE,      RG_,FU_NO},
{_Side(  ANY,  ANY ),    NVI(V_CMPFALSE),R_CMPFALSE,     RG_,FU_NO},

/* instructions we can generate*/

{_SidCC( R,    R ),      V_NO,           G_RR2,          RG_BYTE,FU_ALU1},
{_SidCC( R,    M ),      V_NO,           G_RM2,          RG_BYTE,FU_ALU1},
{_SidCC( R,    C ),      V_OP2ZERO,      G_TEST,         RG_BYTE,FU_ALU1},
{_SidCC( R,    C ),      V_NO,           G_AC,           RG_BYTE_ACC,FU_ALU1},
{_SidCC( R,    C ),      V_NO,           G_RC,           RG_BYTE,FU_ALU1},
{_SidCC( M,    C ),      V_NO,           G_MC,           RG_,FU_ALU1},

/* simplifying reductions*/

{_Side(  C,    C ),      V_NO,           R_MOVOP1REG,    RG_BYTE,FU_NO},
{_Side(  C,    ANY ),    V_NO,           R_SWAPCMP,      RG_BYTE,FU_NO},
{_Side(  M,    R ),      V_NO,           R_SWAPCMP,      RG_BYTE,FU_NO},
{_Side(  M,    M ),      V_NO,           R_MOVOP1REG,    RG_BYTE,FU_NO},
{_Side(  ANY,  R|C ),    V_NO,           G_UNKNOWN,      RG_BYTE,FU_NO},
{_Side(  R|C,  ANY ),    V_NO,           G_UNKNOWN,      RG_BYTE,FU_NO},
{_Side(  ANY,  ANY ),    V_NO,           G_UNKNOWN,      RG_BYTE_NEED,FU_NO},
};


static  opcode_entry    Test2[] = {
/*************************/
/*       op1   op2       verify          gen             reg fu*/

/* optimizing reductions*/

{_Side(  R|M|U,C ),      NVI(V_OP2LOW_B_ZERO),R_CYPHIGH,RG_TWOBYTE,FU_NO},
{_Side(  R|M|U,C ),      NVI(V_OP2HIGH_B_ZERO),R_CYPLOW,RG_TWOBYTE,FU_NO},
{_Side(  R,    U ),      V_CONSTTEMP,    R_TEMP2CONST,   RG_WORD,FU_NO},
{_Side(  U,    R ),      V_CONSTTEMP,    R_TEMP2CONST,   RG_WORD,FU_NO},
{_Side(  ANY,  C ),      V_CMPTRUE,      R_CMPTRUE,      RG_,FU_NO},
{_Side(  ANY,  C ),      V_CMPFALSE,     R_CMPFALSE,     RG_,FU_NO},

/* instructions we can generate*/

{_SidCC( R,    R ),      V_NO,           G_RR2,          RG_WORD,FU_ALU1},
{_SidCC( R,    M ),      V_NO,           G_RM2,          RG_WORD,FU_ALU1},
{_SidCC( R,    C ),      V_NO,           G_AC,           RG_WORD_ACC,FU_ALU1},
{_SidCC( R,    C ),      V_NO,           G_RC,           RG_WORD,FU_ALU1},
{_SidCC( M,    C ),      V_NO,           G_MC,           RG_,FU_ALU1},

/* simplifying reductions*/

{_Side(  C,    C ),      V_NO,           R_MOVOP1REG,    RG_WORD,FU_NO},
{_Side(  C,    ANY ),    V_NO,           R_SWAPCMP,      RG_WORD,FU_NO},
{_Side(  M,    R ),      V_NO,           R_SWAPCMP,      RG_WORD,FU_NO},
{_Side(  M,    M ),      V_NO,           R_MOVOP1REG,    RG_WORD,FU_NO},
{_Side(  ANY,  R|C ),    V_NO,           G_UNKNOWN,      RG_WORD,FU_NO},
{_Side(  R|C,  ANY ),    V_NO,           G_UNKNOWN,      RG_WORD,FU_NO},
{_Side(  ANY,  ANY ),    V_NO,           G_UNKNOWN,      RG_WORD_NEED,FU_NO},
};


static  opcode_entry    Test4[] = {
/*************************/
/*       op1   op2       verify          gen             reg fu*/
{_Side(  ANY,  C   ),    NVI(V_OP2LOW_W_ZERO),R_CYPHIGH, RG_,FU_NO},
{_Side(  ANY,  C   ),    NVI(V_OP2HIGH_W_ZERO),R_CYPLOW, RG_,FU_NO},
{_Side(  C,    R|M|U ),  V_NO,           R_SWAPCMP,      RG_,FU_NO},
{_Side(  ANY,  ANY ),    V_NO,           R_SPLITCMP,     RG_,FU_NO},
};


static  opcode_entry    Test8[] = {
/*************************/
/*       op1   op2       verify          gen             reg fu*/
{_Side(  C,    R|M|U ),  V_NO,           R_SWAPCMP,      RG_,FU_NO},
{_Side(  ANY,  ANY ),    V_NO,           R_SPLIT8TST,    RG_8,FU_NO},
};


static  opcode_entry    Cmp2[] = {
/************************/
/*       op1   op2       verify          gen             reg fu*/
{_Side(  R,    U ),      V_CONSTTEMP,    R_TEMP2CONST,   RG_WORD,FU_NO},
{_Side(  U,    R ),      V_CONSTTEMP,    R_TEMP2CONST,   RG_WORD,FU_NO},
{_Side(  ANY,  ANY ),    NVI(V_CMPTRUE), R_CMPTRUE,      RG_,FU_NO},
{_Side(  ANY,  ANY ),    NVI(V_CMPFALSE),R_CMPFALSE,     RG_,FU_NO},

/* instructions we can generate*/

{_SidCC( R,    C ),      V_OP2ZERO,      G_TEST,         RG_WORD,FU_ALU1},
{_SidCC( R,    R ),      V_NO,           G_RR2,          RG_WORD,FU_ALU1},
{_SidCC( R,    M ),      V_NO,           G_RM2,          RG_WORD,FU_ALU1},
{_SidCC( R,    C ),      V_NO,           G_AC,           RG_WORD_ACC,FU_ALU1},
{_SidCC( R,    C ),      V_NO,           G_RC,           RG_WORD,FU_ALU1},
{_SidCC( M,    C ),      V_NO,           G_MC,           RG_,FU_ALU1},

/* simplifying reductions*/

{_Side(  ANY,  R ),      V_NO,           R_MOVOP2TEMP,   RG_SEG_SEG,FU_NO},
{_Side(  R,    ANY ),    V_NO,           R_MOVOP1TEMP,   RG_SEG_SEG,FU_NO},
{_Side(  C,    C ),      V_NO,           R_MOVOP1REG,    RG_WORD,FU_NO},
{_Side(  C,    ANY ),    V_NO,           R_SWAPCMP,      RG_WORD,FU_NO},
{_Side(  M,    R ),      V_NO,           R_SWAPCMP,      RG_WORD,FU_NO},
{_Side(  M,    M ),      V_NO,           R_MOVOP1REG,    RG_WORD,FU_NO},
{_Side(  ANY,  R|C ),    V_NO,           G_UNKNOWN,      RG_WORD,FU_NO},
{_Side(  R|C,  ANY ),    V_NO,           G_UNKNOWN,      RG_WORD,FU_NO},
{_Side(  ANY,  ANY ),    V_NO,           G_UNKNOWN,      RG_WORD_NEED,FU_NO},
};


static  opcode_entry    Cmp4[] = {
/************************/
/*       op1   op2       verify          gen             reg fu*/

// 2006-06-01 RomanT: It's not effective. Compare is DoNothing()'ed and
//                    operands are stuck unsplit, poisoning all analysis.
// {_Side(  ANY,  C ),      V_CMPTRUE,      R_CMPTRUE,      RG_,FU_NO},
// {_Side(  ANY,  C ),      V_CMPFALSE,     R_CMPFALSE,     RG_,FU_NO},
{_Side(  ANY,  C   ),    V_U_TEST,       R_U_TEST,       RG_DOUBLE,FU_NO},
{_Side(  C,    R|M|U ),  V_NO,           R_SWAPCMP,      RG_DOUBLE,FU_NO},
{_Side(  ANY,  ANY ),    V_NO,           R_SPLITCMP,     RG_DOUBLE,FU_NO},
};


static  opcode_entry    Cmp8[] = {
/************************/
/*       op1   op2       verify          gen             reg fu*/

{_Side(  C,    R|M|U ),  V_NO,           R_SWAPCMP,      RG_,FU_NO},
{_Side(  ANY,  ANY ),    V_NO,           R_SPLIT8CMP,    RG_8,FU_NO},
};


static  opcode_entry    CmpFS[] = {
/*************************/
/*       op1   op2       verify          gen             reg fu*/
{_Side(  ANY,  C ),      V_OP2ZERO,      R_HIGHCMP,      RG_DBL_OR_PTR,FU_NO},
{_Side(  ANY,  C ),      V_NO,           R_FSCONSCMP,    RG_DBL_OR_PTR,FU_NO},
{_Side(  C,    R|M|U ),  V_NO,           R_SWAPCMP,      RG_DBL_OR_PTR,FU_NO},
{_Side(  ANY,  ANY ),    V_NO,           R_MAKECMPCALL,  RG_DBL_OR_PTR,FU_NO},
};


static  opcode_entry    CmpFD[] = {
/*************************/
/*       op1   op2       verify          gen             reg fu*/
{_Side(  ANY,  C ),      V_OP2ZERO,      R_HIGHCMP,      RG_8,FU_NO},
{_Side(  ANY,  C ),      V_NO,           R_FDCONSCMP,    RG_8,FU_NO},
{_Side(  C,    R|M|U ),  V_NO,           R_SWAPCMP,      RG_8,FU_NO},
{_Side(  ANY,  ANY ),    V_NO,           R_MAKECMPCALL,  RG_8,FU_NO},
};


static  opcode_entry    Move1[] = {
/*************************/
/*       op    res   eq          verify          gen             reg fu*/

/* optimizing reductions*/

{_UnPP(  ANY,  ANY,  EQ_R1 ),    NVI(V_NO),      G_NO,           RG_,FU_NO},
{_UnPP(  M,    M,    NONE  ),    V_SAME_LOCN,    G_NO,           RG_,FU_NO},

/* instructions we can generate*/

{_Un(    C,    R,    NONE ),     V_OP1ZERO,      R_MAKEXORRR,    RG_BYTE,FU_NO},
{_UnPP(  C,    R,    NONE ),     V_NO,           G_MOVRC,        RG_BYTE,FU_ALU1},
{_UnPP(  C,    M,    NONE ),     V_NO,           G_MOVMC,        RG_,FU_ALU1},
{_UnPP(  R,    M,    NONE ),     V_RESLOC,       G_MOVMA,        RG_BYTE_ACC,FU_ALU1},
{_UnPP(  M,    R,    NONE ),     V_OP1LOC,       G_MOVAM,        RG_BYTE_ACC,FU_ALU1},
{_UnPP(  R,    R,    NONE ),     V_NO,           G_RR1,          RG_BYTE,FU_ALU1},
{_UnPP(  M,    R,    NONE ),     V_NO,           G_RM1,          RG_BYTE,FU_ALU1},
{_UnPP(  R,    M,    NONE ),     V_NO,           G_MR1,          RG_BYTE,FU_ALU1},

/* simplifying reductions*/

{_Un(    M,    M,    NONE ),     V_NO,           R_MOVOP1REG,    RG_BYTE,FU_NO},
{_Un(    ANY,  R,    NONE ),     V_NO,           G_UNKNOWN,      RG_BYTE,FU_NO},
{_Un(    R|C,  ANY,  NONE ),     V_NO,           G_UNKNOWN,      RG_BYTE,FU_NO},
{_Un(    ANY,  ANY,  NONE ),     V_NO,           G_UNKNOWN,      RG_BYTE_NEED,FU_NO},
};

static  opcode_entry    Move2CC[] = {
/***************************/
/*       op    res   eq          verify          gen             reg fu*/

/* optimizing reductions*/

{_Un(    C,    R,    NONE ),     V_OP1ZERO,      R_MAKEXORRR,    RG_WORD,FU_NO},

/* fall through into move2 table*/
/**** NB. Move2 points here ****/
/* opcode_entry    Move2[]; */
/*************************/
/*       op    res   eq          verify          gen             reg fu*/

/* optimizing reductions*/

{_UnPP(  ANY,  ANY,  EQ_R1 ),    NVI(V_NO),      G_NO,           RG_,FU_NO},
{_UnPP(  M,    M,    NONE  ),    V_SAME_LOCN,    G_NO,           RG_,FU_NO},

/* instructions we can generate*/

{_UnPP(  C,    R,    NONE ),     V_NO,           G_MOVRC,        RG_WORD,FU_ALU1},
{_UnPP(  C,    M,    NONE ),     V_NO,           G_MOVMC,        RG_,FU_ALU1},
{_UnPP(  R,    M,    NONE ),     V_RESLOC,       G_MOVMA,        RG_WORD_ACC,FU_ALU1},
{_UnPP(  M,    R,    NONE ),     V_OP1LOC,       G_MOVAM,        RG_WORD_ACC,FU_ALU1},
{_UnPP(  R,    R,    NONE ),     V_NO,           G_RR1,          RG_WORD,FU_ALU1},
{_UnPP(  M,    R,    NONE ),     V_NO,           G_RM1,          RG_WORD,FU_ALU1},
{_UnPP(  R,    M,    NONE ),     V_NO,           G_MR1,          RG_WORD,FU_ALU1},
{_UnPP(  R,    R,    NONE ),     V_NO,           G_SR,           RG_WORD_SEG,FU_ALU1},
{_UnPP(  R,    R,    NONE ),     V_NO,           G_RS,           RG_SEG_WORD,FU_ALU1},
{_UnPP(  R,    M,    NONE ),     V_NO,           G_MS1,          RG_SEG_WORD,FU_ALU1},
{_UnPP(  M,    R,    NONE ),     V_NO,           G_SM1,          RG_WORD_SEG,FU_ALU1},
{_UnPP(  R,    R,    NONE ),     V_SIZE,         G_SEG_SEG,      RG_SEG_SEG,FU_ALU1},
{_UnPP(  R,    R,    NONE ),     V_WORDREG_AVAIL,R_SEG_SEG,      RG_SEG_SEG,FU_NO},
{_UnPP(  R,    R,    NONE ),     V_NO,           G_SEG_SEG,      RG_SEG_SEG,FU_ALU1},

/* simplifying reductions*/

{_Un(    C,    R,    NONE ),     V_NO,           R_MOVOP1REG,    RG_WORD_SEG,FU_NO},
{_Un(    M,    M,    NONE ),     V_NO,           R_MOVOP1REG,    RG_WORD,FU_NO},
{_Un(    R|M|U,R,    NONE ),     V_NO,           G_UNKNOWN,      RG_ANYWORD,FU_NO},
{_Un(    R,    ANY,  NONE ),     V_NO,           G_UNKNOWN,      RG_ANYWORD,FU_NO},
{_Un(    ANY,  ANY,  NONE ),     V_NO,           G_UNKNOWN,      RG_ANYWORD_NEED,FU_NO},
};

/* Point at where Move2 used to start */
/*************************/
opcode_entry   *Move2 = &Move2CC[1]; /* used from intel/c/i86split.c */

static opcode_entry    Move4op[] = {
/*************************/
/*       op    res   eq          verify          gen             reg fu*/
/*Un(    C,    ANY,  NONE ),    NVI(V_HIGHEQLOW),R_HIGHLOWMOVE,RG_DOUBLE,FU_NO,*/
{_Un(    ANY,  ANY,  NONE ),     V_NO,           R_SPLITMOVE,    RG_DOUBLE,FU_NO},
};

/* Pointer to Move4 table (required for symmetry with 386table.c) */
/*************************/
opcode_entry    *Move4 = Move4op; /* used from intel/c/i86split.c */

static  opcode_entry    Move8[] = {
/*************************/
/*       op    res   eq          verify          gen             reg fu*/
{_Un(    R|M,  R|M,  EQ_R1 ),    NVI(V_NO),      G_NO,           RG_8,FU_NO},
{_Un(    M,    M,    NONE  ),    V_SAME_LOCN,    G_NO,           RG_,FU_NO},
{_Un(    M,    M,    NONE ),     V_NO,           R_MAYBSTRMOVE,  RG_,FU_NO},
/*Un(    R|M|C,R|M,  NONE ),     V_NO,           R_SPLIT8,       RG_8,FU_NO,*/
/*Un(    ANY,  ANY,  NONE ),     V_NO,           G_UNKNOWN,      RG_8_NEED_WORD,FU_NO,*/
{_Un(    ANY,  ANY,  NONE ),     V_NO,           R_SPLIT8,       RG_,FU_NO},
};


static  opcode_entry    MoveXX[] = {
/**************************/
/*       op    res   eq          verify          gen             reg fu*/
{_Un(    M|U,  M|U,  EQ_R1 ),    NVI(V_NO),      G_NO,           RG_,FU_NO},
{_UnPP(  M,    M,    NONE  ),    V_SAME_LOCN,    G_NO,           RG_,FU_NO},
{_Un(    U,    ANY,  NONE ),     V_NO,           R_FORCEOP1MEM,  RG_,FU_NO},
{_Un(    ANY,  U,    NONE ),     V_NO,           R_FORCERESMEM,  RG_,FU_NO},
{_Un(    ANY,  ANY,  NONE ),     V_NO,           R_MAKESTRMOVE,  RG_,FU_NO},
};


static  opcode_entry    CmpXX[] = {
/*************************/
/*       op1   op2       verify          gen             reg fu*/
{_Side(  U,    ANY ),    V_NO,           R_FORCEOP1MEM,  RG_,FU_NO},
{_Side(  ANY,  U   ),    V_NO,           R_FORCEOP2MEM,  RG_,FU_NO},
{_Side(  ANY,  ANY ),    V_NO,           R_MAKESTRCMP,   RG_,FU_NO},
};


opcode_entry    String[] = {
/**************************/
/*       op1   len       verify          gen             reg fu*/
{_Side(  ANY,  ANY ),    V_NO,           G_REPOP,        RG_,FU_ALU1},
};


static  opcode_entry    LoadA4[] = {
/**************************/
/*       op    res   eq          verify          gen             reg fu*/
{_Un(    M,    ANY,  NONE ),     V_NO,           R_LOADLONGADDR, RG_DBL_OR_PTR,FU_NO},
{_Un(    ANY,  ANY,  NONE ),     V_NO,           R_FORCEOP1MEM,  RG_DBL_OR_PTR,FU_NO},
};


static  opcode_entry    LoadA[] = {
/*************************/
/*       op    res   eq          verify          gen             reg fu*/

/* optimizing reductions*/

{_Un(    M,    ANY,  NONE ),     V_OFFSETZERO,    R_MOVEINDEX,   RG_WORD,FU_NO},

/* instructions we can generate*/

{_UnPP(  M,    M,    NONE ),     V_OP1ADDR,      G_MADDR,        RG_,FU_ALU1},
{_UnPP(  M,    R,    NONE ),     V_OP1ADDR,      G_RADDR,        RG_WORD,FU_ALU1},
{_UnPP(  M,    R,    NONE ),     V_NO,           G_LOADADDR,     RG_WORD,FU_ALU1},

/* simplifying reductions*/

{_Un(    M,    ANY,  NONE ),     V_OP1ADDR,      G_UNKNOWN,      RG_WORD,FU_NO},
{_Un(    M,    M,    NONE ),     V_NO,           R_RESREG,       RG_WORD,FU_NO},
{_Un(    M,    ANY,  NONE ),     V_NO,           G_UNKNOWN,      RG_WORD_NEED,FU_NO},
{_Un(    ANY,  ANY,  NONE ),     V_NO,           R_FORCEOP1MEM,  RG_WORD,FU_NO},
};


static  opcode_entry    Not1[] = {
/************************/
/*       op    res   eq          verify          gen             reg fu*/

/* instructions we can generate*/

{_UnPP(  R,    R,    EQ_R1 ),    V_NO,           G_R1,           RG_BYTE,FU_ALU1},
{_UnPP(  M,    M,    EQ_R1 ),    V_NO,           G_M1,           RG_,FU_ALU1},

/*       Reduction routines*/

{_Un(    R|M|C,R,    NONE ),     V_NO,           R_MOVOP1RES,    RG_BYTE,FU_NO},
{_Un(    R|M|C,M,    NONE ),     V_NO,           R_MOVOP1RES,    RG_BYTE,FU_NO},
{_Un(    ANY,  R,    NONE ),     V_NO,           G_UNKNOWN,      RG_BYTE,FU_NO},
{_Un(    R,    ANY,  NONE ),     V_NO,           G_UNKNOWN,      RG_BYTE,FU_NO},
{_Un(    ANY,  ANY,  EQ_R1 ),    V_NO,           G_UNKNOWN,      RG_BYTE,FU_NO},
{_Un(    ANY,  ANY,  NONE ),     V_NO,           G_UNKNOWN,      RG_BYTE_NEED,FU_NO},
};


static  opcode_entry    Not2[] = {
/************************/
/*       op    res   eq          verify          gen             reg fu*/

/* instructions we can generate*/

{_UnPP(  R,    R,    EQ_R1 ),    V_NO,           G_R1,           RG_WORD,FU_ALU1},
{_UnPP(  M,    M,    EQ_R1 ),    V_NO,           G_M1,           RG_,FU_ALU1},

/*       Reduction routines*/

{_Un(    R|M|C,R,    NONE ),     V_NO,           R_MOVOP1RES,    RG_WORD,FU_NO},
{_Un(    R|M|C,M,    NONE ),     V_NO,           R_MOVOP1RES,    RG_WORD,FU_NO},
{_Un(    ANY,  R,    NONE ),     V_NO,           G_UNKNOWN,      RG_WORD,FU_NO},
{_Un(    R,    ANY,  NONE ),     V_NO,           G_UNKNOWN,      RG_WORD,FU_NO},
{_Un(    ANY,  ANY,  EQ_R1 ),    V_NO,           G_UNKNOWN,      RG_WORD,FU_NO},
{_Un(    ANY,  ANY,  NONE ),     V_NO,           G_UNKNOWN,      RG_WORD_NEED,FU_NO},
};


static  opcode_entry    Not4[] = {
/************************/
/*       op    res   eq          verify          gen             reg fu*/
{_Un(    ANY,  ANY,  NONE ),     V_NO,           R_SPLITUNARY,   RG_DOUBLE,FU_NO},
};


static  opcode_entry    Not8[] = {
/************************/
/*       op    res   eq          verify          gen             reg fu*/
{_Un(    ANY,  ANY,  NONE ),     V_NO,           R_SPLIT8,       RG_8,FU_NO},
};


static  opcode_entry    Neg1[] = {
/************************/
/*       op    res   eq          verify          gen             reg fu*/

/* instructions we can generate*/

{_Un(  R,    R,    EQ_R1 ),      V_NO,           G_R1,           RG_BYTE,FU_ALU1},
{_Un(  M,    M,    EQ_R1 ),      V_NO,           G_M1,           RG_,FU_ALU1},

/*       Reduction routines*/

{_Un(    R|M|C,R,    NONE ),     V_NO,           R_MOVOP1RES,    RG_BYTE,FU_NO},
{_Un(    R|M|C,M,    NONE ),     V_NO,           R_MOVOP1RES,    RG_BYTE,FU_NO},
{_Un(    ANY,  R,    NONE ),     V_NO,           G_UNKNOWN,      RG_BYTE,FU_NO},
{_Un(    R,    ANY,  NONE ),     V_NO,           G_UNKNOWN,      RG_BYTE,FU_NO},
{_Un(    ANY,  ANY,  EQ_R1 ),    V_NO,           G_UNKNOWN,      RG_BYTE,FU_NO},
{_Un(    ANY,  ANY,  NONE ),     V_NO,           G_UNKNOWN,      RG_BYTE_NEED,FU_NO},
};


static  opcode_entry    Neg2[] = {
/************************/
/*       op    res   eq          verify          gen             reg fu*/

/* instructions we can generate*/

{_Un(  R,    R,    EQ_R1 ),      V_NO,           G_R1,           RG_WORD,FU_ALU1},
{_Un(  M,    M,    EQ_R1 ),      V_NO,           G_M1,           RG_,FU_ALU1},

/*       Reduction routines*/

{_Un(    R|M|C,R,    NONE ),     V_NO,           R_MOVOP1RES,    RG_WORD,FU_NO},
{_Un(    R|M|C,M,    NONE ),     V_NO,           R_MOVOP1RES,    RG_WORD,FU_NO},
{_Un(    ANY,  R,    NONE ),     V_NO,           G_UNKNOWN,      RG_WORD,FU_NO},
{_Un(    R,    ANY,  NONE ),     V_NO,           G_UNKNOWN,      RG_WORD,FU_NO},
{_Un(    ANY,  ANY,  EQ_R1 ),    V_NO,           G_UNKNOWN,      RG_WORD,FU_NO},
{_Un(    ANY,  ANY,  NONE ),     V_NO,           G_UNKNOWN,      RG_WORD_NEED,FU_NO},
};


static  opcode_entry    Neg4[] = {
/************************/
/*       op    res   eq          verify          gen             reg fu*/
{_Un(    ANY,  ANY,  NONE ),     V_NO,           R_SPLITNEG,     RG_,FU_NO},
};


static  opcode_entry    Neg8[] = {
/************************/
/*       op    res   eq          verify          gen             reg fu*/
{_Un(    ANY,  ANY,  NONE ),     V_NO,           R_SPLIT8NEG,    RG_8,FU_NO},
};


static  opcode_entry    NegF[] = {
/************************/
/**/
{_Un(    ANY,  ANY,  NONE   ),     V_NO,           R_MAKEFNEG,   RG_,FU_NO},
};


static  opcode_entry    RTCall[] = {
/************************/
/**/
{_Un(    ANY,  ANY,  NONE   ),     V_NO,           R_MAKECALL, RG_,FU_NO},
};


static  opcode_entry    Push1[] = {
/*************************/
/*       op    res   eq          verify          gen             reg fu*/

/* instructions we can generate*/

{_Un(  C,    ANY,  NONE ),       V_NO,           R_EXT_PUSHC,    RG_,FU_NO},
{_Un(  ANY,  ANY,  NONE ),       V_NO,           R_EXT_PUSH1,    RG_,FU_NO},
};


static  opcode_entry    Push2[] = {
/*************************/
/*       op    res   eq          verify          gen             reg fu*/

/* instructions we can generate*/

{_UnPP(  R,    ANY,  NONE ),     V_NO,           G_WORDR1,       RG_WORD,FU_ALU1},
{_UnPP(  R,    ANY,  NONE ),     V_NO,           G_SEGR1,        RG_SEG_WORD,FU_ALU1},
{_UnPP(  M,    ANY,  NONE ),     V_NO,           G_M1,           RG_,FU_ALU1},
{_UnPP(  C,    ANY,  NONE ),     V_80186,        G_C1,           RG_,FU_ALU1},

/* simplifying reductions*/

{_UnPP(  C,    ANY,  NONE ),     V_NO,           R_MOVOP1TEMP,   RG_WORD,FU_NO},
{_Un(    ANY,  ANY,  NONE ),     V_NO,           G_UNKNOWN,      RG_ANYWORD,FU_NO},
};


static  opcode_entry    Pop2[] = {
/********************************/
/*       op    res   eq          verify          gen             reg fu*/

/* instructions we can generate*/

{_UnPP(  ANY,  R,    NONE ),     V_NO,           G_WORDR1,       RG_WORD,FU_ALU1},
{_UnPP(  ANY,  R,    NONE ),     V_NO,           G_SEGR1,        RG_WORD_SEG,FU_ALU1},
};


static  opcode_entry    Push4[] = {
/*************************/
/*       op    res   eq          verify          gen             reg fu*/
{_Un(    ANY,  ANY,  NONE ),     V_NO,           R_SPLITUNARY,   RG_DOUBLE,FU_NO},
};


static  opcode_entry    Push8[] = {
/*************************/
/*       op    res   eq          verify          gen             reg fu*/
{_Un(    ANY,  ANY,  NONE ),     V_NO,           R_SPLIT8,       RG_DOUBLE,FU_NO},
};

static  opcode_entry    PushXX[] = {
/**************************/
/*       op    res   eq          verify          gen             reg fu*/
{_Un(    U,    ANY,  NONE ),     V_NO,           R_FORCEOP1MEM,  RG_,FU_NO},
{_Un(    M,    ANY,  NONE ),     V_NO,           R_DOLONGPUSH,   RG_,FU_NO},
};


/*   Calls always have their return value put in a register name*/

static  opcode_entry    Call[] = {
/************************/
/*       op    op2,  res   eq          verify          gen             reg fu*/
{_Bin(   ANY,  ANY,  ANY,  NONE ),     V_NO,           G_CALL,         RG_,FU_CALL},
};


static  opcode_entry    CallI[] = {
/*************************/
/*       op1   op2   res   eq    verify          gen             reg fu*/
{_Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,         G_ICALL,        RG_,FU_CALL}
};


static  opcode_entry    SJump[] = {
/*************************/
/*       op    res   eq          verify          gen             reg fu*/
{_Un(    R|U,  ANY,  NONE ),     V_NO,           G_RJMP,         RG_WORD,FU_CALL},
{_Un(    M,    ANY,  NONE ),     V_NO,           G_MJMP,         RG_,FU_CALL},
{_Un(    C,    ANY,  NONE ),     V_NO,           G_CJMP,         RG_,FU_CALL},
};


static  opcode_entry    Rtn8[] = {
/************************/
/*       op1   op2   res   eq      verify        gen             reg fu*/
{_Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,         R_MAKECALL,     RG_8,FU_NO},
};



static  opcode_entry    Rtn8C[] = {
/*************************/
/*       op1   op2   res   eq      verify        gen             reg fu*/
{_Bin(   ANY,  ANY,  ANY,  NONE ), V_NO,         R_MAKECALL,     RG_8,FU_NO},
};



static  opcode_entry    Parm[] = {
/************************/
/*       op    res   eq          verify          gen             reg fu*/
{_Un(    ANY,  ANY,  NONE ),     V_NO,           G_NO,           RG_,FU_NO},
};


/*  If a value is returned, op1 will be the proper register name*/

static  opcode_entry    CmpPT[] = {
/*************************/
/*       op1   op2       verify          gen             reg fu*/
{_Side(  ANY,  C ),      V_OP2ZERO,      R_MAKEU4,       RG_DBL_OR_PTR,FU_NO},
{_Side(  C,    R|M|U ),  V_NO,           R_SWAPCMP,      RG_DBL_OR_PTR,FU_NO},
{_Side(  ANY,  ANY ),    V_NO,           R_MAKECALL,     RG_DBL_OR_PTR,FU_NO},
};


static  opcode_entry    BitCP[] = {
/*************************/
/*     op1  op2  res  eq       verify          gen             reg fu*/
{_Bin( ANY, ANY, ANY, NONE ),  V_NO,           R_MAKEU2,       RG_DBL_OR_PTR,FU_NO},
};


static  opcode_entry    CmpCP[] = {
/*************************/
/*       op1   op2       verify          gen             reg fu*/
{_Side(  ANY,  C ),      V_OP2ZERO,      R_CMPCP,        RG_DBL_OR_PTR,FU_NO},
{_Side(  C,    R|M|U ),  V_NO,           R_SWAPCMP,      RG_DBL_OR_PTR,FU_NO},
{_Side(  ANY,  ANY ),    V_CMPEQ,        R_MAKEU4,       RG_DBL_OR_PTR,FU_NO},
{_Side(  ANY,  ANY ),    V_NO,           R_MAKEU2,       RG_DBL_OR_PTR,FU_NO},
};


static  opcode_entry    Cvt[] = {
/***********************/
/*       op1   op2   eq          verify          gen             reg fu*/

{_Un(    ANY,  ANY,  NONE ),     V_NO,           R_DOCVT,        RG_,FU_NO},
};

opcode_entry    DoNop[] = {
/*************************/
/*       op1   op2   res   eq      verify          gen           reg fu*/
{_BinPP( ANY,  ANY,  ANY,  NONE ), V_NO,           G_NO,         RG_,FU_NO},
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
