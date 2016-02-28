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


#include "commonui.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "wio.h"
#include "watcom.h"
#include "wresall.h"
#include "wglbl.h"
#include "waccel.h"
#include "winst.h"
#include "wmemf.h"
#include "wrename.h"
#include "wnewitem.h"
#include "wdel.h"
#include "wkey.h"
#include "wmsg.h"
#include "wedit.h"
#include "wstat.h"
#include "wribbon.h"
#include "whints.h"
#include "wopts.h"
#include "whndl.h"
#include "sys_rc.h"
#include "wctl3d.h"
#include "wsvobj.h"
#include "wsetedit.h"
#include "wmain.h"
#include "rcstr.gh"
#include "wacc2rc.h"
#include "weditsym.h"
#include "wstrdup.h"
#include "wrdll.h"

#include "wwinhelp.h"
#include "jdlg.h"
#include "watini.h"
#include "inipath.h"
#include "aboutdlg.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define WTIMER            666
#define ABOUT_TIMER       WTIMER
#define CLOSE_TIMER       WTIMER
#define WACC_MINTRACKX    495
#define WACC_MINTRACKY    485

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT LRESULT CALLBACK WMainWndProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static bool         WInit( HINSTANCE );
static void         WFini( void );
static WAccelInfo   *WAccelGetEInfo( WAccelHandle, bool );
static bool         WRegisterMainClass( HINSTANCE );
static bool         WCreateEditWindow( HINSTANCE, WAccelEditInfo * );
static void         WUpdateScreenPosOpt( HWND );
static bool         WCleanup( WAccelEditInfo * );
static bool         WQuerySave( WAccelEditInfo *, bool );
static bool         WQuerySaveRes( WAccelEditInfo *, bool );
static bool         WQuerySaveSym( WAccelEditInfo *, bool );
static bool         WHandleWM_CLOSE( WAccelEditInfo *, bool );
static void         WHandleClear( WAccelEditInfo * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static WAccelEditInfo   *WCurrEditInfo = NULL;
static char             WMainClass[] = "WAccMainClass";
static char             WMainMenuName[] = "WMainMenu";
static char             WMainSOMenuName[] = "WSOMenu";
static char             WProfileName[_MAX_PATH] = WATCOM_INI;
static char             WSectionName[] = "waccel";
static char             WItemClipbdFmt[] = "WACCEL_ITEM_CLIPFMT";

static int      ref_count = 0;
static HACCEL   AccelTable = NULL;

UINT            WClipbdFormat     = 0;
UINT            WItemClipbdFormat = 0;

extern int appWidth;
extern int appHeight;

static void *_MemAlloc( size_t size )
{
    return( WRMemAlloc( size ) );
}

static void _MemFree( void *p )
{
    WRMemFree( p );
}

/* set the WRES library to use compatible functions */
WResSetRtns( open, close, read, write, lseek, tell, _MemAlloc, _MemFree );

#ifdef __NT__

BOOL WINAPI DllMain( HINSTANCE inst, DWORD dwReason, LPVOID lpReserved )
{
    int ret;

    _wtouch( lpReserved );

    ret = TRUE;

    switch( dwReason ) {
    case DLL_PROCESS_ATTACH:
        ref_count = 0;
        WSetEditInstance( inst );
        break;
    case DLL_PROCESS_DETACH:
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        /* do nothing here */
        break;
    }

    return( ret );
}

#else

int WINAPI LibMain( HINSTANCE inst, WORD dataseg, WORD heapsize, LPSTR cmdline )
{
    _wtouch( dataseg );
    _wtouch( heapsize );
    _wtouch( cmdline );

    __win_alloc_flags = GMEM_MOVEABLE | GMEM_SHARE;
    __win_realloc_flags = GMEM_MOVEABLE | GMEM_SHARE;
    ref_count = 0;
    WSetEditInstance( inst );

    return( TRUE );
}

int WINAPI WEP( int parm )
{
    _wtouch( parm );

    return( TRUE );
}
#endif

void WRESEAPI WAccelInit( void )
{
    HINSTANCE   inst;

    inst = WGetEditInstance();
    if( AccelTable == (HACCEL)NULL ) {
        AccelTable = LoadAccelerators( inst, "WAccelAccelTable");
    }
    if( ref_count == 0 ) {
        WRInit();
        SetInstance( inst );
        WInit( inst );
    }
    ref_count++;
}

void WRESEAPI WAccelFini( void )
{
    ref_count--;
    if( ref_count == 0 ) {
        WFini();
        WRFini();
    }
}

WAccelHandle WRESEAPI WAccelStartEdit( WAccelInfo *info )
{
    bool            ok;
    WAccelEditInfo  *einfo;

    einfo = NULL;

    ok = (info != NULL && info->parent != NULL && info->inst != NULL);

    if( ok ) {
        if( appWidth == -1 ) {
            WInitEditDlg( WGetEditInstance(), info->parent );
        }
        ok = ((einfo = WAllocAccelEInfo()) != NULL);
    }

    if( ok ) {
        einfo->info = info;
        einfo->tbl = WMakeAccelTableFromInfo( info );
        ok = (einfo->tbl != NULL);
    }

    if( ok ) {
        if( einfo->info->file_name != NULL ) {
            einfo->file_name = WStrDup( einfo->info->file_name );
            ok = (einfo->file_name != NULL);
            if( ok ) {
                einfo->file_type = WRIdentifyFile( einfo->file_name );
                ok = (einfo->file_type != WR_DONT_KNOW);
            }
        }
    }

    if( ok ) {
        ok = WResolveAllEntrySymbols( einfo );
    }

    if( ok ) {
        ok = WCreateEditWindow( WGetEditInstance(), einfo );
    }

    if( ok ) {
        einfo->hndl = WRegisterEditSession( einfo );
        ok = (einfo->hndl != 0);
    }

    if( !ok ) {
        if( einfo != NULL ) {
            WFreeAccelEInfo( einfo );
        }
        return( 0 );
    }

    return( einfo->hndl );
}

void WRESEAPI WAccelShowWindow( WAccelHandle hndl, int show )
{
    WAccelEditInfo *einfo;

    einfo = (WAccelEditInfo *)WGetEditSessionInfo( hndl );

    if( einfo != NULL && einfo->win != (HWND)NULL ) {
        if( show ) {
            ShowWindow( einfo->win, SW_SHOWNA );
        } else {
            ShowWindow( einfo->win, SW_HIDE );
        }
    }
}

void WRESEAPI WAccelBringToFront( WAccelHandle hndl )
{
    WAccelEditInfo *einfo;

    einfo = (WAccelEditInfo *)WGetEditSessionInfo( hndl );

    if( einfo != NULL && einfo->win != (HWND)NULL ) {
        ShowWindow( einfo->win, SW_RESTORE );
        BringWindowToTop( einfo->win );
    }
}

int WRESEAPI WAccelIsDlgMsg( MSG *msg )
{
    return( WIsAccelDialogMessage( msg, AccelTable ) );
}

WAccelInfo *WRESEAPI WAccelEndEdit( WAccelHandle hndl )
{
    return( WAccelGetEInfo( hndl, FALSE ) );
}

WAccelInfo *WRESEAPI WAccelGetEditInfo( WAccelHandle hndl )
{
    return( WAccelGetEInfo( hndl, TRUE ) );
}

int WRESEAPI WAccelCloseSession( WAccelHandle hndl, int force_exit )
{
    WAccelEditInfo *einfo;

    einfo = (WAccelEditInfo *)WGetEditSessionInfo( hndl );

    if( einfo != NULL && einfo->info != NULL ) {
        if( SendMessage( einfo->win, WM_CLOSE, (WPARAM)force_exit, 0 ) != 0 ) {
            return( FALSE );
        }
    }

    return( TRUE );
}

WAccelInfo *WAccelGetEInfo( WAccelHandle hndl, bool keep )
{
    WAccelEditInfo  *einfo;
    WAccelInfo      *info;
    bool            ok;

    info = NULL;

    einfo = (WAccelEditInfo *)WGetEditSessionInfo( hndl );

    ok = (einfo != NULL);

    if( ok ) {
        info = einfo->info;
        ok = (info != NULL);
    }

    if( ok ) {
        if( einfo->info->modified ) {
            if( info->data != NULL ) {
                WRMemFree( info->data );
            }
            info->data = NULL;
            info->data_size = 0;
            WMakeDataFromAccelTable( einfo->tbl, &info->data, &info->data_size );
        }
        if( !keep ) {
            WUnRegisterEditSession( hndl );
            WFreeAccelEInfo( einfo );
        }
    }

    return( info );
}

bool WInit( HINSTANCE inst )
{
    bool ok;

    ok = (inst != (HINSTANCE)NULL);

    if( ok ) {
        WCtl3DInit( inst );
        ok = JDialogInit();
    }

    if( ok ) {
        ok = WRegisterMainClass( inst );
    }

    if( ok ) {
        ok = WInitStatusLines( inst );
    }

    if( ok ) {
        WClipbdFormat = RegisterClipboardFormat( WR_CLIPBD_ACCEL );
        ok = (WClipbdFormat != 0);
    }

    if( ok ) {
        WItemClipbdFormat = RegisterClipboardFormat( WItemClipbdFmt );
        ok = (WItemClipbdFormat != 0);
    }

    if( ok ) {
        GetConfigFilePath( WProfileName, sizeof( WProfileName ) );
        strcat( WProfileName, "\\" WATCOM_INI );
        WInitOpts( WProfileName, WSectionName );
        WInitEditWindows( inst );
        ok = WInitRibbons( inst );
    }

    return( ok );
}

void WFini( void )
{
    HINSTANCE inst;

    inst = WGetEditInstance();

    WFiniStatusLines();
    WOptsShutdown();
    WShutdownRibbons();
    WShutdownToolBars();
    WFiniEditWindows();
    WCtl3DFini( inst );
    UnregisterClass( WMainClass, inst );
    JDialogFini();
}

bool WRegisterMainClass( HINSTANCE inst )
{
    WNDCLASS wc;

    /* fill in the window class structure for the main window */
    wc.style = CS_DBLCLKS | CS_GLOBALCLASS;
    wc.lpfnWndProc = WMainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( LONG_PTR );
    wc.hInstance = inst;
    wc.hIcon = LoadIcon( inst, "APPLICON" );
    wc.hCursor = LoadCursor( (HINSTANCE)NULL, IDC_ARROW );
    wc.hbrBackground = NULL;
    wc.lpszMenuName = WMainMenuName;
    wc.lpszClassName = WMainClass;

    return( RegisterClass( &wc ) != 0 );
}

char *WCreateEditTitle( WAccelEditInfo *einfo )
{
    char        *title;
    char        *fname;
    char        *text;
    int         offset;
    size_t      len;

    title = NULL;
    fname = NULL;

    if( einfo == NULL ) {
        return( NULL );
    }

    if( einfo->file_name == NULL ) {
        fname = einfo->info->file_name;
    } else {
        fname = einfo->file_name;
    }

    text = AllocRCString( W_ACCELAPPTITLE );

    if( fname == NULL || text == NULL ) {
        return( NULL );
    }

    offset = WRFindFnOffset( fname );
    fname = &fname[offset];
    len = strlen( fname ) + strlen( text ) + 6;
    title = (char *)WRMemAlloc( len );
    if( title != NULL ) {
        strcpy( title, text );
        strcat( title, " - [" );
        strcat( title, fname );
        strcat( title, "]" );
    }

    if( text != NULL ) {
        FreeRCString( text );
    }

    return( title );
}

void WSetEditTitle( WAccelEditInfo *einfo )
{
    char        *title;
    bool        is_rc;

    title = WCreateEditTitle( einfo );
    is_rc = FALSE;

    if( title == NULL ) {
        title = AllocRCString( W_ACCELAPPTITLE );
        is_rc = TRUE;
    }

    if( title != NULL ) {
        SendMessage( einfo->win, WM_SETTEXT, 0, (LPARAM)title );
        if( is_rc ) {
            FreeRCString( title );
        } else {
            WRMemFree( title );
        }
    }
}

bool WCreateEditWindow( HINSTANCE inst, WAccelEditInfo *einfo )
{
    int         x, y, width, height;
    char        *title;
    HMENU       hmenu;
    HMENU       menu;
    bool        is_rc;
    RECT        rect;

    if( einfo == NULL ) {
        return( FALSE );
    }

    x = CW_USEDEFAULT;
    y = CW_USEDEFAULT;
    width = appWidth;
    height = appHeight;

    if( einfo->info->stand_alone ) {
        WGetScreenPosOption( &rect );
        if( !IsRectEmpty( &rect ) ) {
            x = rect.left;
            y = rect.top;
            width = appWidth;
            if( width < rect.right - rect.left )
                width = rect.right - rect.left;
            height = appHeight;
            if( height < rect.bottom - rect.top ) {
                height = rect.bottom - rect.top;
            }
        }
    }

    is_rc = FALSE;
    title = WCreateEditTitle( einfo );
    if( title == NULL ) {
        title = AllocRCString( W_ACCELAPPTITLE );
        is_rc = TRUE;
    }

    menu = (HMENU)NULL;
    if( einfo->info->stand_alone ) {
        menu = LoadMenu( inst, WMainSOMenuName );
    }

    einfo->win = CreateWindow( WMainClass, title, WS_OVERLAPPEDWINDOW,
                               x, y, width, height, einfo->info->parent,
                               menu, inst, einfo );

    if( title != NULL ) {
        if( is_rc ) {
            FreeRCString( title );
        } else {
            WRMemFree( title );
        }
    }

    if( einfo->win == (HWND)NULL ) {
        return( FALSE );
    }

    if( !WCreateRibbon( einfo ) ) {
        return( FALSE );
    }

    einfo->wsb = WCreateStatusLine( einfo->win, inst );
    if( einfo->wsb == NULL ) {
        return( FALSE );
    }

    if( !WCreateAccelEditWindow( einfo, inst ) ) {
        return( FALSE );
    }

    hmenu = GetMenu( einfo->win );
    if( hmenu != (HMENU)NULL ) {
        EnableMenuItem( hmenu, IDM_ACC_CUT, MF_GRAYED );
        EnableMenuItem( hmenu, IDM_ACC_COPY, MF_GRAYED );
    }

    if( WGetOption( WOptScreenMax ) ) {
        ShowWindow( einfo->win, SW_SHOWMAXIMIZED );
    } else {
        ShowWindow( einfo->win, SW_SHOWNORMAL );
    }
    UpdateWindow( einfo->win );

    WResizeWindows( einfo );

    SetFocus( einfo->edit_dlg );

    return( TRUE );
}

WAccelEditInfo *WGetCurrentEditInfo( void )
{
    return( WCurrEditInfo );
}

void WSetCurrentEditInfo( WAccelEditInfo *einfo )
{
    WCurrEditInfo = einfo;
}

HMENU WGetMenuHandle( WAccelEditInfo *einfo )
{
    if( einfo == NULL ) {
        einfo = WGetCurrentEditInfo();
    }

    if( einfo != NULL && einfo->win != NULL ) {
        return( GetMenu( einfo->win ) );
    }

    return( NULL );
}

static void handleSymbols( WAccelEditInfo *einfo )
{
    char        *text;

    if( !WEditSymbols( einfo->win, &einfo->info->symbol_table,
                       WGetEditInstance(), WAccHelpRoutine ) ) {
        return;
    }

    WResolveAllEntrySymIDs( einfo );

    text = WGetStrFromEdit( GetDlgItem( einfo->edit_dlg, IDM_ACCEDCMDID ), NULL );
    WRAddSymbolsToComboBox( einfo->info->symbol_table, einfo->edit_dlg,
                            IDM_ACCEDCMDID, WR_HASHENTRY_ALL );
    if( text != NULL ) {
        WSetEditWithStr( GetDlgItem( einfo->edit_dlg, IDM_ACCEDCMDID ), text );
        WRMemFree( text );
    }

    WHandleSelChange( einfo );
}

static void handleLoadSymbols( WAccelEditInfo *einfo )
{
    char        *file;

    file = WLoadSymbols( &einfo->info->symbol_table, einfo->info->symbol_file,
                         einfo->win, TRUE );
    if( file == NULL ) {
        return;
    }

    if( einfo->info->symbol_file != NULL ) {
        WRMemFree( einfo->info->symbol_file );
    }
    einfo->info->symbol_file = file;

    // lookup the id associated with the symbol for all entries
    WResolveAllEntrySymIDs( einfo );

    // look for the symbol matching the id for all entries
    WResolveAllEntrySymbols( einfo );

    WInitEditWindowListBox( einfo );

    if( einfo->current_pos != -1 ) {
        SendDlgItemMessage( einfo->edit_dlg, IDM_ACCEDLIST,
                            LB_SETCURSEL, einfo->current_pos, 0 );
    }

    WRAddSymbolsToComboBox( einfo->info->symbol_table, einfo->edit_dlg,
                            IDM_ACCEDCMDID, WR_HASHENTRY_ALL );

    einfo->info->modified = true;

    WDoHandleSelChange( einfo, FALSE, TRUE );
}

void setLastMenuSelect( WAccelEditInfo *einfo, WPARAM wParam, LPARAM lParam )
{
    if( MENU_CLOSED( wParam, lParam ) ) {
        einfo->last_menu_select = -1;
    } else {
        if( GET_WM_MENUSELECT_FLAGS( wParam, lParam ) & (MF_SYSMENU | MF_SEPARATOR | MF_POPUP) ) {
            einfo->last_menu_select = -1;
        } else {
            einfo->last_menu_select = GET_WM_MENUSELECT_ITEM( wParam, lParam );
        }
    }
}

WINEXPORT LRESULT CALLBACK WMainWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    HMENU           menu;
#if 0
    HWND            win;
#endif
    LRESULT         ret;
    bool            pass_to_def;
    WAccelEditInfo  *einfo;
    WORD            wp;
    MINMAXINFO      *minmax;
    about_info      ai;

    pass_to_def = TRUE;
    ret = FALSE;
    einfo = (WAccelEditInfo *)GET_WNDLONGPTR( hWnd, 0 );
    WSetCurrentEditInfo( einfo );

    if( einfo != NULL && einfo->getting_key ) {
        if( WGetKeyPressProc( einfo, message, wParam, lParam ) ) {
            einfo->getting_key = FALSE;
            DestroyWindow( einfo->key_info.text_win );
            ReleaseCapture();
            WHandleChange( einfo );
            pass_to_def = FALSE;
        }
    }

    switch( message ) {
    case WM_ACTIVATE:
        if( GET_WM_ACTIVATE_FACTIVE( wParam, lParam ) &&
            !GET_WM_ACTIVATE_FMINIMIZED( wParam, lParam ) &&
            einfo != NULL && einfo->edit_dlg != (HWND)NULL ) {
            SetFocus( einfo->edit_dlg );
            pass_to_def = FALSE;
        }
        break;

    case WM_INITMENU:
        if( wParam == (WPARAM)GetMenu( hWnd ) ) {
            // set the cut and copy menu items
            if( SendDlgItemMessage( einfo->edit_dlg, IDM_ACCEDLIST, LB_GETCURSEL, 0, 0 ) != LB_ERR ) {
                EnableMenuItem( (HMENU)wParam, IDM_ACC_CUT, MF_ENABLED );
                EnableMenuItem( (HMENU)wParam, IDM_ACC_COPY, MF_ENABLED );
            } else {
                EnableMenuItem( (HMENU)wParam, IDM_ACC_CUT, MF_GRAYED );
                EnableMenuItem( (HMENU)wParam, IDM_ACC_COPY, MF_GRAYED );
            }
            // set the paste menu item
            if( OpenClipboard( hWnd ) ) {
                if( //IsClipboardFormatAvailable( WClipbdFormat ) ||
                    IsClipboardFormatAvailable( WItemClipbdFormat ) ) {
                    EnableMenuItem( (HMENU)wParam, IDM_ACC_PASTE, MF_ENABLED );
                } else {
                    EnableMenuItem( (HMENU)wParam, IDM_ACC_PASTE, MF_GRAYED );
                }
                CloseClipboard();
            }
        }
        break;

    case WM_CREATE:
        einfo = ((CREATESTRUCT *)lParam)->lpCreateParams;
        SET_WNDLONGPTR( hWnd, 0, (LONG_PTR)einfo );
        break;

    case WM_MENUSELECT:
        if( einfo != NULL ) {
            menu = WGetMenuHandle( einfo );
            WHandleMenuSelect( einfo->wsb, menu, wParam, lParam );
            setLastMenuSelect( einfo, wParam, lParam );
        }
        break;

    case WM_GETMINMAXINFO:
        minmax = (MINMAXINFO *)lParam;
        minmax->ptMinTrackSize.x = appWidth;
        minmax->ptMinTrackSize.y = appHeight;
        break;

    case WM_MOVE:
        if( einfo != NULL ) {
            if( IsZoomed( hWnd ) ) {
                WSetOption( WOptScreenMax, TRUE );
            } else if( !IsIconic( hWnd ) ) {
                WUpdateScreenPosOpt( hWnd );
                WSetOption( WOptScreenMax, FALSE );
            }
        }
        break;

    case WM_SIZE:
        if( einfo != NULL ) {
            if( wParam == SIZE_MAXIMIZED ) {
                WSetOption( WOptScreenMax, TRUE );
            } else if( wParam != SIZE_MINIMIZED ) {
                WUpdateScreenPosOpt( hWnd );
                WSetOption( WOptScreenMax, FALSE );
            }
            WResizeWindows( einfo );
        }
        break;

#if 0
    case WM_ACTIVATE:
        if( GET_WM_ACTIVATE_FACTIVE( wParam, lParam ) != WA_INACTIVE ) {
            einfo = (WAccelEditInfo *)GET_WNDLONGPTR( hWnd, 0 );
            if( einfo != NULL && einfo->edit_dlg != (HWND)NULL ) {
                SetFocus( einfo->edit_dlg );
            }
            WSetCurrentEditInfo( einfo );
        } else {
            WSetCurrentEditInfo( NULL );
        }
        break;
#endif

    case WM_COMMAND:
        wp = LOWORD( wParam );
        switch( wp ) {
        case IDM_ACC_CLEAR:
            WHandleClear( einfo );
            pass_to_def = FALSE;
            break;

        case IDM_ACC_UPDATE:
            SendMessage( einfo->info->parent, ACCEL_PLEASE_SAVEME, 0, (LPARAM)einfo->hndl );
            pass_to_def = FALSE;
            break;

        case IDM_ACC_OPEN:
            pass_to_def = FALSE;
            if( einfo->info->modified ) {
                ret = WQuerySave( einfo, FALSE );
                if( !ret ) {
                    break;
                }
            }
            ret = SendMessage( einfo->info->parent, ACCEL_PLEASE_OPENME, 0,
                               (LPARAM)einfo->hndl );
            ret = FALSE;
            break;

        case IDM_ACC_SAVE:
            WSaveObject( einfo, FALSE, FALSE );
            pass_to_def = FALSE;
            break;

        case IDM_ACC_SAVEAS:
            WSaveObject( einfo, TRUE, FALSE );
            pass_to_def = FALSE;
            break;

        case IDM_ACC_SAVEINTO:
            WSaveObject( einfo, TRUE, TRUE );
            pass_to_def = FALSE;
            break;

        case IDM_ACC_EXIT:
            /* clean up before we exit */
            PostMessage( einfo->win, WM_CLOSE, 0, 0 );
            break;

        case IDM_ACC_PASTE:
            WPasteAccelItem( einfo );
            pass_to_def = FALSE;
            break;

        case IDM_ACC_COPY:
        case IDM_ACC_CUT:
            WClipAccelItem( einfo, wp == IDM_ACC_CUT );
            pass_to_def = FALSE;
            break;

        case IDM_ACC_DELETE:
            WDeleteAccelEntry( einfo );
            pass_to_def = FALSE;
            break;

        case IDM_ACC_NEWITEM:
            WInsertAccelEntry( einfo );
            pass_to_def = FALSE;
            break;

        case IDM_ACC_KEYVALUE:
            WSetStatusByID( einfo->wsb, W_GETTINGKEYS, -1 );
            WHandleGetKeyValue( einfo, einfo->last_menu_select == IDM_ACC_KEYVALUE );
            WSetStatusReadyText( einfo->wsb );
            pass_to_def = FALSE;
            break;

        case IDM_ACC_SYMBOLS:
            handleSymbols( einfo );
            pass_to_def = FALSE;
            break;

        case IDM_ACC_LOAD_SYMBOLS:
            handleLoadSymbols( einfo );
            pass_to_def = FALSE;
            break;

        case IDM_ACC_SHOWRIBBON:
            menu = WGetMenuHandle( einfo );
            WShowRibbon( einfo, menu );
            pass_to_def = FALSE;
            break;

        case IDM_ACC_MEM_FLAGS:
            WSetStatusByID( einfo->wsb, W_CHANGEACCELMEMFLAGS, -1 );
            einfo->info->modified |= WChangeMemFlags( einfo->win, &einfo->info->MemFlags,
                                                      einfo->info->res_name,
                                                      WGetEditInstance(),
                                                      WAccHelpRoutine );
            pass_to_def = FALSE;
            WSetStatusReadyText( einfo->wsb );
            break;

        case IDM_ACC_RENAME:
            WHandleRename( einfo );
            pass_to_def = FALSE;
            break;

        case IDM_HELP:
            WAccHelpRoutine();
            pass_to_def = FALSE;
            break;

        case IDM_HELP_SEARCH:
            WAccHelpSearchRoutine();
            pass_to_def = FALSE;
            break;

        case IDM_HELP_ON_HELP:
            WAccHelpOnHelpRoutine();
            pass_to_def = FALSE;
            break;

        case IDM_ACC_ABOUT:
            ai.owner = hWnd;
            ai.inst = WGetEditInstance();
            ai.name = AllocRCString( W_ABOUT_NAME );
            ai.version = AllocRCString( W_ABOUT_VERSION );
            ai.title = AllocRCString( W_ABOUT_TITLE );
            DoAbout( &ai );
            FreeRCString( ai.name );
            FreeRCString( ai.version );
            FreeRCString( ai.title );
            pass_to_def = FALSE;
            break;
        }
        break;

    case WM_DESTROY:
        WWinHelp( hWnd, "resacc.hlp", HELP_QUIT, 0 );
        WCleanup( einfo );
        break;

    case WM_CLOSE:
        ret = TRUE;
        pass_to_def = WHandleWM_CLOSE( einfo, wParam != 0 );
        wParam = 0;
        break;
    }

    if( pass_to_def ) {
        ret = DefWindowProc( hWnd, message, wParam, lParam );
    }

    return( ret );
}

bool WQuerySave( WAccelEditInfo *einfo, bool force_exit )
{
    return( WQuerySaveRes( einfo, force_exit ) && WQuerySaveSym( einfo, force_exit ) );
}

bool WQuerySaveRes( WAccelEditInfo *einfo, bool force_exit )
{
    int         ret;
    UINT        style;
    char        *title;
    char        *text;

    if( einfo != NULL && einfo->info->modified ) {
        ret = IDYES;
        if( einfo->info->stand_alone ) {
            if( force_exit ) {
                style = MB_YESNO | MB_APPLMODAL | MB_ICONEXCLAMATION;
            } else {
                style = MB_YESNOCANCEL | MB_APPLMODAL | MB_ICONEXCLAMATION;
            }
            title = WCreateEditTitle( einfo );
            text = AllocRCString( W_UPDATEMODIFIEDACCEL );
            ret = MessageBox( einfo->edit_dlg, text, title, style );
            if( text != NULL ) {
                FreeRCString( text );
            }
            if( title != NULL ) {
                WRMemFree( title );
            }
        }
        if( ret == IDYES ) {
            if( einfo->info->stand_alone ) {
                return( WSaveObject( einfo, FALSE, FALSE ) );
            } else {
                SendMessage( einfo->info->parent, ACCEL_PLEASE_SAVEME, 0,
                             (LPARAM)einfo->hndl );
            }
        } else if( ret == IDCANCEL ) {
            return( FALSE );
        }
    }

    return( TRUE );
}

bool WQuerySaveSym( WAccelEditInfo *einfo, bool force_exit )
{
    int         ret;
    UINT        style;
    char        *title;
    char        *text;

    if( einfo == NULL || !einfo->info->stand_alone ) {
        return( TRUE );
    }

    if( !WRIsHashTableDirty( einfo->info->symbol_table ) ) {
        return( TRUE );
    }

    if( force_exit ) {
        style = MB_YESNO | MB_APPLMODAL | MB_ICONEXCLAMATION;
    } else {
        style = MB_YESNOCANCEL | MB_APPLMODAL | MB_ICONEXCLAMATION;
    }

    title = WCreateEditTitle( einfo );
    text = AllocRCString( W_UPDATEMODIFIEDSYM );
    ret = MessageBox( einfo->edit_dlg, text, title, style );
    if( text != NULL ) {
        FreeRCString( text );
    }
    if( title != NULL ) {
        WRMemFree( title );
    }

    if( ret == IDYES ) {
        if( einfo->info->symbol_file == NULL ) {
            char        *fname;
            if( einfo->file_name == NULL ) {
                fname = einfo->info->file_name;
            } else {
                fname = einfo->file_name;
            }
            einfo->info->symbol_file = WCreateSymName( fname );
        }
        return( WSaveSymbols( einfo, einfo->info->symbol_table,
                              &einfo->info->symbol_file, FALSE ) );
    } else if( ret == IDCANCEL ) {
        return( FALSE );
    }

    return( TRUE );
}

bool WHandleWM_CLOSE( WAccelEditInfo *einfo, bool force_exit )
{
    bool        ret;

    ret = TRUE;

    if( einfo != NULL ) {
        if( einfo->info->modified ||
            WRIsHashTableDirty( einfo->info->symbol_table ) ) {
            ret = WQuerySave( einfo, force_exit );
        }
        if( ret ) {
            SendMessage( einfo->info->parent, ACCEL_I_HAVE_CLOSED, 0,
                         (LPARAM)einfo->hndl );
            WUnRegisterEditSession( WGetEditSessionHandle( einfo ) );
        }
    }

    return( ret );
}

void WHandleRename( WAccelEditInfo *einfo )
{
    if( einfo != NULL ) {
        WSetStatusByID( einfo->wsb, W_RENAMINGACCEL, -1 );
        einfo->info->modified |= WRenameResource( einfo->win, &einfo->info->res_name,
                                                  WAccHelpRoutine );
        WSetEditWinResName( einfo );
        WSetStatusReadyText( einfo->wsb );
    }
}

bool WQueryClearRes( WAccelEditInfo *einfo )
{
    int         ret;
    UINT        style;
    char        *title;
    char        *text;

    if( einfo != NULL ) {
        style = MB_YESNO | MB_APPLMODAL | MB_ICONEXCLAMATION;
        text = AllocRCString( W_ACCELCLEARWARNING );
        title = AllocRCString( W_ACCELCLEARTITLE );
        ret = MessageBox( einfo->edit_dlg, text, title, style );
        if( text != NULL ) {
            FreeRCString( text );
        }
        if( title != NULL ) {
            FreeRCString( title );
        }
        if( ret == IDYES ) {
            return( TRUE );
        }
    }

    return( FALSE );
}

void WHandleClear( WAccelEditInfo *einfo )
{
    if( einfo->tbl != NULL && einfo->tbl->num != 0 ) {
        if( WQueryClearRes( einfo ) ) {
            WResetEditWindow( einfo );
            SendDlgItemMessage( einfo->edit_dlg, IDM_ACCEDLIST, LB_RESETCONTENT, 0, 0 );
            WFreeAccelTableEntries( einfo->tbl->first_entry );
            einfo->tbl->first_entry = NULL;
            einfo->tbl->num = 0;
            einfo->current_entry = NULL;
            einfo->current_pos = -1;
            einfo->getting_key = FALSE;
            if( einfo->info->stand_alone ) {
                if( einfo->file_name != NULL ) {
                    WRMemFree( einfo->file_name );
                    einfo->file_name = NULL;
                    WSetEditTitle( einfo );
                }
                if( einfo->info->symbol_table != NULL ) {
                    WRFreeHashTable( einfo->info->symbol_table );
                    einfo->info->symbol_table = WRInitHashTable();
                }
            }
            einfo->info->modified = true;
            SetFocus( einfo->edit_dlg );
            WSetStatusByID( einfo->wsb, W_ACCELCLEARMSG, -1 );
        }
    }
}

void WUpdateScreenPosOpt( HWND win )
{
    RECT        rect;

    GetWindowRect( win, &rect );

    WSetScreenPosOption( &rect );
}

void WResizeWindows( WAccelEditInfo *einfo )
{
    RECT  rect;

    if( einfo == NULL ) {
        einfo = WGetCurrentEditInfo();
    }

    if( einfo != NULL && einfo->win != NULL ) {
        GetClientRect( einfo->win, &rect );
        WResizeAccelEditWindow( einfo, &rect );
        WResizeStatusWindows( einfo->wsb, &rect );
        WResizeRibbon( einfo, &rect );
    }
}

bool WCleanup( WAccelEditInfo *einfo )
{
    HWND        owner;
    bool        ok;

    ok = (einfo != NULL);

    if( ok ) {
        owner = (HWND)NULL;
        if( !einfo->info->stand_alone ) {
            owner = GetWindow( einfo->win, GW_OWNER );
        }
        einfo->getting_key = FALSE;
        einfo->win = (HWND)NULL;
        WFreeAccelEInfo( einfo );
        if( owner != (HWND)NULL ) {
            BringWindowToTop( owner );
        }
    }

    return( ok );
}

WINEXPORT void CALLBACK WAccHelpRoutine( void )
{
    WAccelEditInfo      *einfo;

    einfo = WGetCurrentEditInfo();
    if( einfo != NULL ) {
        if( !WHtmlHelp( einfo->win, "resacc.chm", HELP_CONTENTS, 0 ) ) {
            WWinHelp( einfo->win, "resacc.hlp", HELP_CONTENTS, 0 );
        }
    }
}

WINEXPORT void CALLBACK WAccHelpSearchRoutine( void )
{
    WAccelEditInfo      *einfo;

    einfo = WGetCurrentEditInfo();
    if( einfo != NULL ) {
        if( !WHtmlHelp( einfo->win, "resacc.chm", HELP_PARTIALKEY, (HELP_DATA)"" ) ) {
            WWinHelp( einfo->win, "resacc.hlp", HELP_PARTIALKEY, (HELP_DATA)"" );
        }
    }
}

WINEXPORT void CALLBACK WAccHelpOnHelpRoutine( void )
{
    WAccelEditInfo      *einfo;

    einfo = WGetCurrentEditInfo();
    if( einfo != NULL ) {
        WWinHelp( einfo->win, "winhelp.hlp", HELP_HELPONHELP, 0 );
    }
}
