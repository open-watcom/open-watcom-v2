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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <string.h>
#ifdef __NT__
#include <windows.h>
#else
#include <fcntl.h>
#include <dos.h>
#endif
#include "batpipe.h"


#ifdef __NT__
#define READUP_NAME         "ReadUpSem"
#define WRITTEN_NAME        "WrittenSem"
#define READDONE_NAME       "ReadDoneSem"
#endif

batch_data              bdata;

#ifdef __NT__
static batch_shmem      *SharedMemPtr;
static HANDLE           SemReadUp;
static HANDLE           SemWritten;
static HANDLE           SemReadDone;
static HANDLE           MemHdl;
#else
static int              pipeHdl = -1;
#endif

#ifdef __NT__
int BatservPipeCreate( const char *name )
{
    SemReadUp = CreateSemaphore( NULL, 0, 1, READUP_NAME );
    SemReadDone = CreateSemaphore( NULL, 0, 1, READDONE_NAME );
    SemWritten = CreateSemaphore( NULL, 0, 1, WRITTEN_NAME );
    MemHdl = CreateFileMapping( INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, TRANS_MAXLEN, name );
    if( MemHdl == NULL || SemReadUp == NULL || SemReadDone == NULL || SemWritten == NULL ) {
        return( 1 );
    }
    SharedMemPtr = MapViewOfFile( MemHdl, FILE_MAP_WRITE, 0, 0, 0 );
    return( 0 );
}
#endif

int BatservPipeOpen( const char *name )
{
#ifdef __NT__
    SemReadUp = OpenSemaphore( SEMAPHORE_ALL_ACCESS, FALSE, READUP_NAME );
    SemReadDone = OpenSemaphore( SEMAPHORE_ALL_ACCESS, FALSE, READDONE_NAME );
    SemWritten = OpenSemaphore( SEMAPHORE_ALL_ACCESS, FALSE, WRITTEN_NAME );
    MemHdl = OpenFileMapping( FILE_MAP_WRITE, FALSE, name );
    if( MemHdl == NULL || SemReadUp == NULL || SemReadDone == NULL || SemWritten == NULL ) {
        return( 1 );
    }
    SharedMemPtr = MapViewOfFile( MemHdl, FILE_MAP_WRITE, 0, 0, 0 );
#else
    if( _dos_open( name, O_RDWR, &pipeHdl ) != 0 ) {
        return( 1 );
    }
#endif
    return( 0 );
}

void BatservPipeClose( void )
{
#ifdef __NT__
    if( SemReadUp != NULL ) {
        CloseHandle( SemReadUp );
        SemReadUp = NULL;
    }
    if( SemReadDone != NULL ) {
        CloseHandle( SemReadDone );
        SemReadDone = NULL;
    }
    if( SemWritten != NULL ) {
        CloseHandle( SemWritten );
        SemWritten = NULL;
    }
    if( MemHdl != NULL ) {
        CloseHandle( MemHdl );
        MemHdl = NULL;
    }
    if( SharedMemPtr != NULL ) {
        UnmapViewOfFile( SharedMemPtr );
        SharedMemPtr = NULL;
    }
#else
    if( pipeHdl != -1 ) {
        _dos_close( pipeHdl );
        pipeHdl = -1;
    }
#endif
}

int BatservReadData( void )
{
    unsigned    bytes_read;

#ifdef __NT__
    ReleaseSemaphore( SemReadUp, 1, NULL );
    WaitForSingleObject( SemWritten, INFINITE );
    bytes_read = SharedMemPtr->len;
    if( bytes_read > 0 ) {
        memcpy( bdata.u.buffer, SharedMemPtr->data.u.buffer, bytes_read );
    }
    ReleaseSemaphore( SemReadDone, 1, NULL );
#else
    if( _dos_read( pipeHdl, bdata.u.buffer, TRANS_BDATA_MAXLEN, &bytes_read ) != 0 )
        return( -1 );
#endif
    return( bytes_read - 1 );
}

int BatservWriteCmd( char link_cmd )
{
#ifdef __NT__
    WaitForSingleObject( SemReadUp, INFINITE );
    SharedMemPtr->len = 1;
    SharedMemPtr->data.u.s.cmd = link_cmd;
    ReleaseSemaphore( SemWritten, 1, NULL );
    WaitForSingleObject( SemReadDone, INFINITE );
#else
    unsigned    sent;

    if( _dos_write( pipeHdl, &link_cmd, 1, &sent ) != 0 )
        return( -1 );
#endif
    return( 0 );
}

int BatservWriteData( char link_cmd, const void *buff, unsigned len )
{
    if( len > TRANS_DATA_MAXLEN )
        len = TRANS_DATA_MAXLEN;
#ifdef __NT__
    WaitForSingleObject( SemReadUp, INFINITE );
    SharedMemPtr->len = len + 1;
    SharedMemPtr->data.u.s.cmd = link_cmd;
    memcpy( SharedMemPtr->data.u.s.u.data, buff, len );
    ReleaseSemaphore( SemWritten, 1, NULL );
    WaitForSingleObject( SemReadDone, INFINITE );
#else
    bdata.u.s.cmd = link_cmd;
    memcpy( bdata.u.s.u.data, buff, len );
    if( _dos_write( pipeHdl, bdata.u.buffer, len + 1, &len ) != 0 )
        return( -1 );
    len--;
#endif
    return( len );
}
