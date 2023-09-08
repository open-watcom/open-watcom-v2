/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  File supplementary trap functions for RDOS.
*
****************************************************************************/

#include <io.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "stdrdos.h"
#include "rdos.h"
#include "pathgrp2.h"


#define TRPH2LH(th)     (int)((th)->handle.u._32[0])
#define LH2TRPH(th,lh)  (th)->handle.u._32[0]=(unsigned_32)lh;(th)->handle.u._32[1]=0

trap_retval TRAP_FILE( get_config )( void )
{
    file_get_config_ret *ret;

    ret = GetOutPtr( 0 );

    ret->file.ext_separator = '.';
    ret->file.drv_separator = ':';
    ret->file.path_separator[0] = '\\';
    ret->file.path_separator[1] = '/';
    ret->file.line_eol[0] = '\r';
    ret->file.line_eol[1] = '\n';
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Read_user_keyboard )( void )
{
    read_user_keyboard_req  *acc;
    read_user_keyboard_ret  *ret;
    DWORD                   delay;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    delay = acc->wait * 1000;
    RdosWaitMilli( delay );

    ret->key = ' ';
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( open )( void )
{
    file_open_req           *acc;
    file_open_ret           *ret;
    int                     handle;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    handle = RdosOpenFile( GetInPtr( sizeof( *acc ) ), 0 );
    LH2TRPH( ret, handle );
    if( handle == 0 ) {
        ret->err = MSG_FILE_NOT_FOUND;
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( seek )( void )
{
    file_seek_req   *acc;
    file_seek_ret   *ret;
    long            pos;
    int             h;

    acc = GetInPtr( 0 );
    pos = acc->pos;
    h = TRPH2LH( acc );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    switch( acc->mode ) {
    case DIG_SEEK_ORG:
        RdosSetFilePos( h, pos );
        break;
    case DIG_SEEK_CUR:
        pos += RdosGetFilePos( h );
        RdosSetFilePos( h, pos );
        break;
    case DIG_SEEK_END:
        pos += RdosGetFileSize( h );
        RdosSetFilePos( h, pos );
        break;
    default:
        ret->err = MSG_FILE_MODE_ERROR;
        break;
    }
    ret->pos = pos;
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( write )( void )
{
    file_write_req  *acc;
    file_write_ret  *ret;
    size_t          len;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    len = GetTotalSizeIn() - sizeof( *acc );
    if( len > 0 ) {
        ret->len = RdosWriteFile( TRPH2LH( acc ), GetInPtr( sizeof( *acc ) ), len );
    } else {
        ret->len = 0;
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( write_console )( void )
{
    file_write_console_req  *acc;
    file_write_console_ret  *ret;
    size_t                  len;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    len = GetTotalSizeIn() - sizeof( *acc );
    RdosWriteSizeString( GetInPtr( sizeof( *acc ) ), len );
    ret->len = len;
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( read )( void )
{
    file_read_req   *acc;
    file_read_ret   *ret;
    int             bytes;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    bytes = RdosReadFile( TRPH2LH( acc ), GetOutPtr( sizeof( *ret ) ), acc->len );
    return( sizeof( *ret ) + bytes );
}

trap_retval TRAP_FILE( close )( void )
{
    file_close_req  *acc;
    file_close_ret  *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    RdosCloseFile( TRPH2LH( acc ) );
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( erase )( void )
{
    file_erase_ret  *ret;

    ret = GetOutPtr( 0 );
    ret->err = 0;
    if( RdosDeleteFile( GetInPtr( sizeof( file_erase_req ) ) ) == 0 ) {
        ret->err = MSG_FILE_NOT_FOUND;
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( run_cmd )( void )
{
    file_run_cmd_ret    *ret;

    //NYI: to do
    ret = GetOutPtr( 0 );
    ret->err = 0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( file_to_fullpath )( void )
{
    file_string_to_fullpath_req *acc;
    file_string_to_fullpath_ret *ret;
    char                        *name;
    char                        *fullname;
    pgroup2                     pg;

    acc = GetInPtr( 0 );
    name = GetInPtr( sizeof( *acc ) );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    fullname = GetOutPtr( sizeof( *ret ) );

    _splitpath2( name, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    _makepath( fullname, pg.drive, pg.dir, pg.fname, pg.ext );

    if( access( fullname, 0 ) != 0 ) {
        _makepath( fullname, pg.drive, pg.dir, pg.fname, "exe" );

        if( access( fullname, 0 ) != 0 ) {
            _makepath( fullname, pg.drive, pg.dir, pg.fname, "dll" );

            if( access( fullname, 0 ) != 0 ) {
                ret->err = MSG_FILE_NOT_FOUND;
                *fullname = 0;
            }
        }
    }

    return( sizeof( *ret ) + strlen( fullname ) + 1 );
}
