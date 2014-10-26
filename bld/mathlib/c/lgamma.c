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
*    Translated from Fortran and based on specfun
*    http://www.netlib.org/specfun/algama
*
*  ========================================================================
*
* Description:  Returns the value of the logarithm of the Gamma function
*
****************************************************************************/

#include "variety.h"
#include <math.h>
#include <float.h>
#include "pi.h"
#include "_matherr.h"

/* Numerator and denominator coefficients for rational minimax
 *     approximation over (0.5,1.5).
 */
#define D1      -5.772156649015328605195174E-1
#define P11      4.945235359296727046734888E0
#define P12      2.018112620856775083915565E2
#define P13      2.290838373831346393026739E3
#define P14      1.131967205903380828685045E4
#define P15      2.855724635671635335736389E4
#define P16      3.848496228443793359990269E4
#define P17      2.637748787624195437963534E4
#define P18      7.225813979700288197698961E3

#define Q11      6.748212550303777196073036E1
#define Q12      1.113332393857199323513008E3
#define Q13      7.738757056935398733233834E3
#define Q14      2.763987074403340708898585E4
#define Q15      5.499310206226157329794414E4
#define Q16      6.161122180066002127833352E4
#define Q17      3.635127591501940507276287E4
#define Q18      8.785536302431013170870835E3


/* Numerator and denominator coefficients for rational minimax
 *     Approximation over (1.5,4.0).
 */
#define D2       4.227843350984671393993777E-1
#define P21      4.974607845568932035012064E0
#define P22      5.424138599891070494101986E2
#define P23      1.550693864978364947665077E4
#define P24      1.847932904445632425417223E5
#define P25      1.088204769468828767498470E6
#define P26      3.338152967987029735917223E6
#define P27      5.106661678927352456275255E6
#define P28      3.074109054850539556250927E6

#define Q21      1.830328399370592604055942E2
#define Q22      7.765049321445005871323047E3
#define Q23      1.331903827966074194402448E5
#define Q24      1.136705821321969608938755E6
#define Q25      5.267964117437946917577538E6
#define Q26      1.346701454311101692290052E7
#define Q27      1.782736530353274213975932E7
#define Q28      9.533095591844353613395747E6

/* Numerator and denominator coefficients for rational minimax
 *     Approximation over (4.0,12.0).
 */
#define D4       1.791759469228055000094023E0
#define P41      1.474502166059939948905062E4
#define P42      2.426813369486704502836312E6
#define P43      1.214755574045093227939592E8
#define P44      2.663432449630976949898078E9
#define P45      2.940378956634553899906876E10
#define P46      1.702665737765398868392998E11
#define P47      4.926125793377430887588120E11
#define P48      5.606251856223951465078242E11

#define Q41      2.690530175870899333379843E3
#define Q42      6.393885654300092398984238E5
#define Q43      4.135599930241388052042842E7
#define Q44      1.120872109616147941376570E9
#define Q45      1.488613728678813811542398E10
#define Q46      1.016803586272438228077304E11
#define Q47      3.417476345507377132798597E11
#define Q48      4.463158187419713286462081E11

/* Coefficients for minimax approximation over (12, INF). */
#define C1      -1.910444077728E-03
#define C2       8.4171387781295E-04
#define C3      -5.952379913043012E-04
#define C4       7.93650793500350248E-04
#define C5      -2.777777777777681622553E-03
#define C6       8.333333333333333331554247E-02
#define C7       5.7083835261E-03

#define PNT68    0.6796875E0

#define XBIG     2.55E305
#define XMININ   2.23E-308
#define XINF     _INFINITY
#define EPS      2.22E-16
#define FRTBIG   2.25E76

_WMRTLINK double lgamma_r( double x, int *sign )
{
    double y;
    double res;
    double corr;
    double xm1, xm2, xm4, ysq;
    double xden, xnum;

    if( sign != NULL )
        *sign = 1;
    if( isnan( x ) )
        return( x );
    
    y = x;
    if( y > 0 && y < XBIG ) {
        if( y < EPS ) {                 /* 0 < x < EPS */
            res = -log( y );
        } else if( y <= 1.5 ) {         /* EPS < x <= 1.5 */
            if( y < PNT68 ) {
                corr = -log( y );
                xm1 = y;
            } else {
                corr = 0.0;
                xm1 = ( y - 0.5 ) - 0.5;
            }
            if( y <= 0.5 || y >= PNT68 ) {
                xden = 1.0; xnum = 0.0;
                
                xnum = xnum * xm1 + P11;  xden = xden * xm1 + Q11;
                xnum = xnum * xm1 + P12;  xden = xden * xm1 + Q12;
                xnum = xnum * xm1 + P13;  xden = xden * xm1 + Q13;
                xnum = xnum * xm1 + P14;  xden = xden * xm1 + Q14;
                xnum = xnum * xm1 + P15;  xden = xden * xm1 + Q15;
                xnum = xnum * xm1 + P16;  xden = xden * xm1 + Q16;
                xnum = xnum * xm1 + P17;  xden = xden * xm1 + Q17;
                xnum = xnum * xm1 + P18;  xden = xden * xm1 + Q18;
                
                res = corr + xm1 * ( D1 + xm1 * ( xnum / xden ) );
            } else {
                xm2 = ( y - 0.5 ) - 0.5;
                
                xden = 1.0; xnum = 0.0;
                
                xnum = xnum * xm2 + P21;  xden = xden * xm2 + Q21;
                xnum = xnum * xm2 + P22;  xden = xden * xm2 + Q22;
                xnum = xnum * xm2 + P23;  xden = xden * xm2 + Q23;
                xnum = xnum * xm2 + P24;  xden = xden * xm2 + Q24;
                xnum = xnum * xm2 + P25;  xden = xden * xm2 + Q25;
                xnum = xnum * xm2 + P26;  xden = xden * xm2 + Q26;
                xnum = xnum * xm2 + P27;  xden = xden * xm2 + Q27;
                xnum = xnum * xm2 + P28;  xden = xden * xm2 + Q28;
                
                res = corr + xm2 * ( D2 + xm2 * ( xnum / xden ) );
            }
        } else if( y <= 4.0 ) {         /* 1.5 < x <= 4.0 */
            xm2 = y - 2.0;
            xden = 1.0; xnum = 0.0;
                
            xnum = xnum * xm2 + P21;  xden = xden * xm2 + Q21;
            xnum = xnum * xm2 + P22;  xden = xden * xm2 + Q22;
            xnum = xnum * xm2 + P23;  xden = xden * xm2 + Q23;
            xnum = xnum * xm2 + P24;  xden = xden * xm2 + Q24;
            xnum = xnum * xm2 + P25;  xden = xden * xm2 + Q25;
            xnum = xnum * xm2 + P26;  xden = xden * xm2 + Q26;
            xnum = xnum * xm2 + P27;  xden = xden * xm2 + Q27;
            xnum = xnum * xm2 + P28;  xden = xden * xm2 + Q28;
            
            res = xm2 * ( D2 + xm2 * ( xnum / xden ) );
        } else if( y <= 12.0 ) {        /* 4.0 < x <= 12.0 */
            xm4 = y - 4.0;
            xden = -1.0; xnum = 0.0;
                
            xnum = xnum * xm4 + P41;  xden = xden * xm4 + Q41;
            xnum = xnum * xm4 + P42;  xden = xden * xm4 + Q42;
            xnum = xnum * xm4 + P43;  xden = xden * xm4 + Q43;
            xnum = xnum * xm4 + P44;  xden = xden * xm4 + Q44;
            xnum = xnum * xm4 + P45;  xden = xden * xm4 + Q45;
            xnum = xnum * xm4 + P46;  xden = xden * xm4 + Q46;
            xnum = xnum * xm4 + P47;  xden = xden * xm4 + Q47;
            xnum = xnum * xm4 + P48;  xden = xden * xm4 + Q48;

            res = D4 + xm4 * ( xnum / xden );
        } else {                        /* 12.0 < x < XBIG  */
            res = 0.0;
            
            if( y <= FRTBIG ) {
                res = C7;
                ysq = y * y;
                
                res = res / ysq + C1;
                res = res / ysq + C2;
                res = res / ysq + C3;
                res = res / ysq + C4;
                res = res / ysq + C5;
                res = res / ysq + C6;
            }
            res = res / y;
            corr = log( y );
            res = res + LnSqrt2PI - 0.5 * corr;
            res = res + y * ( corr - 1.0 );
        }
    } else if( -XINF < y && y < 0 ) {   /* -XINF < y < 0 */
        res = log( fabs( tgamma( y ) ) );
    } else {
        __reporterror(SING, __func__, y, 0, XINF);
        return( XINF );
    }
    /* Set the sign parameter before leaving */
    if( sign != NULL ) {
        if( x < 0 ) {
            if( sin( PI * x ) < 0 ) {
                *sign = -1;
            }
        }
    }
    return( res );
}

_WMRTLINK double lgamma( double x )
{
    return( lgamma_r( x, &signgam ) );
}
