/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
#include "wderes.h"
#include "wdemain.h"
#include "wdefmenu.h"
#include "wdefordr.h"
#include "wde_wres.h"
#include "wdedebug.h"
#include "wdestat.h"
#include "wdemsgbx.h"
#include "wdeopts.h"
#include "wdectl3d.h"
#include "wde.rh"
#include "wdetest.h"
#include "wclbproc.h"


/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/* Local Window callback functions prototypes */
WINEXPORT BOOL CALLBACK WdeSetControlEnumProc( HWND, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static bool     WdeTestCurrentObject( void );
static bool     WdeSetDefaultTestControlEntries( HWND );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static HWND             WdeTestDialogHandle = NULL;
static bool             WdeTestMode = FALSE;
static WdeResInfo       *WdeTestResource = FALSE;

bool WdeHandleTestModeMenu( WdeResInfo *info )
{
    HWND   main_window;
    bool   ret;

    if( info == NULL ) {
        return( FALSE );
    }

    main_window = WdeGetMainWindowHandle();

    if( WdeTestMode ) {
        WdeTestMode = FALSE;
        if( WdeTestDialogHandle != NULL ) {
            DestroyWindow( WdeTestDialogHandle );
        }
        WdeSetTestModeMenu( FALSE );
        WdeSetStatusReadyText();
        WdeShowResourceWindows( SW_SHOWNA );
        if( WdeTestResource != NULL ) {
            //InitState( WdeTestResource->forms_win );
            //SendMessage( WdeTestResource->res_win, WM_MDIRESTORE, 0, 0 );
            WdeTestResource = NULL;
        }
        ret = TRUE;
    } else {
        WdeSetStatusText( NULL, "", false );
        WdeSetStatusByID( WDE_TESTMODE, 0 );
        ret = WdeTestCurrentObject();
        if( ret ) {
            WdeShowResourceWindows( SW_HIDE );
            WdeSetTestModeMenu( TRUE );
            WdeTestMode = TRUE;
            WdeTestResource = info;
        } else {
            WdeSetStatusReadyText();
        }
    }

    return( ret );
}

bool WdeIsTestMessage( MSG *msg )
{
    if( WdeTestMode && WdeTestDialogHandle != NULL ) {
        return( IsDialogMessage( WdeTestDialogHandle, msg ) != 0 );
    } else {
        return( false );
    }
}

bool WdeGetTestMode( void )
{
    return( WdeTestMode );
}

static bool WdeTestCurrentObject( void )
{
    OBJPTR              obj;
    WdeOrderMode        mode;
    bool                ret;

    ret = ((obj = WdeGetCurrentDialog()) != NULL);

    if( ret ) {
        ret = ( Forward( obj, GET_ORDER_MODE, &mode, NULL ) != 0 );
    }

    if( ret ) {
        if( mode != WdeSelect ) {
            Forward( obj, SET_ORDER_MODE, &mode, NULL );
        }
    }

    if( ret ) {
        ret = ( Forward( obj, TEST, NULL, NULL ) != 0 );
    }

    return( ret );
}

bool WdeSetTestControlDefaults( HWND dialog )
{
    WNDENUMPROC wndenumproc;
    bool        ret;

    if( dialog != NULL ) {
        wndenumproc = MakeProcInstance_WNDENUM( WdeSetControlEnumProc, WdeGetAppInstance() );
        ret = ( EnumChildWindows( dialog, wndenumproc, 0 ) != 0 );
        FreeProcInstance_WNDENUM( wndenumproc );
    }
    return( ret );
}

BOOL CALLBACK WdeSetControlEnumProc( HWND win, LPARAM ret )
{
    /* touch unused var to get rid of warning */
    _wde_touch( ret );

    WdeSetDefaultTestControlEntries( win );

    return( TRUE );
}

static bool WdeSetDefaultTestControlEntries( HWND win )
{
    int         index;
    char        *text;
    char        *str;
    char        cname[20];
    int         len;
    uint_8      class;

    len = GetClassName( win, cname, sizeof( cname ) );
    if( len == 0 ) {
        return( TRUE );
    }

    class = WdeGetClassFromClassName( cname );

    if( class == CLASS_LISTBOX || class == CLASS_COMBOBOX ) {
        str = NULL;
        text = WdeAllocRCString( WDE_TESTITEM );
        if( text != NULL ) {
            str = (char *)WRMemAlloc( strlen( text ) + 10 + 1 );
            if( str == NULL ) {
                WdeFreeRCString( text );
                return( TRUE );
            }
        } else {
            return( TRUE );
        }
        SendMessage( win, WM_SETREDRAW, FALSE, 0 );
        for( index = 1; index <= TEST_DEFAULT_ENTRIES; index++ ) {
            sprintf( str, text, index );
            if( class == CLASS_LISTBOX ) {
                SendMessage( win, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)str );
            } else {
                SendMessage( win, CB_ADDSTRING, 0, (LPARAM)(LPCSTR)str );
            }
        }
        SendMessage( win, WM_SETREDRAW, TRUE, 0 );
        InvalidateRect( win, NULL, TRUE );
        WRMemFree( str );
        WdeFreeRCString( text );
    }

    return( TRUE );
}

INT_PTR CALLBACK WdeTestDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    bool ret;

    /* touch unused var to get rid of warning */
    _wde_touch( wParam );
    _wde_touch( lParam );

    ret = false;

    switch( message ) {
#if defined( __WINDOWS__ ) || defined( __NT__ ) && !defined( _WIN64 )
    case WM_DLGBORDER:
        if( lParam ) {
            *((int *)lParam) = CTL3D_BORDER;
            if( !WdeGetOption( WdeOptUse3DEffects ) ) {
                *((int *)lParam) = CTL3D_NOBORDER;
            }
        }
        ret = true;
        break;
    case WM_DLGSUBCLASS:
        if( lParam ) {
            *((int *)lParam) = CTL3D_SUBCLASS;
            if( !WdeGetOption( WdeOptUse3DEffects ) ) {
                *((int *)lParam) = CTL3D_NOSUBCLASS;
            }
        }
        ret = true;
        break;
#endif
    case WM_INITDIALOG:
        if( WdeGetOption( WdeOptUse3DEffects ) ) {
            WdeCtl3dSubclassDlgAll( hWnd );
        }
        WdeTestDialogHandle = hWnd;
        break;

    case WM_DESTROY:
        WdeTestDialogHandle = NULL;
        break;

    case WM_COMMAND:
        switch( LOWORD( wParam ) ) {
        case IDOK:
        case IDCANCEL:
            PostMessage( WdeGetMainWindowHandle(), WM_COMMAND,
                         IDM_TEST_MODE, MAKELONG( 0, 0 ) );
            break;
        }
        break;

    case WM_CLOSE:
        /* send a IDM_TEST message to the parent to toggle test mode off */
        PostMessage( WdeGetMainWindowHandle(), WM_COMMAND,
                     IDM_TEST_MODE, MAKELONG( 0, 0 ) );
        break;
    }

    return( ret );
}
