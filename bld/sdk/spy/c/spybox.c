/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2021 The Open Watcom Contributors. All Rights Reserved.
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
#include "loadcc.h"
#include "log.h"
#ifdef __NT__
    #include <commctrl.h>
#endif

#ifdef __WINDOWS__
    #define LISTBOX_X       10
    #define LISTBOX_Y       10
#else
    #define LISTBOX_X       6
    #define LISTBOX_Y       6
#endif

static int          xChar, yChar;

#ifdef __NT__
typedef struct column_info {
    int string_id;
    int width;
} column_info;

#define NUM_COLUMNS 5

#ifdef _WIN64
#define VAR_COLUMN_WIDTH    150
#else
#define VAR_COLUMN_WIDTH    80
#endif

column_info columns[NUM_COLUMNS] = {
    { STR_HEADING_1, 200 },                 // text
    { STR_HEADING_2, VAR_COLUMN_WIDTH },    // HWND
    { STR_HEADING_3, 80 },                  // UINT
    { STR_HEADING_4, VAR_COLUMN_WIDTH },    // WPARAM
    { STR_HEADING_5, VAR_COLUMN_WIDTH }     // LPARAM
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
void SpyOut( const char *msg, LPMSG pmsg, const char *class_name )
{
    int             i;
    char            res[SPYOUT_LENGTH + 1 + 80];
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
        res[SPYOUT_LENGTH] = ' ';
        strcpy( res + SPYOUT_LENGTH + 1, class_name );
    } else {
        strcpy( res, msg );
    }
    LogOut( res );

#ifdef __NT__
    if( IsCommCtrlLoaded() ) {
        lvi.mask = LVIF_TEXT;
        lvi.iItem = (int)SendMessage( SpyListBox, LVM_GETITEMCOUNT, 0, 0L );
        lvi.iSubItem = 0;
        lvi.pszText = NULL;
        i = (int)SendMessage( SpyListBox, LVM_INSERTITEM, 0, (LPARAM)&lvi );
        if( pmsg != NULL && i != -1 ) {
            GetHexStr( hwnd_str, (UINT_PTR)pmsg->hwnd, SPYOUT_HWND_LEN );
            hwnd_str[SPYOUT_HWND_LEN] = '\0';
            GetHexStr( msg_str, pmsg->message, SPYOUT_MSG_LEN );
            msg_str[SPYOUT_MSG_LEN] = '\0';
            GetHexStr( wparam_str, pmsg->wParam, SPYOUT_WPARAM_LEN );
            wparam_str[SPYOUT_WPARAM_LEN] = '\0';
            GetHexStr( lparam_str, pmsg->lParam, SPYOUT_LPARAM_LEN );
            lparam_str[SPYOUT_LPARAM_LEN] = '\0';
            lvi.iSubItem = 0;
            lvi.pszText = strcpy( res, msg );
            SendMessage( SpyListBox, LVM_SETITEMTEXT, i, (LPARAM)&lvi );
            lvi.iSubItem = 1;
            lvi.pszText = hwnd_str;
            SendMessage( SpyListBox, LVM_SETITEMTEXT, i, (LPARAM)&lvi );
            lvi.iSubItem = 2;
            lvi.pszText = msg_str;
            SendMessage( SpyListBox, LVM_SETITEMTEXT, i, (LPARAM)&lvi );
            lvi.iSubItem = 3;
            lvi.pszText = wparam_str;
            SendMessage( SpyListBox, LVM_SETITEMTEXT, i, (LPARAM)&lvi );
            lvi.iSubItem = 4;
            lvi.pszText = lparam_str;
            SendMessage( SpyListBox, LVM_SETITEMTEXT, i, (LPARAM)&lvi );
        }
        if( SpyMessagesAutoScroll ) {
            SendMessage( SpyListBox, LVM_ENSUREVISIBLE, i, FALSE );
        }
    } else {
#endif
        i = (int)SendMessage( SpyListBox, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)res );
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
        SpyListBox = CreateWindowEx(
            WS_EX_CLIENTEDGE,                   /* Window extended style */
            WC_LISTVIEW,                        /* Window class name */
            NULL,                               /* Window caption */
            WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
                                                /* Window style */
            LISTBOX_X,                          /* Initial X position */
            LISTBOX_Y,                          /* Initial Y position */
            0,                                  /* Initial X size */
            0,                                  /* Initial Y size */
            parent,                             /* Parent window handle */
            (HANDLE)SPY_LIST_BOX,               /* Window menu handle */
            Instance,                           /* Program instance handle */
            NULL );                             /* Create parameters */
        SendMessage( SpyListBox, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT );
        lvc.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
        for( i = 0; i < NUM_COLUMNS; i++ ) {
            lvc.cx = columns[i].width;
            lvc.pszText = AllocRCString( columns[i].string_id );
            lvc.iSubItem = i;
            SendMessage( SpyListBox, LVM_INSERTCOLUMN, i, (LPARAM)&lvc );
            FreeRCString( lvc.pszText );
        }
  #ifdef _WIN64
    } else
  #else
    } else if( LOBYTE( LOWORD( GetVersion() ) ) >= 4 ) {
  #endif
        SpyListBox = CreateWindowEx(
            WS_EX_CLIENTEDGE,                   /* Window extended style */
            "LISTBOX",                          /* Window class name */
            "Messages",                         /* Window caption */
            WS_CHILD | LBS_NOTIFY | WS_VSCROLL, /* Window style */
            LISTBOX_X,                          /* Initial X position */
            LISTBOX_Y,                          /* Initial Y position */
            0,                                  /* Initial X size */
            0,                                  /* Initial Y size */
            parent,                             /* Parent window handle */
            (HANDLE)SPY_LIST_BOX,               /* Window menu handle */
            Instance,                           /* Program instance handle */
            NULL );                             /* Create parameters */
  #ifndef _WIN64
    } else
  #endif
#endif
  #ifndef _WIN64
        SpyListBox = CreateWindow(
            "LISTBOX",                          /* Window class name */
            "Messages",                         /* Window caption */
            WS_CHILD | LBS_NOTIFY | WS_VSCROLL | WS_BORDER,
                                                /* Window style */
            LISTBOX_X,                          /* Initial X position */
            LISTBOX_Y,                          /* Initial Y position */
            0,                                  /* Initial X size */
            0,                                  /* Initial Y size */
            parent,                             /* Parent window handle */
            (HANDLE)SPY_LIST_BOX,               /* Window menu handle */
            Instance,                           /* Program instance handle */
            NULL );                             /* Create parameters */
  #endif

    ShowWindow( SpyListBox, SW_NORMAL );
    UpdateWindow( SpyListBox );
    SetMonoFont( SpyListBox );

#ifdef __NT__
    if( !IsCommCtrlLoaded() ) {
#endif
        SpyListBoxTitle = CreateWindow(
            "STATIC",                           /* Window class name */
            TitleBar,                           /* Window caption */
            SS_LEFT | WS_CHILD,                 /* Window style */
            LISTBOX_X + 4,                      /* Initial X position */
            LISTBOX_Y,                          /* Initial Y position */
            ( TitleBarLen + 1 ) * xChar,        /* Initial X size */
            yChar,                              /* Initial Y size */
            parent,                             /* Parent window handle */
            (HMENU)NULL,                        /* Window menu handle */
            Instance,                           /* Program instance handle */
            NULL );                             /* Create parameters */
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
    SpyMessagesPaused = !SpyMessagesPaused;
    CheckMenuItem( SpyMenu, SPY_PAUSE_MESSAGES, MF_BYCOMMAND | (( SpyMessagesPaused ) ? MF_CHECKED : MF_UNCHECKED) );

} /* SpyMessagePauseToggle */

/*
 * ResizeSpyBox - make list box new size, based on height/width of parent
 *                client area.
 */
void ResizeSpyBox( WORD width, WORD height )
{
    int         nheight;
    int         xheight;
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
    xheight = 0;
    if( SpyMainWndInfo.show_toolbar ) {
        GetSpyToolRect( &area );
        xheight = area.bottom - area.top;
        ypos += xheight;
        nheight -= xheight;
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
    MoveWindow( SpyListBoxTitle, LISTBOX_X + 4, LISTBOX_Y + xheight, ( TitleBarLen + 1 ) * xChar, yChar, TRUE );

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

    InvalidateRect( SpyListBox, NULL, FALSE );
    InvalidateRect( SpyListBoxTitle, NULL, FALSE );
    UpdateWindow( SpyListBox );
    UpdateWindow( SpyListBoxTitle );

} /* ResetSpyListBox */

#ifdef __NT__
static void message_name_align( char *name )
{
    size_t  len;

    for( len = strlen( name ); len < SPYOUT_NAME_LEN; ++len ) {
        name[len] = ' ';
    }
    len = SPYOUT_NAME_LEN;
    name[len] = '\0';
}
#endif

/*
 * GetSpyBoxSelection - get the currently selected message
 */
bool GetSpyBoxSelection( char *str )
{
    int     sel;

#ifdef __NT__
    if( !IsCommCtrlLoaded() ) {
#endif
        sel = (int)SendMessage( SpyListBox, LB_GETCURSEL, 0, 0L );
        if( sel == LB_ERR ) {
            return( false );
        }
        SendMessage( SpyListBox, LB_GETTEXT, sel, (LPARAM)(LPSTR)str );
        return( true );
#ifdef __NT__
    } else {
        LVITEM  lvi;
        char    buf[80];

        memset( str, ' ', SPYOUT_LENGTH );
        str[SPYOUT_LENGTH] = '\0';
        sel = (int)SendMessage( SpyListBox, LVM_GETNEXTITEM, (WPARAM)-1, LVNI_SELECTED );
        if( sel == -1 ) {
            return( false );
        }
        lvi.pszText = buf;
        lvi.cchTextMax = sizeof( buf );
        lvi.iSubItem = 0;
        SendMessage( SpyListBox, LVM_GETITEMTEXT, sel, (LPARAM)&lvi );
        message_name_align( buf );
        memcpy( str, buf, SPYOUT_NAME_LEN );
        lvi.iSubItem = 1;
        SendMessage( SpyListBox, LVM_GETITEMTEXT, sel, (LPARAM)&lvi );
        memcpy( str + SPYOUT_HWND, buf, SPYOUT_HWND_LEN );
        lvi.iSubItem = 2;
        SendMessage( SpyListBox, LVM_GETITEMTEXT, sel, (LPARAM)&lvi );
        memcpy( str + SPYOUT_MSG, buf, SPYOUT_MSG_LEN );
        return( true );
    }
#endif

} /* GetSpyBoxSelection */

char *LogSpyBoxLine( bool listview, HWND list, int line )
{
    static char     str[256];

#ifndef __NT__

    /* unused parameters */ (void)listview;

#endif

    str[0] = '\0';
#ifdef __NT__
    if( listview ) {
        LVITEM      lvi;
        char        buf[80];

        lvi.pszText = buf;
        lvi.cchTextMax = sizeof( buf );
        lvi.iSubItem = 0;
        SendMessage( list, LVM_GETITEMTEXT, line, (LPARAM)&lvi );
        message_name_align( buf );
        strcpy( str, buf );
        buf[0] = ' ';
        lvi.pszText++;
        lvi.cchTextMax--;
        lvi.iSubItem = 1;
        SendMessage( list, LVM_GETITEMTEXT, line, (LPARAM)&lvi );
        strcat( str, buf );
        lvi.iSubItem = 2;
        SendMessage( list, LVM_GETITEMTEXT, line, (LPARAM)&lvi );
        strcat( str, buf );
        lvi.iSubItem = 3;
        SendMessage( list, LVM_GETITEMTEXT, line, (LPARAM)&lvi );
        strcat( str, buf );
        lvi.iSubItem = 4;
        SendMessage( list, LVM_GETITEMTEXT, line, (LPARAM)&lvi );
        strcat( str, buf );
    } else {
#endif
        SendMessage( list, LB_GETTEXT, line, (LPARAM)(LPSTR)str );
#ifdef __NT__
    }
#endif
    return( str );
}
