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

trap_elen ReqFile_get_config( void )
{
    file_get_config_ret *ret;

    ret = GetOutPtr( 0 );

    ret->file.ext_separator = '.';
    ret->file.path_separator[0] = '\\';
    ret->file.path_separator[1] = '/';
    ret->file.path_separator[2] = ':';
    ret->file.newline[0] = '\r';
    ret->file.newline[1] = '\n';
    return( sizeof( *ret ) );
}

trap_elen ReqRead_user_keyboard( void )
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

trap_elen ReqFile_open( void )
{
    file_open_req           *acc;
    file_open_ret           *ret;
    void                    *buff;
    int                     handle;

    acc = GetInPtr( 0 );
    buff = GetInPtr( sizeof( *acc ) );

    ret = GetOutPtr( 0 );

    ret->err = 0;
    ret->handle = 0;

    handle = RdosOpenFile( buff, 0 );

    if( handle )
        ret->handle = handle;
    else
        ret->err = MSG_FILE_NOT_FOUND;

    return( sizeof( *ret ) );
}

trap_elen ReqFile_seek( void )
{
    file_seek_req   *acc;
    file_seek_ret   *ret;
    long            pos;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    pos = acc->pos;
    ret->err = 0;
    ret->pos = 0;
    switch( acc->mode ) {
    case 0:
        RdosSetFilePos( acc->handle, pos );
        ret->pos = pos;
        break;
    case 1:
        pos += RdosGetFilePos( acc->handle );
        RdosSetFilePos( acc->handle, pos );
        ret->pos = pos;
        break;
    case 2:
        pos += RdosGetFileSize( acc->handle );
        RdosSetFilePos( acc->handle, pos );
        ret->pos = pos;
        break;
    default:
        ret->err = MSG_FILE_MODE_ERROR;
        break;
    }
    return( sizeof( *ret ) );
}

trap_elen ReqFile_write( void )
{
    file_write_req  *acc;
    file_write_ret  *ret;
    int             len;
    void            *buff;

    acc = GetInPtr( 0 );
    buff = GetInPtr( sizeof( *acc ) );
    ret = GetOutPtr( 0 );

    len = GetTotalSize() - sizeof( *acc );

    ret->err = 0;

    if( len )
        ret->len = RdosWriteFile( acc->handle, buff, len );
    else
        ret->len = 0;

    return( sizeof( *ret ) );
}

trap_elen ReqFile_write_console( void )
{
    file_write_console_req  *acc;
    file_write_console_ret  *ret;
    int                     len;
    void                    *buff;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    buff = GetInPtr( sizeof( *acc ) );
    len = GetTotalSize() - sizeof( *acc );

    RdosWriteSizeString( buff, len );
    
    ret->err = 0;
    ret->len = len;

    return( sizeof( *ret ) );
}

trap_elen ReqFile_read( void )
{
    file_read_req   *acc;
    file_read_ret   *ret;
    void            *buff;
    int             bytes;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    buff = GetOutPtr( sizeof( *ret ) );

    bytes = RdosReadFile( acc->handle, buff, acc->len );
    ret->err = 0;

    return( sizeof( *ret ) + bytes );
}

trap_elen ReqFile_close( void )
{
    file_close_req  *acc;
    file_close_ret  *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    ret->err = 0;
    RdosCloseFile( acc->handle );
    
    return( sizeof( *ret ) );
}

trap_elen ReqFile_erase( void )
{
    file_erase_ret  *ret;
    char            *buff;

    buff = GetInPtr( sizeof( file_erase_req ) );
    ret = GetOutPtr( 0 );

    ret->err = 0;

    if( !RdosDeleteFile( buff ) )
        ret->err = MSG_FILE_NOT_FOUND;

    return( sizeof( *ret ) );

}

trap_elen ReqFile_run_cmd( void )
{
    file_run_cmd_ret    *ret;

    //NYI: to do
    ret = GetOutPtr( 0 );
    ret->err = 0;
    return( sizeof( *ret ) );
}

trap_elen ReqFile_string_to_fullpath( void )
{
    file_string_to_fullpath_req *acc;
    file_string_to_fullpath_ret *ret;
    char                        *name;
    char                        *fullname;
    char                        drive[10];
    char                        dir[256];
    char                        fname[100];
    char                        ext[10];

    acc = GetInPtr( 0 );
    name = GetInPtr( sizeof( *acc ) );
    ret = GetOutPtr( 0 );
    fullname = GetOutPtr( sizeof( *ret ) );

    _splitpath( name, drive, dir, fname, ext );
    _makepath( fullname, drive, dir, fname, ext );

    if( access( fullname, 0 ) == 0 )
        ret->err = 0;
    else {
        _makepath( fullname, drive, dir, fname, "exe" );

        if( access( fullname, 0 ) == 0 )
            ret->err = 0;
        else {
            _makepath( fullname, drive, dir, fname, "dll" );

            if( access( fullname, 0 ) == 0 )
                ret->err = 0;
            else {
                ret->err = MSG_FILE_NOT_FOUND;
                *fullname = 0;
            }
        }
    }
        
    return( sizeof( *ret ) + strlen( fullname ) + 1 );
}
