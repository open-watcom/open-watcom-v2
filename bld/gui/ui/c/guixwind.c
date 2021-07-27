/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2018-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  GUI library main window procedure and other assorted guts
*
****************************************************************************/


#include "guiwind.h"
#include <string.h>
#ifdef __WATCOMC__
    #include <process.h>
#endif
#include <stdlib.h>
#include "guixloop.h"
#include "guixutil.h"
#include "guicolor.h"
#include "guixmain.h"
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

ui_event GUIWndGetEvent( VSCREEN *vs )
{
    ui_event    ui_ev;

    do {
        ui_ev = uivgetevent( vs );
        ui_ev = GUIUIProcessEvent( ui_ev );
    } while( ui_ev == EV_NO_EVENT );
    return( ui_ev );
}

/*
 * UI application stub
 */
void uistartevent( void )
{
    GUIStartEventProcessing();
}

/*
 * UI application stub
 */
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
            ui_ev = GUIWndGetEvent( &GUICurrWnd->vs );
        } else {
            ui_ev = GUIWndGetEvent( NULL );
        }
        if( !GUIProcessEvent( ui_ev ) ) {
            break;
        }
    }
    uipoplist( /* GUIAllEvents */ );
}

void GUIAPI GUICleanup( void )
{
    GUIDeath();                 /* user replaceable stub function */
    uirefresh();
    uiswap();
    uirestorebackground();      /* must be after uiswap */
    GUICleanupHotSpots();
    GUISysFini();               /* user replaceable stub function */
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
#ifdef __WINDOWS__
    SAREA       area;
#endif

    if( LoadStrings() ) {
        if( GUIInitInternalStringTable() ) {
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
            }
            GUIFiniInternalStringTable();
        }
        GUILoadStrFini();
    }
}

/*
 * GUIXMain
 */

int GUIXMain( int argc, char * argv[] )
{
    GUIMainTouched = true;
    GUIMemOpen();
    GUIStoreArgs( argv, argc );
    if( GUIFirstCrack() ) {         /* user replaceable stub function */
        MainLoop();
        GUIDead();                  /* user replaceable stub function */
    }
    GUIMemClose();
    return( 0 );
}

/*
 * GUIXSetupWnd - initializes the gui_window struture
 */

void GUIXSetupWnd( gui_window *wnd )
{
    uiscreeninit( &wnd->vs, NULL, V_GUIWINDOW );
    wnd->flags = CHECK_CHILDREN_ON_RESIZE;
    wnd->background = ' ';
}

bool GUIAPI GUISetBackgroundChar( gui_window *wnd, char background )
{
    wnd->background = background;
    return( true );
}

/*
 * GUIXCreateWindow - create a UI window
 */

bool GUIXCreateWindow( gui_window *wnd, gui_create_info *dlg_info, gui_window *parent_wnd )
{
    if( parent_wnd != NULL ) {
        wnd->sibling = parent_wnd->child;
        parent_wnd->child = wnd;
        wnd->parent = parent_wnd;
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
    if( uivopen( &wnd->vs ) != NULL ) {
        if( dlg_info->style & GUI_INIT_MAXIMIZED ) {
            GUIMaximizeWindow( wnd );
        } else if( dlg_info->style & GUI_INIT_MINIMIZED ) {
            GUIMinimizeWindow( wnd );
        }
        if( dlg_info->style & GUI_INIT_INVISIBLE ) {
            uivhide( &wnd->vs );
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
        GUIDirtyWhole( wnd );
        uisetmouse( wnd->vs.area.row, wnd->vs.area.col );
        return( true );
    } else {
        return( false );
    }
}

void GUIAPI GUIShowWindow( gui_window *wnd )
{
    uivshow( &wnd->vs );
}

void GUIAPI GUIShowWindowNA( gui_window *wnd )
{
    GUIShowWindow( wnd );
}

bool GUIAPI GUIIsFirstInstance( void )
{
    return( true );
}
