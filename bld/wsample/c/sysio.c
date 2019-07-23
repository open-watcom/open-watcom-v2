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
* Description:  System I/O layered on top of POSIX calls.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "sample.h"
#include "smpstuff.h"
#include "sysio.h"

#include "clibext.h"


#ifndef O_BINARY
    #define O_BINARY 0
#endif

#if defined( _MSC_VER ) && defined( _WIN64 )
static ssize_t  posix_write( int fildes, void const *buffer, size_t nbyte )
{
    unsigned    write_len;
    unsigned    amount;
    size_t      size;

    amount = INT_MAX;
    size = 0;
    while( nbyte > 0 ) {
        if( amount > nbyte )
            amount = (unsigned)nbyte;
        write_len = _write( fildes, buffer, amount );
        if( write_len == (unsigned)-1 ) {
            return( (ssize_t)-1 );
        }
        size += write_len;
        if( write_len != amount ) {
            break;
        }
        buffer = (char *)buffer + amount;
        nbyte -= amount;
    }
    return( size );
}
#else
#define posix_write     write
#endif

int SysCreate( const char *name )
{
    return( open( name, O_BINARY | O_WRONLY | O_TRUNC | O_CREAT,
                S_IRUSR|S_IWUSR | S_IRGRP|S_IWGRP | S_IROTH|S_IWOTH ) );
}

size_t SysWrite( int handle, const void FAR_PTR *buff, size_t len )
{
    return( posix_write( handle, buff, len ) );
}

unsigned long SysSeek( int handle, unsigned long loc )
{
    return( lseek( handle, loc, SEEK_SET ) );
}

int SysClose( int handle )
{
    return( close( handle ) );
}

void FAR_PTR *my_alloc( size_t size )
{
    return( malloc( size ) );
}

void my_free( void FAR_PTR *chunk )
{
    free( chunk );
}
