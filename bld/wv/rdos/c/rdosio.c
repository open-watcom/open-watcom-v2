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
* Description:  RDOS file-IO module. Uses standard C support libraries.
*
****************************************************************************/


#include <io.h>
#include <limits.h>
#include <stddef.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgmem.h"
#include "dbgio.h"
#include "errno.h"
#include "strutil.h"
#include "filelcl.h"

file_components         LclFile = { '.', ':', { '\\', '/' }, { '\r', '\n' } };
char                    LclPathSep = { ';' };


void LocalErrMsg( sys_error code, char *buff )
{
    StrCopy( strerror( code ), buff );
}

sys_handle LocalOpen( const char *name, open_access access )
{
    unsigned    openmode;
    int         ret;

    if( (access & OP_WRITE) == 0 ) {
        openmode = O_RDONLY;
    } else if( access & OP_READ ) {
        openmode = O_RDWR;
    } else {
        openmode = O_WRONLY;
    }
    if( access & OP_CREATE ) openmode |= O_CREAT;
    if( access & OP_TRUNC ) openmode |= O_TRUNC;
    ret = open( name, openmode | O_BINARY, 0666 );
    if( (int)ret == -1 ) {
        StashErrCode( errno, OP_LOCAL );
        return( NIL_SYS_HANDLE );
    }
//    fcntl( ret, F_SETFD, (int)FD_CLOEXEC );
    return( ret );
}

size_t LocalRead( sys_handle filehndl, void *ptr, size_t len )
{
    int         ret;
    size_t      total;
    unsigned    buff_len;
    unsigned    read_len;

    buff_len = INT_MAX;
    total = 0;
    while( len > 0 ) {
        if( buff_len > len )
            buff_len = (unsigned)len;
        ret = read( filehndl, ptr, buff_len );
        if( ret < 0 ) {
            StashErrCode( errno, OP_LOCAL );
            return( ERR_RETURN );
        }
        read_len = (unsigned)ret;
        total += read_len;
        if( read_len != buff_len )
            break;
        ptr = (char *)ptr + read_len;
        len -= read_len;
    }
    return( total );
}

size_t LocalWrite( sys_handle filehndl, const void *ptr, size_t len )
{
    int         ret;
    size_t      total;
    unsigned    buff_len;
    unsigned    write_len;

    buff_len = INT_MAX;
    total = 0;
    while( len > 0 ) {
        if( buff_len > len )
            buff_len = (unsigned)len;
        ret = write( filehndl, ptr, buff_len );
        if( ret < 0 ) {
            StashErrCode( errno, OP_LOCAL );
            return( ERR_RETURN );
        }
        write_len = (unsigned)ret;
        total += write_len;
        if( write_len != buff_len )
            break;
        ptr = (char *)ptr + write_len;
        len -= write_len;
    }
    return( total );
}

unsigned long LocalSeek( sys_handle hdl, unsigned long len, seek_method method )
{
    off_t       ret;

    ret = lseek( hdl, len, method );
    if( ret == (off_t)-1 ) {
        StashErrCode( errno, OP_LOCAL );
        return( -1UL );
    }
    return( ret );
}

error_idx LocalClose( sys_handle filehndl )
{
    if( close( filehndl ) == 0 )
        return( 0 );
    return( StashErrCode( errno, OP_LOCAL ) );
}

error_idx LocalErase( const char *name )
{
    if( remove( name ) == 0 )
        return( 0 );
    return( StashErrCode( errno, OP_LOCAL ) );
}

sys_handle LocalHandleSys( handle h )
{
    return( h );
}
