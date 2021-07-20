/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2018-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Status Window handling
*
****************************************************************************/


#include "guiwind.h"
#include "guistat.h"
#include "guicolor.h"
#include "guiutil.h"
#include "guixutil.h"
#include "guiscale.h"
#include <string.h>

static bool SetStatusArea( gui_window *wnd, gui_ord x, gui_ord height, SAREA *area )
{
    SAREA           sarea;
    guix_ord        dim_x;
    guix_ord        dim_y;

    GUIGetClientSAREA( wnd, &sarea );

    dim_x = GUIScaleToScreenH( x );
    dim_y = GUIScaleToScreenV( height );
    if( dim_y == 0 ) {
        dim_y = 1;
    }

    area->row = sarea.height - dim_y;
    area->col = dim_x;
    area->height = dim_y;
    area->width = sarea.width - dim_x;

    return( true );
}

bool GUIAPI GUICreateStatusWindow( gui_window *wnd, gui_ord x, gui_ord height, gui_colour_set *colour )
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

bool GUIAPI GUIResizeStatusWindow( gui_window *wnd, gui_ord x, gui_ord height )
{
    SAREA       area;

    if( !GUIHasStatus( wnd ) ) {
        return( false );
    }
    if( !SetStatusArea( wnd, x, height, &area ) ) {
        return( false );
    } else {
        area.row += wnd->status->area.height;
        COPYRECTX( area, wnd->status->area );
    }
    GUISetUseWnd( wnd );
    GUIDrawStatus( wnd );
    return( true );
}

void GUIDrawStatus( gui_window *wnd )
{
    size_t  length;

    if( GUIHasStatus( wnd ) && !GUI_WND_MINIMIZED( wnd ) ) {
        uivfill( &wnd->vs, wnd->status->area, wnd->status->attr, ' ' );
        if( wnd->status->text != NULL ) {
            length = strlen( wnd->status->text );
            if( length > wnd->status->area.width ) {
                length = wnd->status->area.width;
            }
            uivtextput( &wnd->vs, wnd->status->area.row,
                        wnd->status->area.col, wnd->status->attr,
                        wnd->status->text, length );
        }
    }
}

bool GUIAPI GUIDrawStatusText( gui_window *wnd, const char *text )
{
    bool        ok;

    if( !GUIHasStatus( wnd ) ) {
        return( false );
    }
    if( wnd->status->text != NULL ) {
        GUIMemFree( wnd->status->text );
    }
    wnd->status->text = GUIStrDup( text, &ok );
    GUIDrawStatus( wnd );
    if( GUI_WND_VISIBLE( wnd ) ) {
        uirefresh();
    }
    return( ok );
}

bool GUIAPI GUIHasStatus( gui_window *wnd )
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

bool GUIAPI GUICloseStatusWindow( gui_window *wnd )
{
    SAREA       area;

    if( GUIHasStatus( wnd ) ) {
        COPYRECTX( wnd->status->area, area );
        GUIFreeStatus( wnd );
        GUISetUseWnd( wnd );
        GUIDirtyArea( wnd, &area );
        return( true );
    }
    return( false );
}
