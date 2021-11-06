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
* Description:  Handle based file operations for RDOS kernel mode.
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
#include <rdosdev.h>
#include "rtdata.h"
#include "liballoc.h"
#include "rtinit.h"
#include "iomode.h"
#include "openmode.h"
#include "qread.h"
#include "qwrite.h"
#include "_rdos.h"


typedef struct rdos_handle_type
{
    int         rdos_handle;
    unsigned    mode;
    long        pos;
    int         ref_count;
} rdos_handle_type;

static rdos_handle_type **handle_ptr;
int handle_count;

static struct TKernelSection handle_section;

static rdos_handle_type *AllocHandleObj( void )
{
    rdos_handle_type *h;

    h = ( rdos_handle_type * )lib_malloc( sizeof( rdos_handle_type ) );
    h->pos = 0;
    h->rdos_handle = 0;
    h->mode = 0;
    h->ref_count = 1;
    return( h );
}

static void FreeHandleObj( rdos_handle_type *h)
{
    h->ref_count--;

    if( h->ref_count == 0 ) {
        if( h->rdos_handle )
            RdosCloseCFile( h->rdos_handle );
        lib_free( h );
    }
}

static void GrowHandleArr( void )
{
    int                 i;
    int                 new_count;
    rdos_handle_type  **new_ptr;

    new_count = 1 + 3 * handle_count / 2;
    new_ptr = ( rdos_handle_type ** )lib_malloc( new_count * sizeof( rdos_handle_type * ) );

    for( i = 0; i < handle_count; i++ )
        new_ptr[i] = handle_ptr[i];

    for( i = handle_count; i < new_count; i++ )
        new_ptr[i] = 0;

    lib_free( handle_ptr );
    handle_ptr = new_ptr;
    handle_count = new_count;
}

static int AllocHandleEntry( rdos_handle_type *obj )
{
    int i;

    RdosEnterKernelSection( &handle_section );

    for( i = 0; i < handle_count; i++)
        if( handle_ptr[i] == 0 )
            break;

    if( i == handle_count )
        GrowHandleArr();

    handle_ptr[i] = obj;

    RdosLeaveKernelSection( &handle_section );

    return( i );
}

static rdos_handle_type *FreeHandleEntry( int handle )
{
    rdos_handle_type *obj = 0;

    RdosEnterKernelSection( &handle_section );

    if( handle >= 0 && handle < handle_count ) {
        if( handle_ptr[handle] ) {
            obj = handle_ptr[handle];
            handle_ptr[handle] = 0;
        }
    }

    RdosLeaveKernelSection( &handle_section );

    return( obj );
}

static int ReplaceHandleEntry( int handle, rdos_handle_type *new_obj )
{
    int                 ok = 0;
    rdos_handle_type   *obj = 0;

    RdosEnterKernelSection( &handle_section );

    if( handle >= 0 && handle < handle_count ) {
        if( handle_ptr[handle] )
            obj = handle_ptr[handle];

        handle_ptr[handle] = new_obj;
        ok = 1;
    }

    RdosLeaveKernelSection( &handle_section );

    if( obj )
        FreeHandleObj( obj );

    return ( ok );
}

static int GetHandle( int handle )
{
    int rdos_handle = -1;

    RdosEnterKernelSection( &handle_section );

    if( handle >= 0 && handle < handle_count ) {
        if( handle_ptr[handle] ) {
            rdos_handle = handle_ptr[handle]->rdos_handle;
        }
    }

    RdosLeaveKernelSection( &handle_section );

    return( rdos_handle );
}

static long GetHandlePos( int handle )
{
    long pos = 0;

    RdosEnterKernelSection( &handle_section );

    if( handle >= 0 && handle < handle_count ) {
        if( handle_ptr[handle] ) {
            pos = handle_ptr[handle]->pos;
        }
    }

    RdosLeaveKernelSection( &handle_section );

    return( pos );
}

static void SetHandlePos( int handle, long pos )
{
    RdosEnterKernelSection( &handle_section );

    if( handle >= 0 && handle < handle_count ) {
        if( handle_ptr[handle] ) {
            handle_ptr[handle]->pos = pos;
        }
    }

    RdosLeaveKernelSection( &handle_section );
}

static void InitHandle( void )
{
    int i;

    RdosInitKernelSection( &handle_section );
    handle_count = 5;
    handle_ptr = ( rdos_handle_type ** )lib_malloc( handle_count * sizeof( rdos_handle_type * ) );

    for( i = 0; i < handle_count; i++ )
        handle_ptr[i] = 0;

}

_WCRTLINK int unlink( const CHAR_TYPE *filename )
{
    __ptr_check( filename, 0 );

    if( RdosDeleteFile( filename ) ) {
        return( 0 );
    } else {
        return( -1 );
    }
}

unsigned __GetIOMode( int handle )
{
    unsigned mode = 0;

    RdosEnterKernelSection( &handle_section );

    if( handle >= 0 && handle < handle_count )
        if( handle_ptr[handle] )
            mode = handle_ptr[handle]->mode;

    RdosLeaveKernelSection( &handle_section );

    return( mode );
}

void __SetIOMode_nogrow( int handle, unsigned value )
{
    RdosEnterKernelSection( &handle_section );

    if( handle >= 0 && handle < handle_count )
        if( handle_ptr[handle] )
            handle_ptr[handle]->mode = value;

    RdosLeaveKernelSection( &handle_section );
}

signed __SetIOMode( int handle, unsigned value )
{
    signed ret = -1;

    RdosEnterKernelSection( &handle_section );

    if( handle >= 0 && handle < handle_count )
        if( handle_ptr[handle] ) {
            handle_ptr[handle]->mode = value;
            ret = handle;
        }

    RdosLeaveKernelSection( &handle_section );

    return( ret );
}

static int open_base( const CHAR_TYPE *name, int mode )
{
    int                 handle;
    rdos_handle_type   *obj;
    int                 rdos_handle;
    unsigned            iomode_flags;
    int                 rwmode;

    rwmode = mode & OPENMODE_ACCESS_MASK;
    iomode_flags = 0;
    if( mode == O_RDWR )                iomode_flags |= _READ | _WRITE;
    if( rwmode == O_RDONLY)             iomode_flags |= _READ;
    if( rwmode == O_WRONLY)             iomode_flags |= _WRITE;
    if( mode & O_APPEND )               iomode_flags |= _APPEND;
    if( mode & (O_BINARY|O_TEXT) )
        if( mode & O_BINARY )           iomode_flags |= _BINARY;

    rdos_handle = RdosOpenKernelFile( name, mode );
    if( rdos_handle ) {
        obj = AllocHandleObj();
        if( obj ) {
            obj->rdos_handle = rdos_handle;
            obj->mode = iomode_flags;
            obj->pos = 0;
            handle = AllocHandleEntry( obj );
            return( handle );
        }
    }
    return( -1 );
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

    return( open_base( name, mode ) );
}

_WCRTLINK int open( const CHAR_TYPE *name, int mode, ... )
{
    int                 permission;
    va_list             args;

    va_start( args, mode );
    permission = va_arg( args, int );
    va_end( args );

    return( open_base( name, mode ) );
}


_WCRTLINK int _sopen( const CHAR_TYPE *name, int mode, int shflag, ... )
{
    va_list             args;

    va_start( args, shflag );
    va_end( args );

    return( open_base( name, mode ) );
}

_WCRTLINK int close( int handle )
{
    rdos_handle_type   *obj;

    obj = FreeHandleEntry( handle );

    if( obj ) {
        FreeHandleObj( obj );
        return( 0 );
    }

    return( -1 );
}

_WCRTLINK int dup( int handle )
{
    rdos_handle_type   *obj = 0;

    RdosEnterKernelSection( &handle_section );

    if( handle >= 0 && handle < handle_count )
        if( handle_ptr[handle] )
            obj = handle_ptr[handle];

    if( obj )
        obj->ref_count++;

    RdosLeaveKernelSection( &handle_section );

    if( obj ) {
        return( AllocHandleEntry( obj ) );
    } else {
        return( -1 );
    }
}

_WCRTLINK int dup2( int handle1, int handle2 )
{
    rdos_handle_type   *obj = 0;

    if( handle1 == handle2 )
        return( handle2 );

    if( handle2 < 0 )
        return( -1 );

    while( handle2 >= handle_count )
        GrowHandleArr( );

    RdosEnterKernelSection( &handle_section );

    if( handle1 >= 0 && handle1 < handle_count )
        if( handle_ptr[handle1] )
            obj = handle_ptr[handle1];

    if( obj )
        obj->ref_count++;

    RdosLeaveKernelSection( &handle_section );

    if( obj ) {
        if( ReplaceHandleEntry( handle2, obj ) ) {
            return( handle2 );
        } else {
            obj->ref_count--;
        }
    }

    return( -1 );
}

_WCRTLINK int _eof( int handle )
{
    int         rdos_handle;
    long        pos;

    rdos_handle = GetHandle( handle );
    if( rdos_handle > 0 ) {
        pos = GetHandlePos( handle );
        if( RdosGetCFileSize( rdos_handle ) == pos ) {
            return( 1 );
        } else {
            return( 0 );
        }
    } else {
        return( -1 );
    }
}

_WCRTLINK long _filelength( int handle )
{
    int         rdos_handle;

    rdos_handle = GetHandle( handle );
    if( rdos_handle > 0 ) {
        return( RdosGetCFileSize( rdos_handle ) );
    } else {
        return( -1 );
    }
}

_WCRTLINK int _chsize( int handle, long size )
{
    int         rdos_handle;

    rdos_handle = GetHandle( handle );
    if( rdos_handle > 0 ) {
        RdosSetCFileSize( rdos_handle, size );
        return( size );
    } else
        return( -1 );
}

_WCRTLINK int fstat( int handle, struct stat *buf )
{
    unsigned long       msb;
    unsigned long       lsb;
    int                 ms;
    int                 us;
    struct tm           tm;
    char                type;
    int                 rdos_handle;
    unsigned            mode;

    rdos_handle = GetHandle( handle );

    if( rdos_handle > 0 ) {
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

        buf->st_dev = 0;
        buf->st_rdev = buf->st_dev;

        RdosGetCFileTime( rdos_handle, &msb, &lsb );
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
        return( 0 );
    } else
        return( -1 );
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
    int         rdos_handle;

    rdos_handle = GetHandle( handle );

    if( rdos_handle > 0 ) {
        switch( origin ) {
        case SEEK_SET:
            SetHandlePos( handle, offset );
            break;

        case SEEK_CUR:
            pos = GetHandlePos( handle );
            pos += offset;
            SetHandlePos( handle, pos );
            break;

        case SEEK_END:
            pos = RdosGetCFileSize( rdos_handle );
            pos += offset;
            SetHandlePos( handle, pos );
            break;
        }
        return( GetHandlePos( handle ) );
    } else
        return( -1 );
}

_WCRTLINK off_t _tell( int handle )
{
    return( GetHandlePos( handle ) );
}

int __qread( int handle, void *buffer, unsigned len )
{
    int         rdos_handle;
    long        pos;
    long        count;

    rdos_handle = GetHandle( handle );

    if( rdos_handle > 0 ) {
        pos = GetHandlePos( handle );
        count = RdosReadCFile( rdos_handle, buffer, len, pos );
        pos += count;
        SetHandlePos( handle, pos );
        return( count );
    } else
        return( -1 );
}

int __qwrite( int handle, const void *buffer, unsigned len )
{
    int         rdos_handle;
    long        pos;
    long        count;

    rdos_handle = GetHandle( handle );

    if( rdos_handle > 0 ) {
        pos = GetHandlePos( handle );
        count = RdosWriteCFile( rdos_handle, buffer, len, pos );
        pos += count;
        SetHandlePos( handle, pos );
        return( count );
    } else
        return( -1 );
}

_WCRTLINK int read( int handle, void *buffer, unsigned len )
{
    return( __qread( handle, buffer, len ) );
}

_WCRTLINK int write( int handle, const void *buffer, unsigned len )
{
    return( __qwrite( handle, buffer, len ) );
}

AXI(InitHandle,INIT_PRIORITY_LIBRARY);
