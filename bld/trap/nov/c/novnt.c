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
#include <winsock.h>
#include <wsipx.h>
#include <svcguid.h>
#include <nspapi.h>
#include "wio.h"
#include "watcom.h"
#include "ipxstuff.h"
#include "trptypes.h"
#include "trperr.h"
#include "packet.h"

#if defined( __WATCOMC__ ) && defined( __NT__ )
#pragma library("wsock32")
#endif

#ifdef SERVER
static HANDLE                   ResponderThreadHandle;
static SOCKET                   ResponderSocket = INVALID_SOCKET;
static SOCKET                   ListenSocket = INVALID_SOCKET;
static WORD                     ListenPort = 0;
#endif
static SOCKET                   ConnectionSocket = INVALID_SOCKET;
static struct sockaddr_ipx      PartnerAddr;

static char                     ServerName[128];

/* The _SWAPINT is because NT swaps the bytes on us again */
#define DBG_SAP_ID      SVCID_NETWARE( _SWAPINT( DBG_SERVER_TYPE ) )

bool Terminate( void )
{
    // a sideways dive to terminate the link (with failure)
    return( FALSE );
}

trap_retval RemoteGet( void *data, trap_elen len )
{
    int         got;
    trap_elen   total;

    total = 0;
    for( ;; ) {
        got = recv( ConnectionSocket, data, len, 0 );
        if( got == SOCKET_ERROR )
            return( REQUEST_FAILED );
        total += got;
        if( got != MAX_DATA_SIZE )
            break;
        len -= got;
        data = (char *)data + got;
    }
    return( total );
}

trap_retval RemotePut( void *data, trap_elen len )
{

    while( len >= MAX_DATA_SIZE ) {
        if( send( ConnectionSocket, data, MAX_DATA_SIZE, 0 ) == SOCKET_ERROR ) {
            return( REQUEST_FAILED );
        }
        data = (char *)data + MAX_DATA_SIZE;
        len -= MAX_DATA_SIZE;
    }
    if( send( ConnectionSocket, data, len, 0 ) == SOCKET_ERROR ) {
        return( REQUEST_FAILED );
    }
    return( len );
}

bool RemoteConnect( void )
{
#ifdef SERVER
    struct sockaddr_ipx     address;
    int                     addr_len;
    struct timeval          timeout;
    struct fd_set           fd;

    if( ListenPort == 0 ) {
        memset( &address, 0, sizeof( address ) );
        address.sa_family = AF_IPX;
        ListenSocket = socket( AF_IPX, SOCK_SEQPACKET, NSPROTO_SPX );
        bind( ListenSocket, (struct sockaddr *)&address, sizeof( address ) );
        addr_len = sizeof( address );
        getsockname( ListenSocket, (struct sockaddr *)&address, &addr_len );
        ListenPort = address.sa_socket;
        listen( ListenSocket, 1 );
    }
    FD_ZERO( &fd );
    FD_SET( ListenSocket, &fd );
    timeout.tv_sec = 0;
    timeout.tv_usec = 10000;
    if( select( FD_SETSIZE, &fd, NULL, NULL, &timeout ) > 0 ) {
        addr_len = sizeof( address );
        ConnectionSocket = accept( ListenSocket, (struct sockaddr *)&address, &addr_len );
        ListenPort = 0;
        closesocket( ListenSocket );
        ListenSocket = INVALID_SOCKET;
        return( TRUE );
    }
#else
    ConnectionSocket = socket( AF_IPX, SOCK_SEQPACKET, NSPROTO_SPX );
    if( connect( ConnectionSocket, (struct sockaddr *)&PartnerAddr, sizeof( PartnerAddr ) ) == 0 ) {
        return( TRUE );
    }
#endif
    return( FALSE );
}

void RemoteDisco( void )
{
    Sleep( 500 );       /* make sure last packet gets sent */
    closesocket( ConnectionSocket );
    ConnectionSocket = INVALID_SOCKET;
}

#define _bad_rc(x)              ((x) == SOCKET_ERROR)

#ifdef SERVER

static void register_bindery( char *name, BOOL do_register )
{
    DWORD               status = 0;
    SERVICE_INFO        info;
    GUID                guid = DBG_SAP_ID;
    SERVICE_ADDRESSES   server_address;
    int                 rv;
    struct sockaddr_ipx address;
    int                 addr_len;


    addr_len = sizeof( address );
    getsockname( ResponderSocket, (struct sockaddr *)&address, &addr_len );
    server_address.dwAddressCount = 1;
    server_address.Addresses[0].dwAddressType   = AF_IPX;
    server_address.Addresses[0].dwAddressFlags  = 0;
    server_address.Addresses[0].dwAddressLength = addr_len;
    server_address.Addresses[0].lpAddress       = (LPBYTE)&address;
    server_address.Addresses[0].dwPrincipalLength = 0;
    server_address.Addresses[0].lpPrincipal     = NULL;

    info.lpServiceType  = &guid;
    info.lpServiceName  = name;
    info.lpComment      = "Watcom Remote Debugger";
    info.lpLocale       = NULL;
    info.dwDisplayHint  = 0;
    info.dwVersion      = 0;
    info.dwTime         = 0;
    info.lpMachineName  = NULL;
    info.lpServiceAddress = &server_address;
    info.ServiceSpecificInfo.cbSize = 0;
    info.ServiceSpecificInfo.pBlobData = NULL;

    rv = SetService(NS_SAP,
                      do_register ? SERVICE_REGISTER : SERVICE_DEREGISTER,
                      SERVICE_FLAG_HARD,
                      &info,
                      NULL,
                      &status);

    if( _bad_rc( rv ) ) {
        return;
    }

    rv = SetService(NS_SAP,
                      SERVICE_FLUSH,
                      SERVICE_FLAG_HARD,
                      &info,
                      NULL,
                      &status);
}

static DWORD WINAPI Responder( LPVOID parm )
{
    int                 from_length;
    struct sockaddr_ipx from;
    char                buffer[ 1 ];

    parm = parm;
    for( ;; ) {
        from_length = sizeof( struct sockaddr_ipx );

        if( from_length == 0 ) break;   //to keep the compiler quiet
        if( recvfrom( ResponderSocket,
                        buffer,
                        sizeof( buffer ),
                        0,
                        (struct sockaddr *)&from,
                        &from_length ) >= 0 ) {
            sendto( ResponderSocket,
                      (char *)&ListenPort,
                      sizeof( ListenPort ),
                      0,
                      (struct sockaddr *)&from,
                      from_length );
        }
    }
    return( 0 );
}


static char *InitServer( void )
{
    DWORD       tid;
    struct sockaddr_ipx address;

    ResponderSocket = socket( AF_IPX, SOCK_DGRAM, NSPROTO_IPX+IPX_PACKET_TYPE );
    if( ResponderSocket == INVALID_SOCKET )
        return( "No responder socker" );
    memset( &address, 0, sizeof( address ) );
    address.sa_family = AF_IPX;
    bind( ResponderSocket, (struct sockaddr *)&address, sizeof( address ) );
    ResponderThreadHandle = CreateThread( NULL, 0, Responder, NULL, 0, &tid );
    if( ResponderThreadHandle == NULL )
        return( "No responder thread" );
    register_bindery( ServerName, TRUE );
    return( NULL );
}
#endif

static BOOL read_bindery( char * name ) {
/***************************************/
    GUID                guid = DBG_SAP_ID;
    static char         addr_buff[1024];
    DWORD               len;
    DWORD               alias_len;
    int                 protocols[2] = { NSPROTO_IPX, 0 };
    INT                 num;
    LPCSADDR_INFO       address_info;

    len = sizeof( addr_buff );
    alias_len = 0;
    num = GetAddressByName( NS_SAP,
                            &guid,
                            name,
                            protocols,
                            0,
                            NULL,
                            addr_buff,
                            &len,
                            NULL,
                            &alias_len );

    if( _bad_rc( num ) || num < 1 ) {
        return( FALSE );
    }

    address_info = (LPCSADDR_INFO)addr_buff;
    PartnerAddr = *(struct sockaddr_ipx *)(address_info->RemoteAddr.lpSockaddr);
    return( TRUE );
}

static char FindPartner( void )
{
    SOCKET                      b_socket;
    int                         rv;
    int                         from_length;
    struct sockaddr_ipx         s_address;
    struct timeval              timer;
    fd_set                      read_set;
    WORD                        port;

    // Look in the NetWare bindery for the requested server name.

    if( !read_bindery( ServerName ) ) {
        return( 0 );
    }

    b_socket = socket( AF_IPX, SOCK_DGRAM, NSPROTO_IPX+IPX_PACKET_TYPE );

    if( b_socket == INVALID_SOCKET ) {
        return( 0 );
    }

    memset( &s_address, 0, sizeof( s_address ) );
    s_address.sa_family = AF_IPX;

    bind( b_socket, (struct sockaddr *)&s_address, sizeof( s_address ) );
    sendto( b_socket, "", 1, 0, (struct sockaddr *)&PartnerAddr, sizeof( PartnerAddr ) );
    WSAGetLastError();
    timer.tv_sec  = MAX_PARTNER_WAIT / TICKS_PER_SEC;
    timer.tv_usec = 0;

    FD_ZERO( &read_set );
    FD_SET( b_socket, &read_set );

    rv = select( b_socket + 1,
                  &read_set,
                  NULL,
                  NULL,
                  &timer );

    if( _bad_rc( rv ) ) {
        closesocket( b_socket );
        return( 0 );
    }

    if( rv == 0 ) {
        // Server not found
        closesocket( b_socket );
        return( 0 );
    }

    // Server found -- get the address.

    from_length = sizeof( PartnerAddr );

    rv = recvfrom( b_socket, (char *)&port, sizeof( port ), 0, (struct sockaddr *)&PartnerAddr, &from_length );

    closesocket( b_socket );

    if( _bad_rc( rv ) ) {
        return( 0 );
    }

    PartnerAddr.sa_socket = port;
    return( 1 );
}

const char *RemoteLink( const char *parms, bool server )
{
    unsigned    i;
    WSADATA     data;
#ifdef SERVER
    const char  *p;
#endif

    server = server;

    if( *parms == '\0' )
        parms = "NovLink";
    for( i = 0; i < 47 && *parms != '\0'; ++parms ) {
        if( strchr( "/\\:;,*?+-", *parms ) == NULL ) {
            ServerName[ i++ ] = (char)toupper( *(byte *)parms );
        }
    }
    ServerName[ i ] = '\0';
    if( WSAStartup( 0x101, &data ) != 0 ) {
        return( TRP_ERR_can_not_obtain_socket );
    }
#ifdef SERVER
    if( FindPartner() ) {
        RemoteUnLink();
        return( TRP_ERR_server_name_already_in_use );
    }
    if( (p = InitServer()) != NULL ) {
        RemoteUnLink();
        return( p );
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
    register_bindery( ServerName, FALSE );
    TerminateThread( ResponderThreadHandle, 0 );
    closesocket( ResponderSocket );
    closesocket( ListenSocket );
    ResponderThreadHandle = 0;
    ResponderSocket = 0;
    ListenSocket = INVALID_SOCKET;
    ListenPort = 0;
#endif
    closesocket( ConnectionSocket );
    WSACleanup();
    ConnectionSocket = INVALID_SOCKET;
    memset( &PartnerAddr, 0, sizeof( PartnerAddr ) );
}
