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


#include <windows.h>
#include <string.h>
#include <limits.h>
#include "wdeglbl.h"
#include "wdestat.h"
#include "wdemain.h"
#include "wdemem.h"
#include "wdemsgbx.h"
#include "wdemsgs.h"
#include "wdehints.h"
#include "wdelist.h"
#include "wdetoolb.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define OUTLINE_AMOUNT    4
#define WDE_TOOL_BORDER_X 1
#define WDE_TOOL_BORDER_Y 1

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
extern BOOL WdeToolBarHelpHook  ( HWND hwnd, WPARAM wParam, BOOL pressed );

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static WdeToolBar *WdeFindToolBar         ( HWND );
static WdeToolBar *WdeAllocToolBar        ( void );
static void        WdeAddToolBar          ( WdeToolBar * );
static void        WdeRemoveToolBar       ( WdeToolBar * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static LIST       *WdeToolBarList      = NULL;

WdeToolBar *WdeCreateToolBar( WdeToolBarInfo *info, HWND parent )
{
    WdeToolBar  *tbar;
    int          i;
    int          width;
    int          height;
    HMENU        sys_menu;
    char        *text;

    if( !info ) {
        return( NULL );
    }

    tbar = WdeAllocToolBar();
    if( tbar == NULL ) {
        return( NULL );
    }

    tbar->last_pos = info->dinfo.area;
    tbar->info     = info;
    tbar->parent   = parent;
    tbar->tbar     = (toolbar) ToolBarInit( parent );

    ToolBarDisplay( tbar->tbar, &info->dinfo );

    for( i = 0; i < info->num_items; i++ ) {
        if( info->items[i].bmp != (HBITMAP)NULL ) {
            ToolBarAddItem( tbar->tbar, &info->items[i] );
        }
    }

    tbar->win = ToolBarWindow( tbar->tbar );

    if( (info->dinfo.style & TOOLBAR_FLOAT_STYLE) == TOOLBAR_FLOAT_STYLE ) {
        sys_menu = GetSystemMenu( tbar->win, FALSE );
        i = GetMenuItemCount( sys_menu );
        for( ; i>0; i-- ) {
            DeleteMenu( sys_menu, 0, MF_BYPOSITION );
        }
        text = WdeAllocRCString( WDE_SYSMENUMOVE );
        AppendMenu( sys_menu, MF_STRING , SC_MOVE,  (text) ? text : "Move" );
        if( text ) {
            WdeFreeRCString( text );
        }
        text = WdeAllocRCString( WDE_SYSMENUSIZE );
        AppendMenu( sys_menu, MF_STRING , SC_SIZE,  (text) ? text : "Size" );
        if( text ) {
            WdeFreeRCString( text );
        }
        text = WdeAllocRCString( WDE_SYSMENUHIDE );
        AppendMenu( sys_menu, MF_STRING , SC_CLOSE, (text) ? text : "Hide" );
        if( text ) {
            WdeFreeRCString( text );
        }
    }

    width = info->dinfo.area.right - info->dinfo.area.left;
    height = info->dinfo.area.bottom - info->dinfo.area.top;
    SetWindowPos( tbar->win, HWND_TOP, 0 ,0 , width, height,
                  SWP_NOMOVE | SWP_NOZORDER );

    ShowWindow( tbar->win, SW_SHOWNORMAL );

    UpdateWindow( tbar->win );

    WdeAddToolBar ( tbar );

    return ( tbar );
}

BOOL WdeToolBarHook( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    MINMAXINFO  *minmax;
    WdeToolBar  *tbar;
    Bool         ret;

    if( !( tbar = WdeFindToolBar ( hwnd ) ) || ( tbar->win == NULL ) ) {
        if( msg == WM_GETMINMAXINFO ) {
            minmax = (MINMAXINFO *) lParam;
            minmax->ptMinTrackSize.x = 8;
        }
        return( FALSE );
    }

    ret = FALSE;

    switch( msg ) {

        case WM_USER:
            WdeHandleToolHint ( wParam, (BOOL)lParam );
            WdeHandleStickyToolPress ( tbar, wParam, lParam );
            break;

        case WM_SIZE:
            if ( ( wParam != SIZE_MAXIMIZED ) &&
                 ( wParam != SIZE_MINIMIZED ) ) {
                GetWindowRect ( hwnd, &tbar->last_pos );
            }
            break;

        case WM_MOVE:
            if ( !IsZoomed ( hwnd ) ) {
                GetWindowRect ( hwnd, &tbar->last_pos );
            }
            break;

        case WM_GETMINMAXINFO:
            minmax = (MINMAXINFO *) lParam;
            minmax->ptMinTrackSize.x =
                2 * GetSystemMetrics(SM_CXFRAME) +
                tbar->info->dinfo.border_size.x +
                tbar->info->dinfo.button_size.x;
            minmax->ptMinTrackSize.y =
                2 * GetSystemMetrics(SM_CYFRAME) +
                tbar->info->dinfo.border_size.y +
                GetSystemMetrics(SM_CYCAPTION) +
                tbar->info->dinfo.button_size.y;
            ret = TRUE;
            break;

        case WM_DESTROY:
            WdeCloseToolBar ( tbar );
            break;

    }

    return ( ret );
}

void WdeHandleToolHint ( WPARAM wParam, BOOL pressed )
{
    if ( pressed ) {
        WdeDisplayHint ( wParam );
    } else {
        WdeSetStatusText ( NULL, "", TRUE );
    }
}

void WdeHandleStickyToolPress ( WdeToolBar *tbar, WPARAM wP, LPARAM lP )
{
    int bstate;

    if ( lP ) {
        bstate = BUTTON_UP;
    } else {
        bstate = BUTTON_DOWN;
    }

    WdeSetToolBarItemState ( tbar, wP, bstate );
}

WdeToolBar *WdeFindToolBar ( HWND win )
{
    WdeToolBar *tbar;
    LIST       *tlist;

    for ( tlist = WdeToolBarList; tlist; tlist = ListNext ( tlist ) ) {
        tbar = ListElement ( tlist );
        if ( tbar->win == win ) {
            return ( tbar );
        }
    }

    return ( NULL );
}

Bool WdeCloseToolBar ( WdeToolBar *tbar )
{
    if ( tbar ) {
        tbar->win = (HWND) NULL;
        WdeRemoveToolBar ( tbar );
        WdeFreeToolBar ( tbar );
    }

    return( TRUE );
}

void WdeFreeToolBarInfo ( WdeToolBarInfo *info )
{
    if ( info ) {
        if ( info->items ) {
            WdeMemFree ( info->items );
        }
        if ( info->dinfo.background ) {
            DeleteObject ( info->dinfo.background );
        }
        WdeMemFree ( info );
    }
}

WdeToolBarInfo *WdeAllocToolBarInfo ( int num )
{
    WdeToolBarInfo *info;

    info = (WdeToolBarInfo *) WdeMemAlloc ( sizeof (WdeToolBarInfo) );

    if ( info ) {
        memset ( info, 0, sizeof ( WdeToolBarInfo ) );
        info->items = (TOOLITEMINFO *) WdeMemAlloc ( sizeof(TOOLITEMINFO) * num );
        if ( info->items ) {
            memset ( info->items, 0, sizeof(TOOLITEMINFO) * num );
            info->num_items = num;
        } else {
            WdeMemFree ( info );
            info = NULL;
        }
    }

    return ( info );
}

WdeToolBar *WdeAllocToolBar ( void )
{
    WdeToolBar *tbar;

    tbar = (WdeToolBar *) WdeMemAlloc ( sizeof ( WdeToolBar ) );
    if ( tbar ) {
        memset ( tbar, 0, sizeof ( WdeToolBar ) );
    }

    return ( tbar );
}

void WdeFreeToolBar ( WdeToolBar *tbar )
{
    if ( tbar ) {
        WdeMemFree ( tbar );
    }
}

void WdeAddToolBar ( WdeToolBar *tbar )
{
    WdeInsertObject ( &WdeToolBarList, (void *)tbar );
}

void WdeDestroyToolBar ( WdeToolBar *tbar )
{
    ToolBarDestroy ( tbar->tbar );
}

void WdeRemoveToolBar ( WdeToolBar *tbar )
{
    ListRemoveElt ( &WdeToolBarList, (void *)tbar );
}

void WdeShutdownToolBars ( void )
{
    WdeToolBar  *tbar;
    LIST        *tlist;

    tlist = WdeListCopy ( WdeToolBarList );
    for ( ; tlist; tlist = ListConsume ( tlist ) ) {
        tbar = ListElement ( tlist );
        ToolBarDestroy ( tbar->tbar );
    }
    ToolBarFini ( NULL );

    ListFree ( WdeToolBarList );
}

void WdeSetToolBarItemState ( WdeToolBar *tbar, UINT id, UINT state )
{
    if ( tbar /*&& ( ToolBarGetState ( tbar->tbar, id ) != state )*/ ) {
        ToolBarSetState ( tbar->tbar, id, state );
    }
}

