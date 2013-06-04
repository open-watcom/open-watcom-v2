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
* Description:  Client (trapfile) end of VDM remote debug interface.
*
****************************************************************************/


#include <string.h>
#include <stddef.h>

#define INCL_DOSNMPIPES
#define INCL_DOSPROCESS
#define INCL_DOSERRORS
#include <os2.h>
#include "watcom.h"
#include "namepipe.h"
#include "vdm.h"
#include "trptypes.h"
#include "trperr.h"
#include "packet.h"

HPIPE   pipeHdl;

char *RemoteLink( char *config, bool server )
{
    APIRET      rc;
    char        buf[PREFIX_LEN + MAX_NAME + 1];

    if( server )
        return "this should never be seen";
    strcpy( buf, PREFIX );
    if( *config == 0 ) {
        strcpy( buf + PREFIX_LEN, DEFAULT_NAME );
    } else if( ValidName( config ) ) {
        strcpy( buf + PREFIX_LEN, config );
    } else {
        return( TRP_ERR_invalid_server_name );
    }
    rc = DosCreateNPipe( buf, &pipeHdl,
        NP_NOINHERIT | NP_NOWRITEBEHIND | NP_ACCESS_DUPLEX,
        NP_NOWAIT | NP_READMODE_BYTE | NP_TYPE_BYTE | 1,
        MAX_TRANS, MAX_TRANS, 0 );
    if( rc != 0 ) {
        if( rc == ERROR_PIPE_BUSY )
            return( TRP_ERR_server_name_already_in_use );
        return( TRP_ERR_unable_to_access_server );
    }
    return( NULL );
}


bool RemoteConnect(void)
{
    APIRET      rc;
    int         try;

    /*
        We initially opened the pipe non-blocking so that we can poll
        to see if the server has connected to the pipe.
    */
    for (try = 0; try < 25; ++try) {
        rc = DosConnectNPipe(pipeHdl);
        if (rc == 0) {
            /*
                Now that the server has connected we have to set the pipe
                to blocking mode.
            */
            rc = DosSetNPHState(pipeHdl, NP_WAIT | NP_READMODE_BYTE);
            if (rc != 0) {
                DosDisConnectNPipe(pipeHdl);
                return( FALSE );
            }
            return( TRUE );
        }
        DosSleep(200);
    }
    return( FALSE );
}


trap_retval RemoteGet( byte *data, trap_elen length )
{
    unsigned_16 incoming;
    ULONG       bytes_read;
    ULONG       ret;

    length = length;
    DosRead( pipeHdl, &incoming, sizeof( incoming ), &bytes_read );
    ret = incoming;
    while( incoming != 0 ) {
        DosRead( pipeHdl, data, incoming, &bytes_read );
        data += bytes_read;
        incoming -= bytes_read;
    }
    return ret;
}


trap_retval RemotePut( byte *data, trap_elen length )
{
    unsigned_16 outgoing;
    ULONG       bytes_written;

    outgoing = length;
    DosWrite( pipeHdl, &outgoing, sizeof( outgoing ), &bytes_written );
    if( length > 0 ) {
        DosWrite( pipeHdl, data, length, &bytes_written );
    }
    return length ;
}


void RemoteDisco(void)
{
    APIRET      rc;
    ULONG       bytes_read;
    char        tmp[MAX_TRANS];

    /*
        Before disconnecting we must wait until the server has closed
        their connection.  Otherwise we might throw away the last message
        in the pipe (usually the message that says 'close the connection').
    */
    do {
        rc = DosRead(pipeHdl, tmp, MAX_TRANS, &bytes_read);
    } while (rc == 0 && bytes_read != 0);
    DosDisConnectNPipe(pipeHdl);
}


void RemoteUnLink(void)
{
    DosClose(pipeHdl);
}
