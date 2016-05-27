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


//
// VDMSERV : server end of VDM interface
//

#include <stdlib.h>
#include <string.h>
#include "tinyio.h"
#include "watcom.h"
#include "vdm.h"
#include "trptypes.h"
#include "trperr.h"
#include "packet.h"

typedef unsigned short  USHORT;

int     pipeHdl = -1;
char    pipeName[ MACH_NAME_LEN + PREFIX_LEN + MAX_NAME ];

char    DefLinkName[] = DEFAULT_NAME;

const char *RemoteLink( const char *parms, bool server )
{
    tiny_ret_t          rc;
    tiny_dos_version    ver;
    char                *p;

    if( !server )
        return( "this should never be seen" );
    p = pipeName;
    ver = TinyDOSVersion();
    if( ver.major < 20 ) {
        /* in NT */
        strcpy( p, NT_MACH_NAME );
        p += MACH_NAME_LEN;
    }
    strcpy( p, PREFIX );
    p += PREFIX_LEN;
    if( *parms == '\0' ) {
        strcpy( p, DefLinkName );
    } else if( ValidName( parms ) ) {
        strcpy( p, parms );
    } else {
        return( TRP_ERR_invalid_server_name );
    }
    pipeHdl = -1;
    /*
        Since we can't create the pipe ourselves, we can't reserve the
        name.  We can at least check that there isn't a pipe with the
        name already.  We could open a file in TMP or something... but
        there's no reason for TMP to point to the same place in all
        DOS boxes.  The result is that it's possible for 2 servers to
        have the same name if neither are connected to a client.
    */
    rc = TinyOpen( pipeName, TIO_READ_WRITE );
    if( TINY_ERROR( rc ) ) {
        if( ver.major >= 20 ) {
            /* in OS/2 */
            if( TINY_INFO( rc ) == 5 ) {
                return( TRP_ERR_server_name_already_in_use );
            }
        }
    } else {
        pipeHdl = TINY_INFO( rc );
    }
    return( NULL );
}


bool RemoteConnect( void )
{
    tiny_ret_t  rc;

    if( pipeHdl == -1 ) {
        rc = TinyOpen( pipeName, TIO_READ_WRITE );
        if( TINY_ERROR( rc ) ) {
            /*
                At this point if TINY_INFO( rc ) == 5 then we know that there's
                another server with the same name.  But we have no way of
                indicating this.
            */
            return( false );
        }
        pipeHdl = TINY_INFO( rc );
    }
    return( true );
}


trap_retval RemoteGet( void *data, trap_elen len )
{
    unsigned_16     incoming;
    trap_elen  got;
    trap_elen  ret;

    len = len;
    TinyRead( pipeHdl, &incoming, sizeof( incoming ) );
    ret = incoming;
    while( incoming != 0 ) {
        got = TinyRead( pipeHdl, data, incoming );
        data = (char *)data + got;
        incoming -= got;
    }
    return( ret );
}


trap_retval RemotePut( void *data, trap_elen len )
{
    unsigned_16 outgoing;

    outgoing = len;
    TinyWrite( pipeHdl, &outgoing, sizeof( outgoing ) );
    if( len > 0 ) {
        TinyWrite( pipeHdl, data, len );
    }
    return( len );
}


void RemoteDisco( void )
{
    TinyClose( pipeHdl );
    pipeHdl = -1;
}


void RemoteUnLink( void )
{
}
