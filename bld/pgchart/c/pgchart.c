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
#include <float.h>
#include "pgvars.h"


float _Zero( axistype _WCI86FAR *axis )
//===============================

/*  Initializes "zline" to the line closest to zero on the graph.
    Modifies the "zvalue", the value closest to zero on the graph. */

{
    if( axis->scalemin > 0 ) {
        return( axis->scalemin );
    } else {
        if( axis->scalemax < 0 ) {
            return( axis->scalemax );
        } else {
            return( 0 );
        }
    }
}


static void _Zline( chartenv _WCI86FAR *env )
//======================================

/*  Draw a line where zero is on the graph.  */

{
    short               zline;

    if( env->charttype == _PG_BARCHART ) {
        zline = _XHeight( env, _Zero( &env->xaxis ) );
    } else {
        zline = _YHeight( env, _Zero( &env->yaxis ) );
    }
    if( env->charttype == _PG_BARCHART ) {
        _pg_line( zline, env->datawindow.y1 + 1, zline,
                         env->datawindow.y2 - 1, 1, 1 );
    } else {
        _pg_line( env->datawindow.x1 + 1, zline, env->datawindow.x2 - 1,
                         zline, 1, 1 );
    }
}


static void _LineChart( chartenv _WCI86FAR *env, float _WCI86FAR *values,
                        short nseries, short n, short arraydim )
//==============================================================

/*  Draws a line chart based on information in _PGPalette and
    the chartenv variable "env".  Restricts output to inside the
    datawindow.

    |   П          -screen is divided into "n" sections of equal width
    |  кйРП  П     and is offset by 1/2th of these widths
    | й   РПкйР
    |       й
    +-------------
*/

{
    short               x1, y1;
    short               x2, y2;
    short               i, j;
    short               pal;
    float               value;
    float               d;
    char                charbuf[ 2 ];

    charbuf[ 1 ] = '\0';
    d = (float) ( env->datawindow.x2 - env->datawindow.x1 ) / ( 2 * n );
    for( i = 0; i < nseries; ++i ) {
        pal = i % 15 + 1;
        _setcolor( _PGPalette[ pal ].color );
        _setlinestyle( _PGPalette[ pal ].style );
        x1 = -1;        // in case 1st value is missing
        for( j = 0; j < n; ++j ) {
            value = values[ arraydim * i + j ];
            if( value == _PG_MISSINGVALUE ) {
                continue;
            }
            x2 = env->datawindow.x1 + ( 2 * j + 1 ) * d;
            y2 = _YHeight( env, _pg_scale( &env->yaxis, value ) );
            if( env->chartstyle == _PG_POINTANDLINE && x1 != -1 ) {
                _moveto( x1, y1 );
                _lineto( x2, y2 );
            }
            charbuf[ 0 ] = _PGPalette[ pal ].plotchar;
            _moveto( x2 - _CharWidth / 2, y2 - _CharHeight / 2 );
            _outgtext( charbuf );
            x1 = x2;
            y1 = y2;
        }
    }
}


static void _BarChart( chartenv _WCI86FAR *env, float _WCI86FAR *values,
                       short nseries, short n, short arraydim )
//=============================================================

/*  Draws a column or bar chart.
        - datawindow is divided into n units along its width
        - a bar takes up 4/5ths of this unit
        - ( u, v ) ==> ( x, y ) for column chart
        - ( u, v ) ==> ( y, x ) for bar chart

        | лл                    |ллллллл
        | лл       лл           |
        | лл    лл лл           |лллл
        | лл лл лл лл           |
        | лл лл лл лл           |лллллллллл
        +-------------          +----------
         column chart            bar chart
*/

{
    short               u1, v1;
    short               u2, v2;
    short               i, j;
    short               u0, start_u;
    float               d;
    float               bars;
    float               bar_width;
    float               value;
    float               pos_v;
    float               neg_v;
    axistype _WCI86FAR       *axis;
    short               (*func)( chartenv _WCI86FAR *, float );

    if( env->charttype == _PG_COLUMNCHART ) {
        u0 = env->datawindow.x1;
        d = env->datawindow.x2 - u0;
        axis = &env->yaxis;
        func = &_YHeight;
    } else {
        u0 = env->datawindow.y1;
        d = env->datawindow.y2 - u0;
        axis = &env->xaxis;
        func = &_XHeight;
    }
    d /= 2 * n;
    bars = d * 1.6f;            // width of area covered by bars
    if( env->chartstyle == _PG_STACKEDBARS ) {
        bar_width = bars;       // width of an individual bar
    } else {
        bar_width = bars / nseries;
    }
    for( i = 0; i < n; ++i ) {
        start_u = u0 + 2 * i * d + ( 2 * d - bars ) / 2 + 1;
        u1 = start_u;
        u2 = u1 + bar_width - 1;
        pos_v = 0;
        neg_v = 0;
        v1 = (*func)( env, 0 );
        for( j = 0; j < nseries; ++j ) {
            value = values[ i + j * arraydim ];
            if( value == _PG_MISSINGVALUE ) {
                continue;
            }
            value = _pg_scale( axis, value );
            if( value == 0 ) {
                continue;
            }
            _setcolor( _PGPalette[ j % 15 + 1 ].color );
            _setfillmask( _PGPalette[ j % 15 + 1 ].fill );
            if( env->chartstyle == _PG_STACKEDBARS ) {
                if( value > 0 ) {
                    v1 = (*func)( env, pos_v );
                    pos_v += value;
                    v2 = (*func)( env, pos_v ) - 1;
                } else {
                    v1 = (*func)( env, neg_v );
                    neg_v += value;
                    v2 = (*func)( env, neg_v ) - 1;
                }
            } else {
                u2 = start_u + ( j + 1 ) * bar_width - 1;
                v2 = (*func)( env, value ) - 1;
            }
            if( env->charttype == _PG_COLUMNCHART ) {
                _rectangle( _GFILLINTERIOR, u1, v1, u2, v2 );
            } else {
                _rectangle( _GFILLINTERIOR, v1, u1, v2, u2 );
            }
            if( env->chartstyle != _PG_STACKEDBARS ) {
                u1 = u2 + 1;
            }
        }
    }
}


_WCRTLINK short _WCI86FAR _CGRAPH _pg_chartms( chartenv _WCI86FAR *env,
                        char _WCI86FAR *_WCI86FAR *categories,
                        float _WCI86FAR *values, short nseries, short n,
                        short arraydim, char _WCI86FAR *_WCI86FAR *serieslabels )
//=====================================================================

/*  _pg_chartms accepts a chart environment variable, one array of data,
    the number of data items, the number of series and the column dimension
    of the array being used and draws a bar, column or line graph
    based on the contents of the chartenv variable, the current palette,
    and the styleset variable.
*/

{
    short               error;
    short               x1, y1;
    short               x2, y2;
    short               oldcolor;
    fillmap             oldfillmask;

    /* Record initial settings. */
    _getcliprgn( &x1, &y1, &x2, &y2 );
    oldcolor = _getcolor();
    _getfillmask( &oldfillmask );

    error = _pg_analyzechartms( env, categories, values, nseries, n,
                                     arraydim, serieslabels );
    if( error >= 100 ) {
        return( error );
    }
    _CommonSetup( env, categories, n );
    if( env->charttype == _PG_LINECHART ) {
        _LineChart( env, values, nseries, n, arraydim );
    } else {
        _BarChart( env, values, nseries, n, arraydim );
    }
    _Zline( env );
    if( env->legend.legend ) {
        _DrawLegendWindow( env, serieslabels, nseries );
    }

    _setcliprgn( x1, y1, x2, y2 );
    _setcolor( oldcolor );
    _setfillmask( oldfillmask );
    return( error );
}

Entry( _PG_CHARTMS, _pg_chartms ) // alternate entry-point


_WCRTLINK short _WCI86FAR _CGRAPH _pg_chart( chartenv _WCI86FAR *env,
                              char _WCI86FAR *_WCI86FAR *categories,
                              float _WCI86FAR *values, short n )
//=========================================================

/*  _pg_chart accepts a chart environment variable, one array of data, and
    the number of data items then draws a graph based on the contents of
    the chartenv variable, the current palette, and the styleset variable. */

{
    short               error;
    short               oldlegend;

    oldlegend = env->legend.legend;
    env->legend.legend = FALSE;

    error = _pg_chartms( env, categories, values, 1, n, n, NULL );

    env->legend.legend = oldlegend;
    return( error );
}

Entry( _PG_CHART, _pg_chart ) // alternate entry-point
