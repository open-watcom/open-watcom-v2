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
#include "dbgmem.h"
#include "dbgio.h"
#include "dbgtoggl.h"
#include "farptrs.h"
#include "dbgreg.h"
#include "trpfile.h"
#include <string.h>
#include <stddef.h>
#define INCL_ERRORS
#define INCL_BASE
#include <os2.h>

#include "doserr.h"

#define READONLY    0
#define WRITEONLY   1
#define READWRITE   2
#define FROMEND     2

file_components         LclFile = { '.', { '\\', '/', ':' }, { '\r', '\n' } };
char                    LclPathSep = { ';' };

void LocalErrMsg( sys_error code, char *buff )
{
    char        *s;
    char        *d;
    USHORT      msg_len;
    char        ch;

    if( DosGetMessage( NULL, 0, buff, 50, code, "OSO001.MSG",
                        &msg_len ) != 0 ) {
        GetDOSErrMsg( code, buff );
        return;
    }
    buff[msg_len] = '\0';
    s = d = buff;
    if( s[0] == 'S' && s[1] == 'Y' && s[2] == 'S' ) {
        /* Got the SYSxxxx: at the front. Take it off. */
        s += 3;
        for( ;; ) {
            ch = *s++;
            if( ch == ':' ) break;
            if( ch < '0' || ch > '9' ) {
                s = buff;
                break;
            }
        }
    }
    while( *s == ' ' ) ++s;
    for( ;; ) {
        ch = *s++;
        if( ch == '\0' ) break;
        if( ch == '\n' ) ch = ' ';
        if( ch != '\r' ) *d++ = ch;
    }
    while( d > buff && d[-1] == ' ' ) --d;
    *d = '\0';
}

sys_handle LocalOpen( char *name, open_access access )
{
    HFILE       hdl;
    USHORT      action;
    USHORT      openflags;
    USHORT      openmode;
    USHORT      rc;

    if( (access & OP_WRITE) == 0 ) {
        openmode = READONLY;
        access &= ~(OP_CREATE|OP_TRUNC);
    } else if( access & OP_READ ) {
        openmode = READWRITE;
    } else {
        openmode = WRITEONLY;
    }
    openmode |= 0x20c0;
    openflags = 0;
    if( access & OP_CREATE ) openflags |= 0x10;
    openflags |= (access & OP_TRUNC) ? 0x02 : 0x01;
    rc = DosOpen( name,         /* name */
                &hdl,           /* handle to be filled in */
                &action,        /* action taken */
                0,              /* initial allocation */
                0,              /* normal file */
                openflags,      /* open the file */
                openmode,       /* deny-none, inheritance */
                0 );            /* reserved */
    if( rc != 0 ) {
        StashErrCode( rc, OP_LOCAL );
        return( NIL_SYS_HANDLE );
    }
    return( hdl );
}

unsigned LocalRead( sys_handle filehndl, void *ptr, unsigned len )
{
    USHORT      read;
    USHORT      ret;

    ret = DosRead( filehndl, ptr, len, &read );
    if( ret != 0 ) {
        StashErrCode( ret, OP_LOCAL );
        return( ERR_RETURN );
    }
    return( read );
}

unsigned LocalWrite( sys_handle filehndl, void *ptr, unsigned len )
{
    USHORT  written;
    USHORT  ret;

    ret = DosWrite( filehndl, ptr, len, &written );
    if( ret != 0 ) {
        StashErrCode( ret, OP_LOCAL );
        return( ERR_RETURN );
    }
    return( written );
}

unsigned long LocalSeek( sys_handle hdl, unsigned long len, unsigned method )
{
    unsigned long   new;
    USHORT          ret;

    ret = DosChgFilePtr( hdl, len, method, &new );
    if( ret != 0 ) {
        StashErrCode( ret, OP_LOCAL );
        return( -1UL );
    }
    return( new );
}

unsigned LocalClose( sys_handle filehndl )
{
    USHORT      ret;

    ret = DosClose( filehndl );
    return( StashErrCode( ret, OP_LOCAL ) );
}

unsigned LocalErase( char *name )
{
    USHORT      ret;

    ret = DosDelete( name, 0 );
    return( StashErrCode( ret, OP_LOCAL ) );
}

sys_handle LocalHandle( handle h )
{
    return( h );
}
