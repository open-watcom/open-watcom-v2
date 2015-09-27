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
 * allocImage - allocate screen image
 */
static void allocImage( void )
{
    int     i;
    int     size;

    size = EditVars.WindMaxWidth * EditVars.WindMaxHeight;
    ScreenImage = MemAlloc( size * sizeof( window_id ) );
    for( i = 0; i < size; i++ ) {
        ScreenImage[i] = NO_WINDOW;
    }

} /* allocImage */

/*
 * StartWindows - begin windows session
 */
void StartWindows( void )
{
    allocImage();
    ClearScreen();
    EditFlags.WindowsStarted = true;

} /* StartWindows */

/*
 * FinishWindows - done with windows; close down
 */
void FinishWindows( void )
{
    cursor_type ct;
    window_id   i;

    // Close Down All Straggling Windows
    for( i = 0; i < MAX_WINDS; i++ ) {
        if( Windows[i] != NULL ) {
            // CloseAWindow( i );
        }
    }

    // Close down the windowing system.

    if( EditFlags.ZapColors ) {
        int     j, total;

        if( !EditFlags.Quiet && Scrn != NULL ) {
            total = EditVars.WindMaxWidth * EditVars.WindMaxHeight;
            for( j = 0; j < total; j++ ) {
                Scrn[j].cinfo_attr = EditVars.ExitAttr;
            }
#ifdef __VIO__
            MyVioShowBuf( 0, EditVars.WindMaxWidth * EditVars.WindMaxHeight );
#endif
        }
    }
    FiniColors();
    ct.height = 7;
    ct.width = 100;
    NewCursor( NO_WINDOW, ct );
    MemFree( ScreenImage );

} /* FinishWindows */
