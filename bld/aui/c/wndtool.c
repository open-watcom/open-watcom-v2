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
* Description:  AUI toolbar window.
*
****************************************************************************/


#include "auipvt.h"

gui_ord ToolHeight;
static gui_event ToolEvent = GUI_TOOLBAR_DESTROYED;

extern  gui_colour_set  WndColours[];

void    WndCreateToolBar( gui_ord height, bool fixed,
                          int items, gui_toolbar_struct *tools )
{
    if( GUIHasToolBar( WndMain->gui ) ) GUICloseToolBar( WndMain->gui );
    GUICreateToolBar( WndMain->gui, fixed, height,
                      items, tools, TRUE, &WndColours[ GUI_MENU_PLAIN ],
                      &WndColours[ GUI_MENU_STANDOUT ] );
    ToolHeight = height;
    WndSetToolBar( fixed ? GUI_TOOLBAR_FIXED : GUI_TOOLBAR_FLOATING );
}

void WndCreateToolBarWithTips( gui_ord height, bool fixed, int items,
                               gui_toolbar_struct *tools )
{
    if( GUIHasToolBar( WndMain->gui ) ) {
        GUICloseToolBar( WndMain->gui );
    }
    GUICreateToolBarWithTips( WndMain->gui, fixed, height, items, tools, TRUE,
                              &WndColours[GUI_MENU_PLAIN],
                              &WndColours[GUI_MENU_STANDOUT] );
    ToolHeight = height;
    WndSetToolBar( fixed ? GUI_TOOLBAR_FIXED : GUI_TOOLBAR_FLOATING );
}

bool WndHaveToolBar( void )
{
    return( GUIHasToolBar( WndMain->gui ) );
}

void    WndCloseToolBar( void )
{
    if( WndHaveToolBar() ) GUICloseToolBar( WndMain->gui );
}

gui_ord WndToolHeight( void )
{
    return( ToolHeight );
}

bool WndToolFixed( void )
{
    return( ToolEvent == GUI_TOOLBAR_FIXED );
}

void WndSetToolBar( gui_event event )
{
    gui_event   old;

    old = ToolEvent;
    ToolEvent = event;
    if( old != event ) {
        WndSetWndMax();
    }
}
