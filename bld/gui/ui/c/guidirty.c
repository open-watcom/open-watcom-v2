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
#include "guixdraw.h"
#include "guiwhole.h"

/*
 * GUIWndDirty -- tell the user interface that the screen contents are bad
 */

void GUIWndDirty( gui_window * wnd )
{
    SAREA screen;

    if( wnd != NULL ) {
        wnd->flags |= CONTENTS_INVALID;
        COPYAREA( wnd->use, wnd->dirty );
        wnd->flags &= ~NEEDS_RESIZE_REDRAW;
        GUIWndUpdate( wnd );
    } else {
        for( wnd = GUIGetFront(); wnd != NULL; wnd = GUIGetNextWindow( wnd ) ) {
            GUIWholeWndDirty( wnd );
        }
        screen.row = 0; /* leave this 0! */
        screen.col = 0;
        screen.height = YMAX;
        screen.width = XMAX;
        uidirty( screen );
    }
    uirefresh();
}
