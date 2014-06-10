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


#include <string.h>
#include <ctype.h>

#define INCL_DOSPROCESS
#define INCL_DOSSEMAPHORES
#define INCL_DOSERRORS
#include "os2.h"

#include "ipxcalls.h"
#include "spxcalls.h"
#include "spxerror.h"
#include "nwcalls.h"
#include "watcom.h"
#include "trptypes.h"
#include "trperr.h"
#include "packet.h"

#define FOREVER                 0x7FFFFFFF

#define _IPX_ASSIGNADDR( a, b ) \
                                a##Net = b##Net; \
                                AssignArray( a##Node, b##Node ); \
                                a##Socket = b##Socket

//#define InUse( ecb ) ( ( (ecb).status != 0 && (ecb).status != SPX_SUCCESSFUL ) )
#define InUse( ecb ) ( ( (ecb).status > 0x3000 && (ecb).status <= 0x3fff ) || (ecb).status == 0x1001)

#include "ipxstuff.h"

#ifdef DEBUG

void putstring( char *str )
{
    unsigned bytes;

    while( *str ) {
        DosWrite( 1, str, 1, &bytes );
        ++str;
    }
}

static char hexbuff[80];

char * hex( unsigned long num )
{
    char *p;

    p = &hexbuff[79];
    *p = 0;
    if( num == 0 ) {
      *--p = '0';
      return( p );
    }
    while( num != 0 ) {
        *--p = "0123456789abcdef"[ num & 15 ];
        num >>= 4;
    }
    return( p );
}

void puthex( unsigned long x )
{

    putstring( hex( x ) );
}

void putrc( char *func, WORD rc )
{
    if( rc == 0 ) return;
    putstring( func );
    putstring( " returned " );
    puthex( rc );
    putstring( "\r\n" );
}


void put1( byte *c )
{
    if( *c < 0x10 )
        putstring( "0" );
    puthex( *c );
}

void put2( byte *c )
{
    put1( c );
    put1( c+1 );
}

void put4( byte *c )
{
    put2( c );
    put2( c+2 );
}

void put6( byte *c )
{
    put4( c );
    put2( c+4 );
}


void putnetaddr( SPX_HEADER *l ) {

    putstring( " source" );
    putstring( " net " );
    put4( (void *)&l->sourceNet );
    putstring( " node " );
    put6( (void *)&l->sourceNode );
    putstring( " socket " );
    puthex( l->sourceSocket );
    putstring( "\r\ndestination" );
    putstring( " net " );
    put4( (void *)&l->destNet );
    putstring( " node " );
    put6( (void *)&l->destNode );
    putstring( " socket " );
    puthex( (void *)&l->destSocket );
    putstring( "\r\n" );
}

void putconnstatus( WORD conn )
{
    SPX_SESSION stat;

    putstring( "Connection " );
    puthex( conn );
    putstring( " status " );
    if( SpxGetConnectionStatus( conn, &stat ) != 0 ) {
        putstring( "unknown" );
    } else {
        puthex( stat.sStatus );
        putstring( " id src " );
        puthex( stat.sSourceConnectID );
        putstring( " id dst " );
        puthex( stat.sDestConnectID );
    }
    putstring( "\r\n" );
}

#else

#define putrc( x,y )
#define putnetaddr( x )
#define puthex( x )
#define putstring( x )
#define putnetaddr( x )
#define putconnstatus( x )

#endif

#define NUM_REC_BUFFS   5

SPX_HEADER       SendHead;
SPX_HEADER       RecHead[ NUM_REC_BUFFS ];
SPX_HEADER       ConnHead;

SPX_ECB          SendECB;
SPX_ECB          ConnECB;
SPX_ECB          RecECB[ NUM_REC_BUFFS ];

char            Buffer[NUM_REC_BUFFS][MAX_DATA_SIZE];

USHORT          Connection;
USHORT          IPXSocket;
USHORT          SPXSocket;
USHORT          PartnerSPXSocket;
char            Listening;

struct {
        USHORT          checkSum;
        USHORT          packetLen;
        UCHAR           transportControl;
        UCHAR           packetType;
        ULONG           destNet;
        UCHAR           destNode[6];
        USHORT          destSocket;
        ULONG           sourceNet;
        UCHAR           sourceNode[6];
        USHORT          sourceSocket;
        USHORT          infoType;               /* high-low */
        USHORT          serverType;             /* high-low */
        UCHAR           name[48];
        LONG            addrNet;
        UCHAR           addrNode[6];
        USHORT          addrSocket;
        USHORT          intermediateNetworks;   /* high-low */
} SAPHead;

IPX_ECB          SAPECB;
IPX_HEADER       ServHead;
IPX_ECB          ServECB;
IPX_HEADER       RespHead;
IPX_ECB          RespECB;
ULONG           RecvSem = 0;
ULONG           SendSem = 0;

#define IPXRelinquishControl()  DosSleep( 1 )


static trap_retval DoRemoteGet( byte *rec, trap_elen len )
{
    int         i;
    int         p;
    unsigned    recvd;
    unsigned    got;

putstring( "RemoteGet\r\n" );
putconnstatus( Connection );

    len = len;
    recvd = 0;
    for( ;; ) {
        DosSemSet( &RecvSem );
        i = NUM_REC_BUFFS-1;
        p = -1;
        for( ;; ) {
            if( i < 0 ) {
                if( p != -1 ) break;
                DosSemWait( &RecvSem, 1000 );
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
        got = _SWAPINT( RecHead[p].packetLen ) - sizeof( RecHead[p] );
        _fmemcpy( rec, Buffer[p], got );
        recvd += got;
        PostAListen( p );
        if( got != MAX_DATA_SIZE ) break;
        rec += got;
    }

putstring( "Done RemoteGet\r\n" );
    return( recvd );
}

static trap_retval DoRemotePut( byte *snd, trap_elen len )
{
    WORD        rc;

putstring( "RemotePut\r\n" );
putconnstatus( Connection );
    if( len == 0 ) {
        _INITSPXECB( Send, 1, NULL, 0 );
    } else {
        _INITSPXECB( Send, 2, (char *)snd, len );
    }
    SendECB.hsem = (HSEM) &SendSem;
    SendHead.connectionCtl |= 0x10;
    SendHead.packetLen = _SWAPINT( sizeof( SendHead ) + len );
    DosSemSet( &SendSem );
    rc = SpxSendSequencedPacket( Connection, &SendECB );
    putrc( "SPXSendSequencedPacket", rc );

    for( ;; ) {
        /*
            I don't know what the 0x1001 status is - I can't find it
            in any documentation, but if we don't wait for it to clear,
            things mess up badly.
        */
        if( SendECB.status != SPX_SUCCESSFUL && !InUse( SendECB ) && SendECB.status != 0x1001 )
            break;
        rc = DosSemWait( &SendSem, 1000 );
        if( rc != ERROR_SEM_TIMEOUT ) {
            break;
        }
    }
    SendECB.hsem = 0;
putstring( "Done RemotePut\r\n" );
    return( len );
}

trap_retval RemoteGet( byte *rec, trap_elen len )
{
    return( DoRemoteGet( rec, len ) );
}

trap_retval RemotePut( byte *snd, trap_elen len )
{
    while( len >= MAX_DATA_SIZE ) {
        if( DoRemotePut( snd, MAX_DATA_SIZE ) == REQUEST_FAILED ) {
            return( REQUEST_FAILED );
        }
        snd += MAX_DATA_SIZE;
        len -= MAX_DATA_SIZE;
    }
    if( DoRemotePut( snd, len ) == REQUEST_FAILED ) {
        return( REQUEST_FAILED );
    }
    return( len );
}

static void PostAListen( int i )
{
    WORD        rc;

putstring( "Posting RecECB[i]\r\n" );
    _INITECB( RecECB[i], RecHead[i], 2, SPX )
    RecECB[i].hsem = (HSEM) &RecvSem;
    RecECB[i].fragList[1].fragAddress = &Buffer[i];
    RecECB[i].fragList[1].fragSize = sizeof( Buffer[i] );
    rc = SpxListenForConnectionPacket( Connection, &RecECB[i] );
    putrc( "SPXListenForConnectionPacket", rc );
}

static void PostListens( void )
{
    int         i;

    DosSleep( 0 );
    for( i = NUM_REC_BUFFS-1; i >= 0; --i ) {
        if( !InUse( RecECB[i] ) ) {
            PostAListen( i );
        }
    }
    /* make sure other side get the listens up */
    DosSleep( 100 );
}


bool RemoteConnect( void )
{
    WORD        rc;
#ifdef SERVER
    if( !Listening ) {
        _INITSPXECB( Conn, 1, 0, 0 );
        rc = SpxListenForConnection( SPXSocket, &ConnECB, 0, 0, &Connection );
        putrc( "SPXListenForConnection", rc );
        Listening = 1;
    } else if( !InUse( ConnECB ) ) {
        PostListens();
	return( TRUE );
    }
    IPXRelinquishControl();
#else
putstring( "RemoteConnect\r\n" );
    _INITSPXECB( Send, 1, NULL, 0 );
    if( ( rc = SpxEstablishConnection( SPXSocket, &SendECB, 0, 0x10, &Connection ) ) == 0 ) {
        int i;

        for( i = 1; i < 20; ++i ) {
putstring( "RemoteConnect loop\r\n" );
            if( !InUse( SendECB ) ) {
                PostListens();
putstring( "RemoteConnect loop done\r\n" );
                return( TRUE );
            }
            DosSleep( 100 );        /* 20 * 100 is approx 2 seconds */
        }
putstring( "RemoteConnect aborting\r\n" );
        rc = SpxAbortConnection( Connection );
        putrc( "SPXAbortConnection", rc );
    } else {
        putrc( "SPXEstablishConnection", rc );
    }
#endif
    return( FALSE );
}

void RemoteDisco( void )
{
    int         i;

    DosSleep( 500 );    /* make sure last packet gets sent */
    Listening = 0;
    _INITSPXECB( Conn, 1, 0, 0 );
    if( SpxTerminateConnection( Connection, &ConnECB ) == 0 ) {
        while( InUse( ConnECB ) ) IPXRelinquishControl();
    }
    for( i = NUM_REC_BUFFS-1; i >= 0; --i ) {
        if( InUse( RecECB[i] ) ) {
            SpxCancelPacket( &RecECB[i] );
        }
    }
    if( InUse( SendECB ) ) {
        SpxCancelPacket( &SendECB );
    }
    for( i = NUM_REC_BUFFS-1; i >= 0; --i ) {
        ZeroArray( RecHead[i] );
        ZeroArray( RecECB[i] );
    }
    ZeroArray( ConnHead );
    ZeroArray( SendHead );
    ZeroArray( ConnECB );
    ZeroArray( SendECB );
}

#ifdef SERVER

volatile enum {
    DIE_WANT            = 0x01,
    DIE_RESPOND         = 0x02,
    DIE_BROADCAST       = 0x04
} ServerDie = 0;

#define STACKSIZE 2048

char    RespondStack[ STACKSIZE ];

static void __far Respond( void )
{
    char        dummy;

    ServECB.fragCount = 2;
    ServECB.fragList[1].fragAddress = &dummy;
    ServECB.fragList[1].fragSize = sizeof( dummy );
    DosSetPrty( PRTYS_THREAD, PRTYC_NOCHANGE, +1, 0 );
    for( ;; ) {
        if( IpxReceive( IPXSocket, ~0UL, &ServECB ) == 0 ) {
            _IPX_ASSIGNADDR( RespHead.dest, ServHead.source );
            AssignArray( RespECB.immediateAddress, ServECB.immediateAddress );
            RespECB.fragList[1].fragAddress = &SPXSocket;
            IpxSend( IPXSocket, &RespECB );
        }
        if( ServerDie ) {
            ServerDie |= DIE_RESPOND;
            DosExit( EXIT_THREAD, 0 );
        }
    }
}


char    BroadcastStack[ STACKSIZE ];
ULONG   BroadCastStop = 0;
ULONG   BroadCastStart = 0;

static void __far Broadcast( void )
{
    _INITIPXECB( SAP );
    FillArray( SAPHead.destNode, 0xff );
    SAPHead.destSocket = SAP_SOCKET;
    SAPHead.infoType = _SWAPINT( 0x2 );
    SAPHead.serverType = DBG_SERVER_TYPE;
    IpxGetInternetworkAddress( (PUCHAR)&SAPHead.addrNet );
    SAPHead.addrSocket = IPXSocket;
    SAPHead.intermediateNetworks = _SWAPINT( 0x1 );
    AssignArray( SAPECB.immediateAddress, SAPHead.destNode );
    for( ;; ) {
        IpxSend( IPXSocket, &SAPECB );
        DosSemClear( &BroadCastStart );
        DosSemWait( &BroadCastStop, 60000 );
        if( ServerDie ) {
            SAPHead.intermediateNetworks = _SWAPINT( 0x10 );
            IpxSend( IPXSocket, &SAPECB );
            ServerDie |= DIE_BROADCAST;
            DosExit( EXIT_THREAD, 0 );
        }
    }
}


static bool InitServer( void )
{
    TID tid;

    DosSemSet( &BroadCastStop );
    DosSemSet( &BroadCastStart );
    DosCreateThread( Broadcast, &tid, BroadcastStack + STACKSIZE );
    DosSemWait( &BroadCastStart, -1L );
    DosCreateThread( Respond, &tid, RespondStack + STACKSIZE );
    return( TRUE );
}

#endif

static bool FindPartner( void )
{
    static BYTE segdata[128];
    BYTE        moresegs;
    BYTE        flags;
    WORD        connection;

    if( NWGetPrimaryConnectionID( &connection ) != 0 )
        return( FALSE );
    if( NWReadPropertyValue( connection, &SAPHead.name, DBG_SERVER_TYPE, "NET_ADDRESS", 1, segdata, &moresegs, &flags ) != 0 )
        return( FALSE );
    memcpy( &ServHead.destNet, segdata, 12 );
    if( IpxGetLocalTarget( (PUCHAR)&ServHead.destNet, &ServECB, (PULONG)segdata ) != 0 )
        return( FALSE );
    IpxSend( IPXSocket, &ServECB );
    RespECB.fragList[1].fragAddress = &PartnerSPXSocket;
    if( IpxReceive( IPXSocket, 55*MAX_PARTNER_WAIT, &RespECB ) != 0 )
        return( FALSE );
    _IPX_ASSIGNADDR( SendHead.dest, RespHead.source );
    SendHead.destSocket = PartnerSPXSocket;
    AssignArray( SendECB.immediateAddress, RespECB.immediateAddress );
    return( TRUE );
}

char *RemoteLink( char *name, bool server )
{
    unsigned    i;

    server = server;
putstring( "RemoteLink\r\n" );

    if( name == NULL || *name == '\0' )
        name = "NovLink";
    for( i = 0; i < 47 && *name != '\0'; ++name ) {
        if( strchr( "/\\:;,*?+-", *name ) == NULL ) {
            SAPHead.name[ i++ ] = toupper( *name );
        }
    }
    SAPHead.name[ i ] = '\0';
    if( SpxOpenSocket( &SPXSocket ) != 0 || IpxOpenSocket( &IPXSocket ) != 0 ) {
        return( TRP_ERR_can_not_obtain_socket );
    }
    _INITIPXECB( Serv );
    _INITIPXECB( Resp );
    RespECB.fragCount = 2;
    RespECB.fragList[1].fragSize = sizeof( WORD ); /* for SPXSocket */
#ifdef SERVER
    if( FindPartner() ) {
        RemoteUnLink();
        return( TRP_ERR_server_name_already_in_use );
    }
    if( !InitServer() ) {
        RemoteUnLink();
        return( TRP_ERR_can_not_initialize_server );
    }
#else
    if( !FindPartner() ) {
        RemoteUnLink();
        return( TRP_ERR_no_such_server );
    }
#endif
    return( NULL );
}


void RemoteUnLink( void )
{
#ifdef SERVER
    ServerDie = DIE_WANT;
#endif
    IpxCloseSocket( IPXSocket );
#ifdef SERVER
    DosSemClear( &BroadCastStop );
    if( InUse( ConnECB ) ) {
        SpxCancelPacket( &ConnECB );
    }
    while( ServerDie != (DIE_WANT|DIE_BROADCAST|DIE_RESPOND) )  {
        IPXRelinquishControl();
    }
#endif
    SpxCloseSocket( SPXSocket );
}
