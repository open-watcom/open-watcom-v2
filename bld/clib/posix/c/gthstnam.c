/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of gethostbyname() for Linux and RDOS.
*
* Author: J. Armstrong
*
****************************************************************************/

#include "variety.h"
#include "seterrno.h"
#include "_resolve.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#ifdef __RDOS__
    #include "rdos.h"
#else
    #include "thread.h"
#endif
#include "_hostent.h"


#ifdef __RDOS__

#else

#define DNSRESOLV   "/etc/resolv.conf"

static int _WCNEAR __check_hostdb( const char *name )
{
    int             i;
    int             rc;

    rc = -1;

    if( name == NULL )
        return( rc );

    sethostent( 1 );

    while( rc == -1 && gethostent() != NULL ) {
        if( _RWD_hostent.h_name != NULL && strcmp( _RWD_hostent.h_name, name ) == 0 ) {
            rc = 0;
            break;
        }
        for( i = 0; _RWD_hostent.h_aliases[i] != NULL; i++ ) {
            if( strcmp( _RWD_hostent.h_aliases[i], name ) == 0 ) {
                rc = 0;
                break;
            }
        }
    }

    endhostent();

    return( rc );
}

/* Messy function for retrieving the "last" indexed nameserver
 * from the DNS resolution config file
 */
static int _WCNEAR __get_nameserver( int last, in_addr_t *dnsaddr )
{
    FILE    *fp;
    int     ret;

    /* for parsing resolv.conf */
    char    *currentline;
    size_t  linelength;
    int     which;
    char    *ptrlabel;
    char    *ptraddr;
    char    *ptraddrtail;

    ret = 0;
    fp = fopen( DNSRESOLV, "r" );
    if( fp != NULL ) {
        which = 0;
        currentline = NULL;
        linelength = 0;

        while( getline( &currentline, &linelength, fp ) >= 0 ) {
            ptrlabel = currentline;
            while( *ptrlabel == ' ' || *ptrlabel == '\t' )
                ptrlabel++;

            if( *ptrlabel == '\0' )
                continue;

            ptraddr = ptrlabel;
            while( *ptraddr != ' ' && *ptraddr != '\t' && *ptraddr != '\0' )
                ptraddr++;
            *ptraddr = '\0';

            if( strcmp( ptrlabel, "nameserver" ) == 0 ) {
                if( which == last ) {
                    ptraddr++;
                    while( *ptraddr == ' ' || *ptraddr == '\t' )
                        ptraddr++;
                    ptraddrtail = ptraddr;
                    while( *ptraddrtail != ' ' && *ptraddrtail != '\t' &&
                          *ptraddrtail != '\0' && *ptraddrtail != '\n' )
                        ptraddrtail++;
                    *ptraddrtail = '\0';
                    if( *ptraddr != '\0' ) {
                        *dnsaddr = inet_addr( ptraddr );
                        ret = 1;
                        break;
                    }
                } else {
                    which++;
                }
            }
        }
        fclose( fp );
    }
    return( ret );
}

static int _WCNEAR __check_dns_4( const char *name )
{
    in_addr_t       dnsaddr;
    int             servercount;
    int             dns_success;

    dns_success = 0;
    for( servercount = 0; __get_nameserver( servercount, &dnsaddr ); servercount++ ) {
        dns_success = _dns_query( name, DNSQ_TYPE_A, dnsaddr );
        if( dns_success > 0 ) {
            return( 0 );
        }
    }
    switch( -dns_success ) {
    case EINVAL:
    case ENOMEM:
    case EAGAIN:
    case ENOMSG:
        h_errno = TRY_AGAIN;
        break;
    case ENOENT:
        h_errno = HOST_NOT_FOUND;
        break;
    case 0:
        h_errno = NO_DATA;
        break;
    }
    return( -1 );
}
#endif


_WCRTLINK struct hostent *gethostbyname( const char *name )
{
#ifdef __RDOS__
    static uint32_t *list[2];
    static uint32_t ip;

    ip = inet_addr( name );
    if( ip == INADDR_NONE ) {
        ip = RdosNameToIp( name );
        if( ip == 0 ) {
            h_errno = HOST_NOT_FOUND;
            return( NULL );
        }
    }
    _RWD_hostent.h_name = (char *)name;
    _RWD_hostent.h_aliases = NULL;
    _RWD_hostent.h_addrtype = AF_INET;
    _RWD_hostent.h_length = sizeof( uint32_t );
    list[0] = &ip;
    list[1] = 0;
    _RWD_hostent.h_addr_list = (char **)list;
#else
    if( __check_hostdb( name ) == -1 ) {
        if( __check_dns_4( name ) == -1 ) {
            return( NULL );
        }
    }
#endif
    return( &_RWD_hostent );
}
