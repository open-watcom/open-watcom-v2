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
* Description:  QNX trap file support routines.
*
****************************************************************************/


#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <process.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <sys/proxy.h>
#include <sys/kernel.h>
#include <sys/debug.h>
#include <sys/stat.h>
#include <sys/proc_msg.h>
#include <sys/osinfo.h>
#include <sys/psinfo.h>
#include <sys/seginfo.h>
#include <sys/sched.h>
#include <sys/vc.h>
#include <sys/magic.h>
#include <sys/wait.h>
#include <sys/dumper.h>
#include <sys/console.h>
#include <sys/dev.h>
#include "trpimp.h"
#include "qnxcomm.h"


trap_retval ReqFile_get_config( void )
{
    file_get_config_ret *ret;

    ret = GetOutPtr( 0 );
    ret->file.ext_separator = '.';
    ret->file.drv_separator = '\0';
    ret->file.path_separator[0] = '/';
    ret->file.path_separator[1] = '\0';
    ret->file.newline[ 0 ] = '\n';
    ret->file.newline[ 1 ] = '\0';
    return( sizeof( *ret ) );
}

trap_retval ReqFile_run_cmd( void )
{
    char         buff[256];
    char         *argv[4];
    char         *shell;
    pid_t        pid;
    int          status;
    file_run_cmd_ret    *ret;
    int          len;


    shell = getenv( "SHELL" );
    if( shell == NULL ) shell = "/bin/sh";
    ret = GetOutPtr( 0 );
    len = GetTotalSize() - sizeof( file_run_cmd_req );
    argv[0] = shell;
    if( len != 0 ) {
        argv[1] = "-c";
        memcpy( buff, GetInPtr( sizeof( file_run_cmd_req ) ), len );
        buff[len] = '\0';
        argv[2] = buff;
        argv[3] = NULL;
    } else {
        argv[1] = NULL;
    }
    pid = qnx_spawn( 0, 0, 0, -1, -1, _SPAWN_NEWPGRP | _SPAWN_TCSETPGRP,
                    shell, argv, dbg_environ, NULL, -1 );
    if( pid == -1 ) {
        ret->err = errno;
        return( sizeof( *ret ) );
    }
    waitpid( pid, &status, 0 );
    ret->err = WEXITSTATUS( status );
    return( sizeof( *ret ) );
}

trap_retval ReqFile_open( void )
{
    file_open_req       *acc;
    file_open_ret       *ret;
    int                 handle;
    static const int    MapAcc[] = { O_RDONLY, O_WRONLY, O_RDWR };
    int                 mode;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    mode = MapAcc[ (acc->mode & (TF_READ|TF_WRITE)) - 1];
    if( acc->mode & TF_CREATE ) mode |= O_CREAT | O_TRUNC;
    handle = open( (char *)GetInPtr( sizeof( *acc ) ), mode,
                    S_IRUSR|S_IWUSR | S_IRGRP|S_IWGRP | S_IROTH|S_IWOTH );
    if( handle != -1 ) {
        fcntl( handle, F_SETFD, (int)FD_CLOEXEC );
        errno = 0;
        ret->err = 0;
        ret->handle = handle;
    } else {
        ret->err = errno;
        ret->handle = 0;
    }
    return( sizeof( *ret ) );
}


trap_retval ReqFile_close( void )
{
    file_close_req      *acc;
    file_close_ret      *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    if( close( acc->handle ) != -1 ) {
        errno = 0;
        ret->err = 0;
    } else {
        ret->err = errno;
    }
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
    return( sizeof( *ret ) );
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
    ret = GetOutPtr( 0 );
    ret->len = DoWrite( acc->handle, GetInPtr( sizeof( *acc ) ),
                        GetTotalSize() - sizeof( *acc ) );
    ret->err = errno;
    return( sizeof( *ret ) );
}

trap_retval ReqFile_write_console( void )
{
    file_write_console_ret      *ret;

    ret = GetOutPtr( 0 );
    ret->len = DoWrite( 2, GetInPtr( sizeof( file_write_console_req ) ),
                        GetTotalSize() - sizeof( file_write_console_req ) );
    ret->err = errno;
    return( sizeof( *ret ) );
}

trap_retval ReqFile_read( void )
{
    unsigned     total;
    unsigned     len;
    char         *ptr;
    unsigned     curr;
    int          rv;
    file_read_req       *acc;
    file_read_ret       *ret;

    acc = GetInPtr( 0 );
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
    return( sizeof( *ret ) + total );
}
