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


#include "debugme.h"
#include <string.h>     /* may only call functions defined in nlmstrt.c */
#include <fcntl.h>
#include <stdio.h>
#include <nwtypes.h>

#include "miniproc.h"
#include "loader.h"
#include "nwsemaph.h"

#undef TRUE
#undef FALSE

#include "trpimp.h"
#include "packet.h"

#include "nlmport.h"
#include "nw3to5.h"
#include "nlmio.h"


trap_retval ReqFile_get_config( void )
{
    file_get_config_ret *ret;

    ret = GetOutPtr( 0 );

    ret->file.ext_separator = '.';
    ret->file.drv_separator = ':';
    ret->file.path_separator[0] = '\\';
    ret->file.path_separator[1] = '/';
    ret->file.newline[0] = '\r';
    ret->file.newline[1] = '\n';
    return( sizeof( *ret ) );
}

trap_retval ReqFile_open( void )
{
    file_open_req       *acc;
    file_open_ret       *ret;
    int             retval;
    static int MapAcc[] = { O_RDONLY, O_WRONLY, O_RDWR };

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    if( acc->mode & TF_CREATE ) {
        retval = IOCreat( GetInPtr( sizeof( *acc ) ) );
    } else {
        retval = IOOpen( GetInPtr( sizeof( *acc ) ),
                         MapAcc[acc->mode - 1] );
    }
    if( retval < 0 ) {
        ret->err = retval;
        ret->handle = 0;
    } else {
        ret->err = 0;
        ret->handle = retval;
    }
    return( sizeof( *ret ) );
}

trap_retval ReqFile_close( void )
{
    file_close_req      *acc;
    file_close_ret      *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = IOClose( acc->handle );
    return( sizeof( *ret ) );
}

trap_retval ReqFile_write( void )
{
    int          retval;
    file_write_req      *acc;
    file_write_ret      *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    retval = IOWrite( acc->handle, GetInPtr( sizeof(*acc) ),
                      ( GetTotalSize() - sizeof( *acc ) ) );
    if( retval < 0 ) {
        ret->err = retval;
        ret->len = 0;
    } else {
        ret->err = 0;
        ret->len = retval;
    }
    return( sizeof( *ret ) );
}

trap_retval ReqFile_write_console( void )
{
    int          retval;
    file_write_console_req      *acc;
    file_write_console_ret      *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    retval = IOWriteConsole( GetInPtr( sizeof(*acc) ),
                             ( GetTotalSize() - sizeof( *acc ) ) );
    if( retval < 0 ) {
        ret->err = retval;
        ret->len = 0;
    } else {
        ret->err = 0;
        ret->len = retval;
    }
    return( sizeof( *ret ) );
}

trap_retval ReqFile_seek( void )
{
    file_seek_req       *acc;
    file_seek_ret       *ret;
    long            retval;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    retval = IOSeek( acc->handle, acc->mode, acc->pos );
    if( retval < 0 ) {
        ret->err = retval;
        ret->pos = 0;
    } else {
        ret->err = 0;
        ret->pos = retval;
    }
    return( sizeof( *ret ) );
}

trap_retval ReqFile_read( void )
{
    file_read_req       *acc;
    file_read_ret       *ret;
    int           retval;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    retval = IORead( acc->handle, GetOutPtr( sizeof( *ret ) ),
                     acc->len );
    if( retval < 0 ) {
        ret->err = retval;
        retval = 0;
    } else {
        ret->err = 0;
    }
    return( sizeof( *ret ) + retval );
}

trap_retval ReqFile_string_to_fullpath( void )
{
    char               *name;
    char               *fullname;
    file_string_to_fullpath_req *acc;
    file_string_to_fullpath_ret *ret;
    int                len;

    acc = GetInPtr( 0 );
    name = GetInPtr( sizeof( *acc ) );
    ret = GetOutPtr( 0 );
    fullname = GetOutPtr( sizeof( *ret ) );
    if( acc->file_type == TF_TYPE_EXE ) {
        StringToNLMPath( name, fullname );
    } else {
        strcpy( fullname, name );
    }
    len = strlen( fullname );
    ret->err = ( len == 0 ) ? 1 : 0;
    return( sizeof( *ret ) + len + 1 );
}

trap_retval ReqFile_erase( void )
{
    file_erase_ret      *ret;

    ret = GetOutPtr( 0 );
    ret->err = 1;       //NYI:
    return( sizeof( *ret ) );
}

trap_retval ReqFile_run_cmd( void )
{
    file_run_cmd_ret *ret;

    ret = GetOutPtr( 0 );
    ret->err = 0;  //NYI: No error?
    return( sizeof( *ret ) );
}
