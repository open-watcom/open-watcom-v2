/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
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
#include "trptypes.h"
#include "packet.h"


char                PackBuff[0x400];

static trap_elen    PackInd = 0;

#if defined( SERVER )

trap_retval PutBuffPacket( void *buff, trap_elen len )
{
    trap_retval result;

    _DBG_Writeln( "in PutBuffPacket()" );
    result = RemotePut( buff, len );
    PackInd = 0;
    return( result );
}

void *GetPacketBuffPtr( void )
{
    return( &PackBuff[PackInd] );
}

#else

void StartPacket( void )
{
    _DBG_Writeln( "in StartPacket()" );
    PackInd = 0;
}

trap_retval PutPacket( void )
{
    trap_retval result;

    _DBG_Writeln( "in PutPacket()" );
    result = RemotePut( PackBuff, PackInd );
    PackInd = 0;
    return( result );
}

void AddPacket( const void *ptr, trap_elen len )
{
    if( ( len + PackInd ) > sizeof( PackBuff ) ) {
        len = sizeof( PackBuff ) - PackInd;
    }
    memcpy( &PackBuff[PackInd], ptr, len );
    PackInd += len;
}

void RemovePacket( void *ptr, trap_elen len )
{
    if( ( len + PackInd ) > sizeof( PackBuff ) ) {
        len = sizeof( PackBuff ) - PackInd;
    }
    memcpy( ptr, &PackBuff[PackInd], len );
    PackInd += len;
}

#endif

trap_retval GetPacket( void )
{
    _DBG_Writeln( "in GetPacket()" );
    PackInd = 0;
    return( RemoteGet( PackBuff, sizeof( PackBuff ) ) );
}

trap_elen MaxPacketSize( void )
{
    return( sizeof( PackBuff ) );
}
