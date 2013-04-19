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
* Description:  Font selection dialog for Windows.
*
****************************************************************************/


#include "precomp.h"
#include <stdio.h>
#include <string.h>
#include "font.h"
#include "wi163264.h"

#define MAX_STR 256

static LOGFONT  logFont;
static HFONT    fixedFont = (HFONT)0;
static HFONT    courierFont = (HFONT)0;
static BOOL     variableAllowed = FALSE;

static char     *fontKey = "Font";

/*
 * EnumFunc - enumerate fonts
 */
WINEXPORT int CALLBACK EnumFunc( LPLOGFONT lf, LPTEXTMETRIC tm, UINT ftype, LPSTR data )
{
    tm = tm;
    ftype = ftype;
    data = data;

    /*
     * Something has happened in the font world and Windows font mapper since
     * the original source was written, it checked only for Courier. All the
     * font names below are verified as good monospaced fonts. Check for the
     * best fonts first, so that the system picks the best if enumerated first.
     * Changed the test to == 0, because it is easier to read and understand.
     */
#if defined( __NT__ )
    if( FARstricmp( lf->lfFaceName, "andale mono" ) == 0 ||
        FARstricmp( lf->lfFaceName, "lucida console" ) == 0 ||
        FARstricmp( lf->lfFaceName, "vera sans mono" ) == 0 ||
        FARstricmp( lf->lfFaceName, "courier new" ) == 0 ||
        FARstricmp( lf->lfFaceName, "courier" ) == 0 ) {
#else
    if( FARstricmp( lf->lfFaceName, "courier new" ) == 0 ||
        FARstricmp( lf->lfFaceName, "courier" ) == 0 ) {
#endif
        courierFont = CreateFont( 13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, lf->lfCharSet,
                                  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                  lf->lfPitchAndFamily, lf->lfFaceName );
        return( 0 );
    }
    return( 1 );

} /* EnumFunc */

/*
 * getCourierFont - find a mono font
 */
static void getCourierFont( HANDLE inst )
{
    LOGFONT     logfont;
    FARPROC     fp;
    HDC         hdc;

    inst = inst;        /* shut up the compiler for NT */
    hdc = GetDC( (HWND)NULL );
    fp = MakeProcInstance( (FARPROC)EnumFunc, inst );
    EnumFonts( hdc, NULL, (LPVOID)fp, 0 );
    FreeProcInstance( fp );
    ReleaseDC( (HWND)NULL, hdc );

    if( courierFont == NULL ) {
        courierFont = GetStockObject( ANSI_FIXED_FONT );
        GetObject( courierFont, sizeof( LOGFONT ), (LPSTR)&logfont );
        courierFont = CreateFontIndirect( &logfont );
    }

} /* getCourierFont */

/*
 * SetDlgMonoFont - set a mono font in a dialog item
 */
void SetDlgMonoFont( HWND hwnd, int id )
{
    SendDlgItemMessage( hwnd, id, WM_SETFONT, (UINT)fixedFont, 0L );

} /* SetDlgMonoFont */

/*
 * SetMonoFont - set a mono font in a window
 */
void SetMonoFont( HWND hwnd )
{
    SendMessage( hwnd, WM_SETFONT, (UINT)fixedFont, 0L );

} /* SetMonoFont */

/*
 * SetDlgCourierFont - set a courier font in a dialog item
 */
void SetDlgCourierFont( HWND hwnd, int id )
{
    SendDlgItemMessage( hwnd, id, WM_SETFONT, (UINT)fixedFont, 0L );

} /* SetDlgCourierFont */

/*
 * SetCourierFont - set a courier font in a window
 */
void SetCourierFont( HWND hwnd )
{
    SendMessage( hwnd, WM_SETFONT, (UINT)courierFont, 0L );

} /* SetCourierFont */

/*
 * InitMonoFont - find a mono font
 */
void InitMonoFont( char *app, char *inifile, int default_font, HANDLE inst )
{
    char        str[MAX_STR];
    BOOL        need_stock;

    need_stock = TRUE;
    GetPrivateProfileString( app, fontKey, "", str, sizeof( str ), inifile );
    if( str[0] != 0 ) {
        if( GetLogFontFromString( &logFont, str )  ) {
            fixedFont = CreateFontIndirect( &logFont );
            if( fixedFont != NULL ) {
                need_stock = FALSE;
            }
        }
    }
    getCourierFont( inst );
    if( need_stock ) {
#if defined( __NT__ )
        fixedFont = courierFont;
#endif
        if( fixedFont == (HFONT)0 ) {
#if defined( __NT__ )
            fixedFont = GetStockObject( ANSI_FIXED_FONT );
#endif
            fixedFont = GetStockObject( default_font );
            GetObject( fixedFont, sizeof( LOGFONT ), &logFont );
            fixedFont = CreateFontIndirect( &logFont );
        } else {
            GetObject( fixedFont, sizeof( LOGFONT ), &logFont );
        }
    }

} /* InitMonoFont */

/*
 * SaveMonoFont - save the current mono font
 */
void SaveMonoFont( char *app, char *inifile )
{
    char        str[MAX_STR];

    GetFontFormatString( &logFont, str );
    WritePrivateProfileString( app, fontKey, str, inifile );

} /* SaveMonoFont */

/*
 * ChooseMonoFont - allow the picking of a mono font
 */
BOOL ChooseMonoFont( HWND hwnd )
{
    CHOOSEFONT  cf;
    LOGFONT     lf;
    HFONT       font;

    memset( &cf, 0, sizeof( CHOOSEFONT ) );
    lf = logFont;

    cf.lStructSize = sizeof( CHOOSEFONT );
    cf.hwndOwner = hwnd;
    cf.lpLogFont = &lf;
    cf.Flags = CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT;
    if( !variableAllowed ) {
        cf.Flags |= CF_FIXEDPITCHONLY;
    }
    cf.nFontType = SCREEN_FONTTYPE;
    cf.rgbColors = RGB( 0, 0, 0 );

    if( !ChooseFont( &cf ) ) {
        return( FALSE );
    }
    font = CreateFontIndirect( &lf );
    if( font == NULL ) {
        return( FALSE );
    }
    DeleteObject( fixedFont );
    logFont = lf;
    fixedFont = font;
    return( TRUE );

} /* ChooseMonoFont */

/*
 * DestroyMonoFonts
 */
void DestroyMonoFonts( void )
{
    DeleteObject( fixedFont );
    DeleteObject( courierFont );

} /* DestroyMonoFonts */

/*
 * GetMonoFont
 */
HFONT GetMonoFont( void )
{
    return( fixedFont );

} /* GetMonoFont */

/*
 * AllowVariableFonts - enable selection of variable pitch fonts
 */
void AllowVariableFonts( void )
{
    variableAllowed = TRUE;

} /* AllowVariableFonts */

