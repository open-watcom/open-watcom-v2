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
#include <stdlib.h>
#include <string.h>
#include "guigetx.h"
#include "guiscale.h"

extern WPI_TEXTMETRIC GUItm;

gui_ord GUIGetRow( gui_window * wnd, gui_point * in_pos )
{
    int         height;
    gui_ord     row;
    gui_point   pos;


    pos = *in_pos;
    GUIGetMetrics( wnd );
    height = AVGYCHAR(GUItm);

    GUIScaleToScreenRPt( &pos );
    row = pos.y / height;
    return( row );
}

gui_ord GUIGetCol( gui_window *wnd, const char *text, gui_point *in_pos )
{
    int         width;
    bool        got_new;
    gui_point   pos;

    got_new = GUIGetTheDC( wnd );
    GUIGetMetrics( wnd );
    pos = *in_pos;
    GUIScaleToScreenRPt( &pos );
    width = pos.x / MAXXCHAR( GUItm );
    while( ( width < strlen( text ) ) &&
           ( GUIGetTextExtentX( wnd, text, width ) <= pos.x ) ) {
        width++ ;
    }
    if( got_new ) {
        GUIReleaseTheDC( wnd );
    }
    return( (gui_ord) ( width - 1 ) );
}
