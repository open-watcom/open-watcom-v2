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


typedef int     cmp_func( const void *, const void *, size_t );

typedef struct wv_sym_list      wv_sym_list;
struct wv_sym_list {
    wv_sym_list         *next;
    wv_sym_entry        s;
};

static wv_sym_list      *WmonSymLst;

enum {
    INTERNAL_radix,
    INTERNAL_monitor,
    INTERNAL_top,
    INTERNAL_bottom,
    INTERNAL_psp,
    INTERNAL_pid,
    INTERNAL_remote,
    INTERNAL_code,
    INTERNAL_data,
    INTERNAL_machine,
    INTERNAL_cpu,
    INTERNAL_fpu,
    INTERNAL_os,
    INTERNAL_32,
    INTERNAL_left,
    INTERNAL_right,
    INTERNAL_etid,
    INTERNAL_ctid,
    INTERNAL_ntid,
    INTERNAL_ip,
    INTERNAL_sp,
    INTERNAL_bp,
    INTERNAL_loaded,
    INTERNAL_WV_TNG,
    INTERNAL_NIL,
    INTERNAL_src,
    INTERNAL_kanji,
};


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


#define INTERNAL_SYM( tk, tm, s, n ) \
    WV_SYM( INT, tk, tm, s, INTERNAL, INTERNAL_ ## n, n, "dbg$" )

#define INTEGER_SYM( name ) \
    INTERNAL_SYM( INTEGER, UNSIGNED, sizeof( unsigned_16 ), name )

#define ADDRESS_SYM( name ) \
    INTERNAL_SYM( ADDRESS, FAR, sizeof( address ), name )

/*
 * Definitions
 */
INTEGER_SYM( radix );
INTEGER_SYM( monitor );
INTEGER_SYM( top );
INTEGER_SYM( bottom );
INTEGER_SYM( psp );
INTEGER_SYM( pid );
INTEGER_SYM( remote );
INTEGER_SYM( machine );
INTEGER_SYM( cpu );
INTEGER_SYM( fpu );
INTEGER_SYM( os );
INTEGER_SYM( 32 );
INTEGER_SYM( left );
INTEGER_SYM( right );
INTEGER_SYM( etid );
INTEGER_SYM( ctid );
INTEGER_SYM( ntid );
INTEGER_SYM( loaded );
INTEGER_SYM( WV_TNG );
INTEGER_SYM( src );
INTEGER_SYM( kanji );

ADDRESS_SYM( code );
ADDRESS_SYM( data );
ADDRESS_SYM( ip );
ADDRESS_SYM( sp );
ADDRESS_SYM( bp );
ADDRESS_SYM( NIL );

CONST wv_sym_entry *const ListInternal[] = {
    (wv_sym_entry *)&wvINT_radix,
    (wv_sym_entry *)&wvINT_monitor,
    (wv_sym_entry *)&wvINT_top,
    (wv_sym_entry *)&wvINT_bottom,
    (wv_sym_entry *)&wvINT_psp,
    (wv_sym_entry *)&wvINT_pid,
    (wv_sym_entry *)&wvINT_remote,
    (wv_sym_entry *)&wvINT_code,
    (wv_sym_entry *)&wvINT_data,
    (wv_sym_entry *)&wvINT_machine,
    (wv_sym_entry *)&wvINT_cpu,
    (wv_sym_entry *)&wvINT_fpu,
    (wv_sym_entry *)&wvINT_os,
    (wv_sym_entry *)&wvINT_32,
    (wv_sym_entry *)&wvINT_left,
    (wv_sym_entry *)&wvINT_right,
    (wv_sym_entry *)&wvINT_etid,
    (wv_sym_entry *)&wvINT_ctid,
    (wv_sym_entry *)&wvINT_ntid,
    (wv_sym_entry *)&wvINT_ip,
    (wv_sym_entry *)&wvINT_sp,
    (wv_sym_entry *)&wvINT_bp,
    (wv_sym_entry *)&wvINT_loaded,
    (wv_sym_entry *)&wvINT_WV_TNG,
    (wv_sym_entry *)&wvINT_src,
    (wv_sym_entry *)&wvINT_kanji,
    NULL
};

static wv_sym_entry *StaticLookup( const wv_sym_entry * const *list, lookup_item *li )
{
    cmp_func            *cmp;
    wv_sym_entry        *curr;

    if( li->scope.start != NULL )
        return( FALSE );
    if( li->case_sensitive ) {
        cmp = memcmp;
    } else {
        cmp = memicmp;
    }
    for( ; (curr = (wv_sym_entry *)*list) != NULL; ++list ) {
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

    if( memicmp( ld->name, ri->name, ld->len ) != 0 )
        return( WR_CONTINUE );
    if( ri->name[ld->len] != '\0' )
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

wv_sym_entry *LookupInternalName( lookup_item *li )
{
    wv_sym_entry    *se;
    const char      *null_start;
    unsigned        null_len;
    cmp_func        *cmp;

    se = StaticLookup( ListInternal, li );
    if( se != NULL )
        return( se );
    if( TokenName( TSTR_NULL, &null_start, &null_len ) ) {
        ++null_start;
        --null_len;
        if( null_len != li->name.len )
            return( NULL );
        if( li->case_sensitive ) {
            cmp = memcmp;
        } else {
            cmp = memicmp;
        }
        if( cmp( null_start, li->name.start, null_len ) != 0 )
            return( NULL );
        return( (wv_sym_entry *)&wvINT_NIL );
    }
    return( NULL );
}

wv_sym_entry *LookupUserName( lookup_item *li )
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
        if( sl->s.t.k == TK_STRING )
            _Free( sl->s.v.string );
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

    if( (li->mod != NO_MOD) && !IsInternalMod( li->mod ) )
        return( FALSE );
    if( li->scope.start != NULL )
        return( FALSE );
    info.k = ti->kind;
    info.m = ti->modifier;
    info.s = ti->size;
    switch( info.k ) {
    case TK_INTEGER:
    case TK_ENUM:
    case TK_CHAR:
        info.k = TK_INTEGER;
        info.s = sizeof( new->s.v.uint );
        break;
    case TK_REAL:
        info.s = sizeof( new->s.v.real );
        break;
    case TK_COMPLEX:
        info.s = sizeof( new->s.v.cmplx );
        break;
    case TK_ADDRESS:
    case TK_POINTER:
        info.k = TK_ADDRESS;
        info.m = TM_FAR;
        info.s = sizeof( new->s.v.addr.mach );
        break;
    default:
        return( FALSE );
    }
    new = DbgMustAlloc( sizeof( *new ) + li->name.len );
    new->next = WmonSymLst;
    WmonSymLst = new;
    new->s.t = info;
    new->s.sc = SC_USER;
    SET_SYM_NAME_LEN( new->s.name, li->name.len );
    memcpy( SYM_NAME_NAME( new->s.name ), li->name.start, li->name.len );
    return( TRUE );
}

static void GetNPXType( void )
{
    if( _IsOff( SW_KNOW_EMULATOR ) ) {
        GetSysConfig(); /* might be emulator */
        _SwitchOn( SW_KNOW_EMULATOR );
    }
}

void InternalValue( unsigned index, void *d )
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
        *(unsigned_16 *)d = (mti.b.bits >= 32);
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
        *(unsigned_16 *)d = ActiveWindowLevel == SOURCE;
        break;
    case INTERNAL_NIL:
        *(address *)d = NilAddr;
        break;
    case INTERNAL_kanji:
        *(unsigned_16 *)d = DUIIsDBCS();
        break;
    }
}
