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


void global uitextfield( VSCREEN *vptr, ORD row, ORD col, ORD len,
                         ATTR attr, char __FAR *string, int slen )
/****************************************************************/
{
    int             count;
    int             scount;
    SAREA           dirtyarea;

    if( len == 0 ) return;
    if( slen < 0 ) {
        slen = 0;
    }
    count = min( vptr->area.width - col, len );
    scount = min( count, slen );
    if( count > 0 ) {
        okopen( vptr );
        okline( row, col, count, vptr->area );
        dirtyarea.row = row;
        dirtyarea.col = col;
        dirtyarea.height = 1;
        dirtyarea.width = (ORD) count;
        uivdirty( vptr, dirtyarea );
        bstring( &(vptr->window.type.buffer), row, col, attr, string, scount );
        if( count > scount ) {
            bfill( &(vptr->window.type.buffer), row, col+scount, attr, ' ', count-scount );
        }
    }
}


void global uivtextput( register VSCREEN        *vptr,
                        register ORD            row,
                        register ORD            col,
                        register ATTR           attr,
                        register char           *string,
                        register int            len )
/***************************************************/
{
    if( len == 0 && string != NULL ) {
        len = strlen( string );
    }
    uitextfield( vptr, row, col, len, attr, string, len );
}


void global uivrawput( register VSCREEN *vptr,
                       register ORD     row,
                       register ORD     col,
                       register PIXEL   *pixels,
                       register int     len )
/*******************************************/
{
    auto        SAREA                   dirtyarea;

    okopen( vptr );
    okline( row, col, len, vptr->area );
    dirtyarea.row = row;
    dirtyarea.col = col;
    dirtyarea.height = 1;
    dirtyarea.width = (ORD) len;
    uivdirty( vptr, dirtyarea );
    braw( &(vptr->window.type.buffer), row, col, pixels, len );
}
