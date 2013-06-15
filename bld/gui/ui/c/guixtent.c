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


#include <string.h>
#include <stdlib.h>
#include "guiwind.h"
#include "guiscale.h"
#include "clibext.h"

gui_ord GUIGetExtentX( gui_window * wnd, const char * text, int length )
{
    gui_coord coord;

    wnd = wnd;

    if( text ) {
        coord.x = min( length, strlen( text ) );
        coord.y = 1;
        GUIScreenToScaleR( &coord );
        return( coord.x );
    } else {
        return( 0 );
    }
}

gui_ord GUIGetExtentY( gui_window * wnd, const char * text )
{
    gui_coord coord;

    wnd = wnd;
    text = text;
    coord.x = 1;
    coord.y = 1;
    GUIScreenToScaleR( &coord );
    return( coord.y );
}

gui_ord GUIGetControlExtentX( gui_window * wnd, unsigned id, const char * text, int length )
{
    id=id;
    return( GUIGetExtentX( wnd, text, length ) );
}

gui_ord GUIGetControlExtentY( gui_window * wnd, unsigned id, const char * text )
{
    id=id;
    return( GUIGetExtentY( wnd, text ) );
}

