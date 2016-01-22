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
* Description:  Font selection dialog.
*
****************************************************************************/


#include "vi.h"
#include "ftbar.h"
#include "utils.h"
#include "sstyle.h"
#include "font.h"
#include "subclass.h"
#include "hotkey.h"
#include "wprocmap.h"


/* NB: In Win386 mode, the FAR pointers passed to callback procs are
 * really 0:32 near pointers!
 */
#ifdef __WINDOWS_386__
    #define FARCBPARM
#else
    #define FARCBPARM   FAR
#endif


/* Local Windows CALLBACK function prototypes */
#if defined( __WINDOWS_386__ )
WINEXPORT int CALLBACK EnumFamTypefaces( const LOGFONT *_lf, const TEXTMETRIC *tm, int FontType, LPARAM lparam );
WINEXPORT int CALLBACK EnumFamInfo( const LOGFONT *_lf, const TEXTMETRIC *tm, int FontType, LPARAM lparam );
WINEXPORT int CALLBACK SetupFontData( const LOGFONT *_lf, const TEXTMETRIC *tm, int FontType, LPARAM lparam );
#elif defined( __WINDOWS__ )
WINEXPORT int CALLBACK EnumFamTypefaces( const ENUMLOGFONT FAR *elf, const NEWTEXTMETRIC FAR *ntm, int FontType, LPARAM lparam );
WINEXPORT int CALLBACK EnumFamInfo( const ENUMLOGFONT FAR *elf, const NEWTEXTMETRIC FAR *ntm, int FontType, LPARAM lparam );
WINEXPORT int CALLBACK SetupFontData( const ENUMLOGFONT FAR *elf, const NEWTEXTMETRIC FAR *ntm, int FontType, LPARAM lparam );
#else
WINEXPORT int CALLBACK EnumFamTypefaces( const LOGFONT FAR *lf, const TEXTMETRIC FAR *tm, DWORD FontType, LPARAM lparam );
WINEXPORT int CALLBACK EnumFamInfo( const LOGFONT FAR *lf, const TEXTMETRIC FAR *tm, DWORD FontType, LPARAM lparam );
WINEXPORT int CALLBACK SetupFontData( const LOGFONT FAR *lf, const TEXTMETRIC FAR *tm, DWORD FontType, LPARAM lparam );
#endif
WINEXPORT INT_PTR CALLBACK FtDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

HWND    hwndTypeface, hwndStyle, hwndSize, hwndPick, hwndSizeEdit;
LOGFONT CurLogfont;

HWND    hFontbar;

enum {
    STYLE_REGULAR = 0,
    STYLE_BOLD = 1,
    STYLE_ITALIC = 2
    /* add underline, strikeout, if necessary
    */
};
#define MAX_STYLES      4
#define MAX_SIZES       16

char    Style[MAX_STYLES];
int     NStyles, NSizes;
/* value of 96 taken from Petzold pg 512
*/
int     YPIXELS_PER_INCH = 96;


#if defined( __WINDOWS_386__ )
WINEXPORT int CALLBACK EnumFamTypefaces( const LOGFONT *_lf, const TEXTMETRIC *tm, int FontType, LPARAM lparam )
#elif defined( __WINDOWS__ )
WINEXPORT int CALLBACK EnumFamTypefaces( const ENUMLOGFONT FAR *elf, const NEWTEXTMETRIC FAR *ntm, int FontType, LPARAM lparam )
#else
WINEXPORT int CALLBACK EnumFamTypefaces( const LOGFONT FAR *lf, const TEXTMETRIC FAR *tm, DWORD FontType, LPARAM lparam )
#endif
{
#ifdef __WINDOWS_386__
    char                   faceName[LF_FACESIZE];
    const LOGFONT __far    *lf = MK_FP32( (void *)_lf );
    tm = tm;
#elif defined( __WINDOWS__ )
    const LOGFONT FAR      *lf = (const LOGFONT FAR *)elf;
//    const TEXTMETRIC FAR *tm = (const TEXTMETRIC FAR *)ntm;
    ntm = ntm;
#else
    tm = tm;
#endif

    lparam = lparam;
    FontType = FontType;

#ifdef __WINDOWS_386__
    /* On Win386, we need to pass a near 32-bit pointer with LB_ADDSTRING,
     * but we get a far pointer from Windows. Hence the shenanigans with
     * a temp buffer in the flat address space.
     */
    _fstrcpy( faceName, lf->lfFaceName );
    SendMessage( hwndTypeface, LB_ADDSTRING, 0, (LPARAM)faceName );
#else
    SendMessage( hwndTypeface, LB_ADDSTRING, 0, (LPARAM)(lf->lfFaceName) );
#endif

    return( TRUE );
}

#if defined( __WINDOWS_386__ )
WINEXPORT int CALLBACK EnumFamInfo( const LOGFONT *_lf, const TEXTMETRIC *tm, int FontType, LPARAM lparam )
#elif defined( __WINDOWS__ )
WINEXPORT int CALLBACK EnumFamInfo( const ENUMLOGFONT FAR *elf, const NEWTEXTMETRIC FAR *ntm, int FontType, LPARAM lparam )
#else
WINEXPORT int CALLBACK EnumFamInfo( const LOGFONT FAR *lf, const TEXTMETRIC FAR *tm, DWORD FontType, LPARAM lparam )
#endif
{
    char                   sbuf[40];
    int                    height;
    long                   *isTrueType = (long *)lparam;
#ifdef __WINDOWS_386__
    const LOGFONT __far    *lf = MK_FP32( (void *)_lf );
    tm = tm;
#elif defined( __WINDOWS__ )
    const LOGFONT FAR      *lf = (const LOGFONT FAR *)elf;
//    const TEXTMETRIC FAR *tm = (const TEXTMETRIC FAR *)ntm;
    ntm = ntm;
#else
    tm = tm;
#endif

    if( FontType == TRUETYPE_FONTTYPE ) {
        /* truetype creates the style string for us
        */

        /* PROBLEM: the style string may be weird (eg "Outline Italic").
           If so, we are stuck as we cannot apparently specify this
           to CreateFont.  For now, we are simply IGNORING any/all
           TrueType style strings are are just specifying the 4 basic ones
           damNit.
        */
        *isTrueType = 1;
    } else {
        /* add size to list
        */
        height = abs( lf->lfHeight );
        sprintf( sbuf, "%d", height );
        if( SendMessage( hwndSize, CB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)sbuf ) == CB_ERR ) {
            SendMessage( hwndSize, CB_INSERTSTRING, NSizes, (LPARAM)sbuf );
        }
    }
    return( 1 );
}

#if defined( __WINDOWS_386__ )
WINEXPORT int CALLBACK SetupFontData( const LOGFONT *_lf, const TEXTMETRIC *tm, int FontType, LPARAM lparam )
#elif defined( __WINDOWS__ )
WINEXPORT int CALLBACK SetupFontData( const ENUMLOGFONT FAR *elf, const NEWTEXTMETRIC FAR *ntm, int FontType, LPARAM lparam )
#else
WINEXPORT int CALLBACK SetupFontData( const LOGFONT FAR *lf, const TEXTMETRIC FAR *tm, DWORD FontType, LPARAM lparam )
#endif
{
#ifdef __WINDOWS_386__
    const LOGFONT __far    *lf = MK_FP32( (void *)_lf );
    tm = tm;
#elif defined( __WINDOWS__ )
    const LOGFONT FAR      *lf = (const LOGFONT FAR *)elf;
//    const TEXTMETRIC FAR *tm = (const TEXTMETRIC FAR *)ntm;
    ntm = ntm;
#else
    tm = tm;
#endif

    FontType = FontType;
    lparam = lparam;

    /* start setting up CurLogfont based on the font data */
    CurLogfont.lfCharSet = lf->lfCharSet;
    CurLogfont.lfOutPrecision = lf->lfOutPrecision;
    CurLogfont.lfClipPrecision = lf->lfClipPrecision;
    CurLogfont.lfQuality = lf->lfQuality;
    CurLogfont.lfPitchAndFamily = lf->lfPitchAndFamily;

    /* only do this for the 1st font - we just want defaults */
    return( FALSE );
}

static void fillTypefaceBox( HWND hwnd )
{
    FARPROC     fp;
    HDC         hdc;

    hwnd = hwnd;

    /* put typefaces in combo box
    */
    hdc = GetDC( edit_container_id );
    fp = MakeFontEnumProcInstance( EnumFamTypefaces, InstanceHandle );
    EnumFontFamilies( hdc, NULL, (FONTENUMPROC)fp, 0L );
    FreeProcInstance( fp );
    ReleaseDC( edit_container_id, hdc );

    SendMessage( hwndTypeface, LB_SETCURSEL, 0, 0L );
}

static void fillStyleBox( void )
{
    SendMessage( hwndStyle, LB_INSERTSTRING, 0, (LPARAM)"Regular" );
    SendMessage( hwndStyle, LB_INSERTSTRING, 1, (LPARAM)"Italic" );
    SendMessage( hwndStyle, LB_INSERTSTRING, 2, (LPARAM)"Bold" );
    SendMessage( hwndStyle, LB_INSERTSTRING, 3, (LPARAM)"Bold Italic" );
    Style[0] = STYLE_REGULAR;
    Style[1] = STYLE_ITALIC;
    Style[2] = STYLE_BOLD;
    Style[3] = STYLE_ITALIC | STYLE_BOLD;
    NStyles = 4;
}

static void fillInfoBoxes( HWND hwnd )
{
    FARPROC         fp;
    int             index;
    HDC             hdc;
    char            typeface[LF_FACESIZE + 1];
    char            size[8];
    long            isTrueType = 0;
    char            oldSize[8], oldStyle[40];
    static int      vPitchSizes[] = { 8, 9, 10, 11, 12, 14, 16, 18,
                                      20, 22, 24, 26, 28, 36, 48, 72 };

    hwnd = hwnd;

    index = (int)SendMessage( hwndStyle, LB_GETCURSEL, 0, 0L );
    if( index != LB_ERR ) {
        SendMessage( hwndStyle, LB_GETTEXT, index, (LPARAM)oldStyle );
    } else {
        oldStyle[0] = 0;
    }
    index = (int)SendMessage( hwndSize, CB_GETCURSEL, 0, 0L );
    if( index != CB_ERR ) {
        SendMessage( hwndSize, CB_GETLBTEXT, index, (LPARAM)oldSize );
    } else {
        oldSize[0] = 0;
    }

    SendMessage( hwndStyle, LB_RESETCONTENT, 0, 0L );
    SendMessage( hwndSize, CB_RESETCONTENT, 0, 0L );
    NStyles = NSizes = 0;

    index = (int)SendMessage( hwndTypeface, LB_GETCURSEL, 0, 0L );
    if( index == LB_ERR ) {
        return;
    }

    fillStyleBox();

    SendMessage( hwndTypeface, LB_GETTEXT, index, (LPARAM)typeface );
    fp = MakeFontEnumProcInstance( EnumFamInfo, InstanceHandle );
    hdc = GetDC( edit_container_id );
    EnumFontFamilies( hdc, typeface, (FONTENUMPROC)fp, (LPARAM)(&isTrueType) );
    ReleaseDC( edit_container_id, hdc );
    FreeProcInstance( fp );

    if( isTrueType ) {
        /* suggest a few truetype point values
        */
        int nelements = sizeof( vPitchSizes ) / sizeof( vPitchSizes [0] );
        if( NSizes ) {
            SendMessage( hwndSize, CB_RESETCONTENT, 0, 0L );
            NSizes = 0;
        }
        for( index = 0; index < nelements; index++ ) {
            sprintf( size, "%d", vPitchSizes[index] );
            if( SendMessage( hwndSize, CB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)size ) == CB_ERR ) {
                SendMessage( hwndSize, CB_INSERTSTRING, NSizes, (LPARAM)size );
                NSizes++;
            }
        }
    }

    if( oldStyle[0] ) {
        index = (int)SendMessage( hwndStyle, LB_SELECTSTRING, (WPARAM)-1, (LPARAM)oldStyle );
        if( index == LB_ERR ) {
            SendMessage( hwndStyle, LB_SETCURSEL, 0, 0L );
        }
    }
    if( oldSize[0] ) {
        index = (int)SendMessage( hwndSize, CB_SELECTSTRING, (WPARAM)-1, (LPARAM)oldSize );
        if( index == CB_ERR ) {
            SendMessage( hwndSize, CB_SETCURSEL, 0, 0L );
        }
    }
}

static void setDefaultTypeface( void )
{
    int     i;
    LPSTR   typeName;

    // default default
    SendMessage( hwndTypeface, LB_SETCURSEL, 0, 0L );

    typeName = FontlfFaceName( SEType[SE_WHITESPACE].font );
    i = (int)SendMessage( hwndTypeface, LB_SELECTSTRING, (WPARAM)-1, (LPARAM)typeName );
    if( i != LB_ERR ) {
        SendMessage( hwndTypeface, LB_SETCURSEL, i, 0L );
    }
}

static void setDefaultSizeStyle( void )
{
    char    buf[20];
    int     i;

    // default default
    SendMessage( hwndSize, CB_SETCURSEL, 0, 0L );
    SendMessage( hwndStyle, LB_SETCURSEL, 0, 0L );

    sprintf( buf, "%d", abs( FontlfHeight( SEType[SE_WHITESPACE].font ) ) );
    i = (int)SendMessage( hwndSize, CB_SELECTSTRING, (WPARAM)-1, (LPARAM)buf );
    if( i != CB_ERR ) {
        SendMessage( hwndSize, CB_SETCURSEL, i, 0L );
    }
}

static void initHwnds( HWND hwndDlg )
{
    char    tmp[5];

    hwndTypeface = GetDlgItem( hwndDlg, FT_TYPEFACE );
    hwndStyle = GetDlgItem( hwndDlg, FT_STYLE );
    hwndSize = GetDlgItem( hwndDlg, FT_SIZE );
    hwndPick = GetDlgItem( hwndDlg, FT_FTPICK );
    hwndSizeEdit = GetWindow( hwndSize, GW_CHILD );
    GetClassName( hwndSizeEdit, tmp, 5 );
    if( stricmp( tmp, "edit" ) != 0 ) {
        hwndSizeEdit = GetWindow( hwndSizeEdit, GW_HWNDNEXT );
    }

    SubclassGenericAdd( hwndTypeface, (WNDPROC)MakeWndProcInstance( HotkeyProc, InstanceHandle ) );
    SubclassGenericAdd( hwndStyle,    (WNDPROC)MakeWndProcInstance( HotkeyProc, InstanceHandle ) );
    SubclassGenericAdd( hwndSize,     (WNDPROC)MakeWndProcInstance( HotkeyProc, InstanceHandle ) );
    SubclassGenericAdd( hwndSizeEdit, (WNDPROC)MakeWndProcInstance( HotkeyProc, InstanceHandle ) );
}

static void doneWithHwnds( void )
{
    SubclassGenericRemove( hwndTypeface );
    SubclassGenericRemove( hwndStyle );
    SubclassGenericRemove( hwndSize );
    SubclassGenericRemove( hwndSizeEdit );
}

static int setCurLogfont( int overrideSize )
{
    int                 index;
    char                size[8];
    int                 height;
    HDC                 hdc;
    FARPROC             fp;

    if( overrideSize == 0 ) {
        index = (int)SendMessage( hwndSize, CB_GETCURSEL, 0, 0L );
        if( index == CB_ERR ) {
            return( 0 );
        }
        SendMessage( hwndSize, CB_GETLBTEXT, index, (LPARAM)size );
        height = atoi( size );
    } else {
        height = overrideSize;
    }
    CurLogfont.lfHeight = -height;
    CurLogfont.lfWidth = 0;
    CurLogfont.lfEscapement = 0;
    CurLogfont.lfOrientation = 0;
    index = (int)SendMessage( hwndStyle, LB_GETCURSEL, 0, 0L );
    if( index == LB_ERR )
        return( 0 );
    CurLogfont.lfWeight = ( Style[index] & STYLE_BOLD ) ? FW_BOLD : FW_NORMAL;
    CurLogfont.lfItalic = ( Style[index] & STYLE_ITALIC ) ? 1 : 0;
    CurLogfont.lfUnderline = 0;
    CurLogfont.lfStrikeOut = 0;
    /* use defaults set below by SetupFontData */
#if 0
    CurLogfont.lfCharSet = ANSI_CHARSET;
    CurLogfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
    CurLogfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    CurLogfont.lfQuality = PROOF_QUALITY;
    CurLogfont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
#endif
    index = (int)SendMessage( hwndTypeface, LB_GETCURSEL, 0, 0L );
    if( index == LB_ERR ) {
        return( 0 );
    }
    SendMessage( hwndTypeface, LB_GETTEXT, index, (LPARAM)CurLogfont.lfFaceName );

    /* set up defaults for charset, etc. from info for 1st font of this type */
    hdc = GetDC( edit_container_id );
    fp = MakeFontEnumProcInstance( SetupFontData, InstanceHandle );
    EnumFontFamilies( hdc, CurLogfont.lfFaceName, (FONTENUMPROC)fp, 0L );
    FreeProcInstance( fp );
    ReleaseDC( edit_container_id, hdc );

    return( 1 );
}

/*
 * FtDlgProc - callback routine for font drag & drop dialog
 */
WINEXPORT INT_PTR CALLBACK FtDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    static int  inSELCHANGE = 0;
    static bool dontQuit = false;
    char        str[10];
    LRESULT     len;
    int         i;
    WORD        cmd;

#ifdef __NT__
    lparam = lparam;
#endif

    switch( msg ) {
    case WM_INITDIALOG:
        hFontbar = hwnd;
        MoveWindowTopRight( hwnd );
        initHwnds( hwnd );
        fillTypefaceBox( hwnd );
        setDefaultTypeface();
        fillInfoBoxes( hwnd );
        setDefaultSizeStyle();
        if( setCurLogfont( 0 ) ) {
            InvalidateRect( hwndPick, NULL, TRUE );
            UpdateWindow( hwndPick );
        }
        return( TRUE );
    case WM_CLOSE:
        doneWithHwnds();
        DestroyWindow( hwnd );
        hFontbar = NO_WINDOW;
        // update editflags (may have closed from system menu)
        EditFlags.Fontbar = false;
        break;
    case WM_COMMAND:
        cmd = LOWORD( wparam );
        switch( cmd ) {
        case IDOK:
            if( dontQuit ) {
                dontQuit = false;
                break;
            }
            // fall through
        case IDCANCEL:
            EndDialog( hwnd, TRUE );
            return( TRUE );
        case FT_SIZE:
        case FT_STYLE:
        case FT_TYPEFACE:
            dontQuit = false;
            if( GET_WM_COMMAND_CMD( wparam, lparam ) == CBN_SELCHANGE ||
                GET_WM_COMMAND_CMD( wparam, lparam ) == LBN_SELCHANGE ) {
                inSELCHANGE++;
                if( cmd == FT_TYPEFACE ) {
                    fillInfoBoxes( hwnd );
                }
                if( (inSELCHANGE == 1) && setCurLogfont( 0 ) ) {
                    InvalidateRect( hwndPick, NULL, TRUE );
                    UpdateWindow( hwndPick );
                }
                inSELCHANGE--;
                return( FALSE );
            }
            break;
        }
        break;
    case DM_GETDEFID:
        /* Hack!  User hit enter, so grab text from combo box edit control
           & attempt to update pick sample.
        */
        if( (GetFocus() != hwndSizeEdit) || (SendMessage( hwndSize, CB_GETCURSEL, 0, 0L ) != CB_ERR) ) {
            break;
        }

        len = SendMessage( hwndSizeEdit, EM_GETLINE, 0, (LPARAM)str );
        str[len] = '\0';
        GetWindowText( hwndSizeEdit, str, 5 );
        i = atoi( str );
        if( (i != 0) && setCurLogfont( i ) ) {
            InvalidateRect( hwndPick, NULL, TRUE );
            UpdateWindow( hwndPick );
        }
        dontQuit = true;
        return( TRUE );
    }
    return( FALSE );

} /* FtDlgProc */

/*
 * RefreshFontbar - turn font bar on/off to reflect current editflag state
 */
void RefreshFontbar( void )
{
    static FARPROC  proc = NULL;

    if( EditFlags.Fontbar ) {
        if( !BAD_ID( hFontbar ) ) {
            return;
        }
        proc = MakeDlgProcInstance( FtDlgProc, InstanceHandle );
        hFontbar = CreateDialog( InstanceHandle, "FTBAR", root_window_id, (DLGPROC)proc );
        SetMenuHelpString( "Ctrl affects all syntax elements" );
    } else {
        if( BAD_ID( hFontbar ) ) {
            return;
        }
        SendMessage( hFontbar, WM_CLOSE, 0, 0L );
        FreeProcInstance( proc );
        SetMenuHelpString( "" );
    }
    UpdateStatusWindow();

} /* RefreshFontbar */
