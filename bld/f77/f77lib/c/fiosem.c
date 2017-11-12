/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2017 The Open Watcom Contributors. All Rights Reserved.
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
#if defined( __OS2__ )
  #define INCL_DOSSEMAPHORES
  #include <wos2.h>
#elif defined( __NETWARE__ )
  #include "nw_lib.h"
#elif defined( __NT__ )
  #include <windows.h>
#elif defined( __UNIX__ )
  #include <semaphore.h>
  #include <sys/types.h>
#endif
#include "threadid.h"
#include "fiosem.h"


#if defined( __OS2__ )

  #define       _FRequestMutexSem       DosRequestMutexSem
  #define       _FReleaseMutexSem       DosReleaseMutexSem

#elif defined( __NETWARE__ )

  #define       _FRequestMutexSem( sem, x )     WaitOnLocalSemaphore( sem )
  #define       _FReleaseMutexSem               SignalLocalSemaphore

#elif defined( __NT__ )

  #define       _FRequestMutexSem       WaitForSingleObject
  #define       _FReleaseMutexSem       ReleaseMutex

  #define       SEM_INDEFINITE_WAIT     -1

#elif defined( __LINUX__ )

  #define       _FRequestMutexSem( sem, x )      sem_wait( &sem )
  #define       _FReleaseMutexSem( sem )         sem_post( &sem )

  #define       SEM_INDEFINITE_WAIT     -1

#endif


static  _TID            __fio_owner = { 0 };
static  int             __fio_count = { 0 };


void    __AccessFIO( void ) {
//===========================

    _TID        tid;

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
