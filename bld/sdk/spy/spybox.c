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

#define LISTBOX_X       10
#define LISTBOX_Y       ((TOOLBAR_HEIGHT)+8)

static int xChar,yChar;

/*
 * setCharSize - set the character size variables
 */
static void setCharSize( HWND parent )
{
    HDC         dc;
    TEXTMETRIC  tm;

    dc = GetDC( parent );
    SelectObject( dc, GetMonoFont() );
    GetTextMetrics( dc, &tm );
    ReleaseDC( parent, dc );
    xChar = tm.tmMaxCharWidth;
    yChar = tm.tmHeight;

} /* setCharSize */

/*
 * SpyOut - display spy message
 */
void SpyOut( LPSTR res )
{
    static int  i;

    if( SpyMessagesPaused ) {
        return;
    }

    SpyLogOut( res );
    i = SendMessage( SpyListBox, LB_ADDSTRING, 0, (LONG)(LPSTR)res );
    if( SpyMessagesAutoScroll ) {
        SendMessage( SpyListBox, LB_SETCURSEL, i, 0L );
    }

} /* SpyOut */

/*
 * CreateSpyBox - create the spy listbox
 */
void CreateSpyBox( HWND parent )
{
    setCharSize( parent );

    SpyListBox = CreateWindow(
        "LISTBOX",          /* Window class name */
        "Messages",         /* Window caption */
        WS_CHILD | LBS_NOTIFY
        | WS_VSCROLL | WS_BORDER ,/* Window style */
        LISTBOX_X,          /* Initial X position */
        LISTBOX_Y,          /* Initial Y position */
        0,          /* Initial X size */
        0,          /* Initial Y size */
        parent,     /* Parent window handle */
        (HANDLE) SPY_LIST_BOX,              /* Window menu handle */
        Instance,           /* Program instance handle */
        NULL);              /* Create parameters */

    ShowWindow( SpyListBox, SW_NORMAL );
    UpdateWindow( SpyListBox );
    SetMonoFont( SpyListBox );

    SpyListBoxTitle = CreateWindow(
        "STATIC",               /* Window class name */
        TitleBar,
        SS_LEFT | WS_CHILD,     /* Window style */
        LISTBOX_X+4,            /* Initial X position */
        LISTBOX_Y,      /* Initial Y position */
        (1+ TitleBarLen) * xChar,/* Initial X size */
        yChar,                  /* Initial Y size */
        parent,                 /* Parent window handle */
        (HMENU) NULL,           /* Window menu handle */
        Instance,               /* Program instance handle */
        NULL);                  /* Create parameters */
    ShowWindow( SpyListBoxTitle, SW_NORMAL );
    UpdateWindow( SpyListBoxTitle );
    SetMonoFont( SpyListBoxTitle );

} /* CreateSpyBox */

/*
 * ClearSpyBox - erase contents of spy list box
 */
void ClearSpyBox( void )
{
    SendMessage( SpyListBox, LB_RESETCONTENT, 0, 0L );

} /* ClearSpyBox */

/*
 * SpyMessagePauseToggle - switch between paused/unpaused state
 */
void SpyMessagePauseToggle( void )
{
    WORD        check;

    check = MF_BYCOMMAND;
    if( SpyMessagesPaused ) {
        check |= MF_UNCHECKED;
        SpyMessagesPaused = FALSE;
    } else {
        SpyMessagesPaused = TRUE;
        check |= MF_CHECKED;
    }
    CheckMenuItem( SpyMenu, SPY_PAUSE_MESSAGES, check );

} /* SpyMessagePauseToggle */

/*
 * ResizeSpyBox - make list box new size, based on height/width of parent
 *                client area.
 */
void ResizeSpyBox( WORD width, WORD height )
{
    int         nheight;
    int         ypos;
    RECT        area;
    HWND        hinthwnd;

    ypos = LISTBOX_Y + yChar+3;
//    width = (4+TitleBarLen)*xChar;
    width -= 2 * LISTBOX_X;
    nheight = height-ypos-3;
    if( SpyMainWndInfo.show_hints ) {
        hinthwnd = GetHintHwnd( StatusHdl );
        GetWindowRect( hinthwnd, &area );
        nheight -= area.bottom - area.top;
    }
    if( nheight < 0 ) {
        ShowWindow( SpyListBoxTitle, SW_HIDE );
        nheight = 0;
    } else {
        ShowWindow( SpyListBoxTitle, SW_SHOW );
    }
    if( nheight < yChar ) {
        ShowWindow( SpyListBox, SW_HIDE );
    } else {
        ShowWindow( SpyListBox, SW_SHOW );
    }

    MoveWindow( SpyListBox, LISTBOX_X, ypos, width, nheight, TRUE );

} /* ResizeSpyBox */

/*
 * ResetSpyListBox - reset the current spy list box
 */
void ResetSpyListBox( void  )
{
    RECT        r;

    setCharSize( SpyMainWindow );
    SetMonoFont( SpyListBox );
    SetMonoFont( SpyListBoxTitle );


    GetClientRect( SpyMainWindow, &r );
    ResizeSpyBox( r.right - r.left, r.bottom - r.top );

    MoveWindow( SpyListBoxTitle, LISTBOX_X+4, LISTBOX_Y,
                (1+ TitleBarLen) * xChar, yChar, TRUE );

    InvalidateRect( SpyListBox, NULL, NULL );
    InvalidateRect( SpyListBoxTitle, NULL, NULL );
    UpdateWindow( SpyListBox );
    UpdateWindow( SpyListBoxTitle );

} /* ResetSpyListBox */
