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


#include "standard.h"
#include "pattern.h"
#include "funits.h"
#include "coderep.h"
#include "opcodes.h"
#include "regset.h"
#include "rtclass.h"
#include "vergen.h"
#include "model.h"

#include "s37conv.def"


extern  name    *       AllocTemp(type_class_def);
extern  instruction*    MakeUnary(opcode_defs,name*,name*,type_class_def);
extern  void            PrefixIns(instruction*,instruction*);
extern  instruction*    MakeMove(name*,name*,type_class_def);
extern  void            ReplIns(instruction*,instruction*);
extern  instruction     *rMAKECALL( instruction *ins );

extern    int   RoutineNum;


static  opcode_entry    C2to1[] = {
/*********************************/
/*    from  to    eq       verify        gen             reg*/
_Un(  C,    M,    NONE ),  V_NO,         G_MVI,          RG_,FU_NO,
_Un(  M,    M,    NONE ),  V_NO,         R_MEMDOWN,      RG_,FU_NO,
_Un(  ANY,  ANY,  NONE ),  V_NO,         G_UNKNOWN,      RG_,FU_NO
};

static  opcode_entry    C4to1[] = {
/*********************************/
/*    from  to    eq       verify        gen             reg*/
_Un(  C,    M,    NONE ),  V_NO,         G_MVI,          RG_,FU_NO,
_Un(  M,    M,    NONE ),  V_NO,         R_MEMDOWN,      RG_,FU_NO,
_Un(  R,    M,    NONE ),  V_NO,         G_STC,          RG_WORDOP1,FU_NO,
_Un(  ANY,  ANY,  NONE ),  V_NO,         G_UNKNOWN,      RG_,FU_NO
};

static  opcode_entry    C4to2[] = {
/*********************************/
/*    from  to    eq       verify        gen             reg*/
_Un(  C,    M,    NONE ),  V_NO,         G_MVC,          RG_,FU_NO,
_Un(  M,    M,    NONE ),  V_NO,         R_MEMDOWN,      RG_,FU_NO,
_Un(  R,    M,    NONE ),  V_NO,         G_STH,          RG_WORDOP1,FU_NO,
_Un(  ANY,  ANY,  NONE ),  V_NO,         G_UNKNOWN,      RG_,FU_NO
};


extern  opcode_entry    S1to4G[] = {
/**********************************/
/*    from  to    eq       verify        gen             reg*/
_Un(  C,    R,    NONE ),  V_NO,         G_LA1,          RG_WORDRES,FU_NO,
_Un(  M,    R,    NONE ),  V_NO,         G_SCONV1,       RG_WORDRES,FU_NO,
_Un(  ANY,  ANY,  NONE ),  V_NO,         G_UNKNOWN,      RG_WORDRES_NEED,FU_NO
};

static  opcode_entry    S1to4[] = {
/*********************************/
/*    from  to    eq       verify        gen             reg*/
_Un(  C,    M,    NONE ),  V_NO,         G_MVC,          RG_,FU_NO,
_Un(  C,    R,    NONE ),  V_NO,         G_LA1,          RG_WORDRES,FU_NO,
_Un(  M,    R,    NONE ),  V_NO,         G_SCONV1,       RG_WORDRES,FU_NO,
_Un(  ANY,  ANY,  NONE ),  V_NO,         R_PREPS1TO4,    RG_,FU_NO
};


extern  opcode_entry    S2to4G[] = {
/**********************************/
/*    from  to    eq       verify        gen             reg*/
_Un(  C,    R,    NONE ),  V_OP1SMALL,   G_LA1,          RG_WORDRES,FU_NO,
_Un(  C|M,  R,    NONE ),  V_NO,         G_LH,           RG_WORDRES,FU_NO,
_Un(  ANY,  ANY,  NONE ),  V_NO,         G_UNKNOWN,      RG_WORDRES_NEED,FU_NO
};

static  opcode_entry    S2to4[] = {
/*********************************/
/*    from  to    eq       verify        gen             reg*/
_Un(  C,    M,    NONE ),  V_NO,         G_MVC,          RG_,FU_NO,
_Un(  C,    R,    NONE ),  V_OP1SMALL,   G_LA1,          RG_WORDRES,FU_NO,
_Un(  C|M,  R,    NONE ),  V_NO,         G_LH,           RG_WORDRES,FU_NO,
_Un(  ANY,  ANY,  NONE ),  V_NO,         R_PREPS2TO4,    RG_,FU_NO
};


extern  opcode_entry    S4to8G[] = {
/*********************************/
/*    from  to    eq       verify        gen             reg*/
_Un( R,    R,    NONE ),   V_CONVPAIR,   G_SCONV8,       RG_MUL,FU_NO,
_Un( ANY,  ANY,  NONE ),   V_NO,         G_UNKNOWN,      RG_MUL_NEED,FU_NO
};

static  opcode_entry    S4to8[] = {
/*********************************/
/*    from  to    eq       verify        gen             reg*/
_Un( ANY,  ANY,   NONE ),   V_NO,        R_PREPCONV8,    RG_,FU_NO
};


extern  opcode_entry    Z1to4G[] = {
/*********************************/
/*    from  to    eq       verify        gen             reg*/
_Un(  C,    R,    NONE ),  V_NO,         G_LA1,          RG_WORDRES,FU_NO,
_Un(  M,    R,    NONE ),  V_NO,         G_UCONV1,       RG_WORDRES,FU_NO,
_Un(  ANY,  ANY,  NONE ),  V_NO,         G_UNKNOWN,      RG_WORDRES_NEED,FU_NO
};

static  opcode_entry    Z1to4[] = {
/*********************************/
/*    from  to    eq       verify        gen             reg*/
_Un(  C,    R,    NONE ),  V_NO,         G_LA1,          RG_WORDRES,FU_NO,
_Un(  M,    R,    NONE ),  V_NO,         G_UCONV1,       RG_WORDRES,FU_NO,
_Un(  ANY,  ANY,  NONE ),  V_NO,         R_PREPZ1TO4,    RG_,FU_NO
};


extern  opcode_entry    Z2to4G[] = {
/*********************************/
/*    from  to    eq       verify        gen              reg*/
_Un(  C,    R,    NONE ),  V_OP1SMALL,   G_LA1,          RG_WORDRES,FU_NO,
_Un(  C,    R,    NONE ),  V_NO,         G_L,            RG_WORDRES,FU_NO,
_Un(  M,    R,    NONE ),  V_NO,         G_UCONV2,       RG_WORDRES,FU_NO,
_Un(  ANY,  ANY,  NONE ),  V_NO,         G_UNKNOWN,      RG_WORDRES_NEED,FU_NO
};

static  opcode_entry    Z2to4[] = {
/*********************************/
/*    from  to    eq       verify        gen              reg*/
_Un(  C,    R,    NONE ),  V_OP1SMALL,   G_LA1,          RG_WORDRES,FU_NO,
_Un(  C,    R,    NONE ),  V_NO,         G_L,            RG_WORDRES,FU_NO,
_Un(  M,    R,    NONE ),  V_NO,         G_UCONV2,       RG_WORDRES,FU_NO,
_Un(  ANY,  ANY,  NONE ),  V_NO,         R_PREPZ2TO4,    RG_,FU_NO
};


static  opcode_entry    Z4to8[] = {
/*********************************/
/*    from  to    eq       verify        gen              reg*/
_Un(  ANY,  ANY,  NONE ),  V_NO,         R_CLRMOV8,       RG_,FU_NO
};


static  opcode_entry    CStoD[] = {
/*********************************/
/*    from  to    eq       verify        gen              reg*/
_Un(  ANY,  ANY,  NONE ),  V_NO,         R_STOD,          RG_,FU_NO
};


static  opcode_entry    CDtoS[] = {
/*********************************/
/*    from  to    eq       verify        gen              reg*/
_Un(  ANY,  ANY,  NONE ),  V_NO,         R_DTOS,          RG_,FU_NO
};

static  opcode_entry    CItoD[] = {
/*********************************/
/*    from  to    eq       verify        gen              reg*/
_Un(  ANY,  ANY,  NONE ),  V_NO,         R_PREPITOD,      RG_,FU_NO
};

static  opcode_entry    CUtoD[] = {
/*********************************/
/*    from  to    eq       verify        gen              reg*/
_Un(  ANY,  ANY,  NONE ),  V_NO,         R_PREPUTOD,      RG_,FU_NO
};

extern  opcode_entry    CNormF[] = {
/*********************************/
/*    from  to    eq       verify        gen          reg*/
_Un(  M,    R,    NONE ),  V_NO,         G_NORMF,    RG_DOUBLE,FU_NO,
_Un(  ANY,  ANY,  NONE ),  V_NO,         G_UNKNOWN,  RG_DOUBLE_NEED,FU_NO
};

static opcode_entry     *CvtAddr[] = {
        &C2to1,
        &C4to1,
        &C4to2,
        &S1to4,
        &S2to4,
        &S4to8,
        &Z1to4,
        &Z2to4,
        &Z4to8,
        &CStoD,
        &CDtoS,
        &CItoD,
        &CUtoD,
        &CItoD,
        &CUtoD,
};

static  rt_class         CvtTable[] = {
/*                               from*/
/*U1    I1     U2     I2     U4     I4     CP     PT     FS     FD       to*/
OK,    OK,    C2TO1, C2TO1, C4TO1, C4TO1, BAD,   C4TO1, CU4,   CU4,     /* U1*/
OK,    OK,    C2TO1, C2TO1, C4TO1, C4TO1, BAD,   C4TO1, CI4,   CI4,     /* I1*/
CI4,   CI4,   OK,    OK,    C4TO2, C4TO2, BAD,   C4TO2, CU4,   CU4,     /* U2*/
CI4,   CI4,   OK,    OK,    C4TO2, C4TO2, BAD,   C4TO2, CI4,   CI4,     /* I2*/
Z1TO4, CI2,   Z2TO4, S2TO4, OK,    OK,    BAD,   OK,    C_S_U, C_D_U,   /* U4*/
Z1TO4, S1TO4, Z2TO4, S2TO4, OK,    OK,    BAD,   OK,    C_S_4, C_D_4,   /* I4*/
BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,     /* CP*/
Z1TO4, CI2,   Z2TO4, S2TO4, OK,    OK,    BAD,   OK,    C_S_U, C_D_U,   /* PT*/
CU4,   CI4,   CU4,   CI4,   C_U4_S,C_I4_S,BAD,   C_U4_S,OK,    CDTOS,   /* FS*/
CU4,   CI4,   CU4,   CI4,   C_U4_D,C_I4_D,BAD,   C_U4_D,CSTOD, OK };    /* FD*/

extern  bool    CvtOk( type_class_def fr, type_class_def to ) {
/*************************************************************/

    if( fr == XX ) return( FALSE );
    if( to == XX ) return( FALSE );
    if( AskHow( fr, to ) != BAD ) return( TRUE );
    return( FALSE );
}


extern  rt_class        AskHow( type_class_def fr, type_class_def to ) {
/**********************************************************************/

    if( to == XX ) { /* special case for 64 bit operand of DIVIDE */
        if( fr == I4 ) return( S4TO8 );
        if( fr == U4 ) return( Z4TO8 );
        return( BAD );
    }
    return( CvtTable[  fr + to * XX  ] );
}


extern  instruction     *rDOCVT( instruction *ins ) {
/**************************************************/

    name        *src;
    name        *dst;
    name        *name;
    instruction *new_ins;
    rt_class    how;

    src = ins->operands[ 0 ];
    dst = ins->result;
    if( src->n.name_class != XX && ins->base_type_class == XX ) {
        ins->base_type_class = src->n.name_class;
    }
    ins->head.state = INS_NEEDS_WORK;
    if( src->n.class == N_CONSTANT && src->c.const_type == CONS_ABSOLUTE
     && ins->type_class != XX ) {
        how = OK;
    } else {
        how = AskHow( ins->base_type_class, ins->type_class );
    }
    if( how < OK ) {
        name = AllocTemp( how );
        new_ins = MakeUnary( ins->head.opcode, src, name, how );
        new_ins->base_type_class = ins->base_type_class;
        new_ins->type_class = how;
        ins->base_type_class = how;
        ins->table = NULL;
        ins->operands[ 0 ] = name;
        PrefixIns( ins, new_ins );
    } else if( how == OK ) {
        new_ins = MakeMove( src, dst, ins->type_class );
        ReplIns( ins, new_ins );
    } else if( how <= LAST_TABLE ) {
        ins->table = CvtAddr[  how - ( OK + 1 )  ];
        new_ins = ins;
    } else {
        RoutineNum = how - BEG_RTNS;
        new_ins = rMAKECALL( ins );
    }
    return( new_ins );
}
