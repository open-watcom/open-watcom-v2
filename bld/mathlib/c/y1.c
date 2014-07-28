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
* Description:  Implementation of y1() Bessel function.
*
****************************************************************************/


#include "variety.h"
#include "mathlib.h"
#include "pdiv.h"
#include "pi.h"


static const double _Y1P[] = {
    -0.38226957797667741489e-11,
     0.75102718193645142780e-8,
    -0.69109541395518971099e-5,
     0.38153325235929440146e-2,
    -0.13598277402886548477e+1,
     0.31686918865308367843e+3,
    -0.47126666709422440249e+5,
     0.42115793110333106651e+7,
    -0.20149795564786231889e+9,
     0.40594714152021800256e+10,
    -0.14959278504009293871e+11
};

static const double _Y1Q[] = {
    1.0,
    0.15792524288657566178e+4,
    0.11492015934035314015e+7,
    0.44562247550599141276e+9,
    0.76300624755273366999e+11
};

extern const double _P1P[5];
extern const double _P1Q[6];
extern const double _Q1P[5];
extern const double _Q1Q[5];


_WMRTLINK double y1( double x )         /* Bessel function y1(x) */
/*****************************/
{
    double  xx, y, z;

    if( x < 0.0 ) {
        z = __math1err( FP_FUNC_Y1 | M_DOMAIN | V_NEG_HUGEVAL, &x );
    } else if( x < 8.0 ) {
        y = x * x;
        z = PDIV( x * _EvalPoly( y, _Y1P, 10 )  ,  _EvalPoly( y, _Y1Q, 4 ) )
               + TwobyPI * ( j1( x ) * log( x ) - PDIV( 1.0, x ) );
    } else {
        z = PDIV( 8.0, x );
        y = z * z;
        xx = x - ThreePIby4;
        z = sqrt( PDIV( TwobyPI, x ) ) *
            ( PDIV( sin( xx ) * _EvalPoly( y, _P1P, 4 ) , _EvalPoly( y, _P1Q, 5 ) )
        + z * cos( xx ) *(PDIV( _EvalPoly( y, _Q1P, 4 ) , _EvalPoly( y, _Q1Q, 4 ) )) );
    }
    return( z );
}
