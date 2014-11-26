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


#include <errno.h>
#include <limits.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgmem.h"
#include "dbgio.h"


#define READONLY    0
#define WRITEONLY   1
#define READWRITE   2
#define FROMEND     2

typedef long   dosret;

extern char *Format(char *,char *,... );
extern char *StrCopy(char *,char *);
extern void FreeRing(char_ring *);

extern int              DbgConHandle; /* Debugger's console file handle */

file_components         LclFile = { '.', '\0', { '/' }, { '\n' } };
char                    LclPathSep = { ':' };


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
    ret = open( name, openmode, 0666 );
    if( (int)ret == -1 ) {
        StashErrCode( errno, OP_LOCAL );
        return( NIL_SYS_HANDLE );
    }
    fcntl( ret, F_SETFD, (int)FD_CLOEXEC );
    return( ret );
}

unsigned LocalRead( sys_handle filehndl, void *ptr, unsigned len )
{
    int         ret;
    unsigned    curr;
    unsigned    total;

    total = 0;
    for( ;; ) {
        if( len == 0 ) break;
        curr = len;
        if( curr > INT_MAX ) curr = INT_MAX;
        ret = read( filehndl, ptr, curr );
        if( ret < 0 ) {
            StashErrCode( errno, OP_LOCAL );
            return( ERR_RETURN );
        }
        total += ret;
        if( ret != curr ) break;
        len -= ret;
        ptr = (char *)ptr + ret;
    }
    return( total );
}

unsigned LocalWrite( sys_handle filehndl, void *ptr, unsigned len )
{
    int  ret;
    unsigned    total;
    unsigned    curr;

    total = 0;
    for( ;; ) {
        if( len == 0 ) return( total );
        curr = len;
        if( curr > INT_MAX ) curr = INT_MAX;
        ret = write( filehndl, ptr, curr );
        if( ret <= 0 ) {
            StashErrCode( errno, OP_LOCAL );
            return( ERR_RETURN );
        }
        ptr = (char *)ptr + ret;
        total += ret;
        len -= ret;
    }
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
    if( unlink( name ) == 0 )
        return( 0 );
    return( StashErrCode( errno, OP_LOCAL ) );
}

sys_handle LocalHandle( handle h )
{
    return( h );
}
