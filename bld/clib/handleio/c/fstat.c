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
#include <stdio.h>
#include <stddef.h>
#include <io.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <errno.h>
#include "tinyio.h"
#include "iomode.h"
#include "rtcheck.h"
#include "seterrno.h"
#include "widechar.h"


extern time_t _d2ttime();


#ifdef __WIDECHAR__
_WCRTLINK int _wfstat( int handle, struct _wstat *buf )
#else
_WCRTLINK int fstat( int handle, struct stat *buf )
#endif
    {
        unsigned        iomode_flags;
        union {
            tiny_ret_t rc;
            tiny_file_stamp_t stamp;
        } u;

        __handle_check( handle, -1 );

        u.rc = TinyGetDeviceInfo( handle );
        if( TINY_ERROR(u.rc) ) {
            return( __set_errno_dos( TINY_INFO(u.rc) ) );
        }
        /* isolate drive number */
        buf->st_dev = TINY_INFO(u.rc) & TIO_CTL_DISK_DRIVE_MASK;
        buf->st_rdev = buf->st_dev;

        buf->st_nlink = 1;
        buf->st_uid = buf->st_gid = 0;
        buf->st_ino = handle;
        buf->st_mode = 0;
        iomode_flags = __GetIOMode( handle );
        if( iomode_flags & _READ ) {
            buf->st_mode |= S_IRUSR | S_IRGRP | S_IROTH;
        }
        if( iomode_flags & _WRITE ) {
            buf->st_mode |= S_IWUSR | S_IWGRP | S_IWOTH;
        }
        if( TINY_INFO(u.rc) & TIO_CTL_DEVICE ) {
            buf->st_size = 0;
            buf->st_atime = buf->st_ctime = buf->st_mtime = 0;
            buf->st_mode |= S_IFCHR;
        } else {                /* file */
            u.rc = TinyGetFileStamp( handle );
            if( TINY_ERROR(u.rc) ) {
                return( __set_errno_dos( TINY_INFO(u.rc) ) );
            }
            buf->st_mtime = _d2ttime( u.stamp.date, u.stamp.time );
            buf->st_atime = buf->st_ctime = buf->st_mtime;
            buf->st_size = filelength( handle );
            buf->st_mode |= S_IFREG;
        }
        buf->st_btime = buf->st_mtime;
        buf->st_attr = 0;
        buf->st_archivedID = 0;
        buf->st_updatedID = 0;
        buf->st_inheritedRightsMask = 0;
        buf->st_originatingNameSpace = 0;
        buf->st_name[0] = '\0';
        return( 0 );
    }

