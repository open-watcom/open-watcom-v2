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
#include "guixutil.h"
#include "guihook.h"
#include "guixwind.h"
#include "guizlist.h"

extern gui_window *GUICurrWnd;

void GUIBringToFront( gui_window * wnd )
{
    gui_window  *curr;
    bool        change;
    gui_window  *old_curr_wnd;

    change = GUICurrWnd != wnd;
    if( wnd != NULL ) {
        if( ( GUICurrWnd != NULL ) && change ) {
            GUIEVENTWND( GUICurrWnd, GUI_NOT_ACTIVE, NULL );
            old_curr_wnd = GUICurrWnd;
            GUICurrWnd = wnd;
            old_curr_wnd->flags |= NON_CLIENT_INVALID;
            GUIWndUpdate( old_curr_wnd );
        } else {
            GUICurrWnd = wnd;
        }
        if( change ) {
            if( GUIEVENTWND( GUICurrWnd, GUI_NOW_ACTIVE, NULL ) ) {
                return;
            }
            GUIFrontOfList( GUICurrWnd );
        }
        if( GUIIsOpen( GUICurrWnd ) ) {
            uivsetactive( &GUICurrWnd->screen );
        }
        for( curr = GUICurrWnd->child; curr != NULL; curr = curr->sibling ) {
            if( GUIIsOpen( curr ) ) {
                uivsetactive( &curr->screen );
            }
        }
        if( change ) {
            GUICurrWnd->flags |= NON_CLIENT_INVALID;
            GUIMDIBroughtToFront( GUICurrWnd );
            GUIWndUpdate( GUICurrWnd );
        }
    }
}

void GUIActivateNC( gui_window *wnd, bool activate )
{
    wnd=wnd;
    activate=activate;
}

