/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2020 The Open Watcom Contributors. All Rights Reserved.
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
#include <stddef.h>
#include <process.h>
#define INCL_DOSPROCESS
#define INCL_DOSERRORS
#include <wos2.h>
#include "rterrno.h"
#include "thread.h"


_WCRTLINK int cwait( int *status, int process_id, int action )
{
    APIRET      rc;
    RESULTCODES retval;
    PID         pid;
    #pragma pack(__push,1);
    union {
        int         stat;
        struct {
            unsigned char al, ah;
        } s;
    } u;
    #pragma pack(__pop);

    rc = DosCwait( action, 0, &retval, &pid, process_id );
    if( rc != 0 ) {
        _RWD_errno = ( rc == ERROR_WAIT_NO_CHILDREN ) ? ECHILD : EINVAL;
        return( -1 );
    } else {
        u.stat = rc;
        u.s.al = retval.codeTerminate;
        if( u.s.al == 0 )
            u.s.ah = retval.codeResult;
        if( status != NULL )
            *status = u.stat;
        if( u.s.al != 0 ) {
            _RWD_errno = EINTR;
            return( -1 );
        }
    }
    return( pid );
}
