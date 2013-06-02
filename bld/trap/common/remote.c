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
* Description:  Remote trap file request.
*
****************************************************************************/


//#define DEBUG_TRAP
#include "trapdbg.h"

#include <string.h>
#include "trpimp.h"
#include "trperr.h"
#include "packet.h"


static trap_elen DoRequest( void )
{
    trap_elen   left;
    trap_elen   len;
    trap_elen   i;
    trap_elen   piece;

    _DBG_EnterFunc( "DoRequest" );
    StartPacket();
    if( Out_Mx_Num == 0 ) {
        /* Tell the server we're not expecting anything back */
        *(access_req *)In_Mx_Ptr[0].ptr |= 0x80;
    }
    for( i = 0; i < In_Mx_Num; ++i ) {
        AddPacket( In_Mx_Ptr[i].len, In_Mx_Ptr[i].ptr );
    }
    *(access_req *)In_Mx_Ptr[0].ptr &= ~0x80;
    if( PutPacket() == REQUEST_FAILED )
        return( REQUEST_FAILED );
    if( Out_Mx_Num != 0 ) {
        len = GetPacket();
        if( len == REQUEST_FAILED )
            return( REQUEST_FAILED );
        left = len;
        i = 0;
        for( ;; ) {
            if( i >= Out_Mx_Num )
                break;
            if( left > Out_Mx_Ptr[i].len ) {
                piece = Out_Mx_Ptr[i].len;
            } else {
                piece = left;
            }
            RemovePacket( piece, Out_Mx_Ptr[i].ptr );
            i++;
            left -= piece;
            if( left == 0 ) {
                break;
            }
        }
    } else {
        len = 0;
    }
    _DBG_ExitFunc( "DoRequest" );
    return( len );
}


static trap_elen ReqRemoteConnect( void )
{
    connect_ret     *connect;
    char            *data;
    trap_elen       len;
    trap_elen       max;

    _DBG_EnterFunc( "ReqRemoteConnect" );
    connect = GetOutPtr( 0 );
    data = (char *)GetOutPtr( sizeof( connect_ret ) );
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
        ;
    DoRequest();
    _DBG_ExitFunc( "ReqResume" );
}

trap_version TRAPENTRY TrapInit( char *parm, char *error, bool remote )
{
    trap_version    ver;
    char           *err;
    int             fix_minor;

    remote=remote;
    _DBG_EnterFunc( "TrapInit" );
    ver.remote = TRUE;
    fix_minor = 0;
    if( parm != NULL && *parm == '!' ) {
        ++parm;
        if( *parm != '!' ) fix_minor = 1;
    }
    err = RemoteLink( parm, FALSE );
    if( err != NULL ) {
        strcpy( error, err );
    } else {
        error[0] = '\0';
    }
    ver.major = TRAP_MAJOR_VERSION;
    ver.minor = fix_minor ? OLD_TRAP_MINOR_VERSION : TRAP_MINOR_VERSION;
    _DBG_ExitFunc( "TrapInit" );
    return( ver );
}

trap_elen TRAPENTRY TrapRequest( trap_elen num_in_mx, mx_entry_p mx_in, trap_elen num_out_mx, mx_entry_p mx_out )
{
    trap_elen   ret;

    _DBG_EnterFunc( "TrapAccess" );
    _DBG_Writeln( _DBG_Request( *(access_req *)mx_in[0].ptr ) );
    In_Mx_Num = num_in_mx;
    Out_Mx_Num = num_out_mx;
    In_Mx_Ptr = mx_in;
    Out_Mx_Ptr = mx_out;

    switch( *(access_req *)mx_in[0].ptr ) {
    case REQ_CONNECT:
        ret = ReqRemoteConnect();
        break;
    case REQ_DISCONNECT:
    case REQ_SUSPEND:
        ReqRemoteDisco();
        ret = 0;
        break;
    case REQ_RESUME:
        ReqRemoteResume();
        ret = 0;
        break;
    default:
        ret = DoRequest();
        break;
    }
    _DBG_ExitFunc( "TrapAccess" );
    return( ret );
}

void TRAPENTRY TrapFini( void )
{
    _DBG_EnterFunc( "TrapFini" );
    RemoteUnLink();
    _DBG_ExitFunc( "TrapFini" );
}
