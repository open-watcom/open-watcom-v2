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

/*
 * GUISetWindowText - set the title text of a window
 */

bool GUISetWindowText( gui_window *wnd, const char *data )
{
     _wpi_setwindowtext( GUIGetParentFrameHWND( wnd ), data );
     GUIChangeMDITitle( wnd );
     return( true );
}

/*
 * GUIGetWindowText - get the window text
 */

size_t GUIGetWindowText( gui_window *wnd, char *data, size_t max_length )
{
    return( _wpi_getwindowtext( GUIGetParentFrameHWND( wnd ), data, max_length ) );
}

size_t GUIGetWindowTextLength( gui_window *wnd )
{
    return( _wpi_getwindowtextlength( GUIGetParentFrameHWND( wnd ) ) );
}
