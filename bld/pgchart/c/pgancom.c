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


static short _pg_error( chartenv _WCI86FAR *env, short n, short nseries )
//==================================================================

/*  Checks for errors:  _PG_NOTINITIALIZED  (102)   _PG_BADSCREENMODE   (103)
                        _PG_BADCHARTSTYLE   (4)     _PG_BADCHARTTYPE    (104)
                        _PG_BADLEGENDWINDOW (105)   _PG_BADCHARTWINDOW  (7)
                        _PG_BADDATAWINDOW   (107)   _PG_BADLOGBASE      (5)
                        _PG_BADSCALEFACTOR  (6)     _PG_TOOSMALLN       (109)
                        _PG_TOOFEWSERIES    (110)
*/

{
    short               error = 0;
    struct videoconfig  vc;

    // ???? check for not initialized
    _getvideoconfig( &vc );
    if( vc.numxpixels <= 0 ) {
        return( _PG_BADSCREENMODE );
    }
    if( env->charttype < 1 || env->charttype > 5 ) {
        return( _PG_BADCHARTTYPE );
    }
    if( n <= 0 ) {
        return( _PG_TOOSMALLN );
    }
    if( nseries <= 0 ) {
        return( _PG_TOOFEWSERIES );
    }
    if( env->chartstyle < 1 || env->chartstyle > 2 ) {
        env->chartstyle = 1;
        error = _PG_BADCHARTSTYLE;
    }
    if( env->charttype == _PG_SCATTERCHART || env->charttype == _PG_BARCHART ) {
        if( env->xaxis.logbase <= 0 ) {
            env->xaxis.logbase = 10;
            error = _PG_BADLOGBASE;
        }
        if( env->xaxis.scalefactor == 0 ) {
            env->xaxis.scalefactor = 1;
            error = _PG_BADSCALEFACTOR;
        }
    }
    if( env->charttype != _PG_PIECHART && env->charttype != _PG_BARCHART ) {
        if( env->yaxis.logbase <= 0 ) {
            env->yaxis.logbase = 10;
            error = _PG_BADLOGBASE;
        }
        if( env->yaxis.scalefactor == 0 ) {
            env->yaxis.scalefactor = 1;
            error = _PG_BADSCALEFACTOR;
        }
    }
    return( error );
}


short _analyze_datawindow( chartenv _WCI86FAR *env,
                           char _WCI86FAR *_WCI86FAR *categories, short n )
//===============================================================

/*  Handles the datawindow coordinates. */

{
    short               len;
    short               tmp;
    short               error;
    short               num_titles;
    char                buffer[ 30 ];
    struct videoconfig  vc;

    num_titles = 0;     // count y-axis titles
    if( env->yaxis.axistitle.title[ 0 ] != '\0' ) {
        ++num_titles;
    }
    if( env->yaxis.labeled && env->yaxis.scaletitle.title[ 0 ] != '\0' &&
        env->charttype != _PG_BARCHART ) {
        ++num_titles;
    }
    if( num_titles != 0 ) {
        env->datawindow.x1 += ( 2 * num_titles + 1 ) * _CharWidth;
    }

    if( env->yaxis.labeled ) {
        if( env->charttype == _PG_BARCHART ) {
            len = _MaxLabelLength( categories, n, _XVECTOR );
        } else {
            len = _StringPrint( buffer, env->yaxis.ticformat,
                                env->yaxis.ticdecimals, env->yaxis.scalemax );
            tmp = _StringPrint( buffer, env->yaxis.ticformat,
                                env->yaxis.ticdecimals, env->yaxis.scalemin );
            len = max( len, tmp );
        }
        _getvideoconfig( &vc );
        if( vc.numxpixels == 320 ) {    // 320 x 200 mode
            len += _CharWidth;          // - only leave 1 space
        } else {
            len += 2 * _CharWidth;
        }
        env->datawindow.x1 += len;
    }

    if( env->xaxis.axistitle.title[ 0 ] != '\0' ) {
        env->datawindow.y2 -= 2 * _CharHeight;
    }
    if( env->xaxis.labeled ) {
        _getvideoconfig( &vc );
        if( vc.numxpixels == 320 ) {    // 320 x 200 mode
            env->datawindow.y2 -= 7 * _CharHeight / 4;
        } else {
            env->datawindow.y2 -= 5 * _CharHeight / 2;
        }
        if( env->charttype == _PG_BARCHART ||
            env->charttype == _PG_SCATTERCHART ) {
            if( env->xaxis.scaletitle.title[ 0 ] != '\0' ) {
                env->datawindow.y2 -= 2 * _CharHeight;
            }
            env->datawindow.x2 -= _StringPrint( buffer, env->xaxis.ticformat,
                        env->xaxis.ticdecimals, env->xaxis.scalemax ) / 2;
        }
    }
    error = 0;
    if( env->datawindow.x1 >= env->datawindow.x2 ||
        env->datawindow.y1 >= env->datawindow.y2 ) {
        error = _PG_BADDATAWINDOW;
    }
    return( error );
}


short _analyze_common( chartenv _WCI86FAR *env,
                       float _WCI86FAR *xvalues, float _WCI86FAR *yvalues,
                       short nseries, short n,
                       short arraydim, char _WCI86FAR *_WCI86FAR *serieslabels )
//====================================================================

/*  Analyze common values for all chart types. */

{
    short               error;
    short               titles;
    short               y;

    error = _pg_error( env, n, nseries );
    if( error >= 100 ) {
        return( error );
    }
    if( ( ( env->charttype == _PG_BARCHART ) ||
        ( env->charttype == _PG_SCATTERCHART ) ) && ( env->xaxis.autoscale ||
        ( env->xaxis.scalemin >= env->xaxis.scalemax ) ||
        ( env->xaxis.ticinterval <= 0 ) ) ) {
        _pg_autoscale( env->charttype, env->chartstyle, &env->xaxis,
                                       xvalues, nseries, n, arraydim );
    }
    if( ( env->charttype != _PG_PIECHART ) && ( env->charttype != _PG_BARCHART ) &&
        ( env->yaxis.autoscale || ( env->yaxis.scalemin >= env->yaxis.scalemax ) ||
        ( env->yaxis.ticinterval <= 0 ) ) ) {
        _pg_autoscale( env->charttype, env->chartstyle, &env->yaxis,
                                       yvalues, nseries, n, arraydim );
    }

    // adjust chartwindow if necessary
    if( env->chartwindow.x1 >= env->chartwindow.x2 ||
        env->chartwindow.y1 >= env->chartwindow.y2 ) {
        if( env->chartwindow.x1 != 0 || env->chartwindow.y1 != 0 ) {
            error = _PG_BADCHARTWINDOW;
        }
        _getcliprgn( &env->chartwindow.x1, &env->chartwindow.y1,
                     &env->chartwindow.x2, &env->chartwindow.y2 );
    }

    // calculate top line of data window
    y = env->chartwindow.y1 + _CharHeight;
    titles = 0;
    if( env->maintitle.title[ 0 ] != '\0' ) {
        ++titles;
    }
    if( env->subtitle.title[ 0 ] != '\0' ) {
        ++titles;
    }
    if( titles == 1 ) {
        y += 2 * _CharHeight;
    } else if( titles == 2 ) {
        y += 7 * _CharHeight / 2;
    }
    env->datawindow.y1 = y;

    // calculate size of legend window
    if( env->legend.legend && env->legend.autosize ) {
        _CalcLegendWindow( env, serieslabels, nseries );
        if( env->legend.legendwindow.x1 >= env->legend.legendwindow.x2 ||
            env->legend.legendwindow.y1 >= env->legend.legendwindow.y2 ) {
            return( _PG_BADLEGENDWINDOW );
        }
    }

    // calculate rest of data window
    env->datawindow.x1 = env->chartwindow.x1 + _CharWidth;
    if( env->legend.legend && ( env->legend.place == _PG_RIGHT ) ) {
        env->datawindow.x2 = env->legend.legendwindow.x1 - _CharWidth;
    } else {
        env->datawindow.x2 = env->chartwindow.x2 - _CharWidth;
    }
    if( env->legend.legend && ( env->legend.place == _PG_BOTTOM ) ) {
        env->datawindow.y2 = env->legend.legendwindow.y1 - _CharHeight;
    } else {
        env->datawindow.y2 = env->chartwindow.y2 - _CharHeight;
    }
    if( env->datawindow.x1 >= env->datawindow.x2 ||
        env->datawindow.y1 >= env->datawindow.y2 ) {
        error = _PG_BADDATAWINDOW;
    }
    return( error );
}
