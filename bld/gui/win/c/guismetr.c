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
#include "guiscale.h"

void GUIAPI GUIGetSystemMetrics( gui_system_metrics *metrics )
{
    HDC       hdc;

    if( metrics != NULL ) {
        /* size of border on which your cursor will cause resize */
        metrics->resize_border.x = GUIScreenToScaleH( _wpi_getsystemmetrics( SM_CXFRAME ) );
        metrics->resize_border.y = GUIScreenToScaleV( _wpi_getsystemmetrics( SM_CYFRAME ) );

        metrics->mouse = (bool)_wpi_getsystemmetrics( SM_MOUSEPRESENT );
        hdc = _wpi_getpres( HWND_DESKTOP );
        metrics->colour = (bool)( (int)_wpi_getdevicecaps( hdc, NUMCOLORS ) > 2 );
        _wpi_releasepres( HWND_DESKTOP, hdc );

        metrics->top_left.x = GUIScreenToScaleH( _wpi_getsystemmetrics( SM_CXFRAME ) );
        metrics->top_left.y = GUIScreenToScaleV( _wpi_getsystemmetrics( SM_CYFRAME )
                + _wpi_getsystemmetrics( SM_CYCAPTION ) - _wpi_getsystemmetrics( SM_CYBORDER ) );
        metrics->bottom_right.x   = GUIScreenToScaleH( _wpi_getsystemmetrics( SM_CXFRAME ) );
        metrics->bottom_right.y   = GUIScreenToScaleV( _wpi_getsystemmetrics( SM_CYFRAME ) );
        metrics->scrollbar_size.x = GUIScreenToScaleH( _wpi_getsystemmetrics( SM_CXHTHUMB ) - 1 );
        metrics->scrollbar_size.y = GUIScreenToScaleV( _wpi_getsystemmetrics( SM_CYVTHUMB ) - 1 );

        /* get the size of a window caption */
        metrics->dialog_bottom_right_size.x = GUIScreenToScaleH( _wpi_getsystemmetrics( SM_CXDLGFRAME ) );
        metrics->dialog_bottom_right_size.y = GUIScreenToScaleV( _wpi_getsystemmetrics( SM_CYDLGFRAME ) );
        metrics->dialog_top_left_size.x = GUIScreenToScaleH( _wpi_getsystemmetrics( SM_CXDLGFRAME ) );
        metrics->dialog_top_left_size.y = GUIScreenToScaleV( _wpi_getsystemmetrics( SM_CYDLGFRAME )
                + _wpi_getsystemmetrics( SM_CYCAPTION ) );

        metrics->caption_size = GUIScreenToScaleV( _wpi_getsystemmetrics( SM_CYCAPTION ) );
    }
}
