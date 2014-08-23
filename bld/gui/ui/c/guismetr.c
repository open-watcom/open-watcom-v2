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

void GUIGetSystemMetrics( gui_system_metrics *metrics )
{
    gui_text_metrics    tm;

    if( metrics != NULL ) {
        GUIGetTextMetrics( NULL, &tm );
        metrics->resize_border.x = tm.max.x; /* don't need to scale 0 */
        metrics->resize_border.y = tm.max.x;
        metrics->mouse = uimouseinstalled();
        if( UIData != NULL ) {
            metrics->colour = ( UIData->colour != M_MONO );
        } else {
            metrics->colour = false;
        }
        metrics->top_left.x = tm.max.x;
        metrics->top_left.y = tm.max.y;
        metrics->bottom_right.x = tm.max.x;
        metrics->bottom_right.y = tm.max.y;
        metrics->scrollbar_size.x = 0;
        metrics->scrollbar_size.y = 0;
        metrics->dialog_top_left_size.x = tm.max.x;
        metrics->dialog_top_left_size.y = tm.max.y;
        metrics->dialog_bottom_right_size.x = tm.max.x;
        metrics->dialog_bottom_right_size.y = tm.max.y;
        metrics->caption_size = tm.max.y;
    }
}
