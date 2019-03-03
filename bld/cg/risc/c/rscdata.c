/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2018 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Emit data items.
*
****************************************************************************/


#include "_cgstd.h"
#include "coderep.h"
#include "zoiks.h"
#include "reloc.h"
#include "cgmem.h"
#include "data.h"
#include "objout.h"
#include "namelist.h"
#include "opttell.h"
#include "rscobj.h"
#include "feprotos.h"


void    DataAlign( unsigned_32 align )
/************************************/
{
    offset      curr_loc;
    uint        modulus;

    curr_loc = AskLocation();
    modulus = curr_loc % align;
    if( modulus != 0 ) {
        if( !AskSegIsBlank( AskOP() ) ) {
            IterBytes( (offset)( align - modulus ), 0 );
        } else {
            IncLocation( align - modulus );
        }
    }
}

void    DataBytes( unsigned_32 len, const void *src )
/***************************************************/
{
    TellOptimizerByPassed();
    ObjBytes( src, len );
    TellByPassOver();
}

void    DataShort( unsigned_16 val )
/**********************************/
{
    TellOptimizerByPassed();
    ObjBytes( &val, sizeof( val ) );
    TellByPassOver();
}

void    DataLong( unsigned_32 val )
/*********************************/
{
    TellOptimizerByPassed();
    ObjBytes( &val, sizeof( val ) );
    TellByPassOver();
}

// FIXME: This is a cheap, lousy hack
#define MAX_HACK_LEN    ( 8 * 1024 )
static byte     Buffer[MAX_HACK_LEN];

void    IterBytes( offset len, byte pat )
/***************************************/
{
    TellOptimizerByPassed();
    if( len > MAX_HACK_LEN ) {
        byte    *buff;

        buff = CGAlloc( len );
        memset( buff, pat, len );
        ObjBytes( buff, len );
        CGFree( buff );
    } else {
        memset( Buffer, pat, len );
        ObjBytes( Buffer, len );
    }
    TellByPassOver();
}

void    BackPtr( back_handle bck, segment_id segid, offset plus, type_def *tipe )
/*******************************************************************************/
{
    /* unused parameters */ (void)segid; (void)tipe;

    assert( tipe->length == 4 );
    TellOptimizerByPassed();
    OutReloc( bck->lbl, OWL_RELOC_WORD, 0 );
    ObjBytes( &plus, 4 );
    TellByPassOver();
}

void    BackPtrBigOffset( back_handle bck, segment_id segid, offset plus )
/************************************************************************/
{
    /* unused parameters */ (void)segid;

    TellOptimizerByPassed();
    OutReloc( bck->lbl, OWL_RELOC_WORD, 0 );
    ObjBytes( &plus, 4 );
    TellByPassOver();
}

void    BackPtrBase( back_handle bck, segment_id segid )
/******************************************************/
{
    TellOptimizerByPassed();
    OutSegReloc( bck->lbl, segid );
    ObjBytes( "\0\0", 2 );
    TellByPassOver();
}

void    FEPtr( cg_sym_handle sym, type_def *tipe, offset plus )
/*************************************************************/
{
    /* unused parameters */  (void)tipe;

    assert( tipe->length == 4 );
    TellOptimizerByPassed();
    OutReloc( FEBack( sym )->lbl, OWL_RELOC_WORD, 0 );
    ObjBytes( &plus, 4 );
    TellByPassOver();
}

void    FEPtrBaseOffset( cg_sym_handle sym,  offset plus )
/********************************************************/
{
    back_handle     bck;
//    segment_id          segid;

    TellOptimizerByPassed();
    bck = FEBack( sym );
//    segid = FESegID( sym );
    OutReloc( bck->lbl, OWL_RELOC_SECTION_INDEX, 0 );
    ObjBytes( &plus, 4 );
//    OutSegReloc( bck->lbl, segid );
    OutReloc( bck->lbl, OWL_RELOC_SECTION_OFFSET, 0 );
    ObjBytes( "\0\0", 2 );
    TellByPassOver();
}

void    FEPtrBase( cg_sym_handle sym )
/************************************/
{
    segment_id          segid;

    TellOptimizerByPassed();
    segid = FESegID( sym );
    OutSegReloc( FEBack( sym )->lbl, segid );
    ObjBytes( "\0\0", 2 );
    TellByPassOver();
}


void    DataLabel( label_handle lbl )
/***********************************/
{
    TellOptimizerByPassed();
    OutLabel( lbl );
    TellByPassOver();
}

static constant_defn    *GetI64Const( name *cons, type_class_def type_class )
/***************************************************************************/
{
    static constant_defn i64Defn = { NULL, NULL, { 0, 0, 0, 0 }, I8 };

    i64Defn.label = NULL;
    i64Defn.const_class = type_class;
    i64Defn.value[0] = cons->c.lo.uint_value & 0xffff;
    i64Defn.value[1] = ( cons->c.lo.uint_value >> 16 ) & 0xffff;
    i64Defn.value[2] = cons->c.hi.uint_value & 0xffff;
    i64Defn.value[3] = ( cons->c.hi.uint_value >> 16 ) & 0xffff;
    return( &i64Defn );
}

name    *GenFloat( name *cons, type_class_def type_class )
/********************************************************/
{
    constant_defn       *defn;
    segment_id          old_segid;
    name                *result;

    TellOptimizerByPassed();
    if( _IsFloating( type_class ) ) {
        defn = GetFloat( cons, type_class );
    } else {
        defn = GetI64Const( cons, type_class );
    }
    if( defn->label == NULL ) {
        defn->label = AskForLabel( NULL );
        old_segid = SetOP( AskBackSeg() );
        AlignObject( 8 );
        assert( ( AskLocation() & 0x07 ) == 0 );
        OutLabel( defn->label );
        DataBytes( TypeClassSize[type_class], &defn->value );
        SetOP( old_segid );

    }
    result = AllocMemory( defn->label, 0, CG_LBL, type_class );
    result->v.usage |= USE_IN_ANOTHER_BLOCK;
    TellByPassOver();
    return( result );
}
