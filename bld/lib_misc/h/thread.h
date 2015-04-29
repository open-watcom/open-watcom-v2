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
* Description:  Internal header with defined to support the multi-thread
*               runtime library.
*
****************************************************************************/

#ifndef _THREAD_H_INCLUDED
#define _THREAD_H_INCLUDED

#include <time.h>
#if defined( __NT__ )
  #include <windows.h>
#elif defined( __OS2__ )
  #include <wos2.h>
#elif defined(__NETWARE__)
  #include <stdio.h>
#endif

#if defined(_M_IX86)
  #pragma pack(__push,1);
#else
  #pragma pack(__push,8);
#endif

/* Per thread global items */

#include "xfloat.h"
#include "maxchtyp.h"

#include "osthread.h"

#include "sigdefn.h"

// define thread registration function
#include "thrdreg.h"

#include "sema4.h"

/* Make sure these are in C linkage */
#ifdef __cplusplus
extern "C" {
#endif

#if defined(__OS2_286__)
/*
 * C++ thread specific data structure
 *     - must be the same size as struct thread_ctl
 *       in plusplus\cpplib\runtime\h\cpplib.h
 */
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
#if defined(__NT__) || defined(__OS2__) || defined(__RDOS__)
    int                         __errnoP;
    int                         __doserrnoP;
#endif
#if defined(__OS2_286__)
    struct wcpp_thread_ctl      _wint_thread_data;
#endif
    unsigned long int           __randnext;
    char                        *__nexttokP;
    struct tm                   __The_timeP;
    char                        __asctimeP[26];
    char                        __allocated;    // vs auto
    char                        __resize;       // storage has realloc pending
#if !defined(__QNX__) && !defined(__LINUX__) && !defined(__RDOSDEV__)
    __EXCEPTION_RECORD          *xcpt_handler;
    sigtab                      signal_table[__SIGLAST+1];
#endif
    char _WCFAR                 *__nextftokP;
    MAX_CHAR_TYPE               __cvt_buffer[ __FPCVT_BUFFERLEN + 1 ];
#if defined(__NT__) || defined(_NETWARE_LIBC)
    unsigned long               thread_id;
#elif defined(__UNIX__)
    pid_t                       thread_id;
#elif defined(__RDOS__)
    int                         thread_id;
    char                        thread_name[256];
#endif
#if defined(__NT__)
    void                        *thread_handle;
#endif
#if defined(__NETWARE__)
    MAX_CHAR_TYPE               __tmpnambuf[ L_tmpnam ];
#endif
#if defined(__NT__) || defined(__OS2__)
    char *                      __nextmbtokP;
    char _WCFAR *               __nextmbftokP;
    wchar_t *                   __nextwtokP;
#endif
    unsigned                    __data_size;
} thread_data;

_WCRTDATA extern thread_data    *(*__GetThreadPtr)( void );

extern thread_data *__MultipleThread( void );

#if defined( _M_I86 )
    extern thread_data          **__ThreadData;
#elif defined(__OS2__) || defined(_NETWARE_CLIB)
    typedef struct thread_data_vector {
        thread_data *data;
        int         allocated_entry;
    } thread_data_vector;
    extern thread_data_vector   *__ThreadData;
#endif

#if defined( _M_I86 )
    #define __THREADDATAPTR     (__MultipleThread())
#else
    #define __THREADDATAPTR     ((*__GetThreadPtr)())
#endif

#if !defined( _M_I86 )
    // prototype for thread data init function
    int __initthread( void *p );

  #if defined(__NT__) || defined(_NETWARE_LIBC) || defined(__RDOS__)
    #define NO_INDEX        0xffffffffL
  #endif
#endif


extern  unsigned        __GetMaxThreads(void);
#pragma aux __GetMaxThreads "^"

extern  unsigned        __MaxThreads;

#ifdef __cplusplus
}   /* extern "C" */
#endif

#pragma pack(__pop);
#endif
