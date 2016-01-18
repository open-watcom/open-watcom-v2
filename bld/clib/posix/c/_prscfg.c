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
* Description:  Helper function for retrieving significant lines from a
*               POSIX-like configuration file
*
****************************************************************************/

#include "variety.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include "_prscfg.h"

ssize_t __getconfigline( char **s, size_t *n, FILE *fp )
{
    char    *buf;
    int     i;

    do {
        if( getline( s, n, fp ) < 0 ) {
            return( -1 );
        }

        /* Remove newlines */
        buf = strchr( *s, '\n' );
        if( buf != NULL )
            buf[0] = '\0';

        /* Remove comments */
        buf = strchr( *s, '#' );
        if( buf != NULL )
            buf[0] = '\0';

        /* Left-trim the resultant string */
        while( **s == ' ' || **s == '\t' ) {
            buf = *s;
            for( i = 1; i <= strlen( buf ); i++ ) {
                buf[i - 1] = buf[i];
            }
        }

    } while( **s == '\0' || **s == '#' || **s == ';' );

    return( (ssize_t)*n );
}
