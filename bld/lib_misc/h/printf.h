/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Definitions needed by callers to internal string formatter
*               __prtf() for printf() style handling.
*
****************************************************************************/


#ifndef _PRINTF_H_INCLUDED
#define _PRINTF_H_INCLUDED

#include "specs.h"

#if defined( __QNX__ ) && !defined( __STDC_WANT_LIB_EXT1__ ) && !defined( __WIDECHAR__ )
    #define PRTF_CHAR_TYPE  int
    #define PRTF_CALLBACK   __SLIB_CALLBACK
    typedef void (__SLIB_CALLBACK prtf_callback_t)( PTR_SPECS, int );
  #if !defined(_M_I86)
    #pragma aux prtf_callback_t __far __parm [__eax] [__edx] __modify [__eax __edx]
  #endif
#else
    #define PRTF_CHAR_TYPE  CHAR_TYPE
    #define PRTF_CALLBACK
    typedef void prtf_callback_t( PTR_SPECS, CHAR_TYPE );
  #if defined( __WINDOWS_386__ )
    #ifdef __SW_3S
        #pragma aux prtf_callback_t __modify [__eax __edx __ecx __fs __gs]
    #else
        #pragma aux prtf_callback_t __modify [__fs __gs]
    #endif
  #endif
#endif

#if defined( __STDC_WANT_LIB_EXT1__ )
    extern int __F_NAME(__prtf_s,__wprtf_s)(
        void   PTR_PRTF_FAR dest,                   /* parm for use by out_putc    */
        const CHAR_TYPE     * __restrict format,    /* pointer to format string    */
        va_list             args,                   /* pointer to pointer to args  */
        const char          **errmsg,               /* constraint violation msg    */
        prtf_callback_t     *out_putc );            /* character output routine    */
#elif defined( __QNX__ ) && !defined( __STDC_WANT_LIB_EXT1__ ) && !defined( __WIDECHAR__ ) && defined( _M_I86 ) && !defined( IN_SLIB )
    extern int ( __far * ( __far *__f)) ();
    #define __prtf(a,b,c,d) __prtf_slib(a,b,&c,d,sizeof(void *))
    #define __prtf_slib(a,b,c,d,e) ((int(__far *) (void __far *,const char __far *,va_list __far *pargs,prtf_callback_t *__out,int)) __f[24])(a,b,c,d,e)
#else
  #if defined( __QNX__ ) && !defined( __STDC_WANT_LIB_EXT1__ ) && !defined( __WIDECHAR__ ) && defined( IN_SLIB )
    extern int __F_NAME(__prtf_slib,__wprtf_slib)(
        void   PTR_PRTF_FAR dest,           /* parm for use by out_putc    */
        const CHAR_TYPE     *format,        /* pointer to format string    */
        va_list             *args,          /* pointer to pointer to args  */
        prtf_callback_t     *out_putc,      /* character output routine    */
        int                 ptr_size );     /* size of pointer in bytes    */
  #endif
    extern int __F_NAME(__prtf,__wprtf)(
        void   PTR_PRTF_FAR dest,           /* parm for use by out_putc    */
        const CHAR_TYPE     *format,        /* pointer to format string    */
        va_list             args,           /* pointer to pointer to args  */
        prtf_callback_t     *out_putc );    /* character output routine    */
#endif

#endif
