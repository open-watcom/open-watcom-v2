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


#include <stdio.h>
#include "uidef.h"

extern  void    cdecl intern    farattrib( LP_PIXEL, int, int, int );
extern  void    cdecl intern    farattrflip( LP_PIXEL, int, int );


void intern battribute( BUFFER *bptr, int row, int col,
                                      int height, int width, ATTR attr )
/**********************************************************************/
{
    register    LP_PIXEL                dst;
    register    bool                    snow;
    register    int                     r;

    snow = issnow( bptr );
    for( r = row ; r < row + height ; ++r ) {
        dst = bptr->origin + r * bptr->increment + col;
        farattrib( dst, attr, width, snow );
    }
}


void intern battrflip( BUFFER *bptr, int row, int col,
                                     int height, int width )
/**********************************************************/
{
    register    LP_PIXEL                dst;
    register    int                     r;
    register    bool                    snow;

    snow = issnow( bptr );
    for( r = row ; r < row + height ; ++r ) {
        dst = bptr->origin + r * bptr->increment + col;
        farattrflip( dst, width, snow );
    }
}
