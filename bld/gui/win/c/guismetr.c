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

void GUIGetSystemMetrics( gui_system_metrics *metrics )
{
    HDC       hdc;
    int       num_colours;
    gui_coord cap;

    if( metrics != NULL ) {
        /* size of border on which your cursor will cause resize */
        metrics->resize_border.x = (gui_ord) _wpi_getsystemmetrics( SM_CXFRAME );
        metrics->resize_border.y = (gui_ord) _wpi_getsystemmetrics( SM_CYFRAME );

        metrics->mouse = (bool) _wpi_getsystemmetrics( SM_MOUSEPRESENT );
        hdc = _wpi_getpres( HWND_DESKTOP );
        num_colours = _wpi_getdevicecaps( hdc, NUMCOLORS );
        metrics->colour = (bool) ( num_colours > 2 );
        _wpi_releasepres( HWND_DESKTOP, hdc );

        metrics->top_left.x = (gui_ord) _wpi_getsystemmetrics( SM_CXFRAME );
        metrics->top_left.y = (gui_ord) ( _wpi_getsystemmetrics( SM_CYFRAME ) +
           _wpi_getsystemmetrics( SM_CYCAPTION ) - _wpi_getsystemmetrics( SM_CYBORDER ) );
        metrics->bottom_right.x   = (gui_ord) _wpi_getsystemmetrics( SM_CXFRAME );
        metrics->bottom_right.y   = (gui_ord) _wpi_getsystemmetrics( SM_CYFRAME );
        metrics->scrollbar_size.x = (gui_ord) _wpi_getsystemmetrics( SM_CXHTHUMB ) - 1;
        metrics->scrollbar_size.y = (gui_ord) _wpi_getsystemmetrics( SM_CYVTHUMB ) - 1;

        /* get the size of a window caption */
        cap.y = (gui_ord) _wpi_getsystemmetrics( SM_CYCAPTION );
        cap.x = 0;

        metrics->dialog_bottom_right_size.x = (gui_ord) _wpi_getsystemmetrics( SM_CXDLGFRAME );
        metrics->dialog_bottom_right_size.y = (gui_ord) _wpi_getsystemmetrics( SM_CYDLGFRAME );
        metrics->dialog_top_left_size = metrics->dialog_bottom_right_size;
        metrics->dialog_top_left_size.y += cap.y;

        /* convert to user's coordinate system */
        GUIScreenToScaleR( &metrics->resize_border );
        GUIScreenToScaleR( &metrics->top_left );
        GUIScreenToScaleR( &metrics->bottom_right );
        GUIScreenToScaleR( &metrics->scrollbar_size );
        GUIScreenToScaleR( &metrics->dialog_top_left_size );
        GUIScreenToScaleR( &metrics->dialog_bottom_right_size );
        GUIScreenToScaleR( &cap );
        metrics->caption_size = cap.y;
    }
}
