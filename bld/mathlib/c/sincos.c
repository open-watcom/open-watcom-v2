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


#include "variety.h"
#include "libsupp.h"
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include "ifprag.h"
#include "mathcode.h"
#include "rtdata.h"

#define PI              3.14159265358979323846
#define PIby2   (PI/2)
#define PIby4   (PI/4)

extern  double  _cos87(double);
extern  double  _sin87(double);
extern  double  _tan87(double);
extern  void    __fprem( double, double, int *, double * );
#if defined(__386__)
#pragma aux     _cos87  "_*" parm [edx eax] value [edx eax];
#pragma aux     _sin87  "_*" parm [edx eax] value [edx eax];
#pragma aux     _tan87  "_*" parm [edx eax] value [edx eax];
#pragma aux     __fprem "*_" parm [];
#elif defined(M_I86)
#pragma aux     _cos87  "_*" parm [ax bx cx dx] value [ax bx cx dx];
#pragma aux     _sin87  "_*" parm [ax bx cx dx] value [ax bx cx dx];
#pragma aux     _tan87  "_*" parm [ax bx cx dx] value [ax bx cx dx];
#pragma aux     __fprem "*_" parm [];
#endif

extern  double  _EvalPoly( double, const double *, int );
extern  double  _OddPoly( double, const double *, int );


static const double _sinpoly[] = {
         1.0 / (2.*3*4*5*6*7*8*9*10*11*12*13*14*15*16*17),
        -1.0 / (2.*3*4*5*6*7*8*9*10*11*12*13*14*15),
         1.0 / (2.*3*4*5*6*7*8*9*10*11*12*13),
        -1.0 / (2.*3*4*5*6*7*8*9*10*11),
         1.0 / (2.*3*4*5*6*7*8*9),
        -1.0 / (2.*3*4*5*6*7),
         1.0 / (2.*3*4*5),
        -1.0 / (2.*3),
         1.0
};

static const double _cospoly[] = {
         1.0 / (2.*3*4*5*6*7*8*9*10*11*12*13*14*15*16),
        -1.0 / (2.*3*4*5*6*7*8*9*10*11*12*13*14),
         1.0 / (2.*3*4*5*6*7*8*9*10*11*12),
        -1.0 / (2.*3*4*5*6*7*8*9*10),
         1.0 / (2.*3*4*5*6*7*8),
        -1.0 / (2.*3*4*5*6),
         1.0 / (2.*3*4),
        -1.0 / 2.,
        1.0
};

static const int Degree[]    = { 0, 1, 2, 3, 4, 4, 5, 5, 6 };


double __sincos( double x, int flag )
{
    int i;
    auto int exponent;
    auto int index;
    auto double y;
    double siny, cosy;
    double sinx, cosx;
    static const int func_name[] = {
            FUNC_SIN,
            FUNC_TAN,
            FUNC_COS
    };

    frexp( x, &exponent );
    if( exponent >= 32 ) {
//      return( _matherr( TLOSS, func_name[flag], &x, &x, 0.0 ) );
        return( __math1err( func_name[flag] | M_TLOSS | V_ZERO, &x ) );
    }
    __fprem( x, PIby4, &i, &y );    /* 24-nov-88 */
    i = (i + (flag & 2)) & 7;
    if( i & 1 ) {                   /* if odd octant */
        y = PIby4 - y;
    }
    frexp( y, &index );
/*  if( z < ldexp( 1.0, -26 ) ) { */
//  if( z < 1.49011611938476580e-008 ) {
    if( index < -26 ) {
        siny = y;
        cosy = 1.0;
    } else {
        if( index > 0 ) index = 0;
        index = - index;
        if( index > 8 ) index = 8;
        index = Degree[ index ];
        /* only calculate the necessary polynomials */
        if( ((i+1) & 2) || flag == 1 ) {
            cosy = _EvalPoly( y*y, &_cospoly[index], 8 - index );
        }
        if( (((i+1) & 2) == 0) || flag == 1 ) {
            siny = _OddPoly( y, &_sinpoly[index], 8 - index );
        }
    }
#if 0
    switch( i ) {
    case 0: sinx =   siny;  cosx =   cosy;  break;
    case 1: sinx =   cosy;  cosx =   siny;  break;
    case 2: sinx =   cosy;  cosx = - siny;  break;
    case 3: sinx =   siny;  cosx = - cosy;  break;
    case 4: sinx = - siny;  cosx = - cosy;  break;
    case 5: sinx = - cosy;  cosx = - siny;  break;
    case 6: sinx = - cosy;  cosx =   siny;  break;
    case 7: sinx = - siny;  cosx =   cosy;  break;
    }
#endif
    if( (i+1) & 2 ) {       /* if octants 1,2,5,6 */
        sinx = cosy;
    } else {                /* octants 0,3,4,7 */
        sinx = siny;
    }
    if( i & 4 )     sinx = - sinx;  /* octants 4,5,6,7 */
    if( flag == 1 ) {               /* if "tan" */

        /* cos is out of phase with sin by 2 octants */

        i += 2;
        if( (i+1) & 2 ) {       /* if octants 1,2,5,6 */
            cosx = cosy;
        } else {                /* octants 0,3,4,7 */
            cosx = siny;
        }
        if( i & 4 )     cosx = - cosx;  /* octants 4,5,6,7 */
        if( cosx == 0.0 ) {
            __set_ERANGE();
            if( sinx > 0.0 )  return( HUGE_VAL );
            return( - HUGE_VAL );
        }
        sinx = sinx/cosx;           /* calculate value of tan function */
    }
    if( exponent >= 28 ) {
//      return( _matherr( PLOSS, func_name[flag], &x, &x, sinx ) );
        return( __math2err( func_name[flag] | M_PLOSS, &x, &sinx ) );
    }
    return( sinx );
}



_WMRTLINK extern double _IF_dsin( double );
#if defined(_M_IX86)
  #pragma aux (if_rtn) _IF_sin "IF@SIN";
  #pragma aux (if_rtn) _IF_dsin "IF@DSIN";
#endif

_WMRTLINK float _IF_sin( float x )
/*********************/
{
    return( _IF_dsin( x ) );
}

_WMRTLINK double (sin)( double x )
/**********************/
{
    return( _IF_dsin( x ) );
}

_WMRTLINK double _IF_dsin( double x )
/************************/
{
    #if defined(_M_IX86)
        if( _RWD_real87 ) return( _sin87(x) );
    #endif
    return __sincos( x, 0 );
}




_WMRTLINK extern double _IF_dcos( double );
#if defined(_M_IX86)
  #pragma aux (if_rtn) _IF_cos "IF@COS";
  #pragma aux (if_rtn) _IF_dcos "IF@DCOS";
#endif

_WMRTLINK float _IF_cos( float x )
/*********************/
{
    return( _IF_dcos( x ) );
}

_WMRTLINK double (cos)( double x )
/**********************/
{
    return( _IF_dcos( x ) );
}

_WMRTLINK double _IF_dcos( double x )
/************************/
{
    #if defined(_M_IX86)
        if( _RWD_real87 ) return( _cos87(x) );
    #endif
    return __sincos( x, 2 );
}




_WMRTLINK extern double _IF_dtan( double );
#if defined(_M_IX86)
  #pragma aux (if_rtn) _IF_tan "IF@TAN";
  #pragma aux (if_rtn) _IF_dtan "IF@DTAN";
#endif

_WMRTLINK float _IF_tan( float x )
/*********************/
{
    return( _IF_dtan( x ) );
}

_WMRTLINK double (tan)( double x )
/**********************/
{
    return( _IF_dtan( x ) );
}

_WMRTLINK double _IF_dtan( double x )
/************************/
{
    #if defined(_M_IX86)
        if( _RWD_real87 ) return( _tan87(x) );
    #endif
    return __sincos( x, 1 );
}
