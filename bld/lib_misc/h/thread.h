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


#ifndef _THREAD_H_INCLUDED
#define _THREAD_H_INCLUDED

#if defined(_M_IX86)
  #pragma pack(__push,1);
#else
  #pragma pack(__push,8);
#endif

#ifndef __EXCEPTION_RECORD
  #if defined(__NETWARE__)
    #define __EXCEPTION_RECORD  int
  #elif defined(__NT__)
    #define __EXCEPTION_RECORD struct _REGISTRATION_RECORD
  #else
    #define __EXCEPTION_RECORD unsigned
  #endif
#endif

/* Per thread global items */

#include "variety.h"
#include <time.h>

#include "widechar.h"
#if defined(__NETWARE__)
  #include <stdio.h>
#endif
#if defined(__QNX__)
  #include <semaphor.h>
  #include <sys/types.h>
#else
  #include "sigdefn.h"
#endif

/*
 * Note that this structure must not grow larger than 32bytes without
 * also updating the C++ runtime file prwdata.asm
 */
typedef struct  semaphore_object {
  #if defined(__NT__)
        void            *semaphore;
  #elif defined(__QNX__)
        sem_t           semaphore;
  #else
        unsigned long   semaphore;
  #endif
        unsigned        initialized;
        unsigned long   owner;
        unsigned        count;
} semaphore_object;

_WCRTLINK void __AccessSemaphore( semaphore_object * );
_WCRTLINK void __ReleaseSemaphore( semaphore_object * );
_WCRTLINK void __CloseSemaphore( semaphore_object * );

// the following is for the C++ library
#if defined(__386__) || defined(__AXP__) || defined(__PPC__)
    _WCRTLINK extern void (*__AccessSema4)( semaphore_object *);
    _WCRTLINK extern void (*__ReleaseSema4)( semaphore_object *);
    _WCRTLINK extern void (*__CloseSema4)( semaphore_object *);
    #define _AccessSemaphore( sema ) __AccessSema4( sema )
    #define _ReleaseSemaphore( sema ) __ReleaseSema4( sema )
    #define _CloseSemaphore( sema ) __CloseSema4( sema )
#else
    #define _AccessSemaphore( sema ) __AccessSemaphore( sema )
    #define _ReleaseSemaphore( sema ) __ReleaseSemaphore( sema )
    #define _CloseSemaphore( sema ) __CloseSemaphore( sema )
#endif

#if defined(__OS2_286__)
/*
 * C++ thread specific data structure
 *     - must be the same size as struct thread_ctl
 *       in plusplus\cpplib\runtime\h\cpplib.h
 */
// Note: this has changed for 32bit code in 10.0
//       now the C++ library will register how much thread data it wants
//       in an initializer
struct wcpp_thread_ctl {
    void *autos;
    void *d0;
    void *d1;
    void *d2;
    void *d3;
    void *d4;
    void (*c1)();
    void (*c2)();
    void (*c3)();
    void (*c4)();
    void (*c5)();
};
#endif

/* stack checking routine assumes "__stklowP" is first field */
typedef struct thread_data {
    unsigned                    __stklowP;
    #if !defined(__QNX__)
        int                     __errnoP;
        int                     __doserrnoP;
    #endif
    #if defined(__OS2_286__)
        struct wcpp_thread_ctl  _wint_thread_data;
    #endif
    unsigned long int           __randnext;
    char                        *__nexttokP;
    struct tm                   __The_timeP;
    char                        __asctimeP[26];
    char                        __allocated;    // vs auto
    char                        __resize;       // storage has realloc pending
    #if !defined(__QNX__)
        __EXCEPTION_RECORD      *xcpt_handler;
        sigtab                  signal_table[__SIGLAST+1];
    #endif
    char _WCFAR                 *__nextftokP;
    // LDBL_DIG => 15 + 1 for decimal point + 1 for "e" + 3 for exponent => 20
    MAX_CHAR_TYPE               __cvt_buffer[20];
    #if defined(__NT__)
        unsigned long           thread_id;
    #elif defined(__QNX__)
        pid_t                   thread_id;
    #endif
    #if defined(__NT__)
        void                    *thread_handle;
    #endif
    #if defined(__NETWARE__)
        MAX_CHAR_TYPE           __tmpnambuf[ L_tmpnam ];
        unsigned long int       __randnextinit;
    #endif
    #if defined(__NT__) || defined(__OS2__)
        char *                  __nextmbtokP;
        char _WCFAR *           __nextmbftokP;
        wchar_t *               __nextwtokP;
    #endif
    unsigned                    __data_size;
} thread_data;

#if defined(__386__) || defined(__AXP__) || defined(__PPC__)

    // define thread registration function
    #include "thrdreg.h"

    // prototype for thread data init function
    int __initthread( void *p );

    #define __THREADDATAPTR     ((thread_data *)(*__GetThreadPtr)())
    #if defined(__OS2__) || defined(__NETWARE__)
        typedef struct thread_data_vector {
            thread_data *data;
            int         allocated_entry;
        } thread_data_vector;
        extern thread_data_vector *__ThreadData;

        _WCRTLINK extern int *__threadid(void);
        #define _threadid (__threadid())
    #endif
    #if defined(__NT__)
        #define NO_INDEX        0xffffffffL
    #endif
    #if defined(__QNX__)
        // QNX uses magic memory for thread specific data
        extern void *__MultipleThread();
    #endif
#else
    extern int _WCFAR *_threadid;
    extern thread_data **__ThreadData;
    extern void *__MultipleThread();
    #define __THREADDATAPTR     ((thread_data *)__MultipleThread())
#endif

extern  unsigned        __GetMaxThreads(void);
#if defined(_M_IX86)
    #pragma aux __GetMaxThreads "^"
#endif
extern  unsigned        __MaxThreads;

#pragma pack(__pop);
#endif
