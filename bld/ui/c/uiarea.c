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


void intern dividearea( area, centre, areas )
/*******************************************/
/*
        This diagram shows the general case for the overlap of two
        areas. If there is only a partial overlap, areas 1 to 4 may
        degenerate to nothing and area 0 is just the overlapping area,
        not all of the centre area.

            area
          ooooooooooooooooooooooooooooooooo
          o                   .           o
          o     1             .           o
          o . . . . ooooooooooo           o
          o         o centre  o     2     o
          o         o         o           o
          o         o    0    o           o
          o    4    o         o           o
          o         ooooooooooo . . . . . o
          o         .                     o
          o         .              3      o
          ooooooooooooooooooooooooooooooooo
*/

register        SAREA                   area;
register        SAREA                   centre;
register        SAREA                   areas[ 5 ];
{
    register    char                    i;
    register    int                     row;
    register    int                     col;
    register    int                     arow;
    register    int                     acol;
    register    int                     crow;
    register    int                     ccol;
    register    int                     maxrow;
    register    int                     maxcol;
    register    int                     cmaxrow;
    register    int                     cmaxcol;
    register    int                     amaxrow;
    register    int                     amaxcol;

    arow = area.row;
    acol = area.col;
    crow = centre.row;
    ccol = centre.col;
    amaxrow = arow + area.height - 1;
    amaxcol = acol + area.width - 1;
    cmaxrow = crow + centre.height - 1;
    cmaxcol = ccol + centre.width - 1;
    for( i = 0 ; i < 5 ; ++i ) {
        switch( i ) {
        case 0:
            row = max( arow, crow );
            maxrow = min( amaxrow, cmaxrow );
            col = max( acol, ccol );
            maxcol = min( amaxcol, cmaxcol );
            break;
        case 1:
            row = arow;
            maxrow = min( amaxrow, crow - 1 );
            col = acol;
            maxcol = min( amaxcol, cmaxcol );
            break;
        case 2:
            row = arow;
            maxrow = min( amaxrow, cmaxrow );
            col = max( acol, cmaxcol + 1 );
            maxcol = amaxcol;
            break;
        case 3:
            row = max( arow, cmaxrow + 1 );
            maxrow = amaxrow;
            col = max( acol, ccol );
            maxcol = amaxcol;
            break;
        case 4:
            row = max( arow, crow );
            maxrow = amaxrow;
            col = acol;
            maxcol = min( amaxcol, ccol - 1 );
            break;
        }
        if( ( row > maxrow ) || ( col > maxcol ) ) {
            areas[ i ].height = 0;
        } else {
            areas[ i ].row = row;
            areas[ i ].col = col;
            areas[ i ].height = maxrow - row + 1;
            areas[ i ].width = maxcol - col + 1;
        }
    }
}
