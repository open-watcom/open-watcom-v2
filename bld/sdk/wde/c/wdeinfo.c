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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "wdeglbl.h"
#include <mbstring.h>
#include "wdemsgbx.h"
#include "rcstr.gh"
#include "wderesin.h"
#include "wdegeted.h"
#include "wdeobjid.h"
#include "wde_wres.h"
#include "wdefutil.h"
#include "wderibbn.h"
#include "wdedebug.h"
#include "wdemain.h"
#include "wdectl3d.h"
#include "wde_rc.h"
#include "wdeinfo.h"
#include "wrdll.h"
#include "jdlg.h"

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT BOOL CALLBACK WdeInfoWndProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* external variables                                                       */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static void WdeEnableInfoWindowInput( bool );
static void WdeResetInfo( void );
static void WdeChangeInfo( void );
static void WdeDisplayDialogInfo( WdeInfoStruct * );
static void WdeDisplayControlInfo( WdeInfoStruct * );
static void WdeChangeDialogInfo( WdeInfoStruct * );
static void WdeChangeControlInfo( WdeInfoStruct * );
static void WdeInfoLookupComboEntry( HWND, WORD );
static void WdeWriteSymInfo( WdeInfoStruct *, bool, char * );
static void WdeAddUniqueStringToCombo( HWND hdlg, int id, char *str );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static HWND             WdeInfoWindow           = NULL;
static HBRUSH           WdeInfoBrush            = NULL;
static COLORREF         WdeInfoColor            = 0;
static int              WdeInfoWindowDepth      = 0;
static DLGPROC          WdeInfoWinProc          = NULL;
static WdeInfoStruct    WdeCurrentInfo;
static char             *WdeCaptionText         = NULL;
static char             *WdeDlgNameText         = NULL;
static char             *WdeTextText            = NULL;
static char             *WdeIDText              = NULL;

static void WdeFiniInfoText( void )
{
    if( WdeCaptionText != NULL ) {
        WdeFreeRCString( WdeCaptionText );
    }
    if( WdeDlgNameText != NULL ) {
        WdeFreeRCString( WdeDlgNameText );
    }
    if( WdeTextText != NULL ) {
        WdeFreeRCString( WdeTextText );
    }
    if( WdeIDText != NULL ) {
        WdeFreeRCString( WdeIDText );
    }
}

static bool WdeInitInfoText( void )
{
    WdeCaptionText = WdeAllocRCString( WDE_INFOCAPTIONTEXT );
    WdeDlgNameText = WdeAllocRCString( WDE_INFODLGNAMETEXT );
    WdeTextText = WdeAllocRCString( WDE_INFOTEXTTEXT );
    WdeIDText = WdeAllocRCString( WDE_INFOIDTEXT );

    return( WdeCaptionText != NULL && WdeDlgNameText != NULL &&
            WdeTextText != NULL && WdeIDText != NULL );
}

void WdeAddUniqueStringToCombo( HWND hdlg, int id, char *str )
{
    HWND        cbox;
    LRESULT     pos;

    cbox = GetDlgItem( hdlg, id );
    if( cbox == (HWND)NULL ) {
        return;
    }
    pos = SendMessage( cbox, CB_FINDSTRINGEXACT, 0, (LPARAM)(LPSTR)str );
    if( pos == CB_ERR ) {
        SendMessage( cbox, CB_ADDSTRING, 0, (LPARAM)(LPSTR)str );
    }
}

void WdeSetFocusToInfo( void )
{
    if( WdeInfoWindowDepth != 0 && WdeInfoWindow != (HWND)NULL ) {
        SetFocus( WdeInfoWindow );
    }
}

BOOL WdeIsInfoMessage( MSG *msg )
{
    if ( WdeInfoWindowDepth != 0 && WdeInfoWindow != (HWND)NULL ) {
        return( IsDialogMessage( WdeInfoWindow, msg ) );
    } else {
        return( FALSE );
    }
}

HWND WdeGetInfoWindowHandle( void )
{
    return( WdeInfoWindow );
}

int WdeGetInfoWindowDepth( void )
{
    RECT rect;

    if( WdeInfoWindow != (HWND)NULL && IsWindowVisible ( WdeInfoWindow ) &&
        WdeInfoWindowDepth == 0 ) {
        GetWindowRect( WdeInfoWindow, &rect );
        WdeInfoWindowDepth = rect.bottom - rect.top;
    }

    return( WdeInfoWindowDepth );
}

bool WdeCreateInfoWindow( HWND main_window, HINSTANCE inst )
{
    if( !WdeInitInfoText() ) {
        return( FALSE );
    }

    WdeInfoWinProc = (DLGPROC)MakeProcInstance ( (FARPROC)WdeInfoWndProc, inst );

    WdeInfoColor = GetSysColor( COLOR_BTNFACE );
    WdeInfoBrush = CreateSolidBrush( WdeInfoColor );

    WdeInfoWindow = JCreateDialog( inst, "WdeInfo", main_window, WdeInfoWinProc );

    /* if the window could not be created return FALSE */
    if ( WdeInfoWindow == NULL ) {
        WdeWriteTrail( "WdeCreateInfoWindow: Could not create info window!" );
        return( FALSE );
    }

    WdeInfoWindowDepth = 0;

    WdeResizeWindows();

    return( TRUE );
}

void WdeInfoFini( void )
{
    WdeFiniInfoText();
    if( WdeInfoWinProc != NULL ) {
        FreeProcInstance( (FARPROC)WdeInfoWinProc );
    }
    if( WdeInfoBrush != NULL ) {
        DeleteObject( WdeInfoBrush );
    }
}

void WdeDestroyInfoWindow( void )
{
    if( WdeInfoWindow != NULL ) {
        DestroyWindow( WdeInfoWindow );
        WdeInfoWindow = NULL;
        WdeInfoWindowDepth = 0;
    }
    WdeResizeWindows();
}

void WdeShowInfoWindow( bool show )
{
    if( WdeInfoWindowDepth != 0 ) {
        if( show ) {
            ShowWindow( WdeInfoWindow, SW_SHOW );
            WdeInfoWindowDepth = WdeGetInfoWindowDepth();
        } else {
            ShowWindow( WdeInfoWindow, SW_HIDE );
            WdeInfoWindowDepth = 0;
        }
    } else {
        if( show ) {
            ShowWindow( WdeInfoWindow, SW_SHOW );
        }
    }

    WdeResizeWindows();
}

void WdeEnableInfoWindowInput( bool enable )
{
    HWND        win;
    static bool last = TRUE;

    if( WdeInfoWindowDepth != 0 && last != enable ) {
        win = GetDlgItem( WdeInfoWindow, IDB_INFO_CAPTION );
        EnableWindow( win, enable );
        win = GetDlgItem( WdeInfoWindow, IDB_INFO_IDSTR );
        EnableWindow( win, enable );
        win = GetDlgItem( WdeInfoWindow, IDB_INFO_IDNUM );
        EnableWindow( win, enable );
        win = GetDlgItem( WdeInfoWindow, IDB_INFO_SET );
        EnableWindow( win, enable );
        win = GetDlgItem( WdeInfoWindow, IDB_INFO_RESET );;
        EnableWindow( win, enable );
        last = enable;
    }
}

void WdeResizeInfoWindow( RECT *rect )
{
    if( WdeInfoWindowDepth != 0 && WdeInfoWindow != NULL ) {
        MoveWindow( WdeInfoWindow, 0, WdeGetRibbonHeight(),
                    rect->right  - rect->left, WdeInfoWindowDepth, TRUE );
        RedrawWindow ( WdeInfoWindow, NULL, (HRGN) NULL,
                       RDW_INTERNALPAINT | RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW );
    }
}

void WdeWriteSymInfo( WdeInfoStruct *is, bool same_as_last, char *s )
{
    bool    dirty;
    OBJPTR  obj;

    if( is->res_info->hash_table != NULL ) {
        dirty = WdeIsHashTableTouched( is->res_info->hash_table );
        if( dirty && (obj = GetMainObject()) != NULL ) {
            Forward( obj, RESOLVE_SYMBOL, NULL, NULL );
            Forward( obj, RESOLVE_HELPSYMBOL, NULL, NULL );   /* JPK */
        }
        if( !same_as_last || dirty ) {
            WdeAddSymbolsToComboBox( is->res_info->hash_table,
                                     WdeInfoWindow, IDB_INFO_IDSTR );
            WdeUntouchHashTable( is->res_info->hash_table );
        }
    } else {
        if( !same_as_last ) {
            SendDlgItemMessage( WdeInfoWindow, IDB_INFO_IDSTR, CB_RESETCONTENT, 0, 0 );
        }
    }

    if( s != NULL ) {
        WdeSetComboWithStr( s, WdeInfoWindow, IDB_INFO_IDSTR );
    }
}

void WdeWriteInfo( WdeInfoStruct *is )
{
    static WdeHashTable *last_table = NULL;
    static WdeResInfo   *last_res = NULL;
    static OBJ_ID       last_obj = 0;
    char                *cap_text, *id;
    bool                same_hash;

    if( WdeInfoWindowDepth == 0 || WdeInfoWindow == NULL ) {
        return;
    }

    if( is == NULL || is->obj == NULL || is->res_info == NULL ) {
        WdeWriteTrail( "WdeWriteDialogInfo: NULL parameter!" );
        return;
    }

    /* make sure the resource window is not robbed of focus */
    if( GetFocus() != is->res_info->res_win ) {
        SetFocus( is->res_info->res_win );
    }

    WdeCurrentInfo = *is;

    same_hash = (last_res == is->res_info && last_table == is->res_info->hash_table);

    WdeWriteSymInfo( is, same_hash, NULL );

    last_table = is->res_info->hash_table;
    last_res = is->res_info;

    if( is->obj_id == BASE_OBJ ) {
        WdeSetEditWithStr( "", WdeInfoWindow, IDB_INFO_SIZE );
        WdeEnableInfoWindowInput( FALSE );
        WdeSetEditWithStr( "", WdeInfoWindow, IDB_INFO_CAPTION );
        WdeSetEditWithStr( "", WdeInfoWindow, IDB_INFO_IDSTR );
        WdeSetEditWithStr( "", WdeInfoWindow, IDB_INFO_IDNUM );
    } else {
        WdeWriteObjectDimensions( (int_16)is->size.x, (int_16)is->size.y,
                                  (int_16)is->size.width, (int_16)is->size.height );
        WdeEnableInfoWindowInput( TRUE );
        if( is->obj_id == DIALOG_OBJ ) {
            WdeDisplayDialogInfo( is );
        } else if( is->obj_id == CONTROL_OBJ ) {
            WdeDisplayControlInfo( is );
        }
    }

    if( last_obj != is->obj_id ) {
        if( is->obj_id == DIALOG_OBJ ) {
            cap_text = WdeCaptionText;
            id = WdeDlgNameText;
        } else if( is->obj_id == CONTROL_OBJ ) {
            cap_text = WdeTextText;
            id = WdeIDText;
        } else if( is->obj_id == BASE_OBJ ) {
            cap_text = "";
            id       = "";
        }
        WdeSetEditWithStr( cap_text, WdeInfoWindow, IDB_INFO_CAPTEXT );
        WdeSetEditWithStr( id, WdeInfoWindow, IDB_INFO_NAMETEXT );
        last_obj = is->obj_id;
    }
}

void WdeResetInfo( void )
{
    if( WdeCurrentInfo.obj_id == DIALOG_OBJ ) {
        WdeDisplayDialogInfo( &WdeCurrentInfo );
    } else if( WdeCurrentInfo.obj_id == CONTROL_OBJ ) {
        WdeDisplayControlInfo( &WdeCurrentInfo );
    }
}

void WdeDisplayDialogInfo( WdeInfoStruct *is )
{
    char          *str;
    WResID        *name;

    str = WRConvertStringFrom( is->d.caption, "\t\n", "tn" );
    if( str == NULL ) {
        WdeSetEditWithStr( "", WdeInfoWindow, IDB_INFO_CAPTION );
    } else {
        WdeSetEditWithStr( str, WdeInfoWindow, IDB_INFO_CAPTION );
        WRMemFree( str );
    }

    name = is->d.name;
    if( name->IsName ) {
        char    *str1, *str2;
        int     len;
        bool    ok;

        ok = false;
        str1 = WResIDToStr( name );
        if( str1 != NULL ) {
            len = strlen( str1 ) + 3;
            str2 = WRMemAlloc( len );
            if( str2 != NULL ) {
                str2[0] = '"';
                strcpy( &str2[1], str1 );
                str2[len - 2] = '"';
                str2[len - 1] = '\0';
                WdeSetComboWithStr( str2, WdeInfoWindow, IDB_INFO_IDSTR );
                WRMemFree( str2 );
                ok = true;
            }
            WRMemFree( str1 );
        }
        if( !ok ) {
            WdeSetComboWithStr( "", WdeInfoWindow, IDB_INFO_IDSTR );
        }
        WdeSetEditWithStr( "", WdeInfoWindow, IDB_INFO_IDNUM );
    } else {
        if( is->symbol != NULL ) {
            WdeSetComboWithStr( is->symbol, WdeInfoWindow, IDB_INFO_IDSTR );
        } else {
            WdeSetEditWithSINT16( (int_16)name->ID.Num, 10, WdeInfoWindow, IDB_INFO_IDSTR );
        }
        WdeSetEditWithSINT16( (int_16)name->ID.Num, 10, WdeInfoWindow, IDB_INFO_IDNUM );
    }
}

void WdeDisplayControlInfo( WdeInfoStruct *is )
{
    char             *str;
    char             *cp;

    str = NULL;
    cp = WdeResNameOrOrdinalToStr( is->c.text, 10 );
    if( cp != NULL ) {
        str = WRConvertStringFrom( cp, "\t\n", "tn" );
        WRMemFree( cp );
    }

    if( str != NULL ) {
        WdeSetEditWithStr( str, WdeInfoWindow, IDB_INFO_CAPTION );
        WRMemFree( str );
    } else {
        WdeSetEditWithStr( "", WdeInfoWindow, IDB_INFO_CAPTION );
    }

    if( is->symbol ) {
        WdeSetComboWithStr( is->symbol, WdeInfoWindow, IDB_INFO_IDSTR );
    } else {
        WdeSetEditWithSINT16( (int_16)is->c.id, 10, WdeInfoWindow, IDB_INFO_IDSTR );
    }

    WdeSetEditWithSINT16( (int_16)is->c.id, 10, WdeInfoWindow, IDB_INFO_IDNUM );
}

void WdeChangeInfo( void )
{
    if( WdeCurrentInfo.obj_id == DIALOG_OBJ ) {
        WdeChangeDialogInfo( &WdeCurrentInfo );
    } else if( WdeCurrentInfo.obj_id == CONTROL_OBJ ) {
        WdeChangeControlInfo( &WdeCurrentInfo );
    }
}

void WdeChangeDialogInfo( WdeInfoStruct *is )
{
    WdeInfoStruct       c_is;
    char                *str;
    char                *cp;
    bool                quoted_str;
    bool                str_is_ordinal;
    uint_16             ord;
    bool                found;
    WdeHashValue        value;

    c_is = *is;

    str = NULL;
    cp = WdeGetStrFromEdit( WdeInfoWindow, IDB_INFO_CAPTION, NULL );
    if( cp != NULL ) {
        str = WRConvertStringTo( cp, "\t\n", "tn" );
        WRMemFree( cp );
    }
    c_is.d.caption = str;

    str = WdeGetStrFromCombo( WdeInfoWindow, IDB_INFO_IDSTR );
    if( str == NULL ) {
        WRMemFree( c_is.d.caption );
        c_is.d.caption = NULL;
        return;
    }

    WRStripSymbol( str );

    quoted_str = FALSE;
    if( _mbclen( (unsigned char *)str ) == 1 && str[0] == '"' ) {
        unsigned char   *s;

        str[0] = ' ';
        cp = NULL;
        for( s = (unsigned char *)str; *s != '\0'; s = _mbsinc( s ) ) {
            if( _mbclen( s ) == 1 && *s == '"' ) {
                cp = (char *)s;
            }
        }
        if( cp != NULL ) {
            *cp = '\0';
        }
        WRStripSymbol( str );
        quoted_str = TRUE;
    }

    if( str[0] == '\0' ) {
        WRMemFree( str );
        WRMemFree( c_is.d.caption );
        c_is.d.caption = NULL;
        return;
    }

    ord = (uint_16)strtoul( str, &cp, 0 );
    str_is_ordinal = (*cp == '\0');

    c_is.symbol = NULL;

    if( quoted_str ) {
        c_is.d.name = WResIDFromStr( str );
        WRMemFree( str );
    } else if( str_is_ordinal ) {
        c_is.d.name = WResIDFromNum( ord );
        WRMemFree( str );
    } else {
        if( !WdeIsValidSymbol( str ) ) {
            WRMemFree( str );
            WRMemFree( c_is.d.caption );
            c_is.d.caption = NULL;
            return;
        }
        strupr( str );
        c_is.symbol = str;
        c_is.d.name = NULL;
    }

    Forward( is->obj, MODIFY_INFO, &c_is, NULL );

    if( c_is.symbol ) {
        WdeAddUniqueStringToCombo( WdeInfoWindow, IDB_INFO_IDSTR, c_is.symbol );
        value = WdeLookupName( c_is.res_info->hash_table, c_is.symbol, &found );
        if( found ) {
            WdeSetEditWithSINT32( (int_32)value, 10, WdeInfoWindow, IDB_INFO_IDNUM );
        }
    } else if( str_is_ordinal ) {
        WdeSetEditWithSINT32( (int_32)ord, 10, WdeInfoWindow, IDB_INFO_IDNUM );
    } else {
        WdeSetEditWithStr( "", WdeInfoWindow, IDB_INFO_IDNUM );
    }

    *is = c_is;
}

void WdeChangeControlInfo( WdeInfoStruct *is )
{
    char                *str;
    char                *cp;
    WdeInfoStruct       c_is;
    bool                str_is_ordinal;
    uint_16             ord;
    bool                found;
    WdeHashValue        value;

    c_is = *is;

    str = NULL;
    cp = WdeGetStrFromEdit( WdeInfoWindow, IDB_INFO_CAPTION, NULL );
    if( cp != NULL ) {
        str = WRConvertStringTo( cp, "\t\n", "tn" );
        WRMemFree( cp );
    }

    if( str != NULL ) {
        c_is.c.text = ResStrToNameOrOrd( str );
        WRMemFree( str );
    } else {
        c_is.c.text = NULL;
    }

    str = WdeGetStrFromCombo( WdeInfoWindow, IDB_INFO_IDSTR );
    if( str == NULL ) {
        WRMemFree( c_is.c.text );
        c_is.c.text = NULL;
        return;
    }

    WRStripSymbol( str );

    if( str[0] == '\0' ) {
        WRMemFree( str );
        WRMemFree( c_is.c.text );
        c_is.c.text = NULL;
        return;
    }

    ord = (uint_16)strtoul( str, &cp, 0 );
    str_is_ordinal = (*cp == '\0');

    c_is.symbol = NULL;

    if( str_is_ordinal ) {
        c_is.c.id = ord;
        WRMemFree( str );
    } else {
        if( !WdeIsValidSymbol( str ) ) {
            WRMemFree( str );
            WRMemFree( c_is.c.text );
            c_is.c.text = NULL;
            return;
        }
        strupr( str );
        c_is.symbol = str;
    }

    Forward( c_is.obj, MODIFY_INFO, &c_is, NULL );

    if( c_is.symbol ) {
        WdeAddUniqueStringToCombo( WdeInfoWindow, IDB_INFO_IDSTR, c_is.symbol );
        value = WdeLookupName( c_is.res_info->hash_table, c_is.symbol, &found );
        if( found ) {
            WdeSetEditWithSINT32( (int_32)value, 10, WdeInfoWindow, IDB_INFO_IDNUM );
        }
    } else if( str_is_ordinal ) {
        WdeSetEditWithSINT32( (int_32)ord, 10, WdeInfoWindow, IDB_INFO_IDNUM );
    } else {
        WdeSetEditWithStr( "", WdeInfoWindow, IDB_INFO_IDNUM );
    }

    *is = c_is;
}

void WdeWriteObjectDimensions( int x, int y, int width, int height )
{
    char str[56];

    sprintf( str, "(%d,%d) (%d,%d) %dx%d",
             x, y, x + width, y + height, width, height );

    WdeSetEditWithStr( str, WdeInfoWindow, IDB_INFO_SIZE );
}

void WdeInfoLookupComboEntry( HWND hWnd, WORD hw )
{
    char                *cp;
    char                *str;
    WdeHashValue        value;
    bool                found;
    int                 index;
    int                 count;

    if( WdeCurrentInfo.res_info->hash_table == NULL ) {
        return;
    }

    count = (int)SendDlgItemMessage( hWnd, IDB_INFO_IDSTR, CB_GETCOUNT, 0, 0L );
    if( count == 0 || count == CB_ERR ) {
        return;
    }

    str = NULL;
    if( hw == CBN_EDITCHANGE ) {
        str = WdeGetStrFromCombo( hWnd, IDB_INFO_IDSTR );
    } else {
        index = (int)SendDlgItemMessage( hWnd, IDB_INFO_IDSTR, CB_GETCURSEL, 0, 0L );
        if( index != CB_ERR ) {
            str = WdeGetStrFromComboLBox( hWnd, IDB_INFO_IDSTR, index );
        }
    }

    if( str == NULL ) {
        return;
    }

    WRStripSymbol( str );

    // if the string numeric or empty then return
    strtoul( str, &cp, 0 );
    if( *cp == '\0' ) {
        WRMemFree( str );
        return;
    }

    if( _mbclen( (unsigned char *)str ) == 1 && str[0] == '"' ) {
        value = WdeLookupName( WdeCurrentInfo.res_info->hash_table, str, &found );
        if( found ) {
            WdeSetEditWithSINT32( (int_32)value, 10, hWnd, IDB_INFO_IDNUM );
        }
    }

    WRMemFree( str );
}

WINEXPORT BOOL CALLBACK WdeInfoWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    WORD    w;
    RECT    r;
    HDC     dc;

    _wde_touch( lParam );

    switch( message ) {
#if defined( __NT__ )
    case WM_INITDIALOG:
        SetWindowLong( hWnd, GWL_STYLE, WS_CHILD );
        break;
#endif
    case WM_SYSCOLORCHANGE:
#if defined( __NT__ )
        WdeInfoColor = GetSysColor( COLOR_BTNFACE );
        DeleteObject( WdeInfoBrush );
        WdeInfoBrush = CreateSolidBrush( WdeInfoColor );
#endif
        WdeCtl3dColorChange ();
        break;

#ifdef __NT__
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORSTATIC:
        dc = (HDC)wParam;
        SetBkColor( dc, WdeInfoColor );
        return( WdeInfoBrush != NULL );

    case WM_CTLCOLORMSGBOX:
    case WM_CTLCOLOREDIT:
        break;
#else
    case WM_CTLCOLOR:
        w = HIWORD( lParam );
        if( w != CTLCOLOR_MSGBOX && w != CTLCOLOR_EDIT ) {
            dc = (HDC)wParam;
            SetBkColor( dc, WdeInfoColor );
            return( WdeInfoBrush != NULL );
        }
        break;
#endif

    case WM_ERASEBKGND:
#if defined( __NT__ )
        if( WdeInfoColor != GetSysColor( COLOR_BTNFACE ) ) {
            /* Fake it, this proc does not get it ... */
            SendMessage( hWnd, WM_SYSCOLORCHANGE, (WPARAM)0, (LPARAM)0 );
        }
#endif
        GetClientRect( hWnd, &r );
        UnrealizeObject( WdeInfoBrush );
        FillRect( (HDC)wParam, &r, WdeInfoBrush );
        return( TRUE );

    case WM_COMMAND:
        w = GET_WM_COMMAND_CMD( wParam, lParam );
        switch( LOWORD( wParam ) ) {
        case IDB_INFO_IDSTR:
            if( w == CBN_EDITCHANGE || w == CBN_SELCHANGE ) {
                WdeInfoLookupComboEntry( hWnd, w );
            }
            break;

        case IDB_INFO_SET:
            WdeChangeInfo();
            SetFocus( WdeGetMainWindowHandle() );
            break;

        case IDB_INFO_RESET:
            WdeResetInfo();
            break;

        case IDCANCEL:
            WdeResetInfo();
            SetFocus( WdeGetMainWindowHandle() );
            break;
        }
        break;
    }
    return( FALSE );
}
