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


#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "drwatcom.h"

#define MAX_LB_LINES            500

/*
 * SetListBoxFont - set the font
 */

void SetListBoxFont( LBoxHdl *lb ) {

    char        buf[256];
    HFONT       newfont, oldfont;
    SIZE        sz;
    HDC         hdc;
    LRESULT     cnt;

    SetMonoFont( lb->hwnd );
    InvalidateRect( lb->hwnd, NULL, TRUE );
    UpdateWindow( lb->hwnd );
    if( lb->longest_item != -1 ) {
        SendMessage( lb->hwnd, LB_GETTEXT, lb->longest_item, (LPARAM)buf );
        hdc = GetDC( lb->hwnd );
        newfont = GetMonoFont();
        oldfont = SelectObject( hdc, newfont);
        GetTextExtentPoint( hdc, buf, strlen( buf ), &sz );
        lb->text_width = sz.cx;
    } else {
        cnt = SendMessage( lb->hwnd, LB_GETCOUNT, 0, 0 );
        cnt--;
        hdc = GetDC( lb->hwnd );
        newfont = GetMonoFont();
        oldfont = SelectObject( hdc, newfont);
        lb->text_width = 0;
        while( cnt != -1 ) {
            SendMessage( lb->hwnd, LB_GETTEXT, cnt, (LPARAM)buf );
            GetTextExtentPoint( hdc, buf, strlen( buf ), &sz );
            if( sz.cx > lb->text_width ) {
                lb->text_width = sz.cx;
                lb->longest_item = cnt;
            }
            cnt--;
        }
    }
    SelectObject( hdc, oldfont);
    ReleaseDC(lb->hwnd, hdc);
    SendMessage( lb->hwnd, LB_SETHORIZONTALEXTENT, lb->text_width, 0 );
}

/*
 * ClearListBox - reset the lb content
 */

void ClearListBox( LBoxHdl *lb ) {
    lb->text_width = 0;
    lb->longest_item = -1;
    lb->line_cnt = 0;
    SendMessage( lb->hwnd, LB_RESETCONTENT, 0, 0L );
    SendMessage( lb->hwnd, LB_SETHORIZONTALEXTENT, lb->text_width, 0 );
}

/*
 * MoveListBox - move/resize the ListBox
 */

void MoveListBox( LBoxHdl *lb, int x, int y, int width, int hieght ) {

    MoveWindow( lb->hwnd, x, y, width, hieght, TRUE );

    /* this is a kludge to make sure the window is properly
     * refreshed when it is horizontally scrolled */

    InvalidateRect( lb->hwnd, NULL, TRUE );
    UpdateWindow( lb->hwnd );
}

/*
 * CreateListBox - create the list box
 */

LBoxHdl *CreateListBox( HWND parent ) {

    LBoxHdl             *lb;

    lb = MemAlloc( sizeof( LBoxHdl ) );
    lb->longest_item = -1;
    lb->text_width = 0;
    lb->line_cnt = 0;
    lb->text_width = 0;
    lb->hwnd = CreateWindow(
                    "listbox",                  /* Window class name */
                    NULL,                       /* Window caption */
                    WS_CHILD | WS_VISIBLE
                    | WS_BORDER
                    | WS_VSCROLL
                    | WS_HSCROLL
                    | LBS_USETABSTOPS
                    | LBS_NOTIFY,               /* Window style */
                    0,                          /* Initial X position */
                    0,                          /* Initial Y position */
                    0,                          /* Initial X size */
                    0,                          /* Initial Y size */
                    parent,                     /* Parent window handle */
                    (HMENU)LISTBOX_1,           /* child id */
                    Instance,                   /* Program instance handle */
                    NULL );                     /* Create parameters */
    SetMonoFont( lb->hwnd );
    return( lb );
}

/*
 * doLBPrintf - printf to a list box
 */
DWORD doLBPrintf( LBoxHdl *lb, char *str, va_list al )
{
    char        tmp[256];
    HDC         dc;
    SIZE        sz;
    LRESULT     item;
    HFONT       oldfont, newfont;

    vsprintf( tmp, str, al );
    item = SendMessage( lb->hwnd, LB_ADDSTRING, 0, (LONG)(LPSTR)tmp );
    lb->line_cnt++;
    SendMessage( lb->hwnd, LB_SETCURSEL, item, 0L );
    dc = GetDC( lb->hwnd );
    newfont = GetMonoFont();
    oldfont = SelectObject( dc, newfont);
    GetTextExtentPoint( dc, tmp, strlen( tmp ), &sz );
    SelectObject( dc, oldfont);
    ReleaseDC( lb->hwnd, dc );
    if( sz.cx > lb->text_width ) {
        lb->text_width = sz.cx;
        SendMessage( lb->hwnd, LB_SETHORIZONTALEXTENT, lb->text_width, 0 );
        lb->longest_item = item;
    }
    if( lb->line_cnt > MAX_LB_LINES ) {
        SendMessage( lb->hwnd, LB_DELETESTRING, 0, 0 ); /* delete the oldest string */
        lb->line_cnt--;
        if( lb->longest_item != 0 && lb->longest_item != -1 ) {
            lb->longest_item --;
        }
    }
    return( item );
} /* LBPrintf */


DWORD LBPrintf( LBoxHdl *lb, DWORD msgid, ... )
{
    char        *str;
    DWORD       ret;
    va_list     al;

    va_start( al, msgid );
    str = AllocRCString( msgid );
    ret = doLBPrintf( lb, str, al );
    FreeRCString( str );
    va_end( al );
    return( ret );
}

DWORD LBStrPrintf( LBoxHdl *lb, char *str, ... )
{
    DWORD       ret;
    va_list     al;

    va_start( al, str );
    ret = doLBPrintf( lb, str, al );
    va_end( al );
    return( ret );
}

HWND GetListBoxHwnd( LBoxHdl *lb ) {
    return( lb->hwnd );
}

void FiniListBox( LBoxHdl *lb ) {
    MemFree( lb );
}
