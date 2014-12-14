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


#include "guiwind.h"
#include "guix.h"
#include "guixutil.h"
#include "guicontr.h"
#include "guiwhole.h"
#include "guiscale.h"
#include "guimenu.h"
#include "guiwnclr.h"
#include "guistat.h"
#include "guihook.h"
#include "guizlist.h"

/* includes from guixmain.c */
extern gui_window *GUICurrWnd;

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
//    GUIMemFree( wnd->screen.title );
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

gui_ord GUIGetCol( gui_window * wnd, char * text, gui_point * in_pt )
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
