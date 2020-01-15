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

#include "clibext.h"


void UIAPI uitextfield( VSCREEN *vs, ORD row, ORD col, unsigned field_len,
                        ATTR attr, LPC_STRING string, unsigned string_len )
/**************************************************************************/
{
    uisize          count;
    uisize          string_count;
    SAREA           dirty_area;

    if( field_len > 0 && vs->area.width > col ) {
        count = vs->area.width - col;
        if( count > field_len )
            count = field_len;
        string_count = count;
        if( string_count > string_len )
            string_count = string_len;
        okopen( vs );
        okline( row, col, count, vs->area );
        dirty_area.row = row;
        dirty_area.col = col;
        dirty_area.height = 1;
        dirty_area.width = count;
        uivdirty( vs, dirty_area );
        bstring( &(vs->window.buffer), row, col, attr, string, string_count );
        if( count > string_count ) {
            bfill( &(vs->window.buffer), row, col + string_count, attr, ' ', count - string_count );
        }
    }
}


void UIAPI uivtextput( VSCREEN *vs, ORD row, ORD col, ATTR attr, const char *string, unsigned field_len )
/*******************************************************************************************************/
{
    if( field_len == 0 && string != NULL ) {
        field_len = strlen( string );
    }
    uitextfield( vs, row, col, field_len, attr, string, field_len );
}


void UIAPI uivrawput( VSCREEN *vs, ORD row, ORD col, LP_PIXEL pixels, unsigned len )
/**********************************************************************************/
{
    SAREA           dirty_area;

    okopen( vs );
    okline( row, col, len, vs->area );
    dirty_area.row = row;
    dirty_area.col = col;
    dirty_area.height = 1;
    dirty_area.width = len;
    uivdirty( vs, dirty_area );
    braw( &(vs->window.buffer), row, col, pixels, len );
}
