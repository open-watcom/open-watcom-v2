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
* Description:  Environment variable lookup for Linux.
*
****************************************************************************/


#include "dbgdefn.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#ifdef __WATCOMC__
    #include <process.h>
#endif

extern char     *StrCopy( char *, char * );

unsigned EnvLkup( char *name, char *buff, int max_len )
{
    char        *env;
    char        *ptr;
    char        *p;
    char        cmd[_MAX_PATH];

    max_len = max_len; // nyi obey
    /* if we're asking for the PATH variable, we really want to know where
        to search for our support files */
    if( strcmp( name, "PATH" ) == 0 ) {
        ptr = buff;
        *ptr++ = ':';                           /* look in current directory */
        env = getenv( "WD_PATH" );
        if( env != NULL ) {
            ptr = StrCopy( env, ptr );          /* look in WD_PATH dirs */
            *ptr++ =':';
        }
        env = getenv( "HOME" );
        if( env != NULL ) {
            ptr = StrCopy( env, ptr );          /* look in HOME dir */
            *ptr++ = ':';
        }
        if( _cmdname( cmd ) != NULL ) {
            p = strrchr( cmd, '/' );
            if( p != NULL ) {
                *p = NULLCHAR;
                /* look in the executable's directory */
                ptr = StrCopy( cmd, ptr );
                *ptr++ = ':';
                p = strrchr( cmd, '/' );
                if( p != NULL ) {
                    /* look in a sibling directory of where the executable is */
                    p = StrCopy( "wd", p + 1 );
                    ptr = StrCopy( cmd, ptr );
                    *ptr++ = ':';
                }
            }
        }
        ptr = StrCopy( "/opt/watcom/wd", ptr );    /* look in "/opt/watcom/wd" */
        return( ptr - buff );
    }
    env = getenv( name );
    if( env == NULL ) return( 0 );
    return( StrCopy( env, buff ) - buff );
}
