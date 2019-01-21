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
* Description:  Definitions needed by callers to internal string formatter
*               __prtf() for printf() style handling.
*
****************************************************************************/


#ifndef _PRINTF_H_INCLUDED
#define _PRINTF_H_INCLUDED

#include "specs.h"


#if defined(__QNX__)
    #define OUTC_PARM       int
#elif defined( __WIDECHAR__ )
    #define OUTC_PARM       wchar_t
#else
    #define OUTC_PARM       char
#endif

#if defined(__QNX__)
    #if defined(_M_I86)
      #if defined( __SMALL_DATA__ )
        #define __SLIB_CALLBACK _WCFAR __loadds
      #else
        #define __SLIB_CALLBACK _WCFAR
      #endif
    #else
        #define __SLIB_CALLBACK _WCFAR
        #pragma aux slib_callback_t __far __parm [__eax] [__edx] __modify [__eax __edx]
    #endif
#else
    #define __SLIB_CALLBACK
    #if defined( __WINDOWS_386__ )
        #ifdef __SW_3S
            #pragma aux slib_callback_t __modify [__eax __edx __ecx __fs __gs]
        #else
            #pragma aux slib_callback_t __modify [__fs __gs]
        #endif
    #endif
#endif
typedef void (__SLIB_CALLBACK slib_callback_t)( SPECS __SLIB *, OUTC_PARM );

#if defined( __STDC_WANT_LIB_EXT1__ ) && __STDC_WANT_LIB_EXT1__ == 1

extern int __F_NAME(__prtf_s,__wprtf_s)(
    void __SLIB     *dest,                  /* parm for use by out_putc    */
    const CHAR_TYPE * __restrict format,    /* pointer to format string    */
    va_list         args,                   /* pointer to pointer to args  */
    const char      **errmsg,               /* constraint violation msg    */
    slib_callback_t *out_putc );            /* character output routine    */

#else

extern int __F_NAME(__prtf,__wprtf)(
    void __SLIB     *dest,          /* parm for use by out_putc    */
    const CHAR_TYPE *format,        /* pointer to format string    */
    va_list         args,           /* pointer to pointer to args  */
    slib_callback_t *out_putc );    /* character output routine    */

#ifdef __QNX__
  #if defined(IN_SLIB)
extern int __F_NAME(__prtf_slib,__wprtf_slib)(
    void __SLIB     *dest,          /* parm for use by out_putc    */
    const CHAR_TYPE *format,        /* pointer to format string    */
    char            **args,         /* pointer to pointer to args  */
    slib_callback_t *out_putc,      /* character output routine    */
    int             ptr_size );     /* size of pointer in bytes    */

  #elif defined(_M_I86)

    extern int ( __far * ( __far *__f)) ();
    #define __prtf(a,b,c,d) __prtf_slib(a,b,c,d,sizeof(void *))
    #define __prtf_slib(a,b,c,d,e) ((int(__far *) (void __far *,const char __far *,char * __far *args,slib_callback_t *__out,int)) __f[24])(a,b,c,d,e)

  #endif
#endif

#endif  /* Safer C */

#endif
