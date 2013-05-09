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
* Description: TCP/IP transport link for trap files.
*
****************************************************************************/


#if defined(__OS2__) && !defined(__386__)
#define OS2
#define _TCP_ENTRY __cdecl __far
#define BSD_SELECT
#endif

#if defined ( __NETWARE__ )
#define __FUNCTION_DATA_ACCESS
#endif

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#if defined ( __RDOS__ )
    #include "rdos.h"
#else
#if defined(__NT__) || defined(__WINDOWS__)
    #include <winsock.h>
#else
    #if defined(__OS2__)
        #include <types.h>
    #else
        #include <sys/types.h>
    #endif
    #include <unistd.h>
#if !defined ( __NETWARE__ )
    #include <sys/socket.h>
#endif
#if !defined ( __LINUX__ ) && !defined ( __NETWARE__ )
    #include <sys/select.h>
#endif
#if !defined ( __NETWARE__ )
    #include <sys/time.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <netdb.h>
#endif
    #if defined(__OS2__) && !defined(__386__)
        #include <netlib.h>
    #elif defined(__UNIX__) || defined(__DOS__) || defined(__OS2__)
        #include <arpa/inet.h>
    #endif
#endif
#endif

#include <watcom.h>
#if defined(__DOS__)
    #include <machtype.h>
    #include <tcp.h>
#endif

#if defined ( __NETWARE__ )
    #include "novhax.h"
#endif

#if defined ( __NETWARE__ )
    #include "debugme.h"
#else
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

#include "trptypes.h"
#include "trperr.h"
#include "packet.h"
#include "ifi.h"

#if defined( __WATCOMC__ )
#if defined( __NT__ )
#pragma library("wsock32.lib")
#elif defined( __WINDOWS__ )
#pragma library("winsock.lib")
#endif
#endif

#if defined( __NT__ ) || defined( __WINDOWS__ )
#define IS_SOCK_ERROR(x)    (x==INVALID_SOCKET)
#elif defined( __DOS__ )
#define IS_SOCK_ERROR(x)    (x<0)
#else
#define IS_SOCK_ERROR(x)    (x<0)
#define INVALID_SOCKET      -1
#endif

#define DEFAULT_PORT    0xDEB

#ifdef __RDOS__

#define SOCKET_BUFFER   0x7000

static int listen_handle = 0;
static int wait_handle = 0;
static int socket_handle = 0;

#else

#ifndef IPPROTO_TCP
    #define IPPROTO_TCP 6
#endif

#ifndef socklen_t
  #ifdef __LINUX__
    #define socklen_t unsigned int
  #else
    #define socklen_t int
  #endif
#endif

#if !defined( __LINUX__ )
static struct ifi_info  *get_ifi_info( int family, int doaliases );
static void             free_ifi_info( struct ifi_info *ifihead );
#endif

#if defined( __NT__ ) || defined( __WINDOWS__ )
static SOCKET               data_socket = INVALID_SOCKET;
#else
static int                  data_socket = INVALID_SOCKET;
#endif
static struct sockaddr_in   socket_address;
static bool die = FALSE;
#ifdef SERVER
#if defined( __NT__ ) || defined( __WINDOWS__ )
static SOCKET               control_socket;
#else
static int                  control_socket;
#endif
#endif

#endif

#if  defined(SERVER)
extern void     ServMessage( char * );
#endif

#if defined(__UNIX__)
    #define soclose( s )        close( s )
#elif defined(__NT__) || defined(__WINDOWS__)
    #define soclose( s )        closesocket( s )
#elif defined(__NETWARE__)
    #define soclose( s)         close( s )
#endif

bool Terminate( void )
{
#ifndef __RDOS__
    struct linger       linger;

    die = TRUE;
    linger.l_onoff = 1;
    linger.l_linger = 0;
    setsockopt( data_socket, (int)SOL_SOCKET, SO_LINGER, (void*)&linger, sizeof( linger ) );
    soclose( data_socket );
    data_socket = INVALID_SOCKET;
#endif
    return( TRUE );    
}

#ifndef __RDOS__

static unsigned FullGet( void *get, unsigned len )
{
    unsigned    rec, got;

    got = len;
    for( ;; ) {
        rec = recv( data_socket, get, len, 0 );
        if( die || rec == (unsigned)-1 ) return( REQUEST_FAILED );
#if defined(__OS2__)
        /* OS/2 TCP/IP docs say that return value of 0 indicates closed
         * connection; this is unlike other TCP/IP implementations.
         */
        if( rec == 0 ) return( REQUEST_FAILED );
#endif
        len -= rec;
        if( len == 0 ) break;
        get = (unsigned_8 *)get + rec;
    }
    return( got );
}

#endif

trap_elen RemoteGet( char *rec, trap_elen len )
{
    unsigned_16         rec_len;
#ifdef __RDOS__    
    int                 size;
#endif

    _DBG_NET(("RemoteGet\r\n"));
    len = len;

#ifdef __RDOS__

    if( socket_handle && !RdosIsTcpConnectionClosed( socket_handle ) ) {
        size = RdosReadTcpConnection( socket_handle, &rec_len, 2 );

        if( size == 2 ) {
            CONV_LE_16( rec_len );

            if( rec_len ) {
                size = RdosReadTcpConnection( socket_handle, rec, rec_len );

                if( size == rec_len ) {
                    _DBG_NET(("Got a packet - size=%d\r\n", rec_len));
                    return( rec_len );
                }
            }
        }
    }   
    return( REQUEST_FAILED );
#else
    if( FullGet( &rec_len, sizeof( rec_len ) ) != sizeof( rec_len ) ) {
        return( REQUEST_FAILED );
    }
    CONV_LE_16( rec_len );
    if( rec_len != 0 ) {
        if( FullGet( rec, rec_len ) != rec_len ) {
            return( REQUEST_FAILED );
        }
    }
    _DBG_NET(("Got a packet - size=%d\r\n", rec_len));
    return( rec_len );
#endif
}

trap_elen RemotePut( char *rec, trap_elen len )
{
    unsigned_16         send_len;

    _DBG_NET(("RemotePut\r\n"));

#ifdef __RDOS__
    if( socket_handle && !RdosIsTcpConnectionClosed( socket_handle ) ) {
        send_len = len;
        CONV_LE_16( send_len );
        RdosWriteTcpConnection( socket_handle, &send_len, 2 );
        RdosWriteTcpConnection( socket_handle, rec, len );
        RdosPushTcpConnection( socket_handle );
        _DBG_NET(("RemotePut...OK\r\n"));
        return( len );
    } else
        return( REQUEST_FAILED );
#else
    send_len = len;
    CONV_LE_16( send_len );
    if( die || send( data_socket, (void *)&send_len, sizeof( send_len ), 0 ) == -1 ) {
        return( REQUEST_FAILED );
    }
    if( len != 0 ) {
        if( die || send( data_socket, rec, len, 0 ) == -1 ) {
            return( REQUEST_FAILED );
        }
    }
    _DBG_NET(("RemotePut...OK\r\n"));
    return( len );
#endif
}

#ifndef __RDOS__

static void nodelay( void )
{
    struct protoent     *proto;
    int                 delayoff;
    int                 p;

    delayoff = 1;
    proto = getprotobyname( "tcp" );
    p = proto ? proto->p_proto : IPPROTO_TCP;
    setsockopt( data_socket, p, TCP_NODELAY, (void *)&delayoff, sizeof( delayoff ) );
}

#endif

char RemoteConnect( void )
{
#ifdef SERVER
#ifdef __RDOS__
    void *obj;

    obj = RdosWaitTimeout( wait_handle, 250 );

    if( obj )
        socket_handle = RdosGetTcpListen( listen_handle );

    if( socket_handle ) {
        _DBG_NET(("Found a connection\r\n"));
        return( 1 );
    }
#else
    struct      timeval timeout;
    fd_set ready;
    struct      sockaddr dummy;
    socklen_t   dummy_len = sizeof( dummy );

    FD_ZERO( &ready );
    FD_SET( control_socket, &ready );
    timeout.tv_sec = 0;
    timeout.tv_usec = 10000;
    if( select( control_socket+1, &ready, 0, 0, &timeout ) > 0 ) {
        data_socket = accept( control_socket, &dummy, &dummy_len );
        if( !IS_SOCK_ERROR( data_socket ) ) {
            nodelay();
            _DBG_NET(("Found a connection\r\n"));
            return( 1 );
        }
    }
#endif
#else
#ifdef __RDOS__
// todo: Add code for connect!
#else
    data_socket = socket( AF_INET, SOCK_STREAM, 0 );
    if( !IS_SOCK_ERROR( data_socket ) ) {
        if( connect( data_socket, (struct sockaddr *)&socket_address, sizeof( socket_address ) ) >= 0 ) {
            nodelay();
            return( 1 );
        }
    }
#endif
#endif
    return( 0 );
}

void RemoteDisco( void )
{
    _DBG_NET(("RemoteDisco\r\n"));

#ifdef __RDOS__
    if( socket_handle ) {
        RdosCloseTcpConnection( socket_handle );
        socket_handle = 0;
    }
#else
    if( data_socket != INVALID_SOCKET ) {
        soclose( data_socket );
    }
#endif
}


char *RemoteLink( char *name, char server )
{
    unsigned            port;
#ifndef __RDOS__    
    struct servent      *sp;
#endif    

#ifdef SERVER
#ifndef __RDOS__
    socklen_t           length;
#if !defined(__LINUX__)   /* FIXME */
    struct ifi_info     *ifi, *ifihead;
    struct sockaddr     *sa;
#endif
    char                buff2[128];
#endif

    _DBG_NET(("SERVER: Calling into RemoteLink\r\n"));

#if defined(__NT__) || defined(__WINDOWS__)
    {
        WSADATA data;

        if( WSAStartup( 0x101, &data ) != 0 ) {
            return( TRP_ERR_unable_to_initialize_TCPIP );
        }
    }
#endif

#ifndef __RDOS__
    control_socket = socket(AF_INET, SOCK_STREAM, 0);
    if( IS_SOCK_ERROR( control_socket ) ) {
        return( TRP_ERR_unable_to_open_stream_socket );
    }
#endif   
 
    port = 0;
    if( name == NULL || name[0] == '\0' )
        name = "tcplink";

#ifndef __RDOS__
    sp = getservbyname( name, "tcp" );
    if( sp != NULL ) {
        port = sp->s_port;
    } else
#endif 
    {
        while( isdigit( *name ) ) {
            port = port * 10 + (*name - '0');
            ++name;
        }
        if( port == 0 ) port = DEFAULT_PORT;

#ifndef __RDOS__        
        port = htons( port );
#endif
    }

#ifdef __RDOS__
    wait_handle = RdosCreateWait( );
    listen_handle = RdosCreateTcpListen( port, 1, SOCKET_BUFFER );
    RdosAddWaitForTcpListen( wait_handle, listen_handle, &listen_handle );
#else
    /* Name socket using wildcards */
    socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = INADDR_ANY;
    socket_address.sin_port = port;
    if( bind( control_socket, (struct sockaddr *)&socket_address, sizeof( socket_address ) ) ) {
        return( TRP_ERR_unable_to_bind_stream_socket );
    }
    /* Find out assigned port number and print it out */
    length = sizeof( socket_address );
    if( getsockname( control_socket, (struct sockaddr *)&socket_address, &length ) ) {
        return( TRP_ERR_unable_to_get_socket_name );
    }
    sprintf( buff2, "%s%d", TRP_TCP_socket_number, ntohs( socket_address.sin_port ) );
    ServMessage( buff2 );
    _DBG_NET(("TCP: "));
    _DBG_NET((buff2));
    _DBG_NET(("\r\n"));

#if !defined(__LINUX__)   /* FIXME */
    /* Find and print TCP/IP interface addresses, ignore aliases */
    ifihead = get_ifi_info( AF_INET, FALSE );
    for( ifi = ifihead; ifi != NULL; ifi = ifi->ifi_next ) {
        /* Ignore loopback interfaces */
        if( ifi->flags & IFI_LOOP )
            continue;

        if( (sa = ifi->ifi_addr) != NULL ) {
            sprintf( buff2, "%s%s", TRP_TCP_ip_address,
                inet_ntoa( ((struct sockaddr_in*)sa)->sin_addr ) );
            ServMessage( buff2 );
        }
    }
    free_ifi_info( ifihead );
#endif
#endif

    _DBG_NET(("Start accepting connections\r\n"));
    /* Start accepting connections */
#ifndef __RDOS__    
    listen( control_socket, 5 );
#endif
#else
#ifdef __RDOS__
// Todo: handle connect
#else
    char        *sock;

#if defined(__NT__) || defined(__WINDOWS__)
    {
        WSADATA data;

        if( WSAStartup( 0x101, &data ) != 0 ) {
            return( TRP_ERR_unable_to_initialize_TCPIP );
        }
    }
#endif

    /* get port number out of name */
    sock = name;
    while( *sock != '\0' ) {
        if( *sock == ':' ) {
            *sock = '\0';
            ++sock;
            break;
        }
        ++sock;
    }
    if( sock[0] == '\0' )
        sp = getservbyname( "tcplink", "tcp" );
    else
        sp = getservbyname( sock, "tcp" );
    if( sp != NULL ) {
        port = sp->s_port;
    } else {
        port = 0;
        while( isdigit( *sock ) ) {
            port = port * 10 + (*sock - '0');
            ++sock;
        }
        if( *sock != '\0' ) {
            return( TRP_ERR_unable_to_parse_port_number );
        }
        if( port == 0 ) port = DEFAULT_PORT;
        port = htons( port );
    }
    /* Setup for socket connect using name specified by command line. */
    socket_address.sin_family = AF_INET;
    /* OS/2's TCP/IP gethostbyname doesn't handle numeric addresses */
    socket_address.sin_addr.s_addr = inet_addr( name );
    if( socket_address.sin_addr.s_addr == -1UL ) {
        struct hostent  *hp;

        hp = gethostbyname( name );
        if( hp != 0 ) {
            memcpy( &socket_address.sin_addr, hp->h_addr, hp->h_length );
        } else {
            return( TRP_ERR_unknown_host );
        }
    }
    socket_address.sin_port = port;
#endif
#endif
    server = server;
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
    Terminate();
#endif

#ifdef __RDOS__
    if( socket_handle ) {
        RdosPushTcpConnection( socket_handle );
        RdosCloseTcpConnection( socket_handle );
        socket_handle = 0;
    }
#endif

#if defined(__NT__) || defined(__WINDOWS__)
    WSACleanup();
#endif
#if defined(__DOS__)
    sock_exit();
#endif
}

#ifndef __RDOS__

#ifdef SERVER

/* Functions to manage IP interface information lists. On multi-homed hosts,
 * determining the IP address the TCP debug link responds on is not entirely
 * straightforward.
 */

#if defined( __OS2__ ) || defined( __DOS__ )

/* Actual implementation - feel free to port to further OSes */

#ifdef __DOS__
    #define BSD /* To get the right macros from wattcp headers. */
#endif

#include <sys/ioctl.h>
#include <net/if.h>

/* Sort out implementation differences. */
#ifdef __DOS__
    #define w_ioctl         ioctlsocket
    #define HAVE_SA_LEN     FALSE
#endif

#ifdef __OS2__
    #define w_ioctl         ioctl
    #define HAVE_SA_LEN     TRUE
#endif

static struct ifi_info * get_ifi_info( int family, int doaliases )
{
    struct ifi_info     *ifi, *ifihead, **ifipnext;
    int                 len, lastlen, flags, myflags;
    char                *ptr, *buf, lastname[IFNAMSIZ], *cptr;
    struct ifconf       ifc;
    struct ifreq        *ifr, ifrcopy;
    struct sockaddr_in  *sinptr;
#if defined( __NT__ )
    SOCKET              sockfd;
#else
    int                 sockfd;
#endif

    sockfd = socket( AF_INET, SOCK_DGRAM, 0 );

    lastlen = 0;
    len = 20 * sizeof( struct ifreq );   /* initial buffer size guess */
    for( ; ; ) {
        buf = malloc( len );
        ifc.ifc_len = len;
        ifc.ifc_buf = buf;
        if( w_ioctl( sockfd, SIOCGIFCONF, (char *)&ifc ) >= 0 ) {
            if( ifc.ifc_len == lastlen )
                break;      /* success, len has not changed */
            lastlen = ifc.ifc_len;
        }
        len += 10 * sizeof( struct ifreq );   /* increment */
        free( buf );
    }
    ifihead = NULL;
    ifipnext = &ifihead;
    lastname[0] = 0;

    for( ptr = buf; ptr < buf + ifc.ifc_len; ) {
        ifr = (struct ifreq *) ptr;

#if HAVE_SA_LEN
        len = max( sizeof( struct sockaddr ), ifr->ifr_addr.sa_len );
#else
        len = sizeof( struct sockaddr );
#endif
        ptr += sizeof( ifr->ifr_name ) + len; /* for next one in buffer */

        if( ifr->ifr_addr.sa_family != family )
            continue;   /* ignore if not desired address family */

        myflags = 0;
        if(( cptr = strchr( ifr->ifr_name, ':' )) != NULL )
            *cptr = 0;      /* replace colon will null */
        if( strncmp( lastname, ifr->ifr_name, IFNAMSIZ ) == 0 ) {
            if( doaliases == 0 )
                continue;   /* already processed this interface */
            myflags = IFI_ALIAS;
        }
        memcpy( lastname, ifr->ifr_name, IFNAMSIZ );

        ifrcopy = *ifr;
        w_ioctl( sockfd, SIOCGIFFLAGS, (char *)&ifrcopy );
        flags = ifrcopy.ifr_flags;
        if( !( flags & IFF_UP ) )
            continue;   /* ignore if interface not up */

        ifi = calloc( 1, sizeof( struct ifi_info ) );
        *ifipnext = ifi;            /* prev points to this new one */
        ifipnext  = &ifi->ifi_next; /* pointer to next one goes here */

        if( flags & IFF_LOOPBACK )
            myflags |= IFI_LOOP;

        ifi->ifi_flags = flags;     /* IFF_xxx values */
        ifi->flags     = myflags;   /* IFI_xxx values */
        memcpy( ifi->ifi_name, ifr->ifr_name, IFI_NAME );
        ifi->ifi_name[IFI_NAME-1] = '\0';

        switch( ifr->ifr_addr.sa_family ) {
        case AF_INET:
            sinptr = (struct sockaddr_in *)&ifr->ifr_addr;
            if( ifi->ifi_addr == NULL ) {
                ifi->ifi_addr = calloc( 1, sizeof( struct sockaddr_in ) );
                memcpy( ifi->ifi_addr, sinptr, sizeof( struct sockaddr_in ) );
            }
            break;

        default:
            break;
        }
    }
    free( buf );
    return( ifihead );    /* pointer to first structure in linked list */
}

static void free_ifi_info( struct ifi_info *ifihead )
{
    struct ifi_info *ifi, *ifinext;

    for( ifi = ifihead; ifi != NULL; ifi = ifinext ) {
        if( ifi->ifi_addr != NULL )
            free( ifi->ifi_addr );
        ifinext = ifi->ifi_next;    /* can't fetch ifi_next after free() */
        free( ifi );                /* the ifi_info{} itself */
    }
}

#elif !defined( __LINUX__ )

/* Stubbed out */
static struct ifi_info * get_ifi_info(int family, int doaliases)
{
    return NULL;
}

static void free_ifi_info(struct ifi_info *ifihead)
{
}

#endif

#endif

#endif
