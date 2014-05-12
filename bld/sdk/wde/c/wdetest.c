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


#include "wdeglbl.h"
#include <stdio.h>
#include "wderes.h"
#include "wdemain.h"
#include "wdefmenu.h"
#include "wdefordr.h"
#include "wde_wres.h"
#include "wdedebug.h"
#include "wdestat.h"
#include "wdemsgbx.h"
#include "rcstr.gh"
#include "wdeopts.h"
#include "wdectl3d.h"
#include "wde_rc.h"
#include "wdetest.h"

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT bool CALLBACK WdeSetControlEnumProc( HWND, LPARAM );

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
        WdeSetStatusText( NULL, "", FALSE );
        WdeSetStatusByID( WDE_TESTMODE, -1 );
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

BOOL WdeIsTestMessage( MSG *msg )
{
    if( WdeTestMode && WdeTestDialogHandle != NULL ) {
        return( IsDialogMessage( WdeTestDialogHandle, msg ) );
    } else {
        return( FALSE );
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
        ret = Forward( obj, GET_ORDER_MODE, &mode, NULL );
    }

    if( ret ) {
        if( mode != WdeSelect ) {
            Forward( obj, SET_ORDER_MODE, &mode, NULL );
        }
    }

    if( ret ) {
        ret = Forward( obj, TEST, NULL, NULL );
    }

    return( ret );
}

bool WdeSetTestControlDefaults( HWND dialog )
{
    WNDENUMPROC child_proc;
    bool        ret;

    if( dialog != NULL ) {
        child_proc = (WNDENUMPROC)MakeProcInstance( (FARPROC)WdeSetControlEnumProc,
                                                    WdeGetAppInstance() );
        ret = EnumChildWindows( dialog, child_proc, 0 );
        FreeProcInstance( (FARPROC)child_proc );
    }

    return( ret );
}

WINEXPORT bool CALLBACK WdeSetControlEnumProc( HWND win, LPARAM ret )
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

    len = GetClassName( win, cname, 19 );
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
                SendMessage( win, LB_ADDSTRING, 0, (LPARAM)(LPSTR)str );
            } else {
                SendMessage( win, CB_ADDSTRING, 0, (LPARAM)(LPSTR)str );
            }
        }
        SendMessage( win, WM_SETREDRAW, TRUE, 0 );
        InvalidateRect( win, NULL, TRUE );
        WRMemFree( str );
        WdeFreeRCString( text );
    }

    return( TRUE );
}

WINEXPORT BOOL CALLBACK WdeTestDlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    int msg_processed;

    /* touch unused var to get rid of warning */
    _wde_touch( wParam );
    _wde_touch( lParam );

    msg_processed = FALSE;

    switch( message ) {
    case WM_DLGBORDER:
        if( lParam ) {
            *((int *)lParam) = CTL3D_BORDER;
            if( !WdeGetOption( WdeOptUse3DEffects ) ) {
                *((int *)lParam) = CTL3D_NOBORDER;
            }
        }
        msg_processed = TRUE;
        break;
    case WM_DLGSUBCLASS:
        if( lParam ) {
            *((int *)lParam) = CTL3D_SUBCLASS;
            if( !WdeGetOption( WdeOptUse3DEffects ) ) {
                *((int *)lParam) = CTL3D_NOSUBCLASS;
            }
        }
        msg_processed = TRUE;
        break;
    case WM_INITDIALOG:
        if( WdeGetOption( WdeOptUse3DEffects ) ) {
            WdeCtl3dSubclassDlg( hWnd, CTL3D_ALL );
        }
        WdeTestDialogHandle = hWnd;
        msg_processed = FALSE;
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

    return( msg_processed );
}
