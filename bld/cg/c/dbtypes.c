/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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


#include "_cgstd.h"
#include "coderep.h"
#include "cgmem.h"
#include "symdbg.h"
#include "model.h"
#include "types.h"
#include "zoiks.h"
#ifdef DEVBUILD
#include "echoapi.h"
#endif
#include "i64.h"
#include "utils.h"
#if _TARGET_INTEL
#include "wvtypes.h"
#endif
#include "dw.h"
#include "dftypes.h"
#include "cvdbg.h"
#include "cvtypes.h"
#include "dbsupp.h"
#include "cgprotos.h"


#define MAX_TYPE_SIZE  (1024 * 16)

static bool Nested;     /* set when types are nested by others */

/*
  need to fix dbg_type so it's a handle that can be used
  by any format WV, DW and CV.
  figure how fundamental types are done
*/
dbg_type _CGAPI DBFtnType( cchar_ptr name, dbg_ftn_type tipe )
/********************************************************************/
{
    dbg_type ret;

#ifdef DEVBUILD
    EchoAPI( "DBFtnType( %c, %i )", name, tipe );
#endif
    if( _IsModel( CGSW_GEN_DBG_DF ) ) {
        ret = DFFtnType( name, tipe );
    } else if( _IsModel( CGSW_GEN_DBG_CV ) ) {
        ret = CVFtnType( name, tipe );
    } else {
#if _TARGET_INTEL
        ret = WVFtnType( name, tipe );
#else
        ret = 0;
#endif
    }
#ifdef DEVBUILD
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}


dbg_type _CGAPI DBScalar( cchar_ptr name, cg_type tipe )
/**************************************************************/
{
    dbg_type ret;

#ifdef DEVBUILD
    EchoAPI( "DBScalar( %c,%t )", name, tipe );
#endif
    if( _IsModel( CGSW_GEN_DBG_DF ) ) {
        ret = DFScalar( name, tipe );
    } else if( _IsModel( CGSW_GEN_DBG_CV ) ) {
        ret = CVScalar( name, tipe );
    } else {
#if _TARGET_INTEL
        ret = WVScalar( name, tipe );
#else
        ret = 0;
#endif
    }
#ifdef DEVBUILD
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}



dbg_type _CGAPI DBScope( cchar_ptr name )
/***********************************************/
{
    dbg_type ret;

#ifdef DEVBUILD
    EchoAPI( "DBScope( %c )", name );
#endif
    if( _IsModel( CGSW_GEN_DBG_DF ) ) {
        ret = DFScope( name );
    } else if( _IsModel( CGSW_GEN_DBG_CV ) ) {
        ret = CVScope( name );
    } else {
#if _TARGET_INTEL
        ret = WVScope( name );
#else
        ret = 0;
#endif
    }
#ifdef DEVBUILD
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}



dbg_name _CGAPI DBBegName( cchar_ptr nm, dbg_type scope )
/***************************************************************/
{
    dbg_name    name;
    uint        len;

#ifdef DEVBUILD
    EchoAPI( "DBBegName( %c, %i )", nm, scope );
#endif
    len = strlen( nm );
    name = CGAlloc( sizeof( name_entry ) + len );
    strcpy( name->name, nm );
    name->len = len;
    name->scope = scope;
    name->refno = DBG_NIL_TYPE;
#ifdef DEVBUILD
    EchoAPI( " -> %i\n", name );
#endif
    return( name );
}


dbg_type _CGAPI DBForward( dbg_name name )
/************************************************/
{
#ifdef DEVBUILD
    EchoAPI( "DBForward( %i )", name );
#endif
    if( name->refno == DBG_NIL_TYPE ) {
        if( _IsModel( CGSW_GEN_DBG_DF ) ) {
            /* do nothing */
        } else if( _IsModel( CGSW_GEN_DBG_CV ) ) {
            CVDumpName( name, DBG_FWD_TYPE );
#if _TARGET_INTEL
        } else {
            WVDumpName( name, DBG_FWD_TYPE );
#endif
        }
    }
#ifdef DEVBUILD
    EchoAPI( " -> %i\n", name->refno );
#endif
    return( name->refno );
}


dbg_type _CGAPI DBEndName( dbg_name name, dbg_type tipe )
/***************************************************************/
{
    dbg_type    retv;

#ifdef DEVBUILD
    EchoAPI( "DBEndName( %i, %i )", name, tipe );
#endif
    if( name->refno == DBG_NIL_TYPE ) {
        if( _IsModel( CGSW_GEN_DBG_DF ) ) {
           DFDumpName( name, tipe );
        } else if( _IsModel( CGSW_GEN_DBG_CV ) ) {
           CVDumpName( name, tipe );
#if _TARGET_INTEL
        } else {
           WVDumpName( name, tipe );
#endif
        }
    } else if( _IsModel( CGSW_GEN_DBG_TYPES ) ) {
        if( _IsModel( CGSW_GEN_DBG_DF ) ) {
            DFBackRefType( name, tipe );
        } else if( _IsModel( CGSW_GEN_DBG_CV ) ) {
            CVBackRefType( name, tipe );
#if _TARGET_INTEL
        } else {
            WVBackRefType( name, tipe );
#endif
        }
    }
    retv = name->refno;
    CGFree( name );
#ifdef DEVBUILD
    EchoAPI( " -> %i\n", retv );
#endif
    return( retv );
}


dbg_type _CGAPI DBCharBlock( uint_32 len )
/****************************************/
{
    dbg_type ret;

#ifdef DEVBUILD
    EchoAPI( "DBCharBlock( %i )", len );
#endif
    if( _IsModel( CGSW_GEN_DBG_DF ) ) {
        ret = DFCharBlock( len );
    } else if( _IsModel( CGSW_GEN_DBG_CV ) ) {
        ret = CVCharBlock( len );
    } else {
#if _TARGET_INTEL
        ret = WVCharBlock( len );
#else
        ret = 0;
#endif
    }
    return( ret );
}

dbg_type _CGAPI DBCharBlockNamed( cchar_ptr name, uint_32 len )
/*************************************************************/
{
    dbg_type ret;

#ifdef DEVBUILD
    EchoAPI( "DBCharBlock( %i )", len );
#endif
    if( _IsModel( CGSW_GEN_DBG_DF ) ) {
        ret = DFCharBlockNamed( name, len );
    } else if( _IsModel( CGSW_GEN_DBG_CV ) ) {
        ret = CVCharBlock( len );
    } else {
#if _TARGET_INTEL
        ret = WVCharBlock( len );
#else
        ret = 0;
#endif
    }
    return( ret );
}

dbg_type _CGAPI DBIndCharBlock( back_handle len, cg_type len_type, int off )
/**********************************************************************************/
{
    dbg_type ret;

#ifdef DEVBUILD
    EchoAPI( "DBIndCharBlock( %i, %t, %i )", len,len_type, off );
#endif
    if( _IsModel( CGSW_GEN_DBG_DF ) ) {
        ret = DFIndCharBlock( len, len_type, off );
    } else if( _IsModel( CGSW_GEN_DBG_CV ) ) {
        ret = CVIndCharBlock( len, len_type, off );
    } else {
#if _TARGET_INTEL
        ret = WVIndCharBlock( len, len_type, off );
#else
        ret = 0;
#endif
    }
#ifdef DEVBUILD
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}

dbg_type _CGAPI DBLocCharBlock( dbg_loc loc, cg_type len_type )
/*********************************************************************/
{
    dbg_type ret;

#ifdef DEVBUILD
    EchoAPI( "DBLocCharBlock( %i, %t )", loc, len_type );
#endif

    if( _IsModel( CGSW_GEN_DBG_DF ) ) {
        ret = DFLocCharBlock( loc, len_type );
    } else if( _IsModel( CGSW_GEN_DBG_CV ) ) {
        ret = CVLocCharBlock( loc, len_type );
    } else {
#if _TARGET_INTEL
        ret = WVLocCharBlock( loc, len_type );
#else
        ret = 0;
#endif
    }
#ifdef DEVBUILD
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}


dbg_type _CGAPI DBFtnArray( back_handle dims, cg_type lo_bound_tipe,
                                    cg_type num_elts_tipe, int off,
                                    dbg_type base )
/***************************************************************************/
{
    dbg_type ret;

#ifdef DEVBUILD
    EchoAPI( "DBFtnArray( %B,%t,%t,%i,%i)", dims, lo_bound_tipe,
             num_elts_tipe, off, base );
#endif
    if( _IsModel( CGSW_GEN_DBG_DF ) ) {
        ret = DFFtnArray( dims, lo_bound_tipe, num_elts_tipe, off, base );
    } else if( _IsModel( CGSW_GEN_DBG_CV ) ) {
        ret = CVFtnArray( dims, lo_bound_tipe, num_elts_tipe, off, base );
    } else {
#if _TARGET_INTEL
        ret = WVFtnArray( dims, lo_bound_tipe, num_elts_tipe, off, base );
#else
        ret = 0;
#endif
    }
#ifdef DEVBUILD
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}


dbg_type _CGAPI DBArray( dbg_type idx, dbg_type base )
/************************************************************/
{
    dbg_type ret;

#ifdef DEVBUILD
    EchoAPI( "DBArray( %i, %i)", idx, base );
#endif
    if( _IsModel( CGSW_GEN_DBG_DF ) ) {
        ret = DFArray( idx, base );
    } else if( _IsModel( CGSW_GEN_DBG_CV ) ) {
        ret = CVArray( idx, base );
    } else {
#if _TARGET_INTEL
        ret = WVArray( idx, base );
#else
        ret = 0;
#endif
    }
#ifdef DEVBUILD
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}

dbg_array _CGAPI DBBegArray(  dbg_type base, cg_type tipe, bool is_col_major )
/************************************************************************************/
{
    dbg_array   ar;
//  type_def   *tipe_addr;

    /* unused parameters */ (void)tipe;

#ifdef DEVBUILD
    EchoAPI( "DBBegArray( %i,%t,%i)", base, tipe, is_col_major );
#endif
    ar = CGAlloc( sizeof( *ar ) );
    ar->num = 0;
//  tipe_addr = TypeAddress( tipe );
//  ar->size = tipe_addr->length;
    ar->list = NULL;
    ar->base = base;
    ar->is_col_major = is_col_major;
    ar->is_variable = false;
#ifdef DEVBUILD
    EchoAPI( " -> %i\n", ar );
#endif
    return( ar );
}

static  void    AddDim( dbg_array ar, dim_any *dim )
/**************************************************/
{
    dim_any *curr;
    dim_any **owner;

    owner = &ar->list;
    for(;;) {
        curr = *owner;
        if( curr == NULL )
            break;
        owner = &curr->entry.next;
    }
    dim->entry.next = NULL;
    *owner = dim;
    ar->num++;
}

void _CGAPI DBDimCon( dbg_array ar, dbg_type idx, int_32 lo, int_32 hi )
/**********************************************************************/
{
    dim_con *dim;

#ifdef DEVBUILD
    EchoAPI( "DBDimCon( %i, %i, %i, %i )\n", ar, idx, lo, hi );
#endif
    dim = CGAlloc( sizeof( *dim ) );
    dim->entry.kind = DIM_CON;
    dim->lo = lo;
    dim->hi = hi;
    dim->idx = idx;
    AddDim( ar, (dim_any *)dim );
}

void _CGAPI DBDimVar( dbg_array ar, back_handle dims, int off,
                        cg_type lo_bound_tipe,
                        cg_type num_elts_tipe )
/*************************************************/
{
    dim_var *dim;

#ifdef DEVBUILD
    EchoAPI( "DBDimVar(%i, %B, %i, %t, %t)\n", ar, dims, off, lo_bound_tipe, num_elts_tipe);
#endif
    dim = CGAlloc( sizeof( *dim ) );
    dim->entry.kind = DIM_VAR;
    dim->dims = dims;
    dim->off = off;
    dim->lo_bound_tipe = lo_bound_tipe;
    dim->num_elts_tipe = num_elts_tipe;
    AddDim( ar, (dim_any *)dim );
    ar->is_variable = true;
}

dbg_type _CGAPI DBEndArray( dbg_array ar )
/************************************************/
{
    dbg_type ret;

#ifdef DEVBUILD
    EchoAPI( "DBEndArray( %i )", ar );
#endif
    if( _IsModel( CGSW_GEN_DBG_DF ) ) {
        ret =  DFEndArray( ar );
    } else if( _IsModel( CGSW_GEN_DBG_CV ) ) {
        ret =  CVEndArray( ar );
    } else {
#if _TARGET_INTEL
        ret = WVEndArray( ar );
#else
        ret = 0;
#endif
    }
    CGFree( ar );
#ifdef DEVBUILD
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}

dbg_type _CGAPI DBIntArray( uint_32 hi, dbg_type base )
/*****************************************************/
{
    dbg_type ret;

#ifdef DEVBUILD
    EchoAPI(  "DBIntArray( %i, %i )", hi, base );
#endif
    if( _IsModel( CGSW_GEN_DBG_DF ) ) {
        ret = DFIntArray( hi, base );
    } else if( _IsModel( CGSW_GEN_DBG_CV ) ) {
        ret = CVIntArray( hi, base );
    } else {
#if _TARGET_INTEL
        ret = WVIntArray( hi, base );
#else
        ret = 0;
#endif
    }
#ifdef DEVBUILD
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}

dbg_type _CGAPI DBIntArrayCG( cg_type tipe, uint_32 hi, dbg_type base )
/*********************************************************************/
{
    dbg_type          ret;
    type_def          *tipe_addr;

#ifdef DEVBUILD
    EchoAPI( "DBIntArrayCG( %t, %i, %i )", tipe, hi, base );
#endif
    tipe_addr = TypeAddress( tipe );
    if( _IsModel( CGSW_GEN_DBG_DF ) ) {
        ret = DFIntArray( hi, base );
    } else if( _IsModel( CGSW_GEN_DBG_CV ) ) {
        ret = CVArraySize( tipe_addr->length, hi, base );
    } else {
#if _TARGET_INTEL
        ret = WVIntArray( hi, base );
#else
        ret = 0;
#endif
    }
#ifdef DEVBUILD
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}

dbg_type _CGAPI DBSubRange( int_32 lo, int_32 hi, dbg_type base )
/***************************************************************/
{
    dbg_type ret;

#ifdef DEVBUILD
    EchoAPI( "DBSubRange( %i, %i, %i )", lo, hi, base );
#endif
    if( _IsModel( CGSW_GEN_DBG_DF ) ) {
        ret = DFSubRange( lo, hi, base );
    } else if( _IsModel( CGSW_GEN_DBG_CV ) ) {
        ret = CVSubRange( lo, hi, base );
    } else {
#if _TARGET_INTEL
        ret = WVSubRange( lo, hi, base );
#else
        ret = 0;
#endif
    }
#ifdef DEVBUILD
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}


dbg_type _CGAPI DBDereference( cg_type ptr_type, dbg_type base )
/**********************************************************************/
{
    dbg_type ret;


#ifdef DEVBUILD
    EchoAPI( "DBDereference( %t, %i)", ptr_type, base );
#endif
    if( _IsModel( CGSW_GEN_DBG_DF ) ) {
        ret = DFDereference( ptr_type, base );
    } else if( _IsModel( CGSW_GEN_DBG_CV ) ) {
        ret = CVDereference( ptr_type, base );
    } else {
#if _TARGET_INTEL
        ret = WVDereference( ptr_type, base );
#else
        ret = 0;
#endif
    }

#ifdef DEVBUILD
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}

dbg_type _CGAPI DBPtr( cg_type ptr_type, dbg_type base )
/**************************************************************/
{
    dbg_type ret;

#ifdef DEVBUILD
    EchoAPI( "DBPtr( %t, %i )", ptr_type, base );
#endif
    if( _IsModel( CGSW_GEN_DBG_DF ) ) {
        ret = DFPtr( ptr_type, base );
    } else if( _IsModel( CGSW_GEN_DBG_CV ) ) {
        ret = CVPtr( ptr_type, base );
    } else {
#if _TARGET_INTEL
        ret = WVPtr( ptr_type, base );
#else
        ret = 0;
#endif
    }
#ifdef DEVBUILD
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}

dbg_type _CGAPI DBBasedPtr( cg_type ptr_type, dbg_type base,
                                        dbg_loc loc_segment )
/****************************************************************/
{
    dbg_type ret;

#ifdef DEVBUILD
    EchoAPI("DBBasedPtr( %t, %i, %i )", ptr_type, base, loc_segment );
#endif
    if( _IsModel( CGSW_GEN_DBG_DF ) ) {
        ret = DFBasedPtr( ptr_type, base, loc_segment );
    } else if( _IsModel( CGSW_GEN_DBG_CV ) ) {
        ret = CVBasedPtr( ptr_type, base, loc_segment );
    } else {
#if _TARGET_INTEL
        ret = WVBasedPtr( ptr_type, base, loc_segment );
#else
        ret = 0;
#endif
    }
#ifdef DEVBUILD
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}


bool _CGAPI DBNested( bool nested )
/*********************************/
{
    bool ret;

#ifdef DEVBUILD
    EchoAPI( "DBNested(%i)", nested );
#endif
    ret = Nested;
    Nested = nested;
#ifdef DEVBUILD
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}

dbg_struct _CGAPI DBBegNameStruct( cchar_ptr nm, cg_type tipe, bool is_struct )
/*****************************************************************************/
{
    uint        n_len;
    dbg_struct  st;

#ifdef DEVBUILD
    EchoAPI( "DBBegNameStruct( %c, %t, %i )", nm, tipe, is_struct );
#endif
    n_len = Length( nm );
    st = CGAlloc( sizeof( struct_list ) + n_len );
    strcpy( st->name, nm );
    st->num = 0;
    st->list = NULL;
    st->list_tail = &st->list;
    st->size = TypeAddress( tipe )->length;
    st->is_struct = is_struct;   /* v.s. union */
    st->is_class = false;
    st->is_nested = Nested;
    st->is_cnested = false;
    if( _IsModel( CGSW_GEN_DBG_DF ) ) {
        DFBegStruct( st );
    } else if( _IsModel( CGSW_GEN_DBG_CV ) ) {
//       CVBegStruct( st );
        st->me = DBG_NIL_TYPE;
    } else {
        st->me = DBG_NIL_TYPE;
    }
    st->vtbl_off = 0;
    st->vtbl_type = DBG_NIL_TYPE;
    st->ptr_type = 0;
    st->vtbl_esize = 0;
    st->vf = NULL;
#ifdef DEVBUILD
    EchoAPI( " -> %i\n", st );
#endif
    return( st );
}

dbg_struct _CGAPI DBBegStruct( cg_type tipe, bool is_struct )
/***********************************************************/
{
    dbg_struct  st;

#ifdef DEVBUILD
    EchoAPI( "DBBegStruct( %t, %i )", tipe, is_struct );
#endif
    st = DBBegNameStruct( "", tipe, is_struct );
#ifdef DEVBUILD
    EchoAPI( " -> %i\n", st );
#endif
    return( st );
}

static  field_member     *CreateMember( const char *nm, byte strt,
                              byte len, dbg_type base, uint attr ) {
/******************************************************************/
    uint          n_len;
    field_member *field;

    n_len = Length( nm );
    field = CGAlloc( sizeof( field_member ) + n_len );
    strcpy( field->name, nm );
    field->attr = attr;
    field->len = n_len;
    field->base = base;
    field->b_strt = strt;
    field->b_len  = len;
    return( field );
}

static  void    AddField( dbg_struct st, field_any *field )
/*********************************************************/
{
    field->entry.next = NULL;
    *st->list_tail = field;
    st->list_tail = &field->entry.next;
    st->num++;
}


void _CGAPI DBAddBitField( dbg_struct st, uint_32 off, byte strt,
                            byte len, cchar_ptr nm, dbg_type base )
/*****************************************************************/
{
    field_member *field;

#ifdef DEVBUILD
    EchoAPI( "DBAddBitField(%i,%i,%i,%i,%c,%i)\n", st, off, strt, len, nm, base );
#endif
    field = CreateMember( nm, strt, len, base, 0 );
    field->entry.field_type = FIELD_OFFSET;
    field->u.off= off;
    AddField( st, (field_any *)field );
}


void _CGAPI DBAddField( dbg_struct st, uint_32 off,
                        cchar_ptr nm, dbg_type  base )
/****************************************************/
{
#ifdef DEVBUILD
    EchoAPI( "DBAddField( %i, %i,%c,%i)\n", st, off, nm, base );
#endif
    DBAddBitField( st, off, 0, 0, nm, base );
}


void _CGAPI DBAddLocField( dbg_struct st, dbg_loc loc, uint attr,
                         byte strt, byte len, cchar_ptr nm, dbg_type base )
/*************************************************************************/
{
    field_member *field;
    offset      off;

#ifdef DEVBUILD
    EchoAPI( "DBAddLocField( %i,%i,%i,%i,%i,%c,%i)\n", st, loc, attr, strt, len, nm, base );
#endif
    field = CreateMember( nm, strt, len, base, attr );
    off = LocSimpField( loc );
    if( off != NO_OFFSET && (attr==FIELD_ATTR_NONE || attr==FIELD_ATTR_PUBLIC) ) {
        field->entry.field_type = FIELD_OFFSET;
        field->u.off= off;
    } else {
        field->entry.field_type = FIELD_LOC;
        field->u.loc = LocDupl( loc );
    }
    AddField( st, (field_any *)field );
}

void _CGAPI DBAddStField( dbg_struct st, dbg_loc loc, cchar_ptr nm, uint_32 attr, dbg_type base )
/***********************************************************************************************/
{
    uint          n_len;
    field_stfield *field;

#ifdef DEVBUILD
    EchoAPI( "DBAddStField(%i,%i,%c,%i,%i)\n", st, loc, nm, attr, base );
#endif
    n_len = Length( nm );
    field = CGAlloc( sizeof( field_stfield ) + n_len );
    strcpy( field->name, nm );
    field->entry.field_type = FIELD_STFIELD;
    field->loc = LocDupl( loc );
    field->attr = attr;
    field->base = base;
    AddField( st, (field_any *)field );
}

void _CGAPI DBAddMethod( dbg_struct st, dbg_loc loc, uint attr,
                                 uint kind, cchar_ptr nm, dbg_type base )
/***********************************************************************/
{
    uint          n_len;
    field_method *field;

#ifdef DEVBUILD
    EchoAPI( "DBAddMethod( %i,%i,%i,%i,%c,%i)\n", st, loc, attr, kind, nm, base );
#endif
    n_len = Length( nm );
    field = CGAlloc( sizeof( field_method ) + n_len );
    strcpy( field->name, nm );
    field->entry.field_type = FIELD_METHOD;
    field->u.loc = LocDupl( loc );
    field->attr = attr;
    field->kind = kind;
    field->len = n_len;
    field->base = base;
    AddField( st, (field_any *)field );
}

void _CGAPI DBAddNestedType( dbg_struct st, cchar_ptr nm, dbg_type base )
/*******************************************************************************/
{
    uint          n_len;
    field_nested *field;

#ifdef DEVBUILD
    EchoAPI( "DBAddNestedType( %i,%c,%i)\n", st, nm, base );
#endif
    n_len = Length( nm );
    field = CGAlloc( sizeof( field_nested ) + n_len );
    strcpy( field->name, nm );
    field->entry.field_type = FIELD_NESTED;
    field->base = base;
    AddField( st, (field_any *)field );
    st->is_cnested = true;
}


void _CGAPI DBAddInheritance( dbg_struct st, dbg_type inherit,
                                  uint attr, uint kind,  dbg_loc loc )
/**********************************************************************/
{
    field_bclass *field;

#ifdef DEVBUILD
    EchoAPI( "DBAddInheritance(%i,%i,%i,%i,%i)\n", st, inherit, attr, kind, loc );
#endif
    field = CGAlloc( sizeof( field_bclass ) );
    field->entry.field_type = FIELD_INHERIT;
    field->attr = attr;
    field->base = inherit;
    field->attr = attr;
    field->kind = kind;
    field->u.adjustor = LocDupl( loc );
    AddField( st, (field_any *)field );
}

void _CGAPI DBAddBaseInfo( dbg_struct st, uint_32 vb_off, int esize,
                                    dbg_type vtbl, cg_type ptr_type )
/*******************************************************************/
{
#ifdef DEVBUILD
    EchoAPI( "DBAddBaseInfo( %i,%i,%i,%i,%t)\n", st, vb_off, esize, vtbl, ptr_type );
#endif
    st->vtbl_off    = vb_off;
    st->vtbl_type  = vtbl;
    st->ptr_type  =  ptr_type;
    st->vtbl_esize = esize;
}


void _CGAPI DBAddVFuncInfo( dbg_struct st, uint_32 vfptr_off,
                                int size, cg_type vft_cgtype )
/************************************************************/
{
    field_vfunc  *field;

#ifdef DEVBUILD
    EchoAPI( "DBAddVFuncInfo( %i,%i,%i,%t )\n", st, vfptr_off, size, vft_cgtype );
#endif
    field = CGAlloc( sizeof( field_vfunc ) );
    field->entry.field_type = FIELD_VFUNC;
    field->vfptr_off = vfptr_off;
    field->vft_cgtype = vft_cgtype;
    field->vft_size = size;
    st->vf = field;
    AddField( st, (field_any *)field );
}

dbg_type _CGAPI DBEndStruct( dbg_struct st )
/**************************************************/
{
    dbg_type ret;

#ifdef DEVBUILD
    EchoAPI( "DBEndStruct(%i)", st );
#endif
    if( _IsModel( CGSW_GEN_DBG_DF ) ) {
        ret =  DFEndStruct( st );
    } else if( _IsModel( CGSW_GEN_DBG_CV ) ) {
        ret =  CVEndStruct( st );
    } else {
#if _TARGET_INTEL
        ret = WVEndStruct( st );
#else
        ret = 0;
#endif
    }
    CGFree( st );
#ifdef DEVBUILD
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}

dbg_type _CGAPI DBStructForward( dbg_struct st )
/******************************************************/
{
#ifdef DEVBUILD
    EchoAPI( "DBStructForward( %i )", st );
    EchoAPI( " -> %i\n", st->me );
#endif
    return( st->me );
}

dbg_enum _CGAPI DBBegEnum( cg_type tipe )
/***********************************************/
{
    dbg_enum    en;

#ifdef DEVBUILD
    EchoAPI( "DBBegEnum( %t )", tipe );
#endif
    en = CGAlloc( sizeof( enum_list ) );
    en->num = 0;
    en->list = NULL;
    en->tipe = tipe;
    en->is_nested = Nested;
    en->is_c  = true;
#ifdef DEVBUILD
    EchoAPI( " -> %i\n", en );
#endif
    return( en );
}

void _CGAPI DBAddConst( dbg_enum en, cchar_ptr nm, int_32 val )
/*************************************************************/
{
    const_entry *cons;
    uint        len;

#ifdef DEVBUILD
    EchoAPI( "DBAddConst( %i,%c,%i)\n", en, nm, val );
#endif
    len = Length( nm );
    cons = CGAlloc( sizeof( const_entry ) + len );
    strcpy( cons->name, nm );
    cons->len = len;
    I32ToI64( val, &cons->val );
    cons->next = en->list;
    en->list = cons;
    en->num++;
}

void _CGAPI DBAddConst64( dbg_enum en, cchar_ptr nm, signed_64  val )
/*******************************************************************/
{
    const_entry *cons;
    uint        len;

#ifdef DEVBUILD
    EchoAPI( "DBAddConst( %i,%c,%x %x )\n", en, nm, val.u._32[0],val.u._32[1] );
#endif
    len = Length( nm );
    cons = CGAlloc( sizeof( const_entry ) + len );
    strcpy( cons->name, nm );
    cons->len = len;
    cons->val = val;
    cons->next = en->list;
    en->list = cons;
    en->num++;
}

dbg_type _CGAPI DBEndEnum( dbg_enum en )
/**********************************************/
{
    dbg_type ret;

#ifdef DEVBUILD
    EchoAPI( "DBEndEnum(%i)", en );
#endif
    if( _IsModel( CGSW_GEN_DBG_DF ) ) {
        ret = DFEndEnum( en );
    } else if( _IsModel( CGSW_GEN_DBG_CV ) ) {
        ret = CVEndEnum( en );
    } else {
#if _TARGET_INTEL
        ret = WVEndEnum( en );
#else
        ret = 0;
#endif
    }
    CGFree( en );
#ifdef DEVBUILD
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}


dbg_proc _CGAPI DBBegProc( cg_type call_type, dbg_type  ret )
/*******************************************************************/
{
    dbg_proc    pr;

#ifdef DEVBUILD
    EchoAPI( "DBBegProc( %t,%i)", call_type, ret );
#endif
    pr = CGAlloc( sizeof( proc_list ) );
    pr->num = 0;
    pr->list = NULL;
    pr->call = TypeAddress( call_type )->refno;
    pr->ret = ret;
    pr->cls = DBG_NIL_TYPE;
    pr->this = DBG_NIL_TYPE;
#ifdef DEVBUILD
    EchoAPI( " -> %i\n", pr );
#endif
    return(pr);
}

void  _CGAPI DBAddMethParms( dbg_proc pr, dbg_type cls, dbg_type this )
/*****************************************************************************/
{
#ifdef DEVBUILD
    EchoAPI( "DBAddMethParms( %i, %i, %i)\n", pr, cls, this );
#endif
    pr->cls = cls;
    pr->this = this;
}

void _CGAPI DBAddParm( dbg_proc pr, dbg_type tipe )
/*********************************************************/
{
    parm_entry  *parm;
    parm_entry  **owner;

#ifdef DEVBUILD
    EchoAPI( "DBAddParm( %i,%i )\n", pr, tipe );
#endif
    parm = CGAlloc( sizeof( parm_entry ) );
    pr->num++;
    for( owner = &pr->list; *owner != NULL; ) {
        owner = &(*owner)->next;
    }
    *owner = parm;
    parm->tipe = tipe;
    parm->next = NULL;
}


dbg_type _CGAPI DBEndProc( dbg_proc pr )
/**********************************************/
{
    dbg_type ret;

#ifdef DEVBUILD
    EchoAPI( "DBEndProc( %i )", pr );
#endif
    if( _IsModel( CGSW_GEN_DBG_DF ) ) {
        ret =  DFEndProc( pr );
    } else if( _IsModel( CGSW_GEN_DBG_CV ) ) {
        ret =  CVEndProc( pr );
    } else {
#if _TARGET_INTEL
        ret = WVEndProc( pr );
#else
        ret = 0;
#endif
    }
    CGFree( pr );
#ifdef DEVBUILD
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}
