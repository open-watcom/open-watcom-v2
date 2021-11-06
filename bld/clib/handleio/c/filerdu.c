/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Handle based file operations for RDOS.
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <share.h>
#include <time.h>
#include <sys/locking.h>
#include <rdos.h>
#include "rtdata.h"
#include "liballoc.h"
#include "rtinit.h"
#include "iomode.h"
#include "openmode.h"
#include "qread.h"
#include "qwrite.h"
#include "_rdos.h"

_WCRTLINK int unlink( const CHAR_TYPE *filename )
{
    __ptr_check( filename, 0 );

    if( RdosDeleteFile( filename ) )
        return( 0 );
    else
        return( -1 );
}

unsigned __GetIOMode( int handle )
{
    return( RdosGetHandleMode( handle ) );
}

void __SetIOMode_nogrow( int handle, unsigned value )
{
    RdosSetHandleMode( handle, value );
}

signed __SetIOMode( int handle, unsigned value )
{
    return( RdosSetHandleMode( handle, value ) );
}

_WCRTLINK int creat( const CHAR_TYPE *name, mode_t pmode )
{
    unsigned mode;

    mode = O_CREAT | O_TRUNC;
    if( (pmode & S_IWRITE) && (pmode & S_IREAD) ) {
        mode |= O_RDWR;
    } else if( pmode & S_IWRITE ) {
        mode |= O_WRONLY;
    } else if( pmode & S_IREAD ) {
        mode |= O_RDONLY;
    } else if( !pmode ) {
        mode |= O_RDWR;
    }

    return( RdosOpenHandle( name, mode ) );
}

_WCRTLINK int open( const CHAR_TYPE *name, int mode, ... )
{
    int                 permission;
    va_list             args;

    va_start( args, mode );
    permission = va_arg( args, int );
    va_end( args );

    return( RdosOpenHandle( name, mode ) );
}


_WCRTLINK int _sopen( const CHAR_TYPE *name, int mode, int shflag, ... )
{
    va_list             args;

    va_start( args, shflag );
    va_end( args );

    return( RdosOpenHandle( name, mode ) );
}

_WCRTLINK int close( int handle )
{
    return( RdosCloseHandle( handle ) );
}

_WCRTLINK int dup( int handle )
{
    return( RdosDupHandle( handle ) );
}

_WCRTLINK int dup2( int handle1, int handle2 )
{
    return( RdosDup2Handle( handle1, handle2 ) );
}

_WCRTLINK int _eof( int handle )
{
    return( RdosEofHandle( handle ) );
}

_WCRTLINK long _filelength( int handle )
{
    return( RdosGetHandleSize( handle ) );
}

_WCRTLINK int _chsize( int handle, long size )
{
    return( RdosSetHandleSize( handle, size ) );
}

_WCRTLINK int fstat( int handle, struct stat *buf )
{
    unsigned long       msb;
    unsigned long       lsb;
    int                 ms;
    int                 us;
    struct tm           tm;
    int                 res = -1;

    buf->st_attr = 0;
    buf->st_archivedID = 0;
    buf->st_updatedID = 0;
    buf->st_inheritedRightsMask = 0;
    buf->st_originatingNameSpace = 0;
    buf->st_nlink = 1;
    buf->st_uid = buf->st_gid = 0;
    buf->st_ino = handle;
    buf->st_mode = 0;
    buf->st_mode |= S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IWOTH;

    if( RdosIsHandleDevice( handle ) ) {
        res = 0;
        buf->st_dev = 1;
        buf->st_size = 0;
        buf->st_atime = buf->st_ctime = buf->st_mtime = 0;
        buf->st_btime = buf->st_mtime;
        buf->st_mode |= S_IFCHR;
    } else {
        res = RdosGetHandleTime( handle, &msb, &lsb );
        if( res >= 0 ) {
            RdosDecodeMsbTics( msb,
                               &tm.tm_year,
                               &tm.tm_mon,
                               &tm.tm_mday,
                               &tm.tm_hour );

            RdosDecodeLsbTics( lsb,
                               &tm.tm_min,
                               &tm.tm_sec,
                               &ms,
                               &us );

            tm.tm_year -= 1900;
            tm.tm_mon--;
            tm.tm_isdst = -1;
            tm.tm_wday = -1;
            tm.tm_yday = -1;

            buf->st_mtime = mktime( &tm );
            buf->st_atime = buf->st_ctime = buf->st_mtime;
            buf->st_size = _filelength( handle );
            buf->st_mode |= S_IFREG;
            buf->st_btime = buf->st_mtime;
        }
    }

    return( res );
}

_WCRTLINK int _setmode( int handle, int mode )
{
    return( mode );
}

_WCRTLINK int fsync( int handle )
{
    return( -1 );
}

_WCRTLINK int lock( int handle, unsigned long offset, unsigned long nbytes )
{
    return( -1 );
}

_WCRTLINK int unlock( int handle, unsigned long offset, unsigned long nbytes )
{
    return( -1 );
}

_WCRTLINK int (locking)( int handle, int mode, unsigned long nbytes )
{
    return( -1 );
}

_WCRTLINK off_t lseek( int handle, off_t offset, int origin )
{
    off_t       pos;

    switch( origin ) {
    case SEEK_SET:
        RdosSetHandlePos( handle, offset );
        break;

    case SEEK_CUR:
        pos = RdosGetHandlePos( handle );
        pos += offset;
        RdosSetHandlePos( handle, pos );
        break;

    case SEEK_END:
        pos = RdosGetHandleSize( handle );
        pos += offset;
        RdosSetHandlePos( handle, pos );
        break;
    }

    return( RdosGetHandlePos( handle ) );
}

_WCRTLINK off_t _tell( int handle )
{
    return( RdosGetHandlePos( handle ) );
}

int __qread( int handle, void *buffer, unsigned len )
{
    return( RdosReadHandle( handle, buffer, len ) );
}

int __qwrite( int handle, const void *buffer, unsigned len )
{
    return( RdosWriteHandle( handle, buffer, len ) );
}

_WCRTLINK int read( int handle, void *buffer, unsigned len )
{
    return( RdosReadHandle( handle, buffer, len ) );
}

_WCRTLINK int write( int handle, const void *buffer, unsigned len )
{
    return( RdosWriteHandle( handle, buffer, len ) );
}
