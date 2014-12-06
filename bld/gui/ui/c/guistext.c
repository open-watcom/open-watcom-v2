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
#include "guixutil.h"
#include "guihook.h"
#include <string.h>

/*
 * GUISetWindowText - set the title text of a window
 */

bool GUISetWindowText( gui_window *wnd, const char *data )
{
    if( GUIJustSetWindowText( wnd, data ) ) {
        GUIRedrawTitle( wnd );
        GUIChangeMDITitle( wnd );
        return( true );
    } else {
        return( false );
    }

} /* GUISetWindowText */

/*
 * GUIGetWindowTextLength - get the length of the title of a window
 */

size_t GUIGetWindowTextLength( gui_window *wnd )
{
    if( wnd->screen.title == NULL ) {
        return( 0 );
    } else {
        return( strlen( wnd->screen.title ) );
    }
}

/*
 * GUIGetWindowText - copy the name of the window to the buffer, up to
 *                    max_length characters
 */

size_t GUIGetWindowText( gui_window *wnd, char *buff, size_t buff_len )
{
    size_t len;

    if( buff_len == 0 )
        return( 0 );
    if( wnd->screen.title == NULL ) {
        buff_len = 0;
    } else {
        --buff_len;       // reserve space for null character on the end
        len = strlen( wnd->screen.title );
        if( buff_len > len  )
            buff_len = len;
        memcpy( buff, wnd->screen.title, buff_len );
    }
    buff[buff_len] = '\0';
    return( buff_len );
}
