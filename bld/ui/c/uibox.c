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
#include "uidef.h"
#include "uibox.h"
#include "uigchar.h"

#include "clibext.h"


void intern drawbox( BUFFER *bptr, SAREA area, unsigned char *box, ATTR attr, int fill )
/**************************************************************************************/
{
    register    ORD                     row;

    bpixel( bptr, area.row, area.col, attr, BOX_CHAR( box, TOP_LEFT ) );
    bfill( bptr, area.row, area.col + 1, attr, BOX_CHAR( box, TOP_LINE ), area.width - 2 );
    bpixel( bptr, area.row, area.col + area.width - 1, attr, BOX_CHAR( box, TOP_RIGHT ) );
    for( row = area.row + 1 ; row < area.row + area.height - 1 ; ++row ) {
        bpixel( bptr, row, area.col, attr, BOX_CHAR( box, LEFT_LINE ) );
        if( fill ) {
            bfill( bptr, row, area.col + 1, attr, ' ', area.width - 2 );
        }
        bpixel( bptr, row, area.col + area.width - 1, attr, BOX_CHAR( box, RIGHT_LINE ) );
    }
    bpixel( bptr, row, area.col, attr, BOX_CHAR( box, BOTTOM_LEFT ) );
    bfill( bptr, row, area.col + 1, attr, BOX_CHAR( box, BOTTOM_LINE ), area.width - 2 );
    bpixel( bptr, row, area.col + area.width - 1, attr, BOX_CHAR( box, BOTTOM_RIGHT ) );
    physupdate( &area );
}



void intern blowup( BUFFER *bptr, SAREA area, unsigned char *box, ATTR attr )
/***************************************************************************/
{
//    register    ORD                     rows;
//    register    ORD                     cols;
    register    int                     inccol;
    register    int                     incrow;
    auto        SAREA                   grow;

//    rows = area.height - 2;
//    cols = area.width - 2;
    incrow = 1;
    inccol = 3;
    grow.row = area.row + area.height/2 - 1;
    grow.col = area.col + area.width/2 - 1;
    grow.height = 2;
    grow.width = 2;
    while( ( grow.height < area.height ) || ( grow.width < area.width ) ) {
        grow.row = (ORD)max( area.row, grow.row - incrow );
        grow.col = (ORD)max( area.col, grow.col - inccol );
        grow.height = (ORD)min( area.height, grow.height + 2*incrow );
        grow.width = (ORD)min( area.width, grow.width + 2*inccol );
        drawbox( bptr, grow, box, attr, TRUE );
        if( ( grow.height == area.height ) &&
            ( grow.width == area.width ) ) {
            break;
        }
    }
}

void uidrawbox( VSCREEN *vs, SAREA *area, ATTR attr, const char *title )
/**********************************************************************/
{
    int         length;
    int         col;

    if( area->width < 2 ) {
        return;
    }

    drawbox( &(vs->window.type.buffer), *area, SBOX_CHARS(), attr, FALSE );

    if( title == NULL ) {
        return;
    }

    length = strlen( title );
    col = area->col + 1;

    if( length >= area->width - 2 ) {
        length = area->width;
    }

    uivtextput( vs, area->row, col, attr, title, length );
}
