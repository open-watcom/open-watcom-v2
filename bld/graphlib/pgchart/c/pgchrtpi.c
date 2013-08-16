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


#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include "pgvars.h"


#define PI 3.1415926536f


static void _DrawPercentLabel( short xc, short yc, short xradius,
            short yradius, float xratio, float yratio, float value )
//==================================================================

/*  Draws the percents on the pie graph.  This function was created to
    save some space.
*/

{
    short               x, y;
    short               length;
    short               pixels;
    char                buffer[ 10 ];

    _setcolor( _PGPalette[ 1 ].color );
    _StringPrint( buffer, _PG_DECFORMAT, 1, 100 * value );
    length = strlen( buffer );
    buffer[ length ] = '%';
    buffer[ length + 1 ] = '\0';
    pixels = _getgtextextent( buffer );
    x = xc + xratio * ( _CharWidth + xradius );
    y = yc - yratio * ( 3 * _CharHeight / 2 + yradius ) - _CharHeight / 2;
    if( xratio > 0 ) {
        _moveto( x, y );
    } else {
        _moveto( x - pixels, y );
    }
    _outgtext( buffer );
}


static void _DrawTwoPies( short x1, short y1, short x2, short y2,
                          short x3, short y3, short x4, short y4,
                          short xincrement, short yincrement )
//===============================================================

/*  Draws two pies.  One filled, the other border only.  This
    function was created so that the coordinates would only have
    to be calculated once.
*/

{
    x1 += xincrement;
    x2 += xincrement;
    x3 += xincrement;
    x4 += xincrement;
    y1 -= yincrement;
    y2 -= yincrement;
    y3 -= yincrement;
    y4 -= yincrement;
    _pie( _GFILLINTERIOR, x1, y1, x2, y2, x3, y3, x4, y4 );
    _pie( _GBORDER, x1, y1, x2, y2, x3, y3, x4, y4 );
}


static void _Radius( chartenv _WCI86FAR *env, short *xradius, short *yradius )
//=======================================================================

/*  Calculates the x and y radii.   */

{
    short               x1, y1;
    short               x2, y2;
    float               width;
    float               height;
    float               adj_height;
    struct videoconfig  vc;

    _getvideoconfig( &vc );
    x1 = env->datawindow.x1;
    x2 = env->datawindow.x2;
    y1 = env->datawindow.y1;
    y2 = env->datawindow.y2;
    if( env->chartstyle == _PG_PERCENT ) {      // leave room for labels
        x1 += 7 * _CharWidth;
        x2 -= 7 * _CharWidth;
        y1 += 3 * _CharHeight;
        y2 -= 3 * _CharHeight;
    }
    width = 0.9f * ( x2 - x1 );
    height = 0.9f * ( y2 - y1 );
    adj_height = width * ( 4 * vc.numypixels ) / ( 3 * vc.numxpixels );
    if( adj_height > height ) {
        width = height * ( 3 * vc.numxpixels ) / ( 4 * vc.numypixels );
    } else {
        height = adj_height;
    }
    *xradius = width / 2;
    *yradius = height / 2;
}


static void _DrawPie( chartenv _WCI86FAR* env,
                      float _WCI86FAR *values, short _WCI86FAR *explode, short n )
//======================================================================

/*  Uses shorts to graph the pie.

    +-----------+   -circle initially takes up 90% of the datawindow.
    |    |OO    |   -space is made ( maxlength ) for percent if needed
    |  OO|OO--  |   -initial vector is ( 1, 0 )
    | OOOO+-OOO |   -percents are printed out so that they either start or
    |  OOO|OOO  |    end at the bisecting angle of the segment
    |    O|O    |   -exploded segments are placed 1/10th of the radius away
    +-----------+    from the center along the bisecting angle of the segment
*/

{
    short               xradius, yradius;
    short               newx, newy;
    short               oldx, oldy;
    short               xc, yc;
    short               x1, y1;
    short               x2, y2;
    float               xratio, yratio;
    short               xincrement, yincrement;
    float               newangle, oldangle;
    float               total = 0;
    float               running_total = 0;
    float               tmp;
    short               i;

    _Radius( env, &xradius, &yradius );
    oldangle = 0;
    xc = ( env->datawindow.x1 + env->datawindow.x2 ) / 2;
    yc = ( env->datawindow.y1 + env->datawindow.y2 ) / 2;
    x1 = xc - xradius;
    y1 = yc - yradius;
    x2 = xc + xradius;
    y2 = yc + yradius;
    oldx = x2;
    oldy = yc;
    for( i = 0; i < n; i++ ) {
        if( values[ i ] != _PG_MISSINGVALUE && values[ i ] > 0 ) {
            total += values[ i ];
        }
    }
    _setlinestyle( _PGPalette[ 1 ].style );
    for( i = 0; i < n; ++i ) {
        if( values[ i ] != _PG_MISSINGVALUE && values[ i ] > 0 ) {
            running_total += values[ i ];
            newangle = 2.0f * PI * running_total / total;
//          newx = xc + cos( newangle ) * xradius;
            tmp = newangle;
            _GR_cos( &tmp );        // tmp = cos( tmp )
            newx = xc + tmp * xradius;
//          newy = yc - sin( newangle ) * yradius;
            tmp = newangle;
            _GR_sin( &tmp );        // tmp = sin( tmp )
            newy = yc - tmp * yradius;
//          xratio = cos( ( newangle + oldangle ) / 2 );
            xratio = ( newangle + oldangle ) / 2;
            _GR_cos( &xratio );    // xratio = cos( xratio )
//          yratio = sin( ( newangle + oldangle ) / 2 );
            yratio = ( newangle + oldangle ) / 2;
            _GR_sin( &yratio );    // yratio = sin( yratio )
            _setcolor( _PGPalette[ i % 15 + 1 ].color );
            _setfillmask( _PGPalette[ i % 15 + 1 ].fill );
            if( !explode[ i ] ) {
                xincrement = 0;
                yincrement = 0;
            } else {
                xincrement = xratio * xradius / 10;
                yincrement = yratio * yradius / 10;
            }
            _DrawTwoPies( x1, y1, x2, y2, oldx, oldy, newx, newy,
                                          xincrement, yincrement );
            if( env->chartstyle == _PG_PERCENT ) {
                _DrawPercentLabel( xc + xincrement, yc - yincrement, xradius,
                        yradius, xratio, yratio, values[ i ] / total );
            }
            oldangle = newangle;
            oldx = newx;
            oldy = newy;
        }
    }
}


_WCRTLINK short _WCI86FAR _CGRAPH _pg_chartpie( chartenv _WCI86FAR *env,
                         char _WCI86FAR *_WCI86FAR *categories,
                         float _WCI86FAR *values, short _WCI86FAR *explode, short n )
//=========================================================================

/*  _pg_chartpie accepts a chart environment variable, three arrays of data,
    and the number of data items then draws a pie graph based on the
    contents of the chartenv variable, the current palette, and the
    styleset variable. */

{
    short               error;
    grcolor             oldcolor;
    fillmap             oldfillmask;
    short               x1, y1;
    short               x2, y2;

    /* Record old settings. */
    _getcliprgn( &x1, &y1, &x2, &y2 );
    oldcolor = _getcolor();
    _getfillmask( &oldfillmask );

    error = _pg_analyzepie( env, categories, values, explode, n );
    if( error > 100 ) {
        return( error );
    }
    _CommonSetup( env, NULL, n );
    _DrawPie( env, values, explode, n );
    if( env->legend.legend ) {
        _DrawLegendWindow( env, categories, n );
    }

    /* Reset graphics attributes. */
    _setcliprgn( x1, y1, x2, y2 );
    _setcolor( oldcolor );
    _setfillmask( oldfillmask );
    return( error );
}

Entry( _PG_CHARTPIE, _pg_chartpie ) // alternate entry-point
