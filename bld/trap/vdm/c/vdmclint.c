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

//  Modified:   By:             Reason:
//  ---------   ---             -------
//  94/04/20    B.J. Stecher    NT version

#include <string.h>
#include <stddef.h>

#include <windows.h>
#include "watcom.h"
#include "namepipe.h"
#include "vdm.h"
#include "trptypes.h"
#include "trperr.h"
#include "packet.h"

HANDLE  pipeHdl;

#define NT_PREF_LEN     (MACH_NAME_LEN + PREFIX_LEN)

char *RemoteLink( char *config, bool server )
{
    char        buf[ NT_PREF_LEN + MAX_NAME + 1 ];

    if( server )
        return( "this should never be seen" );
    strcpy( buf, NT_MACH_NAME PREFIX );
    if( *config == 0 ) {
        strcpy( buf + NT_PREF_LEN, DEFAULT_NAME );
    } else if( ValidName( config ) ) {
        strcpy( buf + NT_PREF_LEN, config );
    } else {
        return( TRP_ERR_invalid_server_name );
    }
    pipeHdl = CreateNamedPipe( buf,
                    PIPE_ACCESS_DUPLEX | FILE_FLAG_WRITE_THROUGH,
                    PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_NOWAIT,
                    1, MAX_TRANS, MAX_TRANS, 0, NULL );
    if( pipeHdl == INVALID_HANDLE_VALUE ) {
        if( GetLastError() == ERROR_PIPE_BUSY )
            return( TRP_ERR_server_name_already_in_use );
        return( TRP_ERR_unable_to_access_server );
    }
    return( NULL );
}


bool RemoteConnect( void )
{
    DWORD       mode;
    int         try;

    /*
        We initially opened the pipe non-blocking so that we can poll
        to see if the server has connected to the pipe.
    */
    for( try = 0; try < 25; ++try ) {
        SetLastError( 0 );
        ConnectNamedPipe( pipeHdl, NULL );
        if( GetLastError() == ERROR_PIPE_CONNECTED ) {
            /*
                Now that the server has connected we have to set the pipe
                to blocking mode.
            */
            mode = PIPE_READMODE_BYTE | PIPE_WAIT;
            if( !SetNamedPipeHandleState( pipeHdl, &mode, NULL, NULL ) ) {
                DisconnectNamedPipe( pipeHdl );
                return( FALSE );
            }
            return( TRUE );
        }
        Sleep( 200 );
    }
    return( FALSE );
}


trap_retval RemoteGet( byte *data, trap_elen length )
{
    unsigned_16     incoming;
    ULONG           bytes_read;
    trap_elen       ret;

    length = length;
    ReadFile( pipeHdl, &incoming, sizeof( incoming ), &bytes_read, NULL );
    ret = incoming;
    while( incoming != 0 ) {
        ReadFile( pipeHdl, data, incoming, &bytes_read, NULL );
        data += bytes_read;
        incoming -= bytes_read;
    }
    return( ret );
}


trap_retval RemotePut( byte *data, trap_elen length )
{
    unsigned_16 outgoing;
    ULONG       bytes_written;

    outgoing = length;
    WriteFile( pipeHdl, &outgoing, sizeof( outgoing ), &bytes_written, NULL );
    if( length > 0 ) {
        WriteFile( pipeHdl, data, length, &bytes_written, NULL );
    }
    return( length );
}


void RemoteDisco( void )
{
    BOOL        rc;
    DWORD       bytes_read;
    char        tmp[ MAX_TRANS ];

    /*
        Before disconnecting we must wait until the server has closed
        their connection.  Otherwise we might throw away the last message
        in the pipe (usually the message that says 'close the connection').
    */
    do {
        rc = ReadFile( pipeHdl, tmp, sizeof( tmp ), &bytes_read, NULL );
    } while( rc && bytes_read != 0 );
    DisconnectNamedPipe( pipeHdl );
}


void RemoteUnLink( void )
{
    CloseHandle( pipeHdl );
}
