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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stdnt.h"

/*
 * Add a new thread to a process
 */
void AddThread( DWORD tid, HANDLE th, LPVOID sa )
{
    thread_info *ti;

    ti = LocalAlloc( LMEM_FIXED | LMEM_ZEROINIT, sizeof( thread_info ) );
    if( ti == NULL ) {
        return;
    }
    ti->tid = tid;
    ti->start_addr = sa;
    ti->thread_handle = th;
    ti->brk_addr = 0;

    ti->next = ProcessInfo.thread_list;
    ti->alive = TRUE;
    ti->suspended = FALSE;
    ti->is_wow = FALSE;
    ti->is_dos = FALSE;
    ti->is_foreign = FALSE;
    ProcessInfo.thread_list = ti;
}

/*
 * FindThread - find a thread from a given process_info struct
 */
thread_info *FindThread( DWORD tid )
{
    thread_info *ti;

    ti = ProcessInfo.thread_list;
    while( ti != NULL ) {
        if( ti->tid == tid ) {
            return( ti );
        }
        ti = ti->next;
    }
    return( NULL );
}

/*
 * DeadThread - process a dead thread
 */
void DeadThread( DWORD tid )
{
    thread_info *ti;

    ti = FindThread( tid );
    if( ti == NULL ) {
        return;
    }
    ti->alive = FALSE;
}


/*
 * RemoveAllThreads - remove all threads associated with debugee process
 */
void RemoveAllThreads( void )
{
    thread_info *ti;
    thread_info *next;

    ti = ProcessInfo.thread_list;
    while( ti != NULL ) {
        next = ti->next;
        LocalFree( ti );
        ti = next;
    }
    ProcessInfo.thread_list = NULL;
}

trap_elen ReqThread_freeze( void )
{
    thread_freeze_req   *acc;
    thread_freeze_ret   *ret;
    thread_info         *ti;
    int                 rc;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    ret->err = 0;

    ti = FindThread( acc->thread );
    if( ti != NULL ) {
        /*
         * SuspendThread is a counting thingy - it remembers the number
         * of Suspends done.  So, we remember if we have suspended it
         * already, and only if we haven't do we do the SuspendThread call
         */
        if( !ti->suspended ) {
            rc = SuspendThread( ti->thread_handle );
            if( rc == -1 ) {
                ret->err = 1;
            } else {
                ti->suspended = TRUE;
            }
        }
    } else {
        ret->err = 1;
    }
    return( sizeof( *ret ) );
}

trap_elen ReqThread_thaw( void )
{
    thread_thaw_req *acc;
    thread_thaw_ret *ret;
    thread_info     *ti;
    int             rc;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    ret->err = 0;
    ti = FindThread( acc->thread );
    if( ti != NULL ) {
        if( ti->suspended ) {
            rc = ResumeThread( ti->thread_handle );
            if( rc == -1 ) {
                ret->err = 1;
            } else {
                ti->suspended = FALSE;
            }
        }
    } else {
        ret->err = 1;
    }
    return( sizeof( *ret ) );
}

trap_elen ReqThread_set( void )
{
    thread_set_req  *acc;
    thread_set_ret  *ret;
    thread_info     *ti;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    ret->old_thread = DebugeeTid;
    ret->err = 0;
    if( acc->thread != 0 ) {
        ti = FindThread( acc->thread );
        if( ti != NULL ) {
            DebugeeTid = acc->thread;
        } else {
            ret->err = 1;
        }
    }
    return( sizeof( *ret ) );
}

trap_elen ReqThread_get_next( void )
{
    static thread_info  *ti;
    thread_get_next_req *acc;
    thread_get_next_ret *ret;
    int                 rc;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    /*
     * get next pointer
     */
    if( acc->thread == 0 ) {
        ti = ProcessInfo.thread_list;
    } else {
        ti = ti->next;
    }

    /*
     * find a live one
     */
    for( ;; ) {
        if( ti == NULL ) {
            break;
        }
        if( ti->alive ) {
            break;
        }
        ti = ti->next;
    }

    if( ti == NULL ) {
        ret->thread = 0;
    } else {
        ret->thread = ti->tid;
        rc = SuspendThread( ti->thread_handle );
        if( rc > 0 ) {
            ret->state = THREAD_FROZEN;
            ResumeThread( ti->thread_handle );
        } else if( rc == 0 ) {
            ret->state = THREAD_THAWED;
            ResumeThread( ti->thread_handle );
        } else {
            ret->state = THREAD_THAWED;
        }
    }
    return( sizeof( *ret ) );
}

trap_elen ReqThread_get_extra( void )
{
    thread_get_extra_req    *acc;
    char                    *name;
    thread_info             *ti;

    acc = GetInPtr( 0 );
    name = GetOutPtr( 0 );
    strcpy( name, "" );
    if( acc->thread == 0 ) {
        strcpy( name, "priority" );
    } else {
        ti = FindThread( acc->thread );
        if( ti != NULL ) {
            itoa( GetThreadPriority( ti->thread_handle ), name, 10 );
        }
    }
    return( strlen( name ) + 1 );
}
