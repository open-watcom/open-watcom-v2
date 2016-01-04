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
* Description:  Implementations of gethostent/sethostent/endhostent
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

#define MAX_SERV_ALIASES  16

static const char   *__servFname = "/etc/services";
static FILE         *__servFile;
static int          __servClose = 1;

/*
 * Return the next (non-commented) line from the services-file
 * Format is:
 *  ip-address [=] host-name [alias..] {\n | # ..}
 */
_WCRTLINK struct servent *getservent( void )
{
    static struct servent   ret;
    char                    *alias;

    /* For loading a line from the services file */
    static char             *buf = NULL;
    static size_t           buflen = 0;
    char                    *buf_ptr;

    /* Translating the port string */
    char *port;


    /* For line parsing */
    int i;

    if( __servFile == NULL )
        setservent( 0 );

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

    if( __getconfigline( &buf, &buflen, __servFile ) < 0 )
        return( NULL );

    if( __servClose )
        endservent();

    /* Name */
    ret.s_name = strtok_r( buf, " \t", &buf_ptr );

    /* Port */
    port = strtok_r( NULL, "/", &buf_ptr );
    if( port == NULL )
        return( NULL );
    ret.s_port = atoi( port );

    /* Protocol */
    ret.s_proto = strtok_r( NULL, " \t", &buf_ptr );

    /* Load in any aliases */
    if( ret.s_aliases == NULL )
        ret.s_aliases = (char **)malloc( ( 1 + MAX_SERV_ALIASES ) * sizeof( char * ) );

    /* Explcitly NULL the entries in a nice, readable manner */
    for( i = 0; i < ( 1 + MAX_SERV_ALIASES ) && ret.s_aliases != NULL; i++ )
        ret.s_aliases[i] = NULL;

    alias = strtok_r( NULL, " \t", &buf_ptr );

    if( alias && *alias != '#' && *alias != ';' && ret.s_aliases != NULL ) {
        i = -1;
        do {
            i++;
            if( *alias == '#' || *alias == ';' )
                break;

            ret.s_aliases[i] = strtok_r( NULL, " \t", &buf_ptr );

            if( ret.s_aliases[i] == NULL ) {
                break;
            } else if( ret.s_aliases[i][0] == '\0' ) {
                ret.s_aliases[i] = NULL;
                break;
            }

        } while( i < MAX_SERV_ALIASES );

    }

    return( &ret );
}

_WCRTLINK void setservent( int stayopen )
{
    __servClose = ( stayopen == 0 );

    if( __servFile == NULL ) {
        __servFile = fopen( __servFname, "rt" );
    } else {
        rewind( __servFile );
    }
}

_WCRTLINK void endservent( void )
{
    if( __servFile != NULL ) {
        fclose( __servFile );
        __servFile = NULL;
        __servClose = 1;
    }
}
