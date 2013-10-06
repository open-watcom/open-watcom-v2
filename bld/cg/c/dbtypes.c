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


#include <string.h>
#include "cgstd.h"
#include "coderep.h"
#include "cgdefs.h"
#include "cgmem.h"
#include "symdbg.h"
#include "model.h"
#include "typedef.h"
#include "types.h"
#include "ocentry.h"
#include "objrep.h"
#include "zoiks.h"
#include "dbgstrct.h"
#ifndef NDEBUG
#include "echoapi.h"
#endif
#include "i64.h"
#include "utils.h"

extern  dbg_loc         LocDupl( dbg_loc );
extern  offset          LocSimpField( dbg_loc );

/* WV interface */
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
extern  dbg_type        WVFtnType( char *name, dbg_ftn_type tipe );
extern  dbg_type        WVScalar( char *name, cg_type tipe );
extern  dbg_type        WVScope( char *name );
extern  void            WVDumpName( name_entry *name, dbg_type tipe );
extern  void            WVBackRefType( name_entry *name, dbg_type tipe );
extern  dbg_type        WVCharBlock( unsigned_32 len );
extern  dbg_type        WVIndCharBlock( back_handle len, cg_type len_type,
                                        int off );
extern  dbg_type        WVLocCharBlock( dbg_loc loc, cg_type len_type );
extern  dbg_type        WVFtnArray( back_handle dims, cg_type lo_bound_tipe,
                                    cg_type num_elts_tipe, int off,
                                    dbg_type base );
extern  dbg_type        WVArray( dbg_type idx, dbg_type base );
extern  dbg_type        WVIntArray( unsigned_32 hi, dbg_type base );
extern  dbg_type        WVEndArray( array_list *ar );
extern  dbg_type        WVSubRange( signed_32 lo, signed_32 hi,
                                    dbg_type base );
extern  dbg_type        WVDereference( cg_type ptr_type, dbg_type base );
extern  dbg_type        WVPtr( cg_type ptr_type, dbg_type base );
extern  dbg_type        WVBasedPtr( cg_type ptr_type, dbg_type base,
                                        dbg_loc loc_segment );
extern  dbg_type        WVEndStruct( struct_list  *st );
extern  dbg_type        WVEndEnum( enum_list *en );
extern  dbg_type        WVEndProc( proc_list  *pr );
#endif
/* DF interface */
extern  dbg_type        DFFtnType( char *name, dbg_ftn_type tipe );
extern  dbg_type        DFScalar( char *name, cg_type tipe );
extern  dbg_type        DFScope( char *name );
extern  void            DFDumpName( name_entry *name, dbg_type tipe );
extern  void            DFBackRefType( name_entry *name, dbg_type tipe );
extern  dbg_type        DFCharBlock( unsigned_32 len );
extern  dbg_type        DFCharBlockNamed( char * name, unsigned_32 len );
extern  dbg_type        DFIndCharBlock( back_handle len, cg_type len_type,
                                        int off );
extern  dbg_type        DFLocCharBlock( dbg_loc loc, cg_type len_type );
extern  dbg_type        DFFtnArray( back_handle dims, cg_type lo_bound_tipe,
                                    cg_type num_elts_tipe, int off,
                                    dbg_type base );
extern  dbg_type        DFArray( dbg_type idx, dbg_type base );
extern  dbg_type        DFIntArray( unsigned_32 hi, dbg_type base );
extern  dbg_type        DFEndArray( array_list *ar );
extern  dbg_type        DFSubRange( signed_32 lo, signed_32 hi,
                                    dbg_type base );
extern  dbg_type        DFDereference( cg_type ptr_type, dbg_type base );
extern  dbg_type        DFPtr( cg_type ptr_type, dbg_type base );
extern  dbg_type        DFBasedPtr( cg_type ptr_type, dbg_type base,
                                        dbg_loc loc_segment );
extern  void             DFBegStruct( struct_list  *st );
extern  dbg_type        DFEndStruct( struct_list  *st );
extern  dbg_type        DFEndEnum( enum_list *en );
extern  dbg_type        DFEndProc( proc_list  *pr );

/* DF interface */
extern  dbg_type        CVFtnType( char *name, dbg_ftn_type tipe );
extern  dbg_type        CVScalar( char *name, cg_type tipe );
extern  dbg_type        CVScope( char *name );
extern  void            CVDumpName( name_entry *name, dbg_type tipe );
extern  void            CVBackRefType( name_entry *name, dbg_type tipe );
extern  dbg_type        CVCharBlock( unsigned_32 len );
extern  dbg_type        CVIndCharBlock( back_handle len, cg_type len_type,
                                        int off );
extern  dbg_type        CVLocCharBlock( dbg_loc loc, cg_type len_type );
extern  dbg_type        CVFtnArray( back_handle dims, cg_type lo_bound_tipe,
                                    cg_type num_elts_tipe, int off,
                                    dbg_type base );
extern  dbg_type        CVArray( dbg_type idx, dbg_type base );
extern  dbg_type        CVIntArray( unsigned_32 hi, dbg_type base );
extern  dbg_type        CVEndArray( array_list *ar );
extern  dbg_type        CVArraySize( offset size, unsigned_32 hi, dbg_type base );
extern  dbg_type        CVSubRange( signed_32 lo, signed_32 hi,
                                    dbg_type base );
extern  dbg_type        CVDereference( cg_type ptr_type, dbg_type base );
extern  dbg_type        CVPtr( cg_type ptr_type, dbg_type base );
extern  dbg_type        CVBasedPtr( cg_type ptr_type, dbg_type base,
                                        dbg_loc loc_segment );
extern  dbg_type        CVEndStruct( struct_list  *st );
extern  dbg_type        CVEndEnum( enum_list *en );
extern  dbg_type        CVEndProc( proc_list  *pr );
#define MAX_TYPE_SIZE  (1024 * 16)

static bool Nested;     /* set when types are nested by others */

/*
  need to fix dbg_type so it's a handle that can be used
  by any format WV, DW and CV.
  figure how fundamental types are done
*/
extern  dbg_type _CGAPI DBFtnType( char *name, dbg_ftn_type tipe )
/****************************************************************/
{
    dbg_type ret;

#ifndef NDEBUG
    EchoAPI( "DBFtnType( %c, %i )", name, tipe );
#endif
    if( _IsModel( DBG_DF ) ) {
        ret = DFFtnType( name, tipe );
    } else if( _IsModel( DBG_CV ) ) {
        ret = CVFtnType( name, tipe );
    } else {
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
        ret = WVFtnType( name, tipe );
#else
        ret = 0;
#endif
    }
#ifndef NDEBUG
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}


extern  dbg_type _CGAPI DBScalar( char *name, cg_type tipe )
/**********************************************************/
{
    dbg_type ret;

#ifndef NDEBUG
    EchoAPI( "DBScalar( %c,%t )", name, tipe );
#endif
    if( _IsModel( DBG_DF ) ) {
        ret = DFScalar( name, tipe );
    } else if( _IsModel( DBG_CV ) ) {
        ret = CVScalar( name, tipe );
    } else {
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
        ret = WVScalar( name, tipe );
#else
        ret = 0;
#endif
    }
#ifndef NDEBUG
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}



extern  dbg_type _CGAPI DBScope( char *name )
/*******************************************/
{
    dbg_type ret;

#ifndef NDEBUG
    EchoAPI( "DBScope( %c )", name );
#endif
    if( _IsModel( DBG_DF ) ) {
        ret = DFScope( name );
    } else if( _IsModel( DBG_CV ) ) {
        ret = CVScope( name );
    } else {
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
        ret = WVScope( name );
#else
        ret = 0;
#endif
    }
#ifndef NDEBUG
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}



extern  name_entry * _CGAPI DBBegName( const char *nm, dbg_type scope ) {
/***********************************************************************/

    name_entry  *name;
    uint        len;

#ifndef NDEBUG
    EchoAPI( "DBBegName( %c, %i )", nm, scope );
#endif
    len = strlen( nm );
    name = CGAlloc( sizeof( name_entry ) + len );
    strcpy( name->name, nm );
    name->len = len;
    name->scope = scope;
    name->refno = DBG_NIL_TYPE;
#ifndef NDEBUG
    EchoAPI( " -> %i\n", name );
#endif
    return( name );
}


extern  dbg_type _CGAPI DBForward( name_entry *name ) {
/*****************************************************/

#ifndef NDEBUG
    EchoAPI( "DBForward( %i )", name );
#endif
    if( name->refno == DBG_NIL_TYPE ) {
        if( _IsModel( DBG_DF ) ) {
            /* do nothing */
        } else if( _IsModel( DBG_CV ) ) {
            CVDumpName( name, DBG_FWD_TYPE );
        } else {
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
            WVDumpName( name, DBG_FWD_TYPE );
#endif
        }
    }
#ifndef NDEBUG
    EchoAPI( " -> %i\n", name->refno );
#endif
    return( name->refno );
}


extern  dbg_type _CGAPI DBEndName( name_entry *name, dbg_type tipe ) {
/********************************************************************/

    dbg_type    retv;

#ifndef NDEBUG
    EchoAPI( "DBEndName( %i, %i )", name, tipe );
#endif
    if( name->refno == DBG_NIL_TYPE ) {
        if( _IsModel( DBG_DF ) ) {
           DFDumpName( name, tipe );
        } else if( _IsModel( DBG_CV ) ) {
           CVDumpName( name, tipe );
        } else {
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
           WVDumpName( name, tipe );
#endif
        }
    } else if( _IsModel( DBG_TYPES ) ) {
        if( _IsModel( DBG_DF ) ) {
            DFBackRefType( name, tipe );
        } else if( _IsModel( DBG_CV ) ) {
            CVBackRefType( name, tipe );
        } else {
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
            WVBackRefType( name, tipe );
#endif
        }
    }
    retv = name->refno;
    CGFree( name );
#ifndef NDEBUG
    EchoAPI( " -> %i\n", retv );
#endif
    return( retv );
}


extern  dbg_type _CGAPI DBCharBlock( unsigned_32 len ) {
/******************************************************/

    dbg_type ret;

#ifndef NDEBUG
    EchoAPI( "DBCharBlock( %i )", len );
#endif
    if( _IsModel( DBG_DF ) ) {
        ret = DFCharBlock( len );
    } else if( _IsModel( DBG_CV ) ) {
        ret = CVCharBlock( len );
    } else {
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
        ret = WVCharBlock( len );
#else
        ret = 0;
#endif
    }
    return( ret );
}

extern  dbg_type _CGAPI DBCharBlockNamed( char * name, unsigned_32 len ) {
/******************************************************/

    dbg_type ret;

#ifndef NDEBUG
    EchoAPI( "DBCharBlock( %i )", len );
#endif
    if( _IsModel( DBG_DF ) ) {
        ret = DFCharBlockNamed( name, len );
    } else if( _IsModel( DBG_CV ) ) {
        ret = CVCharBlock( len );
    } else {
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
        ret = WVCharBlock( len );
#else
        ret = 0;
#endif
    }
    return( ret );
}

extern  dbg_type _CGAPI DBIndCharBlock( back_handle len, cg_type len_type,
                                        int off ) {
/************************************************************************/

    dbg_type ret;

#ifndef NDEBUG
    EchoAPI( "DBIndCharBlock( %i, %t, %i )", len,len_type, off );
#endif
    if( _IsModel( DBG_DF ) ) {
        ret = DFIndCharBlock( len, len_type, off );
    } else if( _IsModel( DBG_CV ) ) {
        ret = CVIndCharBlock( len, len_type, off );
    } else {
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
        ret = WVIndCharBlock( len, len_type, off );
#else
        ret = 0;
#endif
    }
#ifndef NDEBUG
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}

extern  dbg_type _CGAPI DBLocCharBlock( dbg_loc loc, cg_type len_type ) {
/***********************************************************************/

    dbg_type ret;

#ifndef NDEBUG
    EchoAPI( "DBLocCharBlock( %i, %t )", loc, len_type );
#endif

    if( _IsModel( DBG_DF ) ) {
        ret = DFLocCharBlock( loc, len_type );
    } else if( _IsModel( DBG_CV ) ) {
        ret = CVLocCharBlock( loc, len_type );
    } else {
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
        ret = WVLocCharBlock( loc, len_type );
#else
        ret = 0;
#endif
    }
#ifndef NDEBUG
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}


extern  dbg_type _CGAPI DBFtnArray( back_handle dims, cg_type lo_bound_tipe,
                                    cg_type num_elts_tipe, int off,
                                    dbg_type base ) {
/***************************************************************************/

    dbg_type ret;

#ifndef NDEBUG
    EchoAPI( "DBFtnArray( %B,%t,%t,%i,%i)", dims, lo_bound_tipe,
             num_elts_tipe, off, base );
#endif
    if( _IsModel( DBG_DF ) ) {
        ret = DFFtnArray( dims, lo_bound_tipe, num_elts_tipe, off, base );
    } else if( _IsModel( DBG_CV ) ) {
        ret = CVFtnArray( dims, lo_bound_tipe, num_elts_tipe, off, base );
    } else {
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
        ret = WVFtnArray( dims, lo_bound_tipe, num_elts_tipe, off, base );
#else
        ret = 0;
#endif
    }
#ifndef NDEBUG
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}


extern  dbg_type _CGAPI DBArray( dbg_type idx, dbg_type base ) {
/**************************************************************/

    dbg_type ret;

#ifndef NDEBUG
    EchoAPI( "DBArray( %i, %i)", idx, base );
#endif
    if( _IsModel( DBG_DF ) ) {
        ret = DFArray( idx, base );
    } else if( _IsModel( DBG_CV ) ) {
        ret = CVArray( idx, base );
    } else {
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
        ret = WVArray( idx, base );
#else
        ret = 0;
#endif
    }
#ifndef NDEBUG
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}

extern  array_list * _CGAPI DBBegArray(  dbg_type base, cg_type tipe, bool is_col_major ){
/****************************************************************************************/
    array_list *ar;
//  type_def   *tipe_addr;

#ifndef NDEBUG
    EchoAPI( "DBBegArray( %i,%t,%i)", base, tipe, is_col_major );
#endif
     tipe = tipe;
    ar = CGAlloc( sizeof( *ar ) );
    ar->num = 0;
//  tipe_addr = TypeAddress( tipe );
//  ar->size = tipe_addr->length;
    ar->list = NULL;
    ar->base = base;
    ar->is_col_major = is_col_major;
    ar->is_variable = FALSE;
#ifndef NDEBUG
    EchoAPI( " -> %i\n", ar );
#endif
    return( ar );
}

static  void    AddDim( array_list *ar, dim_any *dim ){
/******************************************************/

    dim_any *curr;
    dim_any **owner;

    owner = &ar->list;
    for(;;) {
        curr = *owner;
        if( curr == NULL ) break;
        owner = &curr->entry.next;
    }
    dim->entry.next = NULL;
    *owner = dim;
    ar->num++;
}

extern  void _CGAPI DBDimCon( array_list *ar, dbg_type idx, signed_32 lo, signed_32 hi ){
/***************************************************/
    dim_con *dim;

#ifndef NDEBUG
    EchoAPI( "DBDimCon( %i, %i, %i, %i )\n", ar, idx, lo, hi );
#endif
    dim = CGAlloc( sizeof( *dim ) );
    dim->entry.kind = DIM_CON;
    dim->lo = lo;
    dim->hi = hi;
    dim->idx = idx;
    AddDim( ar, (dim_any *) dim );
}

extern  void _CGAPI DBDimVar( array_list *ar,
                        back_handle dims, int off,
                        cg_type lo_bound_tipe,
                        cg_type num_elts_tipe ){
/*************************************************/
    dim_var *dim;

#ifndef NDEBUG
    EchoAPI( "DBDimVar(%i, %B, %i, %t, %t)\n", ar, dims, off, lo_bound_tipe,
             num_elts_tipe);
#endif
    dim = CGAlloc( sizeof( *dim ) );
    dim->entry.kind = DIM_VAR;
    dim->dims = dims;
    dim->off = off;
    dim->lo_bound_tipe = lo_bound_tipe;
    dim->num_elts_tipe = num_elts_tipe;
    AddDim( ar, (dim_any *) dim );
    ar->is_variable = TRUE;
}

extern  dbg_type _CGAPI DBEndArray( array_list  *ar ) {
/*****************************************************/

    dbg_type ret;

#ifndef NDEBUG
    EchoAPI( "DBEndArray( %i )", ar );
#endif
    if( _IsModel( DBG_DF ) ) {
        ret =  DFEndArray( ar );
    } else if( _IsModel( DBG_CV ) ) {
        ret =  CVEndArray( ar );
    } else {
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
        ret = WVEndArray( ar );
#else
        ret = 0;
#endif
    }
    CGFree( ar );
#ifndef NDEBUG
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}

extern  dbg_type _CGAPI DBIntArray( unsigned_32 hi, dbg_type base ) {
/*******************************************************************/

    dbg_type ret;

#ifndef NDEBUG
    EchoAPI(  "DBIntArray( %i, %i )", hi, base );
#endif
    if( _IsModel( DBG_DF ) ) {
        ret = DFIntArray( hi, base );
    } else if( _IsModel( DBG_CV ) ) {
        ret = CVIntArray( hi, base );
    } else {
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
        ret = WVIntArray( hi, base );
#else
        ret = 0;
#endif
    }
#ifndef NDEBUG
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}

extern  dbg_type _CGAPI DBIntArrayCG( cg_type tipe, unsigned_32 hi, dbg_type base ) {
/*******************************************************************/

    dbg_type          ret;
    type_def          *tipe_addr;

#ifndef NDEBUG
    EchoAPI( "DBIntArrayCG( %t, %i, %i )", tipe, hi, base );
#endif
    tipe_addr = TypeAddress( tipe );
    if( _IsModel( DBG_DF ) ) {
        ret = DFIntArray( hi, base );
    } else if( _IsModel( DBG_CV ) ) {
        ret = CVArraySize( tipe_addr->length, hi, base );
    } else {
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
        ret = WVIntArray( hi, base );
#else
        ret = 0;
#endif
    }
#ifndef NDEBUG
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}

extern  dbg_type _CGAPI DBSubRange( signed_32 lo, signed_32 hi,
                                    dbg_type base ) {
/***************************************************/

    dbg_type ret;

#ifndef NDEBUG
    EchoAPI( "DBSubRange( %i, %i, %i )", lo, hi, base );
#endif
    if( _IsModel( DBG_DF ) ) {
        ret = DFSubRange( lo, hi, base );
    } else if( _IsModel( DBG_CV ) ) {
        ret = CVSubRange( lo, hi, base );
    } else {
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
        ret = WVSubRange( lo, hi, base );
#else
        ret = 0;
#endif
    }
#ifndef NDEBUG
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}


extern  dbg_type _CGAPI DBDereference( cg_type ptr_type, dbg_type base ) {
/************************************************************************/

    dbg_type ret;


#ifndef NDEBUG
    EchoAPI( "DBDereference( %t, %i)", ptr_type, base );
#endif
    if( _IsModel( DBG_DF ) ) {
        ret = DFDereference( ptr_type, base );
    } else if( _IsModel( DBG_CV ) ) {
        ret = CVDereference( ptr_type, base );
    } else {
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
        ret = WVDereference( ptr_type, base );
#else
        ret = 0;
#endif
    }

#ifndef NDEBUG
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}

extern  dbg_type _CGAPI DBPtr( cg_type ptr_type, dbg_type base ) {
/****************************************************************/

    dbg_type ret;

#ifndef NDEBUG
    EchoAPI( "DBPtr( %t, %i )", ptr_type, base );
#endif
    if( _IsModel( DBG_DF ) ) {
        ret = DFPtr( ptr_type, base );
    } else if( _IsModel( DBG_CV ) ) {
        ret = CVPtr( ptr_type, base );
    } else {
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
        ret = WVPtr( ptr_type, base );
#else
        ret = 0;
#endif
    }
#ifndef NDEBUG
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}

extern  dbg_type _CGAPI DBBasedPtr( cg_type ptr_type, dbg_type base,
                                        dbg_loc loc_segment ) {
/****************************************************************/

    dbg_type ret;

#ifndef NDEBUG
    EchoAPI("DBBasedPtr( %t, %i, %i )", ptr_type, base, loc_segment );
#endif
    if( _IsModel( DBG_DF ) ) {
        ret = DFBasedPtr( ptr_type, base, loc_segment );
    } else if( _IsModel( DBG_CV ) ) {
        ret = CVBasedPtr( ptr_type, base, loc_segment );
    } else {
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
        ret = WVBasedPtr( ptr_type, base, loc_segment );
#else
        ret = 0;
#endif
    }
#ifndef NDEBUG
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}


bool _CGAPI DBNested( bool nested ){
/**********************************/
    bool ret;

#ifndef NDEBUG
    EchoAPI( "DBNested(%i)", nested );
#endif
    ret = Nested;
    Nested = nested;
#ifndef NDEBUG
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}

struct_list * _CGAPI DBBegNameStruct( const char *nm, cg_type tipe, bool is_struct ) {
/************************************************************************************/

    uint      n_len;
    struct_list *st;

#ifndef NDEBUG
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
    st->is_class = FALSE;
    st->is_nested = Nested;
    st->is_cnested = FALSE;
    if( _IsModel(DBG_DF) ){
        DFBegStruct( st );
    } else if( _IsModel( DBG_CV ) ) {
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
#ifndef NDEBUG
    EchoAPI( " -> %i\n", st );
#endif
    return( st );
}

struct_list * _CGAPI DBBegStruct( cg_type tipe, bool is_struct ) {
/****************************************************************/

    struct_list *st;

#ifndef NDEBUG
    EchoAPI( "DBBegStruct( %t, %i )", tipe, is_struct );
#endif
    st = DBBegNameStruct( "", tipe, is_struct );
#ifndef NDEBUG
    EchoAPI( " -> %i\n", st );
#endif
    return( st );
}

static  field_member     *CreateMember( char *nm, byte strt, byte len,
                                        dbg_type base, uint attr ) {
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

static  void    AddField( struct_list *st, field_any *field )
/***********************************************************/
{
    field->entry.next = NULL;
    *st->list_tail = field;
    st->list_tail = &field->entry.next;
    st->num++;
}


extern  void _CGAPI DBAddBitField( struct_list *st, unsigned_32 off, byte strt,
                               byte len, char *nm, dbg_type base ) {
/******************************************************************/

    field_member *field;

#ifndef NDEBUG
    EchoAPI( "DBAddBitField(%i,%i,%i,%i,%c,%i)\n", st, off, strt, len, nm, base );
#endif
    field = CreateMember( nm, strt, len, base, 0 );
    field->entry.field_type = FIELD_OFFSET;
    field->u.off= off;
    AddField( st, (field_any *) field );
}


extern  void _CGAPI DBAddField( struct_list *st, unsigned_32 off,
                            char *nm, dbg_type  base ) {
/******************************************************/

#ifndef NDEBUG
    EchoAPI( "DBAddField( %i, %i,%c,%i)\n", st, off, nm, base );
#endif
    DBAddBitField( st, off, 0, 0, nm, base );
}


extern  void _CGAPI DBAddLocField( struct_list *st, dbg_loc loc, uint attr,
                            byte strt, byte len, char *nm, dbg_type base ) {
/******************************************************************/

    field_member *field;
    offset      off;

#ifndef NDEBUG
    EchoAPI( "DBAddLocField( %i,%i,%i,%i,%i,%c,%i)\n", st, loc, attr, strt, len, nm, base );
#endif
    field = CreateMember( nm, strt, len, base, attr );
    off = LocSimpField( loc );
    if( off != (offset)-1 && (attr==FIELD_ATTR_NONE || attr==FIELD_ATTR_PUBLIC) ) {
        field->entry.field_type = FIELD_OFFSET;
        field->u.off= off;
    } else {
        field->entry.field_type = FIELD_LOC;
        field->u.loc = LocDupl( loc );
    }
    AddField( st, (field_any *) field );
}

extern  void _CGAPI DBAddStField( struct_list *st, dbg_loc loc, char *nm, uint attr,
                                             dbg_type base ) {
/*************************************************************/

    uint          n_len;
    field_stfield *field;

#ifndef NDEBUG
    EchoAPI( "DBAddStField(%i,%i,%c,%i,%i)\n", st, loc, nm, attr, base );
#endif
    n_len = Length( nm );
    field = CGAlloc( sizeof( field_stfield ) + n_len );
    strcpy( field->name, nm );
    field->entry.field_type = FIELD_STFIELD;
    field->loc = LocDupl( loc );
    field->attr = attr;
    field->base = base;
    AddField( st, (field_any *) field );
}

extern  void _CGAPI DBAddMethod( struct_list *st, dbg_loc loc,
                             uint  attr, uint     kind,
                             char *nm,   dbg_type base ) {
/******************************************************************/

    uint          n_len;
    field_method *field;

#ifndef NDEBUG
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
    AddField( st, (field_any *) field );
}

extern  void _CGAPI DBAddNestedType( struct_list *st, char *nm,
                                                  dbg_type base ) {
/******************************************************************/

    uint          n_len;
    field_nested *field;

#ifndef NDEBUG
    EchoAPI( "DBAddNestedType( %i,%c,%i)\n", st, nm, base );
#endif
    n_len = Length( nm );
    field = CGAlloc( sizeof( field_nested ) + n_len );
    strcpy( field->name, nm );
    field->entry.field_type = FIELD_NESTED;
    field->base = base;
    AddField( st, (field_any *) field );
    st->is_cnested = TRUE;
}


extern  void _CGAPI DBAddInheritance( struct_list *st, dbg_type inherit,
                                  uint attr, uint kind,  dbg_loc loc ) {
/*******************************************************************/
    field_bclass *field;

#ifndef NDEBUG
    EchoAPI( "DBAddInheritance(%i,%i,%i,%i,%i)\n", st, inherit, attr, kind, loc );
#endif
    field = CGAlloc( sizeof( field_bclass ) );
    field->entry.field_type = FIELD_INHERIT;
    field->attr = attr;
    field->base = inherit;
    field->attr = attr;
    field->kind = kind;
    field->u.adjustor = LocDupl( loc );
    AddField( st, (field_any *) field );
}

extern  void _CGAPI DBAddBaseInfo( struct_list  *st, offset vb_off,  offset esize,
                                              dbg_type vtbl, cg_type ptr_type ){
/*******************************************************************/
#ifndef NDEBUG
    EchoAPI( "DBAddBaseInfo( %i,%i,%i,%i,%t)\n", st, vb_off, esize, vtbl, ptr_type );
#endif
    st->vtbl_off    = vb_off;
    st->vtbl_type  = vtbl;
    st->ptr_type  =  ptr_type;
    st->vtbl_esize = esize;
}

extern  void _CGAPI DBAddVFuncInfo( struct_list  *st, offset vfptr_off,
                                                  int size,
                                                  cg_type vft_cgtype ){
/*******************************************************************/
    field_vfunc  *field;

#ifndef NDEBUG
    EchoAPI( "DBAddVFuncInfo( %i,%i,%i,%t )\n", st, vfptr_off, size, vft_cgtype );
#endif
    field = CGAlloc( sizeof( field_vfunc ) );
    field->entry.field_type = FIELD_VFUNC;
    field->vfptr_off = vfptr_off;
    field->vft_cgtype = vft_cgtype;
    field->vft_size = size;
    st->vf = field;
    AddField( st, (field_any *) field );
}

extern  dbg_type _CGAPI DBEndStruct( struct_list  *st ) {
/*******************************************************/

    dbg_type ret;

#ifndef NDEBUG
    EchoAPI( "DBEndStruct(%i)", st );
#endif
    if( _IsModel( DBG_DF ) ) {
        ret =  DFEndStruct( st );
    } else if( _IsModel( DBG_CV ) ) {
        ret =  CVEndStruct( st );
    } else {
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
        ret = WVEndStruct( st );
#else
        ret = 0;
#endif
    }
    CGFree( st );
#ifndef NDEBUG
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}

extern  dbg_type _CGAPI DBStructForward( struct_list  *st ) {
/********************************************************************/
#ifndef NDEBUG
    EchoAPI( "DBStructForward( %i )", st );
    EchoAPI( " -> %i\n", st->me );
#endif
    return( st->me );
}

extern  enum_list * _CGAPI DBBegEnum( cg_type  tipe ) {
/*****************************************************/

    enum_list   *en;

#ifndef NDEBUG
    EchoAPI( "DBBegEnum( %t )", tipe );
#endif
    en = CGAlloc( sizeof( enum_list ) );
    en->num = 0;
    en->list = NULL;
    en->tipe = tipe;
    en->is_nested = Nested;
    en->is_c  = TRUE;
#ifndef NDEBUG
    EchoAPI( " -> %i\n", en );
#endif
    return( en );
}

void _CGAPI DBAddConst( enum_list *en, const char *nm, signed_32 val ) {
/**********************************************************************/

    const_entry *cons;
    uint        len;

#ifndef NDEBUG
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

void _CGAPI DBAddConst64( enum_list *en, const char *nm, signed_64  val ) {
/*************************************************************************/

    const_entry *cons;
    uint        len;

#ifndef NDEBUG
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

extern  dbg_type _CGAPI DBEndEnum( enum_list *en ) {
/**************************************************/

    dbg_type ret;

#ifndef NDEBUG
    EchoAPI( "DBEndEnum(%i)", en );
#endif
    if( _IsModel( DBG_DF ) ) {
        ret = DFEndEnum( en );
    } else if( _IsModel( DBG_CV ) ) {
        ret = CVEndEnum( en );
    } else {
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
        ret = WVEndEnum( en );
#else
        ret = 0;
#endif
    }
    CGFree( en );
#ifndef NDEBUG
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}


extern  proc_list * _CGAPI DBBegProc(  cg_type call_type,  dbg_type  ret ) {
/**************************************************************************/

    proc_list   *pr;

#ifndef NDEBUG
    EchoAPI( "DBBegProc( %t,%i)", call_type, ret );
#endif
    pr = CGAlloc( sizeof( proc_list ) );
    pr->num = 0;
    pr->list = NULL;
    pr->call = TypeAddress( call_type )->refno;
    pr->ret = ret;
    pr->cls = DBG_NIL_TYPE;
    pr->this = DBG_NIL_TYPE;
#ifndef NDEBUG
    EchoAPI( " -> %i\n", pr );
#endif
    return(pr);
}

extern  void  _CGAPI DBAddMethParms(  proc_list *pr,  dbg_type  cls, dbg_type this   ) {
/************************************************************************/

#ifndef NDEBUG
    EchoAPI( "DBAddMethParms( %i, %i, %i)\n", pr, cls, this );
#endif
    pr->cls = cls;
    pr->this = this;
}

extern  void _CGAPI DBAddParm( proc_list *pr, dbg_type tipe ) {
/*********************************************************/

    parm_entry  *parm;
    parm_entry  **owner;

#ifndef NDEBUG
    EchoAPI( "DBAddParm( %i,%i )\n", pr, tipe );
#endif
    parm = CGAlloc( sizeof( parm_entry ) );
    pr->num++;
    owner = &pr->list;
    while( *owner != NULL ) {
        owner = &(*owner)->next;
    }
    *owner = parm;
    parm->tipe = tipe;
    parm->next = NULL;
}


extern  dbg_type _CGAPI DBEndProc( proc_list  *pr ) {
/***************************************************/

    dbg_type ret;

#ifndef NDEBUG
    EchoAPI( "DBEndProc( %i )", pr );
#endif
    if( _IsModel( DBG_DF ) ) {
        ret =  DFEndProc( pr );
    } else if( _IsModel( DBG_CV ) ) {
        ret =  CVEndProc( pr );
    } else {
#if _TARGET &( _TARG_IAPX86 | _TARG_80386 )
        ret = WVEndProc( pr );
#else
        ret = 0;
#endif
    }
    CGFree( pr );
#ifndef NDEBUG
    EchoAPI( " -> %i\n", ret );
#endif
    return( ret );
}
