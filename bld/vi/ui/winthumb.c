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


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "vi.h"
#include "win.h"

/*
 * DrawVerticalThumb - draw the scroll thumb on the screen
 */
void DrawVerticalThumb( wind *w, char ch )
{
    char_info   what;
    int         pos,xpos;
    char_info   *txt;
    char        *over;
    char_info   _FAR *scr;

    if( w->vert_scroll_pos < THUMB_START || EditFlags.Quiet ) {
        return;
    }

    txt = (char_info *) w->text;
    over = w->overlap;
    scr = (char_info _FAR *) Scrn;

    what.ch = ch;
    what.attr = MAKE_ATTR( w, w->border_color1, w->border_color2 );

    pos = (w->x2) + (w->y1+w->vert_scroll_pos)*WindMaxWidth;
    xpos = w->width-1 + (w->vert_scroll_pos)*w->width;

    WRITE_SCREEN_DATA( txt[xpos], what );
    if( over[xpos] == NO_CHAR ) {
        WRITE_SCREEN( scr[pos], what );
    }
#ifdef __VIO__
    MyVioShowBuf( sizeof( char_info ) * pos, 1 );
#endif

} /* DrawVerticalThumb */

/*
 * PositionVerticalScrollThumb - draw the scroll thumb on the screen
 */
void PositionVerticalScrollThumb( window_id wn, linenum curr, linenum last )
{
    wind        *w;
    int         height;
    int         newpos;

    w = AccessWindow( wn );
    if( !w->has_gadgets || !w->has_border ) {
        ReleaseWindow( w );
        return;
    }
    height = w->y2 - w->y1-THUMB_START*2;
    if( height <= 0 ) {
        newpos = 0;
    } else if( curr == 1 ) {
        newpos = THUMB_START;
    } else {
        newpos = (int)(((long) (height-1) * curr) / last) + THUMB_START+1;
    }
    if( w->vert_scroll_pos != newpos ) {
        DrawVerticalThumb( w, GadgetString[ WB_RIGHTSIDE ] );
    }
    w->vert_scroll_pos = newpos;
    DrawVerticalThumb( w, GadgetString[ WB_THUMB ] );

    ReleaseWindow( w );

} /* PositionVerticalScrollThumb */


/*
 * PositionToNewThumbPosition - set new position in file based on thumb
 */
int PositionToNewThumbPosition( wind *w, int win_y )
{
    int         height;
    int         rc;
    linenum     lne,clne;

    if( win_y == w->vert_scroll_pos ) {
        return( ERR_NO_ERR );
    }
    win_y -= THUMB_START;
    if( win_y < 0 ) {
        return( ERR_NO_ERR );
    }
    height = w->y2 - w->y1-THUMB_START*2;
    if( win_y > height ) {
        return( ERR_NO_ERR );
    }
    if( height <=0 ) {
        return( ERR_NO_ERR );
    }
    lne = CurrentFile->fcb_tail->end_line;
    clne = (win_y*lne)/height;
    if( clne == 0L ) {
        clne = 1L;
    }
    DisplayMouse( FALSE );
    rc = GoToLineNoRelCurs( clne );
    DisplayMouse( TRUE );

    return( rc );

} /* PositionToNewThumbPosition */

void PositionHorizontalScrollThumb( window_id id, int left_column )
{
    id = id;
    left_column = left_column;
}
