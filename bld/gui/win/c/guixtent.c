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
#include <string.h>
#include <stdlib.h>
#include "guiscale.h"
#include "guigetx.h"

static bool GetExtent( gui_window *wnd, const char *text, size_t length, gui_coord *coord )
{
    size_t    my_length;
    bool      got_new;

    if( wnd != NULL && text != NULL ) {
        got_new = GUIGetTheDC( wnd );
        my_length = strlen( text );
        if( length != (size_t)-1 ) {
            if( my_length > length ) {
                my_length = length;
            }
        }
        GUIGetTextExtent( wnd, text, my_length, &coord->x, &coord->y );
        if( got_new ) {
            GUIReleaseTheDC( wnd );
        }
        GUIScreenToScaleR( coord );
        return( true );
    }
    return( false );
}

gui_ord GUIGetExtentX( gui_window *wnd, const char *text, size_t length )
{
    gui_coord coord;

    if( GetExtent( wnd, text, length, &coord ) ) {
        return( coord.x );
    }
    return( 0 );
}

gui_ord GUIGetExtentY( gui_window *wnd, const char *text )
{
    gui_coord coord;

    if( GetExtent( wnd, text, (size_t)-1, &coord ) ) {
        return( coord.y );
    }
    return( 0 );
}

static bool GetControlInfo( gui_window *wnd, gui_ctl_id id, HWND *hwnd, WPI_PRES *dc )
{
    if ( wnd && wnd->hwnd ) {
        *hwnd = _wpi_getdlgitem( wnd->hwnd, id );
        if ( *hwnd != (HWND)NULL ) {
            *dc = _wpi_getpres( *hwnd );
            return( *dc != NULLHANDLE );
        }
    }
    return( false );
}

static bool GetControlExtent( gui_window *wnd, gui_ctl_id id, const char *text, size_t length, gui_coord *coord )
{
    int         my_length;
    WPI_PRES    dc;
    WPI_FONT    old;
    WPI_FONT    first;
    HWND        hwnd;

    if( text != NULL && GetControlInfo( wnd, id, &hwnd, &dc ) ) {
        my_length = strlen( text );
        if( length != (size_t)-1 ) {
            if( my_length > length ) {
                my_length = length;
            }
        }
        if( wnd->font != NULL ) {
            old = _wpi_selectfont( dc, wnd->font );
        } else {
            old = NULL;
        }
        _wpi_gettextextent( dc, text, my_length, &coord->x, &coord->y );
        if( old != NULL ) {
            first = _wpi_selectfont( dc, old );
        }
        _wpi_releasepres( hwnd, dc );
        GUIScreenToScaleR( coord );
        return( true );
    }
    return( false );
}

gui_ord GUIGetControlExtentX( gui_window *wnd, gui_ctl_id id, const char *text, size_t length )
{
    gui_coord   coord;

    if( GetControlExtent( wnd, id, text, length, &coord ) ) {
        return( coord.x );
    }
    return( 0 );
}

gui_ord GUIGetControlExtentY( gui_window *wnd, gui_ctl_id id, const char *text )
{
    gui_coord coord;

    if( GetControlExtent( wnd, id, text, (size_t)-1, &coord ) ) {
        return( coord.y );
    }
    return( 0 );
}
