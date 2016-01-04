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
* Description:  Implementations of getnetent/setnetent/endnetent
*
* Author: J. Armstrong
*
****************************************************************************/

#include "variety.h"
#include "rtdata.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "_prscfg.h"

#define MAX_NET_ALIASES  16

static const char   *__netFname = "/etc/networks";
static FILE         *__netFile;
static int          __netClose = 1;

_WCRTLINK struct netent *getnetent( void )
{
    static struct netent    ret;
    char                    *alias;

    /* For loading a line from the services file */
    static char             *buf = NULL;
    static size_t           buflen = 0;
    char                    *buf_ptr;

    /* Translating the port string */
    char *address;


    /* For line parsing */
    int i;

    if( __netFile == NULL )
        setnetent( 0 );

    /* First pass */
    if( buf == NULL ) {
        buflen = 64;
        buf = (char *)malloc( buflen );
        if( buf ) {
            *buf = '\0';
        } else {
            buflen = 0;
        }
    }

    if( __getconfigline( &buf, &buflen, __netFile ) < 0 )
        return( NULL );

    if( __netClose )
        endnetent();

    /* Name */
    ret.n_name = strtok_r( buf, " \t", &buf_ptr );

    /* OW only supports AF_INET for now */
    ret.n_addrtype  = AF_INET;

    /* Address */
    address = strtok_r( NULL, " \t", &buf_ptr );
    ret.n_net = inet_addr(address);

    /* Load in any aliases */
    if(ret.n_aliases == NULL)
        ret.n_aliases = (char **)malloc( (1 + MAX_NET_ALIASES)*sizeof( char * ) );

    /* Explcitly NULL the entries in a nice, readable manner */
    for( i = 0; i < (1 + MAX_NET_ALIASES) && ret.n_aliases != NULL; i++ )
        ret.n_aliases[i] = NULL;

    alias = strtok_r( NULL, " \t", &buf_ptr );

    if( alias && *alias != '#' && *alias != ';' && ret.n_aliases != NULL ) {
        i = -1;
        do {
            i++;
            if( *alias == '#' || *alias == ';' )
                break;

            ret.n_aliases[i] = strtok_r( NULL, " \t", &buf_ptr );

            if( ret.n_aliases[i] == NULL ) {
                break;
            } else if( ret.n_aliases[i][0] == '\0' ) {
                ret.n_aliases[i] = NULL;
                break;
            }

        } while( i < MAX_NET_ALIASES );

    }

    return( &ret );
}

_WCRTLINK void setnetent( int stayopen )
{
    __netClose = ( stayopen == 0 );

    if( __netFile == NULL ) {
        __netFile = fopen( __netFname, "rt" );
    } else {
        rewind( __netFile );
    }
}

_WCRTLINK void endnetent( void )
{
    if( __netFile != NULL ) {
        fclose( __netFile );
        __netFile = NULL;
        __netClose = 1;
    }
}
