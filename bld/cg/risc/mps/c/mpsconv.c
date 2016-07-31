/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  MIPS machine type conversion routines.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "zoiks.h"
#include "makeins.h"
#include "convins.h"
#include "data.h"
#include "namelist.h"
#include "insutil.h"


extern  void            UpdateLive( instruction *, instruction * );
extern  name            *TrimConst( name *, type_class_def );


static  opcode_entry    ctable_FDTOS[] = {
/****************************************/
/*        from  to    eq       verify        reg           gen             fu  */
_OE( _Un( R,    R,   NONE ),   V_NO,         RG_FLOAT,     G_CVTTS,        FU_NO ),
_OE( _Un( C,    ANY, NONE ),   V_NO,         RG_FLOAT,     R_FORCEOP1CMEM, FU_NO ),
_OE( _Un( M,    ANY, NONE ),   V_NO,         RG_FLOAT,     R_MOVOP1TEMP,   FU_NO ),
_OE( _Un( ANY,  M,   NONE ),   V_NO,         RG_FLOAT,     R_MOVRESTEMP,   FU_NO ),
_OE( _Un( ANY,  ANY, NONE ),   V_NO,         RG_FLOAT_NEED,G_UNKNOWN,      FU_NO ),
_OE( _Un( ANY,  ANY, NONE ),   V_NO,         RG_,          G_UNKNOWN,      FU_NO ),
};

static  opcode_entry    ctable_FSTOD[] = {
/****************************************/
/*        from  to    eq       verify        reg           gen             fu  */
_OE( _Un( R,    R,   NONE ),   V_NO,         RG_FLOAT,     G_MOVE_FP,      FU_NO ),
_OE( _Un( C,    ANY, NONE ),   V_NO,         RG_FLOAT,     R_FORCEOP1CMEM, FU_NO ),
_OE( _Un( M,    ANY, NONE ),   V_NO,         RG_FLOAT,     R_MOVOP1TEMP,   FU_NO ),
_OE( _Un( ANY,  M,   NONE ),   V_NO,         RG_FLOAT,     R_MOVRESTEMP,   FU_NO ),
_OE( _Un( ANY,  ANY, NONE ),   V_NO,         RG_FLOAT_NEED,G_UNKNOWN,      FU_NO ),
_OE( _Un( ANY,  ANY, NONE ),   V_NO,         RG_,          G_UNKNOWN,      FU_NO ),
};

static  opcode_entry    ctable_FI8TOD[] = {
/*****************************************/
/*        from  to    eq       verify        reg           gen             fu  */
_OE( _Un( M,    R,   NONE ),   V_NO,         RG_FLOAT,     G_MI8TOFREG,    FU_NO ),
_OE( _Un( M,    M,   NONE ),   V_NO,         RG_FLOAT,     R_MOVRESTEMP,   FU_NO ),
_OE( _Un( R,    ANY, NONE ),   V_NO,         RG_QF,        R_MOVOP1TEMP,   FU_NO ),
_OE( _Un( U,    R,   NONE ),   V_NO,         RG_,          R_FORCEOP1MEM,  FU_NO ),
_OE( _Un( ANY,  ANY, NONE ),   V_NO,         RG_FLOAT_NEED,G_UNKNOWN,      FU_NO ),
_OE( _Un( ANY,  ANY, NONE ),   V_NO,         RG_,          G_UNKNOWN,      FU_NO ),
};

#define ctable_FI8TOS   ctable_FI8TOD

static  opcode_entry    ctable_FDTOI8[] = {
/*****************************************/
/*        from  to    eq       verify        reg           gen             fu  */
_OE( _Un( R,    M,   NONE ),   V_NO,         RG_FLOAT,     G_FREGTOMI8,    FU_NO ),
_OE( _Un( M|C,  M,   NONE ),   V_NO,         RG_FLOAT,     R_MOVOP1REG,    FU_NO ),
_OE( _Un( ANY,  R,   NONE ),   V_NO,         RG_FQ,        R_MOVRESTEMP,   FU_NO ),
_OE( _Un( R,    U,   NONE ),   V_NO,         RG_,          R_FORCERESMEM,  FU_NO ),
_OE( _Un( ANY,  ANY, NONE ),   V_NO,         RG_FLOAT_NEED,G_UNKNOWN,      FU_NO ),
_OE( _Un( ANY,  ANY, NONE ),   V_NO,         RG_,          G_UNKNOWN,      FU_NO ),
};

static  opcode_entry    ctable_FDTOI4[] = {
/*****************************************/
/*        from  to    eq       verify        reg           gen             fu  */
_OE( _Un( R,    M,   NONE ),   V_NO,         RG_FLOAT,     G_FREGTOMI8,    FU_NO ),
_OE( _Un( M|C,  M,   NONE ),   V_NO,         RG_FLOAT,     R_MOVOP1REG,    FU_NO ),
_OE( _Un( ANY,  R,   NONE ),   V_NO,         RG_FQ,        R_MOVRESTEMP,   FU_NO ),
_OE( _Un( R,    U,   NONE ),   V_NO,         RG_,          R_FORCERESMEM,  FU_NO ),
_OE( _Un( ANY,  ANY, NONE ),   V_NO,         RG_FLOAT_NEED,G_UNKNOWN,      FU_NO ),
_OE( _Un( ANY,  ANY, NONE ),   V_NO,         RG_,          G_UNKNOWN,      FU_NO ),
};

#define CONVERT_ROUTINE( x, gen, reg )                                               \
static  opcode_entry    ctable_##x[] = {                                             \
/**************************************/                                             \
/*        from  to    eq       verify        reg           gen             fu  */    \
_OE( _Un( R,    R,    NONE ),  V_NO,         RG_##reg,     gen,            FU_ALU ), \
_OE( _Un( R,    M,    NONE ),  V_NO,         RG_##reg,     R_MOVRESREG,    FU_NO ),  \
_OE( _Un( M,    ANY,  NONE ),  V_NO,         RG_##reg,     R_MOVOP1REG,    FU_NO ),  \
_OE( _Un( ANY,  ANY,  NONE ),  V_NO,         RG_##reg##_NEED,G_UNKNOWN,    FU_NO ),  \
_OE( _Un( ANY,  ANY,  NONE ),  V_NO,         RG_,          G_UNKNOWN,      FU_NO ),  \
};

CONVERT_ROUTINE( Z1TO2, G_ZERO, BW );
CONVERT_ROUTINE( Z1TO4, G_ZERO, BD );
CONVERT_ROUTINE( Z1TO8, G_ZERO, BQ );
CONVERT_ROUTINE( Z2TO4, G_ZERO, WD );
CONVERT_ROUTINE( Z2TO8, G_ZERO, WQ );
//CONVERT_ROUTINE( Z4TO8, G_ZERO, DQ );

CONVERT_ROUTINE( S1TO2, G_SIGN, BW );
CONVERT_ROUTINE( S1TO4, G_SIGN, BD );
CONVERT_ROUTINE( S1TO8, G_SIGN, BQ );
CONVERT_ROUTINE( S2TO4, G_SIGN, WD );
CONVERT_ROUTINE( S2TO8, G_SIGN, WQ );
//CONVERT_ROUTINE( S4TO8, G_SIGN, DQ );

CONVERT_ROUTINE( C8TO2, G_MOVE, QW );
CONVERT_ROUTINE( C8TO1, G_MOVE, QB );
CONVERT_ROUTINE( C4TO2, G_MOVE, DW );
CONVERT_ROUTINE( C4TO1, G_MOVE, DB );
CONVERT_ROUTINE( C2TO1, G_MOVE, WB );

static opcode_entry ctable_C8TO4[] = {
/************************************/
/*        from  to    eq       verify        reg           gen             fu  */
_OE( _Un( ANY,  ANY,  NONE ),  V_NO,         RG_,          R_MOVELOW,      FU_NO ),
};

static opcode_entry ctable_S4TO8[] = {
/************************************/
/*        from  to    eq       verify        reg           gen             fu  */
_OE( _Un( ANY,  ANY,  NONE ),  V_NO,         RG_,          R_SEX_4TO8,     FU_NO ),
};

static opcode_entry ctable_Z4TO8[] = {
/************************************/
/*        from  to    eq       verify        reg           gen             fu  */
_OE( _Un( ANY,  ANY,  NONE ),  V_NO,         RG_,          R_CLRHI_4,      FU_NO ),
};

#define CONVERSIONS \
    _C_( C8TO4 ) \
    _C_( C8TO2 ) \
    _C_( C8TO1 ) \
    _C_( C4TO2 ) \
    _C_( C4TO1 ) \
    _C_( C2TO1 ) \
    _C_( Z1TO2 ) \
    _C_( Z1TO4 ) \
    _C_( Z1TO8 ) \
    _C_( Z2TO4 ) \
    _C_( Z2TO8 ) \
    _C_( Z4TO8 ) \
    _C_( S1TO2 ) \
    _C_( S1TO4 ) \
    _C_( S1TO8 ) \
    _C_( S2TO4 ) \
    _C_( S2TO8 ) \
    _C_( S4TO8 ) \
    _C_( FDTOS ) \
    _C_( FSTOD ) \
    _C_( FI8TOS ) \
    _C_( FI8TOD ) \
    _C_( FDTOI4 ) \
    _C_( FDTOI8 )

typedef enum {
    #define pick(e,t) C##e,
    #include "typcldef.h"
    #undef pick
    OK,
    #define _C_( a )    a,
    CONVERSIONS
    #undef _C_
    BAD
} conv_method;

static opcode_entry     *CvtAddr[] = {
    #define _C_( a )    ctable_##a,
    CONVERSIONS
    #undef _C_
};

#define __x__   BAD

static  conv_method         CvtTable[] = {
/*                               from                                                               */
/*U1   I1     U2     I2     U4     I4     U8     I8      CP     PT     FS     FD      FL         to */
OK,    OK,    C2TO1, C2TO1, C4TO1, C4TO1, C8TO1, C8TO1,  C4TO1, C4TO1, CU4,   CU4,    CU4,    /* U1 */
OK,    OK,    C2TO1, C2TO1, C4TO1, C4TO1, C8TO1, C8TO1,  C4TO1, C4TO1, CI4,   CI4,    CI4,    /* I1 */
Z1TO2, S1TO2, OK,    OK,    C4TO2, C4TO2, C8TO2, C8TO2,  C4TO2, C4TO2, CU4,   CU4,    CU4,    /* U2 */
Z1TO2, S1TO2, OK,    OK,    C4TO2, C4TO2, C8TO2, C8TO2,  C4TO2, C4TO2, CI4,   CI4,    CI4,    /* I2 */
Z1TO4, S1TO4, Z2TO4, S2TO4, OK,    OK,    C8TO4, C8TO4,  OK,    OK,    CFD,   __x__,  __x__,  /* U4 */
Z1TO4, S1TO4, Z2TO4, S2TO4, OK,    OK,    C8TO4, C8TO4,  OK,    OK,    CFD,   FDTOI4, __x__,  /* I4 */
Z1TO8, S1TO8, Z2TO8, S2TO8, Z4TO8, S4TO8, OK,    OK,     S4TO8, S4TO8, __x__, __x__,  __x__,  /* U8 */
Z1TO8, S1TO8, Z2TO8, S2TO8, Z4TO8, S4TO8, OK,    OK,     S4TO8, S4TO8, CFD,   FDTOI8, __x__,  /* I8 */
__x__, __x__, __x__, __x__, OK,    OK,    __x__, __x__,  OK,    OK,    __x__, __x__,  __x__,  /* CP */
__x__, __x__, __x__, __x__, OK,    OK,    __x__, __x__,  OK,    OK,    __x__, __x__,  __x__,  /* PT */
CI8,   CI8,   CI8,   CI8,   CI8,   CI8,   __x__, CFD,    __x__, __x__, OK,    FDTOS,  FDTOS,  /* FS */
CI8,   CI8,   CI8,   CI8,   CI8,   CI8,   __x__, FI8TOD, __x__, __x__, FSTOD, OK,     OK,     /* FD */
CI8,   CI8,   CI8,   CI8,   CI8,   CI8,   __x__, FI8TOD, __x__, __x__, FSTOD, OK,     OK,     /* FL */
};

static conv_method AskHow( type_class_def fr, type_class_def to )
/****************************************************************
    return the conversion method required to convert from "fr" to "to"
*/
{
    if( to == XX || fr == XX ) {
        return( BAD );
    }
    return( CvtTable[fr + to * XX] );
}

extern bool CvtOk( type_class_def fr, type_class_def to )
/********************************************************
    return true if a conversion from "fr" to "to" can be done
*/
{
    if( fr == XX ) return( false );
    if( to == XX ) return( false );
    if( AskHow( fr, to ) != BAD ) return( true );
    return( false );
}

static instruction *doConversion( instruction *ins, type_class_def class )
/************************************************************************/
{
    name            *temp;
    instruction     *new_ins;

    temp = AllocTemp( class );
    new_ins = MakeConvert( ins->operands[0], temp, class, ins->base_type_class );
    new_ins->table = NULL;
    ins->operands[0] = temp;
    ins->base_type_class = class;
    PrefixIns( ins, new_ins );
    UpdateLive( new_ins, ins );
    return( new_ins );
}

extern instruction *rDOCVT( instruction *ins )
/********************************************/
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
     && ins->type_class != XX ) {
        src = TrimConst( src, ins->base_type_class );
        how = OK;
    } else {
        how = AskHow( ins->base_type_class, ins->type_class );
    }
    if( how < OK ) {
        new_ins = doConversion( ins, (type_class_def)how );
    } else if( how < BAD && how > OK ) {
        ins->table = CvtAddr[how - ( OK + 1 )];
        new_ins = ins;
    } else {
        new_ins = MakeMove( src, dst, ins->type_class );
        ReplIns( ins, new_ins );
        if( how != OK ) {
            _Zoiks( ZOIKS_092 );
        }
    }
    return( new_ins );
}

extern void LookupConvertRoutine( instruction *ins )
/**************************************************/
{
    ins = ins;
    RoutineNum = RT_NOP;
    _Zoiks( ZOIKS_101 );
}
