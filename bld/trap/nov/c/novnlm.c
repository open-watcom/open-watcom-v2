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


#define static

#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include "bindery.h"
#include "miniproc.h"
#include "ecb.h"

#include "ipxstuff.h"

#include "packet.h"
#include "debugme.h"
#include "trpimp.h"
#include "trperr.h"
#include "trptypes.h"

extern struct ResourceTagStructure              *SocketTag;
extern struct ResourceTagStructure              *TimerTag;
extern struct ResourceTagStructure              *SemaphoreTag;

#define InUse( x )              ( ( (x).status & 0xff ) != 0 )
#define CompletionCode( x )     ( ( (x).status >> 8 ) & 0xFF )
#define Completed( x )          ( (x).status == 0 || (x).status == 0xFFFD )
#define SPXCancelEvent( x )     if( CSPXCancelSessionListen( x ) ) \
                                    CIPXCancelECB( x );

extern  void    NothingToDo(void);

#define NUM_REC_BUFFS   5

SPXHeader       SendHead;
SPXHeader       RecHead[ NUM_REC_BUFFS ];
SPXHeader       ConnHead;

ECB             SendECB;
ECB             ConnECB;
ECB             RecECB[ NUM_REC_BUFFS ];

char            Buffer[NUM_REC_BUFFS][MAX_DATA_SIZE];

LONG            Connection;
LONG            IPXSocket;
LONG            SPXSocket;
LONG            PartnerSPXSocket;
char            Listening;

IPXHeader       ServHead;
ECB             ServECB;
IPXHeader       RespHead;
ECB             RespECB;
char            ServDummy;


typedef struct {
   WORD             SAPPacketType;      /* 2 or 4 */
   WORD             serverType;         /* assigned by Novell */
   BYTE             serverName[48];     /* service name */
   IPXAddress       serverAddress;      /* server internetwork address */
   WORD             interveningNetworks;/* # of networks packet must traverse */
} SAPIdPacket;

typedef struct AdvertisingStruct
   {
   ECB                        AS_ECB;
   IPXHeader                  ASPacket;
   SAPIdPacket                ASServerIDpacket;
} AdvertisingStruct;


struct TimerDataStructure       SAPTimer;
struct TimerDataStructure       Timer;
AdvertisingStruct               SAPStruct;
int                             Tick;

extern void                     IpxGetInternetworkAddress();
extern void                     IpxGetLocalTarget();

static void MyDelay( unsigned amount )
{
    Tick = 0;
    while( Tick < amount ) {
        NothingToDo();
    }
}

static unsigned DoRemoteGet( void *rec, unsigned len )
{
    unsigned    recvd;
    int         i;
    unsigned    got;
    int         p;

    len = len;
_DBG_IPX(("RemoteGet\r\n"));
    recvd = 0;
    for( ;; ) {
        i = NUM_REC_BUFFS-1;
        p = -1;
        for( ;; ) {
            if( i < 0 ) {
                if( p != -1 ) break;
                NothingToDo();
                i = NUM_REC_BUFFS-1;
            }
            if( !InUse( RecECB[i] ) ) {
                if( p == -1
                 || LOWER_SEQ( RecHead[i].sequenceNumber, RecHead[p].sequenceNumber ) ) {
                    p = i;
                }
            }
            --i;
        }
        got = _SWAPINT( RecHead[p].length ) - sizeof( RecHead[p] );
_DBG_IPX(("Got a packet - size=%d\r\n", got));
        memcpy( rec, Buffer[p], got );
        recvd += got;
        PostAListen( p );
        if( got != MAX_DATA_SIZE ) break;
        rec = (unsigned_8 *)rec + got;
    }
    return( recvd );
}

static unsigned DoRemotePut( void *snd, unsigned len )
{
_DBG_IPX(("RemotePut\r\n"));
    _INITSPXECB( Send, 2, snd, len );
    SendHead.connectionControl |= 0x10;
    SendHead.length = _SWAPINT( sizeof( SendHead ) + len );
    CSPXSendSequencedPacket( Connection, &SendECB );
    while( InUse( SendECB ) ) NothingToDo();
    return( len );
}

unsigned RemoteGet( void *rec, unsigned len )
{
    return( DoRemoteGet( rec, len ) );
}

unsigned RemotePut( void *snd, unsigned len )
{
    while( len >= MAX_DATA_SIZE ) {
        if( DoRemotePut( snd, MAX_DATA_SIZE ) == REQUEST_FAILED ) {
            return( REQUEST_FAILED );
        }
        snd = (unsigned_8 *)snd + MAX_DATA_SIZE;
        len -= MAX_DATA_SIZE;
    }
    if( DoRemotePut( snd, len ) == REQUEST_FAILED ) {
        return( REQUEST_FAILED );
    }
    return( len );
}

static void PostAListen( int i )
{
_DBG_IPX(("Posting RecECB[%d]\r\n", i));
    _INITECB( RecECB[i], RecHead[i], 2, SPX );
    RecECB[i].fragmentDescriptor[1].address = Buffer[i];
    RecECB[i].fragmentDescriptor[1].size = sizeof( Buffer[i] );
    CSPXListenForSequencedPacket( &RecECB[i] );
}

static void PostListens()
{
    int         i;

    for( i = NUM_REC_BUFFS-1; i >= 0; --i ) {
        if( !InUse( RecECB[i] ) ) {
            PostAListen( i );
        }
    }
    MyDelay( TICKS_PER_SEC / 5 );
}


char RemoteConnect( void )
{
    PostListens();
    if( !Listening ) {
_DBG_IPX(("Listening for connection\r\n"));
        _INITSPXECB( Conn, 1, 0, 0 );
        CSPXListenForConnection( 0, 0, &Connection, &ConnECB );
        Listening = 1;
    } else if( !InUse( ConnECB ) && Completed( ConnECB ) ) {
_DBG_IPX(("Found a connection\r\n"));
        return( 1 );
    }
    NothingToDo();
    return( 0 );
}

void RemoteDisco( void )
{
    int         i;

_DBG_IPX(("RemoteDisco\r\n"));
    /* make sure last packet gets sent out */
    MyDelay( TICKS_PER_SEC/2 );

    Listening = 0;
    _INITSPXECB( Conn, 1, 0, 0 );
    CSPXTerminateConnection( Connection, &ConnECB );
    for( i = NUM_REC_BUFFS-1; i >= 0; --i ) {
        if( InUse( RecECB[i] ) ) {
            SPXCancelEvent( &RecECB[i] );
        }
    }
    SPXCancelEvent( &SendECB );
    for( i = NUM_REC_BUFFS-1; i >= 0; --i ) {
        RecECB[i].status = 0;
    }
}

void ServRespESR( void )
{
    if( Completed( ServECB ) && !InUse( RespECB )
     && (ServHead.length == _SWAPINT( sizeof( IPXHeader ) )
         || ServHead.length == _SWAPINT( sizeof( IPXHeader ) + 1 ) ) ) {
_DBG_IPX(( "ServRespESR responding\r\n" ));
        RespHead.destination = ServHead.source;
        AssignArray( RespECB.immediateAddress , ServECB.immediateAddress );
        RespECB.fragmentDescriptor[1].address = &SPXSocket;
        CIPXSendPacket( &RespECB );
    }
    CIPXListen( &ServECB );
}


static void cdecl Ticker( struct TimerDataStructure *time )
{
    ++Tick;
    time->TCallBackWaitTime = 1;
    CScheduleInterruptTimeCallBack( time );
}


static void cdecl Advertiser( struct TimerDataStructure *time )
{
    CIPXSendPacket( &SAPStruct.AS_ECB );
    time->TCallBackWaitTime = 60 * TICKS_PER_SEC;
    CScheduleInterruptTimeCallBack( time );
}



static char InitServer( void )
{
    ServECB.fragmentCount = 2;
    ServECB.fragmentDescriptor[1].address = &ServDummy;
    ServECB.fragmentDescriptor[1].size = sizeof( ServDummy );

    ServECB.ESRAddress = &ServRespESR;
    CIPXListen( &ServECB );

    SAPStruct.ASServerIDpacket.serverType = DBG_SERVER_TYPE;
    CIPXGetInternetworkAddress((BYTE*)&SAPStruct.ASServerIDpacket.serverAddress);
    AssignArray( SAPStruct.ASServerIDpacket.serverAddress.socket,
                 ServECB.socketNumber );
    SAPStruct.AS_ECB.ESRAddress = NULL;
    SAPStruct.AS_ECB.socketNumber = SAP_SOCKET_VALUE;
    FillArray( SAPStruct.AS_ECB.immediateAddress, '\xFF' );
    SAPStruct.AS_ECB.fragmentCount = 1;
    SAPStruct.AS_ECB.fragmentDescriptor[0].size = sizeof( IPXHeader ) +
                                      sizeof( SAPIdPacket );
    SAPStruct.AS_ECB.fragmentDescriptor[0].address = &SAPStruct.ASPacket;

    SAPStruct.ASPacket.packetType = 4;
    FillArray( SAPStruct.ASPacket.destination.network, 0 );
    FillArray( SAPStruct.ASPacket.destination.node, '\xFF' );
    {
        LONG        socket;

        socket = SAP_SOCKET;
        AssignArray( SAPStruct.ASPacket.destination.socket, socket );
    }

    SAPStruct.ASServerIDpacket.SAPPacketType = _SWAPINT( 2 );
    SAPStruct.ASServerIDpacket.interveningNetworks = _SWAPINT( 1 );

    CIPXSendPacket( &SAPStruct.AS_ECB );

    SAPTimer.TCallBackProcedure = (void (*)(LONG))Advertiser;
    SAPTimer.TCallBackEBXParameter = (LONG)&SAPTimer;
    SAPTimer.TCallBackWaitTime = 0;
    SAPTimer.TResourceTag = TimerTag;
    CScheduleInterruptTimeCallBack( &SAPTimer );

    return( 1 );
}


int ASCIIZToLenStr( char *lstr, char *string )
{
   int i;

    for( i = 0; i < 255 && string[ i ] != 0; i++ ) {
        lstr[i+1] = string[ i ];
    }
    lstr[ 0 ] = i;
    return( ( i == 255 ) && ( string[ 255 ] != 0 ) );
}

LONG ReadPropertyValue( char *objectName,
                        WORD objectType,
                        char *propertyName,
                        int  segmentNumber,
                        BYTE *propertyValue )
{
    LONG rc;
    LONG objectID;
    BYTE name_buff[48];
    BYTE moreSegmentsT, propertyFlagsT;

    ASCIIZToLenStr( name_buff, objectName );
    MapNameToID( 0, name_buff, objectType, &objectID, NOCHECK );
    ASCIIZToLenStr( name_buff, propertyName );
    rc = ReadProperty( 0, objectID, name_buff, (LONG)segmentNumber,
                       propertyValue, &moreSegmentsT, &propertyFlagsT,
                       CHECK );
    if( rc != 0 ) return( rc );
    return( 0 );
}

static int FindPartner( void )
{
    BYTE        property_value[130];
    LONG        transport_time;

    if( ReadPropertyValue( SAPStruct.ASServerIDpacket.serverName,
                           DBG_SERVER_TYPE, "NET_ADDRESS",
                           1, (BYTE *)&property_value ) != 0 ) return( 0 );
    AssignArray( ServHead.destination, property_value );
    if( CIPXGetLocalTarget( ServHead.destination.network,
                            ServECB.immediateAddress, &transport_time ) != 0 ) {
_DBG_IPX(( "FindPartner -- nobody home\r\n" ));
        return( FALSE );
    }
    RespECB.fragmentDescriptor[1].address = &PartnerSPXSocket;
    CIPXListen( &RespECB );
    CIPXSendPacket( &ServECB );
    while( InUse( ServECB ) ) NothingToDo();
    if( !Completed( ServECB ) ) return( FALSE );
    NothingToDo();
    Tick = 0;
    for( ;; ) {
        if( !InUse( RespECB ) ) {
_DBG_IPX(( "FindPartner -- %s answered\r\n", Completed( RespECB ) ? "someone" : "noone" ));
            return( Completed( RespECB ) );
        }
        NothingToDo();
        if( Tick > MAX_PARTNER_WAIT ) break;
    }
    CIPXCancelECB( &RespECB );
    return( FALSE );
}

char *RemoteLink( char *name, char server )
{
    unsigned    i;


_DBG_IPX(("RemoteLink\r\n"));
    server = server;
    if( name == NULL || *name == '\0' ) name = "NovLink";
    for( i = 0; i < 48 && *name != '\0'; ++name ) {
        if( strchr( "/\\:;,*?+-", *name ) == NULL ) {
            SAPStruct.ASServerIDpacket.serverName[ i++ ] = toupper( *name );
        }
    }
    SAPStruct.ASServerIDpacket.serverName[ i ] = '\0';
    if( CIPXOpenSocketRTag( &SPXSocket, SocketTag ) != 0 ||
        CIPXOpenSocketRTag( &IPXSocket, SocketTag ) != 0 ) {
        return( TRP_ERR_can_not_obtain_socket );
    }

    _INITIPXECB( Serv );
    _INITIPXECB( Resp );
    RespECB.fragmentCount = 2;
    /* for SPXSocket - yes, SPXSocket is a long, but it always goes out on
        the wire as a 16-bit quantity */
    RespECB.fragmentDescriptor[1].size = sizeof( WORD );

    Timer.TCallBackProcedure = (void (*)(LONG))Ticker;
    Timer.TCallBackEBXParameter = (LONG)&Timer;
    Timer.TCallBackWaitTime = 0;
    Timer.TResourceTag = TimerTag;
    CScheduleInterruptTimeCallBack( &Timer );


    if( FindPartner() ) {
        RemoteUnLink();
        return( TRP_ERR_server_name_already_in_use );
    }
    if( !InitServer() ) {
        RemoteUnLink();
        return( TRP_ERR_can_not_initialize_server );
    }
    return( NULL );
}


void RemoteUnLink( void )
{
    CIPXCloseSocket( IPXSocket );
    CIPXCloseSocket( SPXSocket );

    CIPXCancelECB( &ServECB );
    CIPXCancelECB( &RespECB );

    CCancelInterruptTimeCallBack( &Timer );
    CCancelInterruptTimeCallBack( &SAPTimer );

    if (SAPStruct.AS_ECB.status <= 0) {
        SAPStruct.ASServerIDpacket.interveningNetworks = _SWAPINT( 0x10 );
        CIPXSendPacket( &SAPStruct.AS_ECB );
    }
    CIPXCloseSocket( SPXSocket );
    CIPXCloseSocket( IPXSocket );
}
