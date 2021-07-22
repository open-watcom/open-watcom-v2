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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "gui.h"
#include "guitypes.h"

static  gui_rect        Scale           = { 0, 0, 1000, 1000 };

bool MainWndGUIEventProc( gui_window *wnd, gui_event gui_ev, void *param )
{
    /* unused parameters */ (void)gui; (void)gui_ev; (void)param;

    return( true );
}

static gui_create_info Parent = {
    "Sample Application",
    { 250, 250, 500, 500 },
    GUI_HSCROLL | GUI_VSCROLL,
    GUI_GADGETS | GUI_VISIBLE,
    NULL,
    GUI_NO_MENU,                    // Menu array
    GUI_NO_COLOUR,                  // Colour attribute array
    &MainWndGUIEventProc,           // GUI Event Callback function
    NULL,                           // Extra
    NULL,                           // Icon
    NULL                            // Menu Resource
};

static gui_create_info Child = {
    "Child Window",
    { 300, 300, 200, 200 },
    GUI_SCROLL_BOTH,
    GUI_VISIBLE+GUI_CLOSEABLE+GUI_MAXIMIZE+GUI_RESIZEABLE+GUI_MINIMIZE,
    NULL,
    GUI_NO_MENU,                    // Menu array
    GUI_NO_COLOUR,                  // Colour attribute array
    &MainWndGUIEventProc,           // GUI Event Callback function
    NULL,                           // Extra
    NULL,                           // Icon
    NULL                            // Menu Resource
};

void GUIAPI GUImain( void )
{
    GUIWndInit( 300 /* ms */, GUI_GMOUSE );
    GUISetScale( &Scale );
    Child.parent = GUICreateWindow( &Parent );
    GUICreateWindow( &Child );
}
