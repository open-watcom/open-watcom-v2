/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2024      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  NT Spawn server for IDE.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <process.h>
#include <ctype.h>
#include <sys/types.h>
#include <direct.h>
#include <windows.h>


#ifdef DEBUG
    #define Say( x ) printf x; fflush( stdout );
#else
    #define Say( x )
#endif


#include "batpipe.h"

static  HANDLE      RedirRead;
static  HANDLE      NulHdl;
static  char        CmdProc[128];
static  DWORD       ProcId;
static  HANDLE      ProcHdl;
static  HANDLE      MemHdl;

static void exit_link( int rc )
{
    if( SemReadUp != NULL ) {
        CloseHandle( SemReadUp );
    }
    if( SemReadDone != NULL ) {
        CloseHandle( SemReadDone );
    }
    if( SemWritten != NULL ) {
        CloseHandle( SemWritten );
    }
    if( MemHdl != NULL ) {
        CloseHandle( MemHdl );
    }
    if( SharedMemPtr != NULL ) {
        UnmapViewOfFile( SharedMemPtr );
    }
    exit( rc );
}

static void RunCmd( char *cmd_name )
{
    char                cmd[TRANS_MAXLEN + 80];
    PROCESS_INFORMATION info;
    HANDLE              dup;
    STARTUPINFO         start;
    HANDLE              redir_write;

    if( !CreatePipe( &RedirRead, &redir_write, NULL, 0 ) ) {
        RedirRead = 0;
        ProcHdl = 0;
        ProcId = 0;
        return;
    }
    sprintf( cmd, "%s /C %s", CmdProc, cmd_name );
    memset( &start, 0, sizeof( start ) );
    start.cb = sizeof( start );
    // set ShowWindow default value for nCmdShow parameter
    start.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    start.wShowWindow = SW_HIDE;
    DuplicateHandle( GetCurrentProcess(), redir_write, GetCurrentProcess(), &dup,
                0, TRUE, DUPLICATE_SAME_ACCESS );
    start.hStdError  = dup;
    DuplicateHandle( GetCurrentProcess(), redir_write, GetCurrentProcess(), &dup,
                0, TRUE, DUPLICATE_SAME_ACCESS );
    start.hStdOutput = dup;
    start.hStdInput  = NulHdl;
    if( !CreateProcess( NULL, cmd, NULL, NULL, TRUE, CREATE_NEW_PROCESS_GROUP,
                        NULL, NULL, &start, &info ) ) {
        info.dwProcessId = 0;
    }
    CloseHandle( info.hThread );
    CloseHandle( start.hStdError );
    CloseHandle( start.hStdOutput );
    CloseHandle( redir_write );
    ProcId = info.dwProcessId;
    ProcHdl = info.hProcess;
}

static void SendStatus( batch_stat status )
{
    BatservWriteData( LNK_STATUS, &status, sizeof( status ) );
}

static void ProcessConnection( void )
{
    char                buff[TRANS_MAXLEN];
    DWORD               bytes_read;
    DWORD               rc;
    DWORD               status;
    unsigned            max;
    char                link_cmd;
    int                 rbytes;

    for( ;; ) {
        rbytes = BatservReadData( &link_cmd, buff, sizeof( buff ) );
        if( rbytes < 0 )
            break;
        buff[rbytes] = '\0';
        switch( link_cmd ) {
        case LNK_CWD:
            rc = 0;
            if( !SetCurrentDirectory( buff ) ) {
                rc = GetLastError();
            }
            SendStatus( rc );
            break;
        case LNK_RUN:
            RunCmd( buff );
            break;
        case LNK_QUERY:
            max = *(batch_len *)buff;
            if( max > sizeof( buff ) )
                max = sizeof( buff );
            if( PeekNamedPipe( RedirRead, buff, 0, NULL, &bytes_read, NULL )
              && bytes_read != 0 ) {
                if( max > bytes_read )
                    max = bytes_read;
                ReadFile( RedirRead, buff, max, &bytes_read, NULL );
                BatservWriteData( LNK_OUTPUT, buff, bytes_read );
            } else {
                if( WaitForSingleObject( ProcHdl, 0 ) == WAIT_TIMEOUT ) {
                    /* let someone else run */
                    Sleep( 1 );
                    BatservWriteCmd( LNK_NOP );
                } else {
                    GetExitCodeProcess( ProcHdl, &status );
                    CloseHandle( RedirRead );
                    SendStatus( status );
                    ProcId = 0;
                    CloseHandle( ProcHdl );
                }
            }
            break;
        case LNK_CANCEL:
//            GenerateConsoleCtrlEvent( CTRL_BREAK_EVENT, ProcId );
            GenerateConsoleCtrlEvent( CTRL_BREAK_EVENT, 0 );
            break;
        case LNK_ABORT:
            TerminateProcess( ProcHdl, 0 );
            break;
        case LNK_DONE:
            Say(( "LNK_DONE\n" ));
            return;
        case LNK_SHUTDOWN:
            Say(( "LNK_SHUTDOWN\n" ));
            exit_link( 0 );
            break;
        }
    }
}

static BOOL CALLBACK HideWindows( HWND hwnd, LPARAM lp )
{
    DWORD       pid;

    lp=lp;
    GetWindowThreadProcessId( hwnd, &pid );
    if( pid == GetCurrentProcessId() ) {
        ShowWindow( hwnd, SW_HIDE );
        return( FALSE );
    }
    return( TRUE );
}

static BOOL WINAPI Ignore( DWORD CtrlType )
{
    CtrlType = CtrlType;
    return( TRUE );
}


void main( int argc, char *argv[] )
{
    SECURITY_ATTRIBUTES attr;

    /* unused parameters */ (void)argc; (void)argv;

    SemReadUp = CreateSemaphore( NULL, 0, 1, READUP_NAME );
    SemReadDone = CreateSemaphore( NULL, 0, 1, READDONE_NAME );
    SemWritten = CreateSemaphore( NULL, 0, 1, WRITTEN_NAME );
    if( argc > 1 && (argv[1][0] == 'q' || argv[1][0] == 'Q') ) {
        MemHdl = OpenFileMapping( FILE_MAP_WRITE, FALSE, DEFAULT_LINK_NAME );
        if( MemHdl == NULL ) {
            Say(( "can not connect to batcher spawn server\n" ));
        } else {
            SharedMemPtr = MapViewOfFile( MemHdl, FILE_MAP_WRITE, 0, 0, 0 );
            Say(( "LNK_SHUTDOWN\n" ));
            BatservWriteCmd( LNK_SHUTDOWN );
        }
        exit_link( 0 );
    }
    MemHdl = CreateFileMapping( INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 1024, DEFAULT_LINK_NAME );
    SharedMemPtr = MapViewOfFile( MemHdl, FILE_MAP_WRITE, 0, 0, 0 );
    /*
     * there was used getenv C function, but it looks like some versions of Microsoft
     * VS C run-time library has bug that getenv (maybe in some situation only) return invalid pointer and
     * when it is accessed fail and report access violation.
     * I change it to Windows API GetEnvironmentVariable to fix this problem and don't depend on
     * getenv from MS C run-time library anymore
     */
    GetEnvironmentVariable( "ComSpec", CmdProc, sizeof( CmdProc ) );
    if( *CmdProc == '\0' ) {
        fprintf( stderr, "Unable to find command processor\n" );
        exit_link( 1 );
    }
    SetConsoleCtrlHandler( NULL, FALSE );
    SetConsoleCtrlHandler( Ignore, TRUE );
    //NYI: need to accept name for link pipe
    attr.nLength = sizeof( attr );
    attr.lpSecurityDescriptor = NULL;
    attr.bInheritHandle = TRUE;
    NulHdl = CreateFile( "NUL", GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, &attr, OPEN_EXISTING, 0, NULL );
    if( NulHdl == INVALID_HANDLE_VALUE ) {
        fprintf( stderr, "Unable to open NUL device\n" );
        exit_link( 1 );
    }
    EnumWindows( HideWindows , 0 );

    Say(( "LNK_UP\n" ));
    for( ;; ) {
        ProcessConnection();
    }
}
