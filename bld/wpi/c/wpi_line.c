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
* Description:  WPI OS/2 line DDA routine.
*
****************************************************************************/


#define INCL_PM
#include <os2.h>
#include "wpi.h"


// Why doesn't this use integer (perhaps fixed point) math?

void _wpi_linedda( int x1, int y1, int x2, int y2,
                                    WPI_LINEDDAPROC line_proc, WPI_PARAM2 lp )
/****************************************************************************/
{
    float       m;
    float       b;
    int         x;
    int         y;

    // check for a verticle line
    if( x2 - x1 == 0 ) {

        // we want to draw from y1 to y2 ALWAYS
        if( y2 > y1 ) {
            for( y=y1; y < y2; ++y ) {
                line_proc( x1, y, lp );
            }
        } else {
            for( y=y1; y > y2; --y ) {
                line_proc( x1, y, lp );
            }
        }
    } else {
        m = (float)( ( (float)(y2 - y1) ) / ( (float)(x2 - x1) ) );
        b = (float)( (float)y1 - ( m * (float)x1 ) );

        if( abs( x2-x1 ) > abs( y2-y1 ) ) {
            // we want to draw from x1 to x2 ALWAYS!!
            if( x2 > x1 ) {
                for( x=x1; x < x2; ++x ) {
                    y = (int)( (m*(float)x) + b );
                    line_proc( x, y, lp );
                }
            } else {
                for( x=x1; x > x2; --x ) {
                    y = (int)( (m*(float)x) + b );
                    line_proc( x, y, lp );
                }
            }
        } else {
            // we want to draw from y1 to y2 ALWAYS!!
            if( y2 > y1 ) {
                for( y=y1; y < y2; ++y ) {
                    x = (int)( ((float)y - b) / m );
                    line_proc( x, y, lp );
                }
            } else {
                for( y=y1; y > y2; --y ) {
                    x = (int)( ((float)y - b) / m );
                    line_proc( x, y, lp );
                }
            }
        }
    }
} /* _wpi_linedda */
