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
* Description:  Spy toolbar functions.
*
****************************************************************************/


#include "spy.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "toolbr.h"

#define OFFON_ITEM      8

typedef struct {
    char        *name;
    int         id;
    HBITMAP     hbmp;
    int         tip_id;
} button;

static button toolList[] = {
    { "MAGNIFY",    SPY_ANOTHER_WINDOW,         0,  STR_TIP_ANOTHER_WINDOW        },
    { "MAGNO",      SPY_STOP,                   0,  STR_TIP_STOP                  },
    { "PEEK",       SPY_PEEK_WINDOW,            0,  STR_TIP_PEEK_WINDOW           },
    { "MARK",       SPY_MARK,                   0,  STR_TIP_MARK                  },
    { "ERASE",      SPY_CLEAR_MESSAGES,         0,  STR_TIP_CLEAR_MESSAGES        },
    { "SHOWWIN",    SPY_SHOW_SELECTED_WINDOWS,  0,  STR_TIP_SHOW_SELECTED_WINDOWS },
    { "WATCHMSG",   SPY_MESSAGES_WATCH,         0,  STR_TIP_MESSAGES_WATCH        },
    { "STOPMSG",    SPY_MESSAGES_STOP,          0,  STR_TIP_MESSAGES_STOP         },
    { "NOTONOFF",   SPY_OFFON,                  0,  STR_TIP_OFFON                 }
};

static toolbar      *toolBar = NULL;

/*
 * addToolButton - add a button to the tool bar
 */
static void addToolButton( button *tb )
{
    TOOLITEMINFO        info;

    tb->hbmp = LoadBitmap( Instance, tb->name );
    info.u.bmp = tb->hbmp;
    info.id = tb->id;
    info.flags = 0;
    info.depressed = 0;
    if( tb->tip_id < 0 || LoadString( Instance, tb->tip_id, info.tip, MAX_TIP ) <= 0 ) {
        info.tip[0] = '\0';
    }
    ToolBarAddItem( toolBar, &info );

} /* addToolButton */

/*
 * myToolBarProc - called by toolbar window proc
 */
static bool myToolBarProc( HWND hwnd, UINT msg, WPARAM w, LPARAM l )
{
    hwnd = hwnd;
    msg = msg;
    w = w;
    l = l;
    return( false );

} /* myToolBarProc */


/*
 * spyToolBarHint
 */
static void spyToolBarHint( HWND hwnd, ctl_id id, bool select ) {

    hwnd = hwnd;
    HintToolBar( StatusHdl, id, select );
}

/*
 * CreateSpyTool - create the spy toolbar
 */
void CreateSpyTool( HWND parent )
{
    TOOLDISPLAYINFO     dinfo;
    RECT                r;
    int                 i;

    GetClientRect( parent, &r );
    r.bottom = r.top + GET_TOOLBAR_HEIGHT( BITMAP_Y );

    memset( &dinfo, 0, sizeof( dinfo ) );
    dinfo.button_size.x = BITMAP_X;
    dinfo.button_size.y = BITMAP_Y;
    dinfo.border_size.x = BORDER_X( BITMAP_X );
    dinfo.border_size.y = BORDER_Y( BITMAP_Y );
    dinfo.style = TOOLBAR_FIXED_STYLE;
    dinfo.is_fixed = TRUE;
    dinfo.area = r;
    dinfo.hook = myToolBarProc;
    dinfo.helphook = spyToolBarHint;
    dinfo.background = NULL;
    dinfo.use_tips = TRUE;
    toolBar = ToolBarInit( parent );

    ToolBarDisplay( toolBar, &dinfo );

    for( i = 0; i < sizeof( toolList ) / sizeof( button ); i++ ) {
        addToolButton( &toolList[i] );
    }

    ShowWindow( ToolBarWindow( toolBar ), SW_SHOWNORMAL );
    UpdateWindow( ToolBarWindow( toolBar ) );

} /* CreateSpyTool */

/*
 * ShowSpyTool - show or hide toolbar
 */
void ShowSpyTool( BOOL show )
{
    if( show ) {
        ShowWindow( ToolBarWindow( toolBar ), SW_SHOW );
    } else {
        ShowWindow( ToolBarWindow( toolBar ), SW_HIDE );
    }

} /* ShowSpyTool */

/*
 * ResizeSpyTool - resize toolbar
 */
void ResizeSpyTool( WORD width, WORD height )
{
    height = height;
    MoveWindow( ToolBarWindow( toolBar ), 0, 0, width,
                GET_TOOLBAR_HEIGHT( BITMAP_Y ), TRUE );

} /* ResizeSpyTool */

/*
 * GetSpyToolRect - get toolbar position and size
 */
void GetSpyToolRect( RECT *prect )
{
    GetWindowRect( ToolBarWindow( toolBar ), prect );

} /* GetSpyToolRect */

/*
 * DestroySpyTool - destroy the spy toolbar
 */
void DestroySpyTool( void )
{
    int i;

    if( toolBar != NULL ) {
        ToolBarFini( toolBar );
        toolBar = NULL;
    }
    for( i = 0; i < sizeof( toolList ) / sizeof( button ); i++ ) {
        DeleteObject( toolList[i].hbmp );
    }

} /* DestroySpyTool */

/*
 * SetOnOffTool - change off/on (stoplight)
 */
void SetOnOffTool( spystate ss )
{
    char        *str;

    switch( ss ) {
    case OFF:
        str = "OFF";
        break;
    case ON:
        str = "ON";
        break;
    case NEITHER:
        str = "NOTONOFF";
        break;
    }
    DeleteObject( toolList[OFFON_ITEM].hbmp );
    toolList[OFFON_ITEM].hbmp = LoadBitmap( Instance, str );
    ChangeToolButtonBitmap( toolBar, SPY_OFFON, toolList[OFFON_ITEM].hbmp );

} /* SetOnOffTool */

