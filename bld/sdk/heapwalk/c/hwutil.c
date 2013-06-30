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


#include "heapwalk.h"
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

/*
 * CenterDlg - moves a dialog to the center of the screen
 */

void CenterDlg( HWND hwnd ) {

    HWND        parent;
    RECT        area;
    WORD        x;
    WORD        y;
    parent = (HWND)GetWindowWord( hwnd, GWW_HWNDPARENT );
    GetClientRect( parent, &area );
    x = -area.left;
    y = -area.top;
    x += GetSystemMetrics( SM_CXSCREEN ) / 2;
    y += GetSystemMetrics( SM_CYSCREEN ) / 2;
    GetWindowRect( hwnd, &area );
    x -= ( area.right - area.left ) / 2;
    y -= ( area.bottom - area.top ) / 2;
    SetWindowPos( hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
} /* CenterDlg */

/*
 * SetStaticText - set the text in a static field of a dialog
 */

void SetStaticText( HWND hwnd, int id, char *str ) {

//    SetDlgMonoFont( hwnd, id );
    SetDlgItemText( hwnd, id, str );
} /* SetStaticText */


/*
 * ErrorBox - creates a message box with of type 'type' if possible
 *            Otherwise a system modal box with an OK button is created
 *            'msg' must be 3 lines or less
 */
int ErrorBox( HWND hwnd, DWORD msgid, UINT type ) {

    int         ret;
    char        *msg;

    msg = HWGetRCString( msgid );
    ret = MessageBox( hwnd, msg, HeapWalkName, type );
    if( ret == 0 ) {
        ret = MessageBox( hwnd, msg, HeapWalkName,
                          MB_OK | MB_ICONHAND | MB_SYSTEMMODAL );
    }
    return( ret );
}

/*
 * KillPushWid - destroy a series of push windows created using
 *               MakePushWin
 */
void KillPushWin( HWND *push ) {

    HWND        *cur;

    if( push == NULL ) return;
    cur = push;
    while( *cur != NULL ) {
        DestroyWindow( *cur );
        cur++;
    }
    MemFree( push );
}

/*
 * MakePushWin - create push windows and place them at the top of a window
 */

HWND *MakePushWin( HWND hwnd, char *str, WORD cnt, ... ) {

    char        *start;
    char        *end;
    char        tmp;
    char        buf[150];
    va_list     args;
    WORD        pos;
    WORD        i;
    WORD        id;
    WORD        width;
    DWORD       extent;
    HWND        *ret;
    HDC         dc;
    HFONT       font;
    HFONT       old_font;

    pos = 0;
    strcpy( buf, str );
    end = buf;
    start = end;
    while( isspace( *end ) ) end++;
    ret = MemAlloc( (cnt + 1) * sizeof( HWND ) );
    ret[ cnt ] = NULL;
    va_start( args, cnt );
    font = GetMonoFont();

    for( i=0; i < cnt; i++ ) {
        while( !isspace( *end ) && *end != '\0' ) end++;
        while( isspace( *end ) ) end++;
        tmp = *end;
        *end = '\0';
        id = va_arg( args, WORD );
        ret[i] = CreatePushWin( hwnd, start, id, font, Instance );
        dc = GetDC( ret[i] );
        old_font = SelectObject( dc, font );
        extent = GetTextExtent( dc, start, strlen( start ) );
        width = LOWORD( extent );
        MoveWindow( ret[i], pos, 0, width, HIWORD( extent ), TRUE );
        ShowWindow( ret[i], SW_SHOW );
        *end = tmp;
        pos += width;
        start = end;
        SelectObject( dc, old_font );
        ReleaseDC( ret[i], dc );
    }
    va_end( args );
    return( ret );
}
