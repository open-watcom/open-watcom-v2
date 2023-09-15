/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Remote async services access.
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgerr.h"
#include "dbgio.h"
#include "dbgmem.h"
#include "dbgovl.h"
#include "dui.h"
#include "trpasync.h"
#include "trapaccs.h"
#include "madinter.h"
#include "trapglbl.h"
#include "remcore.h"
#include "dbgmisc.h"
#include "remasync.h"
#include "trpld.h"


#define SUPP_ASYNC_SERVICE( in, request )        \
        in.supp.core_req    = REQ_PERFORM_SUPPLEMENTARY_SERVICE;        \
        in.supp.id                  = SuppAsyncId;       \
        in.req                      = request;

static trap_shandle     SuppAsyncId = 0;

bool InitAsyncSupp( void )
{
    SuppAsyncId = GETSUPPID( ASYNC_SUPP_NAME );
    if( SuppAsyncId != 0 )
        return( true );
    return( false );
}

bool HaveRemoteAsync( void )
{
    return( SuppAsyncId != 0 );
}

unsigned MakeAsyncRun( bool single )
{
    async_go_req        acc;
    async_go_ret        ret;
    addr_ptr            tmp;

    if( SuppAsyncId == 0 )
        return( 0 );

    acc.supp.core_req = REQ_PERFORM_SUPPLEMENTARY_SERVICE;
    acc.supp.id = SuppAsyncId;

    if( single ) {
        acc.req = REQ_ASYNC_STEP;
    } else {
        acc.req = REQ_ASYNC_GO;
    }
    OnAnotherThreadSimpleAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    CONV_LE_16( ret.conditions );

    if( ret.conditions & COND_RUNNING ) {
        ret.stack_pointer.offset = 0;
        ret.stack_pointer.segment = 0;
        ret.program_counter.offset = 0;
        ret.program_counter.segment = 0;
    } else {
        CONV_LE_32( ret.stack_pointer.offset );
        CONV_LE_16( ret.stack_pointer.segment );
        CONV_LE_32( ret.program_counter.offset );
        CONV_LE_16( ret.program_counter.segment );
        if( ret.conditions & COND_CONFIG ) {
            RemoteGetSysConfig();
            CheckMADChange();
        }
        DbgRegs->arch = SysConfig.arch;
        /* Use 'tmp' because of alignment problems */
        tmp = ret.stack_pointer;
        MADRegSpecialSet( MSR_SP, &DbgRegs->mr, &tmp );
        tmp = ret.program_counter;
        MADRegSpecialSet( MSR_IP, &DbgRegs->mr, &tmp );
        if( ret.conditions & COND_THREAD ) {
            DbgRegs->tid = RemoteSetThread( 0 );
        }
    }
    return( ret.conditions );
}

unsigned PollAsync( void )
{
    async_poll_req      acc;
    async_poll_ret      ret;
    addr_ptr            tmp;

    if( SuppAsyncId == 0 )
        return( 0 );

    acc.supp.core_req = REQ_PERFORM_SUPPLEMENTARY_SERVICE;
    acc.supp.id = SuppAsyncId;

    acc.req = REQ_ASYNC_POLL;

    OnAnotherThreadSimpleAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    CONV_LE_16( ret.conditions );

    if( ret.conditions & COND_RUNNING ) {
        ret.stack_pointer.offset = 0;
        ret.stack_pointer.segment = 0;
        ret.program_counter.offset = 0;
        ret.program_counter.segment = 0;
    } else {
        CONV_LE_32( ret.stack_pointer.offset );
        CONV_LE_16( ret.stack_pointer.segment );
        CONV_LE_32( ret.program_counter.offset );
        CONV_LE_16( ret.program_counter.segment );
        if( ret.conditions & COND_CONFIG ) {
            RemoteGetSysConfig();
            CheckMADChange();
        }
        DbgRegs->arch = SysConfig.arch;
        /* Use 'tmp' because of alignment problems */
        tmp = ret.stack_pointer;
        MADRegSpecialSet( MSR_SP, &DbgRegs->mr, &tmp );
        tmp = ret.program_counter;
        MADRegSpecialSet( MSR_IP, &DbgRegs->mr, &tmp );
        if( ret.conditions & COND_THREAD ) {
            DbgRegs->tid = RemoteSetThread( 0 );
        }
    }
    return( ret.conditions );
}

unsigned StopAsync( void )
{
    async_stop_req      acc;
    async_stop_ret      ret;
    addr_ptr            tmp;

    if( SuppAsyncId == 0 )
        return( 0 );

    acc.supp.core_req = REQ_PERFORM_SUPPLEMENTARY_SERVICE;
    acc.supp.id = SuppAsyncId;

    acc.req = REQ_ASYNC_STOP;

    OnAnotherThreadSimpleAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    CONV_LE_16( ret.conditions );

    if( ret.conditions & COND_RUNNING ) {
        ret.stack_pointer.offset = 0;
        ret.stack_pointer.segment = 0;
        ret.program_counter.offset = 0;
        ret.program_counter.segment = 0;
    } else {
        CONV_LE_32( ret.stack_pointer.offset );
        CONV_LE_16( ret.stack_pointer.segment );
        CONV_LE_32( ret.program_counter.offset );
        CONV_LE_16( ret.program_counter.segment );
        if( ret.conditions & COND_CONFIG ) {
            RemoteGetSysConfig();
            CheckMADChange();
        }
        DbgRegs->arch = SysConfig.arch;
        /* Use 'tmp' because of alignment problems */
        tmp = ret.stack_pointer;
        MADRegSpecialSet( MSR_SP, &DbgRegs->mr, &tmp );
        tmp = ret.program_counter;
        MADRegSpecialSet( MSR_IP, &DbgRegs->mr, &tmp );
        if( ret.conditions & COND_THREAD ) {
            DbgRegs->tid = RemoteSetThread( 0 );
        }
    }
    return( ret.conditions );
}

bool AsyncAddBreak( address addr, bool local )
{
    async_add_break_req       acc;

    if( SuppAsyncId == 0 )
        return( false );

    AddrFix( &addr );
    acc.break_addr = addr.mach;
    CONV_LE_32( acc.break_addr.offset );
    CONV_LE_16( acc.break_addr.segment );
    acc.local = local;

    acc.supp.core_req = REQ_PERFORM_SUPPLEMENTARY_SERVICE;
    acc.supp.id = SuppAsyncId;

    acc.req = REQ_ASYNC_ADD_BREAK;

    OnAnotherThreadSimpleAccess( sizeof( acc ), &acc, 0, NULL );
    return( true );
}

void AsyncRemoveBreak( address addr, bool local )
{
    async_remove_break_req     acc;

    if( SuppAsyncId == 0 )
        return;

    AddrFix( &addr );
    acc.break_addr = addr.mach;
    CONV_LE_32( acc.break_addr.offset );
    CONV_LE_16( acc.break_addr.segment );
    acc.local = local;

    acc.supp.core_req = REQ_PERFORM_SUPPLEMENTARY_SERVICE;
    acc.supp.id = SuppAsyncId;

    acc.req = REQ_ASYNC_REMOVE_BREAK;

    OnAnotherThreadSimpleAccess( sizeof( acc ), &acc, 0, NULL );
}
