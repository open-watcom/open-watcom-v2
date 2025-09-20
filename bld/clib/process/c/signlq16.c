/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
#include "seterrno.h"
#include <sys/types.h>
#include <stdio.h>
#include <signal.h>
#include <sys/kernel.h>
#include <sys/proc_msg.h>
#include <i86.h>
#include "rtdata.h"
#include "owqnx.h"


/*
 * This table is fished into by the kernel so do not change its
 * size or layout.
 */
static struct __sigtable {
    void                (__SIG_FAR *stub)();
    struct sigaction    acttable[_SIGMAX];
} _SignalTable;


_WCRTLINK void (_WCCALLBACK *signal( int sig, void (_WCCALLBACK *func)(int) ))(int)
{
    struct sigaction act;

#if defined( __SMALL_CODE__ )
    /* this if is necessary in small code models to get the
       correct near ==> far pointer conversions */
    if( func == SIG_ERR ) {
        act.sa_handler = __FAR_SIG_ERR;
    } else if( func == SIG_DFL ) {
        act.sa_handler = __FAR_SIG_DFL;
    } else if( func == SIG_IGN ) {
        act.sa_handler = __FAR_SIG_IGN;
    } else if( func == SIG_HOLD ) {
        act.sa_handler = __FAR_SIG_HOLD;
    } else {
        act.sa_handler = (__far_sig_func)func;
    }
#else
    act.sa_handler = func;
#endif
    act.sa_mask = 0;
    act.sa_flags = 0;

    if( sigaction(sig, &act, &act) )
        return( SIG_ERR );
#if defined( __SMALL_CODE__ )
    return( (__sig_func)(unsigned)act.sa_handler );
#else
    return( act.sa_handler );
#endif
}

static void __sigabort( void )
{
    raise( SIGABRT );
}

_WCRTLINK int sigaction(
    int                             sig,
    register const struct sigaction *act,
    register struct sigaction       *oact )
{
    static int  first = 1;
    union {
        struct _proc_signal         s;
        struct _proc_signal_reply   r;
    } msg;

    /*
     * Tell the process manager the address of our signal table.
     */
    if( first ) {
        _SignalTable.stub = (void (__far *)())&__sigstub;
        msg.s.type = _PROC_SIGNAL;
        msg.s.subtype = _SIGTABLE;
        msg.s.segment = _FP_SEG( &_SignalTable );
        msg.s.offset = (long)_FP_OFF( &_SignalTable );
        msg.s.zero1 = 0;
        Send( PROC_PID, &msg.s, &msg.r, sizeof( msg.s ), sizeof( msg.r ) );
        first = 0;
        _RWD_abort = __sigabort;           /* change the abort rtn address */
    }

    if( (sig < _SIGMIN) || (sig > _SIGMAX) ) {
        return( __set_EINVAL() );
    }

    /*
     * The process manager will stuff the signal table itself
     * before the send returns. It does the stuffing because
     * the update must be atomic (user procs don't do cli ... sti).
     */
    if( act ) {
        msg.s.type = _PROC_SIGNAL;
        msg.s.subtype = _SIGACT;
        msg.s.signum = sig;
        msg.s.segment = _FP_SEG( act->sa_handler );
        msg.s.offset = (long)_FP_OFF( act->sa_handler );
        msg.s.mask = act->sa_mask;
        msg.s.zero1 = 0;
    }

    if( oact )
        *oact = _SignalTable.acttable[sig - 1];

    if( act ) {
        Send( PROC_PID, &msg.s, &msg.r, sizeof( msg.s ), sizeof( msg.r ) );
        if( msg.r.status != EOK ) {
            lib_set_errno( msg.r.status );
            return( -1 );
        }
    }

    return( 0 );
}
