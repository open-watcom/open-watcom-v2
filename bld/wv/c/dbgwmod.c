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
#include "dbgwind.h"
#include "dbgadget.h"
#include "modlist.h"
#include "dbgio.h"
#include "dbgutil.h"
#include "dbgbrk.h"
#include "wndsys.h"
#include "dbgwfil.h"
#include "dbgwglob.h"
#include "dbgwinsp.h"
#include "dbgwmod.h"


extern  bool            ModHasSourceInfo( mod_handle handle );

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
    BITB        open   : 1;
    BITB        source : 1;
} modinfo;

typedef struct {
    module_list ___list;
    mod_handle  handle;
    modinfo     *info;
    gui_ord     max_modlen;
    BITB        all_modules : 1;
} mod_window;

#define WndMod( wnd ) ( (mod_window*)WndExtra( wnd ) )
#define ModList( mod ) ( &((mod)->___list) )


static int ModNumRows( a_window *wnd )
{
    return( ModListNumRows( ModList( WndMod( wnd ) ) ) );
}

static void ModCalcIndent( a_window *wnd )
{
    gui_ord     extent, max_extent;
    int         i, size;
    mod_window  *mod = WndMod( wnd );

    size = ModListNumRows( ModList( mod ) );
    max_extent = 0;
    for( i = 0; i < size; ++i ) {
        ModListName( ModList( mod ), i, TxtBuff );
        extent = WndExtentX( wnd, TxtBuff );
        if( max_extent < extent ) {
            max_extent = extent;
        }
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

static void     ModMenuItem( a_window *wnd, gui_ctl_id id, int row, int piece )
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
            WndMenuEnable( wnd, MENU_MODULES_BREAK_SET_ALL, true );
            WndMenuEnable( wnd, MENU_MODULES_BREAK_CLEAR_ALL, true );
            WndMenuEnable( wnd, MENU_MODULES_ASSEMBLY, !IS_NIL_ADDR( addr ) );
            WndMenuEnable( wnd, MENU_MODULES_FUNCTIONS, true );
            if( ModHasSourceInfo( handle ) ) {
                WndMenuEnable( wnd, MENU_MODULES_SOURCE, true );
            }
        }
        WndMenuEnable( wnd, MENU_MODULES_SHOW_ALL, true );
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
            DIPModName( handle, TxtBuff, TXT_LEN );
            WndFileInspect( TxtBuff, false );
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


static  bool    ModGetLine( a_window *wnd, int row, int piece,
                             wnd_line_piece *line )
{
    mod_handle  handle;
    mod_window  *mod = WndMod( wnd );
    modinfo     *info;

    if( row >= ModListNumRows( ModList( mod ) ) )
        return( false );
    handle = ModListMod( ModList( mod ), row );
    line->tabstop = false;
    switch( piece ) {
    case PIECE_SOURCE:
        info = &mod->info[row];
        info->source = ModHasSourceInfo( handle );
        info->open = OpenGadget( wnd, line, handle, info->source );
        return( true );
    case PIECE_MODULE:
        line->indent = MaxGadgetLength;
        line->tabstop = true;
        ModListName( ModList( mod ), row, TxtBuff );
        line->text = TxtBuff;
        return( true );
    case PIECE_IMAGE:
        line->indent = mod->max_modlen + MaxGadgetLength;
        line->text = ModImageName( handle );
        return( true );
    default:
        return( false );
    }
}

static void ModSetCurrent( a_window *wnd )
{
    int         i;
    mod_window  *mod = WndMod( wnd );
    wnd_row     curr_row;
    int         curr_piece;

    WndGetCurrent( wnd, &curr_row, &curr_piece );
    if( curr_row != WND_NO_ROW && ModListMod( ModList( mod ), curr_row ) == ContextMod ) {
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

    if( mod->handle == handle )
        return;
    mod->handle = handle;
    ModInit( wnd );
    ModSetCurrent( wnd );
}

static void     ModRefresh( a_window *wnd )
{
    int         i;
    mod_window  *mod = WndMod( wnd );
    modinfo     *info;

    if( UpdateFlags & UP_SYM_CHANGE ) {
        ModInit( wnd );
    }
    if( UpdateFlags & UP_OPEN_CHANGE ) {
        for( i = 0; i < ModListNumRows( ModList( mod ) ); ++i ) {
            info = &mod->info[i];
            info->open = CheckOpenGadget( wnd, i, info->open,
                                          ModListMod( ModList( mod ), i ),
                                          info->source, PIECE_SOURCE );
        }
    }
    if( UpdateFlags & (UP_CSIP_CHANGE+UP_STACKPOS_CHANGE) ) {
        ModSetCurrent( wnd );
    }
}

static void ModSetOptions( a_window *wnd )
{
    WndMod( wnd )->all_modules = _IsOn( SW_MOD_ALL_MODULES );
    ModInit( wnd );
}

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
        return( true );
    case GUI_RESIZE:
        ModCalcIndent( wnd );
        return( true );
    case GUI_DESTROY :
        ModListFree( ModList( mod ) );
        WndFree( mod->info );
        WndFree( mod );
        return( true );
    }
    return( false );
}

void ModChangeOptions( void )
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
    ChkFlags,
    UP_SYM_CHANGE+UP_OPEN_CHANGE+UP_CSIP_CHANGE+UP_STACKPOS_CHANGE,
    DefPopUp( ModMenu )
};

extern a_window *DoWndModOpen( mod_handle handle )
{
    mod_window  *mod;

    mod = WndMustAlloc( sizeof( mod_window ) );
    mod->handle = handle;
    return( DbgWndCreate( LIT_DUI( WindowModules ), &ModInfo, WND_MODULES, mod, &ModIcon ) );
}

extern a_window *WndModOpen( void )
{
    return( DoWndModOpen( NO_MOD ) );
}
