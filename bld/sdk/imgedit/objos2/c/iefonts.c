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
#include <string.h>
#include "imgedit.h"

static WPI_LOGFONT      *currentLogFont = NULL;

/*
 * EnumFontFunc - enumerate fonts
 */
int CALLBACK EnumFontFunc( WPI_LOGFONT *lf, WPI_TEXTMETRIC *tm, UINT ftype,
                                                                LPSTR data )
{
    char        *facename;

    tm = tm;
    ftype = ftype;

    facename = _wpi_getfontfacename( *lf );
    if( !FARstricmp( facename, data ) ) {
        currentLogFont = malloc( sizeof(WPI_LOGFONT) );
        memcpy(currentLogFont, lf, sizeof(WPI_LOGFONT));
        return( 0 );
//      return( 1 );
    }
    return( 1 );
} /* EnumFontFunc */

/*
 * CreateStatusFont - creates the font used in the status window
 */
void CreateStatusFont( void )
{
    WPI_LOGFONT                 logfont;
    WPI_PROC                    fp;
    WPI_PRES                    pres;

    pres = _wpi_getpres( HWND_DESKTOP );
#ifdef __OS2_PM__
    _wpi_enumfonts( pres, NULL, (WPI_ENUMFONTPROC)EnumFontFunc, "Helv" );
    fp = fp;
#else
    fp = _wpi_makeprocinstance( EnumFontFunc, Instance );
#ifdef __NT__
    EnumFonts( hdc, NULL, (LPVOID)fp, (LPARAM)(LPVOID)"ms sans serif");
#else
    EnumFonts( hdc, NULL, (LPVOID)fp, (LPVOID)"ms sans serif");
#endif
#endif
    _wpi_freeprocinstance( fp );

    if( currentLogFont == NULL ) {
//      SmallFont = GetStockObject( ANSI_FIXED_FONT );
//      GetObject( SmallFont, sizeof( LOGFONT ), (LPSTR) &logfont );
//      SmallFont = CreateFontIndirect( &logfont );
    } else {
        memcpy( &logfont, currentLogFont, sizeof(WPI_LOGFONT) );
//      _wpi_setfontheight( &logfont, 11 );
//      _wpi_setfontwidth( &logfont, 10 );
        _wpi_setfontpointsize( &logfont, 10, 0, _wpi_fontmatch(&logfont) );
        _wpi_createrealfont( logfont, SmallFont );

#if 0
        SmallFont = CreateFont(
            10,
            0,
            0,
            0,
            FW_NORMAL,
            FALSE,
            FALSE,
            FALSE,
            currentLogFont->lfCharSet,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,
            currentLogFont->lfPitchAndFamily,
            currentLogFont->lfFaceName );
#endif
        free( currentLogFont );
        currentLogFont = NULL;
    }
    _wpi_releasepres( HWND_DESKTOP, pres );
} /* CreateStatusFont */

