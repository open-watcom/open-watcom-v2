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


#include <windows.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "trpimp.h"
#include "packet.h"
#include "control.h"
#include "msjerr.h"

#define OP_TRUNC        0x08

trap_retval ReqFile_get_config( void )
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

trap_retval ReqRead_user_keyboard( void )
{
    read_user_keyboard_req      *acc;
    read_user_keyboard_ret      *ret;
    DWORD                       delay;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    delay = acc->wait * 1000;
//  if( delay == 0 ) delay = 10000;
    Sleep( delay );
    ret->key = ' ';
    return( sizeof( *ret ) );
}

trap_retval ReqFile_open( void )
{
    HANDLE              h;
    file_open_req       *acc;
    file_open_ret       *ret;
    void                *buff;
    unsigned            mode;
    static int          mapAcc[] = { 0, 1, 2 };

    ret = GetOutPtr( 0 );
    acc = GetInPtr( 0 );
    buff = GetInPtr( sizeof(*acc) );


    ret->err = 0;
    h = FakeOpen( buff );
    if( h == INVALID_HANDLE_VALUE ) {
        /*
         * these __GetNT... routines are in the C library.  they turn
         * DOS style access and share bits into NT style ones
         */
        extern void __GetNTAccessAttr( int rwmode, LPDWORD desired_access,
                                        LPDWORD attr );
        extern void __GetNTShareAttr( int share, LPDWORD share_mode );
        DWORD   share_mode,desired_access,attr;
        DWORD   create_disp;

        mode = mapAcc[ (0x3 & acc->mode) -1 ];
        __GetNTAccessAttr( mode & 0x7, &desired_access, &attr );
        __GetNTShareAttr( mode & 0x70, &share_mode );
        if( acc->mode & TF_CREATE ) {
            create_disp = CREATE_ALWAYS;
        } else {
            create_disp = OPEN_EXISTING;
        }
        h = CreateFile( (LPTSTR)buff, desired_access, share_mode, 0, create_disp, FILE_ATTRIBUTE_NORMAL, NULL );
        if( h == INVALID_HANDLE_VALUE ) {
            ret->err = GetLastError();
            h = 0;
        }

    }
    ret->handle = (DWORD) h;
    return( sizeof( *ret ) );
}

trap_retval ReqFile_seek( void )
{
    DWORD               rc;
    file_seek_req       *acc;
    file_seek_ret       *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    rc = SetFilePointer( (HANDLE) acc->handle, acc->pos, NULL, acc->mode );
    if( rc == INVALID_SET_FILE_POINTER ) {
        ret->err = GetLastError();
    } else {
        ret->err = 0;
    }
    ret->pos = rc;
    return( sizeof( *ret ) );
}

trap_retval ReqFile_write( void )
{
    DWORD               bytes;
    BOOL                rc;
    file_write_req      *acc;
    file_write_ret      *ret;
    int                 len;
    void                *buff;

    acc = GetInPtr( 0 );
    buff = GetInPtr( sizeof( *acc ) );
    ret = GetOutPtr( 0 );

    len = GetTotalSize() - sizeof( *acc );

    rc = WriteFile( (HANDLE) acc->handle, buff, len, &bytes, NULL );
    if( !rc ) {
        ret->err = GetLastError();
        bytes = 0;
    } else {
        ret->err = 0;
    }
    ret->len = bytes;
    return( sizeof( *ret ) );
}

trap_retval ReqFile_write_console( void )
{
    DWORD               bytes;
    BOOL                rc;
    file_write_console_req      *acc;
    file_write_console_ret      *ret;
    int                 len;
    void                *buff;
    HANDLE              handle;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    buff = GetInPtr( sizeof( *acc ) );
    len = GetTotalSize() - sizeof( *acc );

    handle = GetStdHandle( STD_ERROR_HANDLE );
    rc = WriteFile( handle, buff, len, &bytes, NULL );
    if( !rc ) {
        ret->err = GetLastError();
        bytes = 0;
    } else {
        ret->err = 0;
    }
    ret->len = bytes;
    return( sizeof( *ret ) );
}

trap_retval ReqFile_read( void )
{
    DWORD               bytes;
    BOOL                rc;
    file_read_req       *acc;
    file_read_ret       *ret;
    void                *buff;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    buff = GetOutPtr( sizeof( *ret ) );

    if( FakeRead( (HANDLE) acc->handle, buff, acc->len, (unsigned*)&bytes ) ) {
        ret->err = 0;
        return( sizeof( *ret ) + bytes );
    }
    rc = ReadFile( (HANDLE) acc->handle, buff, acc->len, &bytes, NULL );
    if( !rc ) {
        ret->err = GetLastError();
        bytes = 0;
    } else {
        ret->err = 0;
    }
    return( sizeof( *ret ) + bytes );
}

trap_retval ReqFile_close( void )
{
    file_close_req      *acc;
    file_close_ret      *ret;
    BOOL                rc;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    ret->err = 0;
    if( (HANDLE)acc->handle != FakeHandle ) {
        rc = CloseHandle( (HANDLE) acc->handle );
        if( !rc ) {
            ret->err = GetLastError();
        }
    }
    return( sizeof( *ret ) );
}

trap_retval ReqFile_erase( void )
{
    file_erase_ret      *ret;
    char                *buff;

    buff = GetInPtr( sizeof( file_erase_req ) );
    ret = GetOutPtr( 0 );

    if( DeleteFile( buff ) ) {
        ret->err = GetLastError();
    } else {
        ret->err = 0;
    }
    return( sizeof( *ret ) );

}

trap_retval ReqFile_run_cmd( void )
{
    file_run_cmd_ret    *ret;

    ret = GetOutPtr( 0 );
    ret->err = 0;
    return( sizeof( *ret ) );
}

trap_retval ReqFile_string_to_fullpath( void )
{
    file_string_to_fullpath_req *acc;
    file_string_to_fullpath_ret *ret;
    char                *name;
    char                *fullname;

    acc = GetInPtr( 0 );
    name = GetInPtr( sizeof( *acc ) );
    ret = GetOutPtr( 0 );
    fullname = GetOutPtr( sizeof( *ret ) );
    if( acc->file_type == TF_TYPE_EXE ) {
        strcat( fullname, name );
    } else {
        _searchenv( name, "PATH", fullname );
    }
    ret->err = 0;
    return( sizeof( *ret ) + strlen( fullname ) + 1 );
}


trap_retval ReqEnv_setvar( void )
{
    env_set_var_req     *req;
    env_set_var_ret     *ret;
    char                *var;
    char                *value;

    req = GetInPtr( 0 );
    var = GetInPtr( sizeof( *req ) );
    value = GetInPtr( sizeof( *req ) + strlen( var ) + 1 );
    ret = GetOutPtr( 0 );

    ret->err = 0;
    if( value[0] == '\0' ) value = NULL;
    if( !SetEnvironmentVariable( var, value ) ) {
        ret->err = GetLastError();
    }
    return( sizeof( *ret ) );
}

trap_retval ReqEnv_getvar( void )
{
    env_get_var_req     *req;
    env_get_var_ret     *ret;
    char                *var;
    char                *value;

    req = GetInPtr( 0 );
    var = GetInPtr( sizeof( *req ) );
    ret = GetOutPtr( 0 );
    value = GetOutPtr( sizeof( *ret ) );
    ret->err = 0;
    if( GetEnvironmentVariable( var, value, req->res_len ) == 0 ) {
        ret->err = GetLastError();
        return( sizeof( *ret ) );
    } else {
        return( sizeof( *ret ) + strlen( value ) + 1 );
    }
}


trap_retval ReqFileInfo_getdate( void )
{
    file_info_get_date_req      *req;
    file_info_get_date_ret      *ret;
    static WIN32_FIND_DATA      ffb;
    char                *name;
    HANDLE              h;
    WORD                md,mt;

    req = GetInPtr( 0 );
    name = GetInPtr( sizeof( *req ) );
    ret = GetOutPtr( 0 );
    h = FindFirstFile( name, &ffb );
    if( h == INVALID_HANDLE_VALUE ) {
        ret->err = ERROR_FILE_NOT_FOUND;
        return( sizeof( *ret ) );
    }
    FindClose( h );
    FileTimeToDosDateTime( &ffb.ftLastWriteTime, &md, &mt );
    ret->err = 0;
    ret->date = ( md << 16 ) + mt;
    return( sizeof( *ret ) );
}


trap_retval ReqFileInfo_setdate( void )
{
    file_info_set_date_req      *req;
    file_info_set_date_ret      *ret;
    char                *name;
    HANDLE              h;
    WORD                md,mt;
    FILETIME            ft;

    req = GetInPtr( 0 );
    name = GetInPtr( sizeof( *req ) );
    ret = GetOutPtr( 0 );
    md = ( req->date >> 16 ) & 0xffff;
    mt = req->date;
    DosDateTimeToFileTime( md, mt, &ft );
    h = CreateFile( name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL );
    if( h == INVALID_HANDLE_VALUE ) {
        ret->err = GetLastError();
        return( sizeof( *ret ) );
    }
    if( !SetFileTime( h, &ft, &ft, &ft ) ) {
        ret->err = GetLastError();
        return( sizeof( *ret ) );
    }
    CloseHandle( h );
    ret->err = 0;
    return( sizeof( *ret ) );
}
