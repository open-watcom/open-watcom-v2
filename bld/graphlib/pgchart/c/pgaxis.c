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
#include "pgvars.h"


static void Tick( char _WCI86FAR *buffer, short len, short x, short y,
                                                short tic_size, short dir )
//=========================================================================

//  Draw tic marks on an axis.

{
    _setlinestyle( _PGPalette[ 1 ].style );
    if( dir == _YVECTOR ) {
        _moveto( x - tic_size, y );
        _lineto( x - 1, y );
        _moveto( x - len - 2 * tic_size, y - _CharHeight / 2 );
    } else {
        _moveto( x, y + 1 );
        _lineto( x, y + tic_size );
        _moveto( x - len / 2, y + 3 * tic_size / 2 );
    }
    _outgtext( buffer );
}


static void Grid( chartenv _WCI86FAR *env, axistype _WCI86FAR *axis,
                                      short x, short y, short dir )
//=================================================================

// Draw grid lines

{
    _setlinestyle( _Style[ axis->gridstyle ] );
    if( dir == _YVECTOR ) {
        _moveto( env->datawindow.x1 + 1, y );
        _lineto( env->datawindow.x2 - 1, y );
    } else {
        _moveto( x, env->datawindow.y1 + 1 );
        _lineto( x, env->datawindow.y2 - 1 );
    }
}


static void _DrawScale( chartenv _WCI86FAR *env, axistype _WCI86FAR *axis,
                                            short tic_size, short dir )
//=====================================================================

/* Draws the axis scale tics and labels.   */

{
    char                buffer[ 30 ];
    float               val;
    short               x_pos, y_pos;
    short               len;

    for( val = axis->scalemin; val <= axis->scalemax; val += axis->ticinterval ) {
        len = _StringPrint( buffer, axis->ticformat, axis->ticdecimals, val );
        if( dir == _YVECTOR ) {
            y_pos = _YHeight( env, val );
            if( axis->labeled ) {
                Tick( buffer, len, env->datawindow.x1, y_pos, tic_size, dir );
            }
            if( axis->grid ) {
                Grid( env, axis, 0, y_pos, dir );
            }
        } else {
            x_pos = _XHeight( env, val );
            if( axis->labeled ) {
                Tick( buffer, len, x_pos, env->datawindow.y2, tic_size, dir );
            }
            if( axis->grid ) {
                Grid( env, axis, x_pos, 0, dir );
            }
        }
    }
}


static void _DrawCategories( chartenv _WCI86FAR *env, axistype _WCI86FAR *axis,
                             char _WCI86FAR *_WCI86FAR *categories,
                             short n, short tic_size, short dir )
//===============================================================

/*  Draws categories based on information passed as parameters.   */

{
    short               x_pos, y_pos;
    short               i;
    short               len;
    float               d;

    if( dir == _YVECTOR ) {     // find distance to first tic mark
        d = (float) ( env->datawindow.y2 - env->datawindow.y1 ) / ( 2 * n );
    } else {
        d = (float) ( env->datawindow.x2 - env->datawindow.x1 ) / ( 2 * n );
    }
    for( i = 0; i < n; ++i ) {
        len = _sLabelLength( categories[ i ], _XVECTOR );
        if( dir == _YVECTOR ) {
            y_pos = env->datawindow.y1 + ( 2 * i + 1 ) * d;
            if( axis->labeled ) {
                Tick( categories[ i ], len, env->datawindow.x1, y_pos, tic_size, dir );
            }
            if( axis->grid ) {
                Grid( env, axis, 0, y_pos, dir );
            }
        } else {
            x_pos = env->datawindow.x1 + ( 2 * i + 1 ) * d;
            if( axis->labeled ) {
                Tick( categories[ i ], len, x_pos, env->datawindow.y2, tic_size, dir );
            }
            if( axis->grid ) {
                Grid( env, axis, x_pos, 0, dir );
            }
        }
    }
}


void _Axes( chartenv _WCI86FAR *env, char _WCI86FAR *_WCI86FAR *categories, short n )
//====================================================================

/*  Draws the axis, axis labels, axis categories, and grid lines.   */

{
    short               tic_size;
    struct videoconfig  vc;

    _pg_line( env->datawindow.x1, env->datawindow.y2,
              env->datawindow.x2, env->datawindow.y2, env->xaxis.axiscolor, 1 );
    _pg_line( env->datawindow.x1, env->datawindow.y1,
              env->datawindow.x1, env->datawindow.y2, env->yaxis.axiscolor, 1 );
    _getvideoconfig( &vc );

    if( env->xaxis.labeled || env->xaxis.grid ) {
        tic_size = _CharHeight;
        if( vc.numxpixels == 320 ) {    // 320 x 200
            tic_size /= 2;
        }
        _setcolor( _PGPalette[ env->xaxis.axiscolor ].color );
        if( env->charttype == _PG_COLUMNCHART || env->charttype == _PG_LINECHART ) {
            _DrawCategories( env, &env->xaxis, categories, n, tic_size, _XVECTOR );
        } else {
            _DrawScale( env, &env->xaxis, tic_size, _XVECTOR );
        }
    }

    if( env->yaxis.labeled || env->yaxis.grid ) {
        tic_size = _CharWidth;
        if( vc.numxpixels == 320 ) {    // 320 x 200
            tic_size /= 2;
        }
        _setcolor( _PGPalette[ env->yaxis.axiscolor ].color );
        if( env->charttype == _PG_BARCHART ) {
            _DrawCategories( env, &env->yaxis, categories, n, tic_size, _YVECTOR );
        } else {
            _DrawScale( env, &env->yaxis, tic_size, _YVECTOR );
        }
    }
}
