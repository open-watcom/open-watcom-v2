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
#include "guimenus.h"
#include "guixutil.h"
#include "guihook.h"

#define NUM_SYSTEM_MENUS        9

static void FreeSystemMenu( gui_window *wnd )
{
    int         num;
    HMENU       system;
    int         i;
    HWND        frame;

    frame = GUIGetParentFrameHWND( wnd );
    if( ( _wpi_getparent( frame ) != HWND_DESKTOP ) &&
        ( wnd->style & GUI_SYSTEM_MENU ) ) {
        system = _wpi_getsystemmenu( frame );
        if( system != NULLHANDLE ) {
            num = (int) _wpi_getmenuitemcount( system );
            for( i = num; i >= NUM_SYSTEM_MENUS; i-- ) {
                _wpi_deletemenu( system, i, TRUE );
            }
        }
    }
}

bool GUIResetMenus( gui_window *wnd, int num_menus, gui_menu_struct *menu )
{
    HMENU       hmenu;
    bool        success;
    HWND        parent;
    HWND        frame;

    GUIInitHint( wnd, 0, NULL, MENU_HINT );
    success = FALSE;
    frame = GUIGetParentFrameHWND( wnd );
    parent = _wpi_getparent( frame );
    if( ( parent == HWND_DESKTOP ) || ( wnd->style & GUI_POPUP ) ) {
        if( menu == NULL ) {
            GUISetMenu( wnd, NULL );
            GUIFreePopupList( wnd );
        } else {
            if( GUICreateMenus( wnd, num_menus, menu, &hmenu ) ) {
                GUISetMenu( wnd, hmenu );
                _wpi_drawmenubar( frame );
                success = TRUE;
            }
        }
    } else {
        FreeSystemMenu( wnd );
        success = GUIAddToSystemMenu( wnd, frame, num_menus, menu, wnd->style );
    }
    if( success ) {
        GUIMDIResetMenus( wnd, wnd->parent, num_menus, menu );
        GUIInitHint( wnd, num_menus, menu, MENU_HINT );
    }
    return( success );
}
