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


#include "windows.h"
#include "hint.h"
#include "rcstr.h"
#include "mem.h"
#include "ldstr.h"
#include "win1632.h"
#include "font.h"

typedef struct {
    HWND                parent;
    WORD                curmsg;
    WORD                hintlen;
    MenuItemHint        *hints;
}HintWndInfo;

static WORD     winExtra;

static WORD getItemMsg( statwnd *wnd, WORD menuid ) {
    WORD                i;
    HWND                hint;
    HintWndInfo         *info;
    MenuItemHint        *hinttable;

    hint = GetHintHwnd( wnd );
    info = (HintWndInfo *)GetWindowLong( hint, winExtra );
    hinttable = info->hints;
    if( hinttable != NULL ) {
        for( i=0; i < info->hintlen; i++ ) {
            if( hinttable[i].menuid == menuid ) return( hinttable[i].msgid );
        }
    }
    return( HINT_EMPTY );
}

static updateHintText( statwnd *wnd, WORD msgid ) {
    HDC         dc;
    HFONT       font;
    char        *str;
    HWND        hint;
    HintWndInfo *info;

    hint = GetHintHwnd( wnd );
    dc = GetDC( hint );
    font = GetMonoFont();
    str = AllocRCString( msgid );
    StatusWndDrawLine( wnd, dc, font, str,
                        DT_LEFT | DT_SINGLELINE | DT_VCENTER  );
    ReleaseDC( hint, dc );
    FreeRCString( str );
    info = (HintWndInfo *)GetWindowLong( hint, winExtra );
    info->curmsg = msgid;
}

void HintToolBar( statwnd *wnd, UINT menuid, BOOL select ) {

    WORD        msgid;

    if( select ) {
        msgid = getItemMsg( wnd, menuid );
    } else {
        msgid = HINT_EMPTY;
    }
    updateHintText( wnd, msgid );
}

WORD SizeHintBar( statwnd *wnd ) {

    HintWndInfo *info;
    HFONT       font;
    HFONT       oldfont;
    HDC         dc;
    SIZE        sz;
    RECT        area;
    HWND        hint;

    hint = GetHintHwnd( wnd );
    info = (HintWndInfo *)GetWindowLong( hint, winExtra );
    dc = GetDC( hint );
    font = GetMonoFont();
    oldfont = SelectObject( dc, font );
    GetTextExtentPoint( dc, "A", 1, &sz );
    SelectObject( dc, oldfont );
    ReleaseDC( hint, dc );
    GetClientRect( info->parent, &area );
    area.top = area.bottom - sz.cy - TOTAL_VERT;
    MoveWindow( hint, area.left, area.top, area.right - area.left,
                area.bottom - area.top, TRUE );
    updateHintText( wnd, info->curmsg );
    return( area.bottom - area.top );
}

void HintMenuSelect( statwnd *wnd, HWND hwnd, WPARAM wparam, LPARAM lparam )
{
    HMENU       menu;
    WORD        flags;
    WORD        msgid;

    menu = GetMenu( hwnd );
    flags = GET_WM_MENUSELECT_FLAGS( wparam, lparam );
    if ( ( flags == (WORD)-1 ) &&
         ( GET_WM_MENUSELECT_HMENU( wparam, lparam ) == (HMENU)NULL ) ) {
        updateHintText( wnd, HINT_EMPTY );
    } else if ( flags & (MF_SYSMENU | MF_SEPARATOR) ) {
        updateHintText( wnd, HINT_EMPTY );
    } else if ( flags & MF_POPUP ) {
        //
        // NYI handle popup message hints
        //
    } else {
        msgid = getItemMsg( wnd, GET_WM_MENUSELECT_ITEM( wparam, lparam ) );
        updateHintText( wnd, msgid );
    }
}

MenuItemHint *SetHintText( statwnd *wnd, MenuItemHint *hints, WORD cnt ) {

    HintWndInfo         *info;
    MenuItemHint        *ret;
    HWND                hint;

    hint = GetHintHwnd( wnd );
    info = (HintWndInfo *)GetWindowLong( hint, winExtra );
    ret = info->hints;
    info->hints = hints;
    info->hintlen = cnt;
    return( ret );
}

statwnd *HintWndCreate( HWND parent, RECT *size,
                          HINSTANCE hinstance, LPVOID lpvParam )
{
    statwnd             *wnd;
    HintWndInfo         *info;

    wnd = StatusWndStart();
    StatusWndCreate( wnd, parent, size, hinstance, lpvParam );
    info = MemAlloc( sizeof( HintWndInfo ) );
    info->curmsg = HINT_EMPTY;
    info->hints = NULL;
    info->hintlen = 0;
    info->parent = parent;
    SetWindowLong( GetHintHwnd( wnd ), winExtra, (DWORD)info );
    return( wnd );
}

void HintWndDestroy( statwnd *wnd ) {

    HintWndInfo         *info;
    HWND                hint;

    hint = GetHintHwnd( wnd );
    info = (HintWndInfo *)GetWindowLong( hint, winExtra );
    MemFree( info );
    StatusWndDestroy( wnd );
}

int HintWndInit( HINSTANCE hinstance, statushook hook, int extra ) {

    winExtra = extra;
    return( StatusWndInit( hinstance, hook, extra + sizeof( void * ) ) );
}

void HintFini( void ) {
    StatusWndFini();
}

HWND GetHintHwnd( statwnd *wnd ) {
    return( wnd->win );
}
