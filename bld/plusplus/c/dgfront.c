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


#include "plusplus.h"
#include "cgfront.h"
#include "ptree.h"
#include "conpool.h"
#include "codegen.h"
#include "floatsup.h"
#include "fold.h"


void DgSymbol( SYMBOL sym )
/*************************/
// data generate: data symbol
{
    if( !SymIsInitialized( sym ) && !SymIsAnonymous( sym ) ) {
        CgFrontDataPtr( IC_DATA_SYMBOL, sym );
    }
    if( sym->id == SC_NULL || sym->id == SC_EXTERN ) {
        sym->id = SC_PUBLIC;
    } else if( SymIsStaticMember( sym ) ) {
        sym->flag |= SF_INITIALIZED;
    }
}

void DgSymbolLabel( SYMBOL sym )
/******************************/
// set location to generate data
{
    if( sym->id == SC_NULL || sym->id == SC_EXTERN ) {
        sym->id = SC_PUBLIC;
    }
    CgFrontDataPtr( IC_DATA_LABEL, sym );
}

void DgSegmentIncrement( void )
/*****************************/
// tell cgback to increment segid
{
    CgFrontData( IC_DATA_SEG );
}

void DgPadBytes( target_size_t size )
/***********************************/
// generate unitialized data for size bytes
{
    CgFrontDataInt( IC_DATA_UNDEF, size );
}

void DgZeroBytes( target_size_t size )
/************************************/
// generate zeros for size bytes
{
    CgFrontDataInt( IC_DATA_SIZE, size );
    CgFrontDataInt( IC_DATA_REPLICATE, 0 );
}

void DgStoreString( PTREE expr )
/******************************/
// store constant character string
{
    CgFrontDataPtr( IC_DATA_TEXT, expr->u.string );
}

static boolean DgStoreScalarValue( TYPE type, PTREE expr, target_size_t offset )
/******************************************************************************/
// store constant pointer expr
// return TRUE if pointer is zero
{
    boolean retn;

    CgFrontDataPtr( IC_SET_TYPE, type );
    switch( expr->op ) {
    case PT_FLOATING_CONSTANT:
        CgFrontDataPtr( IC_DATA_FLT, ConPoolFloatAdd( expr ) );
        retn = ( BFSign( expr->u.floating_constant ) == 0 );
        break;
    case PT_INT_CONSTANT:
        if( NULL == Integral64Type( expr->type ) ) {
            CgFrontDataInt( IC_DATA_INT, expr->u.int_constant );
            retn = ( expr->u.int_constant == 0 );
        } else {
            POOL_CON* pcon = ConPoolInt64Add( expr->u.int64_constant );
            CgFrontDataPtr( IC_DATA_INT64, pcon );
            retn = Zero64( &expr->u.int64_constant );
        }
        break;
    case PT_STRING_CONSTANT:
        CgFrontDataUint( IC_DATA_PTR_OFFSET, offset );
        CgFrontDataPtr( IC_DATA_PTR_STR, expr->u.string );
        retn = FALSE;
        break;
    case PT_SYMBOL:
        CgFrontDataUint( IC_DATA_PTR_OFFSET, offset );
        CgFrontDataPtr( IC_DATA_PTR_SYM, expr->u.symcg.symbol );
        retn = FALSE;
        break;
    default:
        CFatal( "dgfront.c unexpected pointer data in DgStorePointer" );
        retn = FALSE;
        break;
    }
    return( retn );
}

static boolean DgStoreMemberPointer( TYPE type, PTREE expr )
/**********************************************************/
// store constant member-pointer expr
// return TRUE if member-pointer is zero
{
    boolean retn;

    CgFrontDataPtr( IC_SET_TYPE, type );
    switch( expr->op ) {
    case PT_INT_CONSTANT:
        DgZeroBytes( CgMemorySize( type ) );
        retn = TRUE;
        break;
    case PT_UNARY:
        if( expr->cgop != CO_MEMPTR_CONST ) {
            CFatal( "dgfront.c unexpected operator in DgStoreMemberPointer" );
        } else if( MembPtrZeroConst( expr ) ) {
            DgZeroBytes( CgMemorySize( type ) );
            retn = TRUE;
        } else {
            expr = expr->u.subtree[0];
            DgStoreScalarValue( expr->type, expr->u.subtree[1], 0 );
            expr = expr->u.subtree[0];
            DgStoreScalarValue( expr->type, expr->u.subtree[1], 0 );
            expr = expr->u.subtree[0];
            DgStoreScalarValue( expr->type, expr->u.subtree[1], 0 );
            retn = FALSE;
        }
        break;
    default:
        CFatal( "dgfront.c unexpected data in DgStoreMemberPointer" );
        retn = FALSE;
        break;
    }
    return( retn );
}

void DgStoreConstScalar( PTREE expr, TYPE type, SYMBOL sym )
/**********************************************************/
{
//  symbol_flag added;
    type = type;
    switch( expr->op ) {
    case PT_INT_CONSTANT:
        sym = SymBindConstant( sym, expr->u.int64_constant );
        sym->flag |= SF_CONSTANT_INT | SF_INITIALIZED;
#if 0
        if( NULL == Integral64Type( type ) ) {
            sym->u.sval = expr->u.int_constant;
            added = SF_CONSTANT_INT | SF_INITIALIZED;
        } else {
            sym->u.pval = ConPoolInt64Add( expr->u.int64_constant );
            added = SF_CONSTANT_INT64 | SF_CONSTANT_INT | SF_INITIALIZED;
        }
        sym->flag |= added;
#endif
        break;
    }
}

boolean DgStoreScalar( PTREE expr, target_size_t offset, TYPE type )
/******************************************************************/
// return TRUE If all bytes are zero
{
    boolean retn;

    type = TypedefModifierRemove( type );
    if( type->id == TYP_MEMBER_POINTER ) {
        retn = DgStoreMemberPointer( type, expr );
    } else {
        retn = DgStoreScalarValue( type, expr, offset );
    }
    return( retn );
}

boolean DgStoreBitfield( TYPE type, target_ulong value )
/******************************************************/
// store constant bitfield
// return TRUE If all bytes are zero
{
    CgFrontDataPtr( IC_SET_TYPE, type );
    CgFrontDataInt( IC_DATA_INT, value );
    return( value == 0 );
}
