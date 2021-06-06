/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include <stddef.h>
#include <limits.h>
#define INCL_ERRORS
#define INCL_BASE
#include <wos2.h>
#include "dbgdefn.h"
#if !defined( BUILD_RFX )
#include "dbgdata.h"
#else
#include "rfxdata.h"
#endif
#include "dbgmem.h"
#include "dbgio.h"
#include "doserr.h"
#include "filelcl.h"

#define SYSH2LH(sh)     (HFILE)((sh).u._32[0])
#define LH2SYSH(sh,lh)  (sh).u._32[0]=lh;(sh).u._32[1]=0

const file_components   LclFile = { '.', ':', { '\\', '/' }, { '\r', '\n' } };
const char              LclPathSep = { ';' };

static const OS_UINT    local_seek_method[] = { FILE_BEGIN, FILE_CURRENT, FILE_END };

void LocalErrMsg( sys_error code, char *buff )
{
    char        *s;
    char        *d;
    OS_UINT     msg_len;
    char        ch;

    if( DosGetMessage( NULL, 0, buff, 50, code, "OSO001.MSG", &msg_len ) != 0 ) {
        GetDOSErrMsg( code, buff );
        return;
    }
    buff[msg_len] = NULLCHAR;
    s = d = buff;
    if( s[0] == 'S' && s[1] == 'Y' && s[2] == 'S' ) {
        /* Got the SYSxxxx: at the front. Take it off. */
        s += 3;
        while( (ch = *s++) != ':' ) {
            if( ch < '0' || ch > '9' ) {
                s = buff;
                break;
            }
        }
    }
    while( *s == ' ' )
        ++s;
    while( (ch = *s++) != NULLCHAR ) {
        if( ch == '\n' )
            ch = ' ';
        if( ch != '\r' ) {
            *d++ = ch;
        }
    }
    while( d > buff && d[-1] == ' ' )
        --d;
    *d = NULLCHAR;
}

sys_handle LocalOpen( const char *name, obj_attrs oattrs )
{
    HFILE       hdl;
    OS_UINT     action;
    OS_UINT     openflags;
    OS_UINT     openmode;
    APIRET      rc;
    sys_handle  sh;

    openmode = OPEN_FLAGS_FAIL_ON_ERROR | OPEN_FLAGS_NOINHERIT | OPEN_SHARE_DENYNONE;
    if( (oattrs & OP_WRITE) == 0 ) {
        openmode |= OPEN_ACCESS_READONLY;
        oattrs &= ~(OP_CREATE | OP_TRUNC);
    } else if( oattrs & OP_READ ) {
        openmode |= OPEN_ACCESS_READWRITE;
    } else {
        openmode |= OPEN_ACCESS_WRITEONLY;
    }
    if( oattrs & OP_CREATE ) {
        openflags = OPEN_ACTION_CREATE_IF_NEW;
    } else {
        openflags = OPEN_ACTION_FAIL_IF_NEW;
    }
    if( oattrs & OP_TRUNC ) {
        openflags |= OPEN_ACTION_REPLACE_IF_EXISTS;
    } else {
        openflags |= OPEN_ACTION_OPEN_IF_EXISTS;
    }
    rc = DosOpen( (char *)name, /* name */
                &hdl,           /* handle to be filled in */
                &action,        /* action taken */
                0,              /* initial allocation */
                FILE_NORMAL,    /* normal file */
                openflags,      /* open the file */
                openmode,       /* deny-none, inheritance */
                0 );            /* reserved */
    if( rc != 0 ) {
        StashErrCode( rc, OP_LOCAL );
        SET_SYSHANDLE_NULL( sh );
        return( sh );
    }
    LH2SYSH( sh, hdl );
    return( sh );
}

size_t LocalRead( sys_handle sh, void *ptr, size_t len )
{
    OS_UINT     read_len;
    APIRET      ret;
    size_t      total;
    unsigned    piece_len;

    piece_len = INT_MAX;
    total = 0;
    while( len > 0 ) {
        if( piece_len > len )
            piece_len = (unsigned)len;
        ret = DosRead( SYSH2LH( sh ), ptr, piece_len, &read_len );
        if( ret != 0 ) {
            StashErrCode( ret, OP_LOCAL );
            return( ERR_RETURN );
        }
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
    OS_UINT     write_len;
    APIRET      ret;
    size_t      total;
    unsigned    piece_len;

    piece_len = INT_MAX;
    total = 0;
    while( len > 0 ) {
        if( piece_len > len )
            piece_len = (unsigned)len;
        ret = DosWrite( SYSH2LH( sh ), (PVOID)ptr, piece_len, &write_len );
        if( ret != 0 ) {
            StashErrCode( ret, OP_LOCAL );
            return( ERR_RETURN );
        }
        total += write_len;
        if( write_len != piece_len )
            break;
        ptr = (char *)ptr + write_len;
        len -= write_len;
    }
    return( total );
}

unsigned long LocalSeek( sys_handle sh, unsigned long len, seek_method method )
{
    ULONG           new;
    APIRET          ret;

#ifdef _M_I86
    ret = DosChgFilePtr( SYSH2LH( sh ), len, local_seek_method[method], &new );
#else
    ret = DosSetFilePtr( SYSH2LH( sh ), len, local_seek_method[method], &new );
#endif
    if( ret != 0 ) {
        StashErrCode( ret, OP_LOCAL );
        return( ERR_SEEK );
    }
    return( new );
}

error_handle LocalClose( sys_handle sh )
{
    APIRET      ret;

    ret = DosClose( SYSH2LH( sh ) );
    return( StashErrCode( ret, OP_LOCAL ) );
}

error_handle LocalErase( const char *name )
{
    APIRET      ret;

#ifdef _M_I86
    ret = DosDelete( (char *)name, 0 );
#else
    ret = DosDelete( name );
#endif
    return( StashErrCode( ret, OP_LOCAL ) );
}

sys_handle LocalHandleSys( file_handle fh )
{
    sys_handle  sh;

    FH2SYSH( sh, fh );
    return( sh );
}
