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


#include "guiwind.h"
#include <string.h>
#include "guiscale.h"
#include "guigetx.h"


static gui_text_ord DoReturn( gui_text_ord ret, gui_window *wnd, bool got_new )
{
    if( got_new ) {
        GUIReleaseTheDC( wnd );
    }
    return( ret );
}

/*
 * GUIGetStringPos
 */

gui_text_ord GUIAPI GUIGetStringPos( gui_window *wnd, gui_ord indent, const char *string, gui_ord mouse_x )
{
    guix_ord        diff_x;
    gui_text_ord    guess;
    gui_text_ord    length;
    guix_ord        curr;
    guix_ord        new_curr;
    bool            got_new;

    if( indent > mouse_x ) {
        return( GUI_TEXT_NO_COLUMN );
    }

    got_new = GUIGetTheDC( wnd );

    diff_x = GUIScaleToScreenH( mouse_x - indent );
    guess = length = strlen( string );
    curr = GUIGetTextExtentX( wnd, string, guess );
    if( curr < diff_x ) {
        return( DoReturn( GUI_TEXT_NO_COLUMN, wnd, got_new ) );
    }
    if( curr == diff_x ) {
        return( DoReturn( guess, wnd, got_new ) );
    }
    guess = GUIMulDiv( gui_text_ord, length, diff_x, curr );
    curr = GUIGetTextExtentX( wnd, string, guess );
    if( curr == diff_x ) {
        return( DoReturn( guess, wnd, got_new ) );
    }
    if( curr < diff_x ) {
        guess++;
    } else {
        guess--;
    }
    for( ;; ) {
        new_curr = GUIGetTextExtentX( wnd, string, guess );

        if( new_curr == diff_x ) {
            return( DoReturn( guess, wnd, got_new ) );
        }
        if( ( new_curr < diff_x ) && ( curr > diff_x ) ) {
            return( DoReturn( guess, wnd, got_new ) );
        }
        if( ( new_curr > diff_x ) && ( curr < diff_x ) ) {
            return( DoReturn( guess - 1, wnd, got_new ) );
        }
        if( new_curr < diff_x ) {
            guess++;
        } else {
            guess--;
        }
        curr = new_curr;
    }
}
