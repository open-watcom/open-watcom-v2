/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
#include "guikey.h"
#include "guixloop.h"
#include "guixkey.h"
#include "guixwind.h"


bool GUIProcessAltMenuEvent( ui_event ui_ev )
{
    gui_key     key;
    gui_window  *wnd;
    bool        top;

    if( GUICurrWnd != NULL ) {
        key = (gui_key)ui_ev;
        wnd = NULL;
        top = ( GUICurrWnd->parent == NULL ) || (GUICurrWnd->parent->style & GUI_VISIBLE) == 0;
        if( ui_ev == EV_ALT_SPACE ) {
            if( top ) {
                wnd = GUICurrWnd;
            } else {
                wnd = GUICurrWnd->parent;
            }
        }
        if( ( key == GUI_KEY_ALT_MINUS ) && !top ) {
            wnd = GUICurrWnd;
        }
        if( ( wnd != NULL ) && (wnd->style & GUI_VISIBLE) ) {
            guix_coord  scr_point;

            scr_point.x = wnd->vs.area.col;
            scr_point.y = wnd->vs.area.row;
            GUICreatePopup( wnd, &scr_point );
            return( true );
        }
    }
    return( false );
}
