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

#include "variety.h"

#if defined( __NETWARE__ )
  #define TID                   int
  #define GetCurrentThreadId()  (*__threadid())
  extern void           ThreadSwitch( void );
  extern void           *GetThreadID( void );
#elif defined( __NT__ )
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
  #endif
  #include <windows.h>
  #include "ntex.h"
  extern DWORD          __TlsIndex;
  #define TID                   DWORD
  extern void           __NTThreadFini(void);
#elif defined( __QNX__ )
  #include <sys/types.h>
  #include <unistd.h>
  #define TID                   pid_t
  #define GetCurrentThreadId()  (getpid())
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

#if !defined( __QNX__ )
// QNX doesn't maintain a list of allocated thread data blocks


// lookup thread data
thread_data *__GetThreadData( void );

// add to list of thread data
int __AddThreadData( TID, thread_data * );

// remove from list of thread data
void __RemoveThreadData( TID );

// mark each entry in list of thread data for resize
void __ResizeThreadDataList( void );

// realloc data for the current thread
thread_data *__ReallocThreadData( void );

// clean up list of thread data
void __FreeThreadDataList( void );
#endif

#endif
