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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#ifndef _TRDLIST_H_INCLUDED
#define _TRDLIST_H_INCLUDED

#if defined( _NETWARE_CLIB )
  #define TID                   int
  #define GetCurrentThreadId()  (*__threadid())
  extern void           ThreadSwitch( void );
  extern void           *GetThreadID( void );
#elif defined (_NETWARE_LIBC)
  #include "nw_libc.h"
#elif defined( __NT__ )
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
  #endif
  #include <windows.h>
  #include "ntex.h"
  extern DWORD          __TlsIndex;
  #define TID                   DWORD
#elif defined( __QNX__ )
  #include <sys/types.h>
  #include <unistd.h>
  #define TID                   pid_t
  #define GetCurrentThreadId()  (getpid())
#elif defined( __LINUX__ )
  // TODO: Include for Linux threading!
  #include <sys/types.h>
  #include <unistd.h>
  #define TID                   size_t
  #define GetCurrentThreadId()  (0)
#elif defined( __RDOS__ )
  #include <rdos.h>
  #define TID int
  extern int __TlsIndex;
  #define GetCurrentThreadId() (RdosGetThreadHandle())
  extern int __tls_alloc();
  extern void __tls_free(int index);
  extern void *__tls_get_value(int index);
  extern void __tls_set_value(int index, void *data);
  extern void __create_thread(void (*Start)(void *Param), int Prio, const char *Name, void *Param, int StackSize);

  #pragma aux __tls_alloc "*" modify [ecx] value [eax];
  #pragma aux __tls_free "*" parm [ecx] modify [eax];
  #pragma aux __tls_get_value "*" parm [ecx] modify [edx] value [eax];
  #pragma aux __tls_set_value "*" parm [ecx] [eax] modify [edx];
  #pragma aux __create_thread "*" parm [edx] [ebx] [edi] [eax] [ecx];

#elif defined( __RDOSDEV__ )
  #include <rdos.h>
  #include <rdosdev.h>
  #define TID int
  #define GetCurrentThreadId() (RdosGetThreadHandle())
#else
  #define INCL_DOSSEMAPHORES
  #define INCL_DOSPROCESS
  #define INCL_DOSEXCEPTIONS
  #include <wos2.h>
  #if defined( __WARP__ )
    extern int          *__threadid(void);      // OS/2 2.0
    extern unsigned     __threadstack(void);
    #define GetCurrentThreadId() (*__threadid())
  #else
    extern int _WCFAR    *_threadid;            // OS/2 1.x
    #define GetCurrentThreadId() (*_threadid)
  #endif
#endif

#if !defined( __QNX__ ) && !defined(__LINUX__) && !defined(__RDOSDEV__)
// QNX and RDOS device-drivers doesn't maintain a list of allocated thread data blocks


// lookup thread data
thread_data *__GetThreadData( void );

// add to list of thread data
int __AddThreadData( TID, thread_data * );

// remove from list of thread data
void __RemoveThreadData( TID );

#if defined (_NETWARE_LIBC)
void __RemoveAllThreadData( void );
#endif

// mark each entry in list of thread data for resize
void __ResizeThreadDataList( void );

// realloc data for the current thread
thread_data *__ReallocThreadData( void );

// clean up list of thread data
void __FreeThreadDataList( void );
#endif

#endif

