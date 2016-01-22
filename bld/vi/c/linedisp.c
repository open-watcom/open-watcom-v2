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
 * ReDisplayScreen - reset all windows, redraw lines (used by ^L)
 */
vi_rc ReDisplayScreen( void )
{
    if( EditFlags.Menus ) {
        MoveWindowToFrontDammit( menu_window_id, true );
    }

    ReDisplayBuffers( false );

    MoveWindowToFrontDammit( message_window_id, true );
#ifdef __WIN__
    InvalidateRect( message_window_id, NULL, TRUE );
    UpdateWindow( message_window_id );
#endif
    if( EditFlags.StatusInfo ) {
        MoveWindowToFrontDammit( status_window_id, true );
    }
#ifdef __WIN__
    DCInvalidateAllLines();
    if( !BAD_ID( current_window_id ) ) {
        InvalidateRect( current_window_id, NULL, FALSE );
    }
    if( !BAD_ID( status_window_id ) ) {
        InvalidateRect( status_window_id, NULL, TRUE );
        UpdateWindow( status_window_id );
    }
#endif
    SetWindowCursor();
    DCDisplayAllLines();
    return( ERR_NO_ERR );

} /* ReDisplayScreen */

/*
 * ReDisplayBuffers - redraw all buffer windows
 */
vi_rc ReDisplayBuffers( bool runFts )
{
    info        *cinfo;
    info        *ocurrinfo;

    SaveCurrentInfo();
    ocurrinfo = CurrentInfo;
    for( cinfo = InfoHead; cinfo != NULL; cinfo = cinfo->next ) {
        if( cinfo != ocurrinfo ) {
            if( WindowIsVisible( cinfo->current_window_id ) ) {
                BringUpFile( cinfo, runFts );
            }
        }
    }
    BringUpFile( ocurrinfo, runFts );

    return( ERR_NO_ERR );

} /* ReDisplayBuffers */
