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
* Description: implementation of function for list IP interfaces in a machine.
*
****************************************************************************/


#define LIST_INTERFACES

#if defined( SERVER ) && ( defined( __OS2__ ) || defined( __DOS__ ) )

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#if defined( __DOS__ )
    #define BSD
    #include <sys/wtypes.h>
    #include <arpa/inet.h>
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
#endif
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include "trperr.h"
#include "servio.h"
#include "tcpifi.h"


/*
 * Functions to manage IP interface information lists. On multi-homed hosts,
 * determining the IP address the TCP debug link responds on is not entirely
 * straightforward.
 *
 * Actual implementation - feel free to port to further OSes
 *
 * TODO: rework to use POSIX if_nameindex in <net/if.h>
 */

#define IFI_NAME        16      /* same as IFNAMSIZ in <net/if.h> */

#define IFI_ALIAS       1       /* ifi_addr is an alias */
#define IFI_LOOP        2       /* corresponds to IFF_LOOPBACK */

#if defined( __DOS__ )
    #define w_ioctl     ioctlsocket
    #define trp_socket  int
#elif defined( __OS2__ )
    #define w_ioctl     ioctl
    #define trp_socket  int
#endif

struct ifi_info {
    char            ifi_name[IFI_NAME]; /* interface name, null terminated */
    short           ifi_flags;          /* IFF_xxx constants from <net/if.h> */
    short           flags;              /* our own IFI_xxx flags */
    struct sockaddr *ifi_addr;          /* primary address */
    struct ifi_info *ifi_next;          /* next of these structures */
};

static struct ifi_info  *get_ifi_info( int family, int doaliases )
{
    struct ifi_info     *ifi, *ifihead, **ifipnext;
    int                 len, lastlen, flags, myflags;
    char                *ptr, *buf, lastname[IFNAMSIZ], *cptr;
    struct ifconf       ifc;
    struct ifreq        *ifr, ifrcopy;
    struct sockaddr_in  *sinptr;
    trp_socket          sockfd;

    sockfd = socket( AF_INET, SOCK_DGRAM, 0 );

    lastlen = 0;
    len = 20 * sizeof( struct ifreq );   /* initial buffer size guess */
    for( ;; ) {
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
        ifr = (struct ifreq *)ptr;

#if defined( __DOS__ )
        len = sizeof( struct sockaddr );
#elif defined( __OS2__ )
        len = sizeof( struct sockaddr );
        if( len < ifr->ifr_addr.sa_len ) {
            len = ifr->ifr_addr.sa_len ;
        }
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
        ifi->ifi_name[IFI_NAME - 1] = '\0';

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

void list_interfaces( void )
/***************************
 * Find and print TCP/IP interface addresses (IP V4)
 * ignore aliases and loopback
 */
{
    struct ifi_info     *ifi, *ifihead;
    struct sockaddr     *sa;
    char                buff[128];

    ifihead = get_ifi_info( AF_INET, false );
    for( ifi = ifihead; ifi != NULL; ifi = ifi->ifi_next ) {
        /*
         * Ignore loopback interfaces
         */
        if( ifi->flags & IFI_LOOP )
            continue;
        if( (sa = ifi->ifi_addr) != NULL ) {
            sprintf( buff, "%s%s", TRP_TCP_ip_address,
                inet_ntoa( ((struct sockaddr_in *)sa)->sin_addr ) );
            ServMessage( buff );
        }
    }
    free_ifi_info( ifihead );
}

#endif
