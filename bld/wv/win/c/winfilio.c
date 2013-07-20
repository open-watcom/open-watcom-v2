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
* Description:  Low-level Windows 3.x file I/O.
*
****************************************************************************/


#include <string.h>
#include "dbgdefn.h"
#include "dbgmem.h"
#include "tinyio.h"
#include "dbgio.h"
#include "dbgtoggl.h"
#include "farptrs.h"
#include "dbgreg.h"
#include "trptypes.h"

#include "doserr.h"

extern char             _osmajor;

file_components         LclFile = { '.', ':', { '\\', '/' }, { '\r', '\n' } };
char                    LclPathSep = { ';' };

void LocalErrMsg( sys_error code, char *buff )
{
    GetDOSErrMsg( code, buff );
}


sys_handle LocalOpen( char *name, open_access access )
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

unsigned LocalRead( sys_handle filehndl, void *ptr, unsigned len )
{
    tiny_ret_t  ret;

    ret = TinyRead( filehndl, ptr, len );
    if( TINY_ERROR( ret ) ) {
        StashErrCode( TINY_INFO( ret ), OP_LOCAL );
        return( ERR_RETURN );
    }
    return( TINY_INFO( ret ) );
}

unsigned LocalWrite( sys_handle filehndl, void *ptr, unsigned len )
{
    tiny_ret_t  ret;

    ret = TinyWrite( filehndl, ptr, len );
    if( TINY_ERROR( ret ) ) {
        StashErrCode( TINY_INFO( ret ), OP_LOCAL );
        return( ERR_RETURN );
    }
    return( TINY_INFO( ret ) );
}

unsigned long LocalSeek( sys_handle hdl, unsigned long npos, unsigned method )
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

unsigned LocalClose( sys_handle filehndl )
{
    tiny_ret_t  ret;

    ret = TinyClose( filehndl );
    if( TINY_ERROR( ret ) ) {
        return( StashErrCode( TINY_INFO( ret ), OP_LOCAL ) );
    }
    return( 0 );
}

unsigned LocalErase( char *name )
{
    tiny_ret_t  ret;

    ret = TinyDelete( name );
    if( TINY_ERROR( ret ) ) {
        return( StashErrCode( TINY_INFO( ret ), OP_LOCAL ) );
    }
    return( 0 );
}

sys_handle LocalHandle( handle h )
{
    return( h );
}
