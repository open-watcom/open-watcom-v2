/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  C run-time library internal FPE signal handler declaration.
*
****************************************************************************/


#include "extfunc.h"

#if !defined( __UNIX__ )

#if defined( _W64 )
typedef void __sigfpe_func(int,int);
typedef void (* __sig_func)(int);
#endif

#if defined( _M_IX86 ) || defined( _W64 )
#define SIGFPE_CALL(x,y)    ((__clib_sigfpe_func)(x))(SIGFPE, (y))
#define SET_SIGFPE(x)       signal(SIGFPE, (__sig_func)(x))
#else
#define SIGFPE_CALL(x,y)    (x)(SIGFPE)
#define SET_SIGFPE(x)       signal(SIGFPE, (x))
#endif

#if defined( __WATCOMC__ )
typedef __sigfpe_func       *__clib_sigfpe_func;
#if defined( _M_IX86 )
    #pragma aux (__outside_CLIB) __sig_func;
    #pragma aux (__outside_CLIB) __clib_sigfpe_func;
#endif
#endif

#endif
