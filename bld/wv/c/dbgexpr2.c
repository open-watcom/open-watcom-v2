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


#include <stdlib.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbglit.h"
#include "dbgstk.h"
#include "dbgerr.h"
#include "dbgmem.h"
#include "dbgitem.h"
#include "i64.h"
#include "numscan.h"
#include "madinter.h"
#include "dbgutil.h"
#include "dbgexpr4.h"
#include "dbgexpr3.h"
#include "dbgexpr2.h"
#include "dbgexpr.h"
#include "dbgloc.h"
#include "dbgprog.h"
#include "dbglkup.h"


static bool DefaultTypeInfo( dip_type_info *info )
{
    bool        real_type;

    real_type = true;
    switch( info->kind ) {
    case TK_DATA:
    case TK_NONE:
        real_type = false;
        info->kind = TK_INTEGER;
        /* fall through */
    case TK_INTEGER:
        if( info->modifier == TM_NONE ) {
            info->modifier = TM_UNSIGNED;
        }
        if( info->size == 0 ) {
            info->size = DefaultSize( DK_INT );
        }
        break;
    case TK_CODE:
    case TK_ADDRESS:
        real_type = false;
        /* fall through */
    case TK_POINTER:
        if( info->modifier == TM_NONE ) {
            info->modifier = TM_FAR;
        }
        if( info->size == 0 ) {
            info->size = DefaultSize( DK_DATA_PTR );
            switch( info->modifier ) {
            case TM_FAR:
            case TM_HUGE:
                info->size += sizeof( addr_seg );
                break;
            }
        }
        break;
    case TK_STRING:
        if( info->modifier == TM_NONE ) {
            info->modifier = TM_ASCII;
        }
        break;
    }
    return( real_type );
}

bool ClassifyType( location_context *lc, type_handle *th, dip_type_info *info )
{
    TypeInfo( th, lc, info );
    return( DefaultTypeInfo( info ) );
}

void ClassifyEntry( stack_entry *stk, dip_type_info *info )
{
    if( stk->th == NULL ) {
        *info = stk->info;
        DefaultTypeInfo( info );
    } else if( !ClassifyType( stk->lc, stk->th, info ) ) {
        stk->th = NULL;
    }
}

static void GetTrueEntry( stack_entry *entry )
{
    addr_off            near_off;
    type_modifier       mod;

    for( ;; ) {
        mod = entry->info.modifier;
        if( !(mod & TM_FLAG_DEREF) ) break;
        DoAPoints( entry, TK_NONE );
        if( entry->info.kind == TK_VOID ) Error( ERR_NONE, LIT_ENG( ERR_VOID_BASE ) );
        switch( mod & TM_MOD_MASK ) {
        case TM_NEAR:
            if( entry->info.kind == TK_FUNCTION ) {
                near_off = entry->v.loc.e[0].u.addr.mach.offset;
                entry->v.loc.e[0].u.addr = Context.execution;
                entry->v.loc.e[0].u.addr.mach.offset = near_off;
            }
            break;
        }
    }
}

static void LocationToAddr( stack_entry *entry )
{
    entry->v.addr = entry->v.loc.e[0].u.addr;
    entry->flags &= ~SF_LOCATION;
    ExprSetAddrInfo( entry, false );
}

sym_list *ExprGetSymList( stack_entry *entry, bool source_only )
{
    sym_list            *syms;
    void                *d;
    symbol_source       ss;

    CreateLC( entry );
    if( entry->lc->sh != NULL ) {
        ss = SS_SCOPESYM;
        d = entry->lc->sh;
        entry->lc->sh = NULL;
    } else if( entry->lc->th != NULL ) {
        ss = SS_TYPE;
        d = entry->lc->th;
        entry->lc->th = NULL;
        source_only = true;
    } else {
        ss = SS_SCOPED;
        d = &entry->lc->execution;
    }
    syms = LookupSymList( ss, d, source_only, &entry->v.li );
    return( syms );
}

void ExprSymbol( stack_entry *entry, sym_handle *sh )
{
    SET_TH( entry );
    if( SymType( sh, entry->th ) != DS_OK ) entry->th = NULL;
    ClassifyEntry( entry, &entry->info );
    SET_SH( entry );
    HDLAssign( sym, entry->v.sh, sh );
    entry->flags |= SF_SYM;
}


bool NameResolve( stack_entry *entry, bool source_only )
{
    sym_list    *syms;

    if( entry->flags & SF_NAME ) {
        syms = ExprGetSymList( entry, source_only );
        if( syms == NULL ) return( false );
        if( syms->next != NULL ) {
            syms = Disambiguate( syms, entry->lc );
        }
        entry->flags &= ~SF_NAME;
        ExprSymbol( entry, SL2SH( syms ) );
        FreeSymHandle( syms );
    }
    return( true );
}


void SymResolve( stack_entry *entry )
{
    item_mach   tmp;
    sym_handle  *sh;

    if( entry->flags & SF_SYM ) {
        sh = entry->v.sh;
        entry->flags &= ~SF_FORM_MASK;
        if( SymLocation( sh, entry->lc, &entry->v.loc ) == DS_OK ) {
            entry->flags |= SF_LOCATION;
            if( entry->v.loc.e[0].type == LT_ADDR ) {
                entry->flags |= SF_IMP_ADDR;
            }
            GetTrueEntry( entry );
        } else {
            if( entry->info.kind == TK_STRING ) {
                _ChkAlloc( entry->v.string.allocated, entry->info.size,
                            LIT_ENG( ERR_NO_MEMORY_FOR_EXPR ) );
                LocationCreate( &entry->v.string.loc, LT_INTERNAL,
                            entry->v.string.allocated );
                if( SymValue( sh, entry->lc, entry->v.string.allocated ) != DS_OK ) {
                    Error( ERR_NONE, LIT_ENG( ERR_NO_ACCESS ) );
                }
            } else {
                if( SymValue( sh, entry->lc, &tmp ) != DS_OK ) {
                    Error( ERR_NONE, LIT_ENG( ERR_NO_ACCESS ) );
                }
                FromItem( &tmp, entry );
            }
        }
        switch( entry->info.kind ) {
        case TK_CODE:
        case TK_ADDRESS:
            if( !(entry->flags & SF_LOCATION) ) {
                ExprSetAddrInfo( entry, false );
/*
        This was here before, but that messes up things like 'do x=0:0'
        where 'x' is a newly created debugger variable. I can't think
        of any reason why you'd want to do this. If it turns out that there
        is a reason, talk to me.
                        Brian.
            } else {
                LocationToAddr( entry );
*/
            }
            entry->th = NULL;
            break;
        }
    }
}


void LValue( stack_entry *entry )
{
    if( !NameResolve( entry, false ) ) {
        Error( ERR_NONE, LIT_ENG( ERR_UNKNOWN_SYMBOL ), entry->v.li.name.start,
                            entry->v.li.name.len );
    }
    SymResolve( entry );
    GetTrueEntry( entry );
}


void ClassNum( stack_entry *entry )
{
    unsigned long       val;

    entry->info.kind = TK_INTEGER;
    entry->flags &= ~SF_FORM_MASK;
    entry->flags |= SF_CONST;
    if( I64Test( &entry->v.sint ) < 0 ) {
        /*
            A bit backwards - if the top bit is on, it won't fit in in
            63 bits.
        */
        entry->info.modifier = TM_UNSIGNED;
        entry->info.size = sizeof( unsigned_64 );
        return;
    }
    if( !U64IsU32( entry->v.uint ) ) {
        entry->info.modifier = TM_SIGNED;
        entry->info.size = sizeof( signed_64 );
        return;
    }
    val = U32FetchTrunc( entry->v.uint );
    if( val > 0x7fffffff ) {
        entry->info.modifier = TM_UNSIGNED;
        entry->info.size = sizeof( unsigned_32 );
    } else if( DefaultSize( DK_INT ) > 2 ) {
        entry->info.modifier = TM_SIGNED;
        entry->info.size = sizeof( signed_32 );
    } else if( val > 0xffff ) {
        entry->info.modifier = TM_SIGNED;
        entry->info.size = sizeof( signed_32 );
    } else if( val > 0x7fff ) {
        entry->info.modifier = TM_UNSIGNED;
        entry->info.size = sizeof( unsigned_16 );
    } else {
        entry->info.modifier = TM_SIGNED;
        entry->info.size = sizeof( signed_16 );
    }
}

static bool AddressExpression( stack_entry *entry )
{
    if( !(entry->flags & SF_LOCATION) ) return( false );
    if( !(entry->flags & SF_IMP_ADDR) ) return( false );
    if( ExprAddrDepth != 0 ) return( false );
    /* doing an address expression (unnested) */
    entry->th = NULL;
    LocationToAddr( entry );
    return( true );
}

static bool DoNameResolve( stack_entry *entry )
{
    unsigned_64 val;

    if( NameResolve( entry, false ) ) return( true );
    /* check for raw name */
    if( entry->lc->sh != NULL ) return( false );
    if( entry->lc->th != NULL ) return( false );
    if( entry->v.li.mod != NO_MOD ) return( false );
    if( entry->v.li.file_scope ) return( false );
    if( entry->v.li.type != ST_NONE ) return( false );
    if( entry->v.li.scope.start != NULL ) return( false );

    if( !ForceSym2Num( entry->v.li.name.start, entry->v.li.name.len, &val ) )
        return( false );
    entry->v.uint = val;
    ClassNum( entry );
    return( true );
}



void ExprResolve( stack_entry *entry )
{
    if( entry->flags & SF_NAME ) {
        if( !DoNameResolve( entry ) ) {
            Error( ERR_NONE, LIT_ENG( ERR_UNKNOWN_SYMBOL ), entry->v.li.name.start,
                                       entry->v.li.name.len );
        }
    }
    SymResolve( entry );
}

static bool IsCodePointer( stack_entry *entry )
{
    DIPHDL( type, base_th );
    dip_type_info   ti;

    if( entry->th == NULL ) return( false );
    if( TypeBase( entry->th, base_th, NULL, NULL ) != DS_OK ) return( false );
    if( TypeInfo( base_th, entry->lc, &ti ) != DS_OK ) return( false );
    switch( ti.kind ) {
    case TK_CODE:
    case TK_FUNCTION:
        return( true );
    }
    return( false );
}

static void NearToFar( stack_entry *entry )
{
    addr_off    near_off;

    near_off = entry->v.addr.mach.offset;
    if( entry->th == NULL
      || TypePtrAddrSpace( entry->th, entry->lc, &entry->v.addr ) != DS_OK ) {
        if( near_off == 0 ) {
            entry->v.addr = NilAddr;
        } else if( IsCodePointer( entry ) ) {
            entry->v.addr = Context.execution;
        } else if( entry->th != NULL ) {
            entry->v.addr = DefAddrSpaceForMod( TypeMod( entry->th ) );
        } else {
            entry->v.addr = DefAddrSpaceForAddr( Context.execution );
        }
        entry->v.addr.mach.offset = 0;
    } else {
        entry->info.modifier = TM_FAR;
        entry->info.size += sizeof( addr_seg );
    }
    entry->v.addr.mach.offset += near_off;
    if( entry->th == NULL ) {
        entry->info.kind = TK_ADDRESS;
    }
}

void LRValue( stack_entry *entry )
{
    location_list       ll;
    item_mach           tmp;
    bool                extend;
    mad_type_info       mti;
    DIPHDL( type, th );

    ExprResolve( entry );
    if( entry->flags & SF_LOCATION ) {
        if( entry->info.kind == TK_FUNCTION || entry->info.kind == TK_CODE ) {
            /* rvalue of procedure is its address */
            entry->v.addr = entry->v.loc.e[0].u.addr;
            ExprSetAddrInfo( entry, false );
            entry->info.kind = TK_ADDRESS;
        } else if( !AddressExpression( entry ) ) {
            extend = false;
            switch( entry->info.kind ) {
            case TK_ARRAY:
                /* rvalue of array is its address */
                entry->v.addr = entry->v.loc.e[0].u.addr;
                if( entry->th != NULL ) {
                    /* change typing from "array of..." to "pointer to..." */
                    GetMADTypeDefaultAt( entry->v.addr, MTK_ADDRESS, &mti );
                    TypeBase( entry->th, th, NULL, NULL );
                    TypePointer( th, TM_FAR, BITS2BYTES( mti.b.bits ), entry->th );
                    TypeInfo( entry->th, entry->lc, &entry->info );
                } else {
                    ExprSetAddrInfo( entry, false );
                }
                break;
            case TK_STRING:
                _ChkAlloc( entry->v.string.allocated, entry->info.size,
                            LIT_ENG( ERR_NO_MEMORY_FOR_EXPR ) );
                LocationCreate( &ll, LT_INTERNAL, entry->v.string.allocated );
                if( LocationAssign( &ll, &entry->v.loc, entry->info.size, false ) != DS_OK ) {
                    _Free( entry->v.string.allocated );
                    Error( ERR_NONE, LIT_ENG( ERR_NO_ACCESS ) );
                }
                entry->v.string.loc = ll;
                break;
            case TK_BOOL:
            case TK_CHAR:
            case TK_ENUM:
            case TK_INTEGER:
                if( (entry->info.modifier & TM_MOD_MASK) == TM_SIGNED ) extend = true;
                /* fall through */
            default:
                LocationCreate( &ll, LT_INTERNAL, &tmp );
                if( LocationAssign( &ll, &entry->v.loc, entry->info.size, extend ) != DS_OK ) {
                    Error( ERR_NONE, LIT_ENG( ERR_NO_ACCESS ) );
                }
                FromItem( &tmp, entry );
                break;
            }
        }
        entry->flags &= ~(SF_LOCATION | SF_IMP_ADDR);
    }
    if( entry->info.kind == TK_POINTER
        && (entry->info.modifier & TM_MOD_MASK) == TM_NEAR ) {
        NearToFar( entry );
    }
}

void RValue( stack_entry *entry )
{
    ++ExprAddrDepth;
    LRValue( entry );
    --ExprAddrDepth;
}

void ExprValue( stack_entry *entry )
{
    ExprResolve( entry );
    AddressExpression( entry );
    switch( entry->info.kind ) {
    case TK_STRUCT:
    case TK_ARRAY:
        break;
    default:
        RValue( entry );
    }
}
