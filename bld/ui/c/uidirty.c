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


void intern dirtyarea( UI_WINDOW *wptr, SAREA area )
/**************************************************/
{
    int diff;

    if( (int)wptr->dirty.height <= 0 ) {
        wptr->dirty = area;
    } else {
        diff = (int) wptr->dirty.row - (int) area.row;
        if( diff > 0 ) {
            wptr->dirty.height += diff;
            wptr->dirty.row -= diff;
        }
        diff = (int) wptr->dirty.col - (int) area.col;
        if( diff > 0 ) {
            wptr->dirty.width += diff;
            wptr->dirty.col -= diff;
        }
        diff = (int) area.height + (int) area.row
                    - (int) wptr->dirty.height - (int) wptr->dirty.row;
        if( diff > 0 ) {
            wptr->dirty.height += diff;
        }
        diff = (int) area.width + (int) area.col
                    - (int) wptr->dirty.width - (int) wptr->dirty.col;
        if( diff > 0 ) {
            wptr->dirty.width += diff;
        }
    }
}


void intern dirtynext( SAREA area, UI_WINDOW *wptr )
/**************************************************/
{
    register    int                     i;
    auto        SAREA                   areas[ 5 ];

    dividearea( area, wptr->area, areas );
    if( areas[ 0 ].height > 0 ) {
        dirtyarea( wptr, areas[ 0 ] );
    }
    if( wptr->next != NULL ) {
        for( i = 1 ; i < 5 ; ++i ) {
            if( areas[ i ].height > 0 ) {
                dirtynext( areas[ i ], wptr->next );
            }
        }
    }
}


void global uidirty( SAREA area )
/*******************************/
{
    dirtynext( area, UIData->area_head );
    uioffcursor();
}
