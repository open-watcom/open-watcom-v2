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

int GUIGetHScrollCol( gui_window * wnd )
{
    if( wnd->hgadget == NULL ) {
        return( 0 );
    } else {
        return( wnd->hgadget->pos );
    }
}

int GUIGetVScrollRow( gui_window * wnd )
{
    if( wnd->vgadget == NULL ) {
        return( 0 );
    } else {
        return( wnd->vgadget->pos );
    }
}

gui_ord GUIGetHScroll( gui_window * wnd )
{
    gui_coord coord;

    if( wnd->hgadget == NULL ) {
        return( 0 );
    } else {
        coord.x = wnd->hgadget->pos;
        GUIScreenToScaleR( &coord );
        return( coord.x );
    }
}

gui_ord GUIGetVScroll( gui_window * wnd )
{
    gui_coord coord;

    if( wnd->vgadget == NULL ) {
        return( 0 );
    } else {
        coord.y = wnd->vgadget->pos;
        GUIScreenToScaleR( &coord );
        return( coord.y );
    }
}
