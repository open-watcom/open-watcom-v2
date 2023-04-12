/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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


#include "wreglbl.h"
#include <ddeml.h>
#include "wio.h"
#include "waccel.h"
#include "wmenu.h"
#include "wstring.h"
#include "wrehints.h"
#include "wrememf.h"
#include "wremsg.h"
#include "wrenames.h"
#include "wreopts.h"
#include "wreres.h"
#include "wreribbn.h"
#include "wrestat.h"
#include "wresvobj.h"
#include "wresvmlt.h"
#include "wrerenam.h"
#include "wre.rh"
#include "wrectl3d.h"
#include "wredde.h"
#include "wregcres.h"
#include "wredel.h"
#include "wreaccel.h"
#include "wremenu.h"
#include "wrestr.h"
#include "wredlg.h"
#include "wreimg.h"
#include "wreclip.h"
#include "wresym.h"
#include "wregetfn.h"
#include "wreftype.h"
#include "wremain.h"
#include "wresall.h"
#include "wrdll.h"
#include "wrdmsg.h"
#include "wrbitmap.h"
#include "wwinhelp.h"
#include "jdlg.h"
#include "aboutdlg.h"
#include "ldstr.h"
#include "wresdefn.h"
#include "wclbproc.h"

#include "clibint.h"


/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define ABOUT_TIMER     666
#define ABOUT_TIMEOUT   2000
#define CREATE_NEW_FLAG "n"
#define NO_IFACE_FLAG   "nointerface"

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT LRESULT CALLBACK WREMainWndProc( HWND, UINT, WPARAM, LPARAM );
WINEXPORT INT_PTR CALLBACK WRESplashDlgProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static bool     WREInit( HINSTANCE );
static bool     WREInitInst( HINSTANCE );
static bool     WREWasAcceleratorHandled( MSG * );
static HWND     WRECreateMDIClientWindow( HWND, HINSTANCE );
static LRESULT  WREHandleMDIArrangeEvents( WORD );
static void     WREUpdateScreenPosOpt( void );
static bool     WRECleanup( bool );
static bool     WREProcessArgs( char **, int );
static void     WREDisplaySplashScreen( HINSTANCE, HWND, UINT );
static void     WREHideSessions( bool show );

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

#ifdef __NT__
typedef HANDLE (WINAPI *PFNLI)( HINSTANCE, LPCSTR, UINT, int, int, UINT );
#endif

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static HINSTANCE WREInst;
static HACCEL    WREAccel;
static HMENU     WREMenu;
static bool      WREIsMinimized      = false;
static HWND      WREMainWin          = NULL;
static HWND      WREMDIWin           = NULL;
static bool      WRECleanupStarted   = false;
static bool      WREFatalExit        = false;
static char      WREMainClass[]      = "WREMainClass";

bool WRECreateNewFiles  = false;
bool WRENoInterface     = false;

bool WRERemoveResource( WREResInfo * );
bool WREIsEditWindowDialogMessage( MSG *msg );

static void peekArgs( char **argv, int argc )
{
    int  i;

    for( i = 1; i < argc; i++ ) {
        if( ( argv[i][0] == '/' || argv[i][0] == '-' ) && stricmp( argv[i] + 1, CREATE_NEW_FLAG ) == 0 ) {
            //WRECreateNewFiles = true;
        } else if( ( argv[i][0] == '/' || argv[i][0] == '-' ) && stricmp( argv[i] + 1, NO_IFACE_FLAG ) == 0 ) {
            WRENoInterface = true;
        }
    }
}

static void startEditors( void )
{
    WRFileType  ftype;
    WREResInfo  *res_info;
    bool        editor_started;
    int         num_types;
    uint_16     type;

    if( WREGetNumRes() != 1 ) {
        if( WRENoInterface ) {
            WREDisplayErrorMsg( WRE_INVALIDINPUTFILE );
            PostMessage( WREMainWin, WM_CLOSE, 0, 0 );
        }
        return;
    }

    editor_started = false;
    num_types = 0;
    res_info = WREGetCurrentRes();
    ftype = res_info->info->file_type;
    if( res_info->info->file_name == NULL ) {
        ftype = WRIdentifyFile( res_info->info->save_name );
    } else {
        num_types = res_info->info->dir->NumTypes;
    }

    if( ftype != WR_WIN_RC_STR && ftype != WR_WIN_RC_MENU && ftype != WR_WIN_RC_ACCEL ) {
        if( WRENoInterface ) {
            if( !editor_started ) {
                WREDisplayErrorMsg( WRE_INVALIDINPUTFILE );
                PostMessage( WREMainWin, WM_CLOSE, 0, 0 );
            }
        }
        return;
    }

    if( num_types == 1 || num_types == 2 ) {
        type = 0;
        if( ftype == WR_WIN_RC_STR ) {
            type = RESOURCE2INT( RT_STRING );
        } else if( ftype == WR_WIN_RC_MENU ) {
            type = RESOURCE2INT( RT_MENU );
        } else if( ftype == WR_WIN_RC_ACCEL ) {
            type = RESOURCE2INT( RT_ACCELERATOR );
        }
        if( type != 0 && WREFindTypeNode( res_info->info->dir, type, NULL ) ) {
            if( WRESetResNamesFromType( res_info, type, false, NULL, 0 ) ) {
                editor_started = WREHandleResEdit();
            }
        }
    } else if( num_types == 0 ) {
        if( ftype == WR_WIN_RC_STR ) {
            editor_started = WRENewStringResource();
        } else if( ftype == WR_WIN_RC_MENU ) {
            editor_started = WRENewMenuResource();
        } else if( ftype == WR_WIN_RC_ACCEL ) {
            editor_started = WRENewAccelResource();
        }
    }

    if( WRENoInterface ) {
        if( !editor_started ) {
            WREDisplayErrorMsg( WRE_INVALIDINPUTFILE );
            PostMessage( WREMainWin, WM_CLOSE, 0, 0 );
        }
    }
}

bool WREIsResInfoWinMsg( LPMSG pmsg );

int PASCAL WinMain( HINSTANCE hinstCurrent, HINSTANCE hinstPrevious,
                    LPSTR lpszCmdLine, int nCmdShow )
{
    MSG         msg;
    bool        ok;

#if defined( __NT__ ) && !defined( __WATCOMC__ )
    _argc = __argc;
    _argv = __argv;
#endif

    /* unused parameters */ (void)lpszCmdLine; (void)nCmdShow;
#ifdef __NT__
    /* unused parameters */ (void)hinstPrevious;
#endif

    WRInit();
    WAccelInit();
    WMenuInit();
    WStringInit();
    SetInstance( hinstCurrent );

    /* store the handle to this instance of WRE in a static variable */
    WREInst = hinstCurrent;

    peekArgs( _argv, _argc );

    /* is this the first instance of the application? */
#ifndef __NT__
    if( hinstPrevious == NULL ) {
#endif
        /* if so call the routine to initialize the application */
        if( !WREInit( hinstCurrent ) ) {
            return( FALSE );
        }
#ifndef __NT__
    }
#endif

    if( !WREInitInst( hinstCurrent ) ) {
        WREDisplayErrorMsg( WRE_INITFAILED );
        return( FALSE );
    }

    if( !WREDDEStart( hinstCurrent ) ) {
        WREDisplayErrorMsg( WRE_DDEINITFAILED );
        return( FALSE );
    }

    ok = WREProcessArgs( _argv, _argc );

    startEditors();

    // create a new empty res if none have been created at this point
    if( ok && WREGetNumRes() == 0 ) {
        WRECreateNewResource( NULL );
    }

    /* create the message loop */
    while( GetMessage( &msg, (HWND)NULL, 0, 0 ) ) {
        if( !WREIsEditWindowDialogMessage( &msg ) &&
            !WREWasAcceleratorHandled( &msg ) &&
            !WREIsResInfoWinMsg( &msg ) &&
            !WRIsWRDialogMsg( &msg ) ) {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
    }

    WREDDEEnd();
    WStringFini();
    WMenuFini();
    WAccelFini();
    WRFini();

    return( msg.wParam );
}

/* Function to initialize the first instance of WRE */
bool WREInit( HINSTANCE app_inst )
{
    WNDCLASS wc;

    /* fill in the WINDOW CLASS structure for the main window */
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = WREMainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = app_inst;
    wc.hIcon = LoadIcon( app_inst, "APPLICON" );
    wc.hCursor = LoadCursor( (HINSTANCE)NULL, IDC_ARROW );
    wc.hbrBackground = NULL;
    wc.lpszMenuName = "WREMenu";
    wc.lpszClassName = WREMainClass;

    /* register the main window class */
    if( !RegisterClass( &wc ) ) {
        return( false );
    }

    /* register the res MDI window class */
    if( !WRERegisterResClass( app_inst ) ) {
        return( false );
    }

    return( true );
}

/* Function to initialize all instances of WRE */
bool WREInitInst( HINSTANCE app_inst )
{
    RECT        r, screen, t;
    char        *title;
    DWORD       style;

    if( !WRERegisterClipFormats( app_inst ) ) {
        WREDisplayErrorMsg( WRE_NOREGISTERCLIPFORMATS );
        return( false );
    }

    if( !JDialogInit() ) {
        return( false );
    }

    WRECtl3DInit( app_inst );
    WREInitOpts();
    WREInitRibbon( app_inst );
    WREInitTypeNames();
    WREInitTotalText();

    if( !WREInitResources( app_inst ) ) {
        return( false );
    }

    /* load the accelerator table */
    WREAccel = LoadAccelerators( app_inst, "WREAccelTable" );

    /* if the window could not be created return false */
    if( WREAccel == NULL ) {
        WREDisplayErrorMsg( WRE_NOLOADACCELTABLE );
        return( false );
    }

    WREGetScreenPosOption( &r );
    GetWindowRect( GetDesktopWindow(), &screen );
    IntersectRect( &t, &screen, &r );

    title = AllocRCString( WRE_APPNAME );

    /* attempt to create the main application window */
    style = WS_OVERLAPPEDWINDOW;
    if( IsRectEmpty( &r ) ) {
        WREMainWin = CreateWindow( WREMainClass, title, style,
                                   CW_USEDEFAULT, CW_USEDEFAULT,
                                   CW_USEDEFAULT, CW_USEDEFAULT,
                                   (HWND)NULL, (HMENU)NULL, app_inst, NULL );
    } else {
        WREMainWin = CreateWindow( WREMainClass, title, style,
                                   r.left, r.top, r.right - r.left, r.bottom - r.top,
                                   (HWND)NULL, (HMENU)NULL, app_inst, NULL );
    }

    if( title != NULL ) {
        FreeRCString( title );
    }

    /* if the window could not be created return false */
    if( WREMainWin == NULL ) {
        WREDisplayErrorMsg( WRE_NOCREATEAPPWINDOW );
        return( false );
    }

    WREMDIWin = WRECreateMDIClientWindow( WREMainWin, app_inst );

    /* attempt to create the main application ribbon */
    if( !WRECreateRibbon( WREMainWin ) ) {
        WREDisplayErrorMsg( WRE_NOCREATETOOLRIBBON );
        return( false );
    }

    if( !WRECreateStatusLine( WREMainWin, app_inst ) ) {
        WREDisplayErrorMsg( WRE_NOCREATESTATUSLINE );
        return( false );
    }

    WREMenu = GetMenu( WREMainWin );
    if( WREMenu != (HMENU)NULL ) {
        EnableMenuItem( WREMenu, IDM_CUT, MF_GRAYED );
        EnableMenuItem( WREMenu, IDM_COPY, MF_GRAYED );
        EnableMenuItem( WREMenu, IDM_OPTIONS, MF_GRAYED );
    }

    WREEnableMenus( FALSE );

    if( !WREInitHints() ) {
        WREDisplayErrorMsg( WRE_NOINITHINTS );
        return( false );
    }

    /* if the window was created Show and Update it */
    if( !WRENoInterface ) {
        if( WREGetOption( WREOptScreenMax ) ) {
            ShowWindow( WREMainWin, SW_SHOWMAXIMIZED );
        } else {
            ShowWindow( WREMainWin, SW_SHOWNORMAL );
        }
        UpdateWindow( WREMainWin );

        WREDisplaySplashScreen( WREInst, WREMainWin, 1250 );
    }

    return( true );
}

bool WREIsEditWindowDialogMessage( MSG *msg )
{
    bool    ok;

    ok = WAccelIsDlgMsg( msg );
    ok |= WMenuIsDlgMsg( msg );
    ok |= WStringIsDlgMsg( msg );

    return( ok );
}

bool WREWasAcceleratorHandled( MSG *msg )
{
    if( TranslateMDISysAccel( WREMDIWin, msg ) == 0 && TranslateAccelerator( WREMainWin, WREAccel, msg ) == 0 ) {
        return( false );
    }
    return( true );
}

HWND WRECreateMDIClientWindow( HWND win, HINSTANCE app_inst )
{
    CLIENTCREATESTRUCT ccs;
    RECT               rect;
    HWND               client;
    int                ribbon_depth;
    int                stat_depth;

    ribbon_depth = WREGetRibbonHeight();

    stat_depth = WREGetStatusDepth();

    GetClientRect( win, &rect );

    ccs.hWindowMenu = GetSubMenu( GetMenu( win ), 3 );
    ccs.idFirstChild = WRE_MDI_FIRST;

    /* attempt to create the main application edit window */
    client = CreateWindow( "MDIClient", "",
                           WS_CLIPCHILDREN | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL,
                           0, ribbon_depth,
                           rect.right - rect.left,
                           rect.bottom - (rect.top + stat_depth + ribbon_depth),
                           win, (HMENU)NULL, app_inst, &ccs );

    /* if the window could not be created return FALSE */
    if( client == NULL ) {
        WREDisplayErrorMsg( WRE_MOCREATEMDICLIENT );
        return( 0 );
    }

    return( client );
}

void WREEnableMenus( bool enable )
{
    HMENU       hsubmenu;
    int         flag;

    if( WREMenu != (HMENU)NULL ) {
        flag = MF_GRAYED;
        if( enable ) {
            flag = MF_ENABLED;
        }
        EnableMenuItem( WREMenu, IDM_SAVE, flag );
        EnableMenuItem( WREMenu, IDM_SAVEAS, flag );
        EnableMenuItem( WREMenu, IDM_RES_EDIT, flag );
        EnableMenuItem( WREMenu, IDM_RES_SAVEAS, flag );
        EnableMenuItem( WREMenu, IDM_RES_SAVE_INTO, flag );
        EnableMenuItem( WREMenu, IDM_RES_RENAME, flag );
        EnableMenuItem( WREMenu, IDM_RES_MEM_FLAGS, flag );
        hsubmenu = GetSubMenu( WREMenu, 2 );
        if( hsubmenu != (HMENU)NULL ) {
            EnableMenuItem( hsubmenu, 2, MF_BYPOSITION | flag );
        }
    }
}

HMENU WREGetMenuHandle( void )
{
    return( WREMenu );
}

HWND WREGetMDIWindowHandle( void )
{
    return( WREMDIWin );
}

HWND WREGetMainWindowHandle( void )
{
    return( WREMainWin );
}

HINSTANCE WREGetAppInstance( void )
{
    return( WREInst );
}

LRESULT CALLBACK WREMainWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    HMENU       hmenu;
    LRESULT     ret;
    bool        pass_to_def;
    WREResInfo  *res_info;
    WORD        wp;
    about_info  ai;

    if( WRECleanupStarted && message != WM_CLOSE ) {
        if( message == WM_DESTROY ) {
            PostQuitMessage( 0 );
        }
        return( DefFrameProc( hWnd, WREMDIWin, message, wParam, lParam ) );
    }

    pass_to_def = true;
    ret = FALSE;
    res_info = WREGetCurrentRes();
    hmenu = WREGetMenuHandle();

    switch( message ) {
    case WM_DESTROYCLIPBOARD:
        WRForgetBitmapName();
        break;

    case WM_INITMENU:
        if( wParam == (WPARAM)GetMenu( hWnd ) ) {
            WRESetCopyMenuItem( hWnd );
            WRESetPasteMenuItem( hWnd );
        }
        break;

    case WM_MENUSELECT:
        WREHandleMenuSelect( wParam, lParam );
        break;

    case WM_MOVE:
        if( IsZoomed( hWnd ) ) {
            WRESetOption( WREOptScreenMax, TRUE );
        } else if( !IsIconic( hWnd ) ) {
            WREUpdateScreenPosOpt();
            WRESetOption( WREOptScreenMax, FALSE );
        }
        break;

    case WM_SIZE:
        if( wParam == SIZE_MAXIMIZED ) {
            WRESetOption( WREOptScreenMax, TRUE );
        } else if( wParam == SIZE_MINIMIZED ) {
            WREHideSessions( false );
            WREIsMinimized = true;
        } else {
            WREUpdateScreenPosOpt();
            WRESetOption( WREOptScreenMax, FALSE );
        }
        if( wParam != SIZE_MINIMIZED && WREIsMinimized ) {
            WREHideSessions( true );
            BringWindowToTop( hWnd );
            WREIsMinimized = false;
        }
        WREResizeWindows();
        pass_to_def = false;
        break;

    case ACCEL_I_HAVE_CLOSED:
        WREEndEditAccelResource( (WAccelHandle)lParam );
        if( WRENoInterface ) {
            WRERemoveResource( res_info );
        }
        pass_to_def = false;
        break;

    case ACCEL_PLEASE_SAVEME:
        WRESaveEditAccelResource( (WAccelHandle)lParam );
        pass_to_def = false;
        break;

    case MENU_I_HAVE_CLOSED:
        WREEndEditMenuResource( (WMenuHandle)lParam );
        if( WRENoInterface ) {
            WRERemoveResource( res_info );
        }
        pass_to_def = false;
        break;

    case MENU_PLEASE_SAVEME:
        WRESaveEditMenuResource( (WMenuHandle)lParam );
        pass_to_def = false;
        break;

    case STRING_I_HAVE_CLOSED:
        WREEndEditStringResource( (WStringHandle)lParam );
        if( WRENoInterface ) {
            WRERemoveResource( res_info );
        }
        pass_to_def = false;
        break;

    case STRING_PLEASE_SAVEME:
        WRESaveEditStringResource( (WStringHandle)lParam );
        pass_to_def = false;
        break;

    case ACCEL_PLEASE_OPENME:
    case MENU_PLEASE_OPENME:
    case STRING_PLEASE_OPENME:
        ret = PleaseOpenFile( message );
        pass_to_def = false;
        break;

    case WM_COMMAND:
        wp = LOWORD( wParam );
        switch( wp ) {
        case IDM_OPTIONS:
            pass_to_def = false;
            break;

        case IDM_LOAD_SYMBOLS:
            WRELoadResourceSymbols( res_info );
            pass_to_def = false;
            break;

        case IDM_EDIT_SYMBOLS:
            WREEditResourceSymbols( res_info );
            pass_to_def = false;
            break;

        case IDM_SAVE_SYMBOLS:
            WREResourceSaveSymbols( res_info );
            pass_to_def = false;
            break;

        case IDM_NEW:
            WRECreateNewResource( NULL );
            pass_to_def = false;
            break;

        case IDM_OPEN:
            WREOpenResource( NULL );
            pass_to_def = false;
            break;

        case IDM_SAVE:
            WRESaveResource( res_info, false );
            pass_to_def = false;
            break;

        case IDM_SAVEAS:
            WRESaveResource( res_info, true );
            pass_to_def = false;
            break;

        case IDM_DELETE:
            WREDeleteCurrResource( false );
            pass_to_def = false;
            break;

        case IDM_PASTE:
            WREPasteResource( hWnd );
            pass_to_def = false;
            break;

        case IDM_CUT:
            WREClipCurrentResource( hWnd, true );
            pass_to_def = false;
            break;

        case IDM_COPY:
            WREClipCurrentResource( hWnd, false );
            pass_to_def = false;
            break;

        case IDM_NEW_CURSOR:
            WRENewImageResource( CursorService, RESOURCE2INT( RT_GROUP_CURSOR ) );
            break;

        case IDM_NEW_BITMAP:
            WRENewImageResource( BitmapService, RESOURCE2INT( RT_BITMAP ) );
            break;

        case IDM_NEW_ICON:
            WRENewImageResource( IconService, RESOURCE2INT( RT_GROUP_ICON ) );
            break;

        case IDM_NEW_DIALOG:
            WRENewDialogResource();
            break;

        case IDM_NEW_FONT:
        case IDM_NEW_RCDATA:
            pass_to_def = false;
            break;

        case IDM_NEW_ACCELERATOR:
            WRENewAccelResource();
            pass_to_def = false;
            break;

        case IDM_NEW_MENU:
            WRENewMenuResource();
            pass_to_def = false;
            break;

        case IDM_NEW_STRING:
            WRENewStringResource();
            pass_to_def = false;
            break;

        case IDM_RES_SAVEAS:
        case IDM_RES_SAVE_INTO:
            if( res_info != NULL ) {
                if( res_info->current_type == RESOURCE2INT( RT_STRING ) ) {
                    SaveMultObjects( wp == IDM_RES_SAVE_INTO );
                } else {
                    SaveObject( wp == IDM_RES_SAVE_INTO );
                }
            }
            pass_to_def = false;
            break;

        case IDM_RES_EDIT:
            WREHandleResEdit();
            pass_to_def = false;
            break;

        case IDM_RES_MEM_FLAGS:
            WREChangeMemFlags();
            pass_to_def = false;
            break;

        case IDM_RES_RENAME:
            WRERenameResource();
            pass_to_def = false;
            break;

        case IDM_MDI_CASCADE:
        case IDM_MDI_TILEV:
        case IDM_MDI_TILEH:
        case IDM_MDI_ARRANGE:
            WREHandleMDIArrangeEvents( wp );
            pass_to_def = false;
            break;

        case IDM_EXIT:
            /* clean up before we exit */
            PostMessage( WREMainWin, WM_CLOSE, 0, 0 );
            break;

        case IDM_SHOW_RIBBON:
            WREShowRibbon( hmenu );
            pass_to_def = false;
            break;

        case IDM_HELP:
            WREHelpRoutine();
            pass_to_def = false;
            break;

        case IDM_HELP_SEARCH:
            WREHelpSearchRoutine();
            pass_to_def = false;
            break;

        case IDM_HELP_ON_HELP:
            WREHelpOnHelpRoutine();
            pass_to_def = false;
            break;

        case IDM_ABOUT:
            ai.owner = hWnd;
            ai.inst = WREInst;
            ai.name = AllocRCString( WRE_ABOUT_NAME );
            ai.version = AllocRCString( WRE_ABOUT_VERSION );
            ai.title = AllocRCString( WRE_ABOUT_TITLE );
            DoAbout( &ai );
            FreeRCString( ai.name );
            FreeRCString( ai.version );
            FreeRCString( ai.title );
            pass_to_def = false;
            break;
        }
        break;

    case WM_DESTROY:
        WWinHelp( WREMainWin, "resedt.hlp", HELP_QUIT, 0 );
        PostQuitMessage( 0 );
        break;

    case WRE_FATAL_EXIT:
        WREFatalExit = true;
        WREQueryKillApp( true );
        PostMessage( WREMainWin, WM_CLOSE, 0, 0 );
        break;

    case WM_ENDSESSION:
        if( !wParam ) {
            WREFatalExit = false;
        }
        break;

    case WM_QUERYENDSESSION:
        if( (ret = WREQueryKillApp( false )) != 0 ) {
            WREFatalExit = true;
        }
        pass_to_def = false;
        break;

    case WM_CLOSE:
        /* clean up before we exit */
        if( WRECleanupStarted ) {
            pass_to_def = false;
            break;
        }
        WRECleanupStarted = true;
        if( !WRECleanup( WREFatalExit ) ) {
            WRECleanupStarted = false;
            pass_to_def = false;
        }
    }

    if( pass_to_def ) {
        ret = DefFrameProc( hWnd, WREMDIWin, message, wParam, lParam );
    }
    return( ret );
}

void WREHideSessions( bool show )
{
    WREShowAllDialogSessions( show );
    WREShowAllImageSessions( show );
}

bool WREHandleResEdit( void )
{
    WRECurrentResInfo  curr;
    bool               ok;

    if( WREGetPendingService() != NoServicePending ) {
        WRESetStatusByID( 0, WRE_EDITSESSIONPENDING );
        return( true );
    }

    ok = WREGetCurrentResource( &curr );

    // correct ok if this the 'All Strings' entry
    if( !ok ) {
        ok = ( curr.info != NULL && curr.type != NULL &&
              curr.info->current_type == RESOURCE2INT( RT_STRING ) );
    }

    if( ok ) {
        if( curr.info->current_type == RESOURCE2INT( RT_ACCELERATOR ) ) {
            ok = WREEditAccelResource( &curr );
        } else if( curr.info->current_type == RESOURCE2INT( RT_MENU ) ) {
            ok = WREEditMenuResource( &curr );
        } else if( curr.info->current_type == RESOURCE2INT( RT_STRING ) ) {
            ok = WREEditStringResource( &curr );
        } else if( curr.info->current_type == RESOURCE2INT( RT_DIALOG ) ) {
            ok = WREEditDialogResource( &curr );
        } else if( curr.info->current_type == RESOURCE2INT( RT_GROUP_CURSOR ) ) {
            ok = WREEditImageResource( &curr );
        } else if( curr.info->current_type == RESOURCE2INT( RT_GROUP_ICON ) ) {
            ok = WREEditImageResource( &curr );
        } else if( curr.info->current_type == RESOURCE2INT( RT_BITMAP ) ) {
            ok = WREEditImageResource( &curr );
        } else {
            ok = false;
        }
    }

    return( ok );
}

LRESULT WREHandleMDIArrangeEvents( WORD w )
{
    UINT    msg;
    WPARAM  wp;

    wp = 0;
    switch( w ) {
    case IDM_MDI_CASCADE:
        msg = WM_MDICASCADE;
        break;

    case IDM_MDI_TILEV:
        msg = WM_MDITILE;
        wp = MDITILE_VERTICAL;
        break;

    case IDM_MDI_TILEH:
        msg = WM_MDITILE;
        wp = MDITILE_HORIZONTAL;
        break;

    case IDM_MDI_ARRANGE:
        msg = WM_MDIICONARRANGE;
        break;

    default:
        return( FALSE );
    }

    return( SendMessage( WREMDIWin, msg, wp, 0 ) );
}

void WREUpdateScreenPosOpt( void )
{
    RECT        rect;

    GetWindowRect( WREMainWin, &rect );

    WRESetScreenPosOption( &rect );
}

void WREResizeWindows( void )
{
    RECT        rect;
    int         ribbon_depth;
    int         stat_depth;

    if( !WRECleanupStarted ) {
        ribbon_depth = WREGetRibbonHeight();
        stat_depth = WREGetStatusDepth();
        GetClientRect( WREMainWin, &rect );
        WREResizeStatusWindows( &rect );
        WREResizeRibbon( &rect );
        MoveWindow( WREMDIWin, 0, ribbon_depth, rect.right - rect.left,
                    rect.bottom - (rect.top + stat_depth + ribbon_depth), TRUE );
    }
}

bool WRECleanup( bool fatal_exit )
{
    /* clean up before we exit */
    if( !WREEndAllStringSessions( fatal_exit ) ||
        !WREEndAllMenuSessions( fatal_exit ) ||
        !WREEndAllAccelSessions( fatal_exit ) ||
        !WREEndAllImageSessions( fatal_exit ) ||
        !WREEndAllDialogSessions( fatal_exit ) ) {
        return( false );
    }

    if( fatal_exit || WREQueryKillApp( false ) ) {
        WREFreeResList();
    } else {
        return( false );
    }

    WRESetOption( WREOptScreenMax, IsZoomed( WREMainWin ) );

    WREFiniHints();
    WREOptsShutdown();
    WREDestroyRibbon();
    WREDestroyStatusLine();
    WREShutdownRibbon();
    WREFiniTypeNames();
    WREFiniTotalText();
    WREFiniResources();
    WREShutdownToolBars();
    WREFiniClipboard();
    WRECtl3DFini( WREInst );
    WREFreeFileFilter();
    JDialogFini();

    return( true );
}

bool WREProcessArgs( char **argv, int argc )
{
    int     i;
    bool    ok;

    ok = true;

    for( i = 1; i < argc; i++ ) {
        if( ( argv[i][0] == '/' || argv[i][0] == '-' ) && stricmp( argv[i] + 1, CREATE_NEW_FLAG ) == 0 ) {
            WRECreateNewFiles = true;
        } else if( ( argv[i][0] == '/' || argv[i][0] == '-' ) && stricmp( argv[i] + 1, NO_IFACE_FLAG ) == 0 ) {
            WRENoInterface = true;
        } else {
            if( WRFileExists( argv[i] ) ) {
                ok = WREOpenResource( argv[i] ) && ok;
            } else if( WRECreateNewFiles ) {
                ok = (WRECreateNewResource( argv[i] ) != NULL && ok);
            } else {
                ok = false;
            }
        }
    }

    if( !ok ) {
        WREDisplayErrorMsg( WRE_INPUTFILESNOTLOADED );
    }

    return( ok );
}

#include "wrwresid.h"

void WREDisplaySplashScreen( HINSTANCE inst, HWND parent, UINT msecs )
{
    DLGPROC     dlgproc;

    dlgproc = MakeProcInstance_DLG( WRESplashDlgProc, WREInst );
    JDialogBoxParam( inst, "WRESplashScreen", parent, dlgproc, (LPARAM)&msecs );
    FreeProcInstance_DLG( dlgproc );
}

INT_PTR CALLBACK WRESplashDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    UINT        msecs, start;
    UINT_PTR    timer;
    HDC         dc, tdc;
    HBITMAP     old;
    HWND        w666;
    RECT        rect, arect;
    PAINTSTRUCT ps;
#ifdef __NT__
    HINSTANCE   hInstUser;
    PFNLI       pfnLoadImage;
#endif

    static BITMAP    bm;
    static HBITMAP   logo;
    static HBRUSH    brush;
    static COLORREF  color;

    /* unused parameters */ (void)wParam;

    switch( message ) {
    case WM_SYSCOLORCHANGE:
        WRECtl3dColorChange();
        break;

    case WM_DESTROY:
        if( logo != NULL ) {
            DeleteObject( logo );
        }
        if( brush != NULL ) {
            DeleteObject( brush );
        }
        break;

    case WM_INITDIALOG:
        msecs = *(UINT *)lParam;
        if( msecs != 0 ) {
            timer = SetTimer( hDlg, ABOUT_TIMER, msecs, NULL );
            if( timer != 0 ) {
                SET_DLGDATA( hDlg, timer );
            }
        }

#ifdef __NT__
        hInstUser = GetModuleHandle( "USER32.DLL" );
        pfnLoadImage = (PFNLI)GetProcAddress( hInstUser, "LoadImageA" );
        if( pfnLoadImage != NULL ) {
            logo = pfnLoadImage( WREInst, "AboutLogo", IMAGE_BITMAP, 0, 0,
                                 LR_LOADMAP3DCOLORS );
        } else {
#endif
            logo = LoadBitmap( WREInst, "AboutLogo" );
#ifdef __NT__
        }
#endif

        //color = RGB( 128, 128, 128 );
        color = GetSysColor( COLOR_BTNFACE );
        brush = CreateSolidBrush( color );

        GetObject( logo, sizeof( BITMAP ), &bm );
        return( TRUE );

#if 0
#ifdef __NT__
    case WM_CTLCOLORSTATIC:
        if( brush != NULL ) {
            dc = (HDC)wParam;
            SetBkColor( dc, color );
            return( (LRESULT)brush );
        }
        break;
#else
    case WM_CTLCOLOR:
        if( brush != NULL ) {
            dc = (HDC)wParam;
            if( HIWORD( lParam ) == CTLCOLOR_STATIC ) {
                SetBkColor( dc, color );
            }
            return( (LRESULT)brush );
        }
        break;
#endif

    case WM_ERASEBKGND:
        if( brush != NULL ) {
            GetClientRect( hDlg, &rect );
            UnrealizeObject( brush );
            FillRect( (HDC)wParam, &rect, brush );
            return( TRUE );
        }
        break;
#endif

    case WM_PAINT:
        dc = BeginPaint( hDlg, &ps );
        if( dc != NULL ) {
            w666 = GetDlgItem( hDlg, 666 );
            GetClientRect( w666, &rect );
            GetClientRect( hDlg, &arect );
            start = (arect.right - arect.left - bm.bmWidth) / 2;
            MapWindowPoints( w666, hDlg, (POINT *)&rect, 2 );
            tdc = CreateCompatibleDC( dc );
            old = SelectObject( tdc, logo );
            BitBlt( dc, start, rect.top + 20, bm.bmWidth, bm.bmHeight, tdc, 0, 0, SRCCOPY );
            SelectObject( tdc, old );
            DeleteDC( tdc );
            EndPaint( hDlg, &ps );
        }
        break;

    case WM_TIMER:
        timer = (UINT_PTR)GET_DLGDATA( hDlg );
        if( timer != 0 ) {
            KillTimer( hDlg, timer );
        }
        EndDialog( hDlg, TRUE );
        return( TRUE );
    }

    return( FALSE );
}

WINEXPORT void CALLBACK WREHelpRoutine( void )
{
    if( !WHtmlHelp( WREMainWin, "resedt.chm", HELP_CONTENTS, 0 ) ) {
        WWinHelp( WREMainWin, "resedt.hlp", HELP_CONTENTS, 0 );
    }
}

void WREHelpSearchRoutine( void )
{
    if( !WHtmlHelp( WREMainWin, "resedt.chm", HELP_PARTIALKEY, (HELP_DATA)(LPCSTR)"" ) ) {
        WWinHelp( WREMainWin, "resedt.hlp", HELP_PARTIALKEY, (HELP_DATA)(LPCSTR)"" );
    }
}

void WREHelpOnHelpRoutine( void )
{
    WWinHelp( WREMainWin, "winhelp.hlp", HELP_HELPONHELP, 0 );
}
