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
* Description:  PowerPC type conversions.
*
****************************************************************************/


#include "cgstd.h"
#include "pattern.h"
#include "coderep.h"
#include "zoiks.h"
#include "makeins.h"
#include "convins.h"
#include "data.h"

extern  name            *AllocS32Const( signed_32 );
extern  name            *AllocTemp(type_class_def);
extern  void            PrefixIns(instruction*,instruction*);
extern  void            ReplIns(instruction*,instruction*);
extern  void            UpdateLive(instruction*,instruction*);


static  opcode_entry    ctable_FSTOD[] = {
/****************************************/
/*        from  to    eq       verify        reg           gen             fu  */
_OE( _Un( R,    R,    NONE ),  V_NO,         RG_FLOAT,     G_MOVE_FP,      FU_NO ),
_OE( _Un( C,    ANY,  NONE ),  V_NO,         RG_FLOAT,     R_FORCEOP1CMEM, FU_NO ),
_OE( _Un( M,    ANY,  NONE ),  V_NO,         RG_FLOAT,     R_MOVOP1TEMP,   FU_NO ),
_OE( _Un( ANY,  M,    NONE ),  V_NO,         RG_FLOAT,     R_MOVRESTEMP,   FU_NO ),
_OE( _Un( ANY,  ANY,  NONE ),  V_NO,         RG_FLOAT_NEED,G_UNKNOWN,      FU_NO ),
_OE( _Un( ANY,  ANY,  NONE ),  V_NO,         RG_,          G_UNKNOWN,      FU_NO ),
};

#if 0
static  opcode_entry    ctable_FDToI4[] = {
/*****************************************/
/*        from  to    eq       verify        reg           gen             fu  */
_OE( _Un( R,    M,    NONE ),  V_NO,         RG_FLOAT,     G_FREGTOMI4,    FU_NO ),
_OE( _Un( M|C,  M,    NONE ),  V_NO,         RG_FLOAT,     R_MOVOP1TEMP,   FU_NO ),
_OE( _Un( ANY,  R,    NONE ),  V_NO,         RG_FD,        R_MOVRESTEMP,   FU_NO ),
_OE( _Un( R,    U,    NONE ),  V_NO,         RG_,          R_FORCERESMEM,  FU_NO ),
_OE( _Un( ANY,  ANY,  NONE ),  V_NO,         RG_FLOAT_NEED,G_UNKNOWN,      FU_NO ),
_OE( _Un( ANY,  ANY,  NONE ),  V_NO,         RG_,          G_UNKNOWN,      FU_NO ),
};
#endif

#define CONVERT_ROUTINE( x, gen, reg )                                               \
static  opcode_entry    ctable_##x[] = {                                             \
/**************************************/                                             \
/*        from  to    eq       verify        reg           gen             fu  */    \
_OE( _Un( R,    R,    NONE ),  V_NO,         RG_##reg,     gen,            FU_ALU ), \
_OE( _Un( R,    M,    NONE ),  V_NO,         RG_##reg,     R_MOVRESTEMP,   FU_ALU ), \
_OE( _Un( M,    ANY,  NONE ),  V_NO,         RG_##reg,     R_MOVOP1TEMP,   FU_ALU ), \
_OE( _Un( ANY,  ANY,  NONE ),  V_NO,         RG_##reg##_NEED,G_UNKNOWN,    FU_ALU ), \
_OE( _Un( ANY,  ANY,  NONE ),  V_NO,         RG_,          G_UNKNOWN,      FU_NO ),  \
};

CONVERT_ROUTINE( Z1TO2, G_ZERO, BW );
CONVERT_ROUTINE( Z1TO4, G_ZERO, BD );
CONVERT_ROUTINE( Z1TO8, G_ZERO, BQ );
CONVERT_ROUTINE( Z2TO4, G_ZERO, WD );
CONVERT_ROUTINE( Z2TO8, G_ZERO, WQ );

CONVERT_ROUTINE( S1TO2, G_SIGN, BW );
CONVERT_ROUTINE( S1TO4, G_SIGN, BD );
CONVERT_ROUTINE( S1TO8, G_SIGN, BQ );
CONVERT_ROUTINE( S2TO4, G_SIGN, WD );
CONVERT_ROUTINE( S2TO8, G_SIGN, WQ );

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

//FIXME: this is way too inefficient (and guaranteed to show up on a benchmark)
static  opcode_entry    CRtn[] = {
/********************************/
/*        from  to    eq       verify        reg           gen             fu  */
_OE( _Un( ANY,  ANY,  NONE ),  V_NO,         RG_FLOAT,     R_MAKECALL,     FU_NO ),
_OE( _Un( ANY,  ANY,  NONE ),  V_NO,         RG_,          G_UNKNOWN,      FU_NO ),
};

#define CVTFUNC_MAPS \
    CVT_MAP( C8TO4 ) \
    CVT_MAP( C8TO2 ) \
    CVT_MAP( C8TO1 ) \
    CVT_MAP( C4TO2 ) \
    CVT_MAP( C4TO1 ) \
    CVT_MAP( C2TO1 ) \
    CVT_MAP( Z1TO2 ) \
    CVT_MAP( Z1TO4 ) \
    CVT_MAP( Z1TO8 ) \
    CVT_MAP( Z2TO4 ) \
    CVT_MAP( Z2TO8 ) \
    CVT_MAP( Z4TO8 ) \
    CVT_MAP( S1TO2 ) \
    CVT_MAP( S1TO4 ) \
    CVT_MAP( S1TO8 ) \
    CVT_MAP( S2TO4 ) \
    CVT_MAP( S2TO8 ) \
    CVT_MAP( S4TO8 ) \
    CVT_MAP( FSTOD )

#define RTFUNC_MAPS \
    RT_MAP( I4TOD, RT_I4TOD ) \
    RT_MAP( U4TOD, RT_U4TOD ) \
    RT_MAP( DTOU4, RT_DTOU4 ) \
    RT_MAP( DTOI4, RT_DTOI4 )

typedef enum {
    #define pick(e) C##e,
    #include "typcldef.h"
    #undef pick
    OK,
    #define CVT_MAP(a) a,
    CVTFUNC_MAPS
    #undef CVT_MAP
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

static  rt_class     RTRoutineTable[] = {
    #define RT_MAP(a,b) b,
    RTFUNC_MAPS
    #undef RT_MAP
};

#define __x__   BAD

static  conv_method         CvtTable[] = {
/*                               from                                                             */
/*U1   I1     U2     I2     U4     I4     U8     I8     CP     PT     FS     FD     FL         to */
OK,    OK,    C2TO1, C2TO1, C4TO1, C4TO1, CU4,   CI4,   C4TO1, C4TO1, CU4,   CU4,   CU4,    /* U1 */
OK,    OK,    C2TO1, C2TO1, C4TO1, C4TO1, CU4,   CI4,   C4TO1, C4TO1, CI4,   CI4,   CI4,    /* I1 */
Z1TO2, S1TO2, OK,    OK,    C4TO2, C4TO2, CU4,   CI4,   C4TO2, C4TO2, CU4,   CU4,   CU4,    /* U2 */
Z1TO2, S1TO2, OK,    OK,    C4TO2, C4TO2, CU4,   CI4,   C4TO2, C4TO2, CI4,   CI4,   CI4,    /* I2 */
Z1TO4, S1TO4, Z2TO4, S2TO4, OK,    OK,    C8TO4, C8TO4, OK,    OK,    CFD,   DTOU4, DTOU4,  /* U4 */
Z1TO4, S1TO4, Z2TO4, S2TO4, OK,    OK,    C8TO4, C8TO4, OK,    OK,    CFD,   DTOI4, DTOI4,  /* I4 */
CU4,   CU4,   CU4,   CU4,   Z4TO8, S4TO8, OK,    OK,    S4TO8, S4TO8, __x__, __x__, __x__,  /* U8 */
CU4,   CI4,   CU4,   CI4,   Z4TO8, S4TO8, OK,    OK,    S4TO8, S4TO8, CFD,   __x__, __x__,  /* I8 */
__x__, __x__, __x__, __x__, OK,    OK,    __x__, __x__, OK,    OK,    __x__, __x__, __x__,  /* CP */
__x__, __x__, __x__, __x__, OK,    OK,    __x__, __x__, OK,    OK,    __x__, __x__, __x__,  /* PT */
CI4,   CI4,   CI4,   CI4,   CI4,   CFD,   CFD,   CFD,   __x__, __x__, OK,    FSTOD, FSTOD,  /* FS */
CI4,   CI4,   CI4,   CI4,   U4TOD, I4TOD, __x__, __x__, __x__, __x__, OK,    OK,    OK,     /* FD */
CI4,   CI4,   CI4,   CI4,   U4TOD, I4TOD, __x__, __x__, __x__, __x__, OK,    OK,    OK,     /* FL */
};

static  conv_method      AskHow( type_class_def fr, type_class_def to ) {
/************************************************************************
    return the conversion method required to convert from "fr" to "to"
*/

    if( to == XX || fr == XX ) {
        return( BAD );
    }
    return( CvtTable[fr + to * XX] );
}

extern  bool    CvtOk( type_class_def fr, type_class_def to ) {
/**************************************************************
    return true if a conversion from "fr" to "to" can be done
*/

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
    new_ins = MakeConvert( ins->operands[0], temp, class, ins->base_type_class );
    new_ins->table = NULL;
    ins->operands[0] = temp;
    ins->base_type_class = class;
    PrefixIns( ins, new_ins );
    UpdateLive( new_ins, ins );
    return( new_ins );
}

extern  instruction     *rDOCVT( instruction *ins ) {
/***************************************************/

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
        how = OK;
    } else {
        how = AskHow( ins->base_type_class, ins->type_class );
    }
    if( how < OK ) {
        new_ins = doConversion( ins, (type_class_def)how );
    } else if( how < BAD && how > OK ) {
        ins->table = CvtAddr[how - ( OK + 1 )];
        new_ins = ins;
    } else if( how > BAD ) {
        ins->table = CRtn;
        RoutineNum = RTRoutineTable[how - ( BAD + 1 )];
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

extern  void    LookupConvertRoutine( instruction *ins ) {
/********************************************************/

    int         rtn;

    #define _Munge( x, y )              ( (x) * XX + (y) )
    switch( _Munge( ins->base_type_class, ins->type_class ) ) {
    case _Munge( I4, FD ):
        rtn = RT_I4TOD;
        break;
    case _Munge( U4, FD ):
        rtn = RT_U4TOD;
        break;
    case _Munge( FD, I4 ):
        rtn = RT_DTOI4;
        break;
    case _Munge( FD, U4 ):
        rtn = RT_DTOU4;
        break;
    default:
        rtn = RT_NOP;
        break;
    }
    RoutineNum = rtn;
}
