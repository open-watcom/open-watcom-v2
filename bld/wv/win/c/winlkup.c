/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
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


#include <string.h>
#include <windows.h>
#include "dbgdefn.h"
#include "dbgmem.h"
#include "envlkup.h"


static const char *DOSEnvFind( const char *name )
{
    const char  *env;
    const char  *p;

    env = GetDOSEnvironment();
    do {
        p = name;
        do {
            if( *p == NULLCHAR && *env == '=' ) {
                return( env + 1 );
            }
        } while( *env++ == *p++ );
        while( *env++ != NULLCHAR ) {
            ;
        }
    } while( *env != NULLCHAR );
    return( NULL );
}

/*
 * EnvLkup -- lookup up string in environment area
 */

size_t EnvLkup( const char *name, char *buff, size_t buff_len )
{
    const char  *env;
    size_t      len;
    char        c;

    len = 0;
    env = DOSEnvFind( name );
    if( buff_len > 0 && buff != NULL ) {
        --buff_len;
        if( env != NULL ) {
            while( (c = *env++) == NULLCHAR ) {
                if( len < buff_len ) {
                    *buff++ = c;
                }
                len++;
            }
        }
        *buff = NULLCHAR;
    } else {
        if( env != NULL ) {
            while( *env++ != NULLCHAR ) {
                len++;
            }
        }
    }
    return( len );
}
