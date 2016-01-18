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
#include <stddef.h>
#include "walloca.h"
#include "win.h"

/*
 * ResetWindow - close a window an re-create it
 */
vi_rc ResetWindow( window_id *wn )
{
    window      *w;
    char        *tmp;
    vi_rc       rc;

    w = WINDOW_FROM_ID( *wn );
    if( w->title != NULL ) {
        tmp = alloca( strlen( w->title ) + 1 );
        strcpy( tmp, w->title );
    } else {
        tmp = NULL;
    }
    CloseAWindow( *wn );
    rc = NewWindow2( wn, &editw_info );
    if( rc != ERR_NO_ERR ) {
        return( rc );
    }
    SetBorderGadgets( *wn, EditFlags.WindowGadgets );
    if( tmp != NULL ) {
        WindowTitle( *wn, tmp );
    }
    DCDisplayAllLines();
    return( ERR_NO_ERR );

} /* ResetWindow */

/*
 * Valid Dimension - see if a window has a valid dim or not
 */
bool ValidDimension( int x1, int y1, int x2, int y2, bool has_border )
{
    int lb;

    if( !has_border ) {
        lb = 0;
    } else {
        lb = 2;
    }

    if( x2 - x1 < lb || x2 >= EditVars.WindMaxWidth ) {
        return( false );
    }
    if( y2 - y1 < lb || y2 >= EditVars.WindMaxHeight ) {
        return( false );
    }
    if( x1 < 0 || y1 < 0 ) {
        return( false );
    }
    return( true );

} /* ValidDimension */

/*
 * GimmeWindow - find next avaliable window
 */
window_id GimmeWindow( void )
{
    window_id   wn;

    for( wn = 0; wn < MAX_WINDS; wn++ ) {
        if( WINDOW_FROM_ID( wn ) == NULL ) {
            return( wn );
        }
    }
    return( NO_WINDOW );

} /* GimmeWindow */

/*
 * AllocWindow - allocate a new window
 */
window *AllocWindow( window_id wn, int x1, int y1, int x2, int y2, bool has_border, bool has_gadgets,
                        bool accessed, vi_color bc1, vi_color bc2, vi_color tc, vi_color bgc )
{
    window      *tmp;
    int         width, height, size, i;

    width = x2 - x1 + 1;
    height = y2 - y1 + 1;
    size = width * height;

    tmp = MemAlloc( offsetof( window, overcnt ) + height );
    tmp->id = wn;
    tmp->has_gadgets = has_gadgets;
    tmp->accessed = ( accessed ) ? 1 : 0;
    tmp->text = MemAlloc( size * sizeof( char_info ) );
    tmp->overlap = MemAlloc( size * sizeof( window_id ) );
    tmp->whooverlapping = MemAlloc( size * sizeof( window_id ) );
    tmp->x1 = x1;
    tmp->x2 = x2;
    tmp->y1 = y1;
    tmp->y2 = y2;
    tmp->has_border = has_border;
    tmp->border_color1 = bc1;
    tmp->border_color2 = bc2;
    tmp->text_color = tc;
    tmp->background_color = bgc;
    tmp->width = width;
    tmp->height = height;
    tmp->text_lines = height;
    tmp->text_cols = width;
    if( has_border ) {
        tmp->text_lines -= 2;
        tmp->text_cols -= 2;
        tmp->vert_scroll_pos = THUMB_START;
    }
    for( i = 0; i < size; ++i ) {
        tmp->overlap[i] = NO_WINDOW;
        tmp->whooverlapping[i] = NO_WINDOW;
    }
    for( i = 0; i < height; ++i ) {
        tmp->overcnt[i] = 0;
    }
    WINDOW_TO_ID( wn, tmp );
    return( tmp );

} /* AllocWindow */

/*
 * NewWindow - build a new window
 */
vi_rc NewWindow( window_id *wn, int x1, int y1, int x2, int y2, bool has_border,
               vi_color bc1, vi_color bc2, type_style *s )
{
    window_id   new_wn;
    bool        has_mouse;

    if( !ValidDimension( x1, y1, x2, y2, has_border ) ) {
        return( ERR_WIND_INVALID );
    }

    new_wn = GimmeWindow();
    if( BAD_ID( new_wn ) ) {
        return( ERR_WIND_NO_MORE_WINDOWS );
    }

    has_mouse = DisplayMouse( false );

    AllocWindow( new_wn, x1, y1, x2, y2, has_border, false, false, bc1, bc2, s->foreground, s->background );

    MarkOverlap( new_wn );

    ClearWindow( new_wn );
    DrawBorder( new_wn );

    *wn = new_wn;
    DisplayMouse( has_mouse );
    return( ERR_NO_ERR );

} /* NewWindow */

/*
 * FreeWindow - free data associated with a window
 */
void FreeWindow( window *w )
{
    MemFree( w->text );
    MemFree( w->overlap );
    MemFree( w->whooverlapping );
    MemFree( w->title );
    MemFree( w->borderdata );
    MemFree( w );

} /* FreeWindow */

/*
 * CloseAWindow - close down specified window
 */
void CloseAWindow( window_id wn )
{
    window      *w;

    w = WINDOW_FROM_ID( wn );

    RestoreOverlap( wn, true );
    if( w->min_slot ) {
        MinSlots[w->min_slot - 1] = 0;
    }

    FreeWindow( w );

    WINDOW_TO_ID( wn, NULL );

} /* CloseAWindow */
