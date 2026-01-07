/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Fortran thread-specific data and related functions
*
****************************************************************************/


#ifndef _FTHREAD_H_INCLUDED
#define _FTHREAD_H_INCLUDED

#include <setjmp.h>
#include "trcback.h"
#include "xfflags.h"

#if defined( __WINDOWS__ ) && defined( _M_I86 )
  #define __setjmp      Catch
  #define __longjmp     Throw
  #define __jmp_buf     CATCHBUF
#else
  #define __setjmp      setjmp
  #define __longjmp     longjmp
  #define __jmp_buf     jmp_buf
#endif

#ifdef __MT__
    extern void         (*_AccessFIO)( void );
    extern void         (*_ReleaseFIO)( void );
    extern void         (*_PartialReleaseFIO)( void );
#else
    #define _AccessFIO()
    #define _ReleaseFIO()
    #define _PartialReleaseFIO()
#endif

#ifdef __MT__
    /*
     * macro to convert C run-time to Fortran thread extension data pointer
     */
    #define C2F_THREADDATAPTR(ct)   ((fthread_data *)((char *)(ct) + __FThreadDataOffset))
    #define __FTHREADDATAPTR        C2F_THREADDATAPTR( __THREADDATAPTR )

    #define _RWD_XcptFlags          (__FTHREADDATAPTR->__XceptionFlags)
    #define _RWD_ExCurr             (__FTHREADDATAPTR->__ExCurr)
#else
    #define _RWD_XcptFlags          XcptFlags
    #define _RWD_ExCurr             ExCurr

    extern volatile unsigned short  XcptFlags;
    extern traceback                PGM *ExCurr;    // head of traceback list
#endif

#ifdef __MT__

/*
 * Fortran extension of C run-time thread data
 */
typedef struct fthread_data {
    __jmp_buf           *__SpawnStack;
    traceback           *__ExCurr;
    volatile unsigned short __XceptionFlags;
    void                (*__rtn)(void *);
    void                *__arglist;
} fthread_data;

extern unsigned         __FThreadDataOffset;

extern void             __FiniFThreadProcessing( void );
extern int              __InitFThreadProcessing( void );
extern void             __InitFThreadData( void * );
extern void             __InitMultiThreadFIO( void );

extern void             __FiniBeginFThread( void );
extern void             __InitBeginFThread( void );

#endif

#endif
