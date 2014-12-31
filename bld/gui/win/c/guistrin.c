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
#include <string.h>
#include "guiscale.h"
#include "guigetx.h"

extern WPI_TEXTMETRIC GUItm;

static gui_ord DoReturn( gui_ord ret, gui_window * wnd, bool got_new )
{
    wnd=wnd;
    if( got_new ) {
        GUIReleaseTheDC( wnd );
    }
    return( ret );
}

/*
 * GUIGetStringPos
 */

extern gui_ord GUIGetStringPos( gui_window *wnd, gui_ord indent,
                                const char *string, int mouse_x )

{
    gui_coord diff;
    int       guess;
    int       length;
    int       curr;
    int       new_curr;
    bool      got_new;

    if( indent > mouse_x ) {
        return( GUI_NO_COLUMN );
    }

    got_new = GUIGetTheDC( wnd );

    diff.x = mouse_x - indent;
    GUIScaleToScreenR( &diff );
    length = strlen( string );
    guess = length;
    curr = GUIGetTextExtentX( wnd, string, guess );
    if( curr < diff.x ) {
        return( DoReturn( GUI_NO_COLUMN, wnd, got_new ) );
    }
    if( curr == diff.x ) {
        return( DoReturn( (gui_ord)guess, wnd, got_new ) );
    }
    guess = diff.x * (long)length / curr;
    curr = GUIGetTextExtentX( wnd, string, guess );
    if( curr == diff.x ) {
        return( DoReturn( (gui_ord)guess, wnd, got_new ) );
    }
    if( curr < diff.x ) {
        guess++;
    } else {
        guess--;
    }
    for( ; ; ) {
        new_curr = GUIGetTextExtentX( wnd, string, guess );

        if( new_curr == diff.x ) {
            return( DoReturn( (gui_ord) guess, wnd, got_new ) );
        }
        if( ( new_curr < diff.x ) && ( curr > diff.x ) ) {
            return( DoReturn( (gui_ord)guess, wnd, got_new ) );
        }
        if( ( new_curr > diff.x ) && ( curr < diff.x ) ) {
            return( DoReturn( (gui_ord)guess - 1, wnd, got_new ) );
        }
        if( new_curr < diff.x ) {
            guess++;
        } else {
            guess--;
        }
        curr = new_curr;
    }
}
