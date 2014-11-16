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


#include "uidef.h"
#include "uidebug.h"

struct update_area {
    unsigned        start;
    unsigned        end;
};


static void dorefresh( struct update_area *total, SAREA area,
                           UI_WINDOW *wptr, UI_WINDOW *cover )
/************************************************************/
{
    register    int                     i;
    auto        SAREA                   areas[ 5 ];
    unsigned                            start;
    unsigned                            end;

    if( cover == NULL ) {
        start = area.row * UIData->width + area.col;
        end = start + (area.height-1) * UIData->width + (area.width-1);
        if( total->start > start ) total->start = start;
        if( total->end < end ) total->end = end;
        if( wptr->update != NULL ) {
            (*(wptr->update))( area, wptr->parm );
        }
    } else {
        dividearea( area, cover->area, areas );
        for( i = 1; i < 5; ++i ) {
            if( areas[ i ].height > 0 ) {
                dorefresh( total, areas[ i ], wptr, cover->prev );
            }
        }
    }
    return;
}

void UIAPI uidorefresh( void )
/*****************************/
{
    register    UI_WINDOW*              wptr;
    struct update_area                  total;
    SAREA                               area;
    int                                 start,end;

    _uicheckuidata();
    wptr = UIData->area_tail;
    total.start = (unsigned)-1;
    total.end = 0;
    while( wptr != NULL ) {
        if( wptr->dirty.height > 0 ) {
            dorefresh( &total, wptr->dirty, wptr, wptr->prev );
            wptr->dirty.height = 0;
        }
        wptr = wptr->prev;
    }
    if( total.start <= total.end ) {
        start = total.start/UIData->width;
        end = total.end/UIData->width;
        area.row = start;
        area.col = 0;
        area.height = end-start+1;
        area.width = UIData->width;
        physupdate( &area );
    }
    uionmouse();               /* redisplay mouse cursor (if it was on) */
}
