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
* Description:  Interface for non-blocking thread services
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgio.h"
#include "trprtrd.h"
#include "trapaccs.h"
#include "madinter.h"
#include "dui.h"
#include "trapglbl.h"
#include "remcore.h"
#include "dbgmisc.h"
#include "remrtrd.h"
#include "trpld.h"


#define DEFAULT_TID     1

static trap_shandle     SuppRunThreadId = 0;

bool InitRunThreadSupp( void )
{
    SuppRunThreadId = GetSuppId( RUN_THREAD_SUPP_NAME );
    if( SuppRunThreadId == 0 )
        return( FALSE );
    DUIInitRunThreadInfo();
    return( TRUE );
}

bool HaveRemoteRunThread( void )
{
    return( SuppRunThreadId != 0 );
}

bool RemoteGetRunThreadInfo( int row, unsigned char *infotype, int *width, char *header, int maxsize )
{
    in_mx_entry         in[1];
    mx_entry            out[2];
    run_thread_info_req acc;
    run_thread_info_ret ret;

    if( SuppRunThreadId == 0 ) return( FALSE );

    acc.supp.core_req = REQ_PERFORM_SUPPLEMENTARY_SERVICE;
    acc.supp.id = SuppRunThreadId;
    acc.req = REQ_RUN_THREAD_INFO;
    acc.col = row;
    ret.info = 0;

    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );
    out[1].ptr = header;
    out[1].len = maxsize;
    TrapAccess( 1, in, 2, out );

    if( ret.info ) {
        *infotype = ret.info;
        *width = ret.width;
        return( TRUE );
    } else {
        return( FALSE );
    }
}

dtid_t RemoteGetNextRunThread( dtid_t tid )
{
    run_thread_get_next_req acc;
    run_thread_get_next_ret ret;

    if( SuppRunThreadId == 0 ) return( tid == 0 ? DEFAULT_TID : 0 );

    acc.supp.core_req = REQ_PERFORM_SUPPLEMENTARY_SERVICE;
    acc.supp.id = SuppRunThreadId;
    acc.req = REQ_RUN_THREAD_GET_NEXT;
    acc.thread = tid;
    TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    return( ret.thread );
}

void RemotePollRunThread( void )
{
    run_thread_poll_req      acc;
    run_thread_poll_ret      ret;

    if( SuppRunThreadId == 0 ) return;

    acc.supp.core_req = REQ_PERFORM_SUPPLEMENTARY_SERVICE;
    acc.supp.id = SuppRunThreadId;
    acc.req = REQ_RUN_THREAD_POLL;

    OnAnotherThreadSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    CONV_LE_16( ret.conditions );

    if( ret.conditions & COND_CONFIG ) {
        GetSysConfig();
        CheckMADChange();
    }
    if( ret.conditions & COND_THREAD ) {
        CheckForNewThreads( TRUE );
    }
}

void RemoteUpdateRunThread( thread_state *thd )
{
    in_mx_entry                     in[1];
    mx_entry                        out[2];
    run_thread_get_runtime_req      acc;
    run_thread_get_runtime_ret      ret;

    if( SuppRunThreadId == 0 ) return;

    acc.supp.core_req = REQ_PERFORM_SUPPLEMENTARY_SERVICE;
    acc.supp.id = SuppRunThreadId;
    acc.req = REQ_RUN_THREAD_GET_RUNTIME;
    acc.thread = thd->tid;

    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );
    out[1].ptr = thd->extra;
    out[1].len = MAX_THD_EXTRA_SIZE;
    TrapAccess( 1, in, 2, out );

    thd->state = ret.state;
    thd->cs = ret.cs;
    thd->eip = ret.eip;    
}

//NYI: We don't know the size of the incoming name. Now assume max is 80.
#define MAX_THD_NAME_LEN       80

void RemoteRunThdName( dtid_t tid, char *name )
{
    run_thread_get_name_req        acc;

    if( SuppRunThreadId == 0 ) {
        *name = NULLCHAR;
        return;
    }

    acc.supp.core_req = REQ_PERFORM_SUPPLEMENTARY_SERVICE;
    acc.supp.id = SuppRunThreadId;
    acc.req = REQ_RUN_THREAD_GET_NAME;
    acc.thread = tid;
    TrapSimpAccess( sizeof( acc ), &acc, MAX_THD_NAME_LEN, name );
}

dtid_t RemoteSetRunThreadWithErr( dtid_t tid, error_idx *erridx )
{
    run_thread_set_req      acc;
    run_thread_set_ret      ret;

    if( SuppRunThreadId == 0 )
        return( DEFAULT_TID );
    acc.supp.core_req = REQ_PERFORM_SUPPLEMENTARY_SERVICE;
    acc.supp.id = SuppRunThreadId;
    acc.req = REQ_RUN_THREAD_SET;
    acc.thread = tid;
    TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    if( ret.err != 0 ) {
        *erridx = StashErrCode( ret.err, OP_REMOTE );
        return( 0 );
    }
    return( ret.old_thread );
}

dtid_t RemoteSetRunThread( dtid_t tid )
{
    error_idx   erridx;

    return( RemoteSetRunThreadWithErr( tid, &erridx ) );
}

void RemoteStopThread( thread_state *thd )
{
    run_thread_stop_req      acc;

    if( SuppRunThreadId == 0 ) return;

    acc.supp.core_req = REQ_PERFORM_SUPPLEMENTARY_SERVICE;
    acc.supp.id = SuppRunThreadId;
    acc.req = REQ_RUN_THREAD_STOP;
    acc.thread = thd->tid;

    OnAnotherThreadSimpAccess( sizeof( acc ), &acc, 0, NULL );
}

void RemoteSignalStopThread( thread_state *thd )
{
    run_thread_signal_stop_req      acc;

    if( SuppRunThreadId == 0 ) return;

    acc.supp.core_req = REQ_PERFORM_SUPPLEMENTARY_SERVICE;
    acc.supp.id = SuppRunThreadId;
    acc.req = REQ_RUN_THREAD_SIGNAL_STOP;
    acc.thread = thd->tid;

    OnAnotherThreadSimpAccess( sizeof( acc ), &acc, 0, NULL );
}
