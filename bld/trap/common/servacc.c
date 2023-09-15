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
* Description:  Remote server request router.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "trpld.h"
#include "trpcore.h"
#include "trpcomm.h"
#include "trperr.h"
#include "packet.h"
#include "servio.h"
#include "nothing.h"


trap_version     TrapVersion;

char    RWBuff[0x400];

static in_mx_entry  In[1];
static mx_entry     Out[1];

static void AccTrap( bool want_return )
{
    if( want_return ) {
        PutBuffPacket( RWBuff, TrapAccess( 1, &In[0], 1, &Out[0] ) );
    } else {
        TrapAccess( 1, &In[0], 0, NULL );
    }
}

static bool AccConnect( void )
{
    connect_req         *acc;
    char                *data;
    connect_ret         *ret;
    trap_elen           max;
    trap_elen           len;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    data = GetOutPtr( sizeof( *ret ) );
    if( acc->ver.major != TrapVersion.major || acc->ver.minor > TrapVersion.minor ) {
        strcpy( data, TRP_ERR_WRONG_SERVER_VERSION );
        PutBuffPacket( RWBuff, sizeof( *acc ) + sizeof( TRP_ERR_WRONG_SERVER_VERSION ) );
    } else {
        len = TrapAccess( 1, &In[0], 1, &Out[0] );
        max = MaxPacketSize();
        if( max > sizeof( RWBuff ) )
            max = sizeof( RWBuff );
        if( ret->max_msg_size > max )
            ret->max_msg_size = max;
        CONV_LE_16( ret->max_msg_size );
        PutBuffPacket( RWBuff, len );
    }
    if( data[0] != '\0' ) {
        ServError( data );
        return( false );
    }
    return( true );
}


static void AccLoadProg( void )
{
    char            *data;

    data = GetInPtr( sizeof( prog_load_req ) );
    AccTrap( true );
}

bool Session( void )
{
    unsigned    req;
    bool        want_return;

    Out[0].len = sizeof( RWBuff );
    Out[0].ptr = RWBuff;
    for( ;; ) {
#ifdef __WINDOWS__
        NothingToDo();
#endif
        In[0].len = GetPacket();
        In[0].ptr = GetPacketBuffPtr();
        In_Mx_Ptr = &In[0];
        Out_Mx_Ptr = &Out[0];
        In_Mx_Num = 1;
        Out_Mx_Num = 1;
        req = TRP_REQUEST( In );
        if( req & REQ_WANT_RETURN ) {
            req &= ~REQ_WANT_RETURN;
            want_return = false;
        } else {
            want_return = true;
        }
        TRP_REQUEST( In ) = req;
        switch( req ) {
        case REQ_CONNECT:
            if( !AccConnect() )
                return( true );
            break;
        case REQ_DISCONNECT:
            AccTrap( false );
            return( true );
        case REQ_SUSPEND:
            AccTrap( false );
            RemoteDisco();
            for(;;) {
                while( !RemoteConnect() )
                    {}
                if( !Session() )
                    break;
                RemoteDisco();
            }
            break;
        case REQ_RESUME:
            AccTrap( false );
            return( false );
        case REQ_PROG_LOAD:
            AccLoadProg();
            break;
        default:
            AccTrap( want_return );
            break;
        }
    }
}
