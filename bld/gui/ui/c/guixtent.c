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


#include <string.h>
#include <stdlib.h>
#include "guiwind.h"
#include "guiscale.h"

#include "clibext.h"


gui_ord GUIAPI GUIGetExtentX( gui_window *wnd, const char *text, size_t length )
{
    guix_ord    scr_x;

    /* unused parameters */ (void)wnd;

    if( text != NULL ) {
        scr_x = strlen( text );
        if( scr_x > length )
            scr_x = length;
        return( GUIScreenToScaleH( scr_x ) );
    } else {
        return( 0 );
    }
}

gui_ord GUIAPI GUIGetExtentY( gui_window *wnd, const char *text )
{
    /* unused parameters */ (void)wnd; (void)text;

    return( GUIScreenToScaleV( 1 ) );
}

gui_ord GUIAPI GUIGetControlExtentX( gui_window *wnd, gui_ctl_id id, const char *text, size_t length )
{
    /* unused parameters */ (void)id;

    return( GUIGetExtentX( wnd, text, length ) );
}

gui_ord GUIAPI GUIGetControlExtentY( gui_window *wnd, gui_ctl_id id, const char *text )
{
    /* unused parameters */ (void)id;

    return( GUIGetExtentY( wnd, text ) );
}
