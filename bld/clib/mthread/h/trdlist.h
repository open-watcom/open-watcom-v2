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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#ifndef _TRDLIST_H_INCLUDED
#define _TRDLIST_H_INCLUDED

#include "threadid.h"

#if defined( __NETWARE__ )
 #if defined( _NETWARE_CLIB )
  #define GetCurrentThreadId()  (*__threadid())
 #elif defined( _NETWARE_LIBC )
  #define GetCurrentThreadId()  __GetSystemWideUniqueTID()
 #endif
#elif defined( __NT__ )
  extern DWORD                  __TlsIndex;
#elif defined( __QNX__ )
  #define GetCurrentThreadId()  (getpid())
#elif defined( __LINUX__ )
  extern sem_t                  __tls_sem;
  #define GetCurrentThreadId()  (gettid())
#elif defined( __RDOS__ )
  extern int                    __TlsIndex;
  #define GetCurrentThreadId()  (RdosGetThreadHandle())
  extern int __tls_alloc();
  extern void __tls_free(int index);
  extern void *__tls_get_value(int index);
  extern void __tls_set_value(int index, void *data);
  extern void __create_thread(void (*Start)(void *Param), int Prio, const char *Name, void *Param, int StackSize);

  #pragma aux __tls_alloc "*" __value [__eax] __modify [__ecx]
  #pragma aux __tls_free "*" __parm [__ecx] __modify [__eax]
  #pragma aux __tls_get_value "*" __parm [__ecx] value [__eax] __modify [__edx]
  #pragma aux __tls_set_value "*" __parm [__ecx] [__eax] __modify [__edx]
  #pragma aux __create_thread "*" __parm [__edx] [__ebx] [__edi] [__eax] [__ecx]
#elif defined( __RDOSDEV__ )
  #define GetCurrentThreadId()  (RdosGetThreadHandle())
#elif defined( __OS2__ )
  #if defined( __WARP__ )
    // OS/2 2.0
    #define GetCurrentThreadId()    (*__threadid())
    extern unsigned                 __threadstack(void);
  #else
    // OS/2 1.x
    #define GetCurrentThreadId()    (*_threadid)
  #endif
#endif

#if defined(_NETWARE_LIBC)
void __RemoveAllThreadData( void );
#endif

#if !defined( __QNX__ ) && !defined(__RDOSDEV__) && !defined(__RDOSDEV__)
// QNX and RDOS device-drivers doesn't maintain a list of allocated thread data blocks

// lookup thread data
thread_data *__GetThreadData( void );

// add to list of thread data
int __AddThreadData( _TID, thread_data * );

// remove from list of thread data
void __RemoveThreadData( _TID );

// mark each entry in list of thread data for resize
void __ResizeThreadDataList( void );

// realloc data for the current thread
thread_data *__ReallocThreadData( void );

// clean up list of thread data
void __FreeThreadDataList( void );
#endif

#endif
