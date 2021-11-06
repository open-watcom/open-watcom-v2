/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Async interface for RDOS
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include "rdos.h"
#include "stdrdos.h"
#include "debug.h"

trap_retval TRAP_ASYNC( go )( void )
{
    struct TDebug           *obj;
    struct TDebugThread     *thread;
    async_go_ret            *ret;
    int                     ok;

    ret = GetOutPtr( 0 );
    ret->conditions = COND_THREAD_EXTRA;

    obj = GetCurrentDebug();

    if (obj) {
        thread = obj->CurrentThread;
        ok = AsyncGo( obj, 250 );

        if( ok ) {
            thread = GetCurrentThread( obj );
            if( thread ) {
                SetCurrentThread( obj, thread->ThreadID );
                SetCurrentDebug( obj );
            }

            if( IsTerminated( obj ) )
                ret->conditions |= COND_TERMINATE;

            if( HasThreadChange( obj ) ) {
                ret->conditions |= COND_THREAD;
                ClearThreadChange( obj );
            }

            if( HasModuleChange( obj ) ) {
                ret->conditions |= COND_LIBRARIES;
            }

            if ( HasConfigChange( obj ) ) {
                ClearConfigChange( obj );
                ret->conditions |= COND_CONFIG;
            }

            if( thread ) {
                if( HasBreakOccurred( thread ) )
                    ret->conditions |= COND_BREAK;

                if( HasTraceOccurred( thread ) )
                    ret->conditions |= COND_WATCH;

                if( HasFaultOccurred( thread ) )
                    ret->conditions |= COND_EXCEPTION;
            }
        } else {
            ret->conditions     |= COND_RUNNING;
        }
    } else
        ret->conditions |= COND_TERMINATE;

    if( thread && ok ) {
        ret->program_counter.offset = thread->Eip;
        ret->stack_pointer.offset = thread->Esp;
        ret->program_counter.segment = thread->Cs;
        ret->stack_pointer.segment = thread->Ss;
    } else {
        ret->program_counter.offset = 0;
        ret->stack_pointer.offset = 0;
        ret->program_counter.segment = 0;
        ret->stack_pointer.segment = 0;
    }

    return( sizeof( *ret ) );
}

trap_retval TRAP_ASYNC( step )( void )
{
    struct TDebug           *obj;
    struct TDebugThread     *thread;
    async_go_ret            *ret;
    int                     ok;

    ret = GetOutPtr( 0 );
    ret->conditions = COND_THREAD_EXTRA;

    obj = GetCurrentDebug();

    if (obj) {
        thread = obj->CurrentThread;
        ok = AsyncTrace( obj, 250 );

        if( ok ) {
            thread = GetCurrentThread( obj );
            if( thread ) {
                SetCurrentThread( obj, thread->ThreadID );
                SetCurrentDebug( obj );
            }

            if( IsTerminated( obj ) )
                ret->conditions |= COND_TERMINATE;

            if( HasThreadChange( obj ) ) {
                ret->conditions |= COND_THREAD;
                ClearThreadChange( obj );
            }

            if( HasModuleChange( obj ) ) {
                ret->conditions |= COND_LIBRARIES;
            }

            if ( HasConfigChange( obj ) ) {
                ClearConfigChange( obj );
                ret->conditions |= COND_CONFIG;
            }

            if( thread ) {
                if( HasBreakOccurred( thread ) )
                    ret->conditions |= COND_BREAK;

                if( HasTraceOccurred( thread ) )
                    ret->conditions |= COND_TRACE;

                if( HasFaultOccurred( thread ) )
                    ret->conditions |= COND_EXCEPTION;
            }
        } else {
            ret->conditions     |= COND_RUNNING;
        }
    } else
        ret->conditions |= COND_TERMINATE;

    if( thread && ok ) {
        ret->program_counter.offset = thread->Eip;
        ret->stack_pointer.offset = thread->Esp;
        ret->program_counter.segment = thread->Cs;
        ret->stack_pointer.segment = thread->Ss;
    } else {
        ret->program_counter.offset = 0;
        ret->stack_pointer.offset = 0;
        ret->program_counter.segment = 0;
        ret->stack_pointer.segment = 0;
    }

    return( sizeof( *ret ) );
}

trap_retval TRAP_ASYNC( poll )( void )
{
    struct TDebug           *obj;
    struct TDebugThread     *thread;
    async_go_ret            *ret;
    int                     ok;

    ret = GetOutPtr( 0 );
    ret->conditions = COND_THREAD_EXTRA;

    obj = GetCurrentDebug();

    if (obj) {
        ok = AsyncPoll( obj, 250 );

        if( ok ) {
            thread = GetCurrentThread( obj );
            if( thread ) {
                SetCurrentThread( obj, thread->ThreadID );
                SetCurrentDebug( obj );
            }

            if( IsTerminated( obj ) )
                ret->conditions |= COND_TERMINATE;

            if( HasThreadChange( obj ) ) {
                ret->conditions |= COND_THREAD;
                ClearThreadChange( obj );
            }

            if( HasModuleChange( obj ) ) {
                ret->conditions |= COND_LIBRARIES;
            }

            if ( HasConfigChange( obj ) ) {
                ClearConfigChange( obj );
                ret->conditions |= COND_CONFIG;
            }

            if( thread ) {
                if( HasBreakOccurred( thread ) )
                    ret->conditions |= COND_BREAK;

                if( HasTraceOccurred( thread ) )
                    ret->conditions |= COND_TRACE;

                if( HasFaultOccurred( thread ) )
                    ret->conditions |= COND_EXCEPTION;
            }
        } else {
            ret->conditions     |= COND_RUNNING;
        }
    } else
        ret->conditions |= COND_TERMINATE;

    if( thread && ok ) {
        ret->program_counter.offset = thread->Eip;
        ret->stack_pointer.offset = thread->Esp;
        ret->program_counter.segment = thread->Cs;
        ret->stack_pointer.segment = thread->Ss;
    } else {
        ret->program_counter.offset = 0;
        ret->stack_pointer.offset = 0;
        ret->program_counter.segment = 0;
        ret->stack_pointer.segment = 0;
    }

    return( sizeof( *ret ) );
}

trap_retval TRAP_ASYNC( stop )( void )
{
    async_go_ret            *ret;

    ret = GetOutPtr( 0 );
    ret->conditions = COND_STOP;
    ret->program_counter.offset = 0;
    ret->stack_pointer.offset = 0;
    ret->program_counter.segment = 0;
    ret->stack_pointer.segment = 0;

    return( sizeof( *ret ) );
}

trap_retval TRAP_ASYNC( add_break )( void )
{
    async_add_break_req *acc;
    struct TDebug       *obj;
    int                 sel;
    int                 offset;
    bool                hw;

    acc = GetInPtr( 0 );
    sel = acc->break_addr.segment;
    offset = acc->break_addr.offset;

    if( acc->local ) {
        hw = true;
    } else {
        hw = ( (sel & 3) == 0 );
    }

    obj = GetCurrentDebug();

    if( obj )
        AddBreak( obj, sel, offset, hw );

    return( 0 );
}

trap_retval TRAP_ASYNC( remove_break )( void )
{
    async_remove_break_req *acc;
    struct TDebug          *obj;

    acc = GetInPtr( 0 );

    obj = GetCurrentDebug();

    if( obj )
        ClearBreak( obj, acc->break_addr.segment, acc->break_addr.offset );

    return( 0 );
}
