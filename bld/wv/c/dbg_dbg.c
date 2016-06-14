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
* Description:  Built-in dbg$ variables.
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbgdata.h"
#include "dipwv.h"
#include "dbgmem.h"
#include "dbglit.h"
#include "madinter.h"
#include "dui.h"
#include "dbgscan.h"
#include "dbgovl.h"
#include "dbg_dbg.h"
#include "remcore.h"
#include "dbgmisc.h"
#include "dipimp.h"
#include "dipinter.h"
#include "dbgreg.h"
#include "dbgdot.h"

#include "clibext.h"


#ifdef _OVERLAYED_
#define CONST
#else
#define CONST static const
#endif

#define WV_SYM_DEF(size) struct { fixed_wv_sym_entry s; unsigned char len, name[size]; }

#define WV_SYM( prfx, tk, tm, ts, sc, intrnl, vn, np ) \
    CONST WV_SYM_DEF( sizeof( np #vn ) - 1 ) wv ## prfx ## _ ## vn = {  \
        {                                       \
            { TK_ ## tk, TM_ ## tm, ts },       \
            SC_ ## sc,                          \
            { intrnl }                          \
        },                                      \
        sizeof( np #vn ) - 1,                   \
        { np #vn }                              \
    }

#define INTERNAL_SYM( n, tk, tm, ts ) \
    WV_SYM( INT, tk, tm, ts, INTERNAL, INTERNAL_ ## n, n, "dbg$" )

typedef int     cmp_func( const void *, const void *, size_t );

typedef struct wv_sym_list {
    struct wv_sym_list  *next;
    wv_sym_entry        s;
} wv_sym_list;

/*
 * Definitions
 */
#define pick(n,tk,tm,tt) INTERNAL_SYM( n, tk, tm, sizeof( tt ) );
#include "dipwvsym.h"
#undef pick

CONST wv_sym_entry *const ListInternal[] = {
    #define pick(n,tk,tm,tt) (const wv_sym_entry *)&wvINT_ ## n,
    #include "dipwvsym.h"
    #undef pick
    NULL
};

static wv_sym_list      *WmonSymLst;

static const wv_sym_entry   *StaticInternalLookup( lookup_item *li )
{
    cmp_func                    *cmp;
    const wv_sym_entry          *curr;
    const wv_sym_entry * const  *list;

    if( li->scope.start != NULL )
        return( NULL );
    if( li->case_sensitive ) {
        cmp = memcmp;
    } else {
        cmp = memicmp;
    }
    for( list = ListInternal; (curr = *list) != NULL; ++list ) {
        if( li->name.len == SYM_NAME_LEN( curr->name )
          && cmp( li->name.start, SYM_NAME_NAME( curr->name ), li->name.len ) == 0 ) {
            return( curr );
        }
    }
    return( NULL );
}

struct lookup_reg {
    const char          *name;
    unsigned            len;
    cmp_func            *cmp;
    const mad_reg_info  *ri;
};

static walk_result FindReg( const mad_reg_info *ri, int has_sublist, void *d )
{
    struct lookup_reg   *ld = d;

    has_sublist=has_sublist;
    if( memicmp( ld->name, ri->name, ld->len ) != 0 )
        return( WR_CONTINUE );
    if( ri->name[ld->len] != NULLCHAR )
        return( WR_CONTINUE );
    ld->ri = ri;
    return( WR_STOP );
}

const mad_reg_info *LookupRegName( const mad_reg_info *parent, lookup_item *li )
{
    struct lookup_reg   lr;

    if( li->scope.start != NULL )
        return( NULL );
    if( li->case_sensitive ) {
        lr.cmp = memcmp;
    } else {
        lr.cmp = memicmp;
    }
    lr.name = li->name.start;
    lr.len  = li->name.len;
    lr.ri   = NULL;
    MADRegWalk( NULL, parent, &FindReg, &lr );
    return( lr.ri );
}

const wv_sym_entry *LookupInternalName( lookup_item *li )
{
    const wv_sym_entry  *se;
    const char          *null_start;
    size_t              null_len;
    cmp_func            *cmp;

    se = StaticInternalLookup( li );
    if( se != NULL )
        return( se );
    if( TokenName( T_SSL_SPEC_NULL, &null_start, &null_len ) ) {
        ++null_start;
        --null_len;
        if( null_len != li->name.len )
            return( NULL );
        if( li->case_sensitive ) {
            cmp = memcmp;
        } else {
            cmp = memicmp;
        }
        if( cmp( null_start, li->name.start, null_len ) == 0 ) {
            return( (const wv_sym_entry *)&wvINT_NIL );
        }
    }
    return( NULL );
}

const wv_sym_entry *LookupUserName( lookup_item *li )
{
    wv_sym_list         *sl;
    cmp_func            *cmp;

    if( li->case_sensitive ) {
        cmp = memcmp;
    } else {
        cmp = memicmp;
    }
    for( sl = WmonSymLst; sl != NULL; sl = sl->next ) {
        if( li->name.len != SYM_NAME_LEN( sl->s.name ) )
            continue;
        if( cmp( li->name.start, SYM_NAME_NAME( sl->s.name ), li->name.len ) != 0 )
            continue;
        return( &sl->s );
    }
    return( NULL );
}

void PurgeUserNames( void )
{
    wv_sym_list *sl;

    while( (sl = WmonSymLst) != NULL ) {
        WmonSymLst = sl->next;
        if( sl->s.info.t.k == TK_STRING )
            _Free( sl->s.info.v.string );
        _Free( sl );
    }
}

/*
 * CreateSym -- create a symbol table entry allocated from dynamic storage
 */
bool CreateSym( lookup_item *li, dip_type_info *ti )
{
    wv_type_entry       info;
    wv_sym_list         *new;

    if( ( li->mod != NO_MOD ) && !IsInternalMod( li->mod ) )
        return( false );
    if( li->scope.start != NULL )
        return( false );
    info.k = ti->kind;
    info.m = ti->modifier;
    info.s = ti->size;
    switch( info.k ) {
    case TK_INTEGER:
    case TK_ENUM:
    case TK_CHAR:
        info.k = TK_INTEGER;
        info.s = sizeof( new->s.info.v.uint );
        break;
    case TK_REAL:
        info.s = sizeof( new->s.info.v.real );
        break;
    case TK_COMPLEX:
        info.s = sizeof( new->s.info.v.cmplx );
        break;
    case TK_ADDRESS:
    case TK_POINTER:
        info.k = TK_ADDRESS;
        info.m = TM_FAR;
        info.s = sizeof( new->s.info.v.addr.mach );
        break;
    default:
        return( false );
    }
    new = DbgMustAlloc( sizeof( *new ) + li->name.len );
    new->next = WmonSymLst;
    WmonSymLst = new;
    new->s.info.t = info;
    new->s.info.sc = SC_USER;
    SET_SYM_NAME_LEN( new->s.name, li->name.len );
    memcpy( SYM_NAME_NAME( new->s.name ), li->name.start, li->name.len );
    return( true );
}

static void GetNPXType( void )
{
    if( _IsOff( SW_KNOW_EMULATOR ) ) {
        GetSysConfig(); /* might be emulator */
        _SwitchOn( SW_KNOW_EMULATOR );
    }
}

void InternalValue( internal_idx index, void *d )
{
    mad_type_info       mti;

    switch( index ) {
    case INTERNAL_radix:
        *(unsigned_16 *)d = DefRadix;
        break;
    case INTERNAL_monitor:
        *(unsigned_16 *)d = DUIGetMonitorType();
        break;
    case INTERNAL_top:
        *(unsigned_16 *)d = 0;
        break;
    case INTERNAL_bottom:
        *(unsigned_16 *)d = DUIScreenSizeY();
        break;
    case INTERNAL_psp:
    case INTERNAL_pid:
        *(unsigned_16 *)d = TaskId;
        break;
    case INTERNAL_remote:
        *(unsigned_16 *)d = _IsOn( SW_REMOTE_FILES );
        break;
    case INTERNAL_code:
        *(address *)d = GetCodeDot();
        AddrFix( d );
        break;
    case INTERNAL_data:
        *(address *)d = GetDataDot();
        AddrFix( d );
        break;
    case INTERNAL_machine:
        *(unsigned_16 *)d = SysConfig.mad;
        break;
    case INTERNAL_cpu:
        *(unsigned_16 *)d = SysConfig.cpu;
        break;
    case INTERNAL_fpu:
        GetNPXType();
        *(signed_16 *)d = SysConfig.fpu;
        break;
    case INTERNAL_os:
        *(unsigned_16 *)d = SysConfig.os;
        break;
    case INTERNAL_32:
        GetMADTypeDefault( MTK_INTEGER, &mti );
        *(unsigned_16 *)d = ( mti.b.bits >= 32 );
        break;
    case INTERNAL_loaded:
        *(unsigned_16 *)d = _IsOff( SW_PROC_ALREADY_STARTED );
        break;
    case INTERNAL_left:
        *(unsigned_16 *)d = 0;
        break;
    case INTERNAL_right:
        *(unsigned_16 *)d = DUIScreenSizeX();
        break;
    case INTERNAL_etid:
        if( ExecThd != NULL ) {
            *(unsigned_16 *)d = ExecThd->tid;
            break;
        }
        /* fall through */
    case INTERNAL_ctid:
        *(unsigned_16 *)d = DbgRegs->tid;
        break;
    case INTERNAL_ntid:
        *(unsigned_16 *)d = GetNextTID();
        break;
    case INTERNAL_ip:
        *(address *)d = GetRegIP();
        AddrFix( d );
        break;
    case INTERNAL_sp:
        *(address *)d = GetRegSP();
        AddrFix( d );
        break;
    case INTERNAL_bp:
        *(address *)d = GetRegBP();
        AddrFix( d );
        break;
    case INTERNAL_WV_TNG:
        *(unsigned_16 *)d = 1;
        break;
    case INTERNAL_src:
        *(unsigned_16 *)d = ( ActiveWindowLevel == SOURCE );
        break;
    case INTERNAL_NIL:
        *(address *)d = NilAddr;
        break;
    case INTERNAL_kanji:
        *(unsigned_16 *)d = DUIIsDBCS();
        break;
    }
}
