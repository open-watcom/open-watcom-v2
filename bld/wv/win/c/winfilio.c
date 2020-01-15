/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Low-level Windows 3.x file I/O.
*
****************************************************************************/


#include <limits.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgmem.h"
#include "tinyio.h"
#include "dbgio.h"
#include "doserr.h"
#include "filelcl.h"


#define SYSH2LH(sh)     (tiny_handle_t)((sh).u._32[0])
#define LH2SYSH(sh,lh)  (sh).u._32[0]=lh;(sh).u._32[1]=0

const file_components   LclFile = { '.', ':', { '\\', '/' }, { '\r', '\n' } };
const char              LclPathSep = { ';' };

static const seek_info  local_seek_method[] = { TIO_SEEK_SET, TIO_SEEK_CUR, TIO_SEEK_END };

void LocalErrMsg( sys_error code, char *buff )
{
    GetDOSErrMsg( code, buff );
}

sys_handle LocalOpen( const char *name, obj_attrs oattrs )
{
    tiny_ret_t  ret;
    unsigned    mode;
    sys_handle  sh;

    if( (oattrs & OP_WRITE) == 0 ) {
        mode = TIO_READ;
        oattrs &= ~(OP_CREATE | OP_TRUNC);
    } else if( oattrs & OP_READ ) {
        mode = TIO_READ_WRITE;
    } else {
        mode = TIO_WRITE;
    }
    if( oattrs & (OP_CREATE | OP_TRUNC) ) {
        ret = TinyCreate( name, TIO_NORMAL );
    } else {
        if( _osmajor >= 3 )
            mode |= 0x80; /* set no inheritance */
        ret = TinyOpen( name, mode );
    }
    if( TINY_ERROR( ret ) ) {
        StashErrCode( TINY_INFO( ret ), OP_LOCAL );
        SET_SYSHANDLE_NULL( sh );
        return( sh );
    }
    LH2SYSH( sh, TINY_INFO( ret ) );
    return( sh );
}

size_t LocalRead( sys_handle sh, void *ptr, size_t len )
{
    tiny_ret_t  ret;
    size_t      total;
    unsigned    piece_len;
    unsigned    read_len;

    piece_len = INT_MAX;
    total = 0;
    while( len > 0 ) {
        if( piece_len > len )
            piece_len = (unsigned)len;
        ret = TinyRead( SYSH2LH( sh ), ptr, piece_len );
        if( TINY_ERROR( ret ) ) {
            StashErrCode( TINY_INFO( ret ), OP_LOCAL );
            return( ERR_RETURN );
        }
        read_len = TINY_INFO( ret );
        total += read_len;
        if( read_len != piece_len )
            break;
        ptr = (char *)ptr + read_len;
        len -= read_len;
    }
    return( total );
}

size_t LocalWrite( sys_handle sh, const void *ptr, size_t len )
{
    tiny_ret_t  ret;
    size_t      total;
    unsigned    piece_len;
    unsigned    write_len;

    piece_len = INT_MAX;
    total = 0;
    while( len > 0 ) {
        if( piece_len > len )
            piece_len = (unsigned)len;
        ret = TinyWrite( SYSH2LH( sh ), ptr, piece_len );
        if( TINY_ERROR( ret ) ) {
            StashErrCode( TINY_INFO( ret ), OP_LOCAL );
            return( ERR_RETURN );
        }
        write_len = TINY_INFO( ret );
        total += write_len;
        if( write_len != piece_len )
            break;
        ptr = (char *)ptr + write_len;
        len -= write_len;
    }
    return( total );
}

unsigned long LocalSeek( sys_handle sh, unsigned long npos, seek_method method )
{
    tiny_ret_t      ret;
    uint_32         pos;

    ret = TinyLSeek( SYSH2LH( sh ), npos, local_seek_method[method], &pos );
    if( TINY_ERROR( ret ) ) {
        StashErrCode( TINY_INFO( ret ), OP_LOCAL );
        return( ERR_SEEK );
    }
    return( pos );
}

error_handle LocalClose( sys_handle sh )
{
    tiny_ret_t  ret;

    ret = TinyClose( SYSH2LH( sh ) );
    if( TINY_ERROR( ret ) ) {
        return( StashErrCode( TINY_INFO( ret ), OP_LOCAL ) );
    }
    return( 0 );
}

error_handle LocalErase( const char *name )
{
    tiny_ret_t  ret;

    ret = TinyDelete( name );
    if( TINY_ERROR( ret ) ) {
        return( StashErrCode( TINY_INFO( ret ), OP_LOCAL ) );
    }
    return( 0 );
}

sys_handle LocalHandleSys( file_handle fh )
{
    sys_handle  sh;

    FH2SYSH( sh, fh );
    return( sh );
}
