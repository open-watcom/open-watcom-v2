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
#include "win.h"

/*
 * LineNumbersSetup - set up line number window
 */
vi_rc LineNumbersSetup( void )
{
    int         j;
    int         x1, x2;
    window_info wi;
    vi_rc       rc;

    if( EditFlags.LineNumbers ) {
        if( editw_info.area.x2 - editw_info.area.x1 < EditVars.LineNumWinWidth ) {
            return( ERR_CANNOT_OPEN_LINENUM_WIND );
        }
        if( !linenumw_info.has_border ) {
            j = 1;
        } else {
            j = 0;
        }
        memcpy( &wi, &linenumw_info, sizeof( window_info ) );
        x1 = WindowAuxInfo( current_window_id, WIND_INFO_X1 );
        x2 = WindowAuxInfo( current_window_id, WIND_INFO_X2 );
        wi.area.y1 = WindowAuxInfo( current_window_id, WIND_INFO_Y1 );
        wi.area.y2 = WindowAuxInfo( current_window_id, WIND_INFO_Y2 );
        if( !EditFlags.LineNumsOnRight ) {
            rc = ResizeWindowRelative( current_window_id, EditVars.LineNumWinWidth, 0, 0, 0, false );
            if( rc != ERR_NO_ERR ) {
                return( rc );
            }
            wi.area.x1 = x1;
            wi.area.x2 = x1 + EditVars.LineNumWinWidth - 1 - j;
        } else {
            rc = ResizeWindowRelative( current_window_id, 0, 0, -EditVars.LineNumWinWidth, 0, false );
            if( rc != ERR_NO_ERR ) {
                return( rc );
            }
            wi.area.x1 = x2 - EditVars.LineNumWinWidth + 1;
            wi.area.x2 = x2 - j;
        }
        rc = NewWindow2( &curr_num_window_id, &wi );
        if( rc != ERR_NO_ERR ) {
            return( rc );
        }
    } else {
        CloseAWindow( curr_num_window_id );
        rc = ResizeWindow( current_window_id, editw_info.area.x1, editw_info.area.y1, editw_info.area.x2, editw_info.area.y2, false );
        if( rc != ERR_NO_ERR ) {
            return( rc );
        }
    }
    SetWindowCursor();
    DCDisplayAllLines();
    return( ERR_NO_ERR );

} /* LineNumbersSetup */
