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
#include "pgvars.h"


void _CalcLegendWindow( chartenv _WCI86FAR *env,
                        char _WCI86FAR *_WCI86FAR *serieslabels, short nseries )
//====================================================================

/*  Calculates the size and location of the legendwindow.   */

{
    short               cols;
    short               rows;
    short               max_len;
    short               window_width;
    short               window_height;
    short               legend_width;
    short               legend_height;

    max_len = _MaxLabelLength( serieslabels, nseries, _XVECTOR );
    max_len += 3 * _CharWidth;
    window_width = env->chartwindow.x2 - env->chartwindow.x1 - 2 * _CharWidth;
    window_height = env->chartwindow.y2 - env->chartwindow.y1 - 2 * _CharHeight;
    if( env->legend.place == _PG_BOTTOM ) {
        cols = ( window_width - _CharWidth ) / max_len;
        if( cols <= 1 ) {
            cols = 1;
            rows = nseries;
        } else {
            rows = ( nseries - 1 ) / cols + 1;
            cols = ( nseries - 1 ) / rows + 1;   // balance columns
        }
    } else {
        rows = 2 * window_height / ( 3 * _CharHeight ) - 1;
        if( rows >= nseries ) {
            rows = nseries;
            cols = 1;
        } else {
            cols = ( nseries - 1 ) / rows + 1;
            rows = ( nseries - 1 ) / cols + 1;  // balance # of rows
        }
    }
    legend_width = _CharWidth + cols * max_len;
    legend_height = ( 3 * _CharHeight * ( rows + 1 ) ) / 2;
    if( env->legend.place == _PG_BOTTOM ) {
        env->legend.legendwindow.x1 = env->chartwindow.x1 + _CharWidth +
                                      ( window_width - legend_width ) / 2;
        env->legend.legendwindow.x2 = env->legend.legendwindow.x1 + legend_width;
        env->legend.legendwindow.y2 = env->chartwindow.y2 - _CharHeight;
        env->legend.legendwindow.y1 = env->legend.legendwindow.y2 - legend_height;
    } else {
        env->legend.legendwindow.y1 = env->datawindow.y1;
        env->legend.legendwindow.y2 = env->legend.legendwindow.y1 + legend_height;
        env->legend.legendwindow.x2 = env->chartwindow.x2 - _CharWidth;
        env->legend.legendwindow.x1 = env->legend.legendwindow.x2 - legend_width;
    }
}


short _DrawLegendWindow( chartenv _WCI86FAR *env,
                         char _WCI86FAR *_WCI86FAR *serieslabels, short nseries )
//=====================================================================

/*
    - yspace is the distance from the top of the legend to the
    top of the first category.

    yspace = 2 x ( window_height - num_of_rows x CharHeight )
                  ------------------------------------------
                              num_of_rows + 3

    - y is the top of the current category

    y = legendwindow.y1 + ( current_row - 1 ) x CharHeight +

                                                       current_row - 1
                                        yspace x ( 1 + --------------- )
                                                             2
    current_row = counter / num_of_columns + 1

    - width is the distance from the left of the legendwindow to the
    right of the first category. one width separates each of the
    categories

    width = legend_width - num_of_cols x maxlen
            -----------------------------------
                     num_of_cols + 1

    x = legendwindow.x1 + width x current_col + ( current_col - 1 ) x maxlen

    current_col = counter modulo num_of_cols
*/

{
    short               counter;
    short               cols;
    short               rows;
    short               max_len;
    short               x, y;
    short               legend_height;
    short               legend_width;
    float               width;
    float               yspace;
    char                charbuf[ 2 ];

    max_len = _MaxLabelLength( serieslabels, nseries, _XVECTOR );
    max_len += 2 * _CharWidth;
    legend_width = env->legend.legendwindow.x2 - env->legend.legendwindow.x1;
    legend_height = env->legend.legendwindow.y2 - env->legend.legendwindow.y1;
    cols = legend_width / ( max_len + _CharWidth ) ;
    if( cols <= 1 ) {
        cols = 1;
        rows = nseries;
    } else {
        rows = ( nseries - 1 ) / cols + 1;
        cols = ( nseries - 1 ) / rows + 1;   // balance columns
    }
    _setcliprgn( env->legend.legendwindow.x1, env->legend.legendwindow.y1,
                 env->legend.legendwindow.x2, env->legend.legendwindow.y2 );
    _pg_window( &env->legend.legendwindow );
    charbuf[ 1 ] = '\0';
    width = ( legend_width - cols * max_len ) / ( float )( cols + 1 );
    yspace = 2 * ( legend_height - rows * _CharHeight ) / ( float )( rows + 3 );
    for( counter = 0; counter < nseries; ++counter ) {
        x = env->legend.legendwindow.x1 +
            width * ( counter % cols + 1 ) +
            ( counter % cols ) * max_len;
        y = env->legend.legendwindow.y1 +
            ( counter / cols ) * _CharHeight +
            yspace * ( 1 + ( counter / cols ) / 2.0f );
        _setcolor( _PGPalette[ counter % 15 + 1 ].color );
        if( env->charttype == _PG_LINECHART || env->charttype == _PG_SCATTERCHART ) {
            charbuf[ 0 ] = _PGPalette[ counter % 15 + 1 ].plotchar;
            _moveto( x, y );
            _outgtext( charbuf );
        } else {
            _setfillmask( _PGPalette[ counter % 15 + 1 ].fill );
            _rectangle( _GFILLINTERIOR, x, y, x + _CharWidth, y + _CharHeight );
        }
        _setcolor( _PGPalette[ env->legend.textcolor ].color );
        _moveto( x + 2 * _CharWidth, y );
        _outgtext( serieslabels[ counter ] );
    }
    return( 0 );
}
