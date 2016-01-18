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
* Description:  Hint status bar window.
*
****************************************************************************/


#include "commonui.h"
#include "bool.h"
#include "hint.h"
#include "uistr.gh"
#include "mem.h"
#include "font.h"

typedef struct {
    HWND                parent;
    MSGID               curmsg;
    WORD                hintlen;
    const MenuItemHint  *hints;
} HintWndInfo;

#define HINT_PROP_ID    "info"

/*
 * getItemMsg - find the hint message for the specified menu item
 */
static MSGID getItemMsg( statwnd *wnd, int menuid )
{
    WORD                i;
    HWND                hint;
    HLOCAL              hinfo;
    HintWndInfo         *info;
    const MenuItemHint  *hinttable;
    MSGID               msgid;

    hint = GetHintHwnd( wnd );
    hinfo = GetProp( hint, HINT_PROP_ID );
    info = LocalLock( hinfo );
    hinttable = info->hints;
    msgid = HINT_EMPTY;
    if( hinttable != NULL ) {
        for( i = 0; i < info->hintlen; i++ ) {
            if( hinttable[i].menuid == menuid ) {
                msgid = hinttable[i].msgid;
                break;
            }
        }
    }
    LocalUnlock( hinfo );
    return( msgid );

} /* getItemMsg */

/*
 * updateHintText - updated the text shown when a menu item is selected
 */
static void updateHintText( statwnd *wnd, MSGID msgid )
{
    HDC         dc;
    HFONT       font;
    char        *str;
    HWND        hint;
    HLOCAL      hinfo;
    HintWndInfo *info;

    hint = GetHintHwnd( wnd );
    dc = GetDC( hint );
    font = GetMonoFont();
    str = AllocRCString( msgid );
    StatusWndDrawLine( wnd, dc, font, str, DT_LEFT | DT_SINGLELINE | DT_VCENTER  );
    ReleaseDC( hint, dc );
    FreeRCString( str );
    hinfo = GetProp( hint, HINT_PROP_ID );
    info = LocalLock( hinfo );
    info->curmsg = msgid;
    LocalUnlock( hinfo );

} /* updateHintText */

/*
 * HintToolbar - handle the selection or deselection of a menu item
 */
void HintToolBar( statwnd *wnd, int menuid, bool select )
{
    MSGID   msgid;

    if( select ) {
        msgid = getItemMsg( wnd, menuid );
    } else {
        msgid = HINT_EMPTY;
    }
    updateHintText( wnd, msgid );

} /* HintToolbar */

/*
 * SizeHintBar - resize the hint bar
 */
WORD SizeHintBar( statwnd *wnd )
{
    HLOCAL      hinfo;
    HintWndInfo *info;
    HFONT       font;
    HFONT       oldfont;
    HDC         dc;
    SIZE        sz;
    RECT        area;
    HWND        hint;

    hint = GetHintHwnd( wnd );
    hinfo = GetProp( hint, HINT_PROP_ID );
    info = LocalLock( hinfo );
    dc = GetDC( hint );
    font = GetMonoFont();
    oldfont = SelectObject( dc, font );
    GetTextExtentPoint( dc, "A", 1, &sz );
    SelectObject( dc, oldfont );
    ReleaseDC( hint, dc );
    GetClientRect( info->parent, &area );
    area.top = area.bottom - sz.cy - TOTAL_VERT;
    MoveWindow( hint, area.left, area.top, area.right - area.left, area.bottom - area.top, TRUE );
    updateHintText( wnd, info->curmsg );
    GetWindowRect( hint, &area );
    LocalUnlock( hinfo );
    return( (WORD)( area.bottom - area.top ) );

} /* SizeHintBar */

/*
 * HintMenuSelect - handle the WM_MENUSELECT message
 */
void HintMenuSelect( statwnd *wnd, HWND hwnd, WPARAM wparam, LPARAM lparam )
{
    HMENU       menu;
    WORD        flags;
    MSGID       msgid;

    menu = GetMenu( hwnd );
    flags = GET_WM_MENUSELECT_FLAGS( wparam, lparam );
    if( flags == (WORD)-1 && GET_WM_MENUSELECT_HMENU( wparam, lparam ) == (HMENU)NULL ) {
        updateHintText( wnd, HINT_EMPTY );
    } else if( flags & (MF_SYSMENU | MF_SEPARATOR) ) {
        updateHintText( wnd, HINT_EMPTY );
    } else if( flags & MF_POPUP ) {
        //
        // NYI handle popup message hints
        //
    } else {
        msgid = getItemMsg( wnd, GET_WM_MENUSELECT_ITEM( wparam, lparam ) );
        updateHintText( wnd, msgid );
    }

} /* HintMenuSelect */

/*
 * SetHintsText - set the hint text for the specified menu items
 */
const MenuItemHint *SetHintsText( statwnd *wnd, const MenuItemHint *hints, WORD cnt )
{
    HintWndInfo         *info;
    const MenuItemHint  *ret;
    HLOCAL              hinfo;
    HWND                hint;

    hint = GetHintHwnd( wnd );
    hinfo = GetProp( hint, HINT_PROP_ID );
    info = LocalLock( hinfo );
    ret = info->hints;
    info->hints = hints;
    info->hintlen = cnt;
    LocalUnlock( hinfo );
    return( ret );

} /* SetHintsText */

/*
 * HintWndCreate - create a hint status bar
 */
statwnd *HintWndCreate( HWND parent, RECT *size, HINSTANCE hinstance, LPVOID lpvParam )
{
    statwnd             *wnd;
    HintWndInfo         *info;
    HLOCAL              hinfo;

    wnd = StatusWndStart();
    StatusWndCreate( wnd, parent, size, hinstance, lpvParam );
    hinfo = LocalAlloc( LHND, sizeof( HintWndInfo ) );
    info = LocalLock( hinfo );
    info->curmsg = HINT_EMPTY;
    info->hints = NULL;
    info->hintlen = 0;
    info->parent = parent;
    LocalUnlock( hinfo );

    SetProp( GetHintHwnd( wnd ), HINT_PROP_ID, hinfo );
    return( wnd );

} /* HintWndCreate */

/*
 * HintWndDestroy - destroy a hint status bar
 */
void HintWndDestroy( statwnd *wnd )
{
    HLOCAL              hinfo;
    HWND                hint;

    hint = GetHintHwnd( wnd );
    hinfo = GetProp( hint, HINT_PROP_ID );
    LocalFree( hinfo );
    StatusWndDestroy( wnd );

} /* HintWndDestroy */

/*
 * HintWndInit - initialize the hint status bar
 */
bool HintWndInit( HINSTANCE hinstance, statushook hook, int extra )
{
    return( StatusWndInit( hinstance, hook, extra, NULL ) );

} /* HintWndInit */

/*
 * HintFini - done with all hint status bars
 */
void HintFini( void )
{
    StatusWndFini();

} /* HintFini */

/*
 * GetHintHwnd - get the window handle of a hint status bar
 */
HWND GetHintHwnd( statwnd *wnd )
{
    return( *(HWND *)wnd );

} /* GetHintHwnd */
