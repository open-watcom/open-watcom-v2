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
* Description:  Implementation of j0() Bessel function.
*
****************************************************************************/


#include "variety.h"
#include <math.h>
#include "pdiv.h"

extern  double _EvalPoly( double, const double *, int );

#define Two_over_pi  0.63661977236758134307
#define PI           3.14159265358979323846
#define PIby4   (PI/4)


static const double _J0P[] = {
     0.35307722170456043917e-10,
    -0.64943670893011936569e-7,
     0.55529137316071119492e-4,
    -0.28251741814863245632e-1,
     0.91931791066585313639e+1,
    -0.19344679360067813003e+4,
     0.25640543425244074556e+6,
    -0.20101723394107941321e+8,
     0.82932752137525404768e+9,
    -0.14343701164751473694e+11,
     0.58828674632868341423e+11
};

static const double _J0Q[] = {
    1.0,
    0.14643417762555995397e+4,
    0.99635360310006026750e+6,
    0.36346749346560087410e+9,
    0.58828674632868342934e+11
};

const double _P0P[] = {
    0.22399036697750964691e+4,
    0.11252515255664380514e+6,
    0.11035444210405851805e+7,
    0.30343163608475269982e+7,
    0.21807736478830516116e+7
};

const double _P0Q[] = {
    1.0,
    0.23403140106394541345e+4,
    0.11366275712613906048e+6,
    0.11068209412295707838e+7,
    0.30367122303337212508e+7,
    0.21807736478830516316e+7
};

const double _Q0P[] = {
    -0.44237584856933353443e+1,
    -0.16263421062270593149e+3,
    -0.12598828601325538670e+4,
    -0.28720316121456664352e+4,
    -0.17665456233802464644e+4
};

const double _Q0Q[] = {
    1.0,
    0.35655140058576330960e+3,
    0.11085805836751486682e+5,
    0.82274660980144657068e+5,
    0.18484510850351025264e+6,
    0.11305891989633581592e+6
};


_WMRTLINK double j0( double x )         /* Bessel function j0(x) */
/*****************************/
{
    double  ax, xx, y, z;

    ax = fabs( x );
    if( ax < 8.0 ) {
        y = x * x;
        z = PDIV( _EvalPoly( y, _J0P, 10 ),  _EvalPoly( y, _J0Q, 4 ) );
    } else {
        z = PDIV( 8.0 , ax );
        y = z * z;
        xx = ax - PIby4;
        z = sqrt( PDIV( Two_over_pi , ax ) ) *
           ( PDIV( cos( xx ) * _EvalPoly( y, _P0P, 4 ) , _EvalPoly( y, _P0Q, 5 ) )
       - z * sin( xx ) * (PDIV( _EvalPoly( y, _Q0P, 4 ) , _EvalPoly( y, _Q0Q, 5 ) )) );
    }
    return( z );
}
