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


#include "variety.h"
#include "extfunc.h"
#include "thread.h"

extern beginner __CBeginThread;
extern ender    __CEndThread;
static int __CInitThread( void *p ) { p=p; return 0; }

static beginner     *__BeginThread      = __CBeginThread;
static ender        *__EndThread        = __CEndThread;
static initializer  *__InitThread       = __CInitThread;

_WCRTLINK int _beginthread( thread_fn *start_addr, void *stack_bottom,
                        unsigned stack_size, void *arglist )
{
    return( __BeginThread( start_addr, stack_bottom, stack_size, arglist ) );
}

_WCRTLINK void _endthread( void )
{
    __EndThread();
}

int __initthread( void *p )
{
    return __InitThread( p );
}

_WCRTLINK void __RegisterThreadData( beginner **begin, ender **end, initializer **init )
/*************************************************************************************/
{
    beginner    *old_begin;
    ender       *old_end;
    initializer *old_init;

    if( begin != NULL ) {
        old_begin = __BeginThread;
        __BeginThread = *begin;
        *begin = old_begin;
    }

    if( end != NULL ) {
        old_end = __EndThread;
        __EndThread = *end;
        *end = old_end;
    }

    if( init != NULL ) {
        old_init = __InitThread;
        __InitThread = *init;
        *init = old_init;
    }
}
