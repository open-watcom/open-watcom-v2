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
* Description:  Implementation of j1() Bessel function.
*
****************************************************************************/


#include "variety.h"
#include <math.h>
#include "pdiv.h"

extern  double _EvalPoly( double, const double *, int );

#define Two_over_pi  0.63661977236758134307
#define PI           3.14159265358979323846
#define ThreePIby4   (PI*0.75)


static const double _J1P[] = {
    -0.60795301796074135994e+3,
     0.74370238171199964410e+6,
    -0.36866689870229816260e+9,
     0.93165652967246732049e+11,
    -0.12498203672620248530e+14,
     0.84824207447812726540e+15,
    -0.25123742147032127895e+17,
     0.22148878804219631392e+18
};

static const double _J1Q[] = {
    1.0,
    0.10777412894333043573e+4,
    0.69885861844850757440e+6,
    0.32819403445341964444e+9,
    0.11581921274668893293e+12,
    0.29898363077254871599e+14,
    0.51247127164848721123e+16,
    0.44297757608439262130e+18
};

const double _P1P[] = {
    -0.14279066288270981099e+4,
    -0.79574568713505959207e+5,
    -0.82850363738723774533e+6,
    -0.23669439140521428348e+7,
    -0.17469576694409285889e+7
};

const double _P1Q[] = {
     1.0,
    -0.13084529833390796875e+4,
    -0.78144050089391110834e+5,
    -0.82423699065628188501e+6,
    -0.23637451390226539850e+7,
    -0.17469576694409285700e+7
};

const double _Q1P[] = {
    0.36363466476034710809e-1,
    0.37994453796980673490e+1,
    0.51736532818365916365e+2,
    0.17442916890924258851e+3,
    0.14465282874995208675e+3
};

const double _Q1Q[] = {
    1.0,
    0.85223920643413403973e+2,
    0.11191098527047487025e+4,
    0.37343401060163017951e+4,
    0.30859270133323172311e+4
};


_WMRTLINK double j1( double x )         /* Bessel function j1(x) */
/*****************************/
{
    double  ax, xx, y, z;

    ax = fabs( x );
    if( ax < 8.0 ) {
        y = x * x;
        z = PDIV( x * _EvalPoly( y, _J1P, 7 )  , _EvalPoly( y, _J1Q, 7 ) );
    } else {
        z = PDIV( 8.0, ax );
        y = z * z;
        xx = ax - ThreePIby4;
        z = sqrt( PDIV( Two_over_pi, ax ) ) *
            ( PDIV( cos( xx ) * _EvalPoly( y, _P1P, 4 ) , _EvalPoly( y, _P1Q, 5 ) )
        - z * sin( xx ) * (PDIV( _EvalPoly( y, _Q1P, 4 ) , _EvalPoly( y, _Q1Q, 4 ) )) );
        if( x < 0.0 ) {
            z = - z;
        }
    }
    return( z );
}
