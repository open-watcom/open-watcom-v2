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


#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include "wio.h"
#include "dbgdefn.h"
#if !defined( BUILD_RFX )
#include "dbgdata.h"
#else
#include "rfxdata.h"
#endif
#include "dbgmem.h"
#include "dbgio.h"
#include "errno.h"
#include "strutil.h"

extern void FreeRing(char_ring *);

file_components         LclFile = { '.', ':', { '\\', '/' }, { '\r', '\n' } };
char                    LclPathSep = { ';' };


//NYI: should use native NT calls

void LocalErrMsg( sys_error code, char *buff )
{
    StrCopy( strerror( code ), buff );
}

sys_handle LocalOpen( char *name, open_access access )
{
    unsigned    openmode;
    int         ret;

    if( (access & OP_WRITE) == 0 ) {
        openmode = O_RDONLY;
    } else if( access & OP_READ ) {
        openmode = O_RDWR;
    } else {
        openmode = O_WRONLY;
    }
    if( access & OP_CREATE ) openmode |= O_CREAT;
    if( access & OP_TRUNC ) openmode |= O_TRUNC;
    ret = open( name, openmode | O_BINARY, 0666 );
    if( (int)ret == -1 ) {
        StashErrCode( errno, OP_LOCAL );
        return( NIL_SYS_HANDLE );
    }
//    fcntl( ret, F_SETFD, (int)FD_CLOEXEC );
    return( ret );
}

unsigned LocalRead( sys_handle filehndl, void *ptr, unsigned len )
{
    int         ret;

    ret = read( filehndl, ptr, len );
    if( ret < 0 ) {
        StashErrCode( errno, OP_LOCAL );
        return( ERR_RETURN );
    }
    return( ret );
}

unsigned LocalWrite( sys_handle filehndl, void *ptr, unsigned len )
{
    int         ret;

    ret = write( filehndl, ptr, len );
    if( ret < 0 ) {
        StashErrCode( errno, OP_LOCAL );
        return( ERR_RETURN );
    }
    return( ret );
}

unsigned long LocalSeek( sys_handle hdl, unsigned long len, unsigned method )
{
    off_t       ret;

    ret = lseek( hdl, len, method );
    if( ret == (off_t)-1 ) {
        StashErrCode( errno, OP_LOCAL );
        return( -1UL );
    }
    return( ret );
}

rc_erridx LocalClose( sys_handle filehndl )
{
    if( close( filehndl ) == 0 )
        return( 0 );
    return( StashErrCode( errno, OP_LOCAL ) );
}

rc_erridx LocalErase( char *name )
{
    if( remove( name ) == 0 )
        return( 0 );
    return( StashErrCode( errno, OP_LOCAL ) );
}

sys_handle LocalHandleSys( handle h )
{
    return( h );
}
