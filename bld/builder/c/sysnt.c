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

static DWORD RunChildProcessCmdl( const char *cmdl, LPPROCESS_INFORMATION pinfo, LPHANDLE readpipe )
{
    HANDLE              cp;
    HANDLE              parent_std_output;
    HANDLE              parent_std_error;
    HANDLE              pipe_input;
    HANDLE              pipe_output;
    SECURITY_ATTRIBUTES sa;
    DWORD               rc;
    STARTUPINFO         sinfo;

    sa.nLength = sizeof( sa );
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
    if( !CreatePipe( readpipe, &pipe_output, &sa, 0 ) ) {
        return( GetLastError() );
    }
    cp = GetCurrentProcess();
    DuplicateHandle( cp, GetStdHandle( STD_OUTPUT_HANDLE ), cp, &parent_std_output, 0, TRUE, DUPLICATE_SAME_ACCESS );
    DuplicateHandle( cp, GetStdHandle( STD_ERROR_HANDLE ), cp, &parent_std_error, 0, TRUE, DUPLICATE_SAME_ACCESS );
    SetStdHandle( STD_OUTPUT_HANDLE, pipe_output );
    SetStdHandle( STD_ERROR_HANDLE, pipe_output );
    memset( &sinfo, 0, sizeof( sinfo ) );
    sinfo.cb = sizeof( sinfo );
    rc = 0;
    if( !CreateProcess( NULL, (LPSTR)cmdl, NULL, NULL, TRUE, 0, NULL, NULL, &sinfo, pinfo ) ) {
        rc = GetLastError();
    }
    CloseHandle( pipe_output );
    SetStdHandle( STD_OUTPUT_HANDLE, parent_std_output );
    SetStdHandle( STD_ERROR_HANDLE, parent_std_error );
    CloseHandle( parent_std_output );
    CloseHandle( parent_std_error );
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
    HANDLE              readpipe;
    PROCESS_INFORMATION pinfo;

    readpipe = INVALID_HANDLE_VALUE;
    memset( &pinfo, 0, sizeof( pinfo ) );
    rc = RunChildProcessCmdl( cmd, &pinfo, &readpipe );
    if( rc != 0 ) {
        if( readpipe != INVALID_HANDLE_VALUE ) {
            CloseHandle( readpipe );
        }
        return( -1 );
    }
    if( readpipe != INVALID_HANDLE_VALUE ) {
        for( ;; ) {
            char    *dst;
            DWORD   i;

            ReadFile( readpipe, buff, sizeof( buff ) - 1, &bytes_read, NULL );
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
        CloseHandle( readpipe );
    }
    WaitForSingleObject( pinfo.hProcess, INFINITE );
    GetExitCodeProcess( pinfo.hProcess, &rc );
    CloseHandle( pinfo.hProcess );
    CloseHandle( pinfo.hThread );
    return( (int)rc );
}
