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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "vi.h"
#include "win.h"
#ifdef __WIN__
#include "winvi.h"
#endif

/*
 * ReDisplayScreen - reset all windows, redraw lines (used by ^L)
 */
int ReDisplayScreen( void )
{
    if( EditFlags.Menus ) {
        MoveWindowToFrontDammit( MenuWindow, TRUE );
    }

    ReDisplayBuffers( FALSE );

    MoveWindowToFrontDammit( MessageWindow, TRUE );
#ifdef __WIN__
    InvalidateRect( MessageWindow, NULL, TRUE );
    UpdateWindow( MessageWindow );
#endif
    if( EditFlags.StatusInfo ) {
        MoveWindowToFrontDammit( StatusWindow, TRUE );
    }
#ifdef __WIN__
    DCInvalidateAllLines();
    if( !BAD_ID( CurrentWindow ) ) {
        InvalidateRect( CurrentWindow, NULL, FALSE );
    }
    if( !BAD_ID( StatusWindow ) ) {
        InvalidateRect( StatusWindow, NULL, TRUE );
        UpdateWindow( StatusWindow );
    }
#endif
    SetWindowCursor();
    DCDisplayAllLines();
    return( ERR_NO_ERR );

} /* ReDisplayScreen */

/*
 * ReDisplayBuffers - redraw all buffer windows
 */
int ReDisplayBuffers( bool runFts )
{
    info        *cinfo;
    info        *ocurrinfo;

    SaveCurrentInfo();
    ocurrinfo = CurrentInfo;
    cinfo = InfoHead;
    while( cinfo != NULL ) {
        if( cinfo != ocurrinfo ) {
            if( WindowIsVisible( cinfo->CurrentWindow ) ) {
                BringUpFile( cinfo, runFts );
            }
        }
        cinfo = cinfo->next;
    }
    BringUpFile( ocurrinfo, runFts );

    return( ERR_NO_ERR );
}
