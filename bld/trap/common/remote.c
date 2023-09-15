/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Remote trap file request.
*
****************************************************************************/


//#define DEBUG_TRAP
#include "trapdbg.h"

#include <string.h>
#include "trpcore.h"
#include "trpcomm.h"
#include "trperr.h"
#include "packet.h"


static trap_retval DoRequest( void )
{
    trap_elen   left;
    trap_retval result;
    trap_elen   i;
    trap_elen   piece;

    _DBG_EnterFunc( "DoRequest" );
    StartPacket();
    if( Out_Mx_Num == 0 ) {
        /* Tell the server we're not expecting anything back */
        TRP_REQUEST( In_Mx_Ptr ) |= REQ_WANT_RETURN;
    }
    for( i = 0; i < In_Mx_Num; ++i ) {
        AddPacket( In_Mx_Ptr[i].ptr, In_Mx_Ptr[i].len );
    }
    TRP_REQUEST( In_Mx_Ptr ) &= ~REQ_WANT_RETURN;
    result = PutPacket();
    if( result != REQUEST_FAILED ) {
        result = 0;
        if( Out_Mx_Num != 0 ) {
            result = GetPacket();
            if( result != REQUEST_FAILED ) {
                left = result;
                for( i = 0; i < Out_Mx_Num; i++ ) {
                    if( left > Out_Mx_Ptr[i].len ) {
                        piece = Out_Mx_Ptr[i].len;
                    } else {
                        piece = left;
                    }
                    RemovePacket( Out_Mx_Ptr[i].ptr, piece );
                    left -= piece;
                    if( left == 0 ) {
                        break;
                    }
                }
            }
        }
    }
    _DBG_ExitFunc( "DoRequest" );
    return( result );
}


static trap_retval ReqRemoteConnect( void )
{
    connect_ret     *connect;
    char            *data;
    trap_elen       len;
    trap_elen       max;

    _DBG_EnterFunc( "ReqRemoteConnect" );
    connect = GetOutPtr( 0 );
    data = GetOutPtr( sizeof( connect_ret ) );
    if( !RemoteConnect() ) {
        strcpy( data, TRP_ERR_CANT_CONNECT );
        _DBG_WriteErr( "!RemoteConnect" );
        _DBG_ExitFunc( "AccRemoteConnect" );
        return( sizeof( connect_ret ) + sizeof( TRP_ERR_CANT_CONNECT ) );
    }
    len = DoRequest();
    if( data[0] != '\0' ) {
        RemoteDisco();
    }
    max = MaxPacketSize();
    if( connect->max_msg_size > max ) {
        connect->max_msg_size = max;
    }
    _DBG_ExitFunc( "ReqRemoteConnect" );
    return( len );
}

static void ReqRemoteDisco( void )
{
    _DBG_EnterFunc( "ReqDisco" );
    DoRequest();
    RemoteDisco();
    _DBG_ExitFunc( "ReqDisco" );
}

static void ReqRemoteResume( void )
{
    _DBG_EnterFunc( "ReqResume" );
    while( !RemoteConnect() )
        {}
    DoRequest();
    _DBG_ExitFunc( "ReqResume" );
}

trap_version TRAPENTRY TrapInit( const char *parms, char *err, bool remote )
{
    trap_version    ver;
    const char      *error;
    bool            fix_minor;

    /* unused parameters */ (void)remote;

    _DBG_EnterFunc( "TrapInit" );
    ver.remote = true;
    fix_minor = false;
    if( *parms == '!' ) {
        ++parms;
        if( *parms != '!' ) {
            fix_minor = true;
        }
    }
    error = RemoteLink( parms, false );
    if( error != NULL ) {
        strcpy( err, error );
    } else {
        err[0] = '\0';
    }
    ver.major = TRAP_VERSION_MAJOR;
    ver.minor = fix_minor ? OLD_TRAP_VERSION_MINOR : TRAP_VERSION_MINOR;
    _DBG_ExitFunc( "TrapInit" );
    return( ver );
}

trap_retval TRAPENTRY TrapRequest( trap_elen num_in_mx, in_mx_entry_p mx_in, trap_elen num_out_mx, mx_entry_p mx_out )
{
    trap_retval     result;

    _DBG_EnterFunc( "TrapAccess" );
    _DBG_Writeln( _DBG_Request( TRP_REQUEST( mx_in ) ) );
    In_Mx_Num = num_in_mx;
    Out_Mx_Num = num_out_mx;
    In_Mx_Ptr = mx_in;
    Out_Mx_Ptr = mx_out;

    switch( TRP_REQUEST( mx_in ) ) {
    case REQ_CONNECT:
        result = ReqRemoteConnect();
        break;
    case REQ_DISCONNECT:
    case REQ_SUSPEND:
        ReqRemoteDisco();
        result = 0;
        break;
    case REQ_RESUME:
        ReqRemoteResume();
        result = 0;
        break;
    default:
        result = DoRequest();
        break;
    }
    _DBG_ExitFunc( "TrapAccess" );
    return( result );
}

void TRAPENTRY TrapFini( void )
{
    _DBG_EnterFunc( "TrapFini" );
    RemoteUnLink();
    _DBG_ExitFunc( "TrapFini" );
}
