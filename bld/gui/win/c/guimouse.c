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

extern bool GUIGetMousePosn( gui_window *wnd, gui_point *point )
{
    WPI_POINT   pt;

    if( !_wpi_getsystemmetrics( SM_MOUSEPRESENT ) ) {
        return( false );
    }
    _wpi_getcursorpos( &pt );
#ifdef __OS2_PM__ // close your eyes!!! gross hack coming up
    _wpi_screentoclient( wnd->hwnd, &pt );
    pt.y = _wpi_cvtc_y( wnd->hwnd, pt.y );
    _wpi_clienttoscreen( wnd->hwnd, &pt );
#endif
    GUIMakeRelative( wnd, &pt, point );
    return( true );
}

