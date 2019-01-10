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


typedef struct file_handle_t {
    int         fhandle;
} *file_handle;

int FileInit( const char *archive )
{
    return( 0 );
}


int FileFini( void )
{
    return( 0 );
}


int FileIsPlainFS( void )
{
    return( 1 );
}


int FileIsArchive( void )
{
    return( 0 );
}


int FileStat( const char *path, struct stat *buf )
{
    return( stat( path, buf ) );
}


file_handle FileOpen( const char *path, int flags )
{
    file_handle fh;

    fh = malloc( sizeof( *fh ) );
    if( fh == NULL )
        return( NULL );

    fh->fhandle = open( path, flags );
    if( fh->fhandle == -1 ) {
        free( fh );
        fh = NULL;
    }
    return( fh );
}


int FileClose( file_handle fh )
{
    int             rc;

    rc = close( fh->fhandle );
    free( fh );

    return( rc );
}


long FileSeek( file_handle fh, long offset, int origin )
{
    return( lseek( fh->fhandle, offset, origin ) );
}


size_t FileRead( file_handle fh, void *buffer, size_t length )
{
    return( read( fh->fhandle, buffer, length ) );
}
