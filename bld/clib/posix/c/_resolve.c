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
* Description:  Helper functions for DNS resolution.
*               Loosely based on code available at:
*               http://www.binarytides.com/dns-query-code-in-c-with-linux-sockets/
*
* Author: J. Armstrong
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include "_resolve.h"

/* Our send/receive buffer size */
#define DNS_BUFFER_SIZE     0xFFFF

struct __dns_header
{
    uint16_t id;            /* identification number */

    uint8_t  recursion          : 1;
    uint8_t  truncated          : 1;
    uint8_t  authoritative      : 1;
    uint8_t  opcode             : 4;
    uint8_t  qr                 : 1;
    uint8_t  respcode           : 4;
    uint8_t  checking_disabled  : 1;
    uint8_t  authenticated      : 1;
    uint8_t  reserved           : 1;
    uint8_t  recursion_avail    : 1;

    uint16_t n_questions;   /* number of question entries */
    uint16_t n_answers;     /* number of answer entries */
    uint16_t n_auth;        /* number of authority entries */
    uint16_t n_res;         /* number of resource entries */

};

struct __dns_question
{
    uint16_t qtype;
    uint16_t qclass;
};

struct __dns_response_data
{
    uint16_t type;
    uint16_t klass;
    uint32_t ttl;
};

struct __dns_resource_record
{
    char                        *name;
    struct __dns_response_data  *resource;
    unsigned char               *rdata;
};


struct __dns_query
{
    char                        *name;
    struct __dns_question       *ques;
};

static int _from_dns_name_format( char *dest, const char *reader, const char *buffer )
{
    int     p, jumped, offset;
    int     c;
    int     count;

    count = 1;
    jumped = 0;

    /* read the names in 3www6google3com format and convert them to dot name format */
    p = 0;
    while( (c = *(unsigned char *)reader) != '\0' ) {

        if( c >= 0xC0 ) {

            offset = ( c - 0xC0 ) * 256 + *(unsigned char *)(reader + 1);
            reader = buffer + offset;
            /* If we havent jumped to another location then we can count up */
            if( jumped == 0 ) {
                count++;
                jumped = 1; /* Stop counting... */
            }

        } else {

            /* If we havent jumped to another location then we can count up */
            if( jumped == 0 )
                count += c + 1;
            /* copy name */
            while( c-- > 0 ) {
                dest[p++] = *reader++;
            }
            /* add dot after name */
            dest[p++] = '.';
        }
    }
    if( p > 0 )     /* remove last dot */
        --p;
    dest[p] = '\0'; /* terminate string */

    return( count );
}

static size_t _to_dns_name_format( char *dest, const char *src )
{
    const char  *dot;
    const char  *segment;
    size_t      i, j, p;

    p = 0;
    dot = src;
    segment = src;
    do {
        dot = strchr( segment, '.' );
        if( dot == NULL ) {
            i = strlen( segment );
        } else {
            i = dot - segment;
        }
        dest[p++] = i;
        for( j = 0; j < i; j++ )
            dest[p++] = segment[j];
        if( dot != NULL ) {
            segment = dot + 1;
        }
    } while( dot != NULL && *segment != '\0' );
    dest[p++] = '\0';
    return( p );
}

static char **_add_string_to_list( char **addr_list, char *text )
{
    int     i;
    char    **lptr;

    /* Find the last entry */
    lptr = addr_list;
    i = 0;
    while( lptr[i] != NULL )
        i++;

    addr_list = (char **)realloc( addr_list, ( i + 2 ) * sizeof( char * ) );
    if( addr_list != NULL ) {
        addr_list[i + 1] = NULL;
        addr_list[i] = text;
    }

    return( addr_list );
}

static char **_add_address_to_list( char **addr_list, struct in_addr addr )
{
    char *allocated_address;

    allocated_address = malloc( 64 );
    if( allocated_address != NULL )
        strcpy( allocated_address, inet_ntoa( addr ) );

    return( _add_string_to_list( addr_list, allocated_address ) );
}

int _dns_query( const char *name, int query_type, in_addr_t dnsaddr, struct hostent *res )
{
    char            *buf, *query_name, *reader;
    int             query_socket;
    int             i, j;
    int             name_length;
    int             result;
    size_t          query_size;
    int             ret;
    int             n_answers;

    struct __dns_resource_record *answers; //the replies from the DNS server
    struct sockaddr_in      dest;

    struct __dns_header     *dns;
    struct __dns_question   *qinfo;

    struct in_addr  *lptr;

    ret = 0;

    if( res == NULL )
        return( -EINVAL );
    if( name == NULL )
        return( -EINVAL );

    buf = malloc( DNS_BUFFER_SIZE );
    if( buf == NULL )
        return( -ENOMEM );

    answers = NULL;

    query_socket = socket( AF_INET , SOCK_DGRAM , IPPROTO_UDP ); //UDP packet for DNS queries

    dest.sin_family = AF_INET;
    dest.sin_port = htons( 53 );    // dns service port
    dest.sin_addr.s_addr = dnsaddr; //dns servers

    //Set the DNS structure to standard queries
    dns = (struct __dns_header *)buf;
    memset( dns, 0, sizeof( *dns ) );

    dns->id = htons( getpid() );
    dns->recursion = 1;      /* request recursion */
    dns->n_questions = htons( 1 );
    query_size = sizeof( *dns );

    query_name = &buf[query_size];
    query_size += _to_dns_name_format( query_name, name );

    qinfo = (struct __dns_question *)&buf[query_size];

    /* type of the query, A, MX, CNAME, NS etc */
    qinfo->qclass = htons( 1 );
    qinfo->qtype = htons( query_type );
    qinfo->qclass = htons( 1 );
    query_size += sizeof( *qinfo );

    result = sendto( query_socket, buf, query_size, 0, (struct sockaddr *)&dest, sizeof( dest ) );

    if( result < 0 ) {
        ret = -ENOMSG;
        goto dns_cleanup;
    }

    i = sizeof( dest );
    result = recvfrom( query_socket, buf, DNS_BUFFER_SIZE, 0, (struct sockaddr *)&dest, (socklen_t *)&i );
    if( result < 0 ) {
        ret = -ENOMSG;
        goto dns_cleanup;
    }

    dns = (struct __dns_header *)buf;
    reader = &buf[query_size];

    n_answers = ntohs( dns->n_answers );
    if( n_answers == 0 ) {
        ret = -ENOENT;
        goto dns_cleanup;
    }

    answers = malloc( n_answers * sizeof( *answers ) );
    if( answers == NULL )
        goto dns_cleanup;
    memset( answers, 0, n_answers * sizeof( *answers ) );

    /* If we've reached this point, we have answers (or none, but
     * a valid lack of answers), so begin filling in the result
     * structure appropriately
     */
    if( query_type == DNSQ_TYPE_A ) {
        res->h_name = malloc( strlen( name ) + 1 );
        if( res->h_name != NULL ) {
            strcpy( res->h_name, name );
        }
    }

    res->h_addr_list = (char **)malloc( sizeof( char * ) );
    if( res->h_addr_list != NULL )
        res->h_addr_list[0] = NULL;

    res->h_aliases = (char **)malloc( sizeof( char * ) );
    if( res->h_aliases != NULL )
        res->h_aliases[0] = NULL;

    /* We only support IPv4 right now. */
    res->h_addrtype = AF_INET;

    for( i = 0; i < n_answers; i++ ) {
        int     rdata_length;

        answers[i].name = malloc( 256 );
        name_length = _from_dns_name_format( answers[i].name, reader, buf );
        reader += name_length;

        answers[i].resource = (struct __dns_response_data *)reader;
        reader += sizeof( struct __dns_response_data );
        rdata_length = ntohs( *(uint16_t *)reader );
        reader += sizeof( uint16_t * );

        if( ntohs( answers[i].resource->type ) == DNSQ_TYPE_A ) { /* IPv4 encountered */

            answers[i].rdata = malloc( rdata_length + 1 );
            if( answers[i].rdata != NULL ) {
                for( j = 0; j < rdata_length; j++ ) {
                    answers[i].rdata[j] = reader[j];
                }
                answers[i].rdata[rdata_length] = '\0';
            }
            reader += rdata_length;

            lptr = (struct in_addr *)answers[i].rdata;

            res->h_addr_list = _add_address_to_list( res->h_addr_list, *lptr );
            ret = 1;
        } else {
            /* Answer should just be a string */
            answers[i].rdata = malloc( 256 );
            if( answers[i].rdata != NULL ) {
                name_length = _from_dns_name_format( answers[i].name, reader, buf );
                if( query_type == ntohs( answers[i].resource->type ) ) {
                    if( res->h_name == NULL ) {
                        res->h_name = answers[i].name;
                        answers[i].name = NULL;
                    } else {
                        res->h_aliases = _add_string_to_list( res->h_aliases, answers[i].name );
                        answers[i].name = NULL;
                    }
                }
                reader += name_length;
            }
        }
    }

dns_cleanup:

    if( answers != NULL && dns != NULL ) {
        for( i = 0; i < n_answers; i++ ) {
            if( answers[i].name != NULL )
                free( answers[i].name );
            if( answers[i].rdata != NULL) {
                free( answers[i].rdata );
            }
        }
        free( answers );
    }
    free( buf );

    return( ret );
}
