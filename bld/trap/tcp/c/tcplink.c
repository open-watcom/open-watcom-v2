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
* Description: TCP/IP transport link for trap files.
*
****************************************************************************/


#define LIST_INTERFACES

#if defined( __NETWARE__ )
#define __FUNCTION_DATA_ACCESS
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#if defined( __RDOS__ )
    #include "rdos.h"
#elif defined( __NT__ ) || defined( __WINDOWS__ )
    #include <winsock.h>
#elif defined( __NETWARE__ )
    #include <sys/types.h>
    #include <unistd.h>
#elif defined( __OS2__ )
    #if defined( _M_I86 )
        #define OS2
        #define _TCP_ENTRY __cdecl __far
        #define BSD_SELECT
        #include <types.h>
        #include <netlib.h>
    #else
        #include <types.h>
        #include <arpa/inet.h>
    #endif
    #include <sys/socket.h>
    #include <sys/time.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <netdb.h>
    #include <unistd.h>
#elif defined( __DOS__ )
    #define BSD
    #include <arpa/inet.h>
    #include <sys/select.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <netdb.h>
    #include <tcp.h>
#else   /* POSIX */
    #include <sys/types.h>
    #include <unistd.h>
  #if defined( __UNIX__ )
    #include <arpa/inet.h>
  #endif
    #include <sys/socket.h>
  #if !defined( __LINUX__ )
    #include <sys/select.h>
  #endif
    #include <sys/time.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <netdb.h>
#endif

#include "watcom.h"
#if defined( __NETWARE__ )
    #include "novhax.h"
#endif

#if defined( __NETWARE__ )
    #include "debugme.h"
#endif

#include "trptypes.h"
#if !defined( SERVER ) && defined( __NT__ )
    #include "trpimp.h"
#endif
#include "trperr.h"
#include "packet.h"
#ifdef SERVER
#include "servio.h"
#endif
#ifdef LIST_INTERFACES
#include "tcpifi.h"
#endif


#if !defined( __NETWARE__ )
    #define _DBG_THREAD( x )
    #define _DBG_DR( x )
    #define _DBG_EVENT( x )
    #define _DBG_IO( x )
    #define _DBG_MISC( x )
    #define _DBG_IPX( x )
    #define _DBG_NET( x )
    #define _DBG_REQ( x )
    #define _DBG_ERROR( x )
#endif

#define DEFAULT_PORT        0x0DEB                  /* 3563 */
#define OW_INADDR_LOOPBACK  htonl( 0x7F000001UL )   /* 127.0.0.1 */

#if defined( __RDOS__ )
    #define INVALID_SOCKET          0
    #define IS_VALID_SOCKET(x)      (x!=INVALID_SOCKET && !RdosIsTcpConnectionClosed(x))
    #define IS_RET_OK(x)            (x!=0)
    #define trp_socket              int
    #define soclose( s )            RdosCloseTcpConnection( s )
    #define INVALID_INADDR(x)       (x==-1)
#elif defined( __NT__ ) || defined( __WINDOWS__ )
    #define IS_VALID_SOCKET(x)      (x!=INVALID_SOCKET)
    #define IS_RET_OK(x)            (x!=SOCKET_ERROR)
    #define trp_socket              SOCKET
    #define trp_socklen             int
    #define soclose( s )            closesocket( s )
    #define INVALID_INADDR(x)       (x==0 || x==-1)
#elif defined( __DOS__ )
    #define IS_VALID_SOCKET(x)      (x>=0)
    #define IS_RET_OK(x)            (x!=-1)
    #define trp_socket              int
    #define trp_socklen             int
    #define INVALID_INADDR(x)       (x==0)
#elif defined( __OS2__ )
    #define INVALID_SOCKET          -1
    #define IS_VALID_SOCKET(x)      (x>=0)
    #define IS_RET_OK(x)            (x!=-1)
    #define trp_socket              int
    #define trp_socklen             int
    #define INVALID_INADDR(x)       (x==-1)
#elif defined( __NETWARE__ )
    #define INVALID_SOCKET          -1
    #define IS_VALID_SOCKET(x)      (x>=0)
    #define IS_RET_OK(x)            (x!=-1)
    #define trp_socket              int
    #define trp_socklen             int
    #define soclose( s )            close( s )
    #define INVALID_INADDR(x)       (x==-1)
#else   /* POSIX */
    #define INVALID_SOCKET          -1
    #define IS_VALID_SOCKET(x)      (x>=0)
    #define IS_RET_OK(x)            (x!=-1)
    #define trp_socket              int
    #define trp_socklen             socklen_t
    #define soclose( s )            close( s )
    #define INVALID_INADDR(x)       (x==-1)
#endif

#if !defined( __NT__ ) && !defined( __WINDOWS__ )
typedef struct sockaddr         *LPSOCKADDR;
#endif

static unsigned short       trap_port;  /* host byte order */
static trp_socket           data_socket = INVALID_SOCKET;
#ifdef __RDOS__
#else
static struct sockaddr_in   socket_address;
#endif
#ifdef SERVER
  #ifdef __RDOS__
static int                  listen_handle = 0;
static int                  wait_handle = 0;
  #else
static trp_socket           control_socket;
  #endif
#else
static unsigned_32          trap_addr;  /* network byte order */
#endif

#ifdef __RDOS__

#define SOCKET_BUFFER       0x7000

static int recv( int handle, void *buf, int size, int timeout )
{
    int count;

    /* unused parameters */ (void)timeout;

    count = 0;
    while( !RdosIsTcpConnectionClosed( handle ) && count == 0 ) {
        count = RdosReadTcpConnection( handle, buf, size );
    }
    return( count );
}

static int send( int handle, const void *buf, int size, int timeout )
{
    int count;

    /* unused parameters */ (void)timeout;

    count = RdosWriteTcpConnection( handle, buf, size);
    return( count );
}

#endif

#ifdef SERVER
#else
static bool terminate_connection( void )
{
  #ifdef __RDOS__
    RdosPushTcpConnection( data_socket );
  #else
    struct linger       linger;

    linger.l_onoff = 1;
    linger.l_linger = 0;
    setsockopt( data_socket, (int)SOL_SOCKET, SO_LINGER, (void*)&linger, sizeof( linger ) );
  #endif
    soclose( data_socket );
    data_socket = INVALID_SOCKET;
    return( true );
}
#endif

#ifdef __RDOS__

#define recvData(buf,len)   recv(data_socket,buf,len,0)

#else

static int recvData( void *get, int len )
{
    int     rec, got;

    got = len;
    while( len > 0 ) {
        rec = recv( data_socket, get, len, 0 );
        if( !IS_RET_OK( rec ) )
            return( -1 );
        get = (char *)get + rec;
        len -= rec;
    }
    return( got );
}

#endif

trap_retval RemoteGet( void *data, trap_elen len )
{
    unsigned_16         rec_len;

    len = len;

    _DBG_NET(("RemoteGet\r\n"));

    if( IS_VALID_SOCKET( data_socket ) ) {
        int     size;

        size = recvData( &rec_len, sizeof( rec_len ) );
#ifdef __RDOS__
        while( size == 0 ) {
            if( !IS_VALID_SOCKET( data_socket ) )
                return( REQUEST_FAILED );
            size = recvData( &rec_len, sizeof( rec_len ) );
        }
#endif
        if( size == sizeof( rec_len ) ) {
            CONV_LE_16( rec_len );
#ifdef __RDOS__
            if( rec_len && recvData( data, rec_len ) == rec_len ) {
#else
            if( rec_len == 0 || recvData( data, rec_len ) == rec_len ) {
#endif
                _DBG_NET(("Got a packet - size=%d\r\n", rec_len));
                return( rec_len );
            }
        }
    }
    return( REQUEST_FAILED );
}

trap_retval RemotePut( void *data, trap_elen len )
{
    unsigned_16         send_len;
    int                 snd;

    _DBG_NET(("RemotePut\r\n"));

    if( IS_VALID_SOCKET( data_socket ) ) {
        send_len = len;
        CONV_LE_16( send_len );
        snd = send( data_socket, (void *)&send_len, sizeof( send_len ), 0 );
        if( IS_RET_OK( snd ) ) {
            if( len != 0 )
                snd = send( data_socket, data, len, 0 );
            if( len == 0 || IS_RET_OK( snd ) ) {
#ifdef __RDOS__
                RdosPushTcpConnection( data_socket );
#endif
                _DBG_NET(("RemotePut...OK\r\n"));
                return( len );
            }
        }
    }
    return( REQUEST_FAILED );
}

#ifdef __RDOS__

#else

static int get_proto( const char *p )
{
    struct protoent     *proto;

    proto = getprotobyname( p );
    return( ( proto != NULL ) ? proto->p_proto : IPPROTO_TCP );
}

static void nodelay( void )
{
    int                 delayoff;

    delayoff = 1;
    setsockopt( data_socket, get_proto( "tcp" ), TCP_NODELAY, (void *)&delayoff, sizeof( delayoff ) );
}

#endif

bool RemoteConnect( void )
{
#ifdef SERVER
  #ifdef __RDOS__
    void *obj;

    obj = (void *)RdosWaitTimeout( wait_handle, 250 );
    if( obj != NULL ) {
        data_socket = RdosGetTcpListen( listen_handle );
        if( IS_VALID_SOCKET( data_socket ) ) {
            _DBG_NET(("Found a connection\r\n"));
            return( true );
        }
    }
  #else
    struct timeval  timeout;
    fd_set          ready;
    struct sockaddr dummy;
    trp_socklen     dummy_len = sizeof( dummy );
    int             rc;

    FD_ZERO( &ready );
    FD_SET( control_socket, &ready );
    timeout.tv_sec = 0;
    timeout.tv_usec = 10000;
    rc = select( control_socket + 1, &ready, 0, 0, &timeout );
    if( IS_RET_OK( rc ) && rc > 0 ) {
        data_socket = accept( control_socket, &dummy, &dummy_len );
        if( IS_VALID_SOCKET( data_socket ) ) {
            nodelay();
            _DBG_NET(("Found a connection\r\n"));
            return( true );
        }
    }
  #endif
#else
  #ifdef __RDOS__
    /*
     * TODO: Add code for connect!
     */
  #else
    int         rc;

    data_socket = socket( AF_INET, SOCK_STREAM, 0 );
    if( IS_VALID_SOCKET( data_socket ) ) {
        rc = connect( data_socket, (LPSOCKADDR)&socket_address, sizeof( socket_address ) );
        if( IS_RET_OK( rc ) ) {
            nodelay();
            return( true );
        }
    }
  #endif
#endif
    return( false );
}

void RemoteDisco( void )
{
    _DBG_NET(("RemoteDisco\r\n"));

    if( IS_VALID_SOCKET( data_socket ) ) {
        soclose( data_socket );
        data_socket = INVALID_SOCKET;
    }
}

static unsigned short get_port( const char *p )
/**********************************************
 * return port value in host byte order
 */
{
    unsigned short port;
#ifdef __RDOS__
#else
    struct servent      *sp;

    sp = getservbyname( ( *p == '\0' ) ? "tcplink" : p, "tcp" );
    if( sp != NULL )
        return( ntohs( sp->s_port ) );
#endif
    port = 0;
    while( isdigit( *p ) ) {
        port = port * 10 + ( *p - '0' );
        ++p;
    }
    if( port == 0 )
        port = DEFAULT_PORT;
    return( port );
}

#ifndef SERVER
static unsigned_32 get_addr( const char *p )
/*******************************************
 * return address value in network byte order
 */
{
    unsigned_32 addr;

    /*
     * TCP/IP gethostbyname OS/2 does not handle numeric addresses
     * so a numeric format is tried first
     */
    addr = inet_addr( p );
    if( INVALID_INADDR( addr ) ) {
        struct hostent  *hp;

        hp = gethostbyname( p );
        if( hp != NULL ) {
            addr = 0;
            memcpy( &addr, hp->h_addr, hp->h_length );
        } else {
            addr = INADDR_NONE;
        }
    }
    return( addr );
}
#endif

#ifdef SERVER
#ifdef TRAPGUI
const char *RemoteLinkGet( char *parms, size_t len )
{
    /* unused parameters */ (void)len;

    sprintf( parms, "%u", (unsigned)trap_port );
    return( NULL );
}
#endif
#endif

const char *RemoteLinkSet( const char *parms )
{
#ifdef SERVER
    trap_port = get_port( parms );
    if( trap_port == 0 ) {
        return( TRP_ERR_unable_to_parse_port_number );
    }
#else
    char        buff[128];
    char        *p;

    /*
     * get port number out of name
     */
    p = buff;
    while( *parms != '\0' ) {
        if( *parms == ':' ) {
            parms++;
            break;
        }
        *p++ = *parms++;
    }
    *p = '\0';
    trap_port = get_port( parms );
    if( trap_port == 0 ) {
        return( TRP_ERR_unable_to_parse_port_number );
    }
    trap_addr = get_addr( buff );
    if( trap_addr == INADDR_NONE ) {
        trap_addr = OW_INADDR_LOOPBACK;
//        return( TRP_ERR_unknown_host );
    }
#endif
    return( NULL );
}

const char *RemoteLink( const char *parms, bool server )
{
#ifdef SERVER
  #ifdef __RDOS__
  #else
    trp_socklen         length;
    char                buff[128];
  #endif

    _DBG_NET(("SERVER: Calling into RemoteLink\r\n"));
#endif

    /* unused parameters */ (void)server;

    if( parms != NULL ) {
        parms = RemoteLinkSet( parms );
        if( parms != NULL ) {
            return( parms );
        }
    }
  #if defined( __RDOS__ )
    /* no initialization */
  #elif defined(__NT__) || defined(__WINDOWS__)
    {
        WSADATA data;

        if( WSAStartup( 0x101, &data ) != 0 ) {
            return( TRP_ERR_unable_to_initialize_TCPIP );
        }
    }
  #else
    /* no initialization */
  #endif
#ifdef SERVER
  #ifdef __RDOS__
    wait_handle = RdosCreateWait();
    listen_handle = RdosCreateTcpListen( trap_port, 1, SOCKET_BUFFER );
    RdosAddWaitForTcpListen( wait_handle, listen_handle, (int)&listen_handle );
  #else
    control_socket = socket( AF_INET, SOCK_STREAM, 0 );
    if( !IS_VALID_SOCKET( control_socket ) ) {
        return( TRP_ERR_unable_to_open_stream_socket );
    }

    /* Name socket using wildcards */
    memset( &socket_address, 0, sizeof( socket_address ) );
    socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = INADDR_ANY;
    socket_address.sin_port = htons( trap_port );
    if( bind( control_socket, (LPSOCKADDR)&socket_address, sizeof( socket_address ) ) ) {
        return( TRP_ERR_unable_to_bind_stream_socket );
    }
    /*
     * Find out assigned port number and print it out
     */
    length = sizeof( socket_address );
    if( getsockname( control_socket, (LPSOCKADDR)&socket_address, &length ) ) {
        return( TRP_ERR_unable_to_get_socket_name );
    }
    sprintf( buff, "%s%u", TRP_TCP_socket_number, (unsigned)trap_port );
    ServMessage( buff );
    _DBG_NET(("TCP: "));
    _DBG_NET((buff));
    _DBG_NET(("\r\n"));

   #ifdef LIST_INTERFACES
    list_interfaces();
   #endif
  #endif
    _DBG_NET(("Start accepting connections\r\n"));
    /*
     * Start accepting connections
     */
  #ifdef __RDOS__
  #else
    listen( control_socket, 5 );
  #endif
#else
  #ifdef __RDOS__
    /*
     * TODO: handle connect
     */
  #else
    /*
     * Setup for socket connect using parms specified by command line.
     */
    memset( &socket_address, 0, sizeof( socket_address ) );
    socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = trap_addr;
    socket_address.sin_port = htons( trap_port );
  #endif
#endif
    return( NULL );
}


void RemoteUnLink( void )
{
#ifdef SERVER
  #ifdef __RDOS__
    if( wait_handle ) {
        RdosCloseWait( wait_handle );
        wait_handle = 0;
    }
    if( listen_handle ) {
        RdosCloseTcpListen( listen_handle );
        listen_handle = 0;
    }
  #else
    soclose( control_socket );
  #endif
#else
    terminate_connection();
#endif

#if defined( __RDOS__ )
    /* no finalization */
#elif defined(__NT__) || defined(__WINDOWS__)
    WSACleanup();
#elif defined(__DOS__)
    sock_exit();
#endif
}
