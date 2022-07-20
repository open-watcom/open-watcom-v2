/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  File access functions for debugger trap files for Linux.
*
****************************************************************************/


#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "trpimp.h"
#include "trpcomm.h"
#include "lnxcomm.h"


#define TRPH2LH(th)     (th)->handle.u._32[0]
#define LH2TRPH(th,lh)  (th)->handle.u._32[0]=lh;(th)->handle.u._32[1]=0

static const int        local_seek_method[] = { SEEK_SET, SEEK_CUR, SEEK_END };

trap_retval TRAP_FILE( get_config )( void )
{
    file_get_config_ret *ret;

    ret = GetOutPtr( 0 );
    ret->file.ext_separator = '.';
    ret->file.drv_separator = '\0';
    ret->file.path_separator[0] = '/';
    ret->file.path_separator[1] = '\0';
    ret->file.line_eol[0] = '\n';
    ret->file.line_eol[1] = '\0';
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( open )( void )
{
    file_open_req       *acc;
    file_open_ret       *ret;
    int                 handle;
    int                 mode;
    int                 access;
    const char          *name;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    name = GetInPtr( sizeof( *acc ) );
    mode = O_RDONLY;
    if( acc->mode & DIG_OPEN_WRITE ) {
        mode = O_WRONLY;
        if( acc->mode & DIG_OPEN_READ ) {
            mode = O_RDWR;
        }
    }
    access = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    if( acc->mode & DIG_OPEN_CREATE ) {
        mode |= O_CREAT | O_TRUNC;
        if( acc->mode & DIG_OPEN_TRUNC ) {
            access |= S_IXUSR | S_IXGRP | S_IXOTH;
        }
    }
    handle = open( name, mode, access );
    if( handle == -1 ) {
        ret->err = errno;
        handle = 0;
    } else {
        fcntl( handle, F_SETFD, FD_CLOEXEC );
    }
    LH2TRPH( ret, handle );
    CONV_LE_32( ret->err );
    CONV_LE_64( ret->handle );
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( seek )( void )
{
    file_seek_req       *acc;
    file_seek_ret       *ret;

    acc = GetInPtr( 0 );
    CONV_LE_64( acc->handle );
    CONV_LE_32( acc->pos );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    ret->pos = lseek( TRPH2LH( acc ), acc->pos, local_seek_method[acc->mode] );
    if( ret->pos == ((off_t)-1) ) {
        ret->err = errno;
    }
    CONV_LE_32( ret->pos );
    CONV_LE_32( ret->err );
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( read )( void )
{
    size_t              total;
    size_t              len;
    char                *ptr;
    size_t              size;
    ssize_t             rv;
    file_read_req       *acc;
    file_read_ret       *ret;

    acc = GetInPtr( 0 );
    CONV_LE_64( acc->handle );
    CONV_LE_16( acc->len );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    ptr = GetOutPtr( sizeof( *ret ) );
    len = acc->len;
    size = SHRT_MAX;
    total = 0;
    while( len > 0 ) {
        if( size > len )
            size = len;
        rv = read( TRPH2LH( acc ), ptr, size );
        if( rv == -1 ) {
            ret->err = errno;
            CONV_LE_32( ret->err );
            return( sizeof( *ret ) );
        }
        total += rv;
        if( rv != size )
            break;
        ptr += rv;
        len -= rv;
    }
    CONV_LE_32( ret->err );
    return( sizeof( *ret ) + total );
}

static size_t DoWrite( int hdl, unsigned_8 *ptr, size_t len )
{
    size_t      total;
    size_t      size;
    ssize_t     rv;

    total = 0;
    size = SHRT_MAX;
    while( len > 0 ) {
        if( size > len )
            size = len;
        rv = write( hdl, ptr, size );
        if( rv == -1 ) {
            total = -1;
            break;
        }
        total += rv;
        ptr += rv;
        len -= rv;
    }
    return( total );
}

trap_retval TRAP_FILE( write )( void )
{
    file_write_req      *acc;
    file_write_ret      *ret;
    size_t              len;

    acc = GetInPtr( 0 );
    CONV_LE_64( acc->handle );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    len = DoWrite( TRPH2LH( acc ), GetInPtr( sizeof( *acc ) ), GetTotalSizeIn() - sizeof( *acc ) );
    if( len == -1 ) {
        ret->err = errno;
    }
    ret->len = len;
    CONV_LE_32( ret->err );
    CONV_LE_16( ret->len );
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( write_console )( void )
{
    file_write_console_ret  *ret;
    size_t                  len;

    ret = GetOutPtr( 0 );
    ret->err = 0;
    len = DoWrite( 2, GetInPtr( sizeof( file_write_console_req ) ), GetTotalSizeIn() - sizeof( file_write_console_req ) );
    if( len == -1 ) {
        ret->err = errno;
    }
    ret->len = len;
    CONV_LE_32( ret->err );
    CONV_LE_16( ret->len );
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( close )( void )
{
    file_close_req      *acc;
    file_close_ret      *ret;

    acc = GetInPtr( 0 );
    CONV_LE_64( acc->handle );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    if( close( TRPH2LH( acc ) ) == -1 ) {
        ret->err = errno;
    }
    CONV_LE_32( ret->err );
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( erase )( void )
{
    file_erase_ret      *ret;

    ret = GetOutPtr( 0 );
    ret->err = 0;
    if( unlink( GetInPtr( sizeof( file_erase_req ) ) ) ) {
        ret->err = errno;
    }
    CONV_LE_32( ret->err );
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( run_cmd )( void )
{
    char                buff[256];
    const char          *argv[4];
    char                *shell;
    pid_t               pid;
    int                 status;
    file_run_cmd_ret    *ret;
    size_t              len;


    shell = getenv( "SHELL" );
    if( shell == NULL )
        shell = "/bin/sh";
    ret = GetOutPtr( 0 );
    len = GetTotalSizeIn() - sizeof( file_run_cmd_req );
    argv[0] = shell;
    if( len > 0 ) {
        argv[1] = "-c";
        memcpy( buff, GetInPtr( sizeof( file_run_cmd_req ) ), len );
        buff[len] = '\0';
        argv[2] = buff;
        argv[3] = NULL;
    } else {
        argv[1] = NULL;
    }
    if ( (pid = fork()) == 0 ) { /* child */
        pid_t   pgrp;

        setpgid( 0, 0 );
        pgrp = getpgrp();
        tcsetpgrp( 0, pgrp );
        tcsetpgrp( 1, pgrp );
        tcsetpgrp( 2, pgrp );
        execve( shell, argv, (const char **)dbg_environ );
        exit( 1 );
    }
    /* parent */
    if( pid == -1 ) {
        ret->err = errno;
        CONV_LE_32( ret->err );
        return( sizeof( *ret ) );
    }
    waitpid( pid, &status, 0 );
    ret->err = WEXITSTATUS( status );
    CONV_LE_32( ret->err );
    return( sizeof( *ret ) );
}
