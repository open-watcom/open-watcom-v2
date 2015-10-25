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


#include <stdlib.h>
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
    #include <process.h>
#endif
#include "guiwind.h"
#include "guix.h"
#include "guiutil.h"
#include "guixutil.h"
#include "guicontr.h"
#include "guiwhole.h"
#include "guiscale.h"
#include "guimenu.h"
#include "guiwnclr.h"
#include "guistat.h"
#include "guihook.h"
#include "guizlist.h"
#include "guixwind.h"
#include "guixloop.h"
#include "guiev.h"
#include "guiuiev.h"
#include "guistr.h"
#include "guisysfi.h"
#include "guiextnm.h"

#include "clibext.h"

gui_window      *GUICurrWnd     =       NULL;

EVENT GUIAllEvents[] = {
    EV_FIRST_EVENT, LAST_EVENT,
    FIRST_GUI_EVENT, LAST_GUI_EVENT,
    EV_NO_EVENT,
    EV_NO_EVENT
};


/*
 * GUIXSetupWnd - initializes the gui_window struture
 */

void GUIXSetupWnd( gui_window *wnd )
{
    wnd->screen.event = EV_NO_EVENT;
    wnd->screen.flags = V_UNFRAMED | V_NO_ZOOM | V_GUI_WINDOW;
    wnd->screen.cursor = C_OFF;
    wnd->flags = CHECK_CHILDREN_ON_RESIZE;
    wnd->background = ' ';
}

bool GUISetBackgroundChar( gui_window *wnd, char background )
{
    wnd->background = background;
    return( true );
}

static void DeleteChild( gui_window * parent, gui_window * child )
{
    gui_window * curr;
    gui_window * prev;

    prev = NULL;
    for( curr = parent->child; curr != NULL; prev = curr, curr=curr->sibling ) {
        if( curr == child ) break;
    }
    if( curr != NULL ) {
        if( prev != NULL ) {
            prev->sibling = curr->sibling;
        } else {
            parent->child = curr->sibling;
        }
    }
}

void GUIWantPartialRows( gui_window *wnd, bool want )
{
    wnd=wnd;
    want=want;
}

void GUIFreeWindowMemory( gui_window *wnd, bool from_parent, bool dialog )
{
    gui_window  *curr_child;
    gui_window  *next_child;
    gui_window  *front;

    GUIDeleteFromList( wnd );
    if( GUIHasToolBar( wnd ) ) {
        GUICloseToolBar( wnd );
    }
    if( GUIHasStatus( wnd ) ) {
        GUIFreeStatus( wnd );
    }
    if( ( wnd->parent != NULL ) && ( !from_parent ) ) {
        DeleteChild( wnd->parent, wnd );
    }
    if( !dialog ) {
        GUIMDIDelete( wnd );
    }
    front = GUIGetFront();
    if( !dialog && !from_parent && ( front != NULL ) ) {
        GUIBringToFront( front );
    }
    GUIFreeAllControls( wnd );
    for( curr_child = wnd->child; curr_child != NULL; curr_child = next_child ) {
        next_child = curr_child->sibling;
        if( curr_child != NULL ) {
            GUIEVENTWND( curr_child, GUI_DESTROY, NULL );
            GUIFreeWindowMemory( curr_child, true, dialog );
        }
    }
    if( wnd->hgadget != NULL ) {
        uifinigadget( wnd->hgadget );
        GUIMemFree( wnd->hgadget );
    }
    if( wnd->vgadget != NULL ) {
        uifinigadget( wnd->vgadget );
        GUIMemFree( wnd->vgadget );
    }
    GUIFreeMenus( wnd );
    GUIFreeHint( wnd );
    GUIMemFree( wnd->icon_name );
    if( !dialog ) {
        uivshow( &wnd->screen );
        wnd->screen.open = true;
        uivclose( &wnd->screen );
    }
    if( GUICurrWnd == wnd ) {
        GUICurrWnd = NULL;
    }
    if( wnd->screen.dynamic_title ) {
        GUIMemFree( (void *)wnd->screen.title );
        wnd->screen.title = NULL;
        wnd->screen.dynamic_title = false;
    }
    GUIFreeColours( wnd );
    GUIMemFree( wnd );
}

static void DoDestroy( gui_window * wnd, bool dialog )
{
    if( wnd != NULL ) {
        GUIEVENTWND( wnd, GUI_DESTROY, NULL );
        GUIFreeWindowMemory( wnd, false, dialog );
    } else {
        wnd = GUIGetFront();
        while( wnd != NULL ) {
            DoDestroy( wnd, GUI_IS_DIALOG( wnd ) );
            wnd = GUIGetFront();
        }
    }
}

void GUIDestroyDialog( gui_window * wnd )
{
    DoDestroy( wnd, true );
}

bool GUICloseWnd( gui_window *wnd )
{
    if( wnd != NULL ) {
        if( GUIEVENTWND( wnd, GUI_CLOSE, NULL ) ) {
            GUIDestroyWnd( wnd );
            return( true );
        }
    }
    return( false );
}

/*
 * GUIDestroyWnd
 */

void GUIDestroyWnd( gui_window * wnd )
{
    DoDestroy( wnd, false );
}

/*
 * GUIGetRow - get the row that the mouse is on
 */

gui_ord GUIGetRow( gui_window * wnd, gui_point * in_pt )
{
    gui_point pt;

    wnd = wnd;
    pt = *in_pt;
    GUIScaleToScreenRPt( &pt );
    if( pt.y >=0 ) {
        return( (gui_ord) pt.y );
    } else {
        return( GUI_NO_ROW );
    }
}

/*
 * GUIGetCol - get the column that the mouse is on
 */

gui_ord GUIGetCol( gui_window *wnd, const char *text, gui_point *in_pt )
{
    gui_point pt;

    wnd = wnd;
    text = text;
    pt = *in_pt;
    GUIScaleToScreenRPt( &pt );
    if( pt.x >=0 ) {
        return( (gui_ord) pt.x );
    } else {
        return( GUI_NO_COLUMN );
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

void GUIHideWindow( gui_window *wnd )
{
    uivhide( &wnd->screen );
}

bool GUIIsWindowVisible( gui_window *wnd )
{
    return( ( wnd->screen.flags & V_HIDDEN ) == 0 );
}

bool GUIIsFirstInstance( void )
{
    return( true );
}
/*
 * GUIWndGetEvent -- get ad event (other than EV_NO_EVENT) from UI
 */

EVENT GUIWndGetEvent( VSCREEN * screen )
{
    EVENT ev;

    do {
        ev = uivgetevent( screen );
        ev = GUIUIProcessEvent( ev );
    } while( ev == EV_NO_EVENT );
    return( ev );
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
    EVENT ev;

    uipushlist( GUIAllEvents );
    while( GUIGetFront() != NULL ) {
        if( GUICurrWnd != NULL ) {
            ev = GUIWndGetEvent( &GUICurrWnd->screen );
        } else {
            ev = GUIWndGetEvent( NULL );
        }
        if( !GUIProcessEvent( ev ) ) {
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
    char *      resource_name;
    char        fname[_MAX_PATH];

    resource_name = GUIGetExtName();
    if( resource_name != NULL ) {
        return( GUILoadStrInit( resource_name ) );
    } else {
        _cmdname( fname );
        if( fname == NULL ) return( false );
        return( GUILoadStrInit( fname ) );
    }
}

static void MainLoop( void )
{
    bool        loadstrings_ok;
    bool        intstringtable_ok;
    #ifdef __WINDOWS__
        SAREA area;
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
 * GUIXCreateWindow - create a UI window
 */

bool GUIXCreateWindow( gui_window *wnd, gui_create_info *dlg_info, gui_window *parent )
{
    if( parent != NULL ) {
        wnd->sibling = parent->child;
        parent->child = wnd;
        wnd->parent = parent;
    } else {
        if( !( dlg_info->style & GUI_POPUP ) ) {
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
        if( !GUIEVENTWND( wnd, GUI_INIT_WINDOW, NULL ) ) {
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
