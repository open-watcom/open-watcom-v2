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
* Description:  Dynamic DBCS (Japanese) dialogs.
*
****************************************************************************/


#include "commonui.h"
#include <string.h>
#include <stdlib.h>
#include "bool.h"
#include "wpi.h"
#include "cguimem.h"
#include "_windlg.h"
#include "jdlg.h"


static BYTE     *JFontInfo = NULL;
static size_t   JFontInfoLen = 0;

/*
 * mbcs2unicode - convert a multibyte character string to Unicode
 */

#ifndef __WINDOWS__
static bool mbcs2unicode( const char *src, LPWSTR *dest, int *len )
{
    LPWSTR      new;
    int         len1, len2;

    len1 = MultiByteToWideChar( CP_OEMCP, MB_ERR_INVALID_CHARS, src, -1, NULL, 0 );

    if( len1 == 0 || len1 == ERROR_NO_UNICODE_TRANSLATION ) {
        return( false );
    }

    new = MemAlloc( len1 * sizeof( WCHAR ) );
    if( new == NULL ) {
        return( false );
    }

    len2 = MultiByteToWideChar( CP_OEMCP, MB_ERR_INVALID_CHARS, src, -1, new, len1 );
    if( len2 != len1 ) {
        MemFree( new );
        return( false );
    }

    *dest = new;
    *len = len1;

    return( true );

} /* mbcs2unicode */
#endif

/*
 * createFontInfoData - allocate and fill the font information data
 */
static bool createFontInfoData( const char *facename, WORD pointsize, BYTE **fidata, size_t *size )
{
    BYTE        *data;
    int         slen;
#ifndef __WINDOWS__
    LPWSTR      uni_facename;
#endif

#ifdef __WINDOWS__
    slen = strlen( facename ) + 1;
    data = (BYTE *)MemAlloc( sizeof( WORD ) + slen );
    if( data != NULL ) {
        *(WORD *)data = pointsize;
        memcpy( data + sizeof( WORD ), facename, slen );
    }
#else
    data = NULL;
    if( mbcs2unicode( facename, &uni_facename, &slen ) ) {
        slen *= sizeof( WCHAR );
        data = (BYTE *)MemAlloc( sizeof( WORD ) + slen );
        if( data != NULL ) {
            *(WORD *)data = pointsize;
            memcpy( data + sizeof( WORD ), uni_facename, slen );
        }
    }
#endif

    if( data == NULL ) {
        return( false );
    }

    *fidata = data;
    *size = slen + sizeof( WORD );

    return( true );

} /* createFontInfoData */

/*
 * skipString - skip over a string
 */
static BYTE *skipString( BYTE *template )
{
#ifdef __WINDOWS__
    /* scan for zero byte */
    for( ; *template != 0; template++ );
    template++;
#else
    /* scan for zero word */
    for( ; *(WORD *)template != 0; template += 2 );
    template += 2;
#endif
    return( template );

} /* skipString */

#ifndef __WINDOWS__
static bool checkClassOrdinal( BYTE *template )
{
#ifdef __WINDOWS__
    return( (*template & 0x80) != 0 );
#else
    return( *(WORD *)template == (WORD)-1 );
#endif

}
#endif

/*
 * hasFontInfo - check whether a dialog template has the DS_SETFONT style
 */
static bool hasFontInfo( const BYTE *template )
{
    const WPDLGTEMPLATE  dt;

    dt = (const WPDLGTEMPLATE)template;

    return( (dt->dtStyle & DS_SETFONT) != 0 );

} /* hasFontInfo */

/*
 * findFontInfo - find font information in a dialog template
 */
static BYTE *findFontInfo( BYTE *template )
{
    /* skip to the menu name */
    template = template + sizeof( WDLGTEMPLATE );

    /* skip the menu name */
    template = skipString( template );

    /* skip the class name or ordinal */
#ifdef __WINDOWS__
    template = skipString( template );
#else
    if( checkClassOrdinal( template ) ) {
        template += 4;
    } else {
        template = skipString( template );
    }
#endif

    /* skip the caption text */
    template = skipString( template );

    return( template );

} /* findFontInfo */

/*
 * getFontInfoSize - get the font size from a dialog template
 */
static size_t getFontInfoSize( BYTE *fontinfo )
{
    BYTE        *afterFontinfo;

    afterFontinfo = fontinfo + sizeof( WORD );
    afterFontinfo = skipString( afterFontinfo );
    return( (size_t)( afterFontinfo - fontinfo ) );

} /* getFontInfoSize */

/*
 * getSystemFontFaceName - get the system font face name and size
 */
static bool getSystemFontFaceName( char **facename, WORD *pointsize )
{
#ifndef USE_SYSTEM_FONT
    /* following is DBCS text in Japanese "‚l‚r –¾’©" */
    *facename = "\x82\x6C\x82\x72\x20\x96\xBE\x92\xA9";
    *pointsize = 10;

    return( true );
#else
    HDC         hDC;
    HFONT       systemFont;
    LOGFONT     lf;
    int         logpixelsy;

    systemFont = (HFONT)GetStockObject( SYSTEM_FONT );
    if( systemFont == NULLHANDLE ) {
        return( false );
    }

    if( !GetObject( systemFont, sizeof( LOGFONT ), &lf ) ) {
        return( false );
    }

    *facename = (char *)MemAlloc( strlen( lf.lfFaceName ) + 1 );
    if( *facename == NULL ) {
        return( false );
    }
    strcpy( *facename, lf.lfFaceName );

    hDC = GetDC( NULLHANDLE );
    logpixelsy = GetDeviceCaps( hDC, LOGPIXELSY );
    ReleaseDC( NULLHANDLE, hDC );
    *pointsize = ( ( lf.lfHeight * 720L ) / (LONG)logpixelsy + 5 ) / 10;

    return( true );
#endif

} /* getSystemFontFaceName */

/*
 * loadDialogTemplate - load a dialog template
 */
static TEMPLATE_HANDLE loadDialogTemplate( HINSTANCE hinst, LPCSTR lpszDlgTemp, DWORD *size )
{
    TEMPLATE_HANDLE jdlgtemplate;
    HRSRC           hrsrc;

    hrsrc = FindResource( hinst, lpszDlgTemp, RT_DIALOG );
    if( hrsrc == NULLHANDLE ) {
        return( (TEMPLATE_HANDLE)NULL );
    }

    *size = SizeofResource( hinst, hrsrc );
    if( *size == 0 ) {
        return( (TEMPLATE_HANDLE)NULL );
    }

    jdlgtemplate = LoadResource( hinst, hrsrc );
    if( jdlgtemplate == (TEMPLATE_HANDLE)NULL ) {
        return( (TEMPLATE_HANDLE)NULL );
    }

    return( jdlgtemplate );

} /* loadDialogTemplate */

/*
 * createJTemplate - create a Japanese dialog template
 */
static TEMPLATE_HANDLE createJTemplate( TEMPLATE_HANDLE dlgtemplate, DWORD size )
{
    TEMPLATE_HANDLE jdlgtemplate;
    size_t          newSize;
    BYTE            *jtemplate;
    BYTE            *template;
    BYTE            *fontinfo;
    size_t          dlgHeaderSize;
    size_t          jdlgHeaderSize;
    size_t          ctlInfoSize;
    size_t          fontinfoSize;

    if( size == -1 ) {
        newSize = GlobalSize( dlgtemplate );
    } else {
        newSize = size;
    }

    template = (BYTE *)LockResource( dlgtemplate );
    if( template == NULL ) {
        return( (TEMPLATE_HANDLE)NULL );
    }

    if( !hasFontInfo( template ) ) {
#ifdef __WINDOWS__
        UnlockResource( dlgtemplate );
#endif
        return( (TEMPLATE_HANDLE)NULL );
    }

    fontinfo = findFontInfo( template );
    fontinfoSize = getFontInfoSize( fontinfo );

    /* calcualte the size of the original dialog header */
    dlgHeaderSize = (size_t)( fontinfo - template ) + fontinfoSize;
    ADJUST_DLGLEN( dlgHeaderSize );
    ctlInfoSize = size - dlgHeaderSize;

    /* calculate the size of the new dialog header */
    jdlgHeaderSize = (size_t)( fontinfo - template ) + JFontInfoLen;
    ADJUST_DLGLEN( jdlgHeaderSize );

    newSize = jdlgHeaderSize + ctlInfoSize;

    jdlgtemplate = GlobalAlloc( GHND, newSize );
    if( jdlgtemplate == (TEMPLATE_HANDLE)NULL ) {
#ifdef __WINDOWS__
        UnlockResource( dlgtemplate );
#endif
        return( (TEMPLATE_HANDLE)NULL );
    }

    jtemplate = (BYTE *)GlobalLock( jdlgtemplate );
    if( jtemplate == NULL ) {
        GlobalFree( jdlgtemplate );
#ifdef __WINDOWS__
        UnlockResource( dlgtemplate );
#endif
        return( (TEMPLATE_HANDLE)NULL );
    }

    /* copy template data up to fontinfo */
    memcpy( jtemplate, template, fontinfo - template );

    /* copy the new fontinfo */
    memcpy( jtemplate + ( fontinfo - template ), JFontInfo, JFontInfoLen );

    /* copy the rest of the template data */
    memcpy( jtemplate + jdlgHeaderSize, template + dlgHeaderSize, ctlInfoSize );

    GlobalUnlock( jdlgtemplate );
#ifdef __WINDOWS__
    UnlockResource( dlgtemplate );
#endif

    return( jdlgtemplate );

} /* createJTemplate */

/*
 * JDialogInit - initialize Japenese dialogs
 */
bool JDialogInit( void )
{
    char        *facename;
    WORD        pointsize;

    if( !GetSystemMetrics( SM_DBCSENABLED ) ) {
        return( true );
    }

    if( !getSystemFontFaceName( &facename, &pointsize ) ) {
        return( false );
    }

    return( createFontInfoData( facename, pointsize, &JFontInfo, &JFontInfoLen ) );
}

/*
 * JDialogFini - done with all Japanese dialogs
 */
void JDialogFini( void )
{
    if( JFontInfo != NULL ) {
        MemFree( JFontInfo );
        JFontInfo = NULL;
        JFontInfoLen = 0;
    }

} /* JDialogFini */

/*
 * cdIndirect - helper for JCreateDialogIndirect
 */
static HWND cdIndirect( HINSTANCE hinst, TEMPLATE_HANDLE dlgtemplate, HWND hwndOwner, DLGPROC dlgproc, DWORD size )
{
    TEMPLATE_HANDLE jdlgtemplate;
    HWND            ret;

    if( JFontInfo == NULL ) {
        goto CDI_DEFAULT_ACTION;
    }

    jdlgtemplate = createJTemplate( dlgtemplate, size );
    if( jdlgtemplate == (TEMPLATE_HANDLE)NULL ) {
        goto CDI_DEFAULT_ACTION;
    }

    ret = CreateDialogIndirect( hinst, GlobalLock( jdlgtemplate ), hwndOwner, dlgproc );
    GlobalUnlock( jdlgtemplate );
    GlobalFree( jdlgtemplate );

    return( ret );

CDI_DEFAULT_ACTION:
    ret = CreateDialogIndirect( hinst, GlobalLock( dlgtemplate ),  hwndOwner, dlgproc );
    GlobalUnlock( dlgtemplate );

    return( ret );

} /* cdIndirect */

/*
 * cdIndirectParam - helper for JCreateDialogIndirectParam
 */
static HWND cdIndirectParam( HINSTANCE hinst, TEMPLATE_HANDLE dlgtemplate, HWND hwndOwner, DLGPROC dlgproc, LPARAM lParamInit, DWORD size )
{
    TEMPLATE_HANDLE jdlgtemplate;
    HWND            ret;

    if( JFontInfo == NULL ) {
        goto CDIP_DEFAULT_ACTION;
    }

    jdlgtemplate = createJTemplate( dlgtemplate, size );
    if( jdlgtemplate == (TEMPLATE_HANDLE)NULL ) {
        goto CDIP_DEFAULT_ACTION;
    }

    ret = CreateDialogIndirectParam( hinst, GlobalLock( jdlgtemplate ), hwndOwner, dlgproc, lParamInit );
    GlobalUnlock( jdlgtemplate );
    GlobalFree( jdlgtemplate );

    return( ret );

CDIP_DEFAULT_ACTION:
    ret = CreateDialogIndirectParam( hinst, GlobalLock( dlgtemplate ), hwndOwner, dlgproc, lParamInit );
    GlobalUnlock( dlgtemplate );

    return( ret );

} /* cdIndirectParam */

/*
 * dbIndirect - helper for JDialogBoxIndirect
 */
static INT_PTR dbIndirect( HINSTANCE hinst, TEMPLATE_HANDLE dlgtemplate, HWND hwndOwner, DLGPROC dlgproc, DWORD size )
{
    TEMPLATE_HANDLE jdlgtemplate;
    INT_PTR         ret;

    if( JFontInfo == NULL ) {
        goto DBI_DEFAULT_ACTION;
    }

    jdlgtemplate = createJTemplate( dlgtemplate, size );
    if( jdlgtemplate == (TEMPLATE_HANDLE)NULL ) {
        goto DBI_DEFAULT_ACTION;
    }

    ret = DialogBoxIndirect( hinst, TEMPLATE_LOCK( jdlgtemplate ), hwndOwner, dlgproc );
    TEMPLATE_UNLOCK( jdlgtemplate );
    GlobalFree( jdlgtemplate );

    return( ret );

DBI_DEFAULT_ACTION:
    ret = DialogBoxIndirect( hinst, TEMPLATE_LOCK( dlgtemplate ), hwndOwner, dlgproc );
    TEMPLATE_UNLOCK( dlgtemplate );
    return( ret );
}

/*
 * dbIndirectParam - helper for JDialogBoxIndirectParam
 */
static INT_PTR dbIndirectParam( HINSTANCE hinst, TEMPLATE_HANDLE dlgtemplate,
                            HWND hwndOwner, DLGPROC dlgproc,
                            LPARAM lParamInit, DWORD size )
{
    TEMPLATE_HANDLE jdlgtemplate;
    INT_PTR         ret;

    if( JFontInfo == NULL ) {
        goto DBIP_DEFAULT_ACTION;
    }

    jdlgtemplate = createJTemplate( dlgtemplate, size );
    if( jdlgtemplate == (TEMPLATE_HANDLE)NULL ) {
        goto DBIP_DEFAULT_ACTION;
    }

    ret = DialogBoxIndirectParam( hinst, TEMPLATE_LOCK( jdlgtemplate ), hwndOwner, dlgproc, lParamInit );
    TEMPLATE_UNLOCK( jdlgtemplate );
    GlobalFree( jdlgtemplate );
    return( ret );

DBIP_DEFAULT_ACTION:
    ret = DialogBoxIndirectParam( hinst, TEMPLATE_LOCK( dlgtemplate ), hwndOwner, dlgproc, lParamInit );
    TEMPLATE_UNLOCK( dlgtemplate );
    return( ret );
}

/*
 * JDialogBoxIndirect - Japanese version of DialogBoxIndirect
 */
INT_PTR JDialogBoxIndirect( HINSTANCE hinst, TEMPLATE_HANDLE dlgtemplate, HWND hwndOwner, DLGPROC dlgproc )
{
    return( dbIndirect( hinst, dlgtemplate, hwndOwner, dlgproc, (DWORD)-1 ) );

} /* JDialogBoxIndirect */

/*
 * JDialogBoxIndirectParam - Japanese version of DialogBoxIndirectParam
 */
INT_PTR JDialogBoxIndirectParam( HINSTANCE hinst, TEMPLATE_HANDLE dlgtemplate,
                             HWND hwndOwner, DLGPROC dlgproc, LPARAM lParamInit )
{
    return( dbIndirectParam( hinst, dlgtemplate, hwndOwner, dlgproc, lParamInit, (DWORD)-1 ) );

} /* JDialogBoxIndirectParam */

/*
 * JCreateDialogIndirect - Japanese version of CreateDialogIndirect
 */
HWND JCreateDialogIndirect( HINSTANCE hinst, TEMPLATE_HANDLE dlgtemplate, HWND hwndOwner, DLGPROC dlgproc )
{
    return( cdIndirect( hinst, dlgtemplate, hwndOwner, dlgproc, (DWORD)-1 ) );

} /* JCreateDialogIndirect */

/*
 * JCreateDialogIndirectParam - Japanese version of CreateDialogIndirectParam
 */
HWND JCreateDialogIndirectParam( HINSTANCE hinst, TEMPLATE_HANDLE dlgtemplate,
                                 HWND hwndOwner, DLGPROC dlgproc, LPARAM lParamInit )
{
    return( cdIndirectParam( hinst, dlgtemplate, hwndOwner, dlgproc, lParamInit, (DWORD)-1 ) );

} /* JCreateDialogIndirectParam */

/*
 * JDialogBox - Japanese version of DialogBox
 */
INT_PTR JDialogBox( HINSTANCE hinst, LPCSTR lpszDlgTemp, HWND hwndOwner, DLGPROC dlgproc )
{
    TEMPLATE_HANDLE jdlgtemplate;
    DWORD           size;
    INT_PTR         ret;

    if( JFontInfo == NULL ) {
        goto JDB_DEFAULT_ACTION;
    }

    jdlgtemplate = loadDialogTemplate( hinst, lpszDlgTemp, &size );
    if( jdlgtemplate == (TEMPLATE_HANDLE)NULL ) {
        goto JDB_DEFAULT_ACTION;
    }

    ret = dbIndirect( hinst, jdlgtemplate, hwndOwner, dlgproc, size );
    FreeResource( jdlgtemplate );

    return( ret );

JDB_DEFAULT_ACTION:
    return( DialogBox( hinst, (LPSTR)lpszDlgTemp, hwndOwner, dlgproc ) );

} /* JDialogBox */

/*
 * JDialogBoxParam - Japanese version of DialogBoxParam
 */
INT_PTR JDialogBoxParam( HINSTANCE hinst, LPCSTR lpszDlgTemp, HWND hwndOwner, DLGPROC dlgproc, LPARAM lParamInit )
{
    TEMPLATE_HANDLE jdlgtemplate;
    DWORD           size;
    INT_PTR         ret;

    if( JFontInfo == NULL ) {
        goto JDBP_DEFAULT_ACTION;
    }

    jdlgtemplate = loadDialogTemplate( hinst, lpszDlgTemp, &size );
    if( jdlgtemplate == (TEMPLATE_HANDLE)NULL ) {
        goto JDBP_DEFAULT_ACTION;
    }

    ret = dbIndirectParam( hinst, jdlgtemplate, hwndOwner, dlgproc, lParamInit, size );
    FreeResource( jdlgtemplate );

    return( ret );

JDBP_DEFAULT_ACTION:
    return( DialogBoxParam( hinst, (LPSTR)lpszDlgTemp, hwndOwner, dlgproc, lParamInit ) );

} /* JDialogBoxParam */

/*
 * JCreateDialog - Japanese version of CreateDialog
 */
HWND JCreateDialog( HINSTANCE hinst, LPCSTR lpszDlgTemp, HWND hwndOwner, DLGPROC dlgproc )
{
    TEMPLATE_HANDLE jdlgtemplate;
    DWORD           size;
    HWND            ret;

    if( JFontInfo == NULL ) {
        goto JCD_DEFAULT_ACTION;
    }

    jdlgtemplate = loadDialogTemplate( hinst, lpszDlgTemp, &size );
    if( jdlgtemplate == (TEMPLATE_HANDLE)NULL ) {
        goto JCD_DEFAULT_ACTION;
    }

    ret = cdIndirect( hinst, jdlgtemplate, hwndOwner, dlgproc, size );
    FreeResource( jdlgtemplate );

    return( ret );

JCD_DEFAULT_ACTION:
    return( CreateDialog( hinst, (LPSTR)lpszDlgTemp, hwndOwner, dlgproc ) );

} /* JCreateDialog */

/*
 * JCreateDialogParam - Japanese version of CreateDialogParam
 */
HWND JCreateDialogParam( HINSTANCE hinst, LPCSTR lpszDlgTemp, HWND hwndOwner, DLGPROC dlgproc, LPARAM lParamInit )
{
    TEMPLATE_HANDLE jdlgtemplate;
    DWORD           size;
    HWND            ret;

    if( JFontInfo == NULL ) {
        goto JCDP_DEFAULT_ACTION;
    }

    jdlgtemplate = loadDialogTemplate( hinst, lpszDlgTemp, &size );
    if( jdlgtemplate == (TEMPLATE_HANDLE)NULL ) {
        goto JCDP_DEFAULT_ACTION;
    }

    ret = cdIndirectParam( hinst, jdlgtemplate, hwndOwner, dlgproc, lParamInit, size );
    FreeResource( jdlgtemplate );

    return( ret );

JCDP_DEFAULT_ACTION:
    return( CreateDialogParam( hinst, (LPSTR)lpszDlgTemp, hwndOwner, dlgproc, lParamInit ) );

} /* JCreateDialogParam */

/*
 * JDialogGetJFont - get the font used for Japanese dialogs
 */
bool JDialogGetJFont( char **facename, WORD *pointsize )
{
    return( getSystemFontFaceName( facename, pointsize ) );

} /* JDialogGetJFont */
