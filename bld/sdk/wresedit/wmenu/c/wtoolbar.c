/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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


#include "commonui.h"
#include <string.h>
#include <limits.h>
#include "watcom.h"
#include "wglbl.h"
#include "ldstr.h"
#include "wstat.h"
#include "wmain.h"
#include "wmsg.h"
#include "whints.h"
#include "wlist.h"
#include "wtoolbar.h"
#include "rcstr.gh"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define OUTLINE_AMOUNT    4
#define TOOL_BORDER_X     1
#define TOOL_BORDER_Y     1

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
extern bool WToolBarHook( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static WToolBar *WFindToolBar( HWND );
static WToolBar *WAllocToolBar( void );
static void     WAddToolBar( WToolBar * );
static void     WRemoveToolBar( WToolBar * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static LIST     *WToolBarList = NULL;

WToolBar *WCreateToolBar( WToolBarInfo *info, HWND parent )
{
    WToolBar    *tbar;
    int         i;
    HMENU       sys_menu;
    char        *text;

    if( info == NULL ) {
        return( NULL );
    }

    tbar = WAllocToolBar();
    if( tbar == NULL ) {
        return( NULL );
    }

    tbar->info = info;
    tbar->parent = parent;
    tbar->tbar = ToolBarInit( parent );

    ToolBarDisplay( tbar->tbar, &info->dinfo );

    for( i = 0; i < info->num_items; i++ ) {
        ToolBarAddItem( tbar->tbar, &info->items[i] );
    }

    tbar->win = ToolBarWindow( tbar->tbar );

    if( (info->dinfo.style & TOOLBAR_FLOAT_STYLE) == TOOLBAR_FLOAT_STYLE ) {
        sys_menu = GetSystemMenu( tbar->win, FALSE );
        i = GetMenuItemCount( sys_menu );
        for( ; i > 0; i-- ) {
            DeleteMenu( sys_menu, i, MF_BYPOSITION );
        }
        text = AllocRCString( W_SYSMENUMOVE );
        AppendMenu( sys_menu, MF_STRING, SC_MOVE, text != NULL ? text : "Move" );
        if( text != NULL ) {
            FreeRCString( text );
        }
        text = AllocRCString( W_SYSMENUSIZE );
        AppendMenu( sys_menu, MF_STRING, SC_SIZE, text != NULL ? text : "Size" );
        if( text != NULL ) {
            FreeRCString( text );
        }
        text = AllocRCString( W_SYSMENUHIDE );
        AppendMenu( sys_menu, MF_STRING, SC_CLOSE, text != NULL ? text : "Hide" );
        if( text != NULL ) {
            FreeRCString( text );
        }
    }

    ShowWindow( tbar->win, SW_SHOWNORMAL );

    UpdateWindow( tbar->win );

    WAddToolBar( tbar );

    return( tbar );
}

bool WToolBarHook( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    MINMAXINFO  *minmax;
    WToolBar    *tbar;
    int         bstate;
    bool        ret;

    if( (tbar = WFindToolBar( hwnd )) == NULL || tbar->win == NULL ) {
        return( false );
    }

    ret = false;

    switch( msg ) {
    case WM_USER:
        if( lParam ) {
            WSetStatusText( NULL, NULL, "" );
        } else {
            WDisplayHint( NULL, wParam );
        }
        if( lParam ) {
            bstate = BUTTON_UP;
        } else {
            bstate = BUTTON_DOWN;
        }
        WSetToolBarItemState( tbar, LOWORD( wParam ), bstate );
        break;

    case WM_SIZE:
        if( wParam != SIZE_MAXIMIZED && wParam != SIZE_MINIMIZED ) {
            GetWindowRect( hwnd, &tbar->last_pos );
        }
        break;

    case WM_MOVE:
        if( !IsZoomed( hwnd ) ) {
            GetWindowRect( hwnd, &tbar->last_pos );
        }
        break;

    case WM_GETMINMAXINFO:
        minmax = (MINMAXINFO *)lParam;
        minmax->ptMinTrackSize.x = 2 * GetSystemMetrics( SM_CXFRAME ) +
                                   tbar->info->dinfo.border_size.x +
                                   tbar->info->dinfo.button_size.x - 1;
        minmax->ptMinTrackSize.y = 2 * GetSystemMetrics( SM_CYFRAME ) +
                                   tbar->info->dinfo.border_size.y +
                                   GetSystemMetrics( SM_CYCAPTION ) +
                                   tbar->info->dinfo.button_size.y - 1;
        break;

    case WM_DESTROY:
        WCloseToolBar( tbar );
        break;
    }

    return( ret );
}

WToolBar *WFindToolBar( HWND win )
{
    WToolBar    *tbar;
    LIST        *tlist;

    for( tlist = WToolBarList; tlist != NULL; tlist = ListNext( tlist ) ) {
        tbar = ListElement( tlist );
        if( tbar->win == win ) {
            return( tbar );
        }
    }

    return( NULL );
}

bool WCloseToolBar( WToolBar *tbar )
{
    if( tbar != NULL ) {
        tbar->win = (HWND)NULL;
        WRemoveToolBar( tbar );
        WFreeToolBar( tbar );
    }

    return( TRUE );
}

void WFreeToolBarInfo( WToolBarInfo *info )
{
    if( info != NULL ) {
        if( info->items != NULL ) {
            WRMemFree( info->items );
        }
        if( info->dinfo.background != NULL ) {
            DeleteObject( info->dinfo.background );
        }
        WRMemFree( info );
    }
}

WToolBarInfo *WAllocToolBarInfo( int num )
{
    WToolBarInfo *info;

    info = (WToolBarInfo *)WRMemAlloc( sizeof( WToolBarInfo ) );

    if( info != NULL ) {
        memset( info, 0, sizeof( WToolBarInfo ) );
        info->items = (TOOLITEMINFO *)WRMemAlloc( sizeof( TOOLITEMINFO ) * num );
        if( info->items != NULL ) {
            memset( info->items, 0, sizeof( TOOLITEMINFO ) * num );
            info->num_items = num;
        } else {
            WRMemFree( info );
            info = NULL;
        }
    }

    return( info );
}

WToolBar *WAllocToolBar( void )
{
    WToolBar *tbar;

    tbar = (WToolBar *)WRMemAlloc( sizeof( WToolBar ) );
    if( tbar != NULL ) {
        memset( tbar, 0, sizeof( WToolBar ) );
    }

    return( tbar );
}

void WFreeToolBar( WToolBar *tbar )
{
    if( tbar != NULL ) {
        WRMemFree( tbar );
    }
}

void WAddToolBar( WToolBar *tbar )
{
    WInsertObject( &WToolBarList, (void *)tbar );
}

void WDestroyToolBar( WToolBar *tbar )
{
    ToolBarDestroy( tbar->tbar );
}

void WRemoveToolBar( WToolBar *tbar )
{
    ListRemoveElt( &WToolBarList, (void *)tbar );
}

void WShutdownToolBars( void )
{
    WToolBar  *tbar;
    LIST      *tlist;

    tlist = WListCopy( WToolBarList );
    for( ; tlist != NULL; tlist = ListConsume( tlist ) ) {
        tbar = ListElement( tlist );
        ToolBarDestroy( tbar->tbar );
    }
    ToolBarFini( NULL );

    ListFree( WToolBarList );
}

void WSetToolBarItemState( WToolBar *tbar, int id, UINT state )
{
    if( tbar != NULL /* && ToolBarGetState( tbar->tbar, id ) != state */ ) {
        ToolBarSetState( tbar->tbar, id, state );
    }
}
