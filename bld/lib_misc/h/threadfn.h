/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Internal header with defintions to support the multi-thread
*               runtime library.
*
****************************************************************************/

#ifndef _THREADFN_H_INCLUDED
#define _THREADFN_H_INCLUDED

#if !defined( __DOS__ ) && !defined( __WINDOWS__ )

#if defined(_M_IX86)
    #include "extfunc.h"
#endif

typedef void            fthread_fn( void * );
typedef fthread_fn      __fthread_fn;

typedef void            *pthread_fn( void * );
typedef pthread_fn      __pthread_fn;

typedef void            _WCI86FAR thread_fn( void _WCI86FAR * );
typedef thread_fn       __thread_fn;
#ifdef __NT__
typedef unsigned        __stdcall thread_fnex( void * );
#endif

#if defined(_M_IX86)
    #pragma aux (__outside_CLIB) __fthread_fn;
    #pragma aux (__outside_CLIB) __pthread_fn;
    #pragma aux (__outside_CLIB) __thread_fn;
#endif

#endif  /* !defined( __DOS__ ) && !defined( __WINDOWS__ ) */

#endif  /* _THREADFN_H_INCLUDED */
