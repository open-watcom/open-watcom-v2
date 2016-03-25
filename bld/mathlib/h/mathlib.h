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
* Description:  Math library internal functions and data declarations.
*
****************************************************************************/


#ifndef _MATHLIB_H_INCLUDED
#define _MATHLIB_H_INCLUDED

#include <math.h>
#include "cplx.h"
#include "mathcode.h"

#define _RWD_matherr    __matherr_handler

#if !defined(_M_IX86) || defined(__FLAT__)
    typedef double                                      *m_dbl_arg;
    typedef int                                         *m_int_arg;
    typedef char                                        *m_char_arg;
#else
    typedef double __based( __segname( "_STACK" ) )     *m_dbl_arg;
    typedef int __based( __segname( "_STACK" ) )        *m_int_arg;
    typedef char __based( __segname( "_STACK" ) )       *m_char_arg;
#endif

#if defined(_M_IX86)

    extern  double  _atan87( double );
    extern  double  _exp87( double );
    extern  double  _log87( double );
    extern  double  _cos87( double );
    extern  double  _sin87( double );
    extern  double  _tan87( double );
    extern  double  __sqrt87( double );
    extern  double  __sqrtd( double );
    extern  void    _ModF( m_dbl_arg, m_dbl_arg );
    extern  void    _ZBuf2F( m_char_arg, m_dbl_arg );

    extern  double  __pow87_err( double, double, unsigned char );
    extern  double  __math87_err( double, unsigned char );
    extern  double  __log87_err( double, unsigned char );

    _WMRTLINK extern int    __matherr( struct _exception * );

 #if defined( __WATCOMC__ )
    #pragma aux __fprem     "*_" parm [];

    #pragma aux __matherr   "*";

  #if defined(__386__)
    #pragma aux _atan87    "_*" parm [edx eax] value [edx eax];
    #pragma aux _exp87     "_*" parm [edx eax] value [edx eax];
    #pragma aux _log87     "_*" parm [edx eax] value [edx eax];
    #pragma aux _cos87     "_*" parm [edx eax] value [edx eax];
    #pragma aux _sin87     "_*" parm [edx eax] value [edx eax];
    #pragma aux _tan87     "_*" parm [edx eax] value [edx eax];
    #pragma aux __sqrt87   "*"  parm [edx eax] value [edx eax];
    #pragma aux __sqrtd    "*"  parm [edx eax] value [edx eax];
    #pragma aux _ModF      "_*" parm caller [eax] [edx];
    #pragma aux _ZBuf2F    "_*" parm caller [eax] [edx];
  #else
    #pragma aux _atan87    "_*" parm [ax bx cx dx] value [ax bx cx dx];
    #pragma aux _exp87     "_*" parm [ax bx cx dx] value [ax bx cx dx];
    #pragma aux _log87     "_*" parm [ax bx cx dx] value [ax bx cx dx];
    #pragma aux _cos87     "_*" parm [ax bx cx dx] value [ax bx cx dx];
    #pragma aux _sin87     "_*" parm [ax bx cx dx] value [ax bx cx dx];
    #pragma aux _tan87     "_*" parm [ax bx cx dx] value [ax bx cx dx];
    #pragma aux __sqrt87   "*"  parm [ax bx cx dx] value [ax bx cx dx];
    #pragma aux __sqrtd    "*"  parm [ax bx cx dx] value [ax bx cx dx];
    #pragma aux _ModF      "_*" parm caller [ax] [dx];
    #pragma aux _ZBuf2F    "_*" parm caller [ax] [dx];
  #endif
 #endif

#endif

extern  int     (*__matherr_handler)( struct _exception * );

extern  void    __fprem( double, double, m_int_arg, m_dbl_arg );
extern  int     __sgn( double );
extern  double  _EvalPoly( double x, const double *poly, int degree );
extern  double  _OddPoly( double x, const double *poly, int degree );

extern  char    *__cvt( double value, int ndigits, int *dec, int *sign, int fmt, char *buf );

/*      The first parm to __...math1err and __...math2err is an unsigned int
        that indicates the math function, the type of error, and the
        expected result value.
*/
_WMRTLINK extern  double   __math1err( unsigned int, double *arg1 );
_WMRTLINK extern  double   __math2err( unsigned int, double *arg1, double *arg2 );
_WMRTLINK extern  xcomplex __xmath1err( unsigned int, xcomplex *arg1 );
_WMRTLINK extern  xcomplex __xmath2err( unsigned int, xcomplex *arg1, xcomplex *arg2 );
_WMRTLINK extern  complex  __zmath1err( unsigned int err_info, complex *arg1 );
_WMRTLINK extern  complex  __zmath2err( unsigned int err_info, complex *arg1, complex *arg2 );
_WMRTLINK extern  dcomplex __qmath1err( unsigned int err_info, dcomplex *arg1 );
_WMRTLINK extern  dcomplex __qmath2err( unsigned int err_info, dcomplex *arg1, dcomplex *arg2 );

#endif
