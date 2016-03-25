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


#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgio.h"
#include "trpthrd.h"
#include "trapglbl.h"
#include "trpld.h"
#include "remthrd.h"

#define DEFAULT_TID     1

trap_shandle    SuppThreadId = 0;

#define SUPP_THREAD_SERVICE( in, request )      \
        in.supp.core_req        = REQ_PERFORM_SUPPLEMENTARY_SERVICE;    \
        in.supp.id              = SuppThreadId; \
        in.req                  = request;


bool InitThreadSupp( void )
{
    SuppThreadId = GetSuppId( THREAD_SUPP_NAME );
    return( SuppThreadId != 0 );
}

dtid_t RemoteGetNextThread( dtid_t tid, unsigned *state )
{
    thread_get_next_req acc;
    thread_get_next_ret ret;

    if( SuppThreadId == 0 ) return( tid == 0 ? DEFAULT_TID : 0 );
    SUPP_THREAD_SERVICE( acc, REQ_THREAD_GET_NEXT );
    acc.thread = tid;
    TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    if( state != NULL ) *state = ret.state;
    return( ret.thread );
}

dtid_t RemoteSetThreadWithErr( dtid_t tid, error_handle *errh )
{
    thread_set_req      acc;
    thread_set_ret      ret;

    if( SuppThreadId == 0 )
        return( DEFAULT_TID );
    SUPP_THREAD_SERVICE( acc, REQ_THREAD_SET );
    acc.thread = tid;
    TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    if( ret.err != 0 ) {
        *errh = StashErrCode( ret.err, OP_REMOTE );
        return( 0 );
    }
    return( ret.old_thread );
}

long RemoteFreezeThread( dtid_t tid )
{
    thread_freeze_req   acc;
    thread_freeze_ret   ret;

    if( SuppThreadId == 0 ) return( 0 );
    SUPP_THREAD_SERVICE( acc, REQ_THREAD_FREEZE );
    acc.thread = tid;
    TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    return( ret.err );
}

long RemoteThawThread( dtid_t tid )
{
    thread_thaw_req     acc;
    thread_thaw_ret     ret;

    if( SuppThreadId == 0 ) return( 0 );
    SUPP_THREAD_SERVICE( acc, REQ_THREAD_THAW );
    acc.thread = tid;
    TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    return( ret.err );
}

//NYI: We don't know the size of the incoming name. Now assume max is 80.
#define MAX_THD_EXTRA_LEN       80

void RemoteThdName( dtid_t tid, char *name )
{
    thread_get_extra_req        acc;

    if( SuppThreadId == 0 ) {
        *name = NULLCHAR;
        return;
    }
    SUPP_THREAD_SERVICE( acc, REQ_THREAD_GET_EXTRA );
    acc.thread = tid;
    TrapSimpAccess( sizeof( acc ), &acc, MAX_THD_EXTRA_LEN, name );
}
