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
#include "guistat.h"
#include "guicolor.h"
#include "guiutil.h"
#include "guixutil.h"
#include "guiscale.h"
#include <string.h>

static bool SetStatusArea( gui_window *wnd, gui_ord x, gui_ord height,
                           SAREA *area )
{
    SAREA               sarea;
    gui_coord           dim;

    GUIGetClientSAREA( wnd, &sarea );

    dim.x = x;
    dim.y = height;
    GUIScaleToScreenR( &dim );
    if( dim.y == 0 ) {
        dim.y = 1;
    }

    area->row = sarea.height - dim.y;
    area->col = dim.x;
    area->height = dim.y;
    area->width = sarea.width - dim.x;

    return( true );
}

bool GUICreateStatusWindow( gui_window *wnd, gui_ord x, gui_ord height,
                            gui_colour_set *colour )
{
    statusinfo  *stat_info;

    if( ( wnd->parent != NULL ) || ( wnd->status != NULL ) || ( colour == NULL ) ) {
        return( false );
    }
    stat_info = (statusinfo *)GUIMemAlloc( sizeof( statusinfo ) );
    if( stat_info == NULL ) {
        return( false );
    }
    stat_info->text = NULL;
    stat_info->attr = GUIMakeColour( colour->fore, colour->back );
    if( !SetStatusArea( wnd, x, height, &stat_info->area ) ) {
        GUIMemFree( stat_info );
        return( false );
    }
    wnd->status = stat_info;
    GUISetUseWnd( wnd );
    GUIDrawStatus( wnd );
    return( true );
}

bool GUIResizeStatusWindow( gui_window *wnd, gui_ord x, gui_ord height )
{
    SAREA       area;

    if( !GUIHasStatus( wnd ) ) {
        return( false );
    }
    if( !SetStatusArea( wnd, x, height, &area ) ) {
        return( false );
    } else {
        area.row += wnd->status->area.height;
        COPYAREA( area, wnd->status->area );
    }
    GUISetUseWnd( wnd );
    GUIDrawStatus( wnd );
    return( true );
}

void GUIDrawStatus( gui_window *wnd )
{
    int length;

    if( GUIHasStatus( wnd ) && !GUI_WND_MINIMIZED( wnd ) ) {
        uivfill( &wnd->screen, wnd->status->area, wnd->status->attr, ' ' );
        if( wnd->status->text != NULL ) {
            length = strlen( wnd->status->text );
            if( length > wnd->status->area.width ) {
                length = wnd->status->area.width;
            }
            uivtextput( &wnd->screen, wnd->status->area.row,
                        wnd->status->area.col, wnd->status->attr,
                        wnd->status->text, length );
        }
    }
}

bool GUIDrawStatusText( gui_window *wnd, const char *text )
{
    bool        ret;

    if( !GUIHasStatus( wnd ) ) {
        return( false );
    }
    if( wnd->status->text != NULL ) {
        GUIMemFree( wnd->status->text );
    }
    ret = GUIStrDup( text, &wnd->status->text );
    GUIDrawStatus( wnd );
    if( GUI_WND_VISIBLE( wnd ) ) {
        uirefresh();
    }
    return( ret );
}

bool GUIHasStatus( gui_window *wnd )
{
    return( wnd->status != NULL );
}

void GUIFreeStatus( gui_window *wnd )
{
    if( GUIHasStatus( wnd ) ) {
        if( wnd->status->text != NULL ) {
            GUIMemFree( wnd->status->text );
        }
        GUIMemFree( wnd->status );
        wnd->status = NULL;
    }
}

void GUIResizeStatus( gui_window *wnd )
{
    if( GUIHasStatus( wnd ) ) {
        wnd->status->area.width = wnd->use.width;
        wnd->status->area.row = wnd->use.height + 1;
    }
}

bool GUICloseStatusWindow( gui_window *wnd )
{
    SAREA       area;

    if( GUIHasStatus( wnd ) ) {
        COPYAREA( wnd->status->area, area );
        GUIFreeStatus( wnd );
        GUISetUseWnd( wnd );
        GUIDirtyArea( wnd, &area );
        return( true );
    }
    return( false );
}
