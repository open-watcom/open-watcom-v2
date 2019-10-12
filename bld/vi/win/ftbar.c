/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
#include "ftbar.rh"
#include "utils.h"
#include "sstyle.h"
#include "vifont.h"
#include "subclass.h"
#include "hotkey.h"


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
WINEXPORT int CALLBACK EnumFamFaceNames( const LOGFONT *lf, const TEXTMETRIC *tm, int FontType, LPARAM lparam );
WINEXPORT int CALLBACK EnumFamInfo( const LOGFONT *lf, const TEXTMETRIC *tm, int FontType, LPARAM lparam );
WINEXPORT int CALLBACK SetupFontData( const LOGFONT *lf, const TEXTMETRIC *tm, int FontType, LPARAM lparam );
#elif defined( __WINDOWS__ )
WINEXPORT int CALLBACK EnumFamFaceNames( const ENUMLOGFONT FAR *elf, const NEWTEXTMETRIC FAR *ntm, int FontType, LPARAM lparam );
WINEXPORT int CALLBACK EnumFamInfo( const ENUMLOGFONT FAR *elf, const NEWTEXTMETRIC FAR *ntm, int FontType, LPARAM lparam );
WINEXPORT int CALLBACK SetupFontData( const ENUMLOGFONT FAR *elf, const NEWTEXTMETRIC FAR *ntm, int FontType, LPARAM lparam );
#else
WINEXPORT int CALLBACK EnumFamFaceNames( const LOGFONT FAR *lf, const TEXTMETRIC FAR *tm, DWORD FontType, LPARAM lparam );
WINEXPORT int CALLBACK EnumFamInfo( const LOGFONT FAR *lf, const TEXTMETRIC FAR *tm, DWORD FontType, LPARAM lparam );
WINEXPORT int CALLBACK SetupFontData( const LOGFONT FAR *lf, const TEXTMETRIC FAR *tm, DWORD FontType, LPARAM lparam );
#endif
WINEXPORT INT_PTR CALLBACK FtDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

HWND    hwndFaceName, hwndStyle, hwndSize, hwndPick, hwndSizeEdit;
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

struct {
    boolbit     bold    : 1;
    boolbit     italic  : 1;
}       Style[MAX_STYLES];

int     NStyles, NSizes;
/* value of 96 taken from Petzold pg 512
*/
int     YPIXELS_PER_INCH = 96;


#if defined( __WINDOWS_386__ )
WINEXPORT int CALLBACK EnumFamFaceNames( const LOGFONT *lf, const TEXTMETRIC *tm, int FontType, LPARAM lparam )
#elif defined( __WINDOWS__ )
WINEXPORT int CALLBACK EnumFamFaceNames( const ENUMLOGFONT FAR *elf, const NEWTEXTMETRIC FAR *ntm, int FontType, LPARAM lparam )
#else
WINEXPORT int CALLBACK EnumFamFaceNames( const LOGFONT FAR *lf, const TEXTMETRIC FAR *tm, DWORD FontType, LPARAM lparam )
#endif
{
#ifdef __WINDOWS_386__
    char                faceName[LF_FACESIZE];
    const ENUMLOGFONT   __far *elf = MK_FP32( (void *)lf );
    (void)tm;
#elif defined( __WINDOWS__ )
    (void)ntm;
#else
    const ENUMLOGFONT   FAR *elf = (const ENUMLOGFONT FAR *)lf;
    (void)tm;
#endif
    (void)lparam; (void)FontType;

#ifdef __WINDOWS_386__
    /* On Win386, we need to pass a near 32-bit pointer with LB_ADDSTRING,
     * but we get a far pointer from Windows. Hence the shenanigans with
     * a temp buffer in the flat address space.
     */
    _fstrcpy( faceName, elf->elfLogFont.lfFaceName );
    SendMessage( hwndFaceName, LB_ADDSTRING, 0, (LPARAM)(LPSTR)faceName );
#else
    SendMessage( hwndFaceName, LB_ADDSTRING, 0, (LPARAM)(LPSTR)(elf->elfLogFont.lfFaceName) );
#endif

    return( TRUE );
}

#if defined( __WINDOWS_386__ )
WINEXPORT int CALLBACK EnumFamInfo( const LOGFONT *lf, const TEXTMETRIC *tm, int FontType, LPARAM lparam )
#elif defined( __WINDOWS__ )
WINEXPORT int CALLBACK EnumFamInfo( const ENUMLOGFONT FAR *elf, const NEWTEXTMETRIC FAR *ntm, int FontType, LPARAM lparam )
#else
WINEXPORT int CALLBACK EnumFamInfo( const LOGFONT FAR *lf, const TEXTMETRIC FAR *tm, DWORD FontType, LPARAM lparam )
#endif
{
    char                    sbuf[40];
    int                     height;
    long                    *isTrueType = (long *)lparam;
#ifdef __WINDOWS_386__
    const ENUMLOGFONT       __far *elf = MK_FP32( (void *)lf );
    tm = tm;
#elif defined( __WINDOWS__ )
    ntm = ntm;
#else
    const ENUMLOGFONT       FAR *elf = (const ENUMLOGFONT FAR *)lf;
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
        height = abs( elf->elfLogFont.lfHeight );
        sprintf( sbuf, "%d", height );
        if( SendMessage( hwndSize, CB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)(LPSTR)sbuf ) == CB_ERR ) {
            SendMessage( hwndSize, CB_INSERTSTRING, NSizes, (LPARAM)(LPSTR)sbuf );
        }
    }
    return( 1 );
}

#if defined( __WINDOWS_386__ )
WINEXPORT int CALLBACK SetupFontData( const LOGFONT *lf, const TEXTMETRIC *tm, int FontType, LPARAM lparam )
#elif defined( __WINDOWS__ )
WINEXPORT int CALLBACK SetupFontData( const ENUMLOGFONT FAR *elf, const NEWTEXTMETRIC FAR *ntm, int FontType, LPARAM lparam )
#else
WINEXPORT int CALLBACK SetupFontData( const LOGFONT FAR *lf, const TEXTMETRIC FAR *tm, DWORD FontType, LPARAM lparam )
#endif
{
#ifdef __WINDOWS_386__
    const ENUMLOGFONT       __far *elf = MK_FP32( (void *)lf );
    (void)tm;
#elif defined( __WINDOWS__ )
    (void)ntm;
#else
    const ENUMLOGFONT       FAR *elf = (const ENUMLOGFONT FAR *)lf;
    (void)tm;
#endif

    (void)FontType; (void)lparam;

    /* start setting up CurLogfont based on the font data */
    CurLogfont.lfCharSet = elf->elfLogFont.lfCharSet;
    CurLogfont.lfOutPrecision = elf->elfLogFont.lfOutPrecision;
    CurLogfont.lfClipPrecision = elf->elfLogFont.lfClipPrecision;
    CurLogfont.lfQuality = elf->elfLogFont.lfQuality;
    CurLogfont.lfPitchAndFamily = elf->elfLogFont.lfPitchAndFamily;

    /* only do this for the 1st font - we just want defaults */
    return( FALSE );
}

static void fillFaceNamesBox( HWND hwnd )
{
    FONTENUMPROC    fontenumproc;
    HDC             hdc;

    hwnd = hwnd;

    /* put facenames in combo box
    */
    hdc = GetDC( edit_container_window_id );
    fontenumproc = MakeProcInstance_FONTENUM( EnumFamFaceNames, InstanceHandle );
    EnumFontFamilies( hdc, NULL, fontenumproc, 0L );
    FreeProcInstance_FONTENUM( fontenumproc );
    ReleaseDC( edit_container_window_id, hdc );

    SendMessage( hwndFaceName, LB_SETCURSEL, 0, 0L );
}

static void fillStyleBox( void )
{
    SendMessage( hwndStyle, LB_INSERTSTRING, 0, (LPARAM)(LPCSTR)"Regular" );
    SendMessage( hwndStyle, LB_INSERTSTRING, 1, (LPARAM)(LPCSTR)"Italic" );
    SendMessage( hwndStyle, LB_INSERTSTRING, 2, (LPARAM)(LPCSTR)"Bold" );
    SendMessage( hwndStyle, LB_INSERTSTRING, 3, (LPARAM)(LPCSTR)"Bold Italic" );
    Style[0].bold = false;
    Style[0].italic = false;
    Style[1].bold = false;
    Style[1].italic = true;
    Style[2].bold = true;
    Style[2].italic = false;
    Style[3].bold = true;
    Style[3].italic = true;
    NStyles = MAX_STYLES;
}

static void fillInfoBoxes( HWND hwnd )
{
    FONTENUMPROC    fontenumproc;
    int             index;
    HDC             hdc;
    char            facename[LF_FACESIZE + 1];
    char            size[8];
    long            isTrueType = 0;
    char            oldSize[8], oldStyle[40];
    static int      vPitchSizes[] = { 8, 9, 10, 11, 12, 14, 16, 18,
                                      20, 22, 24, 26, 28, 36, 48, 72 };

    hwnd = hwnd;

    index = (int)SendMessage( hwndStyle, LB_GETCURSEL, 0, 0L );
    if( index != LB_ERR ) {
        SendMessage( hwndStyle, LB_GETTEXT, index, (LPARAM)(LPSTR)oldStyle );
    } else {
        oldStyle[0] = '\0';
    }
    index = (int)SendMessage( hwndSize, CB_GETCURSEL, 0, 0L );
    if( index != CB_ERR ) {
        SendMessage( hwndSize, CB_GETLBTEXT, index, (LPARAM)(LPSTR)oldSize );
    } else {
        oldSize[0] = '\0';
    }

    SendMessage( hwndStyle, LB_RESETCONTENT, 0, 0L );
    SendMessage( hwndSize, CB_RESETCONTENT, 0, 0L );
    NStyles = NSizes = 0;

    index = (int)SendMessage( hwndFaceName, LB_GETCURSEL, 0, 0L );
    if( index == LB_ERR ) {
        return;
    }

    fillStyleBox();

    SendMessage( hwndFaceName, LB_GETTEXT, index, (LPARAM)(LPSTR)facename );
    fontenumproc = MakeProcInstance_FONTENUM( EnumFamInfo, InstanceHandle );
    hdc = GetDC( edit_container_window_id );
    EnumFontFamilies( hdc, facename, fontenumproc, (LPARAM)(&isTrueType) );
    ReleaseDC( edit_container_window_id, hdc );
    FreeProcInstance_FONTENUM( fontenumproc );

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
            if( SendMessage( hwndSize, CB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)(LPSTR)size ) == CB_ERR ) {
                SendMessage( hwndSize, CB_INSERTSTRING, NSizes, (LPARAM)(LPSTR)size );
                NSizes++;
            }
        }
    }

    if( oldStyle[0] != '\0' ) {
        index = (int)SendMessage( hwndStyle, LB_SELECTSTRING, (WPARAM)-1, (LPARAM)(LPSTR)oldStyle );
        if( index == LB_ERR ) {
            SendMessage( hwndStyle, LB_SETCURSEL, 0, 0L );
        }
    }
    if( oldSize[0] != '\0' ) {
        index = (int)SendMessage( hwndSize, CB_SELECTSTRING, (WPARAM)-1, (LPARAM)(LPSTR)oldSize );
        if( index == CB_ERR ) {
            SendMessage( hwndSize, CB_SETCURSEL, 0, 0L );
        }
    }
}

static void setDefaultFaceName( void )
{
    int     i;
    LPSTR   typeName;

    // default default
    SendMessage( hwndFaceName, LB_SETCURSEL, 0, 0L );

    typeName = FontlfFaceName( SEType[SE_WHITESPACE].font );
    i = (int)SendMessage( hwndFaceName, LB_SELECTSTRING, (WPARAM)-1, (LPARAM)typeName );
    if( i != LB_ERR ) {
        SendMessage( hwndFaceName, LB_SETCURSEL, i, 0L );
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
    i = (int)SendMessage( hwndSize, CB_SELECTSTRING, (WPARAM)-1, (LPARAM)(LPSTR)buf );
    if( i != CB_ERR ) {
        SendMessage( hwndSize, CB_SETCURSEL, i, 0L );
    }
}

static void initHwnds( HWND hwndDlg )
{
    char    tmp[5];
    int     len;

    hwndFaceName = GetDlgItem( hwndDlg, FT_FACENAME );
    hwndStyle = GetDlgItem( hwndDlg, FT_STYLE );
    hwndSize = GetDlgItem( hwndDlg, FT_SIZE );
    hwndPick = GetDlgItem( hwndDlg, FT_FTPICK );
    hwndSizeEdit = GetWindow( hwndSize, GW_CHILD );
    len = GetClassName( hwndSizeEdit, tmp, sizeof( tmp ) );
    tmp[len] = '\0';
    if( stricmp( tmp, "edit" ) != 0 ) {
        hwndSizeEdit = GetWindow( hwndSizeEdit, GW_HWNDNEXT );
    }

    SubclassGenericAdd( hwndFaceName, HotkeyProc, InstanceHandle );
    SubclassGenericAdd( hwndStyle,    HotkeyProc, InstanceHandle );
    SubclassGenericAdd( hwndSize,     HotkeyProc, InstanceHandle );
    SubclassGenericAdd( hwndSizeEdit, HotkeyProc, InstanceHandle );
}

static void doneWithHwnds( void )
{
    SubclassGenericRemove( hwndFaceName );
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
    FONTENUMPROC        fontenumproc;

    if( overrideSize == 0 ) {
        index = (int)SendMessage( hwndSize, CB_GETCURSEL, 0, 0L );
        if( index == CB_ERR ) {
            return( 0 );
        }
        SendMessage( hwndSize, CB_GETLBTEXT, index, (LPARAM)(LPSTR)size );
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
    CurLogfont.lfWeight = ( Style[index].bold ) ? FW_BOLD : FW_NORMAL;
    CurLogfont.lfItalic = Style[index].italic;
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
    index = (int)SendMessage( hwndFaceName, LB_GETCURSEL, 0, 0L );
    if( index == LB_ERR ) {
        return( 0 );
    }
    SendMessage( hwndFaceName, LB_GETTEXT, index, (LPARAM)(LPSTR)CurLogfont.lfFaceName );

    /* set up defaults for charset, etc. from info for 1st font of this type */
    hdc = GetDC( edit_container_window_id );
    fontenumproc = MakeProcInstance_FONTENUM( SetupFontData, InstanceHandle );
    EnumFontFamilies( hdc, CurLogfont.lfFaceName, fontenumproc, 0L );
    FreeProcInstance_FONTENUM( fontenumproc );
    ReleaseDC( edit_container_window_id, hdc );

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
    (void)lparam;
#endif

    switch( msg ) {
    case WM_INITDIALOG:
        hFontbar = hwnd;
        MoveWindowTopRight( hwnd );
        initHwnds( hwnd );
        fillFaceNamesBox( hwnd );
        setDefaultFaceName();
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
        case FT_FACENAME:
            dontQuit = false;
            if( GET_WM_COMMAND_CMD( wparam, lparam ) == CBN_SELCHANGE ||
                GET_WM_COMMAND_CMD( wparam, lparam ) == LBN_SELCHANGE ) {
                inSELCHANGE++;
                if( cmd == FT_FACENAME ) {
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

        len = SendMessage( hwndSizeEdit, EM_GETLINE, 0, (LPARAM)(LPSTR)str );
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
    static DLGPROC  dlgproc = NULL;

    if( EditFlags.Fontbar ) {
        if( !BAD_ID( hFontbar ) ) {
            return;
        }
        dlgproc = MakeProcInstance_DLG( FtDlgProc, InstanceHandle );
        hFontbar = CreateDialog( InstanceHandle, "FTBAR", root_window_id, dlgproc );
        SetMenuHelpString( "Ctrl affects all syntax elements" );
    } else {
        if( BAD_ID( hFontbar ) ) {
            return;
        }
        SendMessage( hFontbar, WM_CLOSE, 0, 0L );
        FreeProcInstance_DLG( dlgproc );
        SetMenuHelpString( "" );
    }
    UpdateStatusWindow();

} /* RefreshFontbar */
