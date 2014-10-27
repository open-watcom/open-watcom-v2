/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 2014 Open Watcom contributors. 
*    All Rights Reserved.
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
*    Translated from Fortran and based on slatec
*    http://www.netlib.org/slatec/fnlib/alnrel.f
*
*  ========================================================================
*
* Description:  Computes the natural logarithm of 1+x.
*
****************************************************************************/

#include "variety.h"
#include <math.h>
#include <float.h>
#include "_matherr.h"

#define XINF    _INFINITY

#define ALNRCS1   1.0378693562743770E0
#define ALNRCS2   -.13364301504908918E0 
#define ALNRCS3    .019408249135520563E0
#define ALNRCS4   -.003010755112753577E0
#define ALNRCS5    .000486946147971548E0
#define ALNRCS6   -.000081054881893175E0
#define ALNRCS7    .000013778847799559E0
#define ALNRCS8   -.000002380221089435E0
#define ALNRCS9    .000000416404162138E0
#define ALNRCS10  -.000000073595828378E0
#define ALNRCS11   .000000013117611876E0
#define ALNRCS12  -.000000002354670931E0
#define ALNRCS13   .000000000425227732E0
#define ALNRCS14  -.000000000077190894E0
#define ALNRCS15   .000000000014075746E0
#define ALNRCS16  -.000000000002576907E0
#define ALNRCS17   .000000000000473424E0
#define ALNRCS18  -.000000000000087249E0
#define ALNRCS19   .000000000000016124E0
#define ALNRCS20  -.000000000000002987E0
#define ALNRCS21   .000000000000000554E0
#define ALNRCS22  -.000000000000000103E0
#define ALNRCS23   .000000000000000019E0

static double _Chebyshev_Evaluate( double x, double *array, double n )
{
    double  b0, b1, b2, twox;
    int     i;

    b0 = 0.0;
    b1 = 0.0;
    twox = 2.0 * x;

    for( i = ( n - 1 ); i >= 0; i-- ) {
        b2 = b1;
        b1 = b0;
        b0 = twox * b1 - b2 + array[i];
    }
    return( 0.5 * ( b0 - b2 ) );
}

_WMRTLINK double log1p( double x )
{
    double array[] = {ALNRCS1,  ALNRCS2,  ALNRCS3,  ALNRCS4,  ALNRCS5,
                      ALNRCS6,  ALNRCS7,  ALNRCS8,  ALNRCS9,  ALNRCS10,
                      ALNRCS11, ALNRCS12, ALNRCS13, ALNRCS14, ALNRCS15,
                      ALNRCS16, ALNRCS17, ALNRCS18, ALNRCS19, ALNRCS20,
                      ALNRCS21, ALNRCS22, ALNRCS23};

    if( x == -1.0 ) {
        __reporterror(SING, __func__, x, 0, XINF);
        return( XINF );
    } else if( x < -1.0 ) {
        __reporterror(DOMAIN, __func__, x, 0, NAN);
        return( nan( "ignore" ) );
    }
        
    if( fabs( x ) <= 0.375 ) {
        return( x * ( 1.0 - x * _Chebyshev_Evaluate( x / 0.375, array, 23 ) ) );
    } else {
        return( log( 1.0 + x ) );
    }
}
