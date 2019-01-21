/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of inet_network() for RDOS.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "variety.h"
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

_WCRTLINK in_addr_t inet_network( const char *__cp )
{
    unsigned long val, base, n;
    unsigned long parts[4], *pp = parts;
    int i;
    char c;

again:
    /*
     * Collect number up to ``.''.
     * Values are specified as for C:
     * 0x=hex, 0=octal, other=decimal.
     */
    val  = 0;
    base = 10;
    /*
     * The 4.4BSD version of this file also accepts 'x__' as a hexa
     * number.  I don't think this is correct.  -- Uli
     */
    if( *__cp == '0' )
    {
        if( *++__cp == 'x' || *__cp == 'X' )
            base = 16, __cp++;
        else 
            base = 8;
    }
    while( ( c = *__cp ) != 0 )
    {
        if( isdigit( c ) )
        {
            val = (val * base) + (c - '0');
            __cp++;
            continue;
        }
        if( base == 16 && isxdigit( c ) )
        {
            val = (val << 4) + (c + 10 - (islower(c) ? 'a' : 'A'));
            __cp++;
            continue;
        }
        break;
    }
    if( *__cp == '.' )
    {
        if( pp >= parts + 4 )
            return( INADDR_NONE );

        *pp++ = val;
        __cp++;
        goto again;
    }
    if( *__cp && !isspace( *__cp ) )
        return( INADDR_NONE );

    *pp++ = val;
    n = pp - parts;
    if (n > 4)
        return( INADDR_NONE );

    for( val = 0, i = 0; i < n; i++ )
    {
        val <<= 8;
        val |= parts[i] & 0xff;
    }
    return( val );
}
