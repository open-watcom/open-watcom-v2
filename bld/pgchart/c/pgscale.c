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
* Description:  Chart scaling routines.
*
****************************************************************************/


#include <string.h>
#include <math.h>
#include <float.h>
#include "pgvars.h"


static short tic_decimals( float interval )
//=========================================

{
    short               ticdecimals;

    if( 0.0f < interval && interval < 1.0f ) {
//      ticdecimals = ceil( -log10( interval ) ) + 1;
        _GR_log10( &interval );     // interval = log10( interval )
        interval = -interval;
        _GR_ceil( &interval );      // interval = ceil( interval )
        ticdecimals = interval + 1;
    } else if( 1.0f <= interval && interval < 10.0f ) {
        ticdecimals = 1;
    } else {
        ticdecimals = 0;
    }
    return( ticdecimals );
}


static void scale_factor( axistype _WCI86FAR *axis )
//=============================================

/*  Calculates an appropriate scale factor. */

{
    short               counter;

    counter = 0;
    axis->scalefactor = 1;
    for( ;; ) {
        if( ( axis->scalemax >= 1000 ) || ( axis->scalemin <= -1000 ) ) {
            axis->scalefactor *= 1000;
            axis->scalemax /= 1000;
            axis->scalemin /= 1000;
            axis->ticinterval /= 1000;
            counter++;
            if( counter == 3 ) {
                break;
            }
        } else {
            break;
        }
    }
    axis->ticdecimals = tic_decimals( axis->ticinterval );
    switch( counter ) {
        case 0:
            axis->scaletitle.title[ 0 ] = '\0';
            break;
        case 1:
            StringCopy( axis->scaletitle.title, "Thousands" );
            break;
        case 2:
            StringCopy( axis->scaletitle.title, "Millions" );
            break;
        case 3:
            StringCopy( axis->scaletitle.title, "Billions" );
    }
}


static float scale_ticinterval( axistype _WCI86FAR *axis, float max, float min )
//=========================================================================

/*  Calcuates the ticinterval based on the maximum and minimum value.   */

{
    float               difference;
    float               ds, tmp;
    float               interval[ 4 ] = { 1.0, 2.0, 2.5, 5.0 };
    int                 i, s;

    difference = max - min;
    if( difference > 0 ) {
        s = 0;
        for( ;; ) {
            if( ( difference / interval[ s ] ) > 7 ) {
                if( ++s >= 4 ) {
                    for( i = 0; i < 4; i++ ) interval[ i ] *= 10;
                    s = 0;
                }
            } else if( ( difference / interval[ s ] ) < 3 ) {
                if( --s < 0 ) {
                    for( i = 0; i < 4; i++ ) interval[ i ] /= 10;
                    s = 3;
                }
            } else {
                break;
            }
       }
    } else {
        s = 0; // was 1
    }

    axis->ticdecimals = tic_decimals( interval[ s ] );

    /* set step increment (adjust to precision) */
    ds = 1.0;
    for( i = 0; i < axis->ticdecimals; ++i ) {
        ds = ds * 10;
    }
//  return( ceil( interval[ s ] * ds ) / ds );
    tmp = interval[ s ] * ds;
    _GR_ceil( &tmp );       // tmp = ceil( tmp )
    return( tmp / ds );
}


static void scalemin_max( axistype _WCI86FAR *axis, float max, float min )
//==================================================================

/*  Calculates the scalemin and scalemax.  Both start at zero and
    are incremented or decremented by the ticinterval until they go
    beyond the minimum or maximum value.
    Assumes scale_ticinterval has been executed already.    */

{
    axis->scalemin = 0;
    axis->scalemax = 0;
    for( ;; ) {
        if( axis->scalemin <= min ) {
            break;
        }
        axis->scalemin -= axis->ticinterval;
    }
    for( ;; ) {
        if( axis->scalemax >= max ) {
            break;
        }
        axis->scalemax += axis->ticinterval;
    }
}


void _pg_autoscale( short charttype, short chartstyle, axistype _WCI86FAR *axis,
                   float _WCI86FAR *values, short nseries, short n, short arraydim )
//============================================================================

/*  Calculates the scale min, max, factor, title, interval, format,
    and decimals.   */

{
    short               i, j;
    float               curr;
    float               max;
    float               min;
    float               pos_total;
    float               neg_total;

    max = 0;
    min = 0;
    if( ( ( charttype == _PG_BARCHART ) || ( charttype == _PG_COLUMNCHART ) ) &&
        ( chartstyle == _PG_STACKEDBARS ) ) {
        for( i = 0; i < n; ++i ) {
            pos_total = 0;
            neg_total = 0;
            for( j = 0; j < nseries; ++j ) {
                curr = *values;
                if( curr != _PG_MISSINGVALUE ) {
                    if( curr > 0 ) {
                        pos_total += curr;
                    } else {
                        neg_total += curr;
                    }
                }
                ++values;
            }
            if( pos_total > max ) {
                max = pos_total;
            }
            if( neg_total < min ) {
                min = neg_total;
            }
            values += arraydim - n;
        }
    } else {
        for( i = 0; i < nseries; ++i ) {
            for( j = 0; j < n; ++j ) {
                curr = *values;
                if( curr != _PG_MISSINGVALUE ) {
                    if( curr > max ) {
                        max = curr;
                    }
                    if( curr < min ) {
                        min = curr;
                    }
                }
                ++values;
            }
            values += arraydim - n;
        }
    }
//  if( max < 0 ) {     // max & min set to 0 above
//      max = 0;
//  } else {
//      if( min > 0 ) {
//          min = 0;
//      }
//  }
    axis->ticinterval = scale_ticinterval( axis, max, min );
    scalemin_max( axis, max, min );
    scale_factor( axis );
    axis->ticformat = _PG_DECFORMAT;
}
