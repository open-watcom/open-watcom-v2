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
* Description:  Arc and pie functions for OS/2 (using floating-point).
*
****************************************************************************/


#include <math.h>

#define INCL_PM
#include <os2.h>
#include "wpi.h"


static FIXED calc_angle( WPI_POINT *cen, int x, int y )
/*****************************************************/
/* This function is used to calculate angles for _wpi_arc and _wpi_pie */
{
    float               dx, dy;
    float               angle;
    SHORT               whole;
//    USHORT            fract;

    dx = (float) x - cen->x;
    dy = (float) y - cen->y;

    angle = asin( dy / (sqrt( dx * dx + dy * dy )) ) * 180.0 / 3.14159265;
    if( dx < 0.0 ) {
        angle = 180.0 - angle;
    }

    if( angle < 0.0 ) {
        angle += 360.0;
    }

    whole = (SHORT) angle;
    /* Fractional part doesn't seem to give us any great advantage */
    /* In fact, it seems to cause more off by one errors */
//    fract = (USHORT) ((float) (angle - (float) whole) * 65536);

    return( MAKEFIXED( whole, 0 ) );
}


void _wpi_arc( WPI_PRES pres, int xx1, int yy1, int xx2, int yy2,
                                    int xx3, int yy3, int xx4, int yy4 )
/**********************************************************************/
{
    WPI_POINT           cen;
    WPI_POINT           vec;
    WPI_POINT           pt;
    ARCPARAMS           arcp;
    FIXED               start, end;
    unsigned long       a, b;           /* Length of axis of the ellipse */
    float               val;
    float               ratio;

    a = abs( xx2 - xx1 ) / 2;
    b = abs( yy2 - yy1 ) / 2;

    arcp.lP = a;
    arcp.lQ = b;
    arcp.lR = 0;
    arcp.lS = 0;
    GpiSetArcParams( pres, &arcp );

    cen.x = xx1 + (xx2 - xx1) / 2;
    cen.y = yy1 + (yy2 - yy1) / 2;

    start = calc_angle( &cen, xx3, yy3 );
    end = calc_angle( &cen, xx4, yy4 );

    /* Calculate the swing angle */
#if 0
    /* Using the fractional part seems to give us no advantage */
    /* In fact, it seems to cause more off by one errors */
    if( FIXEDINT( end ) >= FIXEDINT( start ) ) {
        end = MAKEFIXED( FIXEDINT( end ) - FIXEDINT( start ),
                                     FIXEDFRAC( end ) - FIXEDFRAC( start ) );
    } else {
        end = MAKEFIXED( 360 + FIXEDINT( end ) - FIXEDINT( start ),
                                     FIXEDFRAC( end ) - FIXEDFRAC( start ) );
    }
#else
    if( FIXEDINT( end ) >= FIXEDINT( start ) ) {
        end = MAKEFIXED( FIXEDINT( end ) - FIXEDINT( start ), 0 );
    } else {
        end = MAKEFIXED( 360 + FIXEDINT( end ) - FIXEDINT( start ), 0 );
    }
#endif

    /* We want a^2 and b^2 from now on */
    a *= a;
    b *= b;
    vec.x = xx3 - cen.x;
    vec.y = yy3 - cen.y;

    val = (float)(a * b) / (float)((unsigned) (b * vec.x * vec.x) +
                                           (unsigned) (a * vec.y * vec.y) );
    ratio = sqrt( val );
    pt.x = (ratio * vec.x) + cen.x;
    pt.y = (ratio * vec.y) + cen.y;

    GpiSetCurrentPosition( pres, &pt );
    GpiPartialArc( pres, &cen, MAKEFIXED( 1, 0 ), start, end );
} /* _wpi_arc */


void _wpi_pie( WPI_PRES pres, int xx1, int yy1, int xx2, int yy2,
                                     int xx3, int yy3, int xx4, int yy4 )
/***********************************************************************/
{
    WPI_POINT           cen;
    ARCPARAMS           arcp;
    FIXED               start, end;

    cen.x = xx1 + ( xx2 - xx1 ) / 2;
    cen.y = yy1 + ( yy2 - yy1 ) / 2;

    start = calc_angle( &cen, xx3, yy3 );
    end = calc_angle( &cen, xx4, yy4 );

    /* Calculate the swing angle */
#if 0
    /* Using the fractional part seems to give us no advantage */
    /* In fact, it seems to cause more off by one errors */
    if( FIXEDINT( end ) >= FIXEDINT( start ) ) {
        end = MAKEFIXED( FIXEDINT( end ) - FIXEDINT( start ),
                                     FIXEDFRAC( end ) - FIXEDFRAC( start ) );
    } else {
        end = MAKEFIXED( 360 + FIXEDINT( end ) - FIXEDINT( start ),
                                     FIXEDFRAC( end ) - FIXEDFRAC( start ) );
    }
#else
    if( FIXEDINT( end ) >= FIXEDINT( start ) ) {
        end = MAKEFIXED( FIXEDINT( end ) - FIXEDINT( start ), 0 );
    } else {
        end = MAKEFIXED( 360 + FIXEDINT( end ) - FIXEDINT( start ), 0 );
    }
#endif

    arcp.lP = (xx2 - xx1) / 2;
    arcp.lQ = (yy1 - yy2) / 2;
    arcp.lR = 0;
    arcp.lS = 0;
    GpiSetArcParams( pres, &arcp );

    GpiSetCurrentPosition( pres, &cen );

    GpiBeginArea( pres, BA_BOUNDARY | BA_ALTERNATE );

    GpiPartialArc( pres, &cen, MAKEFIXED( 1, 0 ), start, end );

    GpiEndArea( pres );
} /* _wpi_pie */
