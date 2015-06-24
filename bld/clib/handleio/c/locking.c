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
* Description:  Implementation of the locking() function.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#ifdef __UNIX__
#include <fcntl.h>
#else
#include <dos.h>
#endif
#include <time.h>
#include <sys/locking.h>
#include "rterrno.h"
#include "iomode.h"
#include "rtcheck.h"
#include "lseek.h"
#include "thread.h"

#ifdef __UNIX__
_WCRTLINK int (locking)( int handle, int mode, unsigned long nbytes )
{
    int                 cmd;
    struct flock        flock_buff;
    int                 ret;
    int                 tries;

    __handle_check( handle, -1 );

    flock_buff.l_whence = SEEK_SET;
    flock_buff.l_start = lseek( handle, 0L, SEEK_CUR );
    flock_buff.l_len = nbytes;

    switch( mode ) {
    case LK_UNLCK:      /* unlock */
        cmd = F_SETLK;
        flock_buff.l_type = F_UNLCK;
        break;
    case LK_LOCK:       /* blocking read/write lock */
        cmd = F_SETLK;
        flock_buff.l_type = F_WRLCK;
        break;
    case LK_NBLCK:      /* non-blocking read/write lock */
        cmd = F_SETLK;
        flock_buff.l_type = F_WRLCK;
        break;
    case LK_RLCK:       /* blocking read lock */
        mode = LK_LOCK; /* so we only have to check for 1 thing in loop */
        cmd = F_SETLK;
        flock_buff.l_type = F_RDLCK;
        break;
    case LK_NBRLCK:     /* non-blocking read lock */
        cmd = F_SETLK;
        flock_buff.l_type = F_RDLCK;
        break;
    default:
        _RWD_errno = ENOSYS;
        return( -1 );
    }
    tries = 10;
    for( ;; ) {
        ret = fcntl( handle, cmd, &flock_buff );
        if( ret != -1 ) break;
        if( _RWD_errno != EAGAIN ) break;
        if( mode != LK_LOCK ) break;
        if( --tries == 0 ) break;
        sleep( 1 );
    }
    if( _RWD_errno == EAGAIN ) _RWD_errno = EDEADLK;
    return( ret );
}

#else

_WCRTLINK int (locking)( int handle, int mode, unsigned long nbytes )
{
    unsigned long offset;
    int retry_count;
    int rc;

    __handle_check( handle, -1 );

    offset = __lseek( handle, 0L, SEEK_CUR );
    if( mode == LK_UNLCK ) return( unlock( handle, offset, nbytes ) );
    for( retry_count = 0; retry_count < 10; ++retry_count ) {
        rc = lock( handle, offset, nbytes );
        if( rc == 0 )  return( 0 );
        if( mode == LK_NBLCK  ||  mode == LK_NBRLCK ) {
            return( rc );                               /* JBS 22-aug-90 */
        }
        sleep( 1 );                             /* wait 1 second */
    }
    _RWD_doserrno = rc;
    _RWD_errno = EDEADLOCK;
    return( -1 );
}
#endif
