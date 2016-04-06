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


bool GUICreateFloatToolBar( gui_window *wnd, bool fixed,
                                   gui_ord height, int num_items,
                                   gui_toolbar_struct *toolinfo, bool excl,
                                   gui_colour_set *plain,
                                   gui_colour_set *standout, gui_rect *rect )
{
    if( ( num_items == 0 ) || ( toolinfo == NULL ) ) {
        return( false );
    }
    if( GUIXCreateToolBar( wnd, fixed, height, num_items, toolinfo, excl,
                           plain, standout, rect ) ) {
        GUIInitToolbarHint( wnd, num_items, toolinfo );
        return( true );
    }
    return( false );
}

bool GUICreateToolBar( gui_window *wnd, bool fixed, gui_ord height,
                              int num_items, gui_toolbar_struct *toolinfo,
                              bool excl, gui_colour_set *plain,
                              gui_colour_set *standout )
{
    if( ( num_items == 0 ) || ( toolinfo == NULL ) ) {
        return( false );
    }
    if( GUIXCreateToolBar( wnd, fixed, height, num_items, toolinfo, excl,
                           plain, standout, NULL ) ) {
        GUIInitToolbarHint( wnd, num_items, toolinfo );
        return( true );
    }
    return( false );
}

bool GUICreateToolBarWithTips( gui_window *wnd, bool fixed, gui_ord height,
                                      int num_items, gui_toolbar_struct *toolinfo,
                                      bool excl, gui_colour_set *plain,
                                      gui_colour_set *standout )
{
    if( ( num_items == 0 ) || ( toolinfo == NULL ) ) {
        return( false );
    }
    if( GUIXCreateToolBarWithTips( wnd, fixed, height, num_items, toolinfo, excl,
                                   plain, standout, NULL, true ) ) {
        GUIInitToolbarHint( wnd, num_items, toolinfo );
        return( true );
    }
    return( false );
}

bool GUICloseToolBar( gui_window *wnd )
{
    if( GUIXCloseToolBar( wnd ) ) {
        GUIInitToolbarHint( wnd, 0, NULL );
        return( true );
    }
    return( false );
}
