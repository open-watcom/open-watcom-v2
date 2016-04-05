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


#include <windows.h>
#include <string.h>
#include "mem.h"
#include "hwbiglb.h"


/* Local Window callback functions prototypes */
BOOL __export FAR PASCAL TextBoxProc( HWND hwnd, unsigned msg, WORD wparam, LONG lparam );

#define TB_CLASS_NAME           "WatTextBox"
#define STYLE_FLAGS             WS_VISIBLE | LBS_OWNERDRAWFIXED \
                                | WS_VSCROLL | WS_HSCROLL | LBS_NOTIFY \
                                | WS_CHILD | LBS_NOINTEGRALHEIGHT | WS_BORDER

typedef struct {
    HWND        boxhwnd;
    HWND        parent;
    HFONT       font;
    unsigned    itemcnt;
    char        *(*fmt)(unsigned);
    HMENU       id;
    WORD        extent;
    RECT        old_area;
    WORD        flags;
} TBinfo;

/*
 * CreateTheListBox
 */
static void CreateTheListBox( HWND parent, TBinfo *info, DWORD style ) {

    HINSTANCE                      instance;

    instance = (HINSTANCE)GetWindowWord( parent, GWW_HINSTANCE );
    info->boxhwnd =
        CreateWindow(
            "LISTBOX",                  /* Window class name */
            "",                         /* Window caption */
            style,                      /* Window style */
            0,                          /* Initial X position */
            1,                          /* Initial Y position */
            0,                          /* Initial X size */
            0,                          /* Initial Y size */
            parent,                     /* Parent window handle */
            info->id,                   /* Window menu handle */
            instance,                   /* Program instance handle */
            NULL );                     /* Create parameters */
    SendMessage( info->boxhwnd, WM_SETFONT, (WPARAM)info->font, 0L );
}

static void SizeTheListBox( HWND hwnd, TBinfo *info ) {

    WORD                width, height;
    RECT                area;

    GetWindowRect( hwnd, &area );
    width = area.right - area.left;
    height = area.bottom - area.top;
    SetWindowPos( info->boxhwnd, NULL, 0, 0, width,
                  height - 1, SWP_NOMOVE | SWP_NOZORDER  );

    /* this is a kludge to force window the refresh the window properly
     * when it is scrolled horizontally */

    if( info->old_area.right - info->old_area.left < width ) {
        InvalidateRect( info->boxhwnd, &info->old_area, TRUE );
        UpdateWindow( info->boxhwnd );
    }
    info->old_area.top = 0;
    info->old_area.left = 0;
    info->old_area.right = width;
    info->old_area.bottom = height;
}

BOOL FAR PASCAL TextBoxProc( HWND hwnd, unsigned msg, WORD wparam, LONG lparam )
{
    TBinfo                      *info;
    MEASUREITEMSTRUCT           *mis;
    DRAWITEMSTRUCT              *dis;
    HFONT                       old_font;
    HDC                         dc;
    TEXTMETRIC                  textmetric;
    char                        *text;
    COLORREF                    bkgrndcolor;
    COLORREF                    textcolor;
    WORD                        extent;

    info = (TBinfo *)GetWindowLong( hwnd, 0 );
    switch( msg ) {
    case WM_CREATE:
        info = (TBinfo *)( ( (CREATESTRUCT *)lparam )->lpCreateParams );
        SetWindowLong( hwnd, 0, (DWORD)info );
        CreateTheListBox( hwnd, info, STYLE_FLAGS );
        SetWindowLong( info->boxhwnd, GWL_STYLE,
            WS_VISIBLE | LBS_OWNERDRAWFIXED | WS_VSCROLL | WS_HSCROLL
            | LBS_NOTIFY | WS_CHILD | LBS_NOINTEGRALHEIGHT | WS_BORDER
            | LBS_EXTENDEDSEL );
        break;
    case WM_MEASUREITEM:
        mis = (MEASUREITEMSTRUCT *) lparam;
        dc = GetDC( hwnd );
        old_font = SelectObject( dc, info->font );
        GetTextMetrics( dc, &textmetric );
        SelectObject( dc, old_font );
        ReleaseDC( hwnd, dc );
        mis->itemHeight = textmetric.tmHeight;
        mis->CtlType = ODT_LISTBOX;
        break;
    case WM_DRAWITEM:
        dis = (DRAWITEMSTRUCT *)lparam;
        text = info->fmt( dis->itemID );
        if( text == NULL ) break;
        if( dis->itemState & ODS_SELECTED
            && !( info->flags & LIST_NOSELECT ) ) {
            bkgrndcolor = GetSysColor( COLOR_HIGHLIGHT );
            textcolor = GetSysColor( COLOR_HIGHLIGHTTEXT );
        } else {
            bkgrndcolor = GetSysColor( COLOR_WINDOW );
            textcolor = GetSysColor( COLOR_WINDOWTEXT );
        }

        extent = LOWORD( GetTextExtent( dis->hDC, text,
                         strlen( text ) + 1 ) );
        if( extent > info->extent ) {
            SendMessage( info->boxhwnd, LB_SETHORIZONTALEXTENT, extent, 0L );
            info->extent = extent;
        }

        textcolor = SetTextColor( dis->hDC, textcolor );
        bkgrndcolor = SetBkColor( dis->hDC, bkgrndcolor );
        ExtTextOut( dis->hDC, dis->rcItem.left, dis->rcItem.top,
                        ETO_OPAQUE, &dis->rcItem,
                        text, strlen( text ), NULL );
        if( dis->itemState & ODS_FOCUS ) {
            DrawFocusRect( dis->hDC, &dis->rcItem );
        }
        SetTextColor( dis->hDC, textcolor );
        SetBkColor( dis->hDC, bkgrndcolor );
        break;

    /*
     * These messages are really for the list box
     */

    case LB_GETTEXT:
        if( wparam > info->itemcnt ) return( LB_ERR );
        text = info->fmt( wparam );
        strcpy( (char*)lparam, text );
        break;
    case LB_GETCURSEL:
    case LB_GETCOUNT:
    case LB_GETTOPINDEX:
    case LB_SETTOPINDEX:
    case LB_SETCURSEL:
    case LB_GETSELCOUNT:
    case LB_GETSELITEMS:
        return( SendMessage( info->boxhwnd, msg, wparam, lparam ) );
        break;
    case LB_RESETCONTENT:
        SetBoxCnt( info->boxhwnd, 0 );
        break;
    case WM_SIZE:
        SizeTheListBox( hwnd, info );
        break;
    case WM_COMMAND:
        /* pass messages from the list box onto the real owner */
        SendMessage( info->parent, msg, wparam, lparam );
        break;
    case WM_DESTROY:
        MemFree( info );
        break;
    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( FALSE );
}

/*
 * RegTextBox - register the TextBox window class
 */
BOOL RegTextBox( HANDLE instance ) {

    WNDCLASS    wc;

    wc.style = 0L;
    wc.lpfnWndProc = (WNDPROC)TextBoxProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 4;
    wc.hInstance = instance;
    wc.hIcon = 0L;
    wc.hCursor = LoadCursor( NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.lpszMenuName = 0L;
    wc.lpszClassName = TB_CLASS_NAME;
    return( RegisterClass( &wc ) );
}

/*
 * CreateTextBox - create a text box
 */
TextBoxHdl CreateTextBox( HINSTANCE instance, HWND parent, HFONT font,
                        HMENU boxid, char *(*fmt)(unsigned), WORD flags ) {

    HWND                hwnd;
    TBinfo              *info;

    info = MemAlloc( sizeof( TBinfo ) );
    info->fmt = fmt;
    info->itemcnt = 0;
    info->font = font;
    info->parent = parent;
    info->id = boxid;
    info->extent = 0;
    info->old_area.top = 0;
    info->old_area.left = 0;
    info->old_area.right = 0;
    info->old_area.bottom = 0;
    info->flags = flags;

    hwnd = CreateWindow(
        TB_CLASS_NAME,                  /* Window class name */
        "",                             /* Window caption */
        WS_CHILD | WS_VISIBLE,          /* Window style */
        0,                              /* Initial X position */
        0,                              /* Initial Y position */
        0,                              /* Initial X size */
        0,                              /* Initial Y size */
        parent,                         /* Parent window handle */
        NULL,                           /* Window menu handle */
        instance,                       /* Program instance handle */
        info );                         /* Create parameters */
    return( (TextBoxHdl)hwnd );
}

/*
 * RedrawBox - redraw the item specified by index or the whole box
 *              if index = -1
 */

void RedrawBox( TextBoxHdl hdl, int index ) {

    TBinfo      *info;
    RECT        area;
    LRESULT     ret;

    info = (TBinfo *)GetWindowLong( (HWND)hdl, 0 );
    if( index == -1 ) {
        InvalidateRect( info->boxhwnd, NULL, TRUE );
    } else {
        ret = SendMessage( info->boxhwnd, LB_GETITEMRECT, index, (LPARAM)&area );
        if( ret != LB_ERR ) {
            InvalidateRect( info->boxhwnd, &area, TRUE );
        }
    }
    UpdateWindow( info->boxhwnd );
}

/*
 * SetBoxCnt - set the number of items in the list box
 */

void SetBoxCnt( TextBoxHdl hdl, unsigned cnt ) {

    int         i;
    TBinfo      *info;

    info = (TBinfo *)GetWindowLong( (HWND)hdl, 0 );
    if( info->itemcnt > cnt ) {
        for( i = info->itemcnt - 1; i >= cnt; --i ) {
            SendMessage( info->boxhwnd, LB_DELETESTRING, i, 0L );
        }
    } else {
        for( i = info->itemcnt; i < cnt; ++i ) {
            SendMessage( info->boxhwnd, LB_ADDSTRING, 0, i );
        }
    }
    info->itemcnt = cnt;
    InvalidateRect( info->boxhwnd, NULL, TRUE );
    UpdateWindow( info->boxhwnd );
}

/*
 * SetBoxFont - reset the font for a box
 */

void SetBoxFont( TextBoxHdl hwnd, HFONT font ) {

    TBinfo              *info;
    unsigned            itemcnt;
    DWORD               style;
    LRESULT             top;

    info = (TBinfo *)GetWindowLong( hwnd, 0 );
    style = GetWindowLong( info->boxhwnd, GWL_STYLE );
    top = SendMessage( info->boxhwnd, LB_GETTOPINDEX, 0, 0L );
    DestroyWindow( info->boxhwnd );
    itemcnt = info->itemcnt;
    info->itemcnt = 0;
    info->font = font;
    CreateTheListBox( hwnd, info, style );
    SetBoxCnt( hwnd, itemcnt );
    SendMessage( info->boxhwnd, LB_SETTOPINDEX, top, 0L );
    SizeTheListBox( hwnd, info );
}

void SetListBoxForAdd( TextBoxHdl hwnd, BOOL add ) {

    TBinfo              *info;
    DWORD               style;
    unsigned            itemcnt;
    LRESULT             top;

    info = (TBinfo *)GetWindowLong( hwnd, 0 );
    style = GetWindowLong( info->boxhwnd, GWL_STYLE );
    top = SendMessage( info->boxhwnd, LB_GETTOPINDEX, 0, 0L );
    DestroyWindow( info->boxhwnd );
    itemcnt = info->itemcnt;
    info->itemcnt = 0;
    if( add ) {
        style |= LBS_EXTENDEDSEL;
    } else {
        style &= ~LBS_EXTENDEDSEL;
    }
    CreateTheListBox( hwnd, info, style );
    SetBoxCnt( hwnd, itemcnt );
    SendMessage( info->boxhwnd, LB_SETTOPINDEX, top, 0L );
    SizeTheListBox( hwnd, info );
}
