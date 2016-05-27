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
* Description:  DOS NetWare IPX link core.
*
****************************************************************************/


#include <string.h>
#include <ctype.h>
#include <i86.h>
#include "nitipx.h"
#include "tinyio.h"
#include "trptypes.h"
#include "trperr.h"
#include "packet.h"

#include "ipxstuff.h"

#ifdef DEBUG

void putstring( char __far *str )
{
    unsigned bytes;
    extern unsigned _dos_write( int handle, void __far *buffer, unsigned count, unsigned *bytes );

    while( *str ) {
        _dos_write( 1, str, 1, &bytes );
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

void put1( unsigned char *c )
{
    if( *c < 0x10 ) putstring( "0" );
    puthex( *c );
}

void put2( unsigned char *c )
{
    put1( c );
    put1( c+1 );
}

void put4( unsigned char *c )
{
    put2( c );
    put2( c+2 );
}

void put6( unsigned char *c )
{
    put4( c );
    put2( c+4 );
}


void putnetaddr( SPXHeader *l ) {

    putstring( " source" );
    putstring( " net " );
    put4( l->source.network.a );
    putstring( " node " );
    put6( l->source.node.a );
    putstring( " socket " );
    puthex( l->source.socket );
    putstring( "\r\ndestination" );
    putstring( " net " );
    put4( l->destination.network.a );
    putstring( " node " );
    put6( l->destination.node.a );
    putstring( " socket " );
    puthex( l->destination.socket );
    putstring( "\r\n" );
}

void putconnstatus( WORD conn )
{
    CSB stat;

    putstring( "Connection " );
    puthex( conn );
    putstring( " status " );
    if( _SPXGetConnectionStatus( conn, &stat ) != 0 ) {
        putstring( "unknown" );
    } else {
        puthex( stat.connectionStatus );
        putstring( " id src " );
        puthex( stat.sourceConnectionID );
        putstring( " id dst " );
        puthex( stat.destinationConnectionID );
    }
    putstring( "\r\n" );
}

#else

#define putnetaddr( x )
#define puthex( x )
#define putstring( x )
#define putnetaddr( x )
#define putconnstatus( x )

#endif

#define NUM_REC_BUFFS   5

SPXHeader       SendHead;
SPXHeader       RecHead[NUM_REC_BUFFS];
SPXHeader       ConnHead;

ECB             SendECB;
ECB             ConnECB;
ECB             RecECB[NUM_REC_BUFFS];

char            Buffer[NUM_REC_BUFFS][MAX_DATA_SIZE];

WORD            Connection;
WORD            IPXSocket;
WORD            SPXSocket;
WORD            PartnerSPXSocket;
char            Listening;

SIP             SAPHead;
ECB             SAPECB;
IPXHeader       ServHead;
ECB             ServECB;
IPXHeader       RespHead;
ECB             RespECB;
char            ServDummy;

typedef void    (__far *ESRAddr)();

#ifdef __WINDOWS__
DWORD           IPXTaskID;
typedef int     (WINAPI *NOVWINAPI)();
NOVWINAPI       IPXFuncs[ IPX_MAX_FUNCS ];
extern          void SetLinkName( char* );
#endif

ESRAddr         SAPBroadESRAddr;
ESRAddr         SAPWaitESRAddr;
ESRAddr         ServRespESRAddr;

static void IpxWait( void )
{
    //*******************************************************************
    // NOTE:  This call is absolutely necessary.  The DOS call some
    //        how clears out a condition in IPX where IPXRelinquishControl
    //        won't return.
    //*******************************************************************
    extern void clock(void);
    #pragma aux clock = \
        0xb4 0x2c                       /* mov ah,02ch */       \
        0xcd 0x21                       /* int  21h    */       \
        modify [ cx dx ];
    clock();

    _IPXRelinquishControl();
}

#define WaitOn( ecb )   while( (ecb).inUseFlag ) IpxWait()

static void Delay( unsigned timeout )
{
    unsigned    start;

    start = _IPXGetIntervalMarker();
    for( ;; ) {
        _IPXRelinquishControl();
        if( (_IPXGetIntervalMarker() - start) >= timeout ) break;
    }
}

static char WaitTimeout( ECB *ecb, unsigned timeout, int can )
{
    unsigned    start;

    start = _IPXGetIntervalMarker();
    for( ;; ) {
        _IPXRelinquishControl();
        if( ecb->inUseFlag == 0 ) {
            return( ecb->completionCode == 0 );
        }
        if( (_IPXGetIntervalMarker() - start) >= timeout ) {
            if( can ) _IPXCancelEvent( ecb );
            return( 0 );
        }
    }
}

static void PostAListen( int i )
{
    _INITECB( RecECB[i], RecHead[i], 2, SPX );
    RecECB[i].fragmentDescriptor[1].address = &Buffer[i];
    RecECB[i].fragmentDescriptor[1].size = sizeof( Buffer[i] );
    _SPXListenForSequencedPacket( &RecECB[i] );
}

static trap_retval DoRemoteGet( void *data, trap_elen len )
{
    int         i;
    trap_elen   recvd;
    trap_elen   got;
    int         p;

    len = len;
    recvd = 0;
    for( ;; ) {
        i = NUM_REC_BUFFS - 1;
        p = -1;
        for( ;; ) {
            if( i < 0 ) {
                if( p != -1 )
                    break;
                _IPXRelinquishControl();
                i = NUM_REC_BUFFS-1;
            }
            if( !RecECB[i].inUseFlag ) {
                if( p == -1 || LOWER_SEQ( RecHead[i].sequenceNumber, RecHead[p].sequenceNumber ) ) {
                    p = i;
                }
            }
            --i;
        }
        got = _SWAPINT( RecHead[p].length ) - sizeof( RecHead[p] );
        _fmemcpy( data, Buffer[p], got );
        recvd += got;
        PostAListen( p );
        if( got != MAX_DATA_SIZE )
            break;
        data = (char *)data + got;
    }
    return( recvd );
}

static trap_retval DoRemotePut( void *data, trap_elen len )
{
    _INITECB( SendECB, SendHead, 2, SPX );
    SendHead.connectControl |= 0x10;
    SendHead.length = _SWAPINT( sizeof( SendHead ) + len );
    SendECB.fragmentDescriptor[1].address = data;
    SendECB.fragmentDescriptor[1].size = len;
    _SPXSendSequencedPacket( Connection, &SendECB );
    WaitOn( SendECB );
    return( len );
}

trap_retval RemoteGet( void *data, trap_elen len )
{
    return( DoRemoteGet( data, len ) );
}

trap_retval RemotePut( void *data, trap_elen len )
{
    while( len >= MAX_DATA_SIZE ) {
        if( DoRemotePut( data, MAX_DATA_SIZE ) == REQUEST_FAILED ) {
            return( REQUEST_FAILED );
        }
        data = (char *)data + MAX_DATA_SIZE;
        len -= MAX_DATA_SIZE;
    }
    if( DoRemotePut( data, len ) == REQUEST_FAILED ) {
        return( REQUEST_FAILED );
    }
    return( len );
}

static void PostListens( void )
{
    unsigned    i;

    for( i = 0; i < NUM_REC_BUFFS; ++i ) {
        if( !RecECB[i].inUseFlag ) {
            PostAListen( i );
        }
    }
    /* make sure other side gets the listens up */
    Delay( TICKS_PER_SEC/5 );
}

bool RemoteConnect( void )
{
    PostListens();
#ifdef SERVER
    if( !Listening ) {
        _INITSPXECB( Conn, 1, 0, 0 );
        _SPXListenForConnection( 0, 0, &ConnECB );
        Listening = 1;
    } else if( ConnECB.inUseFlag == 0 ) {
        if( ConnECB.completionCode == 0 ) {
            Connection = ConnECB.SPXConnectionID;
            return( true );
        }
    }
    _IPXRelinquishControl();
#else
    _INITECB( SendECB, SendHead, 1, SPX );
    if( _SPXEstablishConnection( 0, 0, &Connection, &SendECB ) == 0 ) {
        if( WaitTimeout( &SendECB, MAX_CONNECT_WAIT, 0 ) ) {
            return( true );
        } else {
            _SPXAbortConnection( Connection );
        }
    }
#endif
    return( false );
}

void RemoteDisco( void )
{
    int         i;

    /* make sure last packet gets sent */
    Delay( TICKS_PER_SEC/2 );

    Listening = 0;
    _INITSPXECB( Conn, 1, 0, 0 );
    _SPXTerminateConnection( Connection, &ConnECB );
#ifdef SERVER
    _IPXCancelEvent( &ConnECB );
#endif
    for( i = NUM_REC_BUFFS - 1; i >= 0; --i ) {
        if( RecECB[i].inUseFlag ) {
            _IPXCancelEvent( &RecECB[i] );
        }
    }
    _IPXCancelEvent( &SendECB );
    _IPXDisconnectFromTarget( &SendHead.destination.network.a[0] );
    for( i = 0; i < NUM_REC_BUFFS; ++i ) {
        ZeroArray( RecHead[i] );
        ZeroArray( RecECB[i] );
    }
    ZeroArray( ConnHead );
    ZeroArray( SendHead );
    ZeroArray( ConnECB );
    ZeroArray( SendECB );
}

void ESRFUNC SAPWait( void )
{
    SAPECB.ESRAddress = SAPBroadESRAddr;
    _IPXSendPacket( &SAPECB );
}

void ESRFUNC SAPBroad( void )
{
    SAPECB.ESRAddress = SAPWaitESRAddr;
    _IPXScheduleIPXEvent( 60*TICKS_PER_SEC, &SAPECB );
}

void ESRFUNC ServResp( void )
{
    if( ServECB.completionCode == 0
        && (ServHead.length == _SWAPINT( sizeof( IPXHeader ) )
         || ServHead.length == _SWAPINT( sizeof( IPXHeader ) + 1 ) )
        && RespECB.inUseFlag == 0 ) {
        RespHead.destination = ServHead.source;
        RespECB.immediateAddress = ServECB.immediateAddress;
        RespECB.fragmentDescriptor[1].address = &SPXSocket;
        _IPXSendPacket( &RespECB );
    }
    _IPXListenForPacket( &ServECB );
}

#ifdef SERVER

static char InitServer( void )
{
    putstring( "init server\r\n" );
    ServRespESRAddr = _ESR( ServResp );
    SAPBroadESRAddr = _ESR( SAPBroad );
    SAPWaitESRAddr = _ESR( SAPWait );

    ServECB.fragmentCount = 2;
    ServECB.fragmentDescriptor[1].address = &ServDummy;
    ServECB.fragmentDescriptor[1].size = sizeof( ServDummy );

    ServECB.ESRAddress = ServRespESRAddr;
    _IPXListenForPacket( &ServECB );

    _INITIPXECB( SAP );
    FillArray( SAPHead.destination.node, 0xff );
    SAPHead.destination.socket = SAP_SOCKET;
    SAPHead.infoType = _SWAPINT( 0x2 );
    SAPHead.serverType = DBG_SERVER_TYPE;
    _IPXGetInternetworkAddress( &SAPHead.address.network.a[0] );
    SAPHead.address.socket = SAPECB.socketNumber;
    SAPHead.intermediateNetworks = _SWAPINT( 0x1 );
    SAPECB.immediateAddress = SAPHead.destination.node;
    SAPECB.ESRAddress = SAPBroadESRAddr;
    _IPXSendPacket( &SAPECB );
    return( 1 );
}

#endif

#define ACC_WORD( loc ) (*(unsigned *)&(loc))

static char FindPartner( void )
{
    static char RepBuff[132];

#ifdef __WINDOWS__
    {
        BYTE    moresegments;
        BYTE    propertyflags;
        int         binderyerror;

        binderyerror = NWReadPropertyValue( 0,
                                         SAPHead.name,
                                         DBG_SERVER_TYPE,
//                                       _SWAPINT( DBG_SERVER_TYPE ),
                                         "NET_ADDRESS",
                                         1,
                                         RepBuff,
                                         (void *)&moresegments,
                                         (void *)&propertyflags );
        if( binderyerror != 0 ) return( 0 );
        AssignArray( ServHead.destination, RepBuff[0] );
    }
#else
    {
        extern char ReadPropertyValue( void *, void *);

        #pragma aux ReadPropertyValue =         \
        0xb4 0xe3                       /* mov ah,0e3h */       \
        0x1e                            /* push ds */           \
        0x07                            /* pop es */            \
        0xcd 0x21                       /* int 21h */           \
        parm [ si ] [ di ] value [al] modify [ es ];

        static char ReqBuff[80];
        unsigned    i;

        ReqBuff[ 2 ] = 0x3d; /* sub-function */
        ACC_WORD( ReqBuff[ 3 ] ) = DBG_SERVER_TYPE;
        i = strlen( SAPHead.name );
        ReqBuff[5] = i;
        memcpy( &ReqBuff[6], &SAPHead.name, i );
        i += 6;
        ReqBuff[ i++ ] = 1;
        ReqBuff[ i++ ] = sizeof( "NET_ADDRESS" ) - 1;
        memcpy( &ReqBuff[ i ], "NET_ADDRESS", sizeof( "NET_ADDRESS" ) - 1 );
        ACC_WORD( ReqBuff[0] ) = i + (sizeof( "NET_ADDRESS" ) - 2);
        ACC_WORD( RepBuff[0] ) = 130;
putstring( "read prop\r\n" );
        if( ReadPropertyValue( &ReqBuff, &RepBuff ) != 0 ) return( 0 );
putstring( "assgn array\r\n" );
        AssignArray( ServHead.destination, RepBuff[2] );
    }
#endif
putstring( "lcl targ\r\n" );
    if( _IPXGetLocalTarget( &ServHead.destination.network.a[0],
                           &ServECB.immediateAddress.a[0],
                            (WORD *)&RepBuff ) != 0 ) return( 0 );
    RespECB.fragmentDescriptor[1].address = &PartnerSPXSocket;
    _IPXListenForPacket( &RespECB );
putstring( "send p\r\n" );
    _IPXSendPacket( &ServECB );
    WaitOn( ServECB );
putstring( "wait for\r\n" );
    if( !WaitTimeout( &RespECB, MAX_PARTNER_WAIT, 1 ) ) return( 0 );
putstring( "got one\r\n" );
    SendHead.destination = RespHead.source;
    SendHead.destination.socket = PartnerSPXSocket;
    SendECB.immediateAddress = RespECB.immediateAddress;
    return( 1 );
}

char    DefLinkName[] = "NovLink";

const char *RemoteLink( const char *parms, bool server )
{
    unsigned    i;
    BYTE        major_ver,minor_ver;
    WORD        max_conn,avail_conn;

    #ifdef __WINDOWS__
    {
        HINSTANCE       ipxspx;
        HINSTANCE       netapi;
        HMODULE         netware;

        GlobalPageLock( (HGLOBAL)FP_SEG( &SAPECB ) );
        netware = GetModuleHandle( "NETWARE.DRV" );
        ipxspx = LoadLibrary( "NWIPXSPX.DLL" );
        netapi = LoadLibrary( "NWCALLS.DLL" );
        if( (UINT)ipxspx < 32 ) return( TRP_ERR_IPX_SPX_not_present );
        if( (UINT)netapi < 32 ) return( TRP_ERR_Netware_API_not_present );
        if( netware == NULL ) return( TRP_ERR_NETWAREDRV_not_present );
#define str( x ) #x
#define GetAddr( hdl, x ) IPXFuncs[x] = (NOVWINAPI)GetProcAddress( hdl, str( x ) )
#define GetIPXAddr( x ) GetAddr( ipxspx, x )
        GetAddr( netapi, NWReadPropertyValue );
        GetIPXAddr( IPXInitialize );
        GetIPXAddr( IPXSPXDeinit );
        GetIPXAddr( IPXCancelEvent );
        GetIPXAddr( IPXCloseSocket );
        GetIPXAddr( IPXDisconnectFromTarget );
        GetIPXAddr( IPXGetInternetworkAddress );
        GetIPXAddr( IPXGetIntervalMarker );
        GetIPXAddr( IPXGetLocalTarget );
        GetIPXAddr( IPXListenForPacket );
        GetIPXAddr( IPXOpenSocket );
        GetIPXAddr( IPXRelinquishControl );
        GetIPXAddr( IPXScheduleIPXEvent );
        GetIPXAddr( IPXSendPacket );
        GetIPXAddr( SPXAbortConnection );
        GetIPXAddr( SPXEstablishConnection );
        GetIPXAddr( SPXInitialize );
        GetIPXAddr( SPXListenForConnection );
        GetIPXAddr( SPXListenForSequencedPacket );
        GetIPXAddr( SPXSendSequencedPacket );
        GetIPXAddr( SPXTerminateConnection );
        IPXTaskID = 0x00000000L;
        if( IPXInitialize( (DWORD __far *)&IPXTaskID, 20, 576 ) != 0 ) {
            return( TRP_ERR_IPX_not_initialized );
        }
    }
    #endif
    server = server;
    if( *parms == '\0' )
        parms = DefLinkName;
    for( i = 0; i < MAX_NAME_LEN && *parms != '\0'; ++parms ) {
        if( strchr( "/\\:;,*?+-", *parms ) == NULL ) {
            SAPHead.name[ i++ ] = toupper( *parms );
        }
    }
    SAPHead.name[ i ] = '\0';
    if( !_SPXInitialize( 20, 576, &major_ver, &minor_ver, &max_conn, &avail_conn ) ) {
        return( TRP_ERR_SPX_not_present );
    }
    if( _IPXOpenSocket( &SPXSocket, 0 ) != 0 || _IPXOpenSocket( &IPXSocket, 0 ) != 0 ) {
        return( TRP_ERR_can_not_obtain_socket );
    }
    _INITIPXECB( Serv );
    _INITIPXECB( Resp );
    RespECB.fragmentCount = 2;
    RespECB.fragmentDescriptor[1].size = sizeof( WORD ); /* for SPXSocket */
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
    if( FindPartner() == 0 ) {
        RemoteUnLink();
        return( TRP_ERR_no_such_server );
    }
#endif
    return( NULL );
}


void RemoteUnLink( void )
{
    _IPXCloseSocket( SPXSocket );
    _IPXCloseSocket( IPXSocket );
#ifdef SERVER
    _IPXCancelEvent( &ServECB );
    _IPXCancelEvent( &RespECB );
    if( SAPECB.inUseFlag != 0 ) _IPXCancelEvent( &SAPECB );
    /* shutdown notification */
    SAPHead.intermediateNetworks = _SWAPINT( 0x10 );
    SAPECB.ESRAddress = NULL;
    _IPXSendPacket( &SAPECB );
    WaitOn( SAPECB );
#endif
    IPXSPXDeinit();
#ifdef __WINDOWS__
    GlobalPageUnlock( (HGLOBAL)FP_SEG( &SAPECB ) );
#endif
}
