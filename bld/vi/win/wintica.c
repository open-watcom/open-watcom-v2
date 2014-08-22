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


#include "vi.h"
// #include <malloc.h>

/*
 * WindowTile - tile all edit windows
 */
vi_rc WindowTile( int maxx, int maxy )
{
    RECT        rect;
    info        *cinfo;
    info        *cwinfo;
    int         height;
    int         width;

    if( maxx == 1 && maxy == 1 ) {
        // Some sort of bizzaro tile thingy
        GetWindowRect( EditContainer, &rect );
        height = rect.bottom - rect.top;
        width = rect.right - rect.left;

        SaveCurrentInfo();
        cwinfo = CurrentInfo;
        for( cinfo = InfoHead; cinfo != NULL; cinfo = cinfo->next ) {
            if( IsIconic( cinfo->CurrentWindow ) ) {
                ShowWindow( cinfo->CurrentWindow, SW_RESTORE );
            }
            BringUpFile( cinfo, false );
            MoveWindow( cinfo->CurrentWindow, 0, 0, width, height, TRUE );
        }
        BringUpFile( cwinfo, false );
        return( ERR_NO_ERR );
    }
    // else

    SendMessage( EditContainer, WM_MDITILE, 0, 0L );
    return( ERR_NO_ERR );

} /* WindowTile */

/*
 * WindowCascade - cascade all edit windows
 */
vi_rc WindowCascade( int x, int y )
{
    x = x;
    y = y;

    SendMessage( EditContainer, WM_MDICASCADE, 0, 0L );
    return( ERR_NO_ERR );

} /* WindowCascade */
