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
#include "guixutil.h"
#include "guiscrol.h"

static void SetHScrollRange( gui_window *wnd, guix_ord range, guix_ord text_range )
{
    int new_range;
    guix_ord screen_size;

    screen_size = GUIGetScrollScreenSize( wnd, SB_HORZ );
    wnd->hscroll_range = text_range;
    wnd->flags |= HRANGE_SET;
    new_range = range - screen_size;
    if( new_range < 0 ) {
        new_range = 0;
    }
    GUISetScrollRange( wnd, SB_HORZ, 0, new_range, true );
}

static void SetVScrollRange( gui_window *wnd, guix_ord range, guix_ord text_range )
{
    int new_range;
    guix_ord screen_size;

    screen_size = GUIGetScrollScreenSize( wnd, SB_VERT );
    wnd->vscroll_range = text_range;
    wnd->flags |= VRANGE_SET;
    new_range = range - screen_size;
    if( new_range < 0 ) {
        new_range = 0;
    }
    GUISetScrollRange( wnd, SB_VERT, 0, new_range, true );
}

/*
 * GUISetHScrollRangeCols
 */
void GUIAPI GUISetHScrollRangeCols( gui_window *wnd, gui_text_ord text_range )
{
    if( IS_HSCROLL_ON( wnd ) ) {
        wnd->flags |= HRANGE_CHAR_UNIT;
        SetHScrollRange( wnd, GUITextToScreenH( text_range, wnd ), text_range );
    }
}

/*
 * GUISetVScrollRangeRows
 */
void GUIAPI GUISetVScrollRangeRows( gui_window *wnd, gui_text_ord text_range )
{
    if( IS_VSCROLL_ON( wnd ) ) {
        wnd->flags |= VRANGE_CHAR_UNIT;
        SetVScrollRange( wnd, GUITextToScreenV( text_range, wnd ), text_range );
    }
}

/*
 * GUISetHScrollRange
 */
void GUIAPI GUISetHScrollRange( gui_window *wnd, gui_ord range )
{
    guix_ord    scr_range;

    if( IS_HSCROLL_ON( wnd ) ) {
        wnd->flags &= ~HRANGE_CHAR_UNIT;
        scr_range = GUIScaleToScreenH( range );
        SetHScrollRange( wnd, scr_range, scr_range );
    }
}

/*
 * GUISetVScrollRange
 */
void GUIAPI GUISetVScrollRange( gui_window *wnd, gui_ord range )
{
    guix_ord    scr_range;

    if( IS_VSCROLL_ON( wnd ) ) {
        wnd->flags &= ~VRANGE_CHAR_UNIT;
        scr_range = GUIScaleToScreenV( range );
        SetVScrollRange( wnd, scr_range, scr_range );
    }
}

/*
 * GUIGetHScrollRange
 */
gui_ord GUIAPI GUIGetHScrollRange( gui_window *wnd )
{
    if( IS_HSCROLL_ON( wnd ) )
        return( GUIScaleFromScreenH( GUIGetScrollRange( wnd, SB_HORZ ) ) );
    return( GUI_NO_RANGE );
}

/*
 * GUIGetVScrollRange
 */
gui_ord GUIAPI GUIGetVScrollRange( gui_window *wnd )
{
    if( IS_VSCROLL_ON( wnd ) )
        return( GUIScaleFromScreenV( GUIGetScrollRange( wnd, SB_VERT ) ) );
    return( GUI_NO_RANGE );
}

/*
 * GUIGetHScrollRangeCols
 */
gui_text_ord GUIAPI GUIGetHScrollRangeCols( gui_window *wnd )
{
    if( IS_HSCROLL_ON( wnd ) )
        return( GUITextFromScreenH( GUIGetScrollRange( wnd, SB_HORZ ), wnd ) );
    return( GUI_TEXT_NO_RANGE );
}

/*
 * GUIGetVScrollRangeRows
 */
gui_text_ord GUIAPI GUIGetVScrollRangeRows( gui_window *wnd )
{
    if( IS_VSCROLL_ON( wnd ) )
        return( GUITextFromScreenV( GUIGetScrollRange( wnd, SB_VERT ), wnd ) );
    return( GUI_TEXT_NO_RANGE );
}
