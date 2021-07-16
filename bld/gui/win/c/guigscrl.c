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
#include "guixutil.h"

static guix_ord GetPos( gui_window *wnd, int bar )
{
    if( GUIScrollOn( wnd, bar ) ) {
        return( GUIGetScrollPos( wnd, bar ) );
    } else {
        return( 0 );
    }
}

/*
 * GUIGetHScrollCol --
 */

gui_text_ord GUIAPI GUIGetHScrollCol( gui_window *wnd )
{
    return( GUIToTextX( GetPos( wnd, SB_HORZ ), wnd ) );
}

/*
 * GUIGetVScrollRow --
 */

gui_text_ord GUIAPI GUIGetVScrollRow( gui_window *wnd )
{
    return( GUIToTextY( GetPos( wnd, SB_VERT ), wnd ) );
}


static gui_ord GetScroll( gui_window *wnd, int bar )
{
    guix_ord    pos;

    pos = GetPos( wnd, bar );
    if( bar == SB_HORZ ) {
        return( GUIScreenToScaleH( pos ) );
    } else {
        return( GUIScreenToScaleV( pos ) );
    }
}

/*
 * GUIGetHScroll --
 */

gui_ord GUIAPI GUIGetHScroll( gui_window *wnd )
{
    return( GetScroll( wnd, SB_HORZ ) );
}

/*
 * GUIGetVScroll --
 */

gui_ord GUIAPI GUIGetVScroll( gui_window *wnd )
{
    return( GetScroll( wnd, SB_VERT ) );
}
