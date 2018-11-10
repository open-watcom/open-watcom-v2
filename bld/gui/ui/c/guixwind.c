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
* Description:  GUI library message loop.
*
****************************************************************************/


#include "guiwind.h"
#include <string.h>
#ifdef __WATCOMC__
    #include <process.h>
#endif
#include <stdlib.h>
#include "guix.h"
#include "guiscale.h"
#include "guixloop.h"
#include "guixutil.h"
#include "guicolor.h"
#include "guixmain.h"
#include "guisysfi.h"
#include "guimenu.h"
#include "guiwhole.h"
#include "guiwnclr.h"
#include "guihook.h"
#include "guiutil.h"
#include "guigadgt.h"
#include "guizlist.h"
#include "guistr.h"
#include "guiextnm.h"
#include "guiuiev.h"
#include "guiev.h"
#include "guixwind.h"

#include "clibext.h"


gui_window      *GUICurrWnd = NULL;

ui_event GUIAllEvents[] = {
    EV_FIRST_EVENT,     LAST_EVENT,
    FIRST_GUI_EVENT,    LAST_GUI_EVENT,
    __rend__,
    __end__
};

/*
 * GUIWndGetEvent -- get ad event (other than EV_NO_EVENT) from UI
 */

ui_event GUIWndGetEvent( VSCREEN * screen )
{
    ui_event    ui_ev;

    do {
        ui_ev = uivgetevent( screen );
        ui_ev = GUIUIProcessEvent( ui_ev );
    } while( ui_ev == EV_NO_EVENT );
    return( ui_ev );
}

void uistartevent( void )
{
    GUIStartEventProcessing();
}

void uidoneevent( void )
{
    GUIDoneEventProcessing();
}

/*
 * MessageLoop -- get events and process them
 */

static void MessageLoop( void )
{
    ui_event    ui_ev;

    uipushlist( GUIAllEvents );
    while( GUIGetFront() != NULL ) {
        if( GUICurrWnd != NULL ) {
            ui_ev = GUIWndGetEvent( &GUICurrWnd->screen );
        } else {
            ui_ev = GUIWndGetEvent( NULL );
        }
        if( !GUIProcessEvent( ui_ev ) ) {
            break;
        }
    }
    uipoplist( /* GUIAllEvents */ );
}

static void GUICleanupInit( bool intstringtable_cleanup,
                            bool loadstrings_cleanup )
{
    if( intstringtable_cleanup ) {
        GUIFiniInternalStringTable();
    }
    if( loadstrings_cleanup ) {
        GUILoadStrFini();
    }
}

void GUICleanup( void )
{
    GUIDeath();                 /* user replaceable stub function */
    uirefresh();
    uiswap();
    uirestorebackground();      /* must be after uiswap */
    GUICleanupHotSpots();
    GUICleanupInit( true, true );
    GUISysFini();
}

static bool LoadStrings( void )
{
    char        *resource_file_name;
    char        fname[_MAX_PATH];

    resource_file_name = GUIGetResFileName();
    if( resource_file_name != NULL ) {
        return( GUILoadStrInit( resource_file_name ) );
    } else {
        _cmdname( fname );
        if( fname[0] == '\0' )
            return( false );
        return( GUILoadStrInit( fname ) );
    }
}

static void MainLoop( void )
{
    bool        loadstrings_ok;
    bool        intstringtable_ok;
#ifdef __WINDOWS__
    SAREA       area;
#endif

    loadstrings_ok    = LoadStrings();
    intstringtable_ok = GUIInitInternalStringTable();
    if( loadstrings_ok  &&  intstringtable_ok ) {
        GUImain();
        if( GUIIsInit() ) {
#ifdef __WINDOWS__
            area.row = 0;
            area.col = 0;
            area.width = UIData->width;
            area.height = UIData->height;
            uidirty( area );
            uirefresh();
#endif

            MessageLoop();
            GUICleanup();
            loadstrings_ok    = false;
            intstringtable_ok = false;
        }
    }
    GUICleanupInit( intstringtable_ok, loadstrings_ok );
    GUIDead();                 /* user replaceable stub function */
}

/*
 * GUIXMain
 */

int GUIXMain( int argc, char * argv[] )
{
    GUIMainTouched = true;
    GUIMemOpen();
    if( GUIFirstCrack() ) {
        GUIStoreArgs( argv, argc );
        MainLoop();
    }
    GUIMemClose();
    return( 0 );
}

/*
 * GUIXSetupWnd - initializes the gui_window struture
 */

void GUIXSetupWnd( gui_window *wnd )
{
    wnd->screen.event = EV_NO_EVENT;
    wnd->screen.flags = V_UNFRAMED | V_GUI_WINDOW;
    wnd->screen.cursor = C_OFF;
    wnd->flags = CHECK_CHILDREN_ON_RESIZE;
    wnd->background = ' ';
}

bool GUISetBackgroundChar( gui_window *wnd, char background )
{
    wnd->background = background;
    return( true );
}

/*
 * GUIXCreateWindow - create a UI window
 */

bool GUIXCreateWindow( gui_window *wnd, gui_create_info *dlg_info,
                       gui_window *parent )
{
    if( parent != NULL ) {
        wnd->sibling = parent->child;
        parent->child = wnd;
        wnd->parent = parent;
    } else {
        if( (dlg_info->style & GUI_POPUP) == 0 ) {
            wnd->flags |= IS_ROOT;
        }
    }
    if( !GUISetupStruct( wnd, dlg_info, false ) ) {
        return( false );
    }
    GUIFrontOfList( wnd );
    GUISetIcon( wnd, dlg_info->icon );
    if( uivopen( &wnd->screen ) != NULL ) {
        if( dlg_info->style & GUI_INIT_MAXIMIZED ) {
            GUIMaximizeWindow( wnd );
        } else if( dlg_info->style & GUI_INIT_MINIMIZED ) {
            GUIMinimizeWindow( wnd );
        }
        if( dlg_info->style & GUI_INIT_INVISIBLE ) {
            uivhide( &wnd->screen );
        }
        if( wnd->vgadget != NULL ) {
            uiinitgadget( wnd->vgadget );
        }
        if( wnd->hgadget != NULL ) {
            uiinitgadget( wnd->hgadget );
        }
        if( !GUIEVENT( wnd, GUI_INIT_WINDOW, NULL ) ) {
            return( false );
        }
        GUIBringToFront( wnd );
        GUIWholeWndDirty( wnd );
        uisetmouse( wnd->screen.area.row, wnd->screen.area.col );
        return( true );
    } else {
        return( false );
    }
}

void GUIShowWindow( gui_window *wnd )
{
    uivshow( &wnd->screen );
}

void GUIShowWindowNA( gui_window *wnd )
{
    GUIShowWindow( wnd );
}

bool GUIIsFirstInstance( void )
{
    return( true );
}
