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


VSCREEN* intern findvscreen( register ORD       row,
                             register ORD       col )
/***************************************************/
{
    register    UI_WINDOW*              wptr;

    for( wptr = UIData->area_head ; wptr != &UIData->blank; wptr = wptr->next ){
        if( ( row >= wptr->area.row ) &&
            ( row < wptr->area.row + wptr->area.height ) ) {
            if( ( col >= wptr->area.col ) &&
                ( col < wptr->area.col + wptr->area.width ) ) {
                return( wptr->parm );
            }
        }
    }
    return( NULL );
}


void global uivdirty( register VSCREEN  *vptr,
                      register SAREA    area )
/********************************************/
{
    area.row += vptr->area.row;
    area.col += vptr->area.col;
    dirtyarea( &(vptr->window), area );
}


void global uivsetactive( register VSCREEN *vptr )
/************************************************/
{
    okopen( vptr );
    if( ( vptr->flags & V_PASSIVE ) == 0 ) {
        frontwindow( &(vptr->window ) );
    }
}


void global uivsetcursor( register VSCREEN *vptr )
/************************************************/
{
    register    ORD                     row;
    register    ORD                     col;

    if( vptr != NULL ) {
        row = vptr->area.row + vptr->row;
        col = vptr->area.col + vptr->col;
        uisetcursor( row, col, vptr->cursor, -2 );
    } else {
        uioffcursor();
    }
}
