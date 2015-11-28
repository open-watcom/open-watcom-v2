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
* Description:  Inspect a symbol.
*
****************************************************************************/



#include <ctype.h>
#include "_srcmgt.h"
#include "dbgdata.h"
#include "dbgstk.h"
#include "dbgerr.h"
#include "dbgwind.h"
#include "dbgitem.h"
#include "mad.h"
#include "srcmgt.h"
#include "dbgutil.h"
#include "dbgsrc.h"
#include "dbgexpr.h"
#include "dbgbrk.h"
#include "dbgparse.h"
#include "wndsys.h"
#include "dbgtrace.h"
#include "modlist.h"
#include "dipimp.h"
#include "dipinter.h"
#include "dbginsty.h"
#include "dbglkup.h"
#include "dbgwass.h"
#include "dbgwfil.h"
#include "dbgwfunc.h"
#include "dbgwglob.h"
#include "dbgwinsp.h"
#include "dbgwio.h"
#include "dbgwmem.h"
#include "dbgwmod.h"
#include "dbgwvar.h"

#include "clibext.h"

search_result           LineCue( mod_handle, cue_fileid,
                          unsigned long line, unsigned column, cue_handle *ch );
extern unsigned         CueFile( cue_handle *ch, char *file, unsigned max );
extern unsigned long    CueLine( cue_handle *ch );
extern void             MemNewAddr( a_window *wnd, address addr );

extern stack_entry      *ExprSP;
extern WNDOPEN          *WndOpenTab[];


static a_window *WndFindExisting( wnd_class wndcls )
{
    a_window    *wnd;

    wnd = WndFindClass( NULL, wndcls );
    if( wnd != NULL ) {
        WndRestoreToFront( wnd );
    }
    return( wnd );
}


extern void WndTmpFileInspect( const char *file )
{
    void                *viewhndl;

    viewhndl = FOpenSource( file, NO_MOD, 0 );
    DoWndFileOpen( file, viewhndl, NULL, false, true, WND_TMPFILE );
}

typedef struct cue_mod {
    const char  *file;
    mod_handle  mod;
    bool        found;
} cue_mod;

static walk_result CheckFirstFile( cue_handle *ch, void *_d )
{
    cue_mod     *d = _d;
    char        *buff;
    unsigned    len;

    len = CueFile( ch, NULL, 0 ) + 1;
    _AllocA( buff, len );
    CueFile( ch, buff, len );
    if( stricmp( buff, d->file ) == 0 ) {
        d->found = true;
    }
    return( WR_STOP );
}

static walk_result CheckOneMod( mod_handle mh, void *_d )
{
    cue_mod     *d = _d;
    d->mod = mh;
    if( ModHasInfo( mh, HK_CUE ) == DS_OK ) {
        WalkFileList( mh, CheckFirstFile, d );
    }
    return( d->found ? WR_STOP : WR_CONTINUE );
}


static mod_handle       FindFileMod( const char *file )
{
    cue_mod     d;

    d.found = false;
    d.file = file;
    WalkModList( NO_MOD, CheckOneMod, &d );
    return( d.found ? d.mod : NO_MOD );
}

extern void WndFileInspect( const char *file, bool binary )
{
    handle              filehndl;
    void                *viewhndl;
    mod_handle          mod;
    DIPHDL( cue, ch );

    viewhndl = NULL;
    if( binary ) {
        filehndl = FileOpen( file, OP_READ );
        if( filehndl == NIL_HANDLE ) Error( ERR_NONE, LIT_ENG( ERR_FILE_NOT_OPEN ), file );
        DoWndBinOpen( file, filehndl );
    } else {
        mod = LookupModName( NO_MOD, file, strlen( file ) );
        if( mod == NO_MOD ) {
            mod = FindFileMod( file );
        }
        if( FindFirstCue( mod, ch ) ) {
            viewhndl = OpenSrcFile( ch );
        } else {
            viewhndl = FOpenSource( file, NO_MOD, 0 );
            ch = NULL;
        }
        if( viewhndl == NULL ) Error( ERR_NONE, LIT_ENG( ERR_FILE_NOT_OPEN ), file );
        DoWndFileOpen( file, viewhndl, ch, false, false, WND_FILE );
    }
}


extern  void    WndFuncInspect( mod_handle mod )
{
    a_window    *wnd;

    wnd = WndFindExisting( WND_FUNCTIONS );
    if( wnd != NULL ) {
        FuncNewMod( wnd, mod );
    } else {
        DoWndFuncOpen( false, mod );
    }
}

extern  void    WndGblFuncInspect( mod_handle mod )
{
    a_window    *wnd;

    wnd = WndFindExisting( WND_GBLFUNCTIONS );
    if( wnd != NULL ) {
        FuncNewMod( wnd, mod );
    } else {
        DoWndFuncOpen( true, mod );
    }
}

extern  void    WndGblVarInspect( mod_handle mod )
{
    a_window    *wnd;

    wnd = WndFindExisting( WND_GLOBALS );
    if( wnd != NULL ) {
        GlobNewMod( wnd, mod );
    } else {
        DoWndGlobOpen( mod );
    }
}

extern  void    WndMemInspect( address addr, char *next,
                               unsigned len, mad_type_handle type )
{
    a_window    *wnd;

    wnd = DoWndMemOpen( addr, type );
    if( next != NULL ) MemSetFollow( wnd, next );
    if( len != 0 ) MemSetLength( wnd, len );
}

extern  void    WndIOInspect( address *addr, mad_type_handle type )
{
    a_window    *wnd;

    wnd = WndFindExisting( WND_IO );
    if( wnd == NULL ) {
        wnd = DoWndIOOpen( addr, type );
    } else {
        IONewAddr( wnd, addr, type );
    }
}

extern  void    WndAddrInspect( address addr )
{
    WndMemInspect( addr, NULL, 0, MAD_NIL_TYPE_HANDLE );
}

extern  a_window        *WndAsmInspect( address addr )
{
    a_window    *wnd;
    bool        nil;

    nil = false;
    if( IS_NIL_ADDR( addr ) ) {
        nil = true;
        addr = Context.execution;
    }
    wnd = WndFindExisting( WND_ASSEMBLY );
    if( nil && wnd != NULL ) return( wnd );
    if( wnd == NULL ) {
        wnd = DoWndAsmOpen( addr, true );
    }
    AsmMoveDot( wnd, addr );
    return( wnd );
}

static  a_window        *DoWndSrcInspect( address addr, bool existing )
{
    a_window    *wnd;
//    bool        nil;
    mod_handle  mod;
    DIPHDL( cue, ch );
    a_window    *active;

    active = WndFindActive();
//    nil = false;
    if( IS_NIL_ADDR( addr ) ) {
//        nil = true;
        addr = Context.execution;
    }
    wnd = NULL;
    if( existing ) {
        wnd = WndFindExisting( WND_SOURCE );
    }
    if( wnd == NULL ) {
        if( DeAliasAddrMod( addr, &mod ) == SR_NONE
         || DeAliasAddrCue( mod, addr, ch ) == SR_NONE ) {
            ch = NULL;
        }
        wnd = DoWndSrcOpen( ch, existing );
    }
    if( !SrcMoveDot( wnd, addr ) || !SrcHasFileOpen( wnd ) ) {
        if( active != NULL ) WndRestoreToFront( active );
        return( NULL );
    }
    return( wnd );
}

extern  a_window        *WndSrcInspect( address addr )
{
    a_window    *wnd;
    wnd = DoWndSrcInspect( addr, true );
    if( wnd == NULL )
        Warn( LIT_DUI( WARN_Source_Not_Available ) );
    return( wnd );
}

extern  a_window        *WndSrcOrAsmInspect( address addr )
{
    a_window    *wnd;

    wnd = DoWndSrcInspect( addr, true );
    if( wnd != NULL ) return( wnd );
    wnd = WndAsmInspect( addr );
    return( wnd );
}

extern  a_window        *WndNewSrcInspect( address addr )
{
    return( DoWndSrcInspect( addr, false ) );
}

extern  a_window        *WndModInspect( mod_handle mod )
{
    a_window    *wnd;
    DIPHDL( cue, ch );

    wnd = WndFindExisting( WND_SOURCE );
    if( wnd == NULL && LineCue( mod, 0, 0, 0, ch ) != SR_NONE ) {
        wnd = DoWndSrcOpen( ch, true );
    }
    if( wnd != NULL ) {
        SrcMoveDot( wnd, ModFirstAddr( mod ) );
    }
    return( wnd );
}

extern  void    WndModListInspect( mod_handle mod )
{
    a_window    *wnd;

    wnd = WndFindExisting( WND_MODULES );
    if( wnd == NULL ) {
        wnd = DoWndModOpen( mod );
    } else {
        ModNewHandle( wnd, mod );
    }
}

static a_window *WndVarNewWindow( const char *item )
{
    a_window    *wnd;

    WndSetOpenNoShow();
    wnd = WndVarOpen();
    WndSetTitle( wnd, item );
    WndVarAdd( wnd, item, strlen( item ), true );
    WndFirstCurrent( wnd );
    WndForcePaint( wnd );
    WndShrinkToMouse( wnd, WndMetrics[ WND_WATCH ] );
    WndShowWindow( wnd );
    return( wnd );
}


static bool WndDoInspect( const char *item, address *paddr, inspect_type t )
{
    switch( t ) {
    case INSP_CODE:
        WndSrcOrAsmInspect( *paddr );
        return( true );
    case INSP_DATA:
        WndVarNewWindow( item );
        return( true );
    case INSP_RAW_DATA:
        WndAddrInspect( *paddr );
        return( true );
    }
    return( false );
}

extern  void    WndInspectExprSP( const char *item )
{
    address     addr;

    WndDoInspect( item, &addr, WndGetExprSPInspectType( &addr ) );
}

extern  void    WndInspect( const char *item )
{
/* #####  E1052: Expression has void type */
/* #####  E1081: Expression must be scalar type */
    if( WndEvalInspectExpr( item, false ) ) {
        WndInspectExprSP( item );
    }
}

a_window *WndClassInspect( wnd_class wndcls )
{
    a_window    *wnd;

    wnd = WndFindExisting( wndcls );
    if( wnd == NULL ) {
        wnd = WndOpenTab[wndcls]();
    }
    return( wnd );
}


extern void WndVarInspect( const char *buff )
{
    a_window    *wnd;

    wnd = WndClassInspect( WND_WATCH );
    if( wnd != NULL ) {
        WndVarAdd( wnd, buff, strlen( buff ), false );
        WndScrollBottom( wnd );
    }
}
