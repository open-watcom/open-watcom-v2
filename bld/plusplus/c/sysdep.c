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


#include "plusplus.h"
#include <sys/types.h>
#include <stdio.h>
#include "wio.h"

#include "sysdep.h"
#include "watcom.h"

#if defined(__DOS__)

#include "tinyio.h"

size_t SysRead( int fh, void *buff, size_t amt )
/**********************************************/
{
    tiny_ret_t rc;

    rc = TinyRead( fh, buff, amt );
    if( TINY_ERROR( rc ) ) {
        return( -1 );
    }
    return( TINY_INFO( rc ) );
}

unsigned long SysTell( int fh )
/*****************************/
{
    unsigned long pos;
    tiny_ret_t    rc;

    rc = TinyLSeek( fh, 0, SEEK_CUR, (void __near *)&pos );
    if( TINY_ERROR( rc ) ) {
        return( -1L );
    }
    return( pos );
}

void SysSeek( int fh, unsigned long pos )
/***************************************/
{
     TinySeek( fh, pos, SEEK_SET );
}

#elif defined(__OS2__) || defined( __NT__ ) || defined( __UNIX__ )

size_t SysRead( int fh, void *buff, size_t amt )
/**********************************************/
{
    size_t rc;

    rc = read( fh, buff, amt );
    return( rc );
}

unsigned long SysTell( int fh )
/*****************************/
{
    return( lseek( fh, 0, SEEK_CUR ) );
}

void SysSeek( int fh, unsigned long pos )
/***************************************/
{
    lseek( fh, pos, SEEK_SET );
}

#else

#error SYSDEP has not been configured for host OS.

#endif


time_t SysFileTime(             // GET TIME STAMP FOR FILE
    FILE *fp )                  // - file control
{
    struct stat file_info;      // - file information
    time_t time_stamp;          // - time stamp for file

    if( fstat( fileno( fp ), &file_info ) ) {
        time_stamp = 0;     // not terrific error handling
    } else {
        time_stamp = file_info.st_mtime;
    }
    return time_stamp;
}
