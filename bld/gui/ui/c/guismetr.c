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

void GUIAPI GUIGetSystemMetrics( gui_system_metrics *pmetrics )
{
    gui_text_metrics    metrics;

    if( pmetrics != NULL ) {
        GUIGetTextMetrics( NULL, &metrics );
        pmetrics->resize_border.x = metrics.max.x; /* don't need to scale 0 */
        pmetrics->resize_border.y = metrics.max.y;
        pmetrics->mouse = uimouseinstalled();
        pmetrics->colour = ( UIData != NULL && UIData->colour != M_MONO );
        pmetrics->top_left.x = metrics.max.x;
        pmetrics->top_left.y = metrics.max.y;
        pmetrics->bottom_right.x = metrics.max.x;
        pmetrics->bottom_right.y = metrics.max.y;
        pmetrics->scrollbar_size.x = 0;
        pmetrics->scrollbar_size.y = 0;
        pmetrics->dialog_top_left_size.x = metrics.max.x;
        pmetrics->dialog_top_left_size.y = metrics.max.y;
        pmetrics->dialog_bottom_right_size.x = metrics.max.x;
        pmetrics->dialog_bottom_right_size.y = metrics.max.y;
        pmetrics->caption_size = metrics.max.y;
    }
}
