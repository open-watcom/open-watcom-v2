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
* Description:  WHEN YOU FIGURE OUT WHAT THIS MODULE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/



#include "auipvt.h"

void WndSetColours( a_window *wnd, int num_colours, gui_colour_set *colours )
{
    gui_colour_set      *main_colours;
    gui_colour_set      back;

    back = colours[ GUI_BACKGROUND ];
    if( wnd == WndMain ) {
        main_colours = GUIGetWindowColours( WndMain->gui );
        colours[ GUI_BACKGROUND ] = main_colours[ GUI_BACKGROUND ];
        GUIMemFree( main_colours );
    }
    GUISetWindowColours( wnd->gui, num_colours, colours );
    colours[ GUI_BACKGROUND ] = back;
}

void WndBackGround( gui_colour colour )
{
    gui_colour_set      *colours;

    colours = GUIGetWindowColours( WndMain->gui );
    colours[GUI_BACKGROUND].fore = colour;
    colours[GUI_BACKGROUND].back = colour;
    GUISetWindowColours( WndMain->gui, WND_FIRST_UNUSED, colours );
    GUIMemFree( colours );
}

