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


void UIAPI uivmoveblock( VSCREEN *vs, SAREA area, int drow, int dcol )
/********************************************************************/
{
    BUFFER          *bptr;
    ORD             row;
    uisize          height;

    okopen( vs );
    bptr = &(vs->window.buffer);
    if( area.row < -drow ) {
        area.height += area.row + drow;
        area.row = -drow;
    }
    if( area.col < -dcol ) {
        area.width += area.col + dcol;
        area.col = -dcol;
    }
    if( area.height > vs->area.height - area.row - drow ) {
        area.height = vs->area.height - area.row - drow;
    }
    if( area.width > vs->area.width - area.col - dcol ) {
        area.width = vs->area.width - area.col - dcol;
    }
    oksubarea( area, vs->area );
    height = area.height;
    if( drow > 0 ) {
        row = area.row + area.height - 1;
        while( height-- > 0 ) {
            uibcopy( bptr, row, area.col, bptr, row + drow, area.col + dcol, area.width );
            row--;
        }
    } else {
        row = area.row;
        while( height-- > 0 ) {
            uibcopy( bptr, row, area.col, bptr, row + drow, area.col + dcol, area.width );
            row++;
        }
    }
    area.row += drow;
    area.col += dcol;
    uivdirty( vs, area );
}
