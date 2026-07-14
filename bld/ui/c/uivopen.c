/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
#include "uidef.h"
#include "uibox.h"
#include "uigchar.h"

#include "clibext.h"


static void vs_update_fn( SAREA area, void *_vs )
/***********************************************/
{
    int             row;
    int             vrow;
    int             vcol;
    VSCREEN         *vs = (VSCREEN *)_vs;

    vcol = (int)area.col - (int)vs->area.col;
    for( row = area.row; row < area.row + area.height; ++row ) {
        vrow = row - (int)vs->area.row;
        uibcopy( &(vs->window.buffer), vrow, vcol, &UIData->screen, row, area.col, area.width );
    }
}


VSCREEN* UIAPI uivopen( VSCREEN *vs )
/***********************************/
{
    const char              *box;
    ATTR                    attr;
    int                     priority;
    uisize                  len;
    ORD                     col;
    screen_flags            flags;
    bool                    covered;
    SAREA                   area;

    okarea( vs->area );
    flags = vs->flags;
    area = vs->area;
    if( flags & V_DIALOGUE ) {
        if( flags & V_LISTBOX ) {
            box = SBOX_CHARS();
            attr = UIData->attrs[ATTR_NORMAL];
        } else {
            box = BOX_CHARS();
            attr = UIData->attrs[ATTR_DIAL_FRAME];
        }
        priority = P_DIALOGUE;
    } else {
        flags &= ~V_UNBUFFERED;
        box = SBOX_CHARS();
        attr = UIData->attrs[ATTR_FRAME];
        priority = P_VSCREEN;
    }
    if( ISFRAMED( flags ) ) {
        (area.row)--;
        (area.col)--;
        (area.height) += 2;
        (area.width) += 2;
        okarea( area );
    }
    if( flags & V_UNBUFFERED ) {
        priority = P_UNBUFFERED;
        bfake( &(vs->window.buffer), area.row, area.col );
    } else {
        balloc( &(vs->window.buffer), area.height, area.width );
    }
    if( vs->window.buffer.origin != NULL ) {
        if( flags & V_UNBUFFERED ) {
            vs->window.update_func = NULL;
        } else {
            vs->window.update_func = vs_update_fn;
        }
        vs->window.update_parm = vs;
        vs->window.area = area;
        vs->window.priority = priority;
        covered = openwindow( &(vs->window) );
        vs->flags = flags;
        if( ISFRAMED( flags ) ) {
            area.row = 0;
            area.col = 0;
            drawbox( &(vs->window.buffer), area, box, attr, false );
            if( vs->title != NULL ) {
#if 0
do not delete this stuff
                col = 0;
                len = area.width;
                bstring( &(vs->window.buffer), 0, col,
                         UIData->attrs[ATTR_CURR_SELECT_DIAL], " ", len );
                len = strlen( vs->name );
                if( len > area.width )
                    len = area.width;
                col = ( area.width - len ) / 2;
                bstring( &(vs->window.buffer), 0, col,
                         UIData->attrs[ATTR_CURR_SELECT_DIAL], vs->name, len );
#else
                len = strlen( vs->title );
                if( len > area.width )
                    len = area.width;
                col = ( area.width - len ) / 2;
                bstring( &(vs->window.buffer), 0, col, attr, vs->title, len );
#endif
            }
            bframe( &(vs->window.buffer) );
        }
        area = vs->area;
        area.row = 0;
        area.col = 0;
        vs->open = true;
        uivfill( vs, area, UIData->attrs[ATTR_NORMAL], ' ' );
        uivsetcursor( vs );

        return( vs );
    }
    return( NULL );
}


void UIAPI uivclose( VSCREEN *vs )
/********************************/
{
    if( vs->open ) {
        closewindow( &(vs->window) );
        if( ISBUFFERED( vs->flags ) ) {
            if( ISFRAMED( vs->flags ) ) {
                bunframe( &(vs->window.buffer) );
            }
            bfree( &(vs->window.buffer) );
        }
        vs->open = false;
    }
}

/*
 * uivresize -- change VSCREEN to have new SAREA, coping all screen data that
 *              persists instead of losing the data
 */

VSCREEN * UIAPI uivresize( VSCREEN *vs, SAREA new_area )
/******************************************************/
{
    BUFFER      old_buff;
    int         i;
    SAREA       old_area;
    int         min_width;
    int         min_height;

    if( vs->open ) {
        closewindow( &(vs->window) );
    }
    old_buff.origin = vs->window.buffer.origin;
    old_buff.increment = vs->window.buffer.increment;
    old_area = vs->area;
    if( balloc( &(vs->window.buffer), new_area.height, new_area.width ) ) {
        vs->area = new_area;
        if( ISFRAMED( vs->flags ) ) {
            (new_area.row)--;
            (new_area.col)--;
            (new_area.height) += 2;
            (new_area.width) += 2;
            okarea( new_area );
        }
        vs->window.area = new_area;
        okarea( vs->area );
        min_width = new_area.width;
        if( min_width > old_area.width )
            min_width = old_area.width;
        min_height = new_area.height;
        if( min_height > old_area.height )
            min_height = old_area.height;
        for( i = 0; i < min_height; i++ ) {
            uibcopy( &(old_buff), i, 0, &(vs->window.buffer), i, 0, min_width );
        }
        bfree( &old_buff );
        openwindow( &(vs->window) );
        return( vs );
    } else {
        vs->window.buffer.origin = old_buff.origin;
        vs->window.buffer.increment = old_buff.increment;
        vs->area = old_area;
        vs->window.area = old_area;
        openwindow( &(vs->window) );
        return( NULL );
    }
}

void UIAPI uivmove( VSCREEN *vs, ORD row, ORD col )
/*************************************************/
{
    int         rdiff;
    int         cdiff;

    rdiff = (int)row - (int)vs->area.row;
    cdiff = (int)col - (int)vs->area.col;
    vs->area.row = row;
    vs->area.col = col;
    okarea( vs->area );
    movewindow( &(vs->window), rdiff + vs->window.area.row, cdiff + vs->window.area.col );
}
