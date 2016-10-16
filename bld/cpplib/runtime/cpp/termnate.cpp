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
#include <stddef.h>
#include <stdlib.h>
#include "rtexcept.h"
#include <except.h>
#include "rtmsgs.h"
#include "exitwmsg.h" 


namespace std {

_WPRTLINK
_NORETURN
void terminate( void )          // HANDLE TERMINATE
{
    PFV handler;                // - NULL or handler set by "set_terminate"
    THREAD_CTL *thr;            // - thread ptr
    char* msg;                  // - error message

    thr = &_RWD_ThreadData;
    handler = thr->terminate;
    if( NULL == handler ) {
        thr = PgmThread();
        msg = thr->abort_msg;
        if( msg == NULL ) {
            __exit( 1 );
        }
    } else {
        (*handler)();
        msg = RTMSG_RET_TERMIN;
    }
    CPPLIB( fatal_runtime_error )( msg, 1 );
    // never return
}

}
