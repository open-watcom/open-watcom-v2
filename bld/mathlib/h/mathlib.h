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

#define _RWD_matherr    __matherr_handler

extern  void    __fprem( double, double, int *, double * );
extern  int     __sgn( double );

extern  int    (*__matherr_handler)( struct _exception * );

 #if defined(_M_IX86)

  /* The _ModF routine takes near pointer arguments which for 16-bit targets
   * must be pointing into the stack.
   */
  #if defined(_M_I86)
    typedef double __based( __segname( "_STACK" ) )     *modf_arg;
  #else
    typedef double _WCNEAR                              *modf_arg;
  #endif

    extern  double  _atan87( double );
    extern  double  _exp87( double );
    extern  double  _log87( double );
    extern  double  _cos87( double );
    extern  double  _sin87( double );
    extern  double  _tan87( double );
    extern  double  __sqrt87( double );
    extern  double  __sqrtd( double );
    extern  void    _ModF( modf_arg, modf_arg );

    extern  double  __pow87_err( double, double, unsigned char );
    extern  double  __math87_err( double, unsigned char );
    extern  double  __log87_err( double, unsigned char );

    _WMRTLINK extern int    __matherr( struct _exception * );
 #endif

 #if defined( __WATCOMC__ ) && defined(_M_IX86)
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
  #endif
 #endif

#endif
