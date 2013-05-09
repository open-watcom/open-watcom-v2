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


//#define DEBUG_TRAP
#include "trapdbg.h"

#include <string.h>
#include "trpimp.h"
#include "packet.h"

static char         PackBuff[0x400];
static trap_elen    PackInd = 0;

void StartPacket( void )
{
    _DBG_Writeln( "in StartPacket()" );
    PackInd = 0;
}

trap_elen PutPacket( void )
{
    trap_elen   rc;

    _DBG_Writeln( "in PutPacket()" );
    rc = RemotePut( PackBuff, PackInd );
    PackInd = 0;
    return( rc );
}

trap_elen PutBuffPacket( trap_elen len, void *buff )
{
    trap_elen   rc;

    _DBG_Writeln( "in PutBuffPacket()" );
    rc = RemotePut( buff, len );
    PackInd = 0;
    return( rc );
}

void AddPacket( trap_elen len, void *ptr )
{
    if( ( len + PackInd ) > sizeof( PackBuff ) ) {
        len = sizeof( PackBuff ) - PackInd;
    }
    memcpy( &PackBuff[PackInd], ptr, len );
    PackInd += len;
}

trap_elen GetPacket( void )
{
    _DBG_Writeln( "in GetPacket()" );
    PackInd = 0;
    return( RemoteGet( PackBuff, sizeof( PackBuff ) ) );
}

void RemovePacket( trap_elen len, void *ptr )
{
    if( ( len + PackInd ) > sizeof( PackBuff ) ) {
        len = sizeof( PackBuff ) - PackInd;
    }
    memcpy( ptr, &PackBuff[PackInd], len );
    PackInd += len;
}

char *GetPacketBuffPtr( void )
{
    return( &PackBuff[PackInd] );
}

trap_elen MaxPacketSize( void )
{
    return( sizeof( PackBuff ) );
}
