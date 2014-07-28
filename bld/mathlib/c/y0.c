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
* Description:  Implementation of y0() Bessel function.
*
****************************************************************************/


#include "variety.h"
#include "mathlib.h"
#include "pdiv.h"

#define Two_over_pi  0.63661977236758134307
#define PI           3.14159265358979323846
#define PIby4        (PI/4.)


static const double _Y0P[] = {
     0.27295402301235549348e+7,
    -0.27725001364628093891e+10,
     0.10869647137874907869e+13,
    -0.20430408868635630787e+15,
     0.18702004643462447944e+17,
    -0.75173702582406051323e+18,
     0.96736537146262430222e+19,
    -0.40421653126104925544e+19
};

static const double _Y0Q[] = {
    1.0,
    0.86456632023518903220e+3,
    0.50069757439413061879e+6,
    0.22400416108570432901e+9,
    0.80363630456265291118e+11,
    0.22900727639481849036e+14,
    0.49441312329373299270e+16,
    0.72620404690767951505e+18,
    0.54768700204477752393e+20
};

extern const double _P0P[5];
extern const double _P0Q[6];
extern const double _Q0P[5];
extern const double _Q0Q[6];


_WMRTLINK double y0( double x )         /* Bessel function y0(x) */
/*****************************/
{
    double  xx, y, z;

    if( x < 0.0 ) {
        z = __math1err( FP_FUNC_Y0 | M_DOMAIN | V_NEG_HUGEVAL, &x );
    } else if( x < 8.0 ) {
        y = x * x;
        z = PDIV( _EvalPoly( y, _Y0P, 7 ),  _EvalPoly( y, _Y0Q, 8 ) )
              + Two_over_pi * j0( x ) * log( x );
    } else {
        z = PDIV( 8.0, x );
        y = z * z;
        xx = x - PIby4;
        z = sqrt( PDIV( Two_over_pi, x ) ) *
            ( PDIV( sin( xx ) * _EvalPoly( y, _P0P, 4 ), _EvalPoly( y, _P0Q, 5 ) )
        + z * cos( xx ) *(PDIV( _EvalPoly( y, _Q0P, 4 ), _EvalPoly( y, _Q0Q, 5 ) )) );
    }
    return( z );
}
