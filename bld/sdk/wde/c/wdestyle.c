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


#include "precomp.h"

#include "wdeglbl.h"
#include "wde_rc.h"
#include "wdestyle.h"

typedef struct {
    DialogStyle style;
    char        *str;
    int         id;
    int         id2;
} WdeWinStyle;

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static WdeWinStyle WdeStyleMap[] = {
    { WS_CAPTION,       "WS_CAPTION",       IDB_WS_CAPTION,         0 },
    { WS_POPUP,         "WS_POPUP",         IDB_WS_POPUP,           0 },
    { WS_CHILD,         "WS_CHILD",         IDB_WS_CHILD,           0 },
    { WS_CLIPSIBLINGS,  "WS_CLIPSIBLINGS",  IDB_WS_CLIPSIBLINGS,    0 },
    { WS_CLIPCHILDREN,  "WS_CLIPCHILDREN",  IDB_WS_CLIPCHILDREN,    0 },
    { WS_VISIBLE,       "WS_VISIBLE",       IDB_WS_VISIBLE,         0 },
    { WS_DISABLED,      "WS_DISABLED",      IDB_WS_DISABLED,        0 },
    { WS_MINIMIZE,      "WS_MINIMIZE",      IDB_WS_MINIMIZE,        0 },
    { WS_MAXIMIZE,      "WS_MAXIMIZE",      IDB_WS_MAXIMIZE,        0 },
    { WS_BORDER,        "WS_BORDER",        IDB_WS_BORDER,          0 },
    { WS_DLGFRAME,      "WS_DLGFRAME",      IDB_WS_DLGFRAME,        0 },
    { WS_VSCROLL,       "WS_VSCROLL",       IDB_WS_VSCROLL,         0 },
    { WS_HSCROLL,       "WS_HSCROLL",       IDB_WS_HSCROLL,         0 },
    { WS_SYSMENU,       "WS_SYSMENU",       IDB_WS_SYSMENU,         0 },
    { WS_THICKFRAME,    "WS_THICKFRAME",    IDB_WS_THICKFRAME,      0 },
    { WS_GROUP,         "WS_GROUP",         IDB_WS_GROUP,           IDB_WS_MINIMIZEBOX },
    { WS_TABSTOP,       "WS_TABSTOP",       IDB_WS_TABSTOP,         IDB_WS_MAXIMIZEBOX },
    { 0,                NULL,               0,                      0 }
};

static WdeWinStyle *WdeGetWdeWinStyleFromStyle( DialogStyle style )
{
    int i;

    for( i = 0; WdeStyleMap[i].str != NULL; i++ ) {
        if( (WdeStyleMap[i].style & style) == WdeStyleMap[i].style ) {
            return( &WdeStyleMap[i] );
        }
    }

    return( NULL );
}

static void WdeSetWinStyle( HWND hDlg, DialogStyle style, DialogStyle styles )
{
    WdeWinStyle *ws;
    HWND        control;

    ws = WdeGetWdeWinStyleFromStyle( style );
    if( ws != NULL ) {
        if( (style & styles) == style ) {
            control = GetDlgItem( hDlg, ws->id );
            if( control != (HWND)NULL ) {
                CheckDlgButton( hDlg, ws->id, 1 );
            } else if( ws->id2 != 0 ) {
                CheckDlgButton( hDlg, ws->id2, 1 );
            }
        }
    }
}

static DialogStyle WdeGetWinStyle( HWND hDlg, DialogStyle style )
{
    WdeWinStyle *ws;

    ws = WdeGetWdeWinStyleFromStyle( style );
    if( ws != NULL ) {
        if( IsDlgButtonChecked( hDlg, ws->id ) ||
            (ws->id2 != 0 && IsDlgButtonChecked( hDlg, ws->id2 )) ) {
            return( style );
        }
    }
    return( (DialogStyle)0 );
}

WPARAM WdeGetKeys( void )
{
    uint_8  buf[256];
    WPARAM  new_wparam;

    new_wparam = 0;

    GetKeyboardState( buf );

    if( buf[VK_CONTROL] & 0x80 ) {
        new_wparam |= MK_CONTROL;
    }
    if( buf[VK_SHIFT] & 0x80 ) {
        new_wparam |= MK_SHIFT;
    }
    if( buf[VK_LBUTTON] & 0x80 ) {
        new_wparam |= MK_LBUTTON;
    }
    if( buf[VK_RBUTTON] & 0x80 ) {
        new_wparam |= MK_RBUTTON;
    }
    if( buf[VK_MBUTTON] & 0x80 ) {
        new_wparam |= MK_MBUTTON;
    }

    return( new_wparam );
}

void WdeSetWinStyles( HWND hDlg, DialogStyle style, DialogStyle mask )
{
    DialogStyle s;
    int         i;

    for( i = 0, s = ((DialogStyle)1 << 16); i < 16; i++, s <<= 1 ) {
        if( mask & s ) {
            WdeSetWinStyle( hDlg, s, style );
        }
    }
}

void WdeGetWinStyles( HWND hDlg, DialogStyle *style, DialogStyle mask )
{
    DialogStyle s;
    int         i;

    *style &= (DialogStyle)(0x0000ffff | ~mask);

    for( i = 0, s = ((DialogStyle)1 << 16); i < 16; i++, s <<= 1 ) {
        if( mask & s ) {
            *style |= WdeGetWinStyle( hDlg, s );
        }
    }
}

#if 0
static WdeWinStyle *WdeGetWdeWinStyleFromID( int id )
{
    int i;

    for( i = 0; WdeStyleMap[i].str != NULL; i++ ) {
        if( WdeStyleMap[i].id == id ) {
            return( &WdeStyleMap[i] );
        }
    }
    return( NULL );
}
#endif

