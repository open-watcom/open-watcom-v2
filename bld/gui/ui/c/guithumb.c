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
#include "guigadgt.h"

/*
 * SetScrollThumb
 */

static void SetScrollThumb( p_gadget gadget, int percent, bool range_set )
{
    int pos;

    if( gadget == NULL ) {
        return;
    }
    if( percent < 0 ) {
        percent = 0;
    }
    if( percent > 100 ) {
        percent = 100;
    }
    if( gadget != NULL ) {
        if( !range_set ) {
            gadget->total_size = 2 * gadget->page_size;
        }
        pos = (long)( gadget->total_size - gadget->page_size )
                            * percent / 100;
        if( ( pos == 0 ) && ( percent != 0 ) &&
            ( gadget->total_size > gadget->page_size ) ) {
            pos++;
        }
        if( ( pos == gadget->total_size ) && ( percent != 100 ) ) {
            pos--;
        }
        GUISetShowGadget( gadget, TRUE, TRUE, pos );
   }
}

/*
 * GUISetHScrollThumb
 */

void GUISetHScrollThumb( gui_window * wnd, int percent )
{
    if( wnd != NULL ) {
        SetScrollThumb( wnd->hgadget, percent, GUI_HRANGE_SET( wnd ) );
        wnd->flags |= SETHRANGE;
    }
}

/*
 * GUISetVScrollThumb
 */

void GUISetVScrollThumb( gui_window * wnd, int percent )
{
    if( wnd != NULL ) {
        SetScrollThumb( wnd->vgadget, percent, GUI_VRANGE_SET( wnd ) );
        wnd->flags |= SETVRANGE;
    }
}
