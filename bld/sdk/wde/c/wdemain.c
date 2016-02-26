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
#include <io.h>
#include <stdio.h>
#include "wderes.h"
#include "wdetest.h"
#include "wdestat.h"
#include "wdefdiag.h"
#include "wdeedit.h"
#include "wdealign.h"
#include "wdedefin.h"
#include "wdesym.h"
#include "wdeinfo.h"
#include "wdefutil.h"
#include "wdemsgbx.h"
#include "rcstr.gh"
#include "wdefont.h"
#include "wdeopts.h"
#include "wdefinit.h"
#include "wdesdlg.h"
#include "wdetmenu.h"
#include "wdetoolb.h"
#include "wdectool.h"
#include "wderibbn.h"
#include "wdehints.h"
#include "wdedebug.h"
#include "wdecust.h"
#include "wdefordr.h"
#include "wdestken.h"
#include "wdectl3d.h"
#include "wde_rc.h"
#include "wdemain.h"
#include "wdesvdlg.h"
#include "wdedde.h"
#include "wdeclear.h"
#include "wdecurr.h"
#include "wdegoto.h"
#include "wdegetfn.h"
#include "wdefmenu.h"
#include "wdecctl.h"
#include "wrdll.h"
#include "wrdmsg.h"
#include "jdlg.h"

#include "wwinhelp.h"
#include "aboutdlg.h"
#include "ldstr.h"
#include "clibint.h"

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

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define ABOUT_TIMER     666
#define ABOUT_TIMEOUT   2000
#define DDE_OPT         "-DDE"
#define CREATE_NEW_FLAG "/n"

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT LRESULT CALLBACK WdeMainWndProc( HWND, UINT, WPARAM, LPARAM );
WINEXPORT BOOL    CALLBACK WdeSplash( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static bool        WdeInit( HINSTANCE );
static bool        WdeInitInst( HINSTANCE );
static void        WdeUpdateScreenPosOpt( void );
static bool        WdeCleanup( WdeResInfo *, bool );
static HWND        WdeCreateMDIClientWindow( HWND, HINSTANCE );
static bool        WdeWasAcceleratorHandled( MSG * );
static bool        WdeSaveCurrentDialog( WORD );
static bool        WdeRestoreCurrentDialog( void );
static bool        WdeHideCurrentDialog( void );
static void        WdeHandleTabEvent( bool );
static void        WdeHandleSizeToText( void );
static void        WdeSetMakeMeCurrent( WdeResInfo *, void * );
static LRESULT     WdeHandleMDIArrangeEvents( WORD );
static bool        WdeSetDialogMode( WORD );
static bool        WdeProcessArgs( char **, int  );
static void        WdeDisplaySplashScreen( HINSTANCE, HWND, UINT );
static bool        WdeIsDDEArgs( char **argv, int argc );

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

#ifdef __NT__
typedef HANDLE (WINAPI *PFNLI)( HINSTANCE, LPCSTR, UINT, int, int, UINT );
#endif

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static HINSTANCE hInstWde;
static HACCEL    WdeAccel;
static HMENU     WdeDDEMenu         = NULL;
static HMENU     WdeInitialMenu     = NULL;
static HMENU     WdeResMenu         = NULL;
static HMENU     WdeCurrentMenu     = NULL;
static HWND      hWinWdeMain        = NULL;
static HWND      hWinWdeMDIClient   = NULL;
static bool      WdeCleanupStarted  = FALSE;
static bool      WdeFatalExit       = FALSE;
static bool      IsDDE              = FALSE;
static bool      EnableMenuInput    = FALSE;
static char      WdeMainClass[]     = "WdeMainClass";
static bool      WdeFirstInst       = FALSE;
static jmp_buf   WdeEnv;

bool WdeCreateNewFiles  = FALSE;

int PASCAL WinMain( HINSTANCE hinstCurrent, HINSTANCE hinstPrevious,
                    LPSTR lpszCmdLine,  int nCmdShow )
{
    MSG         msg;
#ifndef __NT__
#if 0
    HWND        win;
    HWND        child;
#endif
#endif

    /* touch unused vars to get rid of warning */
    _wde_touch( lpszCmdLine );
    _wde_touch( nCmdShow );
#ifdef __NT__
    _wde_touch( hinstPrevious );
#endif
#if defined( __NT__ ) && !defined( __WATCOMC__ )
    _argc = __argc;
    _argv = __argv;
#endif

    WRInit();
    WdeInitDisplayError( hinstCurrent );

    /* store the handle to this instance of Wde in a static variable */
    hInstWde = hinstCurrent;

    //check we are running in DDE mode
    IsDDE = WdeIsDDEArgs( _argv, _argc );

    WdeFirstInst = (hinstPrevious == NULL);

    WdeInitEditClass();

    /* is this the first instance of the application? */
#ifndef __NT__
    if( WdeFirstInst ) {
#endif
        /* if so call the routine to initialize the application */
        if( !WdeInit( hinstCurrent ) ) {
            if( IsDDE ) {
                WdeDDEDumpConversation( hinstCurrent );
            }
            return( FALSE );
        }
#ifndef __NT__
    }
#if 0
    else if( IsDDE ) {
        WdeDisplayErrorMsg( WDE_NOMULTIPLEINSTANCES );
        WdeDDEDumpConversation( hinstCurrent );
        return( FALSE );
    } else {
        win = FindWindow( WdeMainClass, NULL );
        if( win != NULL ) {
            child = GetLastActivePopup( win );
            PostMessage( win, WM_USER, 0, 0 );
            BringWindowToTop( win );
            if( child != (HWND)NULL && child != win ) {
                BringWindowToTop( child );
            }
        } else {
            WdeDisplayErrorMsg( WDE_NOMULTIPLEINSTANCES );
        }
        return( FALSE );
    }
#endif
#endif

    if( !WdeInitInst( hinstCurrent ) ) {
        WdeDisplayErrorMsg( WDE_INITFAILED );
        if( IsDDE ) {
            WdeDDEDumpConversation( hinstCurrent );
        }
        return( FALSE );
    }

    if( IsDDE ) {
        if( WdeDDEStart( hinstCurrent ) ) {
            if( !WdeDDEStartConversation() ) {
                WdeDisplayErrorMsg( WDE_DDEINITFAILED );
                PostMessage( hWinWdeMain, WDE_FATAL_EXIT, 0, 0 );
            }
        } else {
            WdeDisplayErrorMsg( WDE_DDEINITFAILED );
            PostMessage( hWinWdeMain, WDE_FATAL_EXIT, 0, 0 );
        }
    }

    WdeEnableMenuInput( TRUE );

    if( setjmp( WdeEnv ) ) {
        PostMessage( hWinWdeMain, WDE_FATAL_EXIT, 0, 0 );
        if( setjmp( WdeEnv ) ) {
            WdeDisplayErrorMsg( WDE_EXCEPTIONDURINGABNORMALEXIT );
            exit( -1 );
        }
        WdePushEnv( &WdeEnv );
    } else {
        WdePushEnv( &WdeEnv );
        WdeProcessArgs( _argv, _argc );
    }

    if( !WdeGetNumRes() ) {
        WdeCreateNewResource( NULL );
    }

    /* create the message loop */
    while( GetMessage( &msg, (HWND)NULL, 0, 0 ) ) {
        if( !WdeIsTestMessage( &msg ) && !WdeIsInfoMessage( &msg ) &&
            !WRIsWRDialogMsg( &msg ) ) {
            if( !WdeWasAcceleratorHandled( &msg ) ) {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }
        }
    }

    if( IsDDE ) {
        WdeDDEEndConversation();
    }

    WdePopEnv( &WdeEnv );

    if( IsDDE ) {
        WdeDDEEnd();
    }

    WRFini();

    return( msg.wParam );
}

/* Function to initialize the first instance of Wde */
bool WdeInit( HINSTANCE app_inst )
{
    WNDCLASS wc;

    /* fill in the WINDOW CLASS structure for the main window */
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = WdeMainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = app_inst;
    wc.hIcon = LoadIcon( app_inst, "APPLICON" );
    wc.hCursor = LoadCursor( (HINSTANCE)NULL, IDC_ARROW );
    wc.hbrBackground = NULL;
    wc.lpszMenuName = "WdeMenu";
    wc.lpszClassName = WdeMainClass;

    /* register the main window class */
    if( !RegisterClass( &wc ) ) {
        WdeDisplayErrorMsg( WDE_NOREGISTERMAINCLASS );
        return( FALSE );
    }

    /* register the edit window class */
    if( !WdeRegisterEditClass( app_inst ) ) {
        WdeDisplayErrorMsg( WDE_NOREGISTEREDITCLASS );
        return( FALSE );
    }

    /* register the res MDI window class */
    if( !WdeRegisterResClass( app_inst ) ) {
        WdeDisplayErrorMsg( WDE_NOREGISTERRESCLASS );
        return( FALSE );
    }

    /* register the tag window class */
    if( !WdeRegisterTagClass( app_inst ) ) {
        WdeDisplayErrorMsg( WDE_NOREGISTERTAGCLASS );
        return( FALSE );
    }

    LoadCommCtrl();

    return( TRUE );
}

/* Function to initialize all instances of Wde */
bool WdeInitInst( HINSTANCE app_inst )
{
    RECT        rect, screen, t;
    char        *title;

    /* initialize global variables */

    WdeInitOpts();

    if( !JDialogInit() ) {
        return( FALSE );
    }

    if( !WdeInitResStrings() ) {
        return( FALSE );
    }

    if( !WdeInitCreateTable() ) {
        WdeDisplayErrorMsg( WDE_NOINITCONTROLOBJECTS );
        return( FALSE );
    }

    WdeCtl3DInit( app_inst );
    WdeGetScreenPosOption( &rect );

    /* load the accelerator table */
    WdeAccel = LoadAccelerators( app_inst, "WdeAccelTable");

    /* if the window could not be created return FALSE */
    if( WdeAccel == (HACCEL)NULL ) {
        WdeDisplayErrorMsg( WDE_LOADACCELFAILED );
        return( FALSE );
    }

    GetWindowRect( GetDesktopWindow(), &screen );

    if( !IntersectRect( &t, &screen, &rect ) ) {
        t.right = rect.right - rect.left;
        t.bottom = rect.bottom - rect.top;
        t.left = 0;
        if( screen.right - screen.left - t.right > 0 )
            t.left = ( screen.right - screen.left - t.right ) / 2;
        t.top = 0;
        if( screen.bottom - screen.top - t.bottom > 0 )
            t.top = ( screen.bottom - screen.top - t.bottom ) / 2;
        t.right += t.left;
        t.bottom += t.top;
    } else {
        t = rect;
    }

    title = WdeAllocRCString( WDE_APPTITLE );
    /* attempt to create the main application window */
    hWinWdeMain = CreateWindow( WdeMainClass, title, WS_OVERLAPPEDWINDOW,
                                t.left, t.top, t.right - t.left, t.bottom - t.top,
                                (HWND)NULL, (HMENU)NULL, app_inst, NULL );
    if( title != NULL ) {
        WdeFreeRCString( title );
    }

    /* if the window could not be created return FALSE */
    if( hWinWdeMain == NULL ) {
        WdeDisplayErrorMsg( WDE_NOCREATEMAINWINDOW );
        return( FALSE );
    }

    hWinWdeMDIClient = WdeCreateMDIClientWindow( hWinWdeMain, app_inst );

    /* attempt to create the main application info window */
    if( !WdeCreateInfoWindow( hWinWdeMain, app_inst ) ) {
        WdeDisplayErrorMsg( WDE_NOCREATEINFOWINDOW );
        return( FALSE );
    }

    if( !WdeCreateStatusLine( hWinWdeMain, app_inst ) ) {
        WdeDisplayErrorMsg( WDE_NOCREATESTATUSLINE );
        return( FALSE );
    }

    WdeInitialMenu = GetMenu( hWinWdeMain );
    WdeCurrentMenu = WdeInitialMenu;
    WdeResMenu = LoadMenu( app_inst, "WdeResMenu" );
    WdeDDEMenu = LoadMenu( app_inst, "WdeDDEMenu" );

    if( IsDDE ) {
        WdeSetAppMenuToRes( TRUE );
    }


    if( !WdeInitialMenu || !WdeResMenu ) {
        WdeDisplayErrorMsg( WDE_NOINITMENUS );
        return( FALSE );
    }

    if( !WdeInitHints() ) {
        WdeDisplayErrorMsg( WDE_NOINITHINTS );
        return( FALSE );
    }

    WdeInitRibbon( app_inst );

    if( WdeGetOption( WdeOptIsRibbonVisible ) ) {
        WdeShowRibbon();
    }

    /* if the window was created Show and Update it */
    if( WdeGetOption( WdeOptIsScreenMax ) ) {
        ShowWindow( hWinWdeMain, SW_SHOWMAXIMIZED );
    } else {
        ShowWindow( hWinWdeMain, SW_SHOWNORMAL );
    }
    UpdateWindow( hWinWdeMain );

    WdeSetFontList( hWinWdeMain );

    if( !IsDDE ) {
        WdeDisplaySplashScreen( hInstWde, hWinWdeMain, 1125 );
    }

    if( IsDDE ) {
        WdeInitToolMenu( app_inst, WdeDDEMenu );
    } else {
        WdeInitToolMenu( app_inst, WdeResMenu );
    }
    WdeInitControls( app_inst );

    if( WdeGetOption( WdeOptIsCntlsTBarVisible ) ) {
        if( !WdeCreateControlsToolBar ( ) ) {
            WdeDisplayErrorMsg( WDE_NOCREATETOOLBOX );
            return( FALSE );
        }
    }

    SetFocus( hWinWdeMain );

    return( TRUE );

}

bool WdeWasAcceleratorHandled( MSG *msg )
{
    if( !TranslateMDISysAccel( hWinWdeMDIClient, msg ) &&
        !TranslateAccelerator( hWinWdeMain, WdeAccel, msg ) ) {
        return( FALSE );
    }
    return( TRUE );
}

void WdeSetAppMenuToRes( bool set_to_res_menu )
{
    HMENU   menu;
    HMENU   new_menu;

    WdeShowInfoWindow( set_to_res_menu );

    if( set_to_res_menu ) {
        if( IsDDE ) {
            new_menu = WdeDDEMenu;
        } else {
            new_menu = WdeResMenu;
        }
    } else {
        new_menu = WdeInitialMenu;
    }

    if( new_menu == NULL || WdeCurrentMenu == new_menu ) {
        return;
    }

    if( set_to_res_menu ) {
        menu = GetSubMenu( new_menu, WINDOW_MENU );
    } else {
        menu = GetSubMenu( new_menu, 0 );
    }

#ifdef __NT__
    SendMessage( hWinWdeMDIClient, WM_MDISETMENU, (WPARAM)new_menu, (LPARAM)menu );
#else
    SendMessage( hWinWdeMDIClient, WM_MDISETMENU, FALSE, MAKELPARAM( new_menu, menu ) );
#endif

    WdeCurrentMenu = new_menu;

    DrawMenuBar( hWinWdeMain );
}

HWND WdeCreateMDIClientWindow( HWND win, HINSTANCE app_inst )
{
    CLIENTCREATESTRUCT ccs;
    RECT               rect;
    HWND               client;
    int                info_depth;
    int                stat_depth;
    int                ribbon_depth;

    info_depth = WdeGetInfoWindowDepth();
    stat_depth = WdeGetStatusDepth();
    ribbon_depth = WdeGetRibbonHeight();

    GetClientRect( win, &rect );

    ccs.hWindowMenu = GetSubMenu( GetMenu( win ), 0 );
    ccs.idFirstChild = WDE_MDI_FIRST;

    /* attempt to create the main application edit window */
    client = CreateWindow( "MDIClient", "",
                           WS_CLIPCHILDREN | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL,
                           0, info_depth + ribbon_depth, rect.right - rect.left,
                           rect.bottom - (rect.top + stat_depth + info_depth + ribbon_depth),
                           win, (HMENU)NULL, app_inst, &ccs );

    /* if the window could not be created return FALSE */
    if( client == NULL ) {
        WdeDisplayErrorMsg( WDE_NOCREATEMDICLIENT );
        return( FALSE );
    }

    return( client );
}

bool WdeIsFirstInst( void )
{
    return( WdeFirstInst );
}

void WdeEnableMenuInput( bool enable )
{
    EnableMenuInput = enable;
}

bool WdeIsDDE( void )
{
    return( IsDDE );
}

HMENU WdeGetInitialMenuHandle( void )
{
    return( WdeInitialMenu );
}

HMENU WdeGetResMenuHandle( void )
{
    if( IsDDE ) {
        return( WdeDDEMenu );
    } else {
        return( WdeResMenu );
    }
}

HMENU WdeGetMenuHandle( void )
{
    if( hWinWdeMain != NULL ) {
        return( WdeCurrentMenu );
    } else {
        return( NULL );
    }
}

HWND WdeGetMDIWindowHandle( void )
{
    return( hWinWdeMDIClient );
}

HWND WdeGetMainWindowHandle( void )
{
    return( hWinWdeMain );
}

HINSTANCE WdeGetAppInstance( void )
{
    return( hInstWde );
}

bool WdeInCleanup( void )
{
    return( WdeCleanupStarted );
}

static void handleInitMenu( HMENU menu )
{
    WdeEnableCommonControlsMenu( menu );
    WdeEnableSelectCustCntl( menu );
    WdeEnableCustCntlTools( menu );
    if( WdeGetNumRes() ) {
        WdeEnablePasteItem( menu );
    }
}

WINEXPORT LRESULT CALLBACK WdeMainWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    HMENU       menu;
    LRESULT     ret;
    bool        pass_to_def;
    WdeResInfo  *res_info;
    int         wp;
    about_info  ai;

    if( WdeCleanupStarted ) {
        if( message == WM_DESTROY ) {
            PostQuitMessage( 0 );
        }
        return( DefFrameProc( hWnd, hWinWdeMDIClient, message, wParam, lParam ) );
    }

    pass_to_def = TRUE;
    ret = FALSE;
    res_info = WdeGetCurrentRes();
    menu = WdeGetMenuHandle();

    switch( message ) {
    case WM_INITMENU:
        handleInitMenu( menu );
        break;

    case WM_USER:
        WdeSetStatusByID( -1, WDE_ONLYONEINSTANCE );
        break;

    case WM_MENUSELECT:
        WdeHandleMenuSelect( wParam, lParam );
        break;

    case WM_MEASUREITEM:
        WdeHandleMeasureItem( (MEASUREITEMSTRUCT *)lParam );
        ret = TRUE;
        pass_to_def = FALSE;
        break;

    case WM_DRAWITEM:
        WdeHandleDrawItem( (DRAWITEMSTRUCT *)lParam );
        ret = TRUE;
        pass_to_def = FALSE;
        break;

    case WM_MOVE:
        if( IsZoomed( hWnd ) ) {
            WdeSetOption( WdeOptIsScreenMax, TRUE );
        } else if( !IsIconic( hWnd ) ) {
            WdeUpdateScreenPosOpt();
            WdeSetOption( WdeOptIsScreenMax, FALSE );
        }
        break;

    case WM_SIZE:
        if( wParam == SIZE_MAXIMIZED ) {
            WdeSetOption( WdeOptIsScreenMax, TRUE );
        } else if( wParam != SIZE_MINIMIZED ) {
            WdeUpdateScreenPosOpt();
            WdeSetOption( WdeOptIsScreenMax, FALSE );
        }

        if( wParam != SIZE_MINIMIZED ) {
            WdeResizeWindows();
        }

        pass_to_def = FALSE;
        break;

    case WM_COMMAND:
        wp = LOWORD( wParam );
        if( !WdeIsMenuIDValid( menu, wp ) ) {
            break;
        }
        switch( wp ) {
        case IDM_SELECT_MODE:
        case IDM_DIALOG_TOOL:
        case IDM_PBUTTON_TOOL:
        case IDM_CBUTTON_TOOL:
        case IDM_RBUTTON_TOOL:
        case IDM_GBUTTON_TOOL:
        case IDM_FRAME_TOOL:
        case IDM_TEXT_TOOL:
        case IDM_ICON_TOOL:
        case IDM_EDIT_TOOL:
        case IDM_LISTBOX_TOOL:
        case IDM_COMBOBOX_TOOL:
        case IDM_HSCROLL_TOOL:
        case IDM_VSCROLL_TOOL:
        case IDM_SIZEBOX_TOOL:
        case IDM_STATUSBAR_TOOL:
        case IDM_LISTVIEW_TOOL:
        case IDM_TREEVIEW_TOOL:
        case IDM_TABCNTL_TOOL:
        case IDM_ANIMATE_TOOL:
        case IDM_UPDOWN_TOOL:
        case IDM_TRACKBAR_TOOL:
        case IDM_PROGRESS_TOOL:
        case IDM_HOTKEY_TOOL:
        case IDM_HEADER_TOOL:
        case IDM_CUSTOM1_TOOL:
        case IDM_CUSTOM2_TOOL:
            WdeSetBaseObject( wp );
            pass_to_def = FALSE;
            break;

        case IDM_STICKY_TOOLS:
            WdeToggleStickyTools();
            pass_to_def = FALSE;
            break;

        case IDM_FMLEFT:
        case IDM_FMRIGHT:
        case IDM_FMTOP:
        case IDM_FMBOTTOM:
        case IDM_FMVCENTRE:
        case IDM_FMHCENTRE:
        case IDM_ESCAPE:
        case IDM_DELETEOBJECT:
        case IDM_SPACE_HORZ:
        case IDM_SPACE_VERT:
            ret = WdePassToEdit( message, wParam, lParam );
            pass_to_def = FALSE;
            break;

        case IDM_SELECT_DIALOG:
            WdeSelectDialog( res_info );
            pass_to_def = FALSE;
            break;

        case IDM_OPTIONS:
            WdeDisplayOptions ();
            pass_to_def = FALSE;
            break;

        case IDM_NEW_RES:
            WdeCreateNewResource( NULL );
            pass_to_def = FALSE;
            break;

        case IDM_DDE_CLEAR:
            WdeClearCurrentResource();
            pass_to_def = FALSE;
            break;

        case IDM_OPEN_RES:
            WdeOpenResource( NULL );
            pass_to_def = FALSE;
            break;

        case IDM_SAVE_RES:
            WdeSaveResource( res_info, FALSE );
            pass_to_def = FALSE;
            break;

        case IDM_SAVEAS_RES:
            WdeSaveResource( res_info, TRUE );
            pass_to_def = FALSE;
            break;

        case IDM_DDE_UPDATE_PRJ:
            WdeUpdateDDEEditSession();
            break;

        case IDM_PASTEOBJECT:
            if( FMPasteValid() ) {
                ret = WdePassToEdit( message, wParam, lParam );
            }
            pass_to_def = FALSE;
            break;

        case IDM_CUTOBJECT:
        case IDM_COPYOBJECT:
            ret = WdePassToEdit( message, wParam, lParam );
            pass_to_def = FALSE;
            break;

        case IDM_SAME_WIDTH:
            WdeSameSize( R_RIGHT );
            pass_to_def = FALSE;
            break;

        case IDM_SAME_HEIGHT:
            WdeSameSize( R_BOTTOM );
            pass_to_def = FALSE;
            break;

        case IDM_SAME_SIZE:
            WdeSameSize( R_BOTTOM | R_RIGHT );
            pass_to_def = FALSE;
            break;

        case IDM_SIZETOTEXT:
            WdeHandleSizeToText();
            pass_to_def = FALSE;
            break;

        case IDM_GOTO_INFOBAR:
            WdeSetFocusToInfo();
            pass_to_def = FALSE;
            break;

        case IDM_GOTO_OBJECT:
            WdeHandleGotoCurrentObject();
            pass_to_def = FALSE;
            break;

        case IDM_DIALOG_RESTORE:
            WdeRestoreCurrentDialog();
            pass_to_def = FALSE;
            break;

        case IDM_TAB:
        case IDM_STAB:
            WdeHandleTabEvent( wp == IDM_TAB );
            pass_to_def = FALSE;
            break;

#if 0
        /* this strategy has been rejected in favor of the hide option */
        case IDM_REMOVE_DIALOG:
            WdeRemoveDialog( res_info );
            pass_to_def = FALSE;
            break;
#endif

        case IDM_HIDE_DIALOG:
            WdeHideCurrentDialog();
            pass_to_def = FALSE;
            break;

        case IDM_TEST_MODE:
            WdeHandleTestModeMenu( res_info );
            pass_to_def = FALSE;
            break;

        case IDM_DIALOG_NEW:
            if( WdeCreateNewDialog( NULL, res_info->is32bit ) ) {
                WdeHandleGotoCurrentObject();
            }
            pass_to_def = FALSE;
            break;

        case IDM_DIALOG_SAVE:
        case IDM_DIALOG_SAVEAS:
        case IDM_DIALOG_SAVEINTO:
            WdeSaveCurrentDialog( wp );
            pass_to_def = FALSE;
            break;

        case IDM_SET_ORDER:
        case IDM_SET_TABS:
        case IDM_SET_GROUPS:
            WdeSetDialogMode( wp );
            pass_to_def = FALSE;
            break;

        case IDM_DEFINEOBJECT:
        case IDM_GEN_DEFINEOBJECT:
            if( !WdeDefineCurrentObject( wp ) ) {
                WdeWriteTrail( "WdeResWndProc: Define failed!" );
            }
            pass_to_def = FALSE;
            break;

        case IDM_MDI_CASCADE:
        case IDM_MDI_TILEV:
        case IDM_MDI_TILEH:
        case IDM_MDI_ARRANGE:
            WdeHandleMDIArrangeEvents( wp );
            pass_to_def = FALSE;
            break;

        case IDM_EXIT:
            /* clean up before we exit */
            PostMessage( hWinWdeMain, WM_CLOSE, 0, 0 );
            break;

        case IDM_SELCUST1:
        case IDM_SELCUST2:
            if( !WdeSetCurrentCustControl( wp - IDM_SELCUST1 ) ) {
                WdeWriteTrail( "WdeMainWndProc: WdeSetCurrentCustControl failed!" );
                WdeSetStatusByID( -1, WDE_SETFAILED );
            }
            pass_to_def = FALSE;
            break;

        case IDM_MS_CUSTOM:
            WdeLoadCustomLib( TRUE, FALSE );
            pass_to_def = FALSE;
            break;

#ifndef __NT__
        case IDM_BOR_CUSTOM:
            WdeLoadCustomLib( FALSE, FALSE );
            pass_to_def = FALSE;
            break;
#endif

        case IDM_LOADLIB:
            WdeLoadCustomLib( FALSE, TRUE );
            pass_to_def = FALSE;
            break;

        case IDM_WRITE_SYMBOLS:
            WdeResourceHashTableAction( res_info, WRITE_HASH );
            pass_to_def = FALSE;
            break;

        case IDM_LOAD_SYMBOLS:
            WdeResourceHashTableAction( res_info, LOAD_HASH );
            pass_to_def = FALSE;
            break;

        case IDM_VIEW_SYMBOLS:
            WdeResourceHashTableAction( res_info, VIEW_HASH );
            pass_to_def = FALSE;
            break;

        case IDM_SHOW_TOOLS:
            WdeHandleShowToolsMenu();
            pass_to_def = FALSE;
            break;

        case IDM_SHOW_RIBBON:
            WdeShowRibbon();
            pass_to_def = FALSE;
            break;

        case IDM_HELP:
            WdeHelpRoutine();
            pass_to_def = FALSE;
            break;

       case IDM_HELP_SEARCH:
            WdeHelpSearchRoutine();
            pass_to_def = FALSE;
            break;

       case IDM_HELP_ON_HELP:
            WdeHelpOnHelpRoutine();
            pass_to_def = FALSE;
            break;

       case IDM_ABOUT:
            ai.owner = hWnd;
            ai.inst = hInstWde;
            ai.name = AllocRCString( WDE_ABOUT_NAME );
            ai.version = AllocRCString( WDE_ABOUT_VERSION );
            ai.title = AllocRCString( WDE_ABOUT_TITLE );
            DoAbout( &ai );
            FreeRCString( ai.name );
            FreeRCString( ai.version );
            FreeRCString( ai.title );
            pass_to_def = FALSE;
            break;
        }
        break;

    case WM_DESTROY:
        WWinHelp( hWinWdeMain, "resdlg.hlp", HELP_QUIT, 0 );
        PostQuitMessage( 0 );
        break;

    case WDE_FATAL_EXIT:
        WdeFatalExit = TRUE;
        WdeQueryKillApp( TRUE );
        PostMessage( hWinWdeMain, WM_CLOSE, 0, 0 );
        break;

    case WM_ENDSESSION:
        if ( !wParam ) {
            WdeFatalExit = FALSE;
        }
        break;

    case WM_QUERYENDSESSION:
        ret = WdeQueryKillApp( FALSE );
        if( ret ) {
            WdeFatalExit = TRUE;
            pass_to_def = FALSE;
        }
        break;

    case WM_CLOSE:
        // this prevents the user from closing the editor during
        // DDE initialization
        if( !wParam && !EnableMenuInput ) {
            return( 0 );
        }
        /* clean up before we exit */
        WdeCleanupStarted = TRUE;
        if( wParam ) {  // wParam is non-zero if the DDE connection died
            WdeFatalExit = TRUE;
            WdeQueryKillApp( TRUE );
            wParam = 0;
        }
        if( !WdeCleanup( res_info, WdeFatalExit ) ) {
            WdeCleanupStarted = FALSE;
            pass_to_def = FALSE;
        }
    }

    if( pass_to_def ) {
        ret = DefFrameProc( hWnd, hWinWdeMDIClient, message, wParam, lParam );
    }
    return( ret );
}

bool WdeIsMenuIDValid( HMENU menu, int id )
{
    UINT st;

    if( !EnableMenuInput ) {
        return( FALSE );
    }

    if( WdeGetTestMode() ) {
        if( id != IDM_TEST_MODE ) {
            return( FALSE );
        }
    }

    switch( id ) {
    case IDM_TAB:
    case IDM_STAB:
    case IDM_ESCAPE:
    case IDM_GOTO_INFOBAR:
        return( TRUE );
    }

    if( menu == (HMENU)NULL ) {
        menu = WdeGetMenuHandle();
    }

    handleInitMenu( menu );

    st = GetMenuState( menu, id, MF_BYCOMMAND );

    if( st == -1 || (st & MF_GRAYED) == MF_GRAYED ) {
        return( FALSE );
    }

    return( TRUE );
}

bool WdeSetDialogMode( WORD id )
{
    OBJPTR        obj;
    WdeOrderMode  mode;

    switch( id ) {
    case IDM_SET_ORDER:
        mode = WdeSetOrder;
        break;
    case IDM_SET_TABS:
        mode = WdeSetTabs;
        break;
    case IDM_SET_GROUPS:
        mode = WdeSetGroups;
        break;
    }

    if( (obj = WdeGetCurrentDialog()) != NULL ) {
        if( Forward( obj, SET_ORDER_MODE, &mode, NULL ) ) {
            return( TRUE );
        }
    }

    return( FALSE );
}

bool WdeSaveCurrentDialog( WORD menu_id )
{
    OBJPTR  obj;

    if ( (obj = WdeGetCurrentDialog()) != NULL ) {
        return( Forward( obj, SAVE_OBJECT, &menu_id, NULL ) );
    }

    return( FALSE );
}

bool WdeRestoreCurrentDialog( void )
{
    OBJPTR     obj;

    if( (obj = WdeGetCurrentDialog()) != NULL ) {
        return( Forward( obj, RESTORE_OBJECT, NULL, NULL ) );
    }

    return( FALSE );
}

bool WdeHideCurrentDialog( void )
{
    OBJPTR      obj;
    bool        user_action, hide;

    if( (obj = WdeGetCurrentDialog()) != NULL ) {
        user_action = FALSE;
        hide = TRUE;
        return( Forward( obj, DESTROY, &user_action, &hide ) );
    }

    return( FALSE );
}

void WdeHandleSizeToText( void )
{
    OBJPTR  obj;
    LIST    *l;

    l = WdeGetCurrObjectList();
    for( ; l != NULL; l = ListConsume( l ) ) {
        obj = ListElement( l );
        Forward( obj, SIZE_TO_TEXT, NULL, NULL );
    }
}

void WdeHandleTabEvent( bool up )
{
    OBJPTR obj;

    if( !WdeGetNumRes() ) {
        return;
    }

    if( (obj = WdeGetNextObject( up, NULL, NULL )) != NULL ) {
        MakeObjectCurrent( obj );
        HideSelectBoxes();
        WdeHandleGotoCurrentObject();
        ShowSelectBoxes();
    }
}

LRESULT WdeHandleMDIArrangeEvents( WORD w )
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

    return( SendMessage( hWinWdeMDIClient, msg, wp, 0 ) );
}

void WdeUpdateScreenPosOpt( void )
{
    RECT        rect;

    GetWindowRect( hWinWdeMain, &rect );

    WdeSetScreenPosOption( &rect );
}

void WdeResizeWindows( void )
{
    RECT        rect;
    int         info_depth;
    int         stat_depth;
    int         ribbon_depth;

    if( !WdeCleanupStarted ) {
        info_depth = WdeGetInfoWindowDepth();
        stat_depth = WdeGetStatusDepth();
        ribbon_depth = WdeGetRibbonHeight();

        GetClientRect( hWinWdeMain, &rect );

        MoveWindow( hWinWdeMDIClient, 0, info_depth + ribbon_depth,
                    rect.right - rect.left,
                    rect.bottom - (rect.top + stat_depth + info_depth + ribbon_depth),
                    TRUE );

        WdeResizeStatusWindows( &rect );
        WdeResizeRibbon( &rect );
        WdeResizeInfoWindow( &rect );
    }
}

bool WdeCleanup( WdeResInfo *res_info, bool fatal_exit )
{
    /* clean up before we exit */
    if( WdeGetTestMode() ) {
        WdeHandleTestModeMenu( res_info );
    }

    // must be done before FMEDIT is shutdown
    if( FMPasteValid() ) {
        FMResetClipboard();
    }

    WdeSetOption( WdeOptIsCntlsTBarVisible, WdeControlsToolBarExists() );

    if( WdeQueryKillApp( fatal_exit ) ) {
        WdeFreeResList();
    } else {
        return( FALSE );
    }

    WdeFiniHints();
    WdeFreeAllCustLibs();

#ifndef __NT__
    WdeFreeCustRESProcs();
#endif

    WdeShutdownToolBars();

    WdeSetOption( WdeOptIsScreenMax, IsZoomed( hWinWdeMain ) );

    WdeOptsShutdown();
    WdeFreeControlIDs();
    WdeSetAppMenuToRes( FALSE );
    DestroyMenu( WdeResMenu );
    DestroyMenu( WdeDDEMenu );
    WdeFiniToolMenu();
    WdeShutdownControls();
    WdeDestroyRibbon();
    WdeShutdownRibbon();
    WdeFiniCreateTable();
    WdeFiniOrderStuff();

    /* get rid of all the child windows of the main application window */
    WdeDestroyInfoWindow();
    WdeInfoFini();
    WdeDestroyStatusLine();
    WdeFiniEditClass();
    WdeFiniResStrings();

    WdeFreeFontList();
    WdeCtl3DFini( hInstWde );
    WdeFreeFileFilter();
    JDialogFini();

    return( TRUE );
}

bool WdeIsDDEArgs( char **argv, int argc )
{
    int i;

    for( i = 1; i < argc; i++ ) {
        if( !stricmp( argv[i], DDE_OPT ) ) {
            return( TRUE );
        }
    }

    return( FALSE );
}

bool WdeProcessArgs( char **argv, int argc )
{
    int     i;
    bool    ok;

    ok = true;

    for( i = 1; i < argc; i++ ) {
        if( !stricmp( argv[i], CREATE_NEW_FLAG ) ) {
            WdeCreateNewFiles = TRUE;
        } else if( stricmp( argv[i], DDE_OPT ) ) {
            if( WRFileExists( argv[i] ) ) {
                ok = (WdeOpenResource( argv[i] ) && ok);
            } else if( WdeCreateNewFiles ) {
                ok = (WdeCreateNewResource( argv[i] ) != NULL && ok);
            } else {
                ok = false;
            }
        }
    }

    if( !ok ) {
        WdeSetStatusByID( -1, WDE_INPUTFILENOTFOUND );
    }

    return( ok );
}

void WdeDisplaySplashScreen( HINSTANCE inst, HWND parent, UINT msecs )
{
    FARPROC     lpProcAbout;

    lpProcAbout = MakeProcInstance( (FARPROC)WdeSplash, hInstWde );
    JDialogBoxParam( inst, "WdeSplashScreen", parent, (DLGPROC)lpProcAbout, (LPARAM)&msecs );
    FreeProcInstance( lpProcAbout );
}

WINEXPORT BOOL CALLBACK WdeSplash( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    UINT        msecs, timer, start;
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

    switch( message ) {
    case WM_SYSCOLORCHANGE:
        WdeCtl3dColorChange();
        break;

    case WM_DESTROY:
        if( logo ) {
            DeleteObject( logo );
        }
        if( brush ) {
            DeleteObject( brush );
        }
        break;

    case WM_INITDIALOG:
        msecs = *(UINT *)lParam;
        if( msecs ) {
            timer = SetTimer( hDlg, ABOUT_TIMER, msecs, NULL );
            if( timer ) {
                SET_DLGDATA( hDlg, timer );
            }
        }

#ifdef __NT__
        hInstUser = GetModuleHandle( "USER32.DLL" );
        pfnLoadImage = (PFNLI)GetProcAddress( hInstUser, "LoadImageA" );
        if( pfnLoadImage != NULL ) {
            logo = pfnLoadImage( hInstWde, "AboutLogo", IMAGE_BITMAP, 0, 0,
                                 LR_LOADMAP3DCOLORS );
        } else {
#endif
            logo = LoadBitmap( hInstWde, "AboutLogo" );
#ifdef __NT__
        }
#endif

#if 0
        color = GetSysColor( COLOR_BTNFACE );
        color = RGB( 128, 128, 128 );
#endif
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
        timer = (UINT)GET_DLGDATA( hDlg );
        if( timer ) {
            KillTimer( hDlg, timer );
        }
        EndDialog( hDlg, TRUE );
        return( TRUE );
    }

    return( FALSE );
}

WINEXPORT void CALLBACK WdeHelpRoutine( void )
{
    if( !WHtmlHelp( hWinWdeMain, "resdlg.chm", HELP_CONTENTS, 0 ) ) {
        WWinHelp( hWinWdeMain, "resdlg.hlp", HELP_CONTENTS, 0 );
    }
}

WINEXPORT void CALLBACK WdeHelpSearchRoutine( void )
{
    if( !WHtmlHelp( hWinWdeMain, "resdlg.chm", HELP_PARTIALKEY, (HELP_DATA)"" ) ) {
        WWinHelp( hWinWdeMain, "resdlg.hlp", HELP_PARTIALKEY, (HELP_DATA)"" );
    }
}

WINEXPORT void CALLBACK WdeHelpOnHelpRoutine( void )
{
    WWinHelp( hWinWdeMain, "winhelp.hlp", HELP_HELPONHELP, 0 );
}
