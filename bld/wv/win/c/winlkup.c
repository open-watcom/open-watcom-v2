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


#include "dbgdefn.h"
#include "dbgmem.h"
#include <string.h>
#include "windows.h"


#ifndef __NT__
char *DOSEnvFind( char *src )
{
    char        *env;
    char        *p;

    env = GetDOSEnvironment();
    do {
        p = src;
        do {
            if( *p == NULLCHAR && *env == '=' ) return( env + 1 );
        } while( *env++ == *p++ );
        while( *env++ != NULLCHAR )
            ;
    } while( *env != NULLCHAR );
    return( NULL );
}

/*
 * EnvLkup -- lookup up string in environment area
 */

int EnvLkup( char *src, char *dst, int max_len )
{
    int         len;
    char        *env;

    max_len = max_len; // nyi obey
    env = DOSEnvFind( src );
    if( env == NULL ) {
        *dst = NULLCHAR;
        return( 0 );
    }
    for( len = 0; ( *dst++ = *env++ ) != NULLCHAR; len++ ) {}
    return( len );
}
#else
/*
 * EnvLkup -- lookup up string in environment area
 */

int EnvLkup( char *src, char *dst, int max_len )
{

    return( GetEnvironmentVariable( src, dst, max_len ) );
}
#endif
