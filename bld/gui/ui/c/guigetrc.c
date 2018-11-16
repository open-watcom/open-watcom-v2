/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2018-2018 The Open Watcom Contributors. All Rights Reserved.
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


/*
 * GUIGetRow - get the row that the mouse is on
 */

gui_ord GUIGetRow( gui_window * wnd, gui_point * in_pt )
{
    gui_point pt;

    /* unused parameters */ (void)wnd;

    pt = *in_pt;
    GUIScaleToScreenRPt( &pt );
    if( pt.y >=0 ) {
        return( (gui_ord) pt.y );
    } else {
        return( GUI_NO_ROW );
    }
}

/*
 * GUIGetCol - get the column that the mouse is on
 */

gui_ord GUIGetCol( gui_window *wnd, const char *text, gui_point *in_pt )
{
    gui_point pt;

    /* unused parameters */ (void)wnd; (void)text;

    pt = *in_pt;
    GUIScaleToScreenRPt( &pt );
    if( pt.x >=0 ) {
        return( (gui_ord) pt.x );
    } else {
        return( GUI_NO_COLUMN );
    }
}
