/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Windows NT specific functions for builder.
*
****************************************************************************/


#include <sys/types.h>
#include <direct.h>
#include <string.h>
#include <ctype.h>
#include <process.h>
#include <fcntl.h>
#include <io.h>
#include "builder.h"
#include <windows.h>

#include "clibext.h"


#define TITLESIZE 256
#define BUFSIZE   256

char    Title[TITLESIZE];

static void SysInitTitle( int argc, char *argv[] )
{
    int     i;
    size_t  len;

    strcpy( Title, "Builder " );
    for( i = 1; i < argc; i++ ) {
        if( strlen( Title ) + strlen( argv[i] ) > TITLESIZE - 3 )
            break;
        strcat( Title, argv[i] );
        strcat( Title, " " );
    }
    strcat( Title, "[" );
    len = strlen( Title );
    if( getcwd( Title + len, (int)( TITLESIZE - len - 2 ) ) == NULL )
        Title[len] = '\0';
    strcat( Title, "]" );
    SetConsoleTitle( Title );
}

static void SysSetTitle( char *title )
{
    char    *end;
    size_t  len;

    title = title;
    end = strchr( Title, ']' );
    *( end + 1 ) = '\0';

    strcat( Title, " (" );
    len = strlen( Title );
    if( getcwd( Title + len, (int)( TITLESIZE - len - 2 ) ) == NULL )
        Title[len] = '\0';
    strcat( Title, ")" );
    SetConsoleTitle( Title );
}

// CreateProcessA does not return process id as result
// but fills pinfo on success
// return value
//    == 0 means that child process was not created
//    != 0 means success
// save pinfo for closing child

static DWORD RunChildProcessCmdl( const char *cmdl, LPPROCESS_INFORMATION pinfo, LPHANDLE pipe_input )
{
    HANDLE              cp;
    HANDLE              parent_std_output;
    HANDLE              parent_std_error;
    HANDLE              std_output;
    HANDLE              std_error;
    HANDLE              pipe_output;
    SECURITY_ATTRIBUTES sa;
    DWORD               rc;
    STARTUPINFO         sinfo;

    parent_std_output = GetStdHandle( STD_OUTPUT_HANDLE );
    parent_std_error = GetStdHandle( STD_ERROR_HANDLE );
    sa.nLength = sizeof( sa );
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
    if( !CreatePipe( pipe_input, &pipe_output, &sa, 0 ) ) {
        return( GetLastError() );
    }
    cp = GetCurrentProcess();
    if( DuplicateHandle( cp, pipe_output, cp, &std_output, 0, TRUE, DUPLICATE_SAME_ACCESS ) == 0 ) {
        rc = GetLastError();
    } else {
        if( DuplicateHandle( cp, pipe_output, cp, &std_error, 0, TRUE, DUPLICATE_SAME_ACCESS ) == 0 ) {
            rc = GetLastError();
        } else {
            SetStdHandle( STD_OUTPUT_HANDLE, std_output );
            SetStdHandle( STD_ERROR_HANDLE, std_error );
            memset( &sinfo, 0, sizeof( sinfo ) );
            sinfo.cb = sizeof( sinfo );
            rc = 0;
            if( !CreateProcess( NULL, (LPSTR)cmdl, NULL, NULL, TRUE, 0, NULL, NULL, &sinfo, pinfo ) ) {
                rc = GetLastError();
            }
            SetStdHandle( STD_OUTPUT_HANDLE, parent_std_output );
            SetStdHandle( STD_ERROR_HANDLE, parent_std_error );
            CloseHandle( std_error );
        }
        CloseHandle( std_output );
    }
    CloseHandle( pipe_output );
    return( rc );
}

void SysInit( int argc, char *argv[] )
{
    SysInitTitle( argc, argv );
    setenv( "BLD_HOST", "NT", 1 );
}

int SysChdir( const char *dir )
{
    size_t      len;
    int         drive;
    int         rc;

    rc = 0;
    if( dir[0] != '\0' ) {
        drive = ( dir[1] == ':' ) ? toupper( (unsigned char)dir[0] ) - 'A' + 1 : 0;
        if( dir[1] != '\0' ) {
            len = strlen( dir );
            if( ( dir[len - 1] == '\\' || dir[len - 1] == '/' ) && ( len > 3 || drive == 0 ) ) {
                len--;
                memcpy( tmp_buf, dir, len );
                tmp_buf[len] = '\0';
                dir = tmp_buf;
            }
        }
        if( drive ) {
            _chdrive( drive );
        }
        rc = chdir( dir );
    }
    SysSetTitle( Title );
    return( rc );
}

static void convert_buffer( char *src, size_t len )
{
    char    *dst;
    char    c;

    dst = src;
    while( len-- > 0 ) {
        if( (c = *src++) != '\r' ) {
            *dst++ = c;
        }
    }
    *dst = '\0';
}

int SysRunCommand( const char *cmd )
{
    DWORD               rc;
    DWORD               bytes_read;
    char                buff[BUFSIZE + 1];
    HANDLE              pipe_input;
    PROCESS_INFORMATION pinfo;

    pipe_input = INVALID_HANDLE_VALUE;
    memset( &pinfo, 0, sizeof( pinfo ) );
    rc = RunChildProcessCmdl( cmd, &pinfo, &pipe_input );
    if( rc != 0 ) {
        if( pipe_input != INVALID_HANDLE_VALUE ) {
            CloseHandle( pipe_input );
        }
        return( -1 );
    }
    if( pipe_input != INVALID_HANDLE_VALUE ) {
        for( ;; ) {
            if( ReadFile( pipe_input, buff, sizeof( buff ) - 1, &bytes_read, NULL ) == 0 || bytes_read == 0 )
                break;
            convert_buffer( buff, bytes_read );
            Log( Quiet, "%s", buff );
        }
        CloseHandle( pipe_input );
    }
    WaitForSingleObject( pinfo.hProcess, INFINITE );
    GetExitCodeProcess( pinfo.hProcess, &rc );
    CloseHandle( pinfo.hProcess );
    CloseHandle( pinfo.hThread );
    return( (int)rc );
}
