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
* Description:  Machine type conversion routines.
*
****************************************************************************/


#include "standard.h"
#include "pattern.h"
#include "coderep.h"
#include "opcodes.h"
#include "regset.h"
#include "rtclass.h"
#include "vergen.h"
#include "model.h"
#include "funits.h"
#include "makeins.h"

extern  name            *AllocTemp( type_class_def );
extern  void            MoveSegOp( instruction *, instruction *, int );
extern  void            PrefixIns( instruction *, instruction * );
extern  void            DupSeg( instruction *, instruction * );
extern  void            ReplIns( instruction *, instruction * );

extern  int             RoutineNum;


static  opcode_entry    C2to1[] = {
/*********************************/
/*    from  to    eq       verify        gen             reg    fu*/
_Un( R,    ANY,  NONE ),  V_NO,       R_MOVELOW,        RG_TWOBYTE,FU_NO,
_Un( R,    ANY,  NONE ),  V_NO,       R_MOVOP1TEMP,     RG_,    FU_NO,
_Un( ANY,  ANY,  NONE ),  V_NO,       R_MOVELOW,        RG_,    FU_NO,
};

static  opcode_entry    C4to1[] = {
/*********************************/
/*    from  to    eq          verify          gen        reg    fu*/
_Un( U,    ANY,  NONE ),  V_CONSTTEMP,  G_UNKNOWN,      RG_,    FU_NO,
_Un( C,    ANY,  NONE ),  V_OP1RELOC,   R_MOVOP1TEMP,   RG_,    FU_NO,
_Un( ANY,  ANY,  NONE ),  V_NO,         R_CONVERT_LOW,  RG_,    FU_NO
};

static  opcode_entry    C4to2[] = {
/*********************************/
/*    from  to    eq       verify        gen             reg    fu*/
_Un( U,    ANY,  NONE ),  V_CONSTTEMP,  G_UNKNOWN,      RG_,    FU_NO,
_Un( C,    ANY,  NONE ),  V_OP1RELOC,   R_MOVOP1TEMP,   RG_,    FU_NO,
_Un( ANY,  ANY,  NONE ),  V_NO,         R_MOVELOW,      RG_,    FU_NO
};

static  opcode_entry    C8to4[] = {
/*********************************/
/*    from  to    eq       verify        gen             reg    fu*/
_Un( U,    ANY,  NONE ),  V_CONSTTEMP,  G_UNKNOWN,      RG_,    FU_NO,
_Un( C,    ANY,  NONE ),  V_OP1RELOC,   R_MOVOP1TEMP,   RG_,    FU_NO,
_Un( ANY,  ANY,  NONE ),  V_NO,         R_MOVELOW,      RG_,    FU_NO
};

static  opcode_entry    S1to2[] = {
/*********************************/
/*    from  to    eq       verify        gen             reg            fu*/
_Un( R,    R,    NONE ),   V_NO,         G_SIGNEX,       RG_CBW,        FU_ALUX,
_Un( R|M,  R,    NONE ),   V_NO,         G_MOVSX,        RG_BYTE_WORD,  FU_ALU1,
_Un( U|C,  R,    NONE ),   V_NO,         G_UNKNOWN,      RG_BYTE_WORD,  FU_NO,
_Un( ANY,  M,    NONE ),   V_NO,         R_MOVRESREG,    RG_BYTE_WORD,  FU_NO,
_Un( ANY,  ANY,  NONE ),   V_NO,         G_UNKNOWN,      RG_BYTE_WORD_NEED_WORD,FU_NO,
};

static  opcode_entry    S1to4[] = {
/*********************************/
/*    from  to    eq       verify        gen             reg        fu*/
// _Un( R,    R,    NONE ),   V_NO,         G_SIGNEX,       RG_CBDE,   FU_ALUX,
// _Un( R,    R,    NONE ),   V_NO,         G_SIGNEX,       RG_CBD,    FU_ALUX,
// _Un( ANY,  R,    NONE ),   V_NO,         R_MOVOP1REG,    RG_CBD,    FU_NO,
_Un( M,    R,    NONE ),NVI(V_CYP_SEX),  R_CYP_SEX,      RG_DBL,    FU_NO,
_Un( R|M,  R,    NONE ),   V_NO,         G_MOVSX,        RG_BYTE_DBL,FU_ALU1,
_Un( U|C,  R,    NONE ),   V_NO,         G_UNKNOWN,      RG_BYTE_DBL,FU_NO,
_Un( ANY,  M,    NONE ),   V_NO,         R_MOVRESREG,    RG_BYTE_DBL,FU_NO,
_Un( ANY,  ANY,  NONE ),   V_NO,         G_UNKNOWN,      RG_BYTE_DBL_NEED_DBL,FU_NO,
};

static  opcode_entry    S1to8[] = {
/*********************************/
/*    from  to    eq       verify        gen             reg            fu*/
_Un( ANY,  ANY,  NONE ),   V_NO,         R_CONVERT_UP,   RG_,           FU_NO,
};

static  opcode_entry    S2to8[] = {
/*********************************/
/*    from  to    eq       verify        gen             reg            fu*/
_Un( ANY,  ANY,  NONE ),   V_NO,         R_CONVERT_UP,   RG_,           FU_NO,
};

static  opcode_entry    Z1to8[] = {
/*********************************/
/*    from  to    eq       verify        gen             reg            fu*/
_Un( ANY,  ANY,  NONE ),   V_NO,         R_CONVERT_UP,   RG_,           FU_NO,
};

static  opcode_entry    Z2to8[] = {
/*********************************/
/*    from  to    eq       verify        gen             reg            fu*/
_Un( ANY,  ANY,  NONE ),   V_NO,         R_CONVERT_UP,   RG_,           FU_NO,
};

static  opcode_entry    S2to4[] = {
/*********************************/
/*    from  to    eq       verify        gen             reg        fu*/
_Un( R,    R,    NONE ),   V_NO,         G_SIGNEX,       RG_CWDE,   FU_ALUX,
_Un( R,    R,    NONE ),   V_NO,         G_SIGNEX,       RG_CWD,    FU_ALUX,
_Un( ANY,  R,    NONE ),   V_NO,         R_MOVOP1REG,    RG_CWD,    FU_NO,
_Un( M,    R,    NONE ),NVI(V_CYP_SEX),  R_CYP_SEX,      RG_DBL,    FU_NO,
_Un( R|M,  R,    NONE ),   V_NO,         G_MOVSX,        RG_WORD_DBL,FU_ALU1,
_Un( U|C,  R,    NONE ),   V_NO,         G_UNKNOWN,      RG_WORD_DBL,FU_NO,
_Un( ANY,  M,    NONE ),   V_NO,         R_MOVRESREG,    RG_WORD_DBL,FU_NO,
_Un( ANY,  ANY,  NONE ),   V_NO,         G_UNKNOWN,      RG_WORD_DBL_NEED_DBL,FU_NO,
};


static  opcode_entry    S4to8[] = {
/*********************************/
/*    from  to    eq       verify        gen             reg        fu*/
_Un( R,    R,    NONE ),   V_CDQ,        G_SIGNEX,       RG_CDQ,    FU_ALUX,
_Un( ANY,  ANY,  NONE ),   V_CDQ,        R_OP1RESREG,    RG_CDQ,    FU_NO,
_Un( ANY,  ANY,  NONE ),   V_NO,         R_CDQ,          RG_,       FU_NO,
};


static  opcode_entry    Z1to2[] = {
/*********************************/
/*    from  to    eq       verify        gen             reg            fu*/
_Un( R|M,  R,    NONE ),   V_GOOD_CLR,   R_CLRHIGH_R,    RG_BYTE_2BYTE, FU_NO,
_Un( R|M,  R,    NONE ),   V_NO,         G_MOVZX,        RG_BYTE_WORD,  FU_ALU1,
_Un( U|C,  R,    NONE ),   V_NO,         G_UNKNOWN,      RG_BYTE_WORD,  FU_NO,
_Un( ANY,  M,    NONE ),   V_NO,         R_MOVRESREG,    RG_BYTE_WORD,  FU_NO,
_Un( ANY,  ANY,  NONE ),   V_NO,         G_UNKNOWN,      RG_BYTE_WORD_NEED_WORD,FU_NO,
};


static  opcode_entry    Z1to4[] = {
/*********************************/
/*    from  to    eq       verify        gen             reg            fu*/
_Un( R|M,  R,    NONE ),   V_GOOD_CLR,   R_CLRHIGH_R,    RG_BYTE_4BYTE, FU_NO,
_Un( R|M,  R,    NONE ),   V_NO,         G_MOVZX,        RG_BYTE_DBL,   FU_ALU1,
_Un( U|C,  R,    NONE ),   V_NO,         G_UNKNOWN,      RG_BYTE_DBL,   FU_NO,
_Un( ANY,  M,    NONE ),   V_NO,         R_MOVRESREG,    RG_BYTE_DBL,   FU_NO,
_Un( ANY,  ANY,  NONE ),   V_NO,         G_UNKNOWN,      RG_BYTE_DBL_NEED_DBL,FU_NO,
};


static  opcode_entry    Z2to4[] = {
/*********************************/
/*    from  to    eq       verify        gen             reg        fu*/
_Un( ANY,  R,    NONE ),   V_NO,         R_CLRHIGH_R,    RG_CWD,    FU_NO,
_Un( R|M,  R,    NONE ),   V_GOOD_CLR,   R_CLRHIGH_R,    RG_WORD_DBL,FU_NO,
_Un( C,    ANY,  NONE ),   V_OP1RELOC,   R_MOVOP1TEMP,   RG_,       FU_NO,
_Un( R|M,  R,    NONE ),   V_NO,         G_MOVZX,        RG_WORD_DBL,FU_ALU1,
_Un( R,    R,    NONE ),   V_NO,         G_RS,           RG_SEG_DBL,FU_ALUX,
_Un( U|C,  R,    NONE ),   V_NO,         G_UNKNOWN,      RG_WORD_DBL,FU_NO,
_Un( ANY,  M,    NONE ),   V_NO,         R_MOVRESREG,    RG_WORD_DBL,FU_NO,
_Un( ANY,  ANY,  NONE ),   V_NO,         G_UNKNOWN,      RG_WORD_DBL_NEED_DBL,FU_NO
};


static  opcode_entry    Z4to8[] = {
/*********************************/
/*    from  to    eq       verify        gen             reg    fu*/
_Un( ANY,  ANY,  NONE ),   V_NO,         R_CLRHIGH_D,    RG_,   FU_NO,
};


static  opcode_entry    ExtPT[] = {
/*********************************/
/*    from  to    eq          verify          gen             reg   fu*/
_Un( ANY,  ANY,  NONE ),     V_NO,           R_EXTPT,        RG_,   FU_NO,
};


static  opcode_entry    ChpPT[] = {
/*********************************/
/*    from  to    eq          verify          gen             reg   fu*/
_Un( ANY,  ANY,  NONE ),     V_NO,           R_CHPPT,        RG_,   FU_NO,
};

static  opcode_entry    PTtoI8[] = {
/**********************************/
/*    from  to    eq          verify          gen             reg   fu*/
_Un( ANY,  ANY,  NONE ),     V_NO,           R_MOVPTI8,     RG_,   FU_NO,
};

static  opcode_entry    I8toPT[] = {
/*********************************/
/*    from  to    eq          verify          gen             reg   fu*/
_Un( ANY,  ANY,  NONE ),     V_NO,           R_MOVI8PT,     RG_,   FU_NO,
};

static  opcode_entry    CRtn[] = {
/********************************/
/*    from  to    eq          verify          gen             reg   fu*/
_Un( ANY,  ANY,  NONE ),     V_NO,           R_MAKECALL,     RG_,   FU_NO,
};

static opcode_entry     *CvtAddr[] = {
        C2to1,
        C4to1,
        C4to2,
        C8to4,
        C8to4,
        S1to2,
        S1to4,
        S1to8,
        S2to4,
        S2to8,
        S4to8,
        Z1to2,
        Z1to4,
        Z1to8,
        Z2to4,
        Z2to8,
        Z4to8,
        ExtPT,
        ChpPT,
        PTtoI8,
        I8toPT,
        };

static  rt_class         CvtTable[] = {
/*                               from*/
/*U1    I1     U2     I2     U4     I4     U8     I8     CP     PT     FS     FD     FL       to*/
OK,    OK,    C2TO1, C2TO1, C4TO1, C4TO1, C8TO2, C8TO2, CU4,   BAD,   CU4,   CU4,   CU4,     /* U1*/
OK,    OK,    C2TO1, C2TO1, C4TO1, C4TO1, C8TO2, C8TO2, CU4,   BAD,   CI4,   CI4,   CI4,     /* I1*/
Z1TO2, S1TO2, OK,    OK,    C4TO2, C4TO2, C8TO2, C8TO2, CU4,   BAD,   CU4,   CU4,   CU4,     /* U2*/
Z1TO2, S1TO2, OK,    OK,    C4TO2, C4TO2, C8TO2, C8TO2, CU4,   BAD,   CI4,   CI4,   CI4,     /* I2*/
Z1TO4, S1TO4, Z2TO4, S2TO4, OK,    OK,    C8TO4, C8TO4, CHP_PT,BAD,   C_S_U, C_D_U, C_D_U,   /* U4*/
Z1TO4, S1TO4, Z2TO4, S2TO4, OK,    OK,    C8TO4, C8TO4, CU4,   BAD,   C_S_4, C_D_4, C_D_4,   /* I4*/
Z1TO8, S1TO8, Z2TO8, S2TO8, Z4TO8, S4TO8, OK,    OK,    PTTOI8,BAD,   C_S_U8,C_D_U8,C_D_U8,  /* U8*/
Z1TO8, S1TO8, Z2TO8, S2TO8, Z4TO8, S4TO8, OK,    OK,    PTTOI8,BAD,   C_S_I8,C_D_I8,C_D_I8,  /* I8*/
CU4,   CI4,   CU4,   CI4,   EXT_PT,CU4,   I8TOPT,I8TOPT,OK,    BAD,   BAD,   BAD,   BAD,     /* CP*/
BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,     /* PT*/
CU4,   CI4,   CU4,   CI4,   C_U4_S,C_I4_S,C_U8_S,C_I8_S,BAD,   BAD,   OK,    C_D_S, C_D_S,   /* FS*/
CU4,   CI4,   CU4,   CI4,   C_U4_D,C_I4_D,C_U8_D,C_I8_D,BAD,   BAD,   C_S_D, OK,    OK,      /* FD*/
CU4,   CI4,   CU4,   CI4,   C_U4_D,C_I4_D,C_U8_D,C_U8_D,BAD,   BAD,   C_S_D, OK,    OK,      /* FL*/
};

static  rt_class FPCvtTable[] = {
/*                               from*/
/*U1    I1     U2     I2     U4     I4     U8     I8     CP     PT     FS     FD     FL        to*/
OK,    OK,    C2TO1, C2TO1, C4TO1, C4TO1, C8TO2, C8TO2, CU4,   BAD,   CU4,   CU4,   CU4,     /* U1*/
OK,    OK,    C2TO1, C2TO1, C4TO1, C4TO1, C8TO2, C8TO2, CU4,   BAD,   CI4,   CI4,   CI4,     /* I1*/
Z1TO2, S1TO2, OK,    OK,    C4TO2, C4TO2, C8TO2, C8TO2, CU4,   BAD,   CU4,   CU4,   CU4,     /* U2*/
Z1TO2, S1TO2, OK,    OK,    C4TO2, C4TO2, C8TO2, C8TO2, CU4,   BAD,   CI4,   CI4,   CI4,     /* I2*/
Z1TO4, S1TO4, Z2TO4, S2TO4, OK,    OK,    C8TO4, C8TO4, CHP_PT,BAD,   FPOK,  FPOK,  FPOK,    /* U4*/
Z1TO4, S1TO4, Z2TO4, S2TO4, OK,    OK,    C8TO4, C8TO4, CU4,   BAD,   FPOK,  FPOK,  FPOK,    /* I4*/
Z1TO8, S1TO8, Z2TO8, S2TO8, Z4TO8, S4TO8, OK,    OK,    PTTOI8,BAD,   C7S_U8,C7D_U8,C7D_U8,  /* U8*/
Z1TO8, S1TO8, Z2TO8, S2TO8, Z4TO8, S4TO8, OK,    OK,    PTTOI8,BAD,   FPOK,  FPOK,  FPOK,    /* I8*/
CU4,   CI4,   CU4,   CI4,   EXT_PT,CU4,   I8TOPT,I8TOPT,OK,    BAD,   BAD,   BAD,   BAD,     /* CP*/
BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,   BAD,     /* PT*/
CU4,   CI4,   CU4,   FPOK,  FPOK,  FPOK,  C7U8_S,FPOK,  BAD,   BAD,   FPOK,  FPOK,  FPOK,    /* FS*/
CU4,   CI4,   CU4,   FPOK,  FPOK,  FPOK,  C7U8_D,FPOK,  BAD,   BAD,   FPOK,  FPOK,  FPOK,    /* FD*/
CU4,   CI4,   CU4,   FPOK,  FPOK,  FPOK,  C7U8_D,FPOK,  BAD,   BAD,   FPOK,  FPOK,  FPOK,    /* FL*/
};

extern  rt_class        AskHow( type_class_def fr, type_class_def to )
/*********************************************************************
    return the conversion method required to convert from "fr" to "to"
*/
{
    if( to == XX || fr == XX ) { /* special case for 64 bit operand of IDIV */
        return( BAD );
    }
    if( _FPULevel( FPU_87 ) ) {
        return( FPCvtTable[  fr + to * XX  ] );
    } else {
        return( CvtTable[  fr + to * XX  ] );
    }
}

extern  bool    CvtOk( type_class_def fr, type_class_def to )
/************************************************************
    return true if a conversion from "fr" to "to" can be done
*/
{
    if( fr == XX ) return( FALSE );
    if( to == XX ) return( FALSE );
    if( AskHow( fr, to ) != BAD ) return( TRUE );
    return( FALSE );
}

extern  instruction     *rDOCVT( instruction *ins )
/**************************************************
    decide how to accomplish the conversion, then
    either point the instruction at a new generate
    table, or reduce it into two simpler conversions
*/
{
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
     && ins->type_class != I8
     && ins->type_class != U8
    && ( dst->n.class != N_REGISTER
       || !HW_CEqual( dst->r.reg, HW_DX_AX ) ) ) { /* U2div */
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
        MoveSegOp(ins,new_ins,0);
        PrefixIns( ins, new_ins );
    } else if( how == OK ) {
        new_ins = MakeMove( src, dst, ins->type_class );
        DupSeg(ins,new_ins);
        ReplIns( ins, new_ins );
    } else if( how == FPOK ) {
        ins->head.opcode = OP_MOV;
        ins->table = NULL;
        ins->u.gen_table = NULL;
        new_ins = ins;
        if( !_IsFloating( ins->type_class ) ) {
            ins->type_class = ins->base_type_class;
        }
    } else if( how <= LAST_CONV_TABLE ) {
        ins->table = CvtAddr[  how - ( OK + 1 )  ];
        new_ins = ins;
    } else {
        ins->table = CRtn;
        RoutineNum = how - BEG_RTNS;
        new_ins = ins;
    }
    return( new_ins );
}
