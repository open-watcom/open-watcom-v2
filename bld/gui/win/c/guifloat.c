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
#include "guiscale.h"
#include "guixutil.h"
#include "guimenus.h"
#include "guifloat.h"
#include "guistr.h"

#define  NO_SELECT      -1

extern  HMENU           GUIHFloatingPopup;
extern  int             CurrItem        = NO_SELECT;
extern  WPI_INST        GUIMainHInst;
static  bool            InitComplete    = FALSE;

bool GUITrackFloatingPopup( gui_window *wnd, gui_point *location,
                            gui_mouse_track track, int *curr_item )
{
    WPI_POINT   pt;
    ULONG       flags;
    GUI_RECTDIM left, top, right, bottom;
    HMENU       popup;

    if( ( popup = GUIHFloatingPopup ) == (HMENU)NULL ) {
        return( FALSE );
    }

    GUIScaleToScreenRPt( location );
    _wpi_getrectvalues( wnd->hwnd_client, &left, &top, &right, &bottom );
    location->x += left;
    location->y += top;
    if( GUI_DO_HSCROLL( wnd ) ) {
        location->x -= GUIGetScrollPos( wnd, SB_HORZ );
    }
    if( GUI_DO_VSCROLL( wnd ) ) {
        location->y -= GUIGetScrollPos( wnd, SB_VERT );
    }

    CurrItem = NO_SELECT;
    if( ( curr_item != NULL ) && ( *curr_item != 0 ) ) {
        CurrItem = *curr_item;
    }

    location->y = _wpi_cvth_y( location->y, (bottom - top) );

    pt.x = location->x;
    pt.y = location->y;

    _wpi_mapwindowpoints( wnd->hwnd, HWND_DESKTOP, &pt, 1 );

    flags = TPM_LEFTALIGN;
    if( track & GUI_TRACK_LEFT ) {
        flags |= TPM_LEFTBUTTON;
    }
    if( track & GUI_TRACK_RIGHT ) {
        flags |= TPM_RIGHTBUTTON;
    }
    InitComplete = FALSE;

    GUIFlushKeys();

    _wpi_trackpopupmenu( popup, flags, pt.x, pt.y, wnd->hwnd_frame );

    _wpi_destroymenu( popup );

    GUIHFloatingPopup = NULLHANDLE;

    if( ( CurrItem != NO_SELECT ) && ( curr_item != NULL ) ) {
        *curr_item = CurrItem;
    }
    CurrItem = NO_SELECT;
    GUIDeleteFloatingPopups( wnd );
    return( TRUE );
}

/*
 * GUIXCreateFloatingPopup -- create a floating popup menu
 */

bool GUIXCreateFloatingPopup( gui_window *wnd, gui_point *location,
                             int num, gui_menu_struct *menu,
                             gui_mouse_track track, int *curr_item )
{
    if( GUIHFloatingPopup != NULLHANDLE ) {
        _wpi_destroymenu( GUIHFloatingPopup );
        GUIHFloatingPopup = NULLHANDLE;
    }

    GUIHFloatingPopup = GUICreateSubMenu( wnd, num, menu, FLOAT_HINT );
    if( GUIHFloatingPopup == NULLHANDLE ) {
        GUIError( LIT( Open_Failed ) );
        return( FALSE );
    }

    return( GUITrackFloatingPopup( wnd, location, track, curr_item ) );
}

void GUIPopupMenuSelect( WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    int                 menu_closed;
    unsigned            id;
    int                 is_hilite;
#ifdef __OS2_PM__
    WPI_MENUSTATE       mstate;
    HMENU               hmenu;
#endif

    lparam=lparam;
    id = GET_WM_MENUSELECT_ITEM( wparam, lparam );
    menu_closed = _wpi_is_close_menuselect( wparam, lparam );

#ifndef __OS2_PM__
    is_hilite = ( GET_WM_MENUSELECT_FLAGS( wparam, lparam ) & MF_HILITE );
#else
    hmenu = (HMENU) lparam;
    if( !menu_closed &&
        !WinSendMsg( hmenu, MM_QUERYITEM, MPFROM2SHORT(id, TRUE),
                     MPFROMP(&mstate) ) ) {
        return;
    }
    is_hilite = ( mstate.afAttribute & MF_HILITE );
#endif

    if( menu_closed ) {
        CurrItem = NO_SELECT;
    } else {
        if( !InitComplete ) {
            InitComplete = TRUE;
        } else {
            if( is_hilite ) {
                CurrItem = id;
            }
        }
    }
}

