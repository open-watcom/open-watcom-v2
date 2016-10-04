/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 2015 Open Watcom Contributors.
*    All Rights Reserved.
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
* Description:  Implementation of gethostbyname() for Linux.
*
* Author: J. Armstrong
*
****************************************************************************/

#include "variety.h"
#include "_resolve.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include "rterrno.h"
#include "thread.h"


#define DNSRESOLV   "/etc/resolv.conf"

static struct hostent *__check_hostdb( const char *name )
{
    int             i;
    struct hostent  *one;
    int             alias;

    one = NULL;
    if( name != NULL ) {
        alias = 0;
        sethostent( 1 );
        while( alias == 0 && (one = gethostent()) != NULL ) {
            if( one->h_name != NULL && strcmp( one->h_name, name ) == 0 )
                break;
            for( i = 0; one->h_aliases[i] != NULL; i++ ) {
                if( strcmp( one->h_aliases[i], name ) == 0 ) {
                    alias = 1;
                    break;
                }
            }
        }
        endhostent();
    }
    return( one );
}

/* Messy function for retrieving the "last" indexed nameserver
 * from the DNS resolution config file
 */
static int __get_nameserver( int last, in_addr_t *dnsaddr )
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

static struct hostent *__check_dns_4( const char *name )
{
    in_addr_t               dnsaddr;
    int                     servercount;
    int                     dns_success;
    static struct hostent   ret;

    dns_success = 0;
    for( servercount = 0; __get_nameserver( servercount, &dnsaddr ); servercount++ ) {
        dns_success = _dns_query( name, DNSQ_TYPE_A, dnsaddr, &ret );
        if( dns_success > 0 ) {
            return( &ret );
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
    return( NULL );
}

_WCRTLINK struct hostent *gethostbyname( const char *name )
{
    static struct hostent   *ret;

    ret = __check_hostdb( name );
    if( ret == NULL )
        ret = __check_dns_4( name );
    return( ret );
}
