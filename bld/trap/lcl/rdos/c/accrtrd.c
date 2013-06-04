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
* Description:  Non-blocking thread functions
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdrdos.h"
#include "rdos.h"

#define THD_WAIT        2
#define THD_SIGNAL      3
#define THD_KEYBOARD    4
#define THD_BLOCKED     5
#define THD_RUN         6
#define THD_DEBUG       7

trap_retval ReqRunThread_info( void )
{
    run_thread_info_req *acc;
    run_thread_info_ret *ret;
    char                *header_txt;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    header_txt = GetOutPtr( sizeof( *ret ) );

    switch( acc->col )
    {
    case 0:
        ret->info = RUN_THREAD_INFO_TYPE_NAME;
        ret->width = 25;
        strcpy( header_txt, "ID   Name" );
        break;

    case 1:
        ret->info = RUN_THREAD_INFO_TYPE_EXTRA;
        ret->width = 21;
        strcpy( header_txt, "                Time" );
        break;

    case 2:
        ret->info = RUN_THREAD_INFO_TYPE_STATE;
        ret->width = 10;
        strcpy( header_txt, "State" );
        break;

    case 3:
        ret->info = RUN_THREAD_INFO_TYPE_CS_EIP;
        ret->width = 15;
        strcpy( header_txt, "cs:eip" );
        break;

    default:
        ret->info = RUN_THREAD_INFO_TYPE_NONE;
        ret->width = 0;
        *header_txt = 0;
        break;
    }

    return( sizeof( *ret ) + strlen( header_txt ) + 1 );
}

trap_retval ReqRunThread_get_next( void )
{
    run_thread_get_next_req *acc;
    run_thread_get_next_ret *ret;
    struct TDebug           *obj;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->thread = 0;

    obj = GetCurrentDebug();
        if (obj)
        ret->thread = GetNextThread( obj, acc->thread );

    return( sizeof( *ret ) );
}

trap_retval ReqRunThread_get_runtime( void )
{
    run_thread_get_runtime_req *acc;
    run_thread_get_runtime_ret *ret;
    int                        ok;
    int                        i;
    struct ThreadState         state;
    char                       *time_txt;
    char                       tempstr[10];
    int                        day;
    int                        hour;
    int                        min;
    int                        sec;
    int                        milli;
    int                        micro;
    int                        started;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    time_txt = GetOutPtr( sizeof( *ret ) );

    ok = FALSE;

    for( i = 0; i < 256 && !ok; i++ ) {
        RdosGetThreadState( i, &state );
        if (state.ID == acc->thread) {
            ok = TRUE;
        }
    }

    ret->state = THD_BLOCKED;
    ret->cs = 0;
    ret->eip = 0;
    *time_txt = 0;

    if (ok) {
        if( strstr( state.List, "Ready" ) )
            ret->state = THD_RUN;

        if( strstr( state.List, "Run" ) )
            ret->state = THD_RUN;
                
        if( strstr( state.List, "Debug" ) )
            ret->state = THD_DEBUG;
                
        if( strstr( state.List, "Wait" ) )
            ret->state = THD_WAIT;

        if( strstr( state.List, "Signal" ) )
            ret->state = THD_SIGNAL;

        if( strstr( state.List, "Keyboard" ) )
            ret->state = THD_KEYBOARD;

        ret->cs = state.Sel;
        ret->eip = state.Offset;

        day = state.MsbTime / 24;
        hour = state.MsbTime % 24;
        RdosDecodeLsbTics(state.LsbTime, &min, &sec, &milli, &micro);

        started = FALSE;
        if( day ) {
            sprintf( time_txt, "%3d ", day );
            started = TRUE;
        } else {
            strcpy( time_txt, "    ");
        }

        if( hour || started ) {
            if( started )
                sprintf( tempstr, "%02d.", hour );
            else
                sprintf( tempstr, "%2d.", hour );
            strcat( time_txt, tempstr );
            started = TRUE;
        } else {
            strcat( time_txt, "   " );
        }

        if( min || started ) {
            if( started )
                sprintf( tempstr, "%02d.", min );
            else
                sprintf( tempstr, "%2d.", min );
            strcat( time_txt, tempstr );
            started = TRUE;
        } else {
            strcat( time_txt, "   " );
        }

        if( sec || started ) {
            if( started )
               sprintf( tempstr, "%02d,", sec );
            else
               sprintf( tempstr, "%2d,", sec );
            strcat( time_txt, tempstr );
            started = TRUE;
        } else {
            strcat( time_txt, "   " );
        }

        if( milli || started ) {
            if( started )
                sprintf( tempstr, "%03d ", milli );
            else
                sprintf( tempstr, "%3d ", milli );
            strcat( time_txt, tempstr );
            started = TRUE;
        } else {
            strcat( time_txt, "    " );
        }

        if( started )
            sprintf( tempstr, "%03d", micro );
        else 
            sprintf( tempstr, "%3d", micro );
        strcat( time_txt, tempstr );
    }
    return( sizeof( *ret ) + strlen( time_txt ) + 1 );
}

trap_retval ReqRunThread_poll( void )
{
    struct TDebug           *obj;
    struct TDebugThread     *thread = 0;
    run_thread_poll_ret     *ret;

    ret = GetOutPtr( 0 );
    ret->conditions = 0;

    obj = GetCurrentDebug();

    if (obj) {

        if( IsTerminated( obj ) )
            ret->conditions |= COND_TERMINATE;

        if( HasThreadChange( obj ) ) {
            ret->conditions |= COND_THREAD;
            thread = GetNewThread( obj );
            if( thread ) {
                SetCurrentThread( obj, thread->ThreadID );
                SetCurrentDebug( obj );
            }
            ClearThreadChange( obj );
        }

        if( HasModuleChange( obj ) ) {
            ClearModuleChange( obj );
            ret->conditions |= COND_LIBRARIES;
        }
    }
    return( sizeof( *ret ) );
}

trap_retval ReqRunThread_set( void )
{
    run_thread_set_req      *acc;
    run_thread_set_ret      *ret;
    struct TDebug           *obj;
    struct TDebugThread     *t = 0;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    ret->old_thread = 0;
    ret->err = 0;

    obj = GetCurrentDebug();
    if (obj )
        t = obj->CurrentThread;

    if( t )
        ret->old_thread = t->ThreadID;

    if( obj && acc->thread != 0 ) {
        SetCurrentThread( obj, acc->thread );
        SetCurrentDebug( obj );
    }

    return( sizeof( *ret ) );
}

trap_retval ReqRunThread_get_name( void )
{
    run_thread_get_name_req *acc;
    char                    *name;
    struct TDebug           *obj;
    struct TDebugThread     *t = 0;

    acc = GetInPtr( 0 );
    name = GetOutPtr( 0 );
    strcpy( name, "" );

    if( acc->thread == 0 )
        strcpy( name, "Name" );
    else {
        obj = GetCurrentDebug();
            if (obj) {
            t = LockThread( obj, acc->thread );

            if( t )
                sprintf( name, "%04hX %s", t->ThreadID, t->ThreadName );

            UnlockThread( obj );
        }
    }

    return( strlen( name ) + 1 );
}

trap_retval ReqRunThread_stop( void )
{
    run_thread_stop_req     *acc;

    acc = GetInPtr( 0 );

    RdosSuspendThread( acc->thread );
    return( 0 );
}

trap_retval ReqRunThread_signal_stop( void )
{
    run_thread_signal_stop_req     *acc;

    acc = GetInPtr( 0 );

    RdosSuspendAndSignalThread( acc->thread );
    return( 0 );
}
