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


/*
        BATCLI :  NT interface to BATSERV
*/

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include "batpipe.h"

static HANDLE MemHdl;

char *BatchLink( char *name )
{
    if( name == NULL ) name = DEFAULT_NAME;
    SemReadUp = OpenSemaphore( SEMAPHORE_ALL_ACCESS, FALSE, READUP_NAME );
    SemReadDone = OpenSemaphore( SEMAPHORE_ALL_ACCESS, FALSE, READDONE_NAME );
    SemWritten = OpenSemaphore( SEMAPHORE_ALL_ACCESS, FALSE, WRITTEN_NAME );
    MemHdl = OpenFileMapping( FILE_MAP_WRITE, FALSE, SHARED_MEM_NAME );
    if( MemHdl == NULL || SemReadUp == NULL || SemWritten == NULL ) {
        return( "can not connect to batcher spawn server" );
    }
    SharedMem = MapViewOfFile( MemHdl, FILE_MAP_WRITE, 0, 0, 0 );
    return( NULL );
}

unsigned BatchMaxCmdLine()
{
    return( MAX_TRANS-1 );
}

static char     buff[MAX_TRANS]; /* static to miminize stack usage */

unsigned BatchChdir( char *new_dir )
{

    buff[0] = LNK_CWD;
    strcpy( &buff[1], new_dir );
    BatservWrite( buff, strlen( buff ) + 1 );
    BatservRead( buff, sizeof( buff ) );
    return( *(unsigned long *)&buff[1] );
}

unsigned BatchSpawn( char *cmd )
{
    buff[0] = LNK_RUN;
    strcpy( &buff[1], cmd );
    BatservWrite( buff, strlen( buff ) );
    return( 0 );
}

unsigned BatchCollect( void *ptr, unsigned max, unsigned long *status )
{
    int         len;
    char        *buff = ptr;

    buff[0] = LNK_QUERY;
    *(unsigned long *)&buff[1] = max;
    BatservWrite( buff, 5 );
    len = BatservRead( buff, max ) - 1;
    if( len <= 0 ) return( 0 );
    if( *buff == LNK_STATUS ) {
        *status = *(unsigned long *)&buff[1];
        return( -1 );
    }
    memmove( buff, &buff[1], len );
    return( len );
}

unsigned BatchCancel()
{
    char        cmd;

    cmd = LNK_CANCEL;
    BatservWrite( &cmd, sizeof( cmd ) );
    return( 0 );
}

unsigned BatchAbort()
{
    char        cmd;

    cmd = LNK_ABORT;
    BatservWrite( &cmd, sizeof( cmd ) );
    return( 0 );
}


void BatchUnlink( int shutdown )
{
    char        done;

    done = shutdown ? LNK_SHUTDOWN : LNK_DONE;
    BatservWrite( &done, sizeof( done ) );
    CloseHandle( SemReadUp );
    CloseHandle( SemReadDone );
    CloseHandle( SemWritten );
    CloseHandle( MemHdl );
    UnmapViewOfFile( SharedMem );
    SharedMem = NULL;
}
