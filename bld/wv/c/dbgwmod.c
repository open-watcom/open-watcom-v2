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


#include "dbgdefn.h"
#include "dbginfo.h"
#include "dbgwind.h"
#include "dbgadget.h"
#include "modlist.h"
#include "dbgio.h"
#include "dbgtoggl.h"
#include <string.h>
#include <stdlib.h>


extern char             *TxtBuff;
extern address          NilAddr;
extern mod_handle       ContextMod;

extern void             WndFuncInspect( mod_handle mod );
extern a_window         *WndSrcInspect(address);
extern void             WndFileInspect( char *file, bool binary );
extern a_window         *WndAsmInspect(address);
extern bool             OpenGadget( a_window *, wnd_line_piece *, mod_handle, bool );
extern bool             CheckOpenGadget( a_window*, wnd_row, bool, mod_handle, bool, int );
extern void             ClearAllModBreaks( mod_handle handle );
extern void             BreakAllModEntries( mod_handle handle );
extern a_window         *DoWndSrcOpen( cue_handle *, bool );
extern  bool            ModHasSourceInfo( mod_handle handle );
extern address          ModFirstAddr( mod_handle mod );
extern char             *ModImageName( mod_handle handle );

#include "menudef.h"
static gui_menu_struct ModMenu[] = {
    #include "menumod.h"
};

enum {
    PIECE_SOURCE,
    PIECE_MODULE,
    PIECE_IMAGE,
    PIECE_TABSTOP = PIECE_MODULE
};

typedef struct {
    unsigned    open : 1;
    unsigned    source : 1;
} modinfo;

typedef struct {
    module_list ___list;
    mod_handle  handle;
    unsigned    all_modules : 1;
    modinfo     *info;
    gui_ord     max_modlen;
} mod_window;

#define WndMod( wnd ) ( (mod_window*)WndExtra( wnd ) )
#define ModList( mod ) ( &((mod)->___list) )


static WNDNUMROWS ModNumRows;
static int ModNumRows( a_window *wnd )
{
    return( ModListNumRows( ModList( WndMod( wnd ) ) ) );
}

static void ModCalcIndent( a_window *wnd )
{
    gui_ord     extent,max_extent;
    int         i,size;
    mod_window  *mod = WndMod( wnd );

    size = ModListNumRows( ModList( mod ) );
    max_extent = 0;
    for( i = 0; i < size; ++i ) {
        ModListName( ModList( mod ), i, TxtBuff );
        extent = WndExtentX( wnd, TxtBuff );
        if( extent > max_extent ) max_extent = extent;
    }
    mod->max_modlen = max_extent + WndMidCharX( wnd );
    WndNoSelect( wnd );
    WndRepaint( wnd );
}

static void     ModInit( a_window *wnd )
{
    mod_window  *mod = WndMod( wnd );
    int         size;

    ModListFree( ModList( mod ) );
    ModListAddModules( ModList( mod ), mod->handle, mod->all_modules );
    WndFree( mod->info );
    mod->info = NULL;
    size = sizeof( modinfo ) * ModListNumRows( ModList( mod ) );
    if( size != 0 ) {
        mod->info = WndAlloc( size );
        memset( mod->info, 0, size );
    }
    ModCalcIndent( wnd );
}

static  WNDMENU ModMenuItem;
static void     ModMenuItem( a_window *wnd, unsigned id, int row, int piece )
{
    address     addr;
    mod_handle  handle;
    mod_window  *mod = WndMod( wnd );

    piece=piece;

    handle = NO_MOD;
    addr = NilAddr;
    if( ModListNumRows( ModList( mod ) ) != 0 && row >= 0 ) {
        handle =  ModListMod( ModList( mod ), row );
        if( handle != NO_MOD ) {
            addr = ModFirstAddr( handle );
        }
    }
    switch( id ) {
    case MENU_INITIALIZE:
        WndMenuGrayAll( wnd );
        if( handle != NO_MOD ) {
            WndMenuEnable( wnd, MENU_MODULES_BREAK_SET_ALL, TRUE );
            WndMenuEnable( wnd, MENU_MODULES_BREAK_CLEAR_ALL, TRUE );
            WndMenuEnable( wnd, MENU_MODULES_ASSEMBLY, !IS_NIL_ADDR( addr ) );
            WndMenuEnable( wnd, MENU_MODULES_FUNCTIONS, TRUE );
            if( ModHasSourceInfo( handle ) ) {
                WndMenuEnable( wnd, MENU_MODULES_SOURCE, TRUE );
            }
        }
        WndMenuEnable( wnd, MENU_MODULES_SHOW_ALL, TRUE );
        WndMenuCheck( wnd, MENU_MODULES_SHOW_ALL, mod->all_modules );
        break;
    case MENU_MODULES_BREAK_SET_ALL:
        BreakAllModEntries( handle );
        break;
    case MENU_MODULES_BREAK_CLEAR_ALL:
        ClearAllModBreaks( handle );
        break;
    case MENU_MODULES_SOURCE:
        if( IS_NIL_ADDR( addr ) ) {
            ModName( handle, TxtBuff, TXT_LEN );
            WndFileInspect( TxtBuff, FALSE );
        } else {
            WndSrcInspect( addr );
        }
        break;
    case MENU_MODULES_ASSEMBLY:
        WndAsmInspect( addr );
        break;
    case MENU_MODULES_FUNCTIONS:
        WndFuncInspect( handle );
        break;
    case MENU_MODULES_SHOW_ALL:
        mod->all_modules = !mod->all_modules;
        ModInit( wnd );
        break;
    }
}


static  WNDMODIFY       ModModify;
static void     ModModify( a_window *wnd, int row, int piece )
{
    if( piece == PIECE_SOURCE ) {
        if( ModHasSourceInfo( ModListMod( ModList( WndMod( wnd ) ), row ) ) ) {
            ModMenuItem( wnd, MENU_MODULES_SOURCE, row, piece );
        } else {
            ModMenuItem( wnd, MENU_MODULES_ASSEMBLY, row, piece );
        }
        WndDirtyCurr( wnd );
        WndNewCurrent( wnd, row, PIECE_TABSTOP );
    } else {
        WndFirstMenuItem( wnd, row, piece );
    }
}


static WNDGETLINE ModGetLine;
static  bool    ModGetLine( a_window *wnd, int row, int piece,
                             wnd_line_piece *line )
{
    mod_handle  handle;
    mod_window  *mod = WndMod( wnd );
    modinfo     *info;

    if( row >= ModListNumRows( ModList( mod ) ) ) return( FALSE );
    handle = ModListMod( ModList( mod ), row );
    line->tabstop = FALSE;
    switch( piece ) {
    case PIECE_SOURCE:
        info = &mod->info[ row ];
        info->source = ModHasSourceInfo( handle );
        info->open = OpenGadget( wnd, line, handle, info->source );
        return( TRUE );
    case PIECE_MODULE:
        line->indent = MaxGadgetLength;
        line->tabstop = TRUE;
        ModListName( ModList( mod ), row, TxtBuff );
        line->text = TxtBuff;
        return( TRUE );
    case PIECE_IMAGE:
        line->indent = mod->max_modlen + MaxGadgetLength;
        line->text = ModImageName( handle );
        return( TRUE );
    default:
        return( FALSE );
    }
}

static void ModSetCurrent( a_window *wnd )
{
    int         i;
    mod_window  *mod = WndMod( wnd );
    wnd_row     row;
    int         piece;

    WndGetCurrent( wnd, &row, &piece );
    if( row != WND_NO_ROW && ModListMod( ModList( mod ), row ) == ContextMod ) {
        return;
    }
    for( i = 0; i < ModListNumRows( ModList( mod ) ); ++i ) {
        if( ModListMod( ModList( mod ), i ) == ContextMod ) {
            WndMoveCurrent( wnd, i, PIECE_MODULE );
        }
    }
}

extern void ModNewHandle( a_window *wnd, mod_handle handle )
{
    mod_window  *mod = WndMod( wnd );

    if( mod->handle == handle ) return;
    mod->handle = handle;
    ModInit( wnd );
    ModSetCurrent( wnd );
}

static WNDREFRESH ModRefresh;
static void     ModRefresh( a_window *wnd )
{
    int         i;
    mod_window  *mod = WndMod( wnd );
    modinfo     *info;

    if( WndFlags & UP_SYM_CHANGE ) {
        ModInit( wnd );
    }
    if( WndFlags & UP_OPEN_CHANGE ) {
        for( i = 0; i < ModListNumRows( ModList( mod ) ); ++i ) {
            info = &mod->info[ i ];
            info->open = CheckOpenGadget( wnd, i, info->open,
                                          ModListMod( ModList( mod ), i ),
                                          info->source, PIECE_SOURCE );
        }
    }
    if( WndFlags & (UP_CSIP_CHANGE+UP_STACKPOS_CHANGE) ) {
        ModSetCurrent( wnd );
    }
}

static void ModSetOptions( a_window *wnd )
{
    WndMod( wnd )->all_modules = _IsOn( SW_MOD_ALL_MODULES );
    ModInit( wnd );
}

static WNDCALLBACK ModEventProc;
static bool ModEventProc( a_window * wnd, gui_event gui_ev, void *parm )
{
    mod_window  *mod = WndMod( wnd );

    parm=parm;
    switch( gui_ev ) {
    case GUI_INIT_WINDOW:
        ModListInit( ModList( mod ), NULL );
        mod->info = NULL;
        WndSetKey( wnd, PIECE_MODULE );
        ModSetOptions( wnd );
        ModSetCurrent( wnd );
        return( TRUE );
    case GUI_RESIZE:
        ModCalcIndent( wnd );
        return( TRUE );
    case GUI_DESTROY :
        ModListFree( ModList( mod ) );
        WndFree( mod->info );
        WndFree( mod );
        return( TRUE );
    }
    return( FALSE );
}

void ModChangeOptions()
{
    WndForAllClass( WND_MODULES, ModSetOptions );
}

wnd_info ModInfo = {
    ModEventProc,
    ModRefresh,
    ModGetLine,
    ModMenuItem,
    NoScroll,
    NoBegPaint,
    NoEndPaint,
    ModModify,
    ModNumRows,
    NoNextRow,
    NoNotify,
    UP_SYM_CHANGE+UP_OPEN_CHANGE+UP_CSIP_CHANGE+UP_STACKPOS_CHANGE,
    DefPopUp( ModMenu )
};

extern a_window *DoWndModOpen( mod_handle handle )
{
    mod_window  *mod;

    mod = WndMustAlloc( sizeof( mod_window ) );
    mod->handle = handle;
    return( DbgWndCreate( LIT( WindowModules ), &ModInfo, WND_MODULES, mod, &ModIcon ) );
}

extern WNDOPEN WndModOpen;
extern a_window *WndModOpen()
{
    return( DoWndModOpen( NO_MOD ) );
}
