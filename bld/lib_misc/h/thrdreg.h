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


#ifndef _THRDREG_H_INCLUDED

#if defined(__386__) || defined(__AXP__) || defined(__PPC__)
    #include "extfunc.h"
    typedef void thread_fn( void * );
    #if defined(_M_IX86)
        #pragma aux (__outside_CLIB) thread_fn;
    #endif
    typedef int     beginner( thread_fn *start_addr, void *stack_bottom,
                                unsigned stack_size, void *arglist );
    typedef void    ender( void );
    typedef int     initializer( void *p );

    #ifdef __NT__
        typedef void __stdcall thread_fnex( void * );
        typedef unsigned long beginnerex( void *security,
            unsigned stack_size, thread_fnex start_address, void *arglist,
            unsigned initflag, unsigned *thrdaddr );
        typedef void enderex( unsigned retval );
    #endif

    _WCRTLINK extern  void    __RegisterThreadData( beginner **begin,
                                                 ender **end,
                                                 initializer **init );

    _WCRTLINK extern unsigned __RegisterThreadDataSize( unsigned size );
    _WCRTLINK extern void *(*__GetThreadPtr)();
    _WCRTLINK extern unsigned   __ThreadDataSize;
#endif

#define _THRDREG_H_INCLUDED
#endif
