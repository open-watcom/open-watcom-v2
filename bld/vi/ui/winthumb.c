/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
 * DrawVerticalThumb - draw the scroll thumb on the screen
 */
void DrawVerticalThumb( window *w, char ch )
{
    char_info   what = {0, 0};
    int         addr;
    char_info   *txt;
    window_id   *over;
    unsigned    oscr;

    if( w->vert_scroll_pos < THUMB_START || EditFlags.Quiet ) {
        return;
    }

    txt = w->text;
    over = w->overlap;

    what.cinfo_char = ch;
    what.cinfo_attr = MAKE_ATTR( w, w->border_color1, w->border_color2 );

    addr = w->width - 1 + w->vert_scroll_pos * w->width;
    oscr = (w->x2) + (w->y1 + w->vert_scroll_pos) * EditVars.WindMaxWidth;

    WRITE_SCREEN_DATA( txt[addr], what );
    if( BAD_ID( over[addr] ) ) {
        WRITE_SCREEN( Scrn[oscr], what );
    }
#ifdef __VIO__
    MyVioShowBuf( oscr, 1 );
#endif

} /* DrawVerticalThumb */

/*
 * PositionVerticalScrollThumb - draw the scroll thumb on the screen
 */
void PositionVerticalScrollThumb( window_id wn, linenum curr, linenum last )
{
    window      *w;
    int         height;
    int         newpos;

    w = WINDOW_FROM_ID( wn );
    AccessWindow( w );
    if( !w->has_gadgets || !w->has_border ) {
        ReleaseWindow( w );
        return;
    }
    height = w->y2 - w->y1 - THUMB_START * 2;
    if( height <= 0 ) {
        newpos = 0;
    } else if( curr == 1 ) {
        newpos = THUMB_START;
    } else {
        newpos = (int)(((long) (height - 1) * curr) / last) + THUMB_START + 1;
    }
    if( w->vert_scroll_pos != newpos ) {
        DrawVerticalThumb( w, EditVars.GadgetString[WB_RIGHTSIDE] );
    }
    w->vert_scroll_pos = newpos;
    DrawVerticalThumb( w, EditVars.GadgetString[WB_THUMB] );

    ReleaseWindow( w );

} /* PositionVerticalScrollThumb */


/*
 * PositionToNewThumbPosition - set new position in file based on thumb
 */
vi_rc PositionToNewThumbPosition( window *w, int win_y )
{
    int         height;
    vi_rc       rc;
    linenum     lne, clne;

    if( win_y == w->vert_scroll_pos ) {
        return( ERR_NO_ERR );
    }
    win_y -= THUMB_START;
    if( win_y < 0 ) {
        return( ERR_NO_ERR );
    }
    height = w->y2 - w->y1 - THUMB_START * 2;
    if( win_y > height ) {
        return( ERR_NO_ERR );
    }
    if( height <= 0 ) {
        return( ERR_NO_ERR );
    }
    lne = CurrentFile->fcbs.tail->end_line;
    clne = (win_y * lne) / height;
    if( clne == 0L ) {
        clne = 1L;
    }
    DisplayMouse( false );
    rc = GoToLineNoRelCurs( clne );
    DisplayMouse( true );

    return( rc );

} /* PositionToNewThumbPosition */

void PositionHorizontalScrollThumb( window_id id, int left_column )
{
    id = id;
    left_column = left_column;
}
