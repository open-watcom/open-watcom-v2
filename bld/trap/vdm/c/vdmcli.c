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
// VDMCLI : client (trapfile) end of VDM interface
//

#include <string.h>

#define INCL_DOSNMPIPES
#define INCL_DOSPROCESS
#include <wos2.h>
#define INCL_NOXLATE_DOS16
#include "watcom.h"
#include "trperr.h"
#include "trptypes.h"
#include "packet.h"
#include "namepipe.h"
#include "vdm.h"

HPIPE   pipeHdl;

const char *RemoteLink( const char *parms, bool server )
{
    APIRET      rc;
    char        buf[ PREFIX_LEN + MAX_NAME + 1 ];

    if( server )
        return( "this should never be seen" );
    strcpy( buf, PREFIX );
    if( *parms == '\0' ) {
        strcpy( buf + PREFIX_LEN, DEFAULT_NAME );
    } else if( ValidName( parms ) ) {
        strcpy( buf + PREFIX_LEN, parms );
    } else {
        return( TRP_ERR_invalid_server_name );
    }
    rc = DosMakeNmPipe( buf, &pipeHdl,
        NP_NOINHERIT | NP_NOWRITEBEHIND | NP_ACCESS_DUPLEX,
        NP_NOWAIT | NP_READMODE_BYTE | NP_TYPE_BYTE | 1,
        MAX_TRANS, MAX_TRANS, 0 );
    if( rc != 0 ) {
        /* the bseerr in watcom\h doesn't have ERROR_PIPE_BUSY in it */
        if( rc == 231 ) 
            return( TRP_ERR_server_name_already_in_use );
        return( TRP_ERR_unable_to_access_server );
    }
    return( NULL );
}


bool RemoteConnect( void )
{
    APIRET      rc;
    int         try;

    /*
        We initially opened the pipe non-blocking so that we can poll
        to see if the server has connected to the pipe.
    */
    for( try = 0; try < 25; ++try ) {
        rc = DosConnectNmPipe( pipeHdl );
        if( rc == 0 ) {
            /*
                Now that the server has connected we have to set the pipe
                to blocking mode.
            */
            rc = DosSetNmPHandState( pipeHdl, NP_WAIT | NP_READMODE_BYTE );
            if( rc != 0 ) {
                DosDisConnectNmPipe( pipeHdl );
                return( FALSE );
            }
            return( TRUE );
        }
        DosSleep( 200 );
    }
    return( FALSE );
}


trap_retval RemoteGet( void *data, trap_elen len )
{
    unsigned_16 incoming;
    USHORT      bytes_read;
    unsigned    ret;

    len = len;
    DosRead( pipeHdl, &incoming, sizeof( incoming ), &bytes_read );
    ret = incoming;
    while( incoming != 0 ) {
        DosRead( pipeHdl, data, incoming, &bytes_read );
        data = (char *)data + bytes_read;
        incoming -= bytes_read;
    }
    return( ret );
}


trap_retval RemotePut( void *data, trap_elen len )
{
    unsigned_16 outgoing;
    USHORT      bytes_written;

    outgoing = len;
    DosWrite( pipeHdl, &outgoing, sizeof( outgoing ), &bytes_written );
    if( len > 0 ) {
        DosWrite( pipeHdl, data, len, &bytes_written );
    }
    return( len );
}


void RemoteDisco( void )
{
    APIRET      rc;
    USHORT      bytes_read;
    char        tmp[ MAX_TRANS ];

    /*
        Before disconnecting we must wait until the server has closed
        their connection.  Otherwise we might throw away the last message
        in the pipe (usually the message that says 'close the connection').
    */
    do {
        rc = DosRead( pipeHdl, tmp, MAX_TRANS, &bytes_read );
    } while( rc == 0 && bytes_read != 0 );
    DosDisConnectNmPipe( pipeHdl );
}


void RemoteUnLink( void )
{
    DosClose( pipeHdl );
}
