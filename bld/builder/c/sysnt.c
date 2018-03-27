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
* Description:  Windows NT specific functions for builder.
*
****************************************************************************/


#include <sys/types.h>
#include <direct.h>
#include <string.h>
#include <ctype.h>
#include <dos.h>
#include <process.h>
#include <fcntl.h>
#include <io.h>
#include "builder.h"
#include <windows.h>

#include "clibext.h"


#define TITLESIZE 256

char    Title[TITLESIZE];

static void SysInitTitle( int argc, char *argv[] )
{
    int i;

    strcpy( Title, "Builder " );
    for( i = 1; i < argc; i++ ) {
        if( strlen( Title ) + strlen( argv[i] ) > TITLESIZE - 3 )
            break;
        strcat( Title, argv[i] );
        strcat( Title, " " );
    }
    strcat( Title, "[" );
    getcwd( Title + strlen( Title ), (int)( TITLESIZE - strlen( Title ) - 2 ) );
    strcat( Title, "]" );
    SetConsoleTitle( Title );
}

static void SysSetTitle( char *title )
{
    char        *end;

    title = title;
    end = strchr( Title, ']' );
    *( end + 1 ) = '\0';

    strcat( Title, " (" );
    getcwd( Title + strlen( Title ), (int)( TITLESIZE - strlen( Title ) - 2 ) );
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

int SysChdir( char *dir )
{
    int     retval;

    retval = SysDosChdir( dir );
    SysSetTitle( Title );
    return( retval );
}

int SysRunCommand( const char *cmd )
{
    DWORD               rc;
    DWORD               bytes_read;
    char                buff[256 + 1];
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
            char    *dst;
            DWORD   i;

            ReadFile( pipe_input, buff, sizeof( buff ) - 1, &bytes_read, NULL );
            if( bytes_read == 0 )
                break;
            dst = buff;
            for( i = 0; i < bytes_read; ++i ) {
                if( buff[i] != '\r' ) {
                    *dst++ = buff[i];
                }
            }
            *dst = '\0';
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
