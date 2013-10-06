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


#include "cgstd.h"
#include "pattern.h"
#include "coderep.h"
#include "model.h"
#include "makeins.h"
#include "zoiks.h"
#include "convins.h"
#include "data.h"

extern  name            *AllocTemp( type_class_def );
extern  void            MoveSegOp( instruction *, instruction *, int );
extern  void            PrefixIns( instruction *, instruction * );
extern  void            DupSeg( instruction *, instruction * );
extern  void            ReplIns( instruction *, instruction * );


static  opcode_entry    ctable_C2TO1[] = {
/****************************************/
/*    from  to    eq       verify        gen             reg    fu*/
_Un( R,    ANY,  NONE ),  V_NO,       R_MOVELOW,        RG_TWOBYTE,FU_NO,
_Un( R,    ANY,  NONE ),  V_NO,       R_MOVOP1TEMP,     RG_,    FU_NO,
_Un( ANY,  ANY,  NONE ),  V_NO,       R_MOVELOW,        RG_,    FU_NO,
};

static  opcode_entry    ctable_C4TO1[] = {
/****************************************/
/*    from  to    eq          verify          gen        reg    fu*/
_Un( U,    ANY,  NONE ),  V_CONSTTEMP,  G_UNKNOWN,      RG_,    FU_NO,
_Un( C,    ANY,  NONE ),  V_OP1RELOC,   R_MOVOP1TEMP,   RG_,    FU_NO,
_Un( ANY,  ANY,  NONE ),  V_NO,         R_CONVERT_LOW,  RG_,    FU_NO
};

static  opcode_entry    ctable_C4TO2[] = {
/****************************************/
/*    from  to    eq       verify        gen             reg    fu*/
_Un( U,    ANY,  NONE ),  V_CONSTTEMP,  G_UNKNOWN,      RG_,    FU_NO,
_Un( C,    ANY,  NONE ),  V_OP1RELOC,   R_MOVOP1TEMP,   RG_,    FU_NO,
_Un( ANY,  ANY,  NONE ),  V_NO,         R_MOVELOW,      RG_,    FU_NO
};

#define ctable_C8TO2    ctable_C8TO4

static  opcode_entry    ctable_C8TO4[] = {
/****************************************/
/*    from  to    eq       verify        gen             reg    fu*/
_Un( U,    ANY,  NONE ),  V_CONSTTEMP,  G_UNKNOWN,      RG_,    FU_NO,
_Un( C,    ANY,  NONE ),  V_OP1RELOC,   R_MOVOP1TEMP,   RG_,    FU_NO,
_Un( ANY,  ANY,  NONE ),  V_NO,         R_MOVELOW,      RG_,    FU_NO
};

static  opcode_entry    ctable_S1TO2[] = {
/****************************************/
/*    from  to    eq       verify        gen             reg            fu*/
_Un( R,    R,    NONE ),   V_NO,         G_SIGNEX,       RG_CBW,        FU_ALUX,
_Un( R|M,  R,    NONE ),   V_NO,         G_MOVSX,        RG_BYTE_WORD,  FU_ALU1,
_Un( U|C,  R,    NONE ),   V_NO,         G_UNKNOWN,      RG_BYTE_WORD,  FU_NO,
_Un( ANY,  M,    NONE ),   V_NO,         R_MOVRESREG,    RG_BYTE_WORD,  FU_NO,
_Un( ANY,  ANY,  NONE ),   V_NO,         G_UNKNOWN,      RG_BYTE_WORD_NEED_WORD,FU_NO,
};

static  opcode_entry    ctable_S1TO4[] = {
/****************************************/
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

static  opcode_entry    ctable_S1TO8[] = {
/****************************************/
/*    from  to    eq       verify        gen             reg            fu*/
_Un( ANY,  ANY,  NONE ),   V_NO,         R_CONVERT_UP,   RG_,           FU_NO,
};

static  opcode_entry    ctable_S2TO8[] = {
/****************************************/
/*    from  to    eq       verify        gen             reg            fu*/
_Un( ANY,  ANY,  NONE ),   V_NO,         R_CONVERT_UP,   RG_,           FU_NO,
};

static  opcode_entry    ctable_Z1TO8[] = {
/****************************************/
/*    from  to    eq       verify        gen             reg            fu*/
_Un( ANY,  ANY,  NONE ),   V_NO,         R_CONVERT_UP,   RG_,           FU_NO,
};

static  opcode_entry    ctable_Z2TO8[] = {
/****************************************/
/*    from  to    eq       verify        gen             reg            fu*/
_Un( ANY,  ANY,  NONE ),   V_NO,         R_CONVERT_UP,   RG_,           FU_NO,
};

static  opcode_entry    ctable_S2TO4[] = {
/****************************************/
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


static  opcode_entry    ctable_S4TO8[] = {
/****************************************/
/*    from  to    eq       verify        gen             reg        fu*/
_Un( R,    R,    NONE ),   V_CDQ,        G_SIGNEX,       RG_CDQ,    FU_ALUX,
_Un( ANY,  ANY,  NONE ),   V_CDQ,        R_OP1RESREG,    RG_CDQ,    FU_NO,
_Un( ANY,  ANY,  NONE ),   V_NO,         R_CDQ,          RG_,       FU_NO,
};


static  opcode_entry    ctable_Z1TO2[] = {
/****************************************/
/*    from  to    eq       verify        gen             reg            fu*/
_Un( R|M,  R,    NONE ),   V_GOOD_CLR,   R_CLRHIGH_R,    RG_BYTE_2BYTE, FU_NO,
_Un( R|M,  R,    NONE ),   V_NO,         G_MOVZX,        RG_BYTE_WORD,  FU_ALU1,
_Un( U|C,  R,    NONE ),   V_NO,         G_UNKNOWN,      RG_BYTE_WORD,  FU_NO,
_Un( ANY,  M,    NONE ),   V_NO,         R_MOVRESREG,    RG_BYTE_WORD,  FU_NO,
_Un( ANY,  ANY,  NONE ),   V_NO,         G_UNKNOWN,      RG_BYTE_WORD_NEED_WORD,FU_NO,
};


static  opcode_entry    ctable_Z1TO4[] = {
/****************************************/
/*    from  to    eq       verify        gen             reg            fu*/
_Un( R|M,  R,    NONE ),   V_GOOD_CLR,   R_CLRHIGH_R,    RG_BYTE_4BYTE, FU_NO,
_Un( R|M,  R,    NONE ),   V_NO,         G_MOVZX,        RG_BYTE_DBL,   FU_ALU1,
_Un( U|C,  R,    NONE ),   V_NO,         G_UNKNOWN,      RG_BYTE_DBL,   FU_NO,
_Un( ANY,  M,    NONE ),   V_NO,         R_MOVRESREG,    RG_BYTE_DBL,   FU_NO,
_Un( ANY,  ANY,  NONE ),   V_NO,         G_UNKNOWN,      RG_BYTE_DBL_NEED_DBL,FU_NO,
};


static  opcode_entry    ctable_Z2TO4[] = {
/****************************************/
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


static  opcode_entry    ctable_Z4TO8[] = {
/****************************************/
/*    from  to    eq       verify        gen             reg    fu*/
_Un( ANY,  ANY,  NONE ),   V_NO,         R_CLRHIGH_D,    RG_,   FU_NO,
};


static  opcode_entry    ctable_EXT_PT[] = {
/*****************************************/
/*    from  to    eq          verify          gen             reg   fu*/
_Un( ANY,  ANY,  NONE ),     V_NO,           R_EXTPT,        RG_,   FU_NO,
};


static  opcode_entry    ctable_CHP_PT[] = {
/*****************************************/
/*    from  to    eq          verify          gen             reg   fu*/
_Un( ANY,  ANY,  NONE ),     V_NO,           R_CHPPT,        RG_,   FU_NO,
};

static  opcode_entry    ctable_PTTOI8[] = {
/*****************************************/
/*    from  to    eq          verify          gen             reg   fu*/
_Un( ANY,  ANY,  NONE ),     V_NO,           R_MOVPTI8,     RG_,   FU_NO,
};

static  opcode_entry    ctable_I8TOPT[] = {
/*****************************************/
/*    from  to    eq          verify          gen             reg   fu*/
_Un( ANY,  ANY,  NONE ),     V_NO,           R_MOVI8PT,     RG_,   FU_NO,
};

static  opcode_entry    CRtn[] = {
/********************************/
/*    from  to    eq          verify          gen             reg   fu*/
_Un( ANY,  ANY,  NONE ),     V_NO,           R_MAKECALL,     RG_,   FU_NO,
};

#define CVTFUNC_MAPS \
    CVT_MAP( C2TO1 ) \
    CVT_MAP( C4TO1 ) \
    CVT_MAP( C4TO2 ) \
    CVT_MAP( C8TO2 ) \
    CVT_MAP( C8TO4 ) \
    CVT_MAP( S1TO2 ) \
    CVT_MAP( S1TO4 ) \
    CVT_MAP( S1TO8 ) \
    CVT_MAP( S2TO4 ) \
    CVT_MAP( S2TO8 ) \
    CVT_MAP( S4TO8 ) \
    CVT_MAP( Z1TO2 ) \
    CVT_MAP( Z1TO4 ) \
    CVT_MAP( Z1TO8 ) \
    CVT_MAP( Z2TO4 ) \
    CVT_MAP( Z2TO8 ) \
    CVT_MAP( Z4TO8 ) \
    CVT_MAP( EXT_PT ) \
    CVT_MAP( CHP_PT ) \
    CVT_MAP( PTTOI8 ) \
    CVT_MAP( I8TOPT )

#define RTFUNC_MAPS \
    RT_MAP( C_U4_S, RT_U4FS ) \
    RT_MAP( C_I4_S, RT_I4FS ) \
    RT_MAP( C_U4_D, RT_U4FD ) \
    RT_MAP( C_I4_D, RT_I4FD ) \
    RT_MAP( C_S_D,  RT_FSFD ) \
    RT_MAP( C_S_4,  RT_FSI4 ) \
    RT_MAP( C_S_U,  RT_FSU4 ) \
    RT_MAP( C_D_4,  RT_FDI4 ) \
    RT_MAP( C_D_U,  RT_FDU4 ) \
    RT_MAP( C_D_S,  RT_FDFS ) \
    RT_MAP( C_U8_S, RT_U8FS ) \
    RT_MAP( C_I8_S, RT_I8FS ) \
    RT_MAP( C_U8_D, RT_U8FD ) \
    RT_MAP( C_I8_D, RT_I8FD ) \
    RT_MAP( C_S_I8, RT_FSI8 ) \
    RT_MAP( C_S_U8, RT_FSU8 ) \
    RT_MAP( C_D_I8, RT_FDI8 ) \
    RT_MAP( C_D_U8, RT_FDU8 ) \
    RT_MAP( C7U8_S, RT_U8FS7 ) \
    RT_MAP( C7U8_D, RT_U8FD7 ) \
    RT_MAP( C7S_U8, RT_FSU87 ) \
    RT_MAP( C7D_U8, RT_FDU87 )

typedef enum {
    #define pick(e) C##e,
    #include "typcldef.h"
    #undef pick
    OK,
    #define CVT_MAP(a) a,
    CVTFUNC_MAPS
    #undef CVT_MAP
    FPOK,
    BAD,
    #define RT_MAP(a,b) a,
    RTFUNC_MAPS
    #undef RT_MAP
} conv_method;

static opcode_entry     *CvtAddr[] = {
    #define CVT_MAP(a) ctable_##a,
    CVTFUNC_MAPS
    #undef CVT_MAP
};

static  rt_class        RTRoutineTable[] = {
    #define RT_MAP(a,b) b,
    RTFUNC_MAPS
    #undef RT_MAP
};

#define __x__   BAD

static  conv_method     CvtTable[] = {
/*                               from                                                                    */
/*U1    I1     U2     I2     U4      I4      U8      I8      CP      PT     FS      FD      FL        to */
OK,    OK,    C2TO1, C2TO1, C4TO1,  C4TO1,  C8TO2,  C8TO2,  CU4,    __x__, CU4,    CU4,    CU4,    /* U1 */
OK,    OK,    C2TO1, C2TO1, C4TO1,  C4TO1,  C8TO2,  C8TO2,  CU4,    __x__, CI4,    CI4,    CI4,    /* I1 */
Z1TO2, S1TO2, OK,    OK,    C4TO2,  C4TO2,  C8TO2,  C8TO2,  CU4,    __x__, CU4,    CU4,    CU4,    /* U2 */
Z1TO2, S1TO2, OK,    OK,    C4TO2,  C4TO2,  C8TO2,  C8TO2,  CU4,    __x__, CI4,    CI4,    CI4,    /* I2 */
Z1TO4, S1TO4, Z2TO4, S2TO4, OK,     OK,     C8TO4,  C8TO4,  CHP_PT, __x__, C_S_U,  C_D_U,  C_D_U,  /* U4 */
Z1TO4, S1TO4, Z2TO4, S2TO4, OK,     OK,     C8TO4,  C8TO4,  CU4,    __x__, C_S_4,  C_D_4,  C_D_4,  /* I4 */
Z1TO8, S1TO8, Z2TO8, S2TO8, Z4TO8,  S4TO8,  OK,     OK,     PTTOI8, __x__, C_S_U8, C_D_U8, C_D_U8, /* U8 */
Z1TO8, S1TO8, Z2TO8, S2TO8, Z4TO8,  S4TO8,  OK,     OK,     PTTOI8, __x__, C_S_I8, C_D_I8, C_D_I8, /* I8 */
CU4,   CI4,   CU4,   CI4,   EXT_PT, CU4,    I8TOPT, I8TOPT, OK,     __x__, __x__,  __x__,  __x__,  /* CP */
__x__, __x__, __x__, __x__, __x__,  __x__,  __x__,  __x__,  __x__,  __x__, __x__,  __x__,  __x__,  /* PT */
CU4,   CI4,   CU4,   CI4,   C_U4_S, C_I4_S, C_U8_S, C_I8_S, __x__,  __x__, OK,     C_D_S,  C_D_S,  /* FS */
CU4,   CI4,   CU4,   CI4,   C_U4_D, C_I4_D, C_U8_D, C_I8_D, __x__,  __x__, C_S_D,  OK,     OK,     /* FD */
CU4,   CI4,   CU4,   CI4,   C_U4_D, C_I4_D, C_U8_D, C_U8_D, __x__,  __x__, C_S_D,  OK,     OK,     /* FL */
};

static  conv_method     FPCvtTable[] = {
/*                               from                                                                    */
/*U1    I1     U2     I2     U4      I4      U8      I8      CP      PT     FS      FD      FL        to */
OK,    OK,    C2TO1, C2TO1, C4TO1,  C4TO1,  C8TO2,  C8TO2,  CU4,    __x__, CU4,    CU4,    CU4,    /* U1 */
OK,    OK,    C2TO1, C2TO1, C4TO1,  C4TO1,  C8TO2,  C8TO2,  CU4,    __x__, CI4,    CI4,    CI4,    /* I1 */
Z1TO2, S1TO2, OK,    OK,    C4TO2,  C4TO2,  C8TO2,  C8TO2,  CU4,    __x__, CU4,    CU4,    CU4,    /* U2 */
Z1TO2, S1TO2, OK,    OK,    C4TO2,  C4TO2,  C8TO2,  C8TO2,  CU4,    __x__, CI4,    CI4,    CI4,    /* I2 */
Z1TO4, S1TO4, Z2TO4, S2TO4, OK,     OK,     C8TO4,  C8TO4,  CHP_PT, __x__, FPOK,   FPOK,   FPOK,   /* U4 */
Z1TO4, S1TO4, Z2TO4, S2TO4, OK,     OK,     C8TO4,  C8TO4,  CU4,    __x__, FPOK,   FPOK,   FPOK,   /* I4 */
Z1TO8, S1TO8, Z2TO8, S2TO8, Z4TO8,  S4TO8,  OK,     OK,     PTTOI8, __x__, C7S_U8, C7D_U8, C7D_U8, /* U8 */
Z1TO8, S1TO8, Z2TO8, S2TO8, Z4TO8,  S4TO8,  OK,     OK,     PTTOI8, __x__, FPOK,   FPOK,   FPOK,   /* I8 */
CU4,   CI4,   CU4,   CI4,   EXT_PT, CU4,    I8TOPT, I8TOPT, OK,     __x__, __x__,  __x__,  __x__,  /* CP */
__x__, __x__, __x__, __x__, __x__,  __x__,  __x__,  __x__,  __x__,  __x__, __x__,  __x__,  __x__,  /* PT */
CU4,   CI4,   CU4,   FPOK,  FPOK,   FPOK,   C7U8_S, FPOK,   __x__,  __x__, FPOK,   FPOK,   FPOK,   /* FS */
CU4,   CI4,   CU4,   FPOK,  FPOK,   FPOK,   C7U8_D, FPOK,   __x__,  __x__, FPOK,   FPOK,   FPOK,   /* FD */
CU4,   CI4,   CU4,   FPOK,  FPOK,   FPOK,   C7U8_D, FPOK,   __x__,  __x__, FPOK,   FPOK,   FPOK,   /* FL */
};

static  conv_method     AskHow( type_class_def fr, type_class_def to )
/*********************************************************************
    return the conversion method required to convert from "fr" to "to"
*/
{
    if( to == XX || fr == XX ) { /* special case for 64 bit operand of IDIV */
        return( BAD );
    }
    if( _FPULevel( FPU_87 ) ) {
        return( FPCvtTable[fr + to * XX] );
    } else {
        return( CvtTable[fr + to * XX] );
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

static instruction *doConversion( instruction *ins, type_class_def class )
/************************************************************************/
{
    name            *temp;
    instruction     *new_ins;

    temp = AllocTemp( class );
    new_ins = MakeUnary( ins->head.opcode, ins->operands[0], temp, class );
    new_ins->base_type_class = ins->base_type_class;
    new_ins->type_class = class;
    ins->base_type_class = class;
    ins->table = NULL;
    ins->operands[0] = temp;
    MoveSegOp( ins, new_ins, 0 );
    PrefixIns( ins, new_ins );
    return( new_ins );
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
    instruction *new_ins;
    conv_method how;

    src = ins->operands[0];
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
        new_ins = doConversion( ins, (type_class_def)how );
    } else if( how == FPOK ) {
        ins->head.opcode = OP_MOV;
        ins->table = NULL;
        ins->u.gen_table = NULL;
        new_ins = ins;
        if( !_IsFloating( ins->type_class ) ) {
            ins->type_class = ins->base_type_class;
        }
    } else if( how > OK && how < FPOK ) {
        ins->table = CvtAddr[how - ( OK + 1 )];
        new_ins = ins;
    } else if( how > BAD ) {
        ins->table = CRtn;
        RoutineNum = RTRoutineTable[how - ( BAD + 1 )];
        new_ins = ins;
    } else {
        new_ins = MakeMove( src, dst, ins->type_class );
        DupSeg(ins,new_ins);
        ReplIns( ins, new_ins );
        if( how != OK ) {
            _Zoiks( ZOIKS_092 );
        }
    }
    return( new_ins );
}

extern  void    LookupConvertRoutine( instruction *ins ) {
/********************************************************/

    conv_method how;

    how = AskHow( ins->base_type_class, ins->type_class );
    RoutineNum = RTRoutineTable[how - ( BAD + 1 )];
}
