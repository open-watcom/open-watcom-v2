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


#ifndef _CPLX_H_INCLUDED
#define _CPLX_H_INCLUDED
#include "variety.h"
#include <math.h>
#include "watcom.h"

#ifndef TRUE
#define TRUE            1
#define FALSE           0
#endif


typedef    signed_8     logstar1;       // 8-bit logical
typedef    signed_32    logstar4;       // 32-bit logical
typedef    signed_8     intstar1;       // 8-bit integer
typedef    signed_16    intstar2;       // 16-bit integer
typedef    signed_32    intstar4;       // 32-bit integer

#ifndef __cplusplus
typedef    float        real;
typedef    real         single;         // single precision
typedef    long double  extended;       // extended precision

typedef struct {         // single precision complex
    single      realpart;
    single      imagpart;
} complex;

typedef struct {        // extended precision complex
    extended    realpart;
    extended    imagpart;
} xcomplex;
#endif

typedef struct {        // double precision complex
    double      realpart;
    double      imagpart;
} dcomplex;

#define NO_EXT_KEYS
#define __NO_MATH_OPS

#ifdef __cplusplus
extern "C" {
#endif

_WMRTLINK extern void       __rterrmsg( const int, const char * );
_WMRTLINK extern dcomplex   __qmath1err(unsigned int,dcomplex *);
_WMRTLINK extern dcomplex   __qmath2err(unsigned int,dcomplex *,dcomplex *);
_WMRTLINK extern complex    __zmath1err(unsigned int,complex *);
_WMRTLINK extern complex    __zmath2err(unsigned int,complex *,complex *);

_WMRTLINK extern dcomplex   _IF_C16Div( double r1, double i1, double r2, double i2 );
_WMRTLINK extern dcomplex   _IF_C16Mul( double r1, double i1, double r2, double i2 );
_WMRTLINK extern double     _IF_CDABS( double r, double i );
_WMRTLINK extern dcomplex   _IF_CDCOS( double r, double i );
_WMRTLINK extern dcomplex   _IF_CDEXP( double r, double i );
_WMRTLINK extern dcomplex   _IF_CDLOG( double r, double i );
_WMRTLINK extern dcomplex   _IF_C16Pow( double base_r, double base_i,
                                        double power_r, double power_i );
_WMRTLINK extern dcomplex   _IF_C16PowI( double a, double b, intstar4 i );
_WMRTLINK extern dcomplex   _IF_CDSIN( double r, double i );
_WMRTLINK extern dcomplex   _IF_CDSQRT( double r, double i );

#ifdef __cplusplus
};
#endif

#define DACOS( x )      acos( x )
#define DASIN( x )      asin( x )
#define DATAN( x )      atan( x )
#define DATAN2( x, y )  atan2( x, y )
#define DCOS( x )       cos( x )
#define DCOSH( x )      cosh( x )
#define DEXP( x )       exp( x )
#define DLOG( x )       log( x )
#define DSIN( x )       sin( x )
#define DSINH( x )      sinh( x )
#define DTAN( x )       tan( x )
#define DTANH( x )      tanh( x )

#define ACOS( x )      acos( x )
#define ASIN( x )      asin( x )
#define ATAN( x )      atan( x )
#define ATAN2( x, y )  atan2( x, y )
#define COS( x )       cos( x )
#define COSH( x )      cosh( x )
#define EXP( x )       exp( x )
#define LOG( x )       log( x )
#define SIN( x )       sin( x )
#define SINH( x )      sinh( x )
#define TAN( x )       tan( x )
#define TANH( x )      tanh( x )

#if defined( __386__ )
  #pragma aux rt_rtn parm routine [eax ebx ecx edx 8087];
  #if defined( __SW_3S )
    #if __WATCOMC__ < 900
      #pragma aux (rt_rtn) rt_rtn modify [8087 fs gs];
    #else
      #if defined( __FLAT__ )
        #pragma aux (rt_rtn) rt_rtn modify [8087 gs];
      #else
        #pragma aux (rt_rtn) rt_rtn modify [8087 es fs gs];
      #endif
    #endif
    #if defined( __FPI__ )
      #pragma aux (rt_rtn) flt_rt_rtn value [8087];
    #else
      #pragma aux (rt_rtn) flt_rt_rtn;
    #endif
  #else
    #pragma aux (rt_rtn) flt_rt_rtn;
  #endif
#elif defined(M_I86)
  #pragma aux rt_rtn parm [ax bx cx dx 8087];
  #pragma aux (rt_rtn) flt_rt_rtn;
#endif

#if defined(_M_IX86)
  #pragma aux (rt_rtn) _IF_powii "IF@PowII";
  #pragma aux (flt_rt_rtn) _IF_PowRR "IF@PowRR";
  #pragma aux (flt_rt_rtn) _IF_PowRI "IF@PowRI";
  #pragma aux (flt_rt_rtn) _IF_PowXI "IF@PowXI";
  #pragma aux (flt_rt_rtn) _IF_CDABS "IF@CDABS";
  #pragma aux (rt_rtn) _IF_CDCOS "IF@CDCOS";
  #pragma aux (rt_rtn) _IF_CDEXP "IF@CDEXP";
  #pragma aux (rt_rtn) _IF_CDLOG "IF@CDLOG";
  #pragma aux (rt_rtn) _IF_CDSIN "IF@CDSIN";
  #pragma aux (rt_rtn) _IF_CDSQRT "IF@CDSQRT";
  #pragma aux (rt_rtn) _IF_C8Mul "IF@C8Mul";
  #pragma aux (rt_rtn) _IF_C16Mul "IF@C16Mul";
  #pragma aux (rt_rtn) _IF_C32Mul "IF@C32Mul";
  #pragma aux (rt_rtn) _IF_C8Div "IF@C8Div";
  #pragma aux (rt_rtn) _IF_C16Div "IF@C16Div";
  #pragma aux (rt_rtn) _IF_C32Div "IF@C32Div";
  #pragma aux (rt_rtn) _IF_C8Pow "IF@C8Pow";
  #pragma aux (rt_rtn) _IF_C16Pow "IF@C16Pow";
  #pragma aux (rt_rtn) _IF_C32Pow "IF@C32Pow";
  #pragma aux (rt_rtn) _IF_C8PowI "IF@C8PowI";
  #pragma aux (rt_rtn) _IF_C16PowI "IF@C16PowI";
  #pragma aux (rt_rtn) _IF_C32PowI "IF@C32PowI";
#endif
#endif
