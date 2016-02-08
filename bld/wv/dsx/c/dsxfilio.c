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


#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgmem.h"
#include "tinyio.h"
#include "dbgio.h"
#include "doserr.h"
#include "filelcl.h"

extern char             _osmajor;

file_components         LclFile = { '.', ':', { '\\', '/' }, { '\r', '\n' } };
char                    LclPathSep = { ';' };

void LocalErrMsg( sys_error code, char *buff )
{
    GetDOSErrMsg( code, buff );
}

sys_handle LocalOpen( const char *name, open_access access )
{
    tiny_ret_t  ret;
    unsigned    mode;

    if( (access & OP_WRITE) == 0 ) {
        mode = TIO_READ;
        access &= ~(OP_CREATE|OP_TRUNC);
    } else if( access & OP_READ ) {
        mode = TIO_READ_WRITE;
    } else {
        mode = TIO_WRITE;
    }
    if( access & (OP_CREATE|OP_TRUNC) ) {
        ret = TinyCreate( name, TIO_NORMAL );
    } else {
        if( _osmajor >= 3 ) mode |= 0x80; /* set no inheritance */
        ret = TinyOpen( name, mode );
    }
    if( TINY_ERROR( ret ) ) {
        StashErrCode( TINY_INFO( ret ), OP_LOCAL );
        return( NIL_SYS_HANDLE );
    }
    return( TINY_INFO( ret ) );
}

size_t LocalRead( sys_handle filehndl, void *ptr, size_t len )
{
    tiny_ret_t  ret;
    size_t      total;
    unsigned    buff_len;
    unsigned    read_len;

    buff_len = INT_MAX;
    total = 0;
    while( len > 0 ) {
        if( buff_len > len )
            buff_len = (unsigned)len;
        ret = TinyRead( filehndl, ptr, buff_len );
        if( TINY_ERROR( ret ) ) {
            StashErrCode( TINY_INFO( ret ), OP_LOCAL );
            return( ERR_RETURN );
        }
        read_len = TINY_INFO( ret );
        total += read_len;
        if( read_len != buff_len )
            break;
        ptr = (char *)ptr + read_len;
        len -= read_len;
    }
    return( total );
}

size_t LocalWrite( sys_handle filehndl, const void *ptr, size_t len )
{
    tiny_ret_t  ret;
    size_t      total;
    unsigned    buff_len;
    unsigned    write_len;

    buff_len = INT_MAX;
    total = 0;
    while( len > 0 ) {
        if( buff_len > len )
            buff_len = (unsigned)len;
        ret = TinyWrite( filehndl, ptr, buff_len );
        if( TINY_ERROR( ret ) ) {
            StashErrCode( TINY_INFO( ret ), OP_LOCAL );
            return( ERR_RETURN );
        }
        write_len = TINY_INFO( ret );
        total += write_len;
        if( write_len != buff_len )
            break;
        ptr = (char *)ptr + write_len;
        len -= write_len;
    }
    return( total );
}

unsigned long LocalSeek( sys_handle hdl, unsigned long npos, seek_method method )
{
    tiny_ret_t      ret;
    unsigned long   pos;

    ret = TinyLSeek( hdl, npos, method, (u32_stk_ptr)&pos );
    if( TINY_ERROR( ret ) ) {
        StashErrCode( TINY_INFO( ret ), OP_LOCAL );
        return( -1UL );
    }
    return( pos );
}

error_idx LocalClose( sys_handle filehndl )
{
    tiny_ret_t  ret;

    ret = TinyClose( filehndl );
    if( TINY_ERROR( ret ) ) {
        return( StashErrCode( TINY_INFO( ret ), OP_LOCAL ) );
    }
    return( 0 );
}

error_idx LocalErase( const char *name )
{
    tiny_ret_t  ret;

    ret = TinyDelete( name );
    if( TINY_ERROR( ret ) ) {
        return( StashErrCode( TINY_INFO( ret ), OP_LOCAL ) );
    }
    return( 0 );
}

sys_handle LocalHandleSys( handle h )
{
    return( h );
}
