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


#include "variety.h"
#include <windows.h>
#include <stddef.h>
#include <process.h>
#include <errno.h>
#include "rtdata.h"
#include "seterrno.h"

_WCRTLINK int cwait(int *status, int process_id, int action)
/***********************************************************/
{
    DWORD  rc;
    HANDLE p = (HANDLE)process_id;

    if (action != WAIT_CHILD) {
        __set_errno(EINVAL);
        return -1;
    }

    rc = WaitForSingleObject(p, INFINITE);

    if (rc == WAIT_FAILED) {
        rc = GetLastError();
        __set_errno(EINVAL);
        return -1;
    } else if (rc == WAIT_OBJECT_0) {
        GetExitCodeProcess(p, &rc);
        CloseHandle(p);
        *status = (rc << 8) & 0xff00;
        return process_id;
    } else {
        CloseHandle(p);
        __set_errno(EINVAL);
        return -1;
    }
}
