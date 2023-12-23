/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2023 The Open Watcom Contributors. All Rights Reserved.
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

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <windows.h>
#include "ntext.h"
#include "stdnt.h"
#include "globals.h"
#include "ntpath.h"


#ifdef _WIN64
#define TRPH2LH(th)     (HANDLE)((th)->handle.u._64[0])
#define LH2TRPH(th,lh)  (th)->handle.u._64[0]=lh
#else
#define TRPH2LH(th)     (HANDLE)((th)->handle.u._32[0])
#define LH2TRPH(th,lh)  (th)->handle.u._32[0]=(unsigned_32)lh;(th)->handle.u._32[1]=0
#endif

static const DWORD          local_seek_method[] = { FILE_BEGIN, FILE_CURRENT, FILE_END };

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
//    if( delay == 0 )
//        delay = 10000;
    Sleep( delay );
    /*
     * NYI: get user input
     */
    ret->key = ' ';
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( open )( void )
{
    HANDLE                  h;
    file_open_req           *acc;
    file_open_ret           *ret;
    char                    *buff;
    unsigned                mode;

    acc = GetInPtr( 0 );
    buff = GetInPtr( sizeof( *acc ) );

    ret = GetOutPtr( 0 );
    ret->err = 0;
    /*
     * GetMagicalFileHandle checks if a file name is of a special syntax.
     * We generate magical file names for all DLL's when they load,
     * since NT only gives a file handle to the DLL, not a file name.
     * We fake up a name, and remember the handle.  When the debugger
     * asks to open up a fake name, we reuse the handle
     */
    h = GetMagicalFileHandle( buff );
    if( h == NULL ) {
        DWORD   share_mode;
        DWORD   desired_access;
        DWORD   attr;
        DWORD   create_disp;

        mode = O_RDONLY;
        if( acc->mode & DIG_OPEN_WRITE ) {
            mode = O_WRONLY;
            if( acc->mode & DIG_OPEN_READ ) {
                mode = O_RDWR;
            }
        }
        __GetNTAccessAttr( mode & 0x7, &desired_access, &attr );
        __GetNTShareAttr( mode & 0x70, &share_mode );
        if( acc->mode & DIG_OPEN_CREATE ) {
            create_disp = CREATE_ALWAYS;
        } else {
            create_disp = OPEN_EXISTING;
        }
        h = CreateFile( buff, desired_access, share_mode, 0, create_disp, FILE_ATTRIBUTE_NORMAL, NULL );
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
    DWORD           rc;
    file_seek_req   *acc;
    file_seek_ret   *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    rc = SetFilePointer( TRPH2LH( acc ), acc->pos, NULL, local_seek_method[acc->mode] );
    if( rc == INVALID_SET_FILE_POINTER ) {
        ret->err = GetLastError();
    }
    ret->pos = rc;
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( write )( void )
{
    DWORD           bytes;
    file_write_req  *acc;
    file_write_ret  *ret;

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
    if( DebugeePid ) {
        /*
         * NYI: write to program screen
         */
        ret->len = GetTotalSizeIn() - sizeof( *acc );
    } else {
        if( WriteFile( GetStdHandle( STD_ERROR_HANDLE ), GetInPtr( sizeof( *acc ) ),
                        GetTotalSizeIn() - sizeof( *acc ), &bytes, NULL ) == 0 ) {
            ret->err = GetLastError();
        }
        ret->len = bytes;
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( read )( void )
{
    DWORD           bytes;
    file_read_req   *acc;
    file_read_ret   *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    if( ReadFile( TRPH2LH( acc ), GetOutPtr( sizeof( *ret ) ), acc->len, &bytes, NULL ) == 0 ) {
        ret->err = GetLastError();
        return( sizeof( *ret ) );
    }
    return( sizeof( *ret ) + bytes );
}

trap_retval TRAP_FILE( close )( void )
{
    file_close_req  *acc;
    file_close_ret  *ret;
    HANDLE          h;

    acc = GetInPtr( 0 );
    h = TRPH2LH( acc );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    /*
     * we do not close the file handle if it was a magical one that
     * we remembered from a DLL load
     */
    if( !IsMagicalFileHandle( h ) ) {
        if( CloseHandle( h ) == 0 ) {
            ret->err = GetLastError();
        }
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( erase )( void )
{
    file_erase_ret  *ret;

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

    /*
     * NYI: to do
     */
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

    acc = GetInPtr( 0 );
    name = GetInPtr( sizeof( *acc ) );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    fullname = GetOutPtr( sizeof( *ret ) );

    if( GetMagicalFileHandle( name ) != NULL ) {
        strcpy( fullname, name );
    } else if( FindFilePath( acc->file_type, name, fullname ) == 0 ) {
        ret->err = ENOENT;
    }
    return( sizeof( *ret ) + strlen( fullname ) + 1 );
}
