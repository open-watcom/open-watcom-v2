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
*    The Original Code and all software distributed under the License areø
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


//
// FCALLOC :    Generate call to dynamically allocate/deallocate array
//

#include "ftnstd.h"
#include "rtconst.h"
#include "symbol.h"
#include "wf77cg.h"
#include "wf77auxd.h"
#include "wf77defs.h"
#include "falloc.h"
#include "fcjmptab.h"
#include "fcstruct.h"
#include "fcsubscr.h"
#include "fcgmain.h"
#include "fcrtns.h"
#include "fcstack.h"
#include "emitobj.h"
#include "cgswitch.h"
#include "cgprotos.h"


#if _CPU == 8086 || _CPU == 386
  #define FLAG_PARM_TYPE        TY_UINT_2
#else
  #define FLAG_PARM_TYPE        TY_UINT_4
#endif

static cg_name  getFlags( sym_id sym ) {
//======================================

    uint                tlen;
    cg_name             fl;
    cg_type             typ;

    if( sym->u.ns.flags & SY_SUBSCRIPTED ) {
        typ = ArrayPtrType( sym );
        tlen = BETypeLength( typ );
    } else {
        tlen = BETypeLength( TY_CHAR );
        typ = TY_CHAR;
    }
    fl = StructRef( CGFEName( sym, typ ), tlen );
    return( CGUnary( O_POINTS, fl, TY_UINT_2 ) );
}


void            FCAllocate( void ) {
//============================

    call_handle         handle;
    sym_id              arr;
    act_dim_list        *dim;
    uint                num;
    uint_16             alloc_flags;
    cg_name             expr_stat;
    cg_name             expr_loc;
    cg_name             fl;
    label_handle        label;

    SymPush( NULL );
    for( num = 0; (arr = GetPtr()) != NULL; ++num ) {
        // check if array is already allocated before filling in ADV
        label = BENewLabel();
        fl = getFlags( arr );
        fl = CGBinary( O_AND, fl, CGInteger( ALLOC_MEM, TY_UINT_2 ), TY_UINT_2 );
        CGControl( O_IF_TRUE, CGCompare( O_NE, fl, CGInteger( 0, TY_UINT_2 ), TY_UINT_2 ), label );
        FCodeSequence(); // fill in the ADV, SCB or RCB
        CGControl( O_LABEL, NULL, label );
        BEFiniLabel( label );
        SymPush( arr );
    }
    alloc_flags = GetU16();
    if( alloc_flags & ALLOC_NONE ) {
        expr_loc = CGInteger( 0, TY_POINTER );
        expr_stat = NULL;
    } else {
        FCodeSequence();
        if( alloc_flags & ALLOC_LOC ) {
            expr_loc = XPopValue( TY_INT_4 );
            expr_stat = NULL;
            if( alloc_flags & ALLOC_STAT ) {
                FCodeSequence();
                expr_stat = XPop();
            }
        } else {
            expr_loc = NULL;
            expr_stat = XPop();
        }
    }
    handle = InitCall( RT_ALLOCATE );
    for( ; (arr = SymPop()) != NULL; ) {
        if( arr->u.ns.flags & SY_SUBSCRIPTED ) {
            dim = arr->u.ns.si.va.u.dim_ext;
            CGAddParm( handle, CGInteger( _SymSize( arr ), TY_INT_4 ), TY_INT_4 );
            CGAddParm( handle, CGInteger( _DimCount( dim->dim_flags ), TY_INTEGER ), TY_INTEGER );
            CGAddParm( handle, GetAdv( arr ), TY_LOCAL_POINTER );
        }
        CGAddParm( handle, CGFEName( arr, TY_POINTER ), TY_POINTER );
        CGAddParm( handle, getFlags( arr ), FLAG_PARM_TYPE );
    }
    if( alloc_flags & ALLOC_NONE ) {
        CGAddParm( handle, expr_loc, TY_POINTER );
    } else {
        if( alloc_flags & ALLOC_LOC ) {
            CGAddParm( handle, expr_loc, TY_INT_4 );
        }
        if( alloc_flags & ALLOC_STAT ) {
            CGAddParm( handle, expr_stat, TY_POINTER );
        }
    }
    CGAddParm( handle, CGInteger( num, TY_UNSIGNED ), TY_UNSIGNED );
    CGAddParm( handle, CGInteger( alloc_flags, TY_UINT_2 ), FLAG_PARM_TYPE );
    CGDone( CGCall( handle ) );
}


void            FCDeAllocate( void ) {
//==============================

    call_handle         handle;
    sym_id              arr;
    uint                num;

    handle = InitCall( RT_DEALLOCATE );
    for( num = 0; (arr = GetPtr()) != NULL; ++num ) {
        CGAddParm( handle, CGFEName( arr, TY_POINTER ), TY_POINTER );
        CGAddParm( handle, getFlags( arr ), FLAG_PARM_TYPE );
    }
    CGAddParm( handle, CGInteger( num, TY_UNSIGNED ), TY_UNSIGNED );
    if( GetU16() & ALLOC_STAT ) {
        FCodeSequence();
        CGAddParm( handle, XPop(), TY_POINTER );
    } else {
        CGAddParm( handle, CGInteger( 0, TY_POINTER ), TY_POINTER );
    }
    CGDone( CGCall( handle ) );
}


void    FCAllocated( void ) {
//=====================

// Generate code for ALLOCATED intrinsic function.

    uint                type;
    cg_name             fl;

    type = GetU16();
    fl = XPop();

    if( type & ALLOC_STRING ) {
        fl = CGUnary( O_POINTS, fl, TY_POINTER );
    }
    XPush( CGCompare( O_NE, fl, CGInteger( 0, TY_POINTER ), TY_POINTER ) );
}
