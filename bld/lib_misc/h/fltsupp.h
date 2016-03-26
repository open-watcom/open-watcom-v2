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


#if defined( __MAKE_DLL_MATHLIB )
    #define DLLEXPIMP __declspec(dllexport)
#elif defined( __SW_BR )
    #define DLLEXPIMP __declspec(dllimport)
#else
    #define DLLEXPIMP
#endif

#ifdef __cplusplus
extern "C" {
#endif

    typedef void       _type_EFG_cnvs2d( char *buf, double *value );
#if defined( __cplusplus ) || defined( _MATHLIB )
  #ifdef _LONG_DOUBLE_
    typedef void       _type_EFG_cnvd2ld( dbl_arg, ld_arg );
  #endif
    typedef int        _type_EFG_cnvd2f( double *src, float *tgt );
    typedef void       _type_EFG_LDcvt( long_double *, CVT_INFO *, char * );
#endif
#if !defined( __cplusplus ) || defined( _MATHLIB )
    typedef FAR_STRING _type_EFG_Format( char *buffer, my_va_list *args, SPECS __SLIB *specs );
#endif

#if defined( __MAKE_DLL_MATHLIB ) || defined( __SW_BR )
        DLLEXPIMP extern _type_EFG_cnvs2d   *__get_EFG_cnvs2d( void );
  #if defined( __cplusplus ) || defined( _MATHLIB )
    #ifdef _LONG_DOUBLE_
        DLLEXPIMP extern _type_EFG_cnvd2ld  *__get_EFG_cnvd2ld( void );
    #endif
        DLLEXPIMP extern _type_EFG_cnvd2f   *__get_EFG_cnvd2f( void );
        DLLEXPIMP extern _type_EFG_LDcvt    *__get_EFG_LDcvt( void );
  #endif
  #if !defined( __cplusplus ) || defined( _MATHLIB )
        DLLEXPIMP extern _type_EFG_Format   *__get_EFG_Format( void );
  #endif
#else
        extern _type_EFG_cnvs2d         __cnvs2d;
  #if defined( __cplusplus ) || defined( _MATHLIB )
    #ifdef _LONG_DOUBLE_
        extern _type_EFG_cnvd2ld        __cnvd2ld;
    #endif
        extern _type_EFG_cnvd2f         __cnvd2f;
        extern _type_EFG_LDcvt          _LDcvt;
  #endif
  #if !defined( __cplusplus ) || defined( _MATHLIB )
        extern _type_EFG_Format         _EFG_Format;
  #endif
#endif

#ifdef __cplusplus
};
#endif
