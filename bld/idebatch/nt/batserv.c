/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2024-2025 The Open Watcom Contributors. All Rights Reserved.
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
#include "batpipe.h"


#ifdef DEBUG
    #define Say( x ) printf x; fflush( stdout );
#else
    #define Say( x )
#endif

static HANDLE       RedirRead;
static HANDLE       NulDevHdl;
static char         CmdProc[COMSPEC_MAXLEN + 1];
static DWORD        ProcId;
static HANDLE       ProcHdl;

static void exit_link( int rc )
{
    BatservPipeClose();
    exit( rc );
}

static void RunCmd( const char *cmd_name )
{
    char                cmd[COMSPEC_MAXLEN + 4 + TRANS_DATA_MAXLEN + 1];
    PROCESS_INFORMATION info;
    HANDLE              dup;
    STARTUPINFO         start;
    HANDLE              redir_write;

    if( CreatePipe( &RedirRead, &redir_write, NULL, 0 ) == 0 ) {
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
    DuplicateHandle( GetCurrentProcess(), redir_write, GetCurrentProcess(), &dup, 0, TRUE, DUPLICATE_SAME_ACCESS );
    start.hStdError  = dup;
    DuplicateHandle( GetCurrentProcess(), redir_write, GetCurrentProcess(), &dup, 0, TRUE, DUPLICATE_SAME_ACCESS );
    start.hStdOutput = dup;
    start.hStdInput  = NulDevHdl;
    if( CreateProcess( NULL, cmd, NULL, NULL, TRUE, CREATE_NEW_PROCESS_GROUP, NULL, NULL, &start, &info ) == 0 ) {
        info.dwProcessId = 0;
    }
    CloseHandle( info.hThread );
    CloseHandle( start.hStdError );
    CloseHandle( start.hStdOutput );
    CloseHandle( redir_write );
    ProcId = info.dwProcessId;
    ProcHdl = info.hProcess;
}

static void ProcessConnection( void )
{
    DWORD               bytes_read;
    DWORD               rc;
    batch_stat          status;
    int                 len;

    for( ;; ) {
        len = BatservReadData();
        if( len < 0 )
            break;
        /*
         * add additional null terminate character
         * strings are transferred without null terminate character
         */
        bdata.u.s.u.data[len] = '\0';
        switch( bdata.u.s.cmd ) {
        case LNK_CWD:
            status = 0;
            if( SetCurrentDirectory( bdata.u.s.u.data ) == 0 ) {
                status = GetLastError();
            }
            BatservWriteData( LNK_STATUS, &status, sizeof( status ) );
            break;
        case LNK_RUN:
            RunCmd( bdata.u.s.u.data );
            break;
        case LNK_QUERY:
            len = bdata.u.s.u.len;
            if( PeekNamedPipe( RedirRead, bdata.u.buffer, 0, NULL, &bytes_read, NULL )
              && bytes_read != 0 ) {
                /*
                 * limit read length to maximum output length
                 */
                if( len > TRANS_DATA_MAXLEN )
                    len = TRANS_DATA_MAXLEN;
                ReadFile( RedirRead, bdata.u.buffer, len, &bytes_read, NULL );
                BatservWriteData( LNK_OUTPUT, bdata.u.buffer, bytes_read );
            } else if( WaitForSingleObject( ProcHdl, 0 ) == WAIT_TIMEOUT ) {
                /* let someone else run */
                Sleep( 1 );
                BatservWriteCmd( LNK_NOP );
            } else {
                GetExitCodeProcess( ProcHdl, &rc );
                CloseHandle( RedirRead );
                ProcId = 0;
                CloseHandle( ProcHdl );
                status = rc;
                BatservWriteData( LNK_STATUS, &status, sizeof( status ) );
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

    /* unused parameters */ (void)lp;

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

    if( argc > 1 && (argv[1][0] == 'q' || argv[1][0] == 'Q') ) {
        if( BatservPipeOpen( DEFAULT_LINK_NAME ) ) {
            Say(( "can not connect to batcher spawn server\n" ));
        } else {
            Say(( "LNK_SHUTDOWN\n" ));
            BatservWriteCmd( LNK_SHUTDOWN );
        }
        exit_link( 0 );
    }
    BatservPipeCreate( DEFAULT_LINK_NAME );
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
    NulDevHdl = CreateFile( "NUL", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, &attr, OPEN_EXISTING, 0, NULL );
    if( NulDevHdl == INVALID_HANDLE_VALUE ) {
        fprintf( stderr, "Unable to open NUL device\n" );
        exit_link( 1 );
    }
    EnumWindows( HideWindows, 0 );

    Say(( "LNK_UP\n" ));
    for( ;; ) {
        ProcessConnection();
    }
}
