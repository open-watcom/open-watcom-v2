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
 * ShiftWindowUpDown - shift the stuff in a window in an up/down direction
 */
void ShiftWindowUpDown( window_id id, int diff )
{
    wind                *w;
    int                 start, spl, i, j;
    int                 sline, eline, add;
    char_info           _FAR *scr_d;
    char_info           *txt_s, *txt_d;

    if( EditFlags.DisplayHold || EditFlags.Quiet ) {
        return;
    }
    w = AccessWindow( id );
    if( w->has_border ) {
        start = 1;
        spl = 0;
    } else {
        start = 0;
        spl = -1;
    }
    if( diff < 0 ) {
        sline = w->text_lines + diff;
        eline = 1;
        add = -1;
    } else {
        sline = diff + 1;
        eline = w->text_lines;
        add = 1;
    }

    /*
     * current window is never overlapped, so we can just blast away
     */
    sline += spl;
    eline += spl;
    i = sline;
    while( 1 ) {
        txt_s = (char_info *) &w->text[(i * w->width + start) * sizeof( char_info )];
        txt_d = (char_info *) &w->text[((i - diff) * w->width + start) *
                                       sizeof( char_info )];
        scr_d = (char_info _FAR *) &Scrn[(w->x1 + start + (w->y1 + i - diff) *
                                          WindMaxWidth) * sizeof( char_info )];
        for( j = 0; j < w->text_cols; j++ ) {
            WRITE_SCREEN( scr_d[j], txt_s[j] );
            WRITE_SCREEN_DATA( txt_d[j], txt_s[j] );
        }
#ifdef __VIO__
        MyVioShowBuf( (unsigned)((char *) scr_d - Scrn), w->text_cols );
#endif
        if( i == eline ) {
            break;
        }
        i += add;
    }
    ReleaseWindow( w );

} /* ShiftWindowUpDown */
