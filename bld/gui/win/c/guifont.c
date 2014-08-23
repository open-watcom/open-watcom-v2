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


#include "guiwind.h"
#include <string.h>
#if !defined(__OS2_PM__) && !defined(__WINDOWS_386__)
#include <commdlg.h>
#endif
#include "guifont.h"
#include "fontstr.h"
#include "guiutil.h"
#include "guixutil.h"
#include "guiscrol.h"

#define MAX_STR 256

#ifndef __OS2_PM__
static void SetFont( gui_window *wnd, HFONT font )
{
    wnd->font = font;
    GUISetRowCol( wnd, NULL );
    GUISetScroll( wnd );
    GUIEVENTWND( wnd, GUI_FONT_CHANGED, NULL );
}
#endif

bool GUIChooseFont( HFONT font, LOGFONT *lf, HWND hwnd )
{
#ifdef __OS2_PM__
    hwnd = hwnd;
    lf = lf;
    font = font;
    return( false );
#else
    CHOOSEFONT  cf;
    bool        ret;
 #if defined(__WINDOWS__)
    HINSTANCE   h;
    FARPROC     func;
  #ifdef __WINDOWS_386__
    HINDIR      hIndir;
    DWORD       lfAlias;
  #endif
 #endif

    memset( &cf, 0, sizeof( CHOOSEFONT ) );

    cf.Flags = CF_SCREENFONTS;
    if( font != NULL ) {
        GetObject( font, sizeof( LOGFONT ), (LPSTR) lf );
        cf.Flags |= CF_INITTOLOGFONTSTRUCT;
    }

 #ifndef __WINDOWS_386__
    cf.lpLogFont = lf;
 #endif
    cf.lStructSize = sizeof( CHOOSEFONT );
    cf.hwndOwner = hwnd;

 #if defined(__NT__)
    ret = ChooseFont( &cf ) != 0;
 #else
    h = LoadLibrary( "COMMDLG.DLL" );
    if( (UINT)h < 32 ) {
        return( false );
    }
    func = GetProcAddress( h, "ChooseFont" );
    if( func == NULL ) {
        return( false );
    }
  #ifdef __WINDOWS_386__
    hIndir = GetIndirectFunctionHandle( func, INDIR_PTR, INDIR_ENDLIST );
    if( hIndir == NULL ) {
        FreeLibrary( h );
        return( false );
    }
    lfAlias = AllocAlias16( (void *)lf );
    cf.lpLogFont = (LOGFONT *) lfAlias;
    ret = (short)InvokeIndirectFunction( hIndir, &cf ) != 0;
    if( lfAlias != NULL ) {
        FreeAlias16( lfAlias );
    }
  #else
    ret = ((BOOL(WINAPI *)(LPCHOOSEFONT))func)( &cf ) != 0;
  #endif
    FreeLibrary( h );
 #endif
    return( ret );
#endif
}

bool GUIChangeFont( gui_window *wnd )
{
#ifndef __OS2_PM__
    LOGFONT     lf;
    HFONT       font;

    if( !GUIChooseFont( wnd->font, &lf, wnd->hwnd ) ) {
        return( false );
    }
    font = CreateFontIndirect( &lf );
    if( font == NULL ) {
        return( false );
    }
    DeleteObject( wnd->font );
    SetFont( wnd, font );
    GUIWndDirty( wnd );
    return( true );
#else
    wnd = wnd;
    return( false );
#endif
}

#ifndef __OS2_PM__
static char *GetFontInfo( LOGFONT *lf )
{
    char                buff[MAX_STR];
    char                *str;

    GetFontFormatString( lf, buff );
    if( GUIStrDup( buff, &str ) ) {
        return( str );
    } else {
        return( NULL );
    }
}
#endif

char *GUIGetFontInfo( gui_window *wnd )
{
#ifndef __OS2_PM__
    LOGFONT             lf;

    if( GetObject( wnd->font, sizeof( LOGFONT ), (LPSTR) &lf ) == 0 ) {
        return( NULL );
    }
    return( GetFontInfo( &lf ) );
#else
    wnd = wnd;
    return( NULL );
#endif
}

/*
 * GUIGetFontFromUser -- create font dialog to get font info from use,
 *                       initializing with font info given
 */

char *GUIGetFontFromUser( char *fontinfo )
{
#ifndef __OS2_PM__
    LOGFONT     lf;
    HFONT       font;

    font = NULL;
    if( fontinfo != NULL ) {
        GetLogFontFromString( &lf, fontinfo );
        font = CreateFontIndirect( &lf );
        fontinfo = NULL;
    }
    if( GUIChooseFont( font, &lf, NULL ) ) {
        fontinfo = GetFontInfo( &lf );
    }
    if( font != NULL ) {
        DeleteObject( font );
    }
    return( fontinfo );
#else
    fontinfo = fontinfo;
    return( NULL );
#endif
}

bool GUISetFontInfo( gui_window *wnd, char *fontinfo )
{
#ifndef __OS2_PM__
    HFONT       font;
    LOGFONT     lf;

    if( fontinfo == NULL ) {
        return( false );
    }
    GetLogFontFromString( &lf, fontinfo );
    font = CreateFontIndirect( &lf );
    if( font == NULL ) {
        return( false );
    }
    if( wnd->font != NULL ) {
        DeleteObject( wnd->font );
    }
    SetFont( wnd, font );
    return( true );
#else
    wnd = wnd;
    fontinfo = fontinfo;
    return( false );
#endif
}

bool GUIFontsSupported( void )
{
#ifndef __OS2_PM__
    return( true );
#else
    return( false );
#endif
}

bool GUISetSystemFont( gui_window *wnd, bool fixed )
{
#ifndef __OS2_PM__
    HFONT               font;
#ifdef __NT__
    NONCLIENTMETRICS    ncm;
#endif

    if( fixed ) {
        font = GetStockObject( SYSTEM_FIXED_FONT );
    } else {
#ifdef __NT__
        ncm.cbSize = sizeof( NONCLIENTMETRICS );
        SystemParametersInfo( SPI_GETNONCLIENTMETRICS, sizeof( NONCLIENTMETRICS ),
                              &ncm, 0 );
        font = CreateFontIndirect( &ncm.lfMessageFont );
#else
        font = GetStockObject( SYSTEM_FONT );
#endif
    }
    if( wnd->font != NULL ) {
        DeleteObject( wnd->font );
    }
    SetFont( wnd, font );
    return( true );
#else
    wnd = wnd;
    fixed = fixed;
    return( false );
#endif
}
