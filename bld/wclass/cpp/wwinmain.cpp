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


#include "wwinmain.hpp"


extern "C" void GUImain() {
/*************************/

    wclassmain();
}


void    InitGUI( coordinate_system coord_system, gui_ord w, gui_ord h,
                 gui_rect *screen ) {
/***********************************/

    gui_coord   scale;

    // 300 uS mouse double click rate, graphics mouse
    GUIWndInit( 300, GUI_GMOUSE );
    GUI3DDialogInit();

    if( coord_system == COORD_SCREEN ) {
        GUIGetScreen( screen );
    } else if( coord_system == COORD_VIRTUAL ) {
        scale.x = w;
        scale.y = h;
        GUIGetRoundScale( &scale );
        screen->x = 0;
        screen->y = 0;
        screen->width = scale.x;
        screen->height = scale.y;
    } else { // coord_system == COORD_USERS
        screen->x = 0;
        screen->y = 0;
        screen->width = w;
        screen->height = h;
    }
    GUISetScale( screen );
}
