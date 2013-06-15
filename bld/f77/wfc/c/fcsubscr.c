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


//
// FCSUBSCR  : subscripting code generation routines
//

#include "ftnstd.h"
#include "global.h"
#include "rtconst.h"
#include "wf77defs.h"
#include "cg.h"
#include "cpopt.h"
#include "emitobj.h"
#include "fctypes.h"
#include "cgswitch.h"
#include "cgprotos.h"


extern  cg_name         XPop(void);
extern  void            XPush(cg_name);
extern  cg_name         GetTypedValue(void);
extern  cg_name         SymIndex(sym_id,cg_name);
extern  cg_name         CharItemLen(sym_id);
extern  cg_name         SCBPtrAddr(cg_name);
extern  cg_name         SCBLenAddr(cg_name);
extern  cg_name         StructRef(cg_name,int);
extern  call_handle     InitCall(RTCODE);
extern  sym_id          FindAdvShadow(sym_id);

/* Forward declarations */
static  void    DbSubscript( sym_id arr );
static  void    VariableDims( sym_id arr );
static  void    ConstDims( sym_id arr );
static  void    Index( sym_id arr, cg_name offset );
void    MakeSCB( sym_id scb, cg_name len );

    
    
cg_name GetAdv( sym_id arr ) {
//============================

    act_dim_list        *dim_ptr;

    dim_ptr = arr->ns.si.va.u.dim_ext;
    if( dim_ptr->adv == NULL ) {
        // ADV is allocated on the stack
        return( CGFEName( FindAdvShadow( arr ), TY_ADV_ENTRY ) );
    } else {
        return( CGBackName( dim_ptr->adv, TY_ADV_ENTRY ) );
    }
}


cg_name ArrayEltSize( sym_id arr ) {
//==================================

// Get element size of an array.

    cg_name     elt_size;
    uint        size;

    size = _SymSize( arr );
    if( size == 0 ) {   // character*(*) array
        elt_size = CharItemLen( arr );
    } else {
        elt_size = CGInteger( size, TY_INTEGER );
    }
    return( elt_size );
}


void    FCSubscript( void ) {
//=====================

// Do a subscript operation.

    sym_id      arr;

    arr = GetPtr();
    if( Options & OPT_BOUNDS ) {
        DbSubscript( arr );
    } else {
        if( _AdvRequired( arr->ns.si.va.u.dim_ext ) ) {
            VariableDims( arr );
        } else {
            ConstDims( arr );
        }
    }
    if( arr->ns.u1.s.typ == FT_CHAR ) {
        MakeSCB( GetPtr(), ArrayEltSize( arr ) );
    }
}


void    MakeSCB( sym_id scb, cg_name len ) {
//==========================================

// Make an SCB.

    CGTrash( CGAssign( SCBLenAddr( CGFEName( scb, TY_CHAR ) ), len,
                       TY_INTEGER ) );
    // assumption is that the pointer in the SCB is the first field in
    // the SCB so that when we push the cg_name returned by CGAssign()
    // it is a pointer to the SCB
    XPush( CGLVAssign( SCBPtrAddr( CGFEName( scb, TY_CHAR ) ),
                       XPop(), TY_POINTER ) );
// Don't do it the following way:
//    CGTrash( CGAssign( SCBPtrAddr( CGFEName( scb, TY_CHAR ) ),
//                       XPop(), TY_POINTER ) );
//    XPush( CGFEName( scb, TY_CHAR ) );
}


static  cg_name HiBound( sym_id arr, int ss_offset ) {
//====================================================

// Get hi bound from ADV.

    ss_offset = BETypeLength( TY_ADV_LO ) * ( ss_offset + 1 ) +
                BETypeLength( TY_ADV_HI ) * ss_offset;
    return( CGUnary( O_POINTS, StructRef( GetAdv( arr ), ss_offset ), TY_ADV_HI ) );

}


static  cg_name Multiplier( sym_id arr, int subs_no ) {
//=====================================================

// Compute mulitplier.

    cg_name     multiplier;

    multiplier = CGInteger( 1, TY_INT_4 );
    while( subs_no != 0 ) {
        multiplier = CGBinary( O_TIMES, multiplier,
                               HiBound( arr, subs_no - 1 ), TY_INT_4 );
        subs_no--;
    }
    return( multiplier );
}


cg_name ArrayNumElts( sym_id arr ) {
//==================================

// Get number of elements in an array.

    cg_name             num_elts;
    act_dim_list        *dim;

    dim = arr->ns.si.va.u.dim_ext;
    if( _AdvRequired( dim ) ) {
        num_elts = Multiplier( arr, _DimCount( dim->dim_flags ) );
    } else {
        num_elts = CGInteger( dim->num_elts, TY_INT_4 );
    }
    return( num_elts );
}


cg_name FieldArrayNumElts( sym_id arr ) {
//=======================================

// Get number of elements in an array.

    return( CGInteger( arr->fd.dim_ext->num_elts, TY_INT_4 ) );
}


cg_name ConstArrayOffset( act_dim_list *dims ) {
//==============================================

    int                 dims_no;
    cg_name             hi_off;
    intstar4            multiplier;
    intstar4            hi;
    intstar4            lo;
    intstar4            *bounds;
    intstar4            lo_off;

    dims_no = _DimCount( dims->dim_flags );
    bounds = &dims->subs_1_lo;
    multiplier = 1;
    hi_off = CGInteger( 0, TY_INT_4 );
    lo_off = 0;
    for(;;) {
        lo = *bounds;
        bounds++;
        hi = *bounds;
        bounds++;

        // offset += ( ss - lo ) * multiplier;
        //              or
        // hi_off += ss*multiplier
        // lo_off -= lo*multiplier

        hi_off = CGBinary( O_PLUS,
                           hi_off,
                           CGBinary( O_TIMES,
                                     GetTypedValue(),
                                     CGInteger( multiplier, TY_INT_4 ),
                                     TY_INT_4 ),
                           TY_INT_4 );
        lo_off -= lo * multiplier;
        if( --dims_no == 0 ) break;

        multiplier *= ( hi - lo + 1 );
    }
    return( CGBinary( O_PLUS, CGInteger( lo_off, TY_INT_4 ), hi_off, TY_INT_4 ) );
}


static  void    ConstDims( sym_id arr ) {
//=======================================

// Subscript an array that has a constant array declarator.

    Index( arr, ConstArrayOffset( arr->ns.si.va.u.dim_ext ) );
}


static  void    Index( sym_id arr, cg_name offset ) {
//===================================================

// Perform indexing operation.

    offset = CGBinary( O_TIMES, offset, ArrayEltSize( arr ), TY_INT_4 );
    XPush( SymIndex( arr, offset ) );
}


static  cg_name LoBound( sym_id arr, int ss_offset ) {
//====================================================

// Get lo bound from ADV.

    cg_name             lo_bound;
    act_dim_list        *dim_ptr;

    dim_ptr = arr->ns.si.va.u.dim_ext;
    if( _LoConstBound( dim_ptr->dim_flags, ss_offset + 1 ) ) {
        lo_bound = CGInteger( ((intstar4 *)(&dim_ptr->subs_1_lo))[2*ss_offset],
                              TY_INT_4 );
    } else {
        lo_bound = CGUnary( O_POINTS,
                            StructRef( GetAdv( arr ),
                                       ss_offset*BETypeLength( TY_ADV_ENTRY ) ),
                            TY_ADV_LO );
    }
    return( lo_bound );
}


static  void    VariableDims( sym_id arr ) {
//==========================================

// Subscript an array that has a variable array declarator.

    act_dim_list        *dim_ptr;
    int                 dims_no;
    int                 ss_offset;
    cg_name             offset;
    cg_name             c_offset;

    dim_ptr = arr->ns.si.va.u.dim_ext;
    dims_no = _DimCount( dim_ptr->dim_flags );
    offset = CGInteger( 0, TY_INT_4 );
    c_offset = CGInteger( 0, TY_INT_4 );
    ss_offset = 0;
    while( ss_offset < dims_no ) {

        // offset += ( ss - lo ) * multiplier;
        //              or
        // offset   += ss*multiplier
        // c_offset -= lo*multiplier

        offset = CGBinary( O_PLUS,
                           offset,
                           CGBinary( O_TIMES,
                                     GetTypedValue(),
                                     Multiplier( arr, ss_offset ),
                                     TY_INT_4 ),
                           TY_INT_4 );
        c_offset = CGBinary( O_MINUS,
                             c_offset,
                             CGBinary( O_TIMES,
                                       LoBound( arr, ss_offset ),
                                       Multiplier( arr, ss_offset ),
                                       TY_INT_4 ),
                             TY_INT_4 );
        ss_offset++;
    }
    Index( arr, CGBinary( O_PLUS, c_offset, offset, TY_INT_4 ) );
}


static  void    DbSubscript( sym_id arr ) {
//=========================================

// Generate call to debugging subscript routine.

    act_dim_list        *dim_ptr;
    int                 dims_no;
    int                 i;
    call_handle         call;
    cg_name             offset;
    cg_name             subscripts[MAX_DIM];

    dim_ptr = arr->ns.si.va.u.dim_ext;
    dims_no = _DimCount( dim_ptr->dim_flags );
    call = InitCall( RT_SUBSCRIPT );
    for( i = 0; i < dims_no; ++i ) {
        subscripts[ i ] = GetTypedValue();
    }
    for( i = 1; i <= dims_no; ++i ) {
        CGAddParm( call, subscripts[ dims_no - i ], TY_INT_4 );
    }
    CGAddParm( call, GetAdv( arr ), TY_LOCAL_POINTER );
    CGAddParm( call, CGInteger( _DimCount( dim_ptr->dim_flags ), TY_INTEGER ),
               TY_INTEGER );
    offset = CGUnary( O_POINTS, CGCall( call ), TY_INT_4 );
    Index( arr, offset );
}


void    FCAdvFillLo( void ) {
//=====================

// Fill lo bound of a dimension.

    sym_id              arr;
    int                 lo_offset;
    cg_name             adv;
    cg_name             lo;
    unsigned            ss;

    arr = GetPtr();
    adv = GetAdv( arr );
    ss = GetU16();
    lo = GetTypedValue();
    lo_offset = (ss - 1) * BETypeLength( TY_ADV_ENTRY );
    CGDone( CGAssign( StructRef( adv, lo_offset ), lo, TY_ADV_LO ) );
}


void    FCAdvFillHi( void ) {
//=====================

// Fill hi bound of a dimension (actually computes # of elements in dimension).

    sym_id              arr;
    act_dim_list        *dim_ptr;
    int                 lo_size;
    int                 hi_size;
    int                 hi_offset;
    int                 ss;
    cg_name             num_elts;
    cg_name             hi;
    cg_name             adv;
    call_handle         call;

    arr = GetPtr();
    dim_ptr = arr->ns.si.va.u.dim_ext;
    adv = GetAdv( arr );
    hi_size = BETypeLength( TY_ADV_HI );
    lo_size = BETypeLength( TY_ADV_LO );
    ss = GetU16();
    hi = GetTypedValue();
    if( CGOpts & CGOPT_DI_CV ) {
        hi_offset = _DimCount( dim_ptr->dim_flags ) * BETypeLength( TY_ADV_ENTRY );
        if( Options & OPT_BOUNDS ) {
            hi_offset += BETypeLength( TY_POINTER );
        }
        hi_offset += (ss - 1) * (lo_size + BETypeLength( TY_ADV_HI_CV )) + lo_size;
        hi = CGAssign( StructRef( adv, hi_offset ), hi, TY_ADV_HI_CV );
        adv = GetAdv( arr );
    }
    if( Options & OPT_BOUNDS ) {
        call = InitCall( RT_ADV_FILL_HI );
        CGAddParm( call, hi, TY_INT_4 );
        CGAddParm( call, CGInteger( ss, TY_UNSIGNED ), TY_UNSIGNED );
        CGAddParm( call, adv, TY_LOCAL_POINTER );
        CGDone( CGUnary( O_POINTS, CGCall( call ), TY_INT_4 ) );
    } else {
        hi_offset = (ss - 1) * ( lo_size + hi_size ) + lo_size;
        num_elts = CGBinary( O_PLUS, hi,
                             CGBinary( O_MINUS, CGInteger( 1, TY_INTEGER ),
                                       LoBound( arr, ss - 1 ),
                                       TY_ADV_HI ),
                             TY_ADV_HI );
        CGDone( CGAssign( StructRef( adv, hi_offset ), num_elts, TY_ADV_HI ) );
    }
}


void    FCAdvFillHiLo1( void ) {
//========================

// Fill hi and lo=1 bound of a dimension.

    sym_id              arr;
    cg_name             lo;
    cg_name             hi;
    cg_name             adv;
    unsigned            ss;
    int                 lo_size;
    int                 hi_size;
    int                 lo_offset;
    int                 hi_offset;
    call_handle         call;

    // Get general information
    arr = GetPtr();
    ss = GetU16();

    adv = GetAdv( arr );
    hi_size = BETypeLength( TY_ADV_HI );
    lo_size = BETypeLength( TY_ADV_LO );
    hi = GetTypedValue();

    if( Options & OPT_BOUNDS ) {
        call = InitCall( RT_ADV_FILL_HI_LO1 );
        CGAddParm( call, hi, TY_INT_4 );
        CGAddParm( call, CGInteger( ss, TY_UNSIGNED ), TY_UNSIGNED );
        CGAddParm( call, adv, TY_LOCAL_POINTER );
        CGDone( CGUnary( O_POINTS, CGCall( call ), TY_INT_4 ) );
    } else {
        hi_offset = (ss - 1) * ( lo_size + hi_size ) + lo_size;
        CGDone( CGAssign( StructRef( adv, hi_offset ), hi, TY_ADV_HI ) );
        // set lo bound of the adv
        lo = CGInteger( 1, TY_INT_4 );
        lo_offset = (ss - 1) * BETypeLength( TY_ADV_ENTRY );
        adv = GetAdv( arr );
        CGDone( CGAssign( StructRef( adv, lo_offset ), lo, TY_ADV_LO ) );
    }
}

