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
#include <string.h>
#include "trptypes.h"
#include "packet.h"

#define READUP_NAME             MunchName( "WJVMReadUpSem" )
#define WRITTEN_NAME            MunchName( "WJVMWrittenSem" )
#define READDONE_NAME           MunchName( "WJVMReadDoneSem" )
#define SHARED_MEM_NAME         MunchName( "WJVMSharedMem" )
#define SHARED_MEM_NAME_2       MunchName( "WJVMTerminated" )

#define REMOTE_LINK_MEM_NAME    "WJVMRemoteLink"

#define MAX_TRANS       1024

static HANDLE           SemReadUp;
static HANDLE           SemWritten;
static HANDLE           SemReadDone;
static HANDLE           MemHdl;
static HANDLE           MemHdl2;
static HANDLE           MemHdlLink;
static char             *SharedMem;
static char             *Terminated;
DWORD                   *UniquePid;

#ifdef SERVER
    #define CHECK_DONE() if( (*Terminated) ) RemoteUnLink();
#else
    #define CHECK_DONE()
#endif

char *MunchName( char *name )
{
    static char buff[80];
    char pid[80];

    strcpy( buff, name );
    itoa( *UniquePid, pid, 16 );
    strcat( buff, pid );
    return( buff );
}

trap_retval RemoteGet( byte *buff, trap_elen len )
{
    unsigned    bytes_read;

    ReleaseSemaphore( SemReadUp, 1, NULL );
    WaitForSingleObject( SemWritten, INFINITE );
    CHECK_DONE();
    bytes_read = *(unsigned *)SharedMem;
    if( bytes_read > len )
        bytes_read = len;
    memcpy( buff, SharedMem + sizeof( unsigned ), bytes_read );
    ReleaseSemaphore( SemReadDone, 1, NULL );
    return( bytes_read );
}

trap_retval RemotePut( byte *buff, trap_elen len )
{
    WaitForSingleObject( SemReadUp, INFINITE );
    CHECK_DONE();
    *(unsigned *)SharedMem = len;
    memcpy( SharedMem + sizeof( unsigned ), buff, len );
    ReleaseSemaphore( SemWritten, 1, NULL );
    WaitForSingleObject( SemReadDone, INFINITE );
    CHECK_DONE();
    return( len );
}

bool RemoteConnect( void )
{
    return( TRUE );
}

void RemoteDisco( void )
{
}

#define JAVA_NAME "java_g.exe"

char *RemoteLink( char *parm, bool server )
{
#ifdef SERVER
    MemHdlLink = OpenFileMapping( FILE_MAP_WRITE, FALSE, REMOTE_LINK_MEM_NAME );
    if( MemHdlLink == NULL ) {
        return( "can not connect to debugger" );
    }
    UniquePid = MapViewOfFile( MemHdlLink, FILE_MAP_WRITE, 0, 0, 0 );

    SemReadUp = OpenSemaphore( SEMAPHORE_ALL_ACCESS, FALSE, READUP_NAME );
    SemReadDone = OpenSemaphore( SEMAPHORE_ALL_ACCESS, FALSE, READDONE_NAME );
    SemWritten = OpenSemaphore( SEMAPHORE_ALL_ACCESS, FALSE, WRITTEN_NAME );
    MemHdl = OpenFileMapping( FILE_MAP_WRITE, FALSE, SHARED_MEM_NAME );
    MemHdl2 = OpenFileMapping( FILE_MAP_WRITE, FALSE, SHARED_MEM_NAME_2 );
    SharedMem = MapViewOfFile( MemHdl, FILE_MAP_WRITE, 0, 0, 0 );
    Terminated = MapViewOfFile( MemHdl2, FILE_MAP_WRITE, 0, 0, 0 );
    if( MemHdl2 == NULL || MemHdl == NULL || SemReadUp == NULL ||
        SemReadDone == NULL || SemWritten == NULL ||
        SharedMem == NULL || Terminated == NULL || UniquePid == NULL ) {
        return( "can not connect to debugger" );
    }
#else
    STARTUPINFO             sinfo;
    PROCESS_INFORMATION     pinfo;
    char                    buff[2048];
    char                    command[_MAX_PATH];
    char                    *p;

    strcpy( buff, JAVA_NAME );
    if( GetEnvironmentVariable( "DEBUG_THE_JAVA_DEBUGGER", command, sizeof( command ) ) != 0 ) {
        strcat( command, ".exe " );
//          strcpy( command, "dvw.exe " );
    } else {
        command[0] = '\0';
    }
    strcat( command, buff );
    strcat( command, " " );
    GetModuleFileName( NULL, buff, sizeof( buff ) );
    p = buff + strlen( buff );
    while( --p >= buff ) {
        if( *p == '\\' || *p == '/' ) {
            *p = '\0';
            break;
        }
    }
    if( buff[0] != '\0' ) {
        strcat( command, " -classpath " );
        strcat( command, buff );
        strcat( command, "\\wdebug.zip" );
        if( GetEnvironmentVariable( "CLASSPATH", buff, sizeof( buff ) ) != 0 ) {
            strcat( command, ";" );
            strcat( command, buff );
        }
        if( parm != NULL && parm[0] != '\0' ) {
            p = parm + strlen( parm ) - 1;
            if( *p == '\\' || *p == '/' )
                *p = '\0';
            strcat( command, ";" );
            strcat( command, parm );
        }
    }
    memset( &sinfo, 0, sizeof( sinfo ) );
    strcat( command, " -debug sun.tools.debug.jvmhelp" );
    if( !CreateProcess( NULL, (void *)command, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, (void *)&sinfo, (void *)&pinfo ) ) {
        return( "unable to start " JAVA_NAME );
    }

    MemHdlLink = CreateFileMapping( (HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, sizeof( HANDLE ), REMOTE_LINK_MEM_NAME );
    if( MemHdlLink == NULL ) {
        return( "can not communicate with class sun.tools.debug.jvmhelp" );
    }
    UniquePid = MapViewOfFile( MemHdlLink, FILE_MAP_WRITE, 0, 0, 0 );
    *UniquePid = pinfo.dwProcessId;
    SemReadUp = CreateSemaphore( NULL, 0, 1, READUP_NAME );
    SemReadDone = CreateSemaphore( NULL, 0, 1, READDONE_NAME );
    SemWritten = CreateSemaphore( NULL, 0, 1, WRITTEN_NAME );
    MemHdl = CreateFileMapping( (HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, 1024, SHARED_MEM_NAME  );
    MemHdl2 = CreateFileMapping( (HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, 1, SHARED_MEM_NAME_2  );
    SharedMem = MapViewOfFile( MemHdl, FILE_MAP_WRITE, 0, 0, 0 );
    Terminated = MapViewOfFile( MemHdl2, FILE_MAP_WRITE, 0, 0, 0 );
    if( MemHdl2 == NULL || MemHdl == NULL || SemReadUp == NULL ||
        SemReadDone == NULL || SemWritten == NULL ||
        SharedMem == NULL || Terminated == NULL || UniquePid == NULL ) {
        return( "can not communicate with class sun.tools.debug.jvmhelp" );
    }
    *Terminated = 0;

    ResumeThread( pinfo.hThread );

#endif
    parm = parm;
    server = server;
    return( 0 );
}

void RemoteUnLink( void )
{
#ifdef SERVER
    CloseHandle( SemReadUp );
    CloseHandle( SemReadDone );
    CloseHandle( SemWritten );
    CloseHandle( MemHdl );
    UnmapViewOfFile( SharedMem );
    *Terminated = 0;
    CloseHandle( MemHdl2 );
    UnmapViewOfFile( Terminated );
    CloseHandle( MemHdlLink );
    UnmapViewOfFile( UniquePid );
    ExitProcess( 0 );
#else
    *Terminated = 1;
    ReleaseSemaphore( SemReadUp, 1, NULL );
    ReleaseSemaphore( SemReadDone, 1, NULL );
    ReleaseSemaphore( SemWritten, 1, NULL );
    while( !*Terminated ) Sleep( 500 );
    CloseHandle( SemReadUp );
    CloseHandle( SemReadDone );
    CloseHandle( SemWritten );
    CloseHandle( MemHdl );
    UnmapViewOfFile( SharedMem );
    CloseHandle( MemHdl2 );
    UnmapViewOfFile( Terminated );
    CloseHandle( MemHdlLink );
    UnmapViewOfFile( UniquePid );
#endif
    SharedMem = NULL;
}
