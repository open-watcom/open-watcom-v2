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
* Description:  control access to FORTRAN i/o system
*
****************************************************************************/


#include "ftnstd.h"

#include <stddef.h>

#if defined( __OS2__ )

  #define INCL_DOSSEMAPHORES
  #include <wos2.h>

  #define       _FSEM           ULONG
  #define       _FTID           TID

  #define       _FRequestMutexSem       DosRequestMutexSem
  #define       _FReleaseMutexSem       DosReleaseMutexSem

#elif defined( __NETWARE__ )

  #define       _FSEM           long
  #define       _FTID           int

  #define       _FRequestMutexSem( sem, x )     WaitOnLocalSemaphore( sem )
  #define       _FReleaseMutexSem               SignalLocalSemaphore

  extern int    WaitOnLocalSemaphore( long );
  extern int    SignalLocalSemaphore( long );

#elif defined( __NT__ )

  #include <windows.h>

  #define       _FSEM           HANDLE
  #define       _FTID           DWORD

  #define       _FRequestMutexSem       WaitForSingleObject
  #define       _FReleaseMutexSem       ReleaseMutex

  #define       SEM_INDEFINITE_WAIT     -1

#elif defined( __LINUX__ )

// TODO: semaphore support for Linux !

  #define       _FSEM           long
  #define       _FTID           int

  #define       _FRequestMutexSem( sem, x )
  #define       _FReleaseMutexSem( sem )

  #define       SEM_INDEFINITE_WAIT     -1

#endif

extern  _FSEM           __fio_sem;
static  _FTID           __fio_owner = { 0 };
static  int             __fio_count = { 0 };


void    __AccessFIO( void ) {
//===========================

    _FTID       tid;

    tid = *_threadid;
    if( __fio_owner != tid ) {
        _FRequestMutexSem( __fio_sem, SEM_INDEFINITE_WAIT );
        __fio_owner = tid;
    }
    ++__fio_count;
}


void    __ReleaseFIO( void ) {
//============================

    --__fio_count;
    if( __fio_count == 0 ) {
        _FReleaseMutexSem( __fio_sem );
        __fio_owner = 0;
    }
}


void    __PartialReleaseFIO( void ) {
//===================================

    --__fio_count;
}
