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
* Description:  Dummy data source file I/O routines (local files only)
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wio.h"
#include "setupio.h"


typedef struct data_source_t {
    int         fhandle;
} data_source;

extern int FileInit( const char *archive )
{
    return( 0 );
}


extern int FileFini( void )
{
    return( 0 );
}


extern int FileIsPlainFS( void )
{
    return( 1 );
}


extern int FileIsArchive( void )
{
    return( 0 );
}


extern int FileStat( const char *path, struct stat *buf )
{
    return( stat( path, buf ) );
}


extern void *FileOpen( const char *path, int flags )
{
    data_source     *ds;

    ds = malloc( sizeof( *ds ) );
    if( ds == NULL )
        return( NULL );

    ds->fhandle = open( path, flags );
    if( ds->fhandle == -1 ) {
        free( ds );
        ds = NULL;
    }
    return( ds );
}


extern int FileClose( void *handle )
{
    data_source     *ds = handle;
    int             rc;

    rc = close( ds->fhandle );
    free( ds );

    return( rc );
}


extern long FileSeek( void *handle, long offset, int origin )
{
    data_source     *ds = handle;

    return( lseek( ds->fhandle, offset, origin ) );
}


extern size_t FileRead( void *handle, void *buffer, size_t length )
{
    data_source     *ds = handle;

    return( read( ds->fhandle, buffer, length ) );
}
