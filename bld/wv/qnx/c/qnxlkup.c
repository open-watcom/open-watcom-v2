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
#include <process.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

char *copy_char_to_buff( char *ptr, char c, char **end )
{
    if( *end == NULL )
        return( ptr + 1 );
    if( ptr < *end ) {
        *ptr++ = c;
        *ptr = NULLCHAR;
    }
    return( ptr );
}

char *copy_str_to_buff( char *ptr, char *str, char **end )
{
    int len;

    len = strlen( str );
    if( *end == NULL )
        return( ptr + len );
    while( len-- && ptr < *end ) {
        *ptr++ = *src++;
    }
    *ptr = NULLCHAR;
    return( ptr );
}

unsigned EnvLkup( char *name, char *buff, unsigned buff_len )
{
    char        *env;
    char        *ptr;
    char        *p;
    char        cmd[_MAX_PATH];
    char        *str;
    char        *end = NULL;

    if( buff == NULL ) {
        buff = cmd;
    } else if( buff_len != 0 ) {
        end = buff + buff_len - 1;
    }
    ptr = str = buff;
    /* if we're asking for the PATH variable, we really want to know where
        to search for our support files */
    if( strcmp( name, "PATH" ) == 0 ) {
        /* look in current directory */
        ptr = copy_char_to_buff( ptr, '.', &end );
        ptr = copy_char_to_buff( ptr, ':', &end );
        env = getenv( "WD_PATH" );
        if( env != NULL ) {
            /* look in WD_PATH dirs */
            ptr = copy_str_to_buff( ptr, env, &end );
            ptr = copy_char_to_buff( ptr, ':', &end );
        }
        env = getenv( "HOME" );
        if( env != NULL ) {
            /* look in HOME dir */
            ptr = copy_str_to_buff( ptr, env, &end );
            ptr = copy_char_to_buff( ptr, ':', &end );
        }
        if( _cmdname( cmd ) != NULL ) {
            p = strrchr( cmd, '/' );
            if( p != NULL ) {
                *p = NULLCHAR;
                /* look in the executable's directory */
                ptr = copy_str_to_buff( ptr, cmd, &end );
                ptr = copy_char_to_buff( ptr, ':', &end );
                p = strrchr( cmd, '/' );
                if( p != NULL ) {
                    /* look in a sibling directory of where the executable is */
                    memcpy( p + 1, "wd", 3 );
                    ptr = copy_str_to_buff( ptr, cmd, &end );
                    ptr = copy_char_to_buff( ptr, ':', &end );
                }
            }
        }
        /* look in "/opt/watcom/wd" */
        ptr = copy_str_to_buff( ptr, "/opt/watcom/wd", &end );
        return( ptr - str );
    }
    env = getenv( name );
    if( env != NULL ) {
        ptr = copy_str_to_buff( ptr, env, &end );
    }
    return( ptr - str );
}
