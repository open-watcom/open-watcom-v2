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


#define LITTLE_ENDIAN   1
#define BIG_ENDIAN      2
#define BYTE_ORDER      LITTLE_ENDIAN
#define DEFAULT_PORT    0xDEB

#if defined(__OS2__)
#define OS2
#define _TCP_ENTRY __cdecl __far
#define BSD_SELECT
#endif

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#if defined(__NT__) || defined(__WINDOWS__)
    #include <winsock.h>
#else
    #if defined(__OS2__)
        #define off_t unused_type;
        #include <types.h>
        #undef off_t
    #else
        #include <sys/types.h>
    #endif
    #include <sys/socket.h>
    #include <sys/select.h>
    #include <sys/time.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <netdb.h>
    #if defined(__OS2__)
        #include <netlib.h>
    #elif defined(__QNX__)
        #include <arpa/inet.h>
    #endif
#endif
#include <watcom.h>
#include "packet.h"
#include "trptypes.h"
#include "trperr.h"
#include "bool.h"

int data_socket;
struct sockaddr_in socket_address;
struct hostent *hp;
static bool die = FALSE;
#ifdef SERVER
int control_socket;
#endif

#if  defined(SERVER)
extern void     ServMessage( char * );
#endif

#if defined(__QNX__)
    #define soclose( s )        close( s )
#elif defined(__NT__) || defined(__WINDOWS__)
    #define soclose( s )        closesocket( s )
#endif

bool Terminate( void )
{
    struct linger       linger;

    die = TRUE;
    linger.l_onoff = 1;
    linger.l_linger = 0;
    setsockopt( data_socket, SOL_SOCKET, SO_LINGER, (void*)&linger, sizeof( linger ) );
    soclose( data_socket );
    data_socket = -1;
    return( TRUE );
}

static unsigned FullGet( void *get, unsigned len )
{
    unsigned    rec, got;

    got = len;
    for( ;; ) {
        rec = recv( data_socket, get, len, 0 );
        if( die || rec == (unsigned)-1 ) return( REQUEST_FAILED );
        len -= rec;
        if( len == 0 ) break;
        get = (unsigned_8 *)get + rec;
    }
    return( got );
}

unsigned RemoteGet( void *rec, unsigned len )
{
    unsigned_16         rec_len;

    len = len;
    if( FullGet( &rec_len, sizeof( rec_len ) ) != sizeof( rec_len ) ) {
        return( REQUEST_FAILED );
    }
    if( rec_len != 0 ) {
        if( FullGet( rec, rec_len ) != rec_len ) {
            return( REQUEST_FAILED );
        }
    }
    return( rec_len );
}

unsigned RemotePut( void *rec, unsigned len )
{
    unsigned_16         send_len;

    send_len = len;
    if( die || send( data_socket, (void *)&send_len, sizeof( send_len ), 0 ) == -1 ) {
        return( REQUEST_FAILED );
    }
    if( len != 0 ) {
        if( die || send( data_socket, rec, len, 0 ) == -1 ) {
            return( REQUEST_FAILED );
        }
    }
    return( len );
}

static void nodelay()
{
    struct protoent     *proto;
    int                 delayoff;

    delayoff = 1;
    proto = getprotobyname( "tcp" );
    setsockopt( data_socket, proto->p_proto, TCP_NODELAY, (void *)&delayoff, sizeof( delayoff ) );
}

char RemoteConnect( void )
{
#ifdef SERVER
    struct      timeval timeout;
    struct      fd_set ready;
    struct      sockaddr dummy;
    int         dummy_len = sizeof( dummy );

    FD_ZERO( &ready );
    FD_SET( control_socket, &ready );
    timeout.tv_sec = 0;
    timeout.tv_usec = 10000;
    if( select( control_socket+1, &ready, 0, 0, &timeout ) > 0 ) {
        data_socket = accept( control_socket, &dummy, &dummy_len );
        if( data_socket != -1 ) {
            nodelay();
            return( 1 );
        }
    }
#else
    data_socket = socket( AF_INET, SOCK_STREAM, 0 );
    if( data_socket >= 0 ) {
        if( connect( data_socket, (struct sockaddr *)&socket_address,
                     sizeof( socket_address ) ) >= 0 ) {
            nodelay();
            return( 1 );
        }
    }
#endif
    return( 0 );
}

void RemoteDisco( void )
{
    if( data_socket != -1 ) soclose( data_socket );
}


char *RemoteLink( char *name, char server )
{
    struct servent      *sp;
    unsigned            port;

#ifdef SERVER
    int                 length;

#if defined(__NT__) || defined(__WINDOWS__)
    {
        WSADATA data;

        if( WSAStartup( 0x101, &data ) != 0 ) {
            return( TRP_ERR_unable_to_initialize_TCPIP );
        }
    }
#endif

    control_socket = socket(AF_INET, SOCK_STREAM, 0);
    if( control_socket < 0 ) {
        return( TRP_ERR_unable_to_open_stream_socket );
    }
    port = 0;
    if( name == NULL || name[0] == '\0' ) name = "tcplink";
    sp = getservbyname( name, "tcp" );
    if( sp != NULL ) {
        port = sp->s_port;
    } else {
        while( isdigit( *name ) ) {
            port = port * 10 + (*name - '0');
            ++name;
        }
        if( port == 0 ) port = DEFAULT_PORT;
        port = htons( port );
    }
    /* Name socket using wildcards */
    socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = INADDR_ANY;
    socket_address.sin_port = port;
    if( bind( control_socket, (struct sockaddr *)&socket_address,
              sizeof( socket_address ) ) ) {
        return( TRP_ERR_unable_to_bind_stream_socket );
    }
    /* Find out assigned port number and print it out */
    length = sizeof( socket_address );
    if( getsockname( control_socket, (struct sockaddr *)&socket_address,
                     &length ) ) {
        return( TRP_ERR_unable_to_get_socket_name );
    }
    {
        char buffer[16];
        char buff2[128];
        strcpy( buff2, TRP_TCP_socket_number );
        strcat( buff2, itoa( ntohs( socket_address.sin_port ), buffer, 10 ) );
        ServMessage( buff2 );
    }

    /* Start accepting connections */
    listen( control_socket, 5 );
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
        hp = gethostbyname( name );
        if( hp != 0 ) {
            memcpy( &socket_address.sin_addr, hp->h_addr, hp->h_length );
        } else {
            return( TRP_ERR_unknown_host );
        }
    }
    socket_address.sin_port = port;
#endif
    server = server;
    return( NULL );
}


void RemoteUnLink( void )
{
#ifdef SERVER
    soclose( control_socket );
#endif
#if defined(__NT__) || defined(__WINDOWS__)
    WSACleanup();
#endif
}
