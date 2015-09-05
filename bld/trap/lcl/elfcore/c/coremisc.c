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
* Description:  Support routines for core file debugging.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "trpimp.h"


trap_retval ReqFile_get_config( void )
{
    file_get_config_ret *ret;

    ret = GetOutPtr( 0 );

    ret->file.ext_separator = '.';
#ifdef __UNIX__
    ret->file.drv_separator = '\0';
    ret->file.path_separator[0] = '/';
    ret->file.path_separator[1] = '\0';
    ret->file.newline[0] = '\n';
    ret->file.newline[1] = '\0';
#else
    ret->file.drv_separator = ':';
    ret->file.path_separator[0] = '\\';
    ret->file.path_separator[1] = '/';
    ret->file.newline[0] = '\r';
    ret->file.newline[1] = '\n';
#endif
    return( sizeof( *ret ) );
}

trap_retval ReqFile_open( void )
{
    file_open_req       *acc;
    file_open_ret       *ret;
    int                 handle;
    static const int    MapAcc[] = { O_RDONLY, O_WRONLY, O_RDWR };
    int                 mode;
    int                 access;
    const char          *name;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    name = GetInPtr( sizeof( *acc ) );
    mode = MapAcc[ (acc->mode & (TF_READ|TF_WRITE)) - 1];
    access = S_IRUSR|S_IWUSR | S_IRGRP|S_IWGRP | S_IROTH|S_IWOTH;
    if( acc->mode & TF_CREATE ) {
        mode |= O_CREAT | O_TRUNC;
        if( acc->mode & TF_EXEC )
            access |= S_IXUSR | S_IXGRP | S_IXOTH;
    }
    handle = open( name, mode, access );
    if( handle != -1 ) {
        errno = 0;
        ret->err = 0;
        ret->handle = handle;
    } else {
        ret->err = errno;
        ret->handle = 0;
    }
    return( sizeof( *ret ) );
}

trap_retval ReqFile_seek( void )
{
    file_seek_req       *acc;
    file_seek_ret       *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->pos = lseek( acc->handle, acc->pos, acc->mode );
    if( ret->pos != ((off_t)-1) ) {
        errno = 0;
        ret->err = 0;
    } else {
        ret->err = errno;
    }
    CONV_LE_32( ret->pos );
    CONV_LE_32( ret->err );
    return( sizeof( *ret ) );
}

trap_retval ReqFile_read( void )
{
    unsigned            total;
    unsigned            len;
    char                *ptr;
    size_t              curr;
    ssize_t             rv;
    file_read_req       *acc;
    file_read_ret       *ret;

    acc = GetInPtr( 0 );
    CONV_LE_32( acc->handle );
    CONV_LE_16( acc->len );
    ret = GetOutPtr( 0 );
    ptr = GetOutPtr( sizeof( *ret ) );
    len = acc->len;
    total = 0;
    for( ;; ) {
        if( len == 0 ) break;
        curr = len;
        if( curr > INT_MAX ) curr = INT_MAX;
        rv = read( acc->handle, ptr, curr );
        if( rv < 0 ) {
            total = -1;
            break;
        }
        total += rv;
        if( rv != curr ) break;
        ptr += rv;
        len -= rv;
    }
    if( total == -1 ) {
        total = 0;
    } else {
        errno = 0;
    }
    ret->err = errno;
    CONV_LE_32( ret->err );
    return( sizeof( *ret ) + total );
}

static unsigned DoWrite( int hdl, unsigned_8 *ptr, unsigned len )
{
    unsigned    total;
    unsigned    curr;
    int         rv;

    total = 0;
    for( ;; ) {
        if( len == 0 ) break;
        curr = len;
        if( curr > INT_MAX ) curr = INT_MAX;
        rv = write( hdl, ptr, curr );
        if( rv <= 0 ) {
            total = -1;
            break;
        }
        total += rv;
        ptr += rv;
        len -= rv;
    }
    if( total == -1 ) {
        total = 0;
    } else {
        errno = 0;
    }
    return( total );
}

trap_retval ReqFile_write( void )
{
    file_write_req      *acc;
    file_write_ret      *ret;

    acc = GetInPtr( 0 );
    CONV_LE_32( acc->handle );
    ret = GetOutPtr( 0 );
    ret->len = DoWrite( acc->handle, GetInPtr( sizeof( *acc ) ),
                        GetTotalSize() - sizeof( *acc ) );
    ret->err = errno;
    CONV_LE_32( ret->err );
    CONV_LE_16( ret->len );
    return( sizeof( *ret ) );
}

trap_retval ReqFile_write_console( void )
{
    file_write_console_ret      *ret;

    ret = GetOutPtr( 0 );
    ret->len = DoWrite( 2, GetInPtr( sizeof( file_write_console_req ) ),
                        GetTotalSize() - sizeof( file_write_console_req ) );
    ret->err = errno;
    CONV_LE_32( ret->err );
    CONV_LE_16( ret->len );
    return( sizeof( *ret ) );
}

trap_retval ReqFile_close( void )
{
    file_close_req      *acc;
    file_close_ret      *ret;

    acc = GetInPtr( 0 );
    CONV_LE_32( acc->handle );
    ret = GetOutPtr( 0 );
    if( close( acc->handle ) != -1 ) {
        errno = 0;
        ret->err = 0;
    } else {
        ret->err = errno;
    }
    CONV_LE_32( ret->err );
    return( sizeof( *ret ) );
}

trap_retval ReqFile_erase( void )
{
    file_erase_ret      *ret;

    ret = GetOutPtr( 0 );
    if( unlink( (char *)GetInPtr( sizeof( file_erase_req ) ) ) != 0 ) {
        ret->err = errno;
    } else {
        errno = 0;
        ret->err = 0;
    }
    CONV_LE_32( ret->err );
    return( sizeof( *ret ) );
}

trap_retval ReqSet_user_screen( void )
{
    return( 0 );
}

trap_retval ReqSet_debug_screen( void )
{
    return( 0 );
}

unsigned TryOnePath( const char *path, struct stat *tmp, const char *name, char *result )
{
    char        *ptr;

    if( path == NULL )
        return( 0 );
    ptr = result;
    for( ;; ) {
        switch( *path ) {
        case ':':
        case '\0':
            if( ptr != result && ptr[-1] != '/' )
                *ptr++ = '/';
            strcpy( ptr, name );
            if( stat( result, tmp ) == 0 )
                return( strlen( result ) + 1 );
            if( *path == '\0' )
                return( 0 );
            ptr = result;
            break;
        case ' ':
        case '\t':
            break;
        default:
            *ptr++ = *path;
            break;
        }
        ++path;
    }
}

unsigned FindFilePath( int exe, const char *name, char *result )
{
    struct stat     tmp;
    unsigned        len;

    if( stat( name, &tmp ) == 0 ) {
        strcpy( result, name );
        return( strlen( result ) + 1 );
    }
    if( exe ) {
        return( TryOnePath( getenv( "PATH" ), &tmp, name, result ) );
    } else {
        len = TryOnePath( getenv( "WD_PATH" ), &tmp, name, result );
        if( len != 0 ) return( len );
        len = TryOnePath( getenv( "HOME" ), &tmp, name, result );
        if( len != 0 ) return( len );
        return( TryOnePath( "/usr/watcom/wd", &tmp, name, result ) );
    }
    return( 0 );
}

trap_retval ReqRead_user_keyboard( void )
{
    read_user_keyboard_req      *acc;
    read_user_keyboard_ret      *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->key = 0;
    return( sizeof( *ret ) );
}

trap_retval ReqSplit_cmd( void )
{
    char                *cmd;
    char                *start;
    split_cmd_ret       *ret;
    unsigned            len;

    cmd = GetInPtr( sizeof( split_cmd_req ) );
    ret = GetOutPtr( 0 );
    ret->parm_start = 0;
    start = cmd;
    len = GetTotalSize() - sizeof( split_cmd_req );
    while( len != 0 ) {
        switch( *cmd ) {
        case '\0':
        case ' ':
        case '\t':
            ret->parm_start = 1;
            len = 0;
            continue;
        }
        ++cmd;
        --len;
    }
    ret->parm_start += cmd - start;
    ret->cmd_end = cmd - start;
    CONV_LE_16( ret->cmd_end );
    CONV_LE_16( ret->parm_start );
    return( sizeof( *ret ) );
}

trap_retval ReqGet_next_alias( void )
{
    get_next_alias_req  *acc;
    get_next_alias_ret  *ret;

    ret = GetOutPtr( 0 );
    ret->seg = 0;
    ret->alias = 0;
    acc = GetInPtr( 0 );
    return( sizeof( *ret ) );
}

trap_retval ReqFile_run_cmd( void )
{
    char                *src;
    file_run_cmd_ret    *ret;

    src = GetInPtr( sizeof( file_run_cmd_req ) );
    ret = GetOutPtr( 0 );
    return( sizeof( *ret ) );
}
