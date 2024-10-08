/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
 * SetScrollRange
 */

static void SetScrollRange( p_gadget gadget, gui_text_ord range )
{
    if( gadget->total_size != range ) {
        gadget->total_size = range;
        GUISetShowGadget( gadget, true, true, gadget->pos );
    }
}

/*
 * GUISetHScrollRange
 */

void GUIAPI GUISetHScrollRange( gui_window *wnd, gui_ord range )
{
    if( IS_HSCROLL_ON( wnd ) ) {
        SetScrollRange( wnd->hgadget, GUIScaleToScreenH( range ) );
        wnd->flags |= HRANGE_SET;
    }
}

/*
 * GUISetVScrollRange
 */

void GUIAPI GUISetVScrollRange( gui_window *wnd, gui_ord range )
{
    if( IS_VSCROLL_ON( wnd ) ) {
        SetScrollRange( wnd->vgadget, GUIScaleToScreenV( range ) );
        wnd->flags |= VRANGE_SET;
    }
}

/*
 * GUISetHScrollRangeCols
 */

void GUIAPI GUISetHScrollRangeCols( gui_window *wnd, gui_text_ord range )
{
    if( IS_HSCROLL_ON( wnd ) ) {
        SetScrollRange( wnd->hgadget, range );
        wnd->flags |= HRANGE_SET;
    }
}

/*
 * GUISetVScrollRangeRows
 */

void GUIAPI GUISetVScrollRangeRows( gui_window *wnd, gui_text_ord range )
{
    if( IS_VSCROLL_ON( wnd ) ) {
        SetScrollRange( wnd->vgadget, range );
        wnd->flags |= VRANGE_SET;
    }
}

/*
 * GUIGetVScrollRangeRows
 */

gui_text_ord GUIAPI GUIGetVScrollRangeRows( gui_window *wnd )
{
    if( IS_VSCROLL_ON( wnd ) )
        return( wnd->vgadget->total_size );
    return( GUI_TEXT_NO_RANGE );
}

gui_ord GUIAPI GUIGetVScrollRange( gui_window *wnd )
{
    if( IS_VSCROLL_ON( wnd ) )
        return( GUIScaleFromScreenV( wnd->vgadget->total_size ) );
    return( GUI_NO_RANGE );
}

/*
 * GUIGetHScrollRangeCols
 */

gui_text_ord GUIAPI GUIGetHScrollRangeCols( gui_window *wnd )
{
    if( IS_HSCROLL_ON( wnd ) )
        return( wnd->hgadget->total_size );
    return( GUI_TEXT_NO_RANGE );
}

gui_ord GUIAPI GUIGetHScrollRange( gui_window *wnd )
{
    if( IS_HSCROLL_ON( wnd ) )
        return( GUIScaleFromScreenH( wnd->hgadget->total_size ) );
    return( GUI_NO_RANGE );
}
