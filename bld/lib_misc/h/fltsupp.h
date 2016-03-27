/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Declaration of internal OW floating-point run-time libraries
*                   support functions.
*
****************************************************************************/


#include "xfloat.h"
#include "myvalist.h"
#include "farsupp.h"
#include "printf.h"


#if defined( __MAKE_DLL_MATHLIB )
    #define FLTSUPPFUNC static
#else
    #define FLTSUPPFUNC
#endif

#ifdef __cplusplus
extern "C" {
#endif

// all
    typedef void        _type_EFG_cnvs2d( char *buf, double *value );
// CLIB
    typedef FAR_STRING  _type_EFG_Format( char *buffer, my_va_list *args, _mbcs_SPECS __SLIB *specs );
// CPPLIB
  #ifdef _LONG_DOUBLE_
    typedef void        _type_EFG_cnvd2ld( dbl_arg, ld_arg );
  #endif
    typedef int         _type_EFG_cnvd2f( double *src, float *tgt );
    typedef void        _type_EFG_cvtld( long_double *, CVT_INFO *, char * );

#if defined( __MAKE_DLL_MATHLIB ) || defined( __SW_BR )
// all
    extern _WMRTLINK _type_EFG_cnvs2d   *__get_EFG_cnvs2d( void );
// CLIB
    extern _WMRTLINK _type_EFG_Format   *__get_EFG_Format( void );
// CPPLIB
  #ifdef _LONG_DOUBLE_
    extern _WMRTLINK _type_EFG_cnvd2ld  *__get_EFG_cnvd2ld( void );
  #endif
    extern _WMRTLINK _type_EFG_cnvd2f   *__get_EFG_cnvd2f( void );
    extern _WMRTLINK _type_EFG_cvtld    *__get_EFG_cvtld( void );
#else
// all
    extern _type_EFG_cnvs2d             __cnvs2d;
// CLIB
    extern _type_EFG_Format             _EFG_Format;
  #if defined( __QNX__ )
    extern _type_EFG_Format             __EFG_Format;
  #endif
// CPPLIB
  #ifdef _LONG_DOUBLE_
    extern _type_EFG_cnvd2ld            __cnvd2ld;
  #endif
    extern _type_EFG_cnvd2f             __cnvd2f;
    extern _type_EFG_cvtld              __cvtld;
#endif

// CLIB
    typedef _type_EFG_cnvs2d            *_type_EFG_scanf;
    extern _WCRTDATA _type_EFG_scanf    __EFG_scanf;
    typedef _type_EFG_Format            *_type_EFG_printf;
    extern _WCRTDATA _type_EFG_printf   __EFG_printf;
// CPPLIB
    extern _WPRTDATA _type_EFG_cnvs2d   *__EFG_cnvs2d;
    extern _WPRTDATA _type_EFG_cnvd2f   *__EFG_cnvd2f;
    extern _WPRTDATA _type_EFG_cvtld    *__EFG_cvtld;
  #ifdef _LONG_DOUBLE_
    extern _WPRTDATA _type_EFG_cnvd2ld  *__EFG_cnvd2ld;
  #endif

#ifdef __cplusplus
};
#endif
