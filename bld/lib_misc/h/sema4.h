/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2017 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Semaphore related declarations for locking.
*
****************************************************************************/


#ifndef _SEMA4_H_INCLUDED
#define _SEMA4_H_INCLUDED

#if defined(__UNIX__)
#include <semaphore.h>
#elif defined(__RDOSDEV__)
#include "rdosdev.h"
#endif
#include "threadid.h"
#include "sema4typ.h"

/* Make sure these are in C linkage */
#ifdef __cplusplus
extern "C" {
#endif

/*
 * Note that this structure must not grow larger than 32bytes without
 * also updating the C++ runtime file prwdata.asm
 */
typedef struct  semaphore_object {
        _SEM            semaphore;
        unsigned        initialized;
        _TID            owner;
        unsigned        count;
} semaphore_object;

_WCRTLINK void __AccessSemaphore( semaphore_object * );
_WCRTLINK void __ReleaseSemaphore( semaphore_object * );
_WCRTLINK void __CloseSemaphore( semaphore_object * );

// the following is for the C++ library
#if defined( _M_I86 )
    #define _AccessSemaphore( sema ) __AccessSemaphore( sema )
    #define _ReleaseSemaphore( sema ) __ReleaseSemaphore( sema )
    #define _CloseSemaphore( sema ) __CloseSemaphore( sema )
#else
    _WCRTLINK extern void (*__AccessSema4)( semaphore_object *);
    _WCRTLINK extern void (*__ReleaseSema4)( semaphore_object *);
    _WCRTLINK extern void (*__CloseSema4)( semaphore_object *);
    #define _AccessSemaphore( sema ) __AccessSema4( sema )
    #define _ReleaseSemaphore( sema ) __ReleaseSema4( sema )
    #define _CloseSemaphore( sema ) __CloseSema4( sema )
#endif

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif
