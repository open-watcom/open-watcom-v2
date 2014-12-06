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
#include "dbgerr.h"
#include "namelist.h"


extern brkp             *FindBreak(address);
extern void             *AddBreak(address );
extern void             RemoveBreak(address );
extern void             GoToAddr( address addr );
extern int              HasLinInfo( address );
extern void             WndVarInspect( char *);
extern void             WndAddrInspect(address);


#include "menudef.h"
gui_menu_struct GlobMenu[] = {
    #include "menuglob.h"
};

typedef struct {
    name_list           ___n;           // don't reference directly!
    mod_handle          mod;
    unsigned            d2_only : 1;
} glob_window;

#define WndGlob( wnd ) ( (glob_window*)WndExtra( wnd ) )
#define NameList( f ) ( &(f)->___n )

enum {
    PIECE_NAME,
};

static  void    GlobInit( a_window *wnd )
{
    glob_window *glob = WndGlob( wnd );

    WndScrollAbs( wnd, 0 );
    NameListFree( NameList( glob ) );
    WndZapped( wnd );
    NameListAddModules( NameList( glob ), glob->mod, glob->d2_only, TRUE );
    WndSetKey( wnd, PIECE_NAME );
}

extern  WNDMENU GlobMenuItem;
extern void     GlobMenuItem( a_window *wnd, unsigned id, int row, int piece )
{
    glob_window *glob = WndGlob( wnd );
    address     addr;
    char        buff[TXT_LEN];

    piece=piece;
    addr = NameListAddr( NameList( glob ), row );
    switch( id ) {
    case MENU_INITIALIZE:
        if( IS_NIL_ADDR( addr ) ) {
            WndMenuGrayAll( wnd );
        } else {
            WndMenuEnableAll( wnd );
        }
        WndMenuEnable( wnd, MENU_GLOB_D2_ONLY, TRUE );
        WndMenuCheck( wnd, MENU_GLOB_D2_ONLY, glob->d2_only );
        break;
    case MENU_GLOB_INSPECT_MEMORY:
        WndAddrInspect( addr );
        break;
    case MENU_GLOB_WATCH:
        NameListName( NameList( glob ), row, buff, SN_QUALIFIED );
        WndVarInspect( buff );
        break;
    case MENU_GLOB_D2_ONLY:
        glob->d2_only = !glob->d2_only;
        GlobInit( wnd );
        break;
    default:
        break;
    }
}


extern WNDNUMROWS GlobNumRows;
extern int GlobNumRows( a_window *wnd )
{
    return( NameListNumRows( NameList( WndGlob( wnd ) ) ) );
}

extern WNDGETLINE GlobGetLine;
extern  bool    GlobGetLine( a_window *wnd, int row, int piece,
                             wnd_line_piece *line )
{
    glob_window *glob = WndGlob( wnd );

    if( row >= NameListNumRows( NameList( glob ) ) ) return( FALSE );
    switch( piece ) {
    case PIECE_NAME:
        NameListName( NameList( glob ), row, TxtBuff, SN_QUALIFIED );
        line->text = TxtBuff;
        return( TRUE );
    default:
        return( FALSE );
    }
}

void GlobNewMod( a_window *wnd, mod_handle mod )
{
    glob_window *glob = WndGlob( wnd );

    if( glob->mod == mod ) return;
    glob->mod = mod;
    GlobInit( wnd );
}


extern WNDREFRESH GlobRefresh;
void    GlobRefresh( a_window *wnd )
{
    if( UpdateFlags & UP_SYM_CHANGE ) {
        GlobInit( wnd );
        WndZapped( wnd );
    }
}


static void GlobSetOptions( a_window *wnd )
{
    WndGlob( wnd )->d2_only = _IsOn( SW_GLOB_D2_ONLY );
    GlobInit( wnd );
}

extern WNDCALLBACK GlobEventProc;
bool GlobEventProc( a_window * wnd, gui_event gui_ev, void *parm )
{
    glob_window *glob = WndGlob( wnd );

    parm=parm;
    switch( gui_ev ) {
    case GUI_INIT_WINDOW:
        NameListInit( NameList( glob ), WF_DATA );
        GlobSetOptions( wnd );
        return( TRUE );
    case GUI_DESTROY :
        NameListFree( NameList( glob ) );
        WndFree( glob );
        return( TRUE );
    }
    return( FALSE );
}

void GlobChangeOptions()
{
    WndForAllClass( WND_GLOBALS, GlobSetOptions );
}

wnd_info GlobInfo = {
    GlobEventProc,
    GlobRefresh,
    GlobGetLine,
    GlobMenuItem,
    NoScroll,
    NoBegPaint,
    NoEndPaint,
    WndFirstMenuItem,
    GlobNumRows,
    NoNextRow,
    NoNotify,
    ChkFlags,
    UP_SYM_CHANGE,
    DefPopUp( GlobMenu )
};

extern a_window *DoWndGlobOpen( mod_handle mod )
{
    glob_window *glob;

    glob = WndMustAlloc( sizeof( glob_window ) );
    glob->mod = mod;
    return( DbgWndCreate( LIT( WindowGlobals ), &GlobInfo, WND_GLOBALS, glob, &GlobIcon ) );
}

extern WNDOPEN WndGlobOpen;
extern a_window *WndGlobOpen()
{
    return( DoWndGlobOpen( NO_MOD ) );
}


bool ChkFlags( wnd_update_list flags )
{
    return( flags & UpdateFlags );
}
