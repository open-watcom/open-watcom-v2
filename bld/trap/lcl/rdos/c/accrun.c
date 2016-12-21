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
* Description:  Go and step interface for RDOS
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include "rdos.h"
#include "stdrdos.h"
#include "debug.h"

trap_retval ReqProg_go( void )
{
    struct TDebug           *obj;
    struct TDebugThread     *thread = 0;
    prog_go_ret             *ret;

    ret = GetOutPtr( 0 );
    ret->conditions = COND_THREAD_EXTRA;
    ret->program_counter.offset = 0;
    ret->stack_pointer.offset = 0;
    ret->program_counter.segment = 0;
    ret->stack_pointer.segment = 0;

    obj = GetCurrentDebug();

        if (obj) {
            Go( obj );

        if( IsTerminated( obj ) )
            ret->conditions |= COND_TERMINATE;

        if( HasThreadChange( obj ) ) {
            ClearThreadChange( obj );
            ret->conditions |= COND_THREAD;
        }

        if( HasModuleChange( obj ) ) {
            ClearModuleChange( obj );
            ret->conditions |= COND_LIBRARIES;
        }

        thread = obj->CurrentThread;
        if( thread ) {
            if( HasBreakOccurred( thread ) )
                ret->conditions |= COND_BREAK;

            if( HasTraceOccurred( thread ) )
                ret->conditions |= COND_WATCH;

            if( HasFaultOccurred( thread ) )
                ret->conditions |= COND_EXCEPTION;                
        }
    } else
        ret->conditions |= COND_TERMINATE;

    if( thread ) {
        ret->program_counter.offset = thread->Eip;
        ret->stack_pointer.offset = thread->Esp;
        ret->program_counter.segment = thread->Cs;
        ret->stack_pointer.segment = thread->Ss;
    }

    return( sizeof( *ret ) );
}

trap_retval ReqProg_step( void )
{
    struct TDebug           *obj;
    struct TDebugThread     *thread = 0;
    prog_go_ret             *ret;

    ret = GetOutPtr( 0 );
    ret->conditions = COND_THREAD_EXTRA;
    ret->program_counter.offset = 0;
    ret->stack_pointer.offset = 0;
    ret->program_counter.segment = 0;
    ret->stack_pointer.segment = 0;

    obj = GetCurrentDebug();

        if (obj) {
            Trace( obj );

        if( IsTerminated( obj ) )
            ret->conditions |= COND_TERMINATE;

        if( HasThreadChange( obj ) ) {
            ClearThreadChange( obj );
            ret->conditions |= COND_THREAD;
        }

        if( HasModuleChange( obj ) ) {
            ClearModuleChange( obj );
            ret->conditions |= COND_LIBRARIES;
        }

        thread = obj->CurrentThread;
        if( thread ) {
            if( HasBreakOccurred( thread ) )
                ret->conditions |= COND_BREAK;

            if( HasTraceOccurred( thread ) )
                ret->conditions |= COND_TRACE;

            if( HasFaultOccurred( thread ) )
                ret->conditions |= COND_EXCEPTION;                
        }
    } else
        ret->conditions |= COND_TERMINATE;

    if( thread ) {
        ret->program_counter.offset = thread->Eip;
        ret->stack_pointer.offset = thread->Esp;
        ret->program_counter.segment = thread->Cs;
        ret->stack_pointer.segment = thread->Ss;
    }

    return( sizeof( *ret ) );
}
