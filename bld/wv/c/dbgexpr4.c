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
* Description:  Debugger expression handling, Part IV (Arithmetic).
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbgdata.h"
#include "dipwv.h"
#include "dbglit.h"
#include "dbgstk.h"
#include "dbgerr.h"
#include "dbgmem.h"
#include "dbgitem.h"
#include "dbgtback.h"
#include "dbgri.h"
#include "ldsupp.h"
#include "madinter.h"
#include "i64.h"
#include "dbgutil.h"
#include "dbgmemor.h"
#include "dbgexpr4.h"
#include "dbgexpr3.h"
#include "dbgexpr2.h"
#include "dbgexpr.h"
#include "dbgloc.h"
#include "dbgcall2.h"
#include "dbgovl.h"
#include "dbg_dbg.h"
#include "dbgprog.h"
#include "dipimp.h"
#include "dipinter.h"
#include "dbgreg.h"
#include "addarith.h"


extern void             LclLValue( stack_entry * );
extern char             *DupStringEntry( char *, unsigned long );
extern void             RtnRetValSetup( sym_handle *, unsigned long, address * );
extern void             RtnRetValGet( sym_handle *, unsigned long, address * );

extern stack_entry      *ExprSP;


/*
 * DoPlus - add two stack entries
 */

void DoPlus( void )
{
    stack_entry *left;

    left = StkEntry( 1 );
    LRValue( left );
    RValue( ExprSP );
    switch( ExprSP->info.kind ) {
    case TK_POINTER:
    case TK_ADDRESS:
        /* get the pointer as the left operand */
        left = ExprSP;
        SwapStack( 1 );
    }
    AddOp( left, ExprSP );
    switch( left->info.kind ) {
    case TK_BOOL:
    case TK_ENUM:
    case TK_CHAR:
    case TK_INTEGER:
        U64Add( &left->v.uint, &ExprSP->v.uint, &left->v.uint );
        break;
    case TK_POINTER:
    case TK_ADDRESS:
        switch( ExprSP->info.kind ) {
        case TK_BOOL:
        case TK_ENUM:
        case TK_CHAR:
        case TK_INTEGER:
            break;
        default:
            Error( ERR_NONE, LIT_ENG( ERR_ILL_TYPE ) );
        }
        if( (left->info.modifier & TM_MOD_MASK) == TM_NEAR ) {
            //NYI: 64 bit offsets
            left->v.addr.mach.offset += U32FetchTrunc( ExprSP->v.uint );
        } else {
            //NYI: 64 bit offsets
            left->v.addr = AddrAdd( left->v.addr, U32FetchTrunc( ExprSP->v.uint ) );
        }
        break;
    case TK_REAL:
        LDAdd( &left->v.real, &ExprSP->v.real, &left->v.real );
        break;
    case TK_COMPLEX:
        LDAdd( &left->v.cmplx.re, &ExprSP->v.cmplx.re, &left->v.cmplx.re );
        LDAdd( &left->v.cmplx.im, &ExprSP->v.cmplx.im, &left->v.cmplx.im );
        break;
    default:
        Error( ERR_NONE, LIT_ENG( ERR_ILL_TYPE ) );
        break;
    }
    CombineEntries( left, left, ExprSP );
}


/*
 * DoMinus - subtract two stack entries
 */

void DoMinus( void )
{
    stack_entry *left;

    left = StkEntry( 1 );
    LRValue( left );
    RValue( ExprSP );
    AddOp( left, ExprSP );
    switch( left->info.kind ) {
    case TK_BOOL:
    case TK_ENUM:
    case TK_CHAR:
    case TK_INTEGER:
        U64Sub( &left->v.uint, &ExprSP->v.uint, &left->v.uint );
        left->info.modifier = TM_SIGNED;
        break;
    case TK_POINTER:
    case TK_ADDRESS:
        switch( ExprSP->info.kind ) {
        case TK_BOOL:
        case TK_CHAR:
        case TK_ENUM:
        case TK_INTEGER:
            //NYI: 64 bit offsets
            left->v.addr = AddrAdd( left->v.addr, -U32FetchTrunc( ExprSP->v.uint ) );
            break;
        case TK_POINTER:
        case TK_ADDRESS:
            I32ToI64( AddrDiff( left->v.addr, ExprSP->v.addr ), &left->v.sint );
            left->info.kind = TK_INTEGER;
            left->info.modifier = TM_SIGNED;
            left->info.size = sizeof( signed_64 );
            left->th = NULL;
            break;
        default:
            Error( ERR_NONE, LIT_ENG( ERR_ILL_TYPE ) );
        }
        break;
    case TK_REAL:
        LDSub( &left->v.real, &ExprSP->v.real, &left->v.real );
        break;
    case TK_COMPLEX:
        LDSub( &left->v.cmplx.re, &ExprSP->v.cmplx.re, &left->v.cmplx.re );
        LDSub( &left->v.cmplx.im, &ExprSP->v.cmplx.im, &left->v.cmplx.im );
        break;
    default:
        Error( ERR_NONE, LIT_ENG( ERR_ILL_TYPE ) );
        break;
    }
    CombineEntries( left, left, ExprSP );
}


/*
 * DoMul - multiply two stack entries
 */

void DoMul( void )
{
    stack_entry *left;
    xreal       re, im, t1, t2;

    left = StkEntry( 1 );
    BinOp( left, ExprSP );
    switch( left->info.kind ) {
    case TK_BOOL:
    case TK_ENUM:
    case TK_CHAR:
    case TK_INTEGER:
        U64Mul( &left->v.uint, &ExprSP->v.uint, &left->v.uint );
        break;
    case TK_REAL:
        LDMul( &left->v.real, &ExprSP->v.real, &left->v.real );
        break;
    case TK_COMPLEX:
        /*  (a,b) * (c,d) = (ac-bd,ad+bc)   */
        LDMul( &left->v.cmplx.re, &ExprSP->v.cmplx.re, &t1 );
        LDMul( &left->v.cmplx.im, &ExprSP->v.cmplx.im, &t2 );
        LDSub( &t1, &t2, &re );
        LDMul( &left->v.cmplx.re, &ExprSP->v.cmplx.im, &t1 );
        LDMul( &left->v.cmplx.im, &ExprSP->v.cmplx.re, &t2 );
        LDAdd( &t1, &t2, &im );
        left->v.cmplx.re = re;
        left->v.cmplx.im = im;
        break;
    default:
        Error( ERR_NONE, LIT_ENG( ERR_ILL_TYPE ) );
        break;
    }
    CombineEntries( left, left, ExprSP );
}


/*
 * DoDiv - divide two stack entries
 */

void DoDiv( void )
{
    stack_entry *left;
    xreal       re, im, mag, t1, t2;

    left = StkEntry( 1 );
    BinOp( left, ExprSP );
    switch( left->info.kind ) {
    case TK_BOOL:
    case TK_ENUM:
    case TK_CHAR:
    case TK_INTEGER:
        if( U64Test( &ExprSP->v.uint ) == 0 ) {
            Error( ERR_NONE, LIT_ENG( ERR_ZERO_DIV ) );
        }
        if( (left->info.modifier & TM_MOD_MASK) == TM_UNSIGNED ) {
            U64Div( &left->v.uint, &ExprSP->v.uint, &left->v.uint, NULL );
        } else {
            I64Div( &left->v.sint, &ExprSP->v.sint, &left->v.sint, NULL );
        }
        break;
    case TK_REAL:
        DToLD( 0.0, &t1 );
        if( LDCmp( &ExprSP->v.real, &t1 ) == 0 ) {
            Error( ERR_NONE, LIT_ENG( ERR_ZERO_DIV ) );
        }
        LDDiv( &left->v.real, &ExprSP->v.real, &left->v.real );
        break;
    case TK_COMPLEX:
        DToLD( 0.0, &t1 );
        if( LDCmp( &ExprSP->v.cmplx.re, &t1 ) == 0
         && LDCmp( &ExprSP->v.cmplx.im, &t1 ) == 0 ) {
            Error( ERR_NONE, LIT_ENG( ERR_ZERO_DIV ) );
        }
        /*   (a,b)/(c,d) = (ac+bd,bc-ad) / (c^2+d^2)     */
        LDMul( &ExprSP->v.cmplx.re, &ExprSP->v.cmplx.re, &t1 );
        LDMul( &ExprSP->v.cmplx.im, &ExprSP->v.cmplx.im, &t2 );
        LDAdd( &t1, &t2, &mag );

        LDMul( &left->v.cmplx.re, &ExprSP->v.cmplx.re, &t1 );
        LDMul( &left->v.cmplx.im, &ExprSP->v.cmplx.im, &t2 );
        LDAdd( &t1, &t2, &re );

        LDMul( &left->v.cmplx.im, &ExprSP->v.cmplx.re, &t1 );
        LDMul( &left->v.cmplx.re, &ExprSP->v.cmplx.im, &t2 );
        LDSub( &t1, &t2, &im );

        LDDiv( &re, &mag, &left->v.cmplx.re );
        LDDiv( &im, &mag, &left->v.cmplx.im );
        break;
    default:
        Error( ERR_NONE, LIT_ENG( ERR_ILL_TYPE ) );
        break;
    }
    CombineEntries( left, left, ExprSP );
}


/*
 * DoMod - calculate modulus of two stack entries
 */

void DoMod( void )
{
    stack_entry *left;
    union {
        signed_64       s;
        unsigned_64     u;
    }   dummy;

    left = StkEntry( 1 );
    BinOp( left, ExprSP );
    switch( left->info.kind ) {
    case TK_BOOL:
    case TK_ENUM:
    case TK_CHAR:
    case TK_INTEGER:
        if( U64Test( &ExprSP->v.uint ) == 0 ) {
            Error( ERR_NONE, LIT_ENG( ERR_ZERO_MOD ) );
        }
        if( (left->info.modifier & TM_MOD_MASK) == TM_UNSIGNED ) {
            U64Div( &left->v.uint, &ExprSP->v.uint, &dummy.u, &left->v.uint );
        } else {
            I64Div( &left->v.sint, &ExprSP->v.sint, &dummy.s, &left->v.sint );
        }
        break;
    default:
        Error( ERR_NONE, LIT_ENG( ERR_ILL_TYPE ) );
        break;
    }
    CombineEntries( left, left, ExprSP );
}


/*
 * DoAnd - AND two stack entries
 */

void DoAnd( void )
{
    stack_entry *left;

    left = StkEntry( 1 );
    BinOp( left, ExprSP );
    switch( left->info.kind ) {
    case TK_BOOL:
    case TK_ENUM:
    case TK_CHAR:
    case TK_INTEGER:
        U64And( &left->v.uint, &ExprSP->v.uint, &left->v.uint );
        break;
    default:
        Error( ERR_NONE, LIT_ENG( ERR_ILL_TYPE ) );
        break;
    }
    CombineEntries( left, left, ExprSP );
}


/*
 * DoOr - OR two stack entries
 */

void DoOr( void )
{
    stack_entry *left;

    left = StkEntry( 1 );
    BinOp( left, ExprSP );
    switch( left->info.kind ) {
    case TK_BOOL:
    case TK_ENUM:
    case TK_CHAR:
    case TK_INTEGER:
        U64Or( &left->v.uint, &ExprSP->v.uint, &left->v.uint );
        break;
    default:
        Error( ERR_NONE, LIT_ENG( ERR_ILL_TYPE ) );
        break;
    }
    CombineEntries( left, left, ExprSP );
}


/*
 * DoXor - XOR two stack entries
 */

void DoXor( void )
{
    stack_entry *left;

    left = StkEntry( 1 );
    BinOp( left, ExprSP );
    switch( left->info.kind ) {
    case TK_BOOL:
    case TK_ENUM:
    case TK_CHAR:
    case TK_INTEGER:
        U64Xor( &left->v.uint, &ExprSP->v.uint, &left->v.uint );
        break;
    default:
        Error( ERR_NONE, LIT_ENG( ERR_ILL_TYPE ) );
        break;
    }
    CombineEntries( left, left, ExprSP );
}


/*
 * DoShift - shift a stack entry left or right
 */

void DoShift( void )
{
    stack_entry *left;
    int          shift;

    left = StkEntry( 1 );
    RValue( ExprSP );
    ConvertTo( ExprSP, TK_INTEGER, TM_SIGNED, 0 );
    shift = I32FetchTrunc( ExprSP->v.sint );
    RValue( left );
    switch( left->info.kind ) {
    case TK_BOOL:
    case TK_ENUM:
    case TK_CHAR:
    case TK_INTEGER:
        if( shift >= 0 ) {
            U64ShiftL( &left->v.uint, shift, &left->v.uint );
        } else if( (left->info.modifier & TM_MOD_MASK) == TM_UNSIGNED ) {
            U64ShiftR( &left->v.uint, -shift, &left->v.uint );
        } else {
            I64ShiftR( &left->v.sint, -shift, &left->v.sint );
        }
        break;
    default:
        Error( ERR_NONE, LIT_ENG( ERR_ILL_TYPE ) );
        break;
    }
    CombineEntries( left, left, ExprSP );
}


/*
 * DoAddr - take the address of a stack entry
 */

void DoAddr( void )
{
    mad_type_info       mti;

    LValue( ExprSP );
    if( (ExprSP->flags & SF_LOCATION)
        && ExprSP->v.loc.num == 1
        && ExprSP->v.loc.e[0].type == LT_ADDR ) {
        ExprSP->v.addr = ExprSP->v.loc.e[0].u.addr;
        ExprSetAddrInfo( ExprSP, false );
        if( ExprSP->th != NULL ) {
            GetMADTypeDefaultAt( ExprSP->v.addr, MTK_ADDRESS, &mti );
            TypePointer(ExprSP->th, TM_FAR, mti.b.bits / BITS_PER_BYTE, ExprSP->th);
            ExprSP->info.kind = TK_POINTER;
        } else {
            ExprSP->info.kind = TK_ADDRESS;
        }
        ExprSP->flags &= ~SF_FORM_MASK;
    } else {
        Error( ERR_NONE, LIT_ENG( ERR_NEED_ADDRESS ) );
    }
}


/*
 * DoPoints - do an indirection
 */

void DoAPoints( stack_entry *stk, type_kind def )
{
    stack_flags         was_imp_addr;
    addr_off            off;

    LRValue( stk );
    was_imp_addr = stk->flags & SF_IMP_ADDR;
    switch( stk->info.kind ) {
    case TK_BOOL:
    case TK_ENUM:
    case TK_CHAR:
    case TK_INTEGER:
        //NYI: 64 bit offsets
        off = U32FetchTrunc( stk->v.uint );
        stk->v.addr = DefAddrSpaceForAddr( Context.execution );
        stk->v.addr.mach.offset = off;
        stk->info.modifier = TM_NEAR;
        /* fall through */
    case TK_POINTER:
    case TK_ADDRESS:
        if( stk->th != NULL ) {
            LocationCreate( &stk->v.loc, LT_ADDR, &stk->v.addr );
            TypeBase( stk->th, stk->th, stk->lc, &stk->v.loc );
            ClassifyEntry( stk, &stk->info );
            if( stk->info.kind == TK_VOID ) {
                Error( ERR_NONE, LIT_ENG( ERR_VOID_BASE ) );
            }
        } else {
            if( def == TK_NONE )
                def = TK_INTEGER;
            stk->info.kind = def;
            switch( def ) {
            case TK_INTEGER:
                stk->info.modifier = TM_UNSIGNED;
                stk->info.size = DefaultSize( DK_INT );
                break;
            case TK_ADDRESS:
                ExprSetAddrInfo( stk, false );
                break;
            }
            LocationCreate( &stk->v.loc, LT_ADDR, &stk->v.addr );
        }
        stk->flags |= SF_LOCATION | was_imp_addr;
        break;
    default:
        Error( ERR_NONE, LIT_ENG( ERR_ILL_TYPE ) );
        break;
    }
    stk->flags &= ~SF_CONST;
}

void DoPoints( type_kind def )
{
    DoAPoints( ExprSP, def );
}

/*
 * ConvertGiven -- convert a stack entry to the type of another stack entry
 */

static void ConvertGiven( stack_entry *object, stack_entry *new )
{
    dip_type_info       new_type;
    dip_type_info       obj_type;
    DIPHDL( type, obj_th );
    DIPHDL( type, new_th );

    if( object->th != NULL )
        HDLAssign( type, obj_th, object->th );
    ClassifyEntry( new, &new_type );
    new_type.modifier &= TM_MOD_MASK; /* turn off DEREF bit */
    ConvertTo( object, new_type.kind, new_type.modifier, new_type.size );
    if( object->th == NULL )
        goto no_adjust;
    ClassifyEntry( object, &obj_type );
    if( obj_type.kind != TK_POINTER )
        goto no_adjust;
    if( AddrComp( object->v.addr, NilAddr ) == 0 )
        goto no_adjust;
    TypeBase( obj_th, obj_th, NULL, NULL );
    ClassifyType( object->lc, obj_th, &obj_type );
    if( obj_type.kind != TK_STRUCT )
        goto no_adjust;
    ClassifyEntry( new, &new_type );
    if( new_type.kind != TK_POINTER )
        goto no_adjust;
    TypeBase( new->th, new_th, NULL, NULL );
    ClassifyType( object->lc, new_th, &new_type );
    if( new_type.kind != TK_STRUCT )
        goto no_adjust;
    /*
     * At this point we know both the old type and the new type were
     * pointers to structures (classes) and that the pointer is non-null.
     * Now we have to find out if the new type is a pointer to the base
     * type of the old. If it is, we have to adjust the pointer by the
     * correct amount.
     */
     TypeThunkAdjust( obj_th, new_th, object->lc, &object->v.addr );
     /*
        NYI: C++ actually allows us to go the other way (convert a base
        type to a derived type) if the thunk adjust does not have to go
        through a virtual base type. If the above function doesn't return
        true we can do the following:
        save = object->v.addr.mach.offset;
        if( CalcThunkAdjust( &object->v.addr, new_th, obj_th ) ) {
            diff = object->v.addr.mach.offset - save;
            object->v.addr.mach.offset -= 2*diff;
        }
        But it's such a horky thing, we'll wait for a customer to report a
        problem before putting it in.
    */
no_adjust:
    MoveTH( new, object );
}


/*
 * DoConvert - convert a stack entry to a given type
 */

void DoConvert( void )
{
    stack_entry *left;

    left = StkEntry( 1 );
    RValue( ExprSP );
    ConvertGiven( ExprSP, left );
    CombineEntries( ExprSP, left, ExprSP );
}


/*
 * DoLConvert - convert an lvalue stack entry to a given type
 */

void DoLConvert( void )
{
    stack_entry     *left;
    dip_type_info   new;

    left = StkEntry( 1 );
    LValue( ExprSP );
    if( ExprSP->flags & SF_LOCATION ) {
        ClassifyEntry( left, &new );
        if( ExprSP->v.loc.e[ExprSP->v.loc.num-1].type != LT_ADDR ) {
            if( new.size > ExprSP->info.size ) {
                Error( ERR_NONE, LIT_ENG( ERR_TYPE_CONVERSION ) );
            }
        }
        ExprSP->info = new;
        MoveTH( left, ExprSP );
    } else {
        Error( ERR_NONE, LIT_ENG( ERR_TYPE_CONVERSION ) );
    }
    CombineEntries( ExprSP, left, ExprSP );
}


/*
 * DoMakeComplex - combine the top 2 stack entries into a complex number
 */

void DoMakeComplex( void )
{
    stack_entry *left;
    xreal       zero;

    left = StkEntry( 1 );
    RValue( ExprSP );
    RValue( left );
    DToLD( 0.0, &zero );
    if( ExprSP->info.kind == TK_COMPLEX ) {
        if( LDCmp( &ExprSP->v.cmplx.im, &zero ) != 0 ) {
            Error( ERR_NONE, LIT_ENG( ERR_ILL_TYPE ) );
        }
    }
    if( left->info.kind == TK_COMPLEX ) {
        if( LDCmp( &left->v.cmplx.im, &zero ) != 0 ) {
            Error( ERR_NONE, LIT_ENG( ERR_ILL_TYPE ) );
        }
    }
    ConvertTo( ExprSP, TK_REAL, TM_NONE, sizeof( ExprSP->v.real ) );
    ConvertTo( left, TK_COMPLEX, TM_NONE, sizeof( left->v.cmplx ) );
    left->v.cmplx.im = ExprSP->v.real;
    CombineEntries( left, left, ExprSP );
}


/*
 * DoStringConcat -- Concaternate two character strings
 */

void DoStringConcat( void )
{
    stack_entry         *left, *rite;

    left = StkEntry( 1 );
    rite = ExprSP;
    RValue( left );
    RValue( rite );
    BinOp( left, rite );
    if( left->info.kind != TK_STRING ) {
        Error( ERR_NONE, LIT_ENG( ERR_ILL_TYPE ) );
    }
    CreateEntry();
    ExprSP->info.kind = TK_STRING;
    ExprSP->info.modifier = TM_NONE;
    ExprSP->info.size = left->info.size + rite->info.size;
    _ChkAlloc( ExprSP->v.string.allocated, ExprSP->info.size, LIT_ENG( ERR_NO_MEMORY_FOR_EXPR ) );
    LocationCreate( &ExprSP->v.string.loc, LT_INTERNAL, ExprSP->v.string.allocated );
    LocationAssign( &ExprSP->v.string.loc, &left->v.string.loc, left->info.size, false );
    ExprSP->v.string.loc.e[0].u.p = left->info.size +
                (byte *)ExprSP->v.string.loc.e[0].u.p;
    LocationAssign( &ExprSP->v.string.loc, &rite->v.string.loc, rite->info.size, false );
    ExprSP->v.string.loc.e[0].u.p = ExprSP->v.string.allocated;
    CombineEntries( ExprSP, left, rite );
}


/*
 * DoGivenField - do a structure field selection, given the field pointer
 */

void DoGivenField( sym_handle *member_hdl )
{
    LValue( ExprSP );
    if( !(ExprSP->flags & SF_LOCATION) ) {
        Error( ERR_NONE, LIT_ENG( ERR_NEED_ADDRESS ) );
    }
    CreateLC( ExprSP );
    ExprSP->lc->object = ExprSP->v.loc;
    ExprSP->lc->have_object = true;
    ExprSP->lc->maybe_have_object = false;
    ExprSymbol( ExprSP, member_hdl );
    SymResolve( ExprSP );
}

typedef struct {
    address             proc_addr;
    location_context    *lc;
    bool                found;
} find_context;

OVL_EXTERN bool FindContext( call_chain_entry *entry, void *_info )
{
    find_context *info = _info;
    unsigned    save_use;

    if( AddrComp( entry->start, info->proc_addr ) != 0 )
        return( true );
    save_use = info->lc->use;
    *info->lc = entry->lc;
    info->lc->use = save_use;
    info->found = true;
    return( false );
}

/*
 * DoField - do a structure field selection
 */

void DoField( void )
{
    stack_entry         *object;
    find_context        find;
    location_list       ll;
    DIPHDL( sym, sh );

    if( !(ExprSP->flags & SF_NAME) ) {
        Error( ERR_LOC, LIT_ENG( ERR_WANT_NAME ) );
    }
    object = StkEntry( 1 );
    LValue( object );
    if( object->info.kind == TK_FUNCTION ) {
        RValue( object );
        if( DeAliasAddrSym( NO_MOD, object->v.addr, sh ) == SR_NONE ) {
            Error( ERR_NONE, LIT_ENG( ERR_NO_ROUTINE ), object->v.addr );
        }
        if( SymLocation( sh, object->lc, &ll ) != DS_OK
          || ll.num != 1 || ll.e[0].type != LT_ADDR ) {
            Error( ERR_NONE, LIT_ENG( ERR_NO_ROUTINE ), object->v.addr );
        }
        find.proc_addr = ll.e[0].u.addr;
        find.found = false;
        CreateLC( ExprSP );
        find.lc = ExprSP->lc;
        WalkCallChain( FindContext, &find );
        if( !find.found ) {
            ExprSP->lc->execution = find.proc_addr;
            ExprSP->lc->regs = NULL;
            ExprSP->lc->th = NULL;
            ExprSP->lc->have_frame = false;
            ExprSP->lc->have_stack = false;
            ExprSP->lc->have_object = false;
            ExprSP->lc->maybe_have_frame = false;
            ExprSP->lc->maybe_have_object = false;
        }
        object->flags |= SF_IMP_ADDR;
    } else {
        MoveLC( object, ExprSP );
        CreateLC( ExprSP );
        if( !(object->flags & SF_LOCATION) ) {
            Error( ERR_NONE, LIT_ENG( ERR_NEED_ADDRESS ) );
        }
        ExprSP->lc->object = object->v.loc;
        ExprSP->lc->have_object = true;
        ExprSP->lc->maybe_have_object = false;
        ExprSP->lc->th = object->th;
    }
    NameResolve( ExprSP, true );
    ExprSP->flags &= ~SF_IMP_ADDR;
    ExprSP->flags |= (object->flags & SF_IMP_ADDR);
    DeleteEntry( object );
}

/*
 * DoScope - do a scoped symbol lookup
 */

static char ScopeBuff[TXT_LEN]; // nyi - this should be dynamic

void DoScope( void )
{
    stack_entry         *scope;
    char                *p;
    char                buff[TXT_LEN];

    if( !(ExprSP->flags & SF_NAME) ) {
        Error( ERR_LOC, LIT_ENG( ERR_WANT_NAME ) );
    }
    scope = StkEntry( 1 );
    if( !(scope->flags & SF_NAME) ) {
        Error( ERR_LOC, LIT_ENG( ERR_WANT_NAME ) );
    }
    p = buff;
    if( scope->flags & SF_SCOPE ) {
        memcpy( p, scope->v.li.scope.start, scope->v.li.scope.len );
        p += scope->v.li.scope.len;
    }
    memcpy( p, scope->v.li.name.start, scope->v.li.name.len );
    p += scope->v.li.name.len;
    *p++ = '\0';
    memcpy( ScopeBuff, buff, p - buff );
    ExprSP->v.li.scope.start = ScopeBuff;
    ExprSP->v.li.scope.len = p - buff;
    ExprSP->flags |= SF_SCOPE;
    NameResolve( scope, false );
    if( (scope->flags & SF_SYM) ) {
        CreateLC( ExprSP );
        ExprSP->lc->sh = scope->v.sh;
        NameResolve( ExprSP, false );
    }
    DeleteEntry( scope );
}


/*
 * DoAssign - do an assignment operation
 */

void DoAssign( void )
{
    stack_entry         *dest;
    item_mach           item;
    location_list       ll;
    location_list       src;
    unsigned long       copy;
    unsigned long       pad;

    dest = StkEntry( 1 );
    ExprResolve( ExprSP );
    LValue( ExprSP );
    if( (dest->flags & SF_NAME) && !NameResolve( dest, false ) ) {
        if( !CreateSym( &dest->v.li, &ExprSP->info ) ) {
            Error( ERR_NONE, LIT_ENG( ERR_SYM_NOT_CREATED ), dest->v.li.name.start, dest->v.li.name.len );
        }
    }
    LValue( dest );
    if( dest->flags & SF_LOCATION ) {
        if( dest->info.kind == TK_STRING ) {
            if( ExprSP->info.kind != TK_STRING ) {
                Error( ERR_NONE, LIT_ENG( ERR_TYPE_CONVERSION ) );
            }
            copy = ExprSP->info.size;
            if( copy > dest->info.size )
                copy = dest->info.size;
            if( LocationAssign( &dest->v.loc, &ExprSP->v.loc, copy, false ) != DS_OK ) {
                Error( ERR_NONE, LIT_ENG( ERR_NO_ACCESS ) );
            }
            if( dest->info.size > copy ) {
                /* have to pad */
                #define PADDING "                     "
                #define PAD_LEN (sizeof( PADDING ) - 1)
                ll = dest->v.loc;
                pad = dest->info.size - ExprSP->info.size;
                do {
                    LocationAdd( &ll, copy * 8 );
                    copy = pad;
                    if( copy > PAD_LEN )
                        copy = PAD_LEN;
                    LocationCreate( &src, LT_INTERNAL, PADDING );
                    if( LocationAssign( &ll, &src, copy, false ) != DS_OK ) {
                        Error( ERR_NONE, LIT_ENG( ERR_NO_ACCESS ) );
                    }
                    pad -= copy;
                } while( pad != 0 );
            }
        } else {
            RValue( ExprSP );
            ConvertGiven( ExprSP, dest );
            ToItem( ExprSP, &item );
            LocationCreate( &src, LT_INTERNAL, &item );
            if( LocationAssign( &dest->v.loc, &src, dest->info.size, false ) != DS_OK ) {
                Error( ERR_NONE, LIT_ENG( ERR_NO_ACCESS ) );
            }
        }
    } else {
        Error( ERR_NONE, LIT_ENG( ERR_NEED_ADDRESS ) );
    }
    MoveTH( dest, ExprSP );
    CombineEntries( ExprSP, dest, ExprSP );
}


static address AllocPgmStack( unsigned size )
{
    address     addr;
    address     new;

    if( _IsOn( SW_STACK_GROWS_UP ) )
        size = -size;
    addr = GetRegSP();
    new = addr;
    new.mach.offset -= (int)size;
    PgmStackUsage[NestedCallLevel] += size;
    if( _IsOff( SW_STACK_GROWS_UP ) )
        addr.mach.offset = new.mach.offset;
    SetRegSP( new );
    return( addr );
}


static address PokePgmStack( location_list *ll, unsigned long size )
{
    address             addr;
    location_list       dst;

    addr = AllocPgmStack( size );
    LocationCreate( &dst, LT_ADDR, &addr );
    if( LocationAssign( &dst, ll, size, false ) != DS_OK ) {
        Error( ERR_NONE, LIT_ENG( ERR_NO_WRITE_MEM ), addr );
    }
    return( addr );
}


static unsigned MakeSCB( item_mach *item, address addr, item_type typ )
{
    unsigned len;

    len = ExprSP->info.size;
    switch( typ ) {
    case IT_NWSCB:
        item->nwscb.str = addr.mach.offset;
        item->nwscb.len = len;
        break;
    case IT_FWSCB:
        ConvAddr48ToAddr32( addr.mach, item->fwscb.str );
        item->fwscb.len = len;
        break;
    case IT_NDSCB:
        item->ndscb.str = addr.mach.offset;
        item->ndscb.len = len;
        break;
    case IT_FDSCB:
        item->fdscb.str = addr.mach;
        item->fdscb.len = len;
        break;
    }
    return( ItemSize( typ ) );
}

static type_modifier DerefType( type_handle *th )
{
    dip_type_info   ti;

    if( TypeInfo( th, ExprSP->lc, &ti ) != DS_OK )
        return( TM_NONE );
    if( ti.kind != TK_POINTER )
        return( TM_NONE );
    if( !(ti.modifier & TM_FLAG_DEREF) )
        return( TM_NONE );
    return( ti.modifier & TM_MOD_MASK );
}

static item_type DerefToSCB( type_handle *th )
{
    dip_type_info   ti;

    if( TypeInfo( th, ExprSP->lc, &ti ) != DS_OK )
        return( IT_NIL );
    if( ti.kind != TK_POINTER )
        return( IT_NIL );
    if( !(ti.modifier & TM_FLAG_DEREF) )
        return( IT_NIL );
    switch( ti.modifier & TM_MOD_MASK ) {
    case TM_NEAR:
        //MAD: ????
        if( ti.size == sizeof( addr32_off ) ) {
            return( IT_NWSCB );
        } else {
            return( IT_NDSCB );
        }
    case TM_NONE:
    case TM_FAR:
    case TM_HUGE:
        //MAD: ????
        if( ti.size == sizeof( addr32_ptr ) ) {
            return( IT_FWSCB );
        } else {
            return( IT_FDSCB );
        }
    }
    return( IT_NIL );
}

/*
 * Addressable - prepare stack entry for call-by-reference
 */
            /* for Fortran CHARACTER type arguments, must pass
               a pointer to an SCB of the character block */

static void Addressable( bool build_scb, type_handle *parm_type )
{
    unsigned            len;
    address             addr;
    item_mach           item;
    unsigned            i;
    location_list       src;
    DIPHDL( type, th );

    LValue( ExprSP );
    if( ExprSP->flags & SF_LOCATION ) {
        for( i = 0; i < ExprSP->v.loc.num; ++i ) {
            if( ExprSP->v.loc.e[i].type != LT_ADDR ) {
                RValue( ExprSP );
                break;
            }
        }
    }
    if( ExprSP->flags & SF_LOCATION ) {
        addr = ExprSP->v.loc.e[0].u.addr;
    } else {
        if( ExprSP->info.kind == TK_STRING ) {
            src = ExprSP->v.string.loc;
        } else {
            TypeBase( parm_type, th, NULL, NULL );
            PushType( th );
            SwapStack( 1 );
            DoConvert();
            ToItem( ExprSP, &item );
            LocationCreate( &src, LT_INTERNAL, &item );
        }
        addr = PokePgmStack( &src, ExprSP->info.size );
    }
    if( build_scb && ExprSP->info.kind == TK_STRING ) {
        len = MakeSCB( &item, addr, DerefToSCB( parm_type ) );
        if( len != 0 ) {
            LocationCreate( &src, LT_INTERNAL, &item );
            addr = PokePgmStack( &src, len );
        }
    }
    ExprSP->flags &= ~SF_LOCATION;
    ExprSP->v.addr = addr;
    ExprSetAddrInfo( ExprSP, false );
    ExprSP->th = NULL;
}



/*
 * DoCall - call a procedure
 */

void DoCall( int num_parms, bool build_scbs )
{
    stack_entry         *rtn_entry;
    address             addr;
    address             ret_addr;
    address             string_addr;
    location_list       ll;
    location_list       ret_ll;
    enum {
        RET_NORMAL,
        RET_REFERENCE,
        RET_SCB
    }                   ret_kind;
    DIPHDL( sym, rtn_sh );
    DIPHDL( type, obj_th );
    DIPHDL( type, rtn_th );
    DIPHDL( type, ret_th );
    DIPHDL( type, parm_th );
    DIPHDL( type, th );
    sym_info            rtn_si;
    dip_type_info       this_ti;
    dip_type_info       ti;
    dip_type_info       ret_ti;
    item_mach           item;
    unsigned            parm_loc_adjust;
    unsigned            parm;
    unsigned            size;
    dip_status          ds;

    if( _IsOn( SW_CALL_FATAL ) )
        Error( ERR_NONE, LIT_ENG( ERR_CALL_NOT_ALLOWED ) );
    rtn_entry = StkEntry( num_parms );
    RValue( rtn_entry );
    switch( rtn_entry->info.kind ) {
    case TK_BOOL:
    case TK_ENUM:
    case TK_CHAR:
    case TK_INTEGER:
        addr = Context.execution;
        //NYI: 64 bit offsets
        addr.mach.offset = U32FetchTrunc( rtn_entry->v.uint );
        break;
    case TK_POINTER:
    case TK_ADDRESS:
        if( (rtn_entry->info.modifier & TM_MOD_MASK) == TM_NEAR ) {
            addr = Context.execution;
            addr.mach.offset = rtn_entry->v.addr.mach.offset;
        } else {
            addr = rtn_entry->v.addr;
        }
        break;
    default:
        Error( ERR_NONE, LIT_ENG( ERR_ILL_TYPE ) );
        break;
    }
    if( DeAliasAddrSym( NO_MOD, addr, rtn_sh ) != SR_EXACT ) {
        AddrFix( &addr );
        Error( ERR_NONE, LIT_ENG( ERR_NO_ROUTINE ), addr );
    }
    SymInfo( rtn_sh, rtn_entry->lc, &rtn_si );
    if( rtn_si.kind != SK_PROCEDURE ) {
        Error( ERR_NONE, LIT_ENG( ERR_NO_ROUTINE ), addr );
    }
    SymType( rtn_sh, rtn_th );
    TypeProcInfo( rtn_th, ret_th, 0 );
    /* check if it is Fortran function returning CHARACTER blocks */
    ret_kind = RET_NORMAL;
    if( DerefType( ret_th ) != TM_NONE ) {
        ret_kind = RET_REFERENCE;
        TypeBase( ret_th, th, NULL, NULL );
        TypeInfo( th, rtn_entry->lc, &ti );
        if( ti.kind == TK_STRING ) {
            ret_kind = RET_SCB;
            if( ti.size == 0 ) {
                /* character*(*) function -- not supported */
                Error( ERR_NONE, LIT_ENG( ERR_CHAR_STAR_STAR ) );
            }
            string_addr = AllocPgmStack( ti.size );
            size = MakeSCB( &item, string_addr, DerefToSCB( ret_th ) );
            LocationCreate( &ll, LT_INTERNAL, &item );
            PushAddr( PokePgmStack( &ll, size ) );
            HDLAssign( type, ret_th, th );
            ++num_parms;
        }
    }
    SymObjType( rtn_sh, obj_th, &this_ti );
    if( this_ti.kind == TK_POINTER ) {
        if( !rtn_entry->lc->have_object ) {
            Error( ERR_NONE, LIT_ENG( ERR_NO_OBJECT_FOR_CALL ) );
        }
        parm_loc_adjust = 1;
    } else {
        parm_loc_adjust = 0;
    }
    for( parm = num_parms; parm > 0; --parm ) {
        ds = SymParmLocation( rtn_sh, rtn_entry->lc, &ll, parm + parm_loc_adjust );
        if( ds & DS_ERR ) {
            Error( ERR_NONE, LIT_ENG( ERR_CALL_NOT_ALLOWED ) );
        }
        if( ds != DS_OK ) {
            LocationCreate( &ll, LT_INTERNAL, NULL );
        }
        PushLocation( &ll, &StkEntry( parm )->info );
        MoveSP( 2 );
        if( TypeProcInfo( rtn_th, parm_th, parm ) == DS_OK ) {
            PushType( parm_th );
            MoveSP( -1 );
            if( DerefType( parm_th ) != TM_NONE ) {
                Addressable( build_scbs, parm_th );
            }
            DoConvert();
            MoveSP( 1 );
        }
    }
    if( this_ti.kind == TK_POINTER ) {
        TypeInfo( obj_th, rtn_entry->lc, &ti );
        PushLocation( &rtn_entry->lc->object, &ti );
        Addressable( false, obj_th );
        ConvertTo( ExprSP, this_ti.kind, this_ti.modifier, this_ti.size );
        ds = SymParmLocation( rtn_sh, rtn_entry->lc, &ll, 1 );
        if( ds & DS_ERR ) {
            Error( ERR_NONE, LIT_ENG( ERR_CALL_NOT_ALLOWED ) );
        }
        if( ds != DS_OK ) {
            LocationCreate( &ll, LT_INTERNAL, NULL );
        }
        PushLocation( &ll, &ExprSP->info );
        MoveSP( 2 );
        ++num_parms;
    }
    MoveSP( -2 * num_parms );
    FreezeRegs();
    TypeInfo( ret_th, rtn_entry->lc, &ret_ti );
    ds = SymParmLocation( rtn_sh, rtn_entry->lc, &ret_ll, 0 );
    if( ds & DS_ERR ) {
        Error( ERR_NONE, LIT_ENG( ERR_CALL_NOT_ALLOWED ) );
    }
    if( ds != DS_OK ) {
        if( ret_kind != RET_SCB ) {
            ret_ti.kind = TK_VOID;
        }
    }
    if( rtn_si.rtn_calloc ) {
        ret_addr = AllocPgmStack( ret_ti.size );
        PushAddr( ret_addr );
        ConvertTo( ExprSP, TK_POINTER, rtn_si.ret_modifier, rtn_si.ret_size );
        ToItem( ExprSP, &item );
        DeleteEntry( ExprSP );
        LocationCreate( &ll, LT_INTERNAL, &item );
        LocationAssign( &ret_ll, &ll, rtn_si.ret_size, false );
        LocationCreate( &ret_ll, LT_ADDR, &ret_addr );
    }
    if( PerformCall( addr, rtn_si.rtn_far, num_parms ) ) {
        if( ret_kind == RET_SCB ) {
            LocationCreate( &ret_ll, LT_ADDR, &string_addr );
        } else if( rtn_si.ret_modifier != TM_NONE && !rtn_si.rtn_calloc ) {
            PushLocation( &ret_ll, NULL );
            ExprSP->info.kind = TK_POINTER;
            ExprSP->info.modifier = rtn_si.ret_modifier;
            ExprSP->info.size = rtn_si.ret_size;
            RValue( ExprSP );
            LocationCreate( &ret_ll, LT_ADDR, &ExprSP->v.addr );
            DeleteEntry( ExprSP );
        }
        if( ret_ti.kind == TK_VOID ) {
            CreateEntry();
            ExprSP->info.kind = TK_VOID;
        } else {
            PushType( ret_th );
            ExprSP->v.loc = ret_ll;
            ExprSP->info = ret_ti;
            ExprSP->flags = SF_LOCATION;
            RValue( ExprSP );
            if( ret_kind == RET_REFERENCE ) {
                DoPoints( 0 );
            }
        }
    }
    ExprSP->flags &= ~(SF_CONST | SF_IMP_ADDR);
    DeleteEntry( rtn_entry );
    UnFreezeRegs();
    FreePgmStack( false );
}

#if 0
void InitReturnInfo( sym_handle *f, return_info *ri )
{
    sym_info    si;
    DIPHDL( type, rtn_th );
    DIPHDL( type, ret_th );

    memset( ri, 0, sizeof( *ri ) );
    SymInfo( f, &Context, &si );
    if( si.kind != SK_PROCEDURE ) {
        Error( ERR_NONE, LIT_ENG( ERR_NOT_PROCEDURE ), f );
        return;
    }
    SymType( f, rtn_th );
    TypeProcInfo( rtn_th, ret_th, 0 );
    TypeInfo( ret_th, &Context, &ri->ti );
    /* check if it is Fortran function returning CHARACTER blocks */
    if( (ri->ti.kind == TK_POINTER) && (ri->ti.modifier & TM_FLAG_DEREF) ) {
        ri->want_base_type = 1;
        ri->ref_size = ri->ti.size;
        if( (ri->ti.modifier & TM_MOD_MASK) != TM_NEAR ) {
            ri->ref_far = true;
        }
        TypeBase( ret_th, ret_th, NULL, NULL );
        TypeInfo( ret_th, &Context, &ri->ti );
        if( ri->ti.kind == TK_STRING ) {
            ri->rl_passed_in = true;
            ri->scb = true;
        }
    }
    if( !ri->scb && ri->ti.size == 0 ) {
        Error( ERR_NONE, LIT_ENG( ERR_CANT_GET_RV ) );
        return;
    }
    if( si.ret_modifier != TM_NONE && si.rtn_calloc ) {
        ri->rl_passed_in = true;
    }
}

void PrepReturnInfo( sym_handle *f, return_info *ri )
{
    f = f;
    if( ri->rl_passed_in || ri->scb ) {
        PushLocation( &ri->ll, NULL );
        ExprSP->info.kind = TK_POINTER;
        ExprSP->info.size = ri->ref_size;
        if( ri->ref_far ) {
            ExprSP->info.modifier = TM_FAR;
        } else {
            ExprSP->info.modifier = TM_NEAR;
        }
        RValue( ExprSP );
        LocationCreate( &ri->ll, LT_ADDR, &ExprSP->v.addr );
        if( ri->scb && ri->ti.size == 0 ) {
            /* CHAR*(*) function. Get the size from the passed in SCB */
            ExprSP->v.loc = ri->ll;
            ExprSP->info.kind = TK_INTEGER;
            ExprSP->info.modifier = TM_UNSIGNED;
            ExprSP->info.size = ri->ref_size;
            if( ri->ref_far )
                ExprSP->info.size -= sizeof( addr_seg );
            ExprSP->flags = SF_LOCATION;
            RValue( ExprSP );
            ri->ti.size = ExprSP->v.uint;
        }
        DeleteEntry( ExprSP );
    }
    ri->rl_passed_in = false;
}

void PushReturnInfo( sym_handle *f, return_info *ri )
{
    DIPHDL( type, th );

    if( ri->rl_passed_in ) {
        Error( ERR_NONE, LIT_ENG( ERR_CANT_GET_RV ) );
        return;
    }
    if( ri->ti.kind == TK_VOID ) {
        CreateEntry();
        ExprSP->info.kind = TK_VOID;
        return;
    }
    if( ri->ref_size != 0 ) {
        PushLocation( &ri->ll, NULL );
        ExprSP->info.kind = TK_POINTER;
        ExprSP->info.size = ri->ref_size;
        if( ri->ref_far ) {
            ExprSP->info.modifier = TM_FAR;
        } else {
            ExprSP->info.modifier = TM_NEAR;
        }
        RValue( ExprSP );
        LocationCreate( &ri->ll, LT_ADDR, &ExprSP->v.addr );
        DeleteEntry( ExprSP );
        if( ri->scb ) {
            PushLocation( &ri->ll, &ri->ti );
            return;
        }
    }
    SymType( f, th );
    TypeProcInfo( th, th, 0 );
    if( ri->want_base_type )
        TypeBase( th, th, NULL, NULL );
    PushType( th );
    ExprSP->v.loc = ri->ll;
    ExprSP->flags = SF_LOCATION;
}
#endif
