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


#include "uidef.h"
#include "uicurshk.h"


VSCREEN* intern findvscreen( ORD row, ORD col )
/*********************************************/
{
    UI_WINDOW   *wptr;

    for( wptr = UIData->area_head; wptr != &UIData->blank_window; wptr = wptr->next ) {
        if( ( row >= wptr->area.row ) && ( row < wptr->area.row + wptr->area.height ) ) {
            if( ( col >= wptr->area.col ) && ( col < wptr->area.col + wptr->area.width ) ) {
                return( wptr->update_parm );
            }
        }
    }
    return( NULL );
}


void UIAPI uivdirty( VSCREEN *vs, SAREA area )
/********************************************/
{
    area.row += vs->area.row;
    area.col += vs->area.col;
    dirtyarea( &(vs->window), area );
}


void UIAPI uivsetactive( VSCREEN *vs )
/************************************/
{
    okopen( vs );
    if( ( vs->flags & V_PASSIVE ) == 0 ) {
        frontwindow( &(vs->window ) );
    }
}


void UIAPI uivsetcursor( VSCREEN *vs )
/************************************/
{
    CURSORORD       crow;
    CURSORORD       ccol;

    if( vs != NULL ) {
        crow = vs->area.row + vs->cursor_row;
        ccol = vs->area.col + vs->cursor_col;
        uisetcursor( crow, ccol, vs->cursor_type, CATTR_VOFF );
    } else {
        uioffcursor();
    }
}
