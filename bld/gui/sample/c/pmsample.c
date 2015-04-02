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


#include "gui.h"
#include "guitypes.h"

static  gui_rect        Scale           = { 0, 0, 1000, 1000 };

bool MainEventWnd( gui_window *gui, gui_event gui_ev, void *param )
{
    gui = gui;
    gui_ev = gui_ev;
    param = param;
    return( true );
}

static gui_create_info Parent = {
    "Sample Application",
    { 250, 250, 500, 500 },
    GUI_HSCROLL | GUI_VSCROLL,
    GUI_GADGETS | GUI_VISIBLE,
    NULL,
    0, NULL,
    0, NULL,
    &MainEventWnd,
    NULL,
    NULL,
    NULL                                // Menu Resource
};

static gui_create_info Child = {
    "Child Window",
    { 300, 300, 200, 200 },
    GUI_SCROLL_BOTH,
    GUI_VISIBLE+GUI_CLOSEABLE+GUI_MAXIMIZE+GUI_RESIZEABLE+GUI_MINIMIZE,
    NULL,
    0,
    NULL,
    0, NULL,
    &MainEventWnd,
    NULL,
    NULL,
    NULL                                // Menu Resource
};

void GUImain( void )
{
    GUIMemOpen();
    GUIWndInit( 300, GUI_GMOUSE );
    GUISetScale( &Scale );
    Child.parent = GUICreateWindow( &Parent );
    GUICreateWindow( &Child );
}
