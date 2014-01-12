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


#include "cpplib.h"
#include <except.h>
#include "rtexcept.h"
#include "rtmsgs.h"


extern "C"
void CPPLIB( corrupted_stack )( // TERMINATE, WITH CORRUPTED STACK MSG
    void )
{
    CPPLIB( call_terminate )( RTMSG_CORRUPT_STK, PgmThread() );
}


extern "C"
void CPPLIB( call_terminate )(  // CALL "terminate" SET BY "set_terminate"
    char* abort_msg,            // - abortion message
    THREAD_CTL *thr )           // - thread-specific data
{
    ACTIVE_EXC *exc;            // - active exception

    thr->abort_msg = abort_msg;
    exc = thr->excepts;
    if( exc != NULL ) {
        exc->state = EXCSTATE_TERMINATE;
    }
    thr->flags.terminated = true;
    std::terminate();
    CPPLIB(fatal_runtime_error)( RTMSG_RET_TERMIN, 1 );
}
