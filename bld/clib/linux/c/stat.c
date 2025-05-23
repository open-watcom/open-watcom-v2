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
* Description:  Linux stat() implementation.
*
****************************************************************************/


#include "variety.h"
#include <sys/stat.h>
#include "linuxsys.h"
#include "kstat.h"


_WCRTLINK int stat( const char *filename, struct stat * __buf )
/**************************************************************
 * Even on 64-bit Linux, Open Watcom compiles itself as 32-bit. The problem with stat()
 * for 32-bit processes is that some device nodes are higher than 255 which are normally
 * returned in st_dev, but the Linux kernel will fail the call with EOVERFLOW if the
 * returned st_dev (or any other field) exceeds the storage limits of the plain stat()
 * struct. If compiling on a filesystem mounted from NVME storage (device node 259, x),
 * stat() will fail on every file on that filesystem. To work around this, use stat64()
 * and translate to stat(). Return EOVERFLOW only if the file is too large. Other fields
 * like st_dev and st_rdev are irrelevant to a compiler and should be ignored.
 */
{
#if 0
    struct kstat64  ks;
#else
    struct stat64  ks;
#endif

    syscall_res res = sys_call2( SYS_stat64, (u_long)filename, (u_long)&ks );
    if( !__syscall_iserror( res ) ) {
        if( ks.st_size <= 0x7FFFFFFFU /*2GB - 1*/ ) {
            COPY_STAT( __buf, ks );
        } else {
            res = (syscall_res)(-EOVERFLOW);
        }
    }
    __syscall_return( int, res );
}
