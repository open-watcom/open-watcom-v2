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
* Description:  NT Spawn server for Viper (IDE).
*
****************************************************************************/


#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <process.h>
#include <ctype.h>
#include <sys/types.h>
#include <direct.h>

#ifdef DEBUG
    #define Say( x ) printf x; fflush( stdout );
#else
    #define Say( x )
#endif


#include "batpipe.h"

HANDLE          RedirRead;
HANDLE          NulHdl;
HANDLE          OverlapHdl;

char            *CmdProc;
DWORD           ProcId;
HANDLE          ProcHdl;
static  HANDLE  MemHdl;

static void RunCmd( char *cmd_name )
{
    char                cmd[MAX_TRANS+80];
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
    DuplicateHandle( GetCurrentProcess(), redir_write, GetCurrentProcess(), &dup,
                0, TRUE, DUPLICATE_SAME_ACCESS );
    start.hStdError  = dup;
    DuplicateHandle( GetCurrentProcess(), redir_write, GetCurrentProcess(), &dup,
                0, TRUE, DUPLICATE_SAME_ACCESS );
    start.hStdOutput = dup;
    start.hStdInput  = NulHdl;
    start.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    start.wShowWindow = SW_HIDE;
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

static void SendStatus( unsigned long status )
{
    struct {
        unsigned char   cmd;
        unsigned long   stat;
    } buff;

    buff.cmd = LNK_STATUS;
    buff.stat = status;
    BatservWrite( &buff, sizeof( buff ) );
}

static void ProcessConnection( void )
{
    char                buff[MAX_TRANS];
    DWORD               bytes_read;
    char                *dir;
    DWORD               rc;
    DWORD               status;
    unsigned            max;

    for( ;; ) {
        bytes_read = BatservRead( buff, sizeof( buff ) );
        if( bytes_read == 0 ) break;
        buff[bytes_read] = '\0';
        switch( buff[0] ) {
        case LNK_CWD:
            rc = 0;
            dir = &buff[1];
            if( !SetCurrentDirectory( dir ) ) {
                rc = GetLastError();
            }
            SendStatus( rc );
            break;
        case LNK_RUN:
            RunCmd( &buff[1] );
            break;
        case LNK_QUERY:
            max = *(unsigned long *)&buff[1];
            if( max > sizeof( buff ) ) max = sizeof( buff );
            --max;
            if( PeekNamedPipe( RedirRead, buff, 0, NULL, &bytes_read,
                        NULL ) && bytes_read != 0 ) {
                if( bytes_read < max ) max = bytes_read;
                ReadFile( RedirRead, &buff[1], max, &bytes_read, NULL );
                buff[0] = LNK_OUTPUT;
                BatservWrite( buff, bytes_read + 1 );
            } else {
                if( WaitForSingleObject( ProcHdl, 0 ) == WAIT_TIMEOUT ) {
                    /* let someone else run */
                    Sleep( 1 );
                    buff[0] = LNK_NOP;
                    BatservWrite( buff, 1 );
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
            CloseHandle( SemReadUp );
            CloseHandle( SemReadDone );
            CloseHandle( SemWritten );
            CloseHandle( MemHdl );
            UnmapViewOfFile( SharedMem );
            exit( 0 );
            break;
        }
    }
}

BOOL CALLBACK HideWindows( HWND hwnd, LPARAM lp )
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

BOOL WINAPI Ignore( DWORD CtrlType )
{
    CtrlType = CtrlType;
    return( TRUE );
}


void main( int argc, char *argv[] )
{
    SECURITY_ATTRIBUTES attr;

    argc=argc;argv=argv;
#if 0
    if( argc > 1 && (argv[1][0] == 'q' || argv[1][0] == 'Q') ) {
        h = CreateFile( PREFIX DEFAULT_NAME, GENERIC_WRITE, 0,
                NULL, OPEN_EXISTING, 0, NULL );
        if( h != INVALID_HANDLE_VALUE ) {
            done = LNK_SHUTDOWN;
            WriteFile( h, &done, sizeof( done ), &sent, NULL );
            CloseHandle( h );
        }
        exit( 0 );
    }
#endif
    SemReadUp = CreateSemaphore( NULL, 0, 1, READUP_NAME );
    SemReadDone = CreateSemaphore( NULL, 0, 1, READDONE_NAME );
    SemWritten = CreateSemaphore( NULL, 0, 1, WRITTEN_NAME );
    MemHdl = CreateFileMapping( (HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE,
                                0, 1024, SHARED_MEM_NAME  );
    SharedMem = MapViewOfFile( MemHdl, FILE_MAP_WRITE, 0, 0, 0 );
    CmdProc = getenv( "ComSpec" );
    if( CmdProc == NULL ) {
        fprintf( stderr, "Unable to find command processor\n" );
        exit( 1 );
    }
    SetConsoleCtrlHandler( NULL, FALSE );
    SetConsoleCtrlHandler( Ignore, TRUE );
    //NYI: need to accept name for link pipe
    attr.nLength = sizeof( attr );
    attr.lpSecurityDescriptor = NULL;
    attr.bInheritHandle = TRUE;
    NulHdl = CreateFile( "NUL", GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE,
            &attr, OPEN_EXISTING, 0, NULL );
    if( NulHdl == INVALID_HANDLE_VALUE ) {
        fprintf( stderr, "Unable to open NUL device\n" );
        exit( 1 );
    }
    EnumWindows( HideWindows , 0 );

    Say(( "LNK_UP\n" ));
    for( ;; ) {
        ProcessConnection();
    }
}
