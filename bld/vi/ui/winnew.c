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
#include <malloc.h>
#include "vi.h"
#include "win.h"

/*
 * ResetWindow - close a window an re-create it
 */
int ResetWindow( window_id *wn )
{
    wind        *w;
    char        *tmp;
    int         rc;

    w = Windows[ *wn ];
    if( w->title != NULL ) {
        tmp = alloca( strlen( w->title ) + 1 );
        strcpy( tmp, w->title );
    } else {
        tmp = NULL;
    }
    CloseAWindow( *wn );
    rc = NewWindow2( wn, &editw_info );
    if( rc ) {
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
bool ValidDimension( int x1, int y1, int x2, int y2 , bool has_border )
{
    int lb;

    if( !has_border) {
        lb = 0;
    } else {
        lb = 2;
    }

    if( x2-x1 < lb || x2 >= WindMaxWidth ) {
        return( FALSE );
    }
    if( y2-y1 < lb || y2 >= WindMaxHeight ) {
        return( FALSE );
    }
    if( x1 < 0 || y1 < 0 ) {
        return( FALSE );
    }
    return( TRUE );

} /* ValidDimension */

/*
 * GimmeWindow - find next avaliable window
 */
window_id GimmeWindow( void )
{
    window_id   i;

    for( i=0; i<MAX_WINDS; i++ ) {
        if( Windows[i] == NULL ) {
            return( i );
        }
    }
    return( -1 );

} /* GimmeWindow */

/*
 * AllocWindow - allocate a new window
 */
wind *AllocWindow( int x1, int y1, int x2, int y2, bool has_border,
                        int bc1, int bc2, int tc, int bgc )
{
    wind        *tmp;
    int         width,height,size;

    width = x2-x1+1;
    height = y2-y1+1;
    size = width*height;

    tmp = MemAlloc( WIND_SIZE + height );

    tmp->text = MemAlloc(sizeof(char_info)*size);
    tmp->overlap = MemAlloc(size);
    tmp->whooverlapping = MemAlloc(size);
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
    return( tmp );

} /* AllocWindow */

/*
 * NewWindow - build a new window
 */
int NewWindow( window_id *wn, int x1, int y1, int x2, int y2, bool has_border,
    int bc1, int bc2, type_style *s )
{
    wind        *w;
    window_id   i;
    bool        has_mouse;

    if( !ValidDimension( x1, y1, x2, y2, has_border ) ) {
        return( ERR_WIND_INVALID );
    }

    if( (i = GimmeWindow() ) < 0 ) {
        return( ERR_WIND_NO_MORE_WINDOWS );
    }

    has_mouse = DisplayMouse( FALSE );

    w = AllocWindow( x1,  y1,  x2,  y2,  has_border,  bc1,  bc2,
        s->foreground, s->background );
    w->id = i;

    Windows[i] = w;

    ResetOverlap( w );
    MarkOverlap( i );

    ClearWindow( i );
    DrawBorder( i );

    *wn = i;
    DisplayMouse( has_mouse );
    return( ERR_NO_ERR );

} /* NewWindow */

/*
 * FreeWindow - free data associated with a window
 */
void FreeWindow( wind *w )
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
    wind        *w;

    w = Windows[ wn ];

    RestoreOverlap( wn, TRUE );
    if( w->min_slot ) {
        MinSlots[ w->min_slot-1 ] = 0;
    }

    FreeWindow( w );

    Windows[wn] = NULL;

} /* CloseAWindow */
