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


#include "variety.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#define INCL_WIN
#define INCL_GPI
#include <wos2.h>
#include "win.h"
#include "pmmenu.h"

static  char            *WatcomClass = { "WATCOM" };
static  HWND            ClientWindow;
static  HMQ             hMessageQueue = { NULL };
static  FATTRS          FontAttrs;

#define DISPLAY(x)      WinMessageBox( HWND_DESKTOP, NULL, x, "Error", 0, MB_APPLMODAL | MB_NOICON | MB_OK | MB_MOVEABLE );

extern void _SetWinMenuHandle( HWND hmenu );
extern HWND _GetWinMenuHandle( void );

_WCRTLINK int   __InitDefaultWin() {
//==================================

    ULONG       style;
    RECTL       rcl;
    HWND        menhdl;
    HWND        submenu;
    MENUITEM    *menudesc;
    MENUITEM    menus;
    SWP         swp;

    _ClassName = WatcomClass;

    _AnchorBlock = WinInitialize( 0 );
    if( _AnchorBlock == 0 ) return( 0 );
    hMessageQueue = WinCreateMsgQueue( _AnchorBlock, 0 );
    if( hMessageQueue == 0 ) return( 0 );
    if( !WinRegisterClass( _AnchorBlock, _ClassName, (PFNWP)_MainDriver,
                           CS_SIZEREDRAW, 0 ) ) {
        return( 0 );
    }

    _InitMainWindowData( 0 );

    style = FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER | FCF_MINMAX |
            FCF_SHELLPOSITION | FCF_TASKLIST;
    _MainFrameWindow = WinCreateStdWindow( HWND_DESKTOP,
                        WS_VISIBLE | WS_CLIPCHILDREN,
                        &style, _ClassName, "", 0, NULL, 0, &ClientWindow );

    if( _MainFrameWindow == 0 ) return( 0 );
    WinSendMsg( _MainFrameWindow, WM_SETICON,
        MPFROMLONG( WinQuerySysPointer( HWND_DESKTOP, SPTR_APPICON, TRUE ) ), 0 );
    WinQueryWindowRect( _MainWindow, &rcl );
    WinSetWindowPos( _MainFrameWindow, HWND_TOP,
                rcl.xLeft,
                rcl.yBottom,
                rcl.xLeft - rcl.xRight,
                rcl.yTop - rcl.yBottom,
                SWP_SHOW | SWP_SIZE | SWP_MOVE | SWP_MAXIMIZE );

    _InitFunctionPointers();
    _MainWindow = ClientWindow;
    WinUpdateWindow( _MainWindow );

    menudesc = &menus;
    menhdl = WinCreateMenu( _MainFrameWindow, NULL );
    WinSetWindowBits( menhdl, QWL_STYLE, MS_ACTIONBAR, MS_ACTIONBAR );
    submenu = WinCreateMenu( menhdl, NULL );

    menudesc->afStyle = MIS_TEXT;
    menudesc->afAttribute = 0;
    menudesc->hwndSubMenu = NULL;
    menudesc->hItem = 0;

    menudesc->iPosition = 0;
    menudesc->id = DID_FILE_SAVE;
    if ( MIT_ERROR == (BOOL)WinSendMsg( submenu, ( ULONG )MM_INSERTITEM, MPFROMP( menudesc ), MPFROMP( "Save As..." ) ) ) abort();
    menudesc->iPosition = 1;
    menudesc->id = DID_FILE_CLEAR;
    if ( MIT_ERROR == (BOOL)WinSendMsg( submenu, ( ULONG )MM_INSERTITEM, MPFROMP( menudesc ), MPFROMP( "Set Lines Between Clears..." ) ) ) abort();
    menudesc->afStyle = MIS_SEPARATOR;
    menudesc->iPosition = 2;
    menudesc->id = 0;
    if ( MIT_ERROR == (BOOL)WinSendMsg( submenu, ( ULONG )MM_INSERTITEM, MPFROMP( menudesc ), MPFROMP( "Exit" ) ) ) abort();
    menudesc->afStyle = MIS_TEXT;
    menudesc->iPosition = 3;
    menudesc->id = DID_FILE_EXIT;
    if ( MIT_ERROR == (BOOL)WinSendMsg( submenu, ( ULONG )MM_INSERTITEM, MPFROMP( menudesc ), MPFROMP( "Exit" ) ) ) abort();
    menudesc->iPosition = 0;
    menudesc->id = DID_MAIN_FILE;
    menudesc->hwndSubMenu = submenu;
    menudesc->afStyle = MIS_TEXT | MIS_SUBMENU;
    if ( MIT_ERROR == (BOOL)WinSendMsg( menhdl, ( ULONG )MM_INSERTITEM, MPFROMP( menudesc ), MPFROMP( "~File" ) ) ) abort();

    submenu = WinCreateMenu( menhdl, NULL );
    menudesc->afStyle = MIS_TEXT;
    menudesc->iPosition = 0;
    menudesc->id = DID_EDIT_CLEAR;
    menudesc->hwndSubMenu = NULL;
    if ( MIT_ERROR == (BOOL)WinSendMsg( submenu, ( ULONG )MM_INSERTITEM, MPFROMP( menudesc ), MPFROMP( "Clear" ) ) ) abort();
    menudesc->iPosition = 1;
    menudesc->id = DID_EDIT_COPY;
    if ( MIT_ERROR == (BOOL)WinSendMsg( submenu, ( ULONG )MM_INSERTITEM, MPFROMP( menudesc ), MPFROMP( "Copy" ) ) ) abort();
    menudesc->iPosition = 1;
    menudesc->id = DID_MAIN_EDIT;
    menudesc->hwndSubMenu = submenu;
    menudesc->afStyle = MIS_SUBMENU;
    if ( MIT_ERROR == (BOOL)WinSendMsg( menhdl, ( ULONG )MM_INSERTITEM, MPFROMP( menudesc ), MPFROMP( "~Edit" ) ) ) abort();

    submenu = WinCreateMenu( menhdl, NULL );
    _SetWinMenuHandle( submenu );
    menudesc->iPosition = 2;
    menudesc->afAttribute = 0;
    menudesc->id = DID_MAIN_WIND;
    menudesc->hwndSubMenu = submenu;
    menudesc->afStyle = MIS_SUBMENU;
    if ( MIT_ERROR == (BOOL)WinSendMsg( menhdl, ( ULONG )MM_INSERTITEM, MPFROMP( menudesc ), MPFROMP( "~Windows" ) ) ) abort();

    submenu = WinCreateMenu( menhdl, NULL );
    menudesc->afStyle = MIS_TEXT;
    menudesc->iPosition = 0;
    menudesc->id = DID_HELP_ABOUT;
    menudesc->hwndSubMenu = NULL;
    if ( MIT_ERROR == (BOOL)WinSendMsg( submenu, ( ULONG )MM_INSERTITEM, MPFROMP( menudesc ), MPFROMP( "About" ) ) ) abort();
    menudesc->iPosition = 3;
    menudesc->id = DID_MAIN_HELP;
    menudesc->hwndSubMenu = submenu;
    menudesc->afStyle = MIS_SUBMENU;
    if ( MIT_ERROR == (BOOL)WinSendMsg( menhdl, ( ULONG )MM_INSERTITEM, MPFROMP( menudesc ), MPFROMP( "~Help" ) ) ) abort();

    WinSendMsg( _MainFrameWindow, ( ULONG )WM_UPDATEFRAME, 0, 0 );
    _NewWindow( "Standard IO", 0,1,2,-1 );
    WinUpdateWindow( _MainFrameWindow );
    WinQueryWindowPos( _MainWindow, &swp );
    swp.fl |= SWP_MAXIMIZE;
    WinSetWindowPos( _MainWindow, HWND_TOP, swp.x, swp.y, swp.cx, swp.cy, SWP_MAXIMIZE | SWP_ACTIVATE | SWP_MOVE | SWP_SHOW | SWP_SIZE );

    return( 1 );
}

_WCRTLINK void  __FiniDefaultWin() {
//==================================

    if( _MainWindow != 0 ) {
        WinDestroyWindow( _MainWindow );
    }
    if( hMessageQueue != 0 ) {
        WinDestroyMsgQueue( hMessageQueue );
    }
    WinTerminate( _AnchorBlock );
}

void    _CreateFont( LPWDATA w ) {
//===============================

    LONG                cFonts = { 0 };
    LONG                num_fonts;
    FONTMETRICS *       fonts;
    int                 i;
    unsigned            font_size;
    unsigned            font_selected;
    HPS                 ps;
#ifdef _MBCS
    int                 fontChosenFlag = 0;
#endif

    /*** Prepare to search for a suitable console font ***/
    ps = WinGetPS( w->hwnd );
    num_fonts = GpiQueryFonts( ps, QF_PUBLIC, NULL, &cFonts,
                               sizeof( FONTMETRICS ), NULL );
    fonts = _MemAlloc( num_fonts * sizeof( FONTMETRICS ) );
    GpiQueryFonts( ps, QF_PUBLIC, NULL, &num_fonts, sizeof( FONTMETRICS ),
                   fonts );
    font_size = UINT_MAX;
    font_selected = 0;

    #ifdef _MBCS
        if( __IsDBCS ) {
            /*** Try to find a DBCS font ***/
            for( i = 0; i < num_fonts; ++i ) {
                if( ! (fonts[i].fsType & (FM_TYPE_DBCS|FM_TYPE_FIXED)) )  continue;
                if( fonts[i].lEmHeight > 10 )  continue;
                font_size = fonts[i].lEmHeight;
                font_selected = i;
                fontChosenFlag = 1;
                break;
            }
            /*** Try to find a regular font if can't find DBCS ***/
            if( fontChosenFlag == 0 ) {
                for( i = 0; i < num_fonts; ++i ) {
                    if( ! (fonts[i].fsType & FM_TYPE_DBCS) )  continue;
                    if( fonts[i].lEmHeight > 10 )  continue;
                    font_size = fonts[i].lEmHeight;
                    font_selected = i;
                    fontChosenFlag = 1;
                    break;
                }
            }
        }

        /*** If !__IsDBCS or if DBCS font not found, find a fixed font ***/
        if( fontChosenFlag == 0 ) {
            for( i = 0; i < num_fonts; ++i ) {
                if( ! (fonts[i].fsType & FM_TYPE_FIXED) )  continue;
                if( fonts[i].lEmHeight > 10 )  continue;
                font_size = fonts[i].lEmHeight;
                font_selected = i;
                break;
            }
        }
    #else
        /*** Try to find a suitable font ***/
        for( i = 0; i < num_fonts; ++i ) {
            if( ! (fonts[i].fsType & FM_TYPE_FIXED) )  continue;
            if( fonts[i].lEmHeight > 10 )  continue;
            font_size = fonts[i].lEmHeight;
            font_selected = i;
            break;
        }
    #endif

    /*** Set up the chosen font ***/
    FontAttrs.usRecordLength = sizeof( FATTRS );
    FontAttrs.fsSelection = 0;
    FontAttrs.lMatch = fonts[font_selected].lMatch;
    strcpy( FontAttrs.szFacename, fonts[font_selected].szFacename );
    FontAttrs.idRegistry = fonts[font_selected].idRegistry;
    FontAttrs.usCodePage = 0;
    FontAttrs.lMaxBaselineExt = 0;
    FontAttrs.lAveCharWidth = 0;
    FontAttrs.fsType = 0;
    FontAttrs.fsFontUse = 0;
    w->xchar = fonts[font_selected].lAveCharWidth;
    w->ychar = fonts[font_selected].lMaxBaselineExt + SPACE_BETWEEN_LINES;
    w->base_offset = fonts[font_selected].lMaxDescender;
    _MemFree( fonts );
    WinReleasePS( ps );
}


void    _SelectFont( HPS ps ) {
//============================

    GpiCreateLogFont( ps, NULL, FIXED_FONT, &FontAttrs );
    GpiSetCharSet( ps, FIXED_FONT );
}
