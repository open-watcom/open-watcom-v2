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
#include <float.h>
#include "pgvars.h"


static void _DrawScatter( chartenv _WCI86FAR *env,
                          float _WCI86FAR *xvalues, float _WCI86FAR *yvalues,
                          short nseries, short n, short arraydim )
//================================================================

{
    short               i, j;
    short               x1, y1;
    short               x2, y2;
    short               pal;
    float               x_value, y_value;
    char                charbuf[ 2 ];

    charbuf[ 1 ] = '\0';
    for( i = 0; i < nseries; ++i ) {
        pal = i % 15 + 1;
        _setcolor( _PGPalette[ pal ].color );
        _setlinestyle( _PGPalette[ pal ].style );
        x1 = -1;        // in case 1st value is missing
        for( j = 0; j < n; ++j ) {
            x_value = xvalues[ arraydim * i + j ];
            y_value = yvalues[ arraydim * i + j ];
            if( x_value == _PG_MISSINGVALUE || y_value == _PG_MISSINGVALUE ) {
                continue;
            }
            x2 = _XHeight( env, _pg_scale( &env->xaxis, x_value ) );
            y2 = _YHeight( env, _pg_scale( &env->yaxis, y_value ) );
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


short _WCI86FAR _CGRAPH _pg_chartscatterms( chartenv _WCI86FAR *env,
                            float _WCI86FAR *xvalues,
                            float _WCI86FAR *yvalues, short nseries, short n,
                            short arraydim, char _WCI86FAR *_WCI86FAR *serieslabels )
//=========================================================================

{
    short               x1, y1;
    short               x2, y2;
    short               oldcolor;
    fillmap             oldfillmask;
    short               error;

    _getcliprgn( &x1, &y1, &x2, &y2 );
    oldcolor = _getcolor();
    _getfillmask( &oldfillmask );

    error = _pg_analyzescatterms( env, xvalues, yvalues,
                                  nseries, n, arraydim, serieslabels );
    if( error >= 100 ) {
        return( error );
    }

    _CommonSetup( env, NULL, n );
    _DrawScatter( env, xvalues, yvalues, nseries, n, arraydim );
    if( env->legend.legend ) {
        _DrawLegendWindow( env, serieslabels, nseries );
    }

    _setcliprgn( x1, y1, x2, y2 );
    _setcolor( oldcolor );
    _setfillmask( oldfillmask );
    return( error );
}

Entry( _PG_CHARTSCATTERMS, _pg_chartscatterms ) // alternate entry-point


short _WCI86FAR _CGRAPH _pg_chartscatter( chartenv _WCI86FAR *env,
                        float _WCI86FAR *xvalues, float _WCI86FAR *yvalues, short n )
//=========================================================================

{
    short               error;
    short               oldlegend;

    oldlegend = env->legend.legend;
    env->legend.legend = FALSE;

    error = _pg_chartscatterms( env, xvalues, yvalues, 1, n, n, NULL );

    env->legend.legend = oldlegend;
    return( error );
}

Entry( _PG_CHARTSCATTER, _pg_chartscatter ) // alternate entry-point
