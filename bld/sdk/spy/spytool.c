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


#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "spy.h"
#include "toolbr.h"

#define OFFON_ITEM      8

typedef struct {
    char        *name;
    int         id;
    HBITMAP     hbmp;
} button;

static button toolList[] = {
    { "MAGNIFY", SPY_ANOTHER_WINDOW, 0 },
    { "MAGNO", SPY_STOP, 0 },
    { "PEEK", SPY_PEEK_WINDOW, 0 },
    { "MARK", SPY_MARK, 0 },
    { "ERASE", SPY_CLEAR_MESSAGES, 0 },
    { "SHOWWIN", SPY_SHOW_SELECTED_WINDOWS, 0 },
    { "WATCHMSG", SPY_MESSAGES_WATCH, 0 },
    { "STOPMSG", SPY_MESSAGES_STOP, 0 },
    { "NOTONOFF", SPY_OFFON, 0 }
};

static void     *toolBar = NULL;

/*
 * addToolButton - add a button to the tool bar
 */
static void addToolButton( button *tb )
{
    TOOLITEMINFO        info;

    tb->hbmp = LoadBitmap( Instance, tb->name );
    info.bmp = tb->hbmp;
    info.id = tb->id;
    info.flags = 0;
    info.depressed = 0;
    ToolBarAddItem( toolBar, &info );

} /* addToolButton */

/*
 * MyToolBarProc - called by toolbar window proc
 */
BOOL MyToolBarProc( HWND hwnd, unsigned msg, UINT w, LONG l )
{
    hwnd = hwnd;
    msg = msg;
    w = w;
    l = l;
    return( FALSE );

} /* MyToolBarProc */


/*
 * spyToolBarHint
 */
static void spyToolBarHint( HWND hwnd, UINT menuid, BOOL select ) {

    hwnd = hwnd;
    HintToolBar( StatusHdl, menuid, select );
}

/*
 * CreateSpyTool - create the spy listbox
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
    dinfo.hook = MyToolBarProc;
    dinfo.helphook = spyToolBarHint;
    dinfo.background = NULL;
    toolBar = ToolBarInit( parent );

    ToolBarDisplay( toolBar, &dinfo );

    for( i=0;i<sizeof( toolList ) / sizeof( button );i++ ) {
        addToolButton( &toolList[i] );
    }

    ShowWindow( ToolBarWindow( toolBar ), SW_SHOWNORMAL );
    UpdateWindow( ToolBarWindow( toolBar ) );

} /* CreateSpyTool */

/*
 * ResizeSpyTool - resize tool bar
 */
void ResizeSpyTool( WORD width, WORD height )
{
    height = height;
    MoveWindow( ToolBarWindow( toolBar ), 0, 0, width,
                GET_TOOLBAR_HEIGHT( BITMAP_Y ), TRUE );

} /* ResizeSpyTool */

/*
 * DestroySpyTool - destroy the spy tool bar
 */
void DestroySpyTool( void )
{
    int i;

    if( toolBar != NULL ) {
        ToolBarFini( toolBar );
        toolBar = NULL;
    }
    for( i=0;i<sizeof( toolList ) / sizeof( button );i++ ) {
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
    DeleteObject( toolList[ OFFON_ITEM ].hbmp );
    toolList[ OFFON_ITEM ].hbmp = LoadBitmap( Instance, str );
    ChangeToolButtonBitmap( toolBar, SPY_OFFON, toolList[ OFFON_ITEM].hbmp );

} /* SetOnOffTool */
