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
* Description:  Spy list box functions.
*
****************************************************************************/


#include "spy.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "loadcc.h"

#if defined( __NT__ )
    #define LISTBOX_X       6
    #define LISTBOX_Y       6
#else
    #define LISTBOX_X       10
    #define LISTBOX_Y       10
#endif

static int          xChar, yChar;

#ifdef __NT__
typedef struct column_info {
    int string_id;
    int width;
} column_info;

#define NUM_COLUMNS 5

column_info columns[NUM_COLUMNS] = {
    { STR_HEADING_1, 150 },
    { STR_HEADING_2, 80 },
    { STR_HEADING_3, 80 },
    { STR_HEADING_4, 80 },
    { STR_HEADING_5, 80 }
};
#endif

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
void SpyOut( char *msg, LPMSG pmsg )
{
    static LRESULT  i;
    char            res[SPYOUT_LENGTH + 1];
#ifdef __NT__
    LVITEM          lvi;
    char            hwnd_str[SPYOUT_HWND_LEN + 1];
    char            msg_str[SPYOUT_MSG_LEN + 1];
    char            wparam_str[SPYOUT_WPARAM_LEN + 1];
    char            lparam_str[SPYOUT_LPARAM_LEN + 1];
#endif

    if( SpyMessagesPaused ) {
        return;
    }

    if( pmsg != NULL ) {
        FormatSpyMessage( msg, pmsg, res );
    } else {
        strcpy( res, msg );
    }
    SpyLogOut( res );

#ifdef __NT__
    if( IsCommCtrlLoaded() ) {
        lvi.mask = LVIF_TEXT;
        lvi.iItem = SendMessage( SpyListBox, LVM_GETITEMCOUNT, 0, 0L );
        lvi.iSubItem = 0;
        lvi.pszText = msg;
        SendMessage( SpyListBox, LVM_INSERTITEM, 0, (LPARAM)&lvi );
        if( pmsg != NULL ) {
            GetHexStr( hwnd_str, (DWORD)pmsg->hwnd, SPYOUT_HWND_LEN );
            hwnd_str[SPYOUT_HWND_LEN] = '\0';
            GetHexStr( msg_str, pmsg->message, SPYOUT_MSG_LEN );
            msg_str[SPYOUT_MSG_LEN] = '\0';
            GetHexStr( wparam_str, pmsg->wParam, SPYOUT_WPARAM_LEN );
            wparam_str[SPYOUT_WPARAM_LEN] = '\0';
            GetHexStr( lparam_str, pmsg->lParam, SPYOUT_LPARAM_LEN );
            lparam_str[SPYOUT_LPARAM_LEN] = '\0';
            lvi.iSubItem = 1;
            lvi.pszText = hwnd_str;
            SendMessage( SpyListBox, LVM_SETITEM, 0, (LPARAM)&lvi );
            lvi.iSubItem = 2;
            lvi.pszText = msg_str;
            SendMessage( SpyListBox, LVM_SETITEM, 0, (LPARAM)&lvi );
            lvi.iSubItem = 3;
            lvi.pszText = wparam_str;
            SendMessage( SpyListBox, LVM_SETITEM, 0, (LPARAM)&lvi );
            lvi.iSubItem = 4;
            lvi.pszText = lparam_str;
            SendMessage( SpyListBox, LVM_SETITEM, 0, (LPARAM)&lvi );
        }
        if( SpyMessagesAutoScroll ) {
            SendMessage( SpyListBox, LVM_ENSUREVISIBLE, lvi.iItem, FALSE );
        }
    } else {
#endif
        i = SendMessage( SpyListBox, LB_ADDSTRING, 0, (LONG)(LPSTR)res );
        if( SpyMessagesAutoScroll ) {
            SendMessage( SpyListBox, LB_SETCURSEL, i, 0L );
        }
#ifdef __NT__
    }
#endif

} /* SpyOut */

/*
 * CreateSpyBox - create the spy listbox
 */
void CreateSpyBox( HWND parent )
{
#ifdef __NT__
    LVCOLUMN    lvc;
    int         i;
#endif

    setCharSize( parent );

#ifdef __NT__
    if( LoadCommCtrl() ) {
        AllowVariableFonts();
        SpyListBox = CreateWindowEx( WS_EX_CLIENTEDGE, WC_LISTVIEW, NULL,
                                     WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
                                     LISTBOX_X, LISTBOX_Y, 0, 0, parent,
                                     (HANDLE)SPY_LIST_BOX, Instance, NULL );
        lvc.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
        for( i = 0; i < NUM_COLUMNS; i++ ) {
            lvc.cx = columns[i].width;
            lvc.pszText = AllocRCString( columns[i].string_id );
            lvc.iSubItem = i;
            SendMessage( SpyListBox, LVM_INSERTCOLUMN, i, (LPARAM)&lvc );
            FreeRCString( lvc.pszText );
        }
    } else if( LOBYTE( LOWORD( GetVersion() ) ) >= 4 ) {
        SpyListBox = CreateWindowEx(
            WS_EX_CLIENTEDGE,       /* Window extended style */
            "LISTBOX",              /* Window class name */
            "Messages",             /* Window caption */
            WS_CHILD | LBS_NOTIFY
            | WS_VSCROLL,           /* Window style */
            LISTBOX_X,              /* Initial X position */
            LISTBOX_Y,              /* Initial Y position */
            0,                      /* Initial X size */
            0,                      /* Initial Y size */
            parent,                 /* Parent window handle */
            (HANDLE)SPY_LIST_BOX,   /* Window menu handle */
            Instance,               /* Program instance handle */
            NULL );                 /* Create parameters */
    } else
#endif
        SpyListBox = CreateWindow(
            "LISTBOX",              /* Window class name */
            "Messages",             /* Window caption */
            WS_CHILD | LBS_NOTIFY | WS_VSCROLL
            | WS_BORDER ,           /* Window style */
            LISTBOX_X,              /* Initial X position */
            LISTBOX_Y,              /* Initial Y position */
            0,                      /* Initial X size */
            0,                      /* Initial Y size */
            parent,                 /* Parent window handle */
            (HANDLE)SPY_LIST_BOX,   /* Window menu handle */
            Instance,               /* Program instance handle */
            NULL );                 /* Create parameters */

    ShowWindow( SpyListBox, SW_NORMAL );
    UpdateWindow( SpyListBox );
    SetMonoFont( SpyListBox );

#ifdef __NT__
    if( !IsCommCtrlLoaded() ) {
#endif
        SpyListBoxTitle = CreateWindow(
            "STATIC",                   /* Window class name */
            TitleBar,                   /* Window caption */
            SS_LEFT | WS_CHILD,         /* Window style */
            LISTBOX_X + 4,              /* Initial X position */
            LISTBOX_Y,                  /* Initial Y position */
            (1 + TitleBarLen) * xChar,  /* Initial X size */
            yChar,                      /* Initial Y size */
            parent,                     /* Parent window handle */
            (HMENU)NULL,                /* Window menu handle */
            Instance,                   /* Program instance handle */
            NULL );                     /* Create parameters */
        ShowWindow( SpyListBoxTitle, SW_NORMAL );
        UpdateWindow( SpyListBoxTitle );
        SetMonoFont( SpyListBoxTitle );
#ifdef __NT__
    }
#endif

} /* CreateSpyBox */

/*
 * ClearSpyBox - erase contents of spy list box
 */
void ClearSpyBox( void )
{
#ifdef __NT__
    if( IsCommCtrlLoaded() ) {
        SendMessage( SpyListBox, LVM_DELETEALLITEMS, 0, 0L );
    } else {
#endif
        SendMessage( SpyListBox, LB_RESETCONTENT, 0, 0L );
#ifdef __NT__
    }
#endif

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

    ypos = LISTBOX_Y;
#ifdef __NT__
    if( !IsCommCtrlLoaded() ) {
#endif
        ypos += yChar + 3;
#ifdef __NT__
    }
#endif
    width -= 2 * LISTBOX_X;
    nheight = height - (ypos + LISTBOX_Y);

    if( SpyMainWndInfo.show_hints ) {
        hinthwnd = GetHintHwnd( StatusHdl );
        GetWindowRect( hinthwnd, &area );
        nheight -= area.bottom - area.top;
    }
    if( SpyMainWndInfo.show_toolbar ) {
        GetSpyToolRect( &area );
        ypos += area.bottom - area.top;
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
void ResetSpyListBox( void )
{
    RECT        r;

    setCharSize( SpyMainWindow );
    SetMonoFont( SpyListBox );
    SetMonoFont( SpyListBoxTitle );


    GetClientRect( SpyMainWindow, &r );
    ResizeSpyBox( r.right - r.left, r.bottom - r.top );

    MoveWindow( SpyListBoxTitle, LISTBOX_X + 4, LISTBOX_Y,
                (1 + TitleBarLen) * xChar, yChar, TRUE );

    InvalidateRect( SpyListBox, NULL, FALSE );
    InvalidateRect( SpyListBoxTitle, NULL, FALSE );
    UpdateWindow( SpyListBox );
    UpdateWindow( SpyListBoxTitle );

} /* ResetSpyListBox */

/*
 * GetSpyBoxSelection - get the currently selected message
 */
BOOL GetSpyBoxSelection( char *str )
{
    LRESULT sel;
#ifdef __NT__
    if( !IsCommCtrlLoaded() ) {
#endif
        sel = SendMessage( SpyListBox, LB_GETCURSEL, 0, 0L );
        if( sel == (WORD)LB_ERR ) {
            return( FALSE );
        }
        SendMessage( SpyListBox, LB_GETTEXT, sel, (DWORD)(LPSTR)str );
        return( TRUE );
#ifdef __NT__
    } else {
        LVITEM  lvi;
        char    buf[9];
        memset( str, ' ', SPYOUT_LENGTH );
        str[SPYOUT_LENGTH - 1] = '\0';
        sel = SendMessage( SpyListBox, LVM_GETNEXTITEM, (WPARAM)-1, LVNI_SELECTED );
        if( sel == (LRESULT)-1 ) {
            return( FALSE );
        }
        lvi.mask = LVIF_TEXT;
        lvi.iItem = (int)sel;
        lvi.iSubItem = 0;
        lvi.pszText = buf;
        lvi.cchTextMax = SPYOUT_LENGTH;
        SendMessage( SpyListBox, LVM_GETITEM, 0, (LPARAM)&lvi );
        strcpy( str, buf );
        str[strlen( str )] = ' ';
        lvi.iSubItem = 1;
        SendMessage( SpyListBox, LVM_GETITEM, 1, (LPARAM)&lvi );
        strncpy( &str[SPYOUT_HWND], buf, SPYOUT_HWND_LEN );
        str[SPYOUT_HWND + SPYOUT_HWND_LEN] = ' ';
        lvi.iSubItem = 2;
        SendMessage( SpyListBox, LVM_GETITEM, 1, (LPARAM)&lvi );
        strncpy( &str[SPYOUT_MSG], buf, SPYOUT_MSG_LEN );
        str[SPYOUT_MSG + SPYOUT_MSG_LEN] = ' ';
        return( TRUE );
    }
#endif

} /* GetSpyBoxSelection */

