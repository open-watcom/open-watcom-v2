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
#include "ntext.h"
#include "msjerr.h"


#ifdef _WIN64
#define TRPH2LH(th)     (HANDLE)((th)->handle.u._64[0])
#define LH2TRPH(th,lh)  (th)->handle.u._64[0]=lh
#else
#define TRPH2LH(th)     (HANDLE)((th)->handle.u._32[0])
#define LH2TRPH(th,lh)  (th)->handle.u._32[0]=(unsigned_32)lh;(th)->handle.u._32[1]=0
#endif

#define OP_TRUNC        0x08

static const DWORD      local_seek_method[] = { FILE_BEGIN, FILE_CURRENT, FILE_END };

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
    read_user_keyboard_req      *acc;
    read_user_keyboard_ret      *ret;
    DWORD                       delay;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    delay = acc->wait * 1000;
//    if( delay == 0 )
//        delay = 10000;
    Sleep( delay );
    ret->key = ' ';
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( open )( void )
{
    HANDLE              h;
    file_open_req       *acc;
    file_open_ret       *ret;
    void                *buff;
    unsigned            mode;

    ret = GetOutPtr( 0 );
    ret->err = 0;
    acc = GetInPtr( 0 );
    buff = GetInPtr( sizeof(*acc) );

    h = FakeOpen( buff );
    if( h == INVALID_HANDLE_VALUE ) {
        DWORD   share_mode,desired_access,attr;
        DWORD   create_disp;

        mode = O_RDONLY;
        if( acc->mode & DIG_OPEN_WRITE ) {
            mode = O_WRONLY;
            if( acc->mode & DIG_OPEN_READ ) {
                mode = O_RDWR;
            }
        }
        /*
         * these __GetNT... routines are in the C library.  they turn
         * DOS style access and share bits into NT style ones
         */
        __GetNTAccessAttr( mode & 0x7, &desired_access, &attr );
        __GetNTShareAttr( mode & 0x70, &share_mode );
        if( acc->mode & DIG_OPEN_CREATE ) {
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
    LH2TRPH( ret, h );
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( seek )( void )
{
    DWORD               rc;
    file_seek_req       *acc;
    file_seek_ret       *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    rc = SetFilePointer( TRPH2LH( acc ), acc->pos, NULL, local_seek_method[acc->mode] );
    ret->pos = rc;
    if( rc == INVALID_SET_FILE_POINTER ) {
        ret->err = GetLastError();
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( write )( void )
{
    DWORD               bytes;
    file_write_req      *acc;
    file_write_ret      *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    if( WriteFile( TRPH2LH( acc ), GetInPtr( sizeof( *acc ) ),
            GetTotalSizeIn() - sizeof( *acc ), &bytes, NULL ) == 0 ) {
        ret->err = GetLastError();
    }
    ret->len = bytes;
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( write_console )( void )
{
    DWORD                   bytes;
    file_write_console_req  *acc;
    file_write_console_ret  *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    if( WriteFile( GetStdHandle( STD_ERROR_HANDLE ), GetInPtr( sizeof( *acc ) ),
                        GetTotalSizeIn() - sizeof( *acc ), &bytes, NULL ) ) {
        ret->err = GetLastError();
    }
    ret->len = bytes;
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( read )( void )
{
    DWORD               bytes;
    BOOL                rc;
    file_read_req       *acc;
    file_read_ret       *ret;
    void                *buff;
    HANDLE              h;

    acc = GetInPtr( 0 );
    h = TRPH2LH( acc );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    buff = GetOutPtr( sizeof( *ret ) );
    if( FakeRead( h, buff, acc->len, &bytes ) == 0 ) {
        if( ReadFile( h, buff, acc->len, &bytes, NULL ) == 0 ) {
            ret->err = GetLastError();
            return( sizeof( *ret ) );
        }
    }
    return( sizeof( *ret ) + bytes );
}

trap_retval TRAP_FILE( close )( void )
{
    file_close_req      *acc;
    file_close_ret      *ret;
    BOOL                rc;
    HANDLE              h;

    acc = GetInPtr( 0 );
    h = TRPH2LH( acc );
    ret = GetOutPtr( 0 );
    ret->err = 0;

    if( h != FakeHandle ) {
        rc = CloseHandle( h );
        if( !rc ) {
            ret->err = GetLastError();
        }
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( erase )( void )
{
    file_erase_ret      *ret;

    ret = GetOutPtr( 0 );
    ret->err = 0;
    if( DeleteFile( GetInPtr( sizeof( file_erase_req ) ) ) ) {
        ret->err = GetLastError();
    }
    return( sizeof( *ret ) );

}

trap_retval TRAP_FILE( run_cmd )( void )
{
    file_run_cmd_ret    *ret;

    ret = GetOutPtr( 0 );
    ret->err = 0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( file_to_fullpath )( void )
{
    file_string_to_fullpath_req *acc;
    file_string_to_fullpath_ret *ret;
    char                *name;
    char                *fullname;

    acc = GetInPtr( 0 );
    name = GetInPtr( sizeof( *acc ) );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    fullname = GetOutPtr( sizeof( *ret ) );
    if( acc->file_type == DIG_FILETYPE_EXE ) {
        strcat( fullname, name );
    } else {
        _searchenv( name, "PATH", fullname );
    }
    return( sizeof( *ret ) + strlen( fullname ) + 1 );
}


trap_retval TRAP_ENV( set_var )( void )
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
    if( value[0] == '\0' )
        value = NULL;
    if( !SetEnvironmentVariable( var, value ) ) {
        ret->err = GetLastError();
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_ENV( get_var )( void )
{
    env_get_var_req     *req;
    env_get_var_ret     *ret;
    char                *var;
    char                *value;

    req = GetInPtr( 0 );
    var = GetInPtr( sizeof( *req ) );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    value = GetOutPtr( sizeof( *ret ) );
    if( GetEnvironmentVariable( var, value, req->res_len ) == 0 ) {
        ret->err = GetLastError();
        return( sizeof( *ret ) );
    }
    return( sizeof( *ret ) + strlen( value ) + 1 );
}


trap_retval TRAP_FILE_INFO( get_date )( void )
{
    file_info_get_date_req      *req;
    file_info_get_date_ret      *ret;
    static WIN32_FIND_DATA      ffd;
    char                *name;
    HANDLE              h;
    WORD                md,mt;

    req = GetInPtr( 0 );
    name = GetInPtr( sizeof( *req ) );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    h = FindFirstFile( name, &ffd );
    if( h == INVALID_HANDLE_VALUE ) {
        ret->err = ERROR_FILE_NOT_FOUND;
        return( sizeof( *ret ) );
    }
    FindClose( h );
    FileTimeToDosDateTime( &ffd.ftLastWriteTime, &md, &mt );
    ret->date = ( md << 16 ) + mt;
    return( sizeof( *ret ) );
}


trap_retval TRAP_FILE_INFO( set_date )( void )
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
    ret->err = 0;
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
    return( sizeof( *ret ) );
}
