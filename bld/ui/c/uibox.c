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


void intern drawbox( BUFFER *bptr, SAREA area, const char *box, ATTR attr, bool fill )
/************************************************************************************/
{
    uisize      row;

    bpixel( bptr, area.row, area.col, attr, BOX_CHAR( box, TOP_LEFT ) );
    if( area.width > 2 )
        bfill( bptr, area.row, area.col + 1, attr, BOX_CHAR( box, TOP_LINE ), area.width - 2 );
    bpixel( bptr, area.row, area.col + area.width - 1, attr, BOX_CHAR( box, TOP_RIGHT ) );
    for( row = area.row + 1; row < area.row + area.height - 1; ++row ) {
        bpixel( bptr, row, area.col, attr, BOX_CHAR( box, LEFT_LINE ) );
        if( fill && area.width > 2 ) {
            bfill( bptr, row, area.col + 1, attr, ' ', area.width - 2 );
        }
        bpixel( bptr, row, area.col + area.width - 1, attr, BOX_CHAR( box, RIGHT_LINE ) );
    }
    bpixel( bptr, row, area.col, attr, BOX_CHAR( box, BOTTOM_LEFT ) );
    if( area.width > 2 )
        bfill( bptr, row, area.col + 1, attr, BOX_CHAR( box, BOTTOM_LINE ), area.width - 2 );
    bpixel( bptr, row, area.col + area.width - 1, attr, BOX_CHAR( box, BOTTOM_RIGHT ) );
    physupdate( &area );
}

void uidrawbox( VSCREEN *vs, SAREA *area, ATTR attr, const char *title )
/**********************************************************************/
{
    uisize      field_len;

    if( area->width > 1 ) {
        drawbox( &(vs->window.buffer), *area, SBOX_CHARS(), attr, false );
        if( title != NULL ) {
            field_len = strlen( title );
            if( field_len > area->width - 2 ) {
                field_len = area->width - 2;
            }
            uivtextput( vs, area->row, area->col + 1, attr, title, field_len );
        }
    }
}

