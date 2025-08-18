/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementations of getprotoent/setprotoent/endprotoent
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

#define MAX_PROTO_ALIASES  16

static const char   *__protFname = "/etc/protocols";
static FILE         *__protFile;
static int          __protClose = 1;

/*
 * Return the next (non-commented) line from the services-file
 * Format is:
 *  ip-address [=] host-name [alias..] {\n | # ..}
 */
_WCRTLINK struct protoent *getprotoent( void )
{
    static struct protoent  ret;
    char                    *alias;

    /* For loading a line from the services file */
    static char             *buf = NULL;
    static size_t           buflen = 0;
    char                    *buf_ptr;

    /* Translating the port string */
    char *protonum;


    /* For line parsing */
    int i;

    if( __protFile == NULL )
        setprotoent( 0 );
    if( __protFile == NULL )
        return NULL;

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

    if( __getconfigline( &buf, &buflen, __protFile ) < 0)
        return( NULL );

    if( __protClose )
        endprotoent();

    /* Name */
    ret.p_name = strtok_r( buf, " \t", &buf_ptr );

    /* Protocol number */
    protonum = strtok_r( NULL, " \t", &buf_ptr );
    if(protonum == NULL)
        return( NULL );
    ret.p_proto = atoi( protonum );

    /* Load in any aliases */
    if(ret.p_aliases == NULL)
        ret.p_aliases = (char **)malloc( (1 + MAX_PROTO_ALIASES)*sizeof( char * ) );

    /* Explcitly NULL the entries in a nice, readable manner */
    for( i = 0; i < (1 + MAX_PROTO_ALIASES) && ret.p_aliases != NULL; i++ )
        ret.p_aliases[i] = NULL;

    alias = strtok_r( NULL, " \t", &buf_ptr );

    if( alias && *alias != '#' && *alias != ';' && ret.p_aliases != NULL ) {
        i = -1;
        do {
            i++;
            if( *alias == '#' || *alias == ';' )
                break;

            ret.p_aliases[i] = strtok_r( NULL, " \t", &buf_ptr );

            if( ret.p_aliases[i] == NULL ) {
                break;
            } else if( ret.p_aliases[i][0] == '\0' ) {
                ret.p_aliases[i] = NULL;
                break;
            }

        } while( i < MAX_PROTO_ALIASES );

    }

    return( &ret );
}

_WCRTLINK void setprotoent( int stayopen )
{
    __protClose = ( stayopen == 0 );

    if( __protFile == NULL ) {
        __protFile = fopen( __protFname, "rt" );
    } else {
        rewind( __protFile );
    }
}

_WCRTLINK void endprotoent( void )
{
    if( __protFile != NULL ) {
        fclose( __protFile );
        __protFile = NULL;
        __protClose = 1;
    }
}
