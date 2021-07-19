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
#include <stdlib.h>
#include <string.h>
#include "guigetx.h"
#include "guiscale.h"


gui_text_ord GUIAPI GUIGetRow( gui_window *wnd, const gui_point *in_pos )
{
    GUIGetMetrics( wnd );
    return( GUIScaleToScreenV( in_pos->y ) / AVGYCHAR( GUItm ) );
}

gui_text_ord GUIAPI GUIGetCol( gui_window *wnd, const char *text, const gui_point *in_pos )
{
    gui_text_ord    width;
    guix_ord        scr_x;
    bool            got_new;

    got_new = GUIGetTheDC( wnd );
    GUIGetMetrics( wnd );
    scr_x = GUIScaleToScreenH( in_pos->x );
    width = scr_x / MAXXCHAR( GUItm );
    while( ( width < strlen( text ) ) && ( GUIGetTextExtentX( wnd, text, width ) <= scr_x ) ) {
        width++ ;
    }
    if( got_new ) {
        GUIReleaseTheDC( wnd );
    }
    return( width - 1 );
}
