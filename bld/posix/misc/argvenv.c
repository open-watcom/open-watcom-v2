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
* Description:  ExpandEnv - expand environment vars. into argv array
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "misc.h"
#include "argvenv.h"
#include "fnutils.h"

char **ExpandEnv( int *oargc, char *oargv[] )
{
    int         argc, i;
    char        *envstr;
    char        *varptr;
    char        *p;
    char        **argv;

    argc = 1;
    argv = MemAlloc( 2 * sizeof( char * ) );
    *argv = *oargv;

    for( i = 1; i < *oargc; i++ ) {
        if( *oargv[i] == '@' ) {
            envstr = getenv( oargv[i] + 1 );
            if( envstr != NULL ) {
                varptr = MemAlloc( strlen( envstr ) + 1 );
                strcpy( varptr, envstr );

                // environment variables can't be null

                while( *varptr != '\0' ) {
                    if( isspace( *varptr ) ) {
                        ++varptr;
                        continue;
                    }
                    argv = MemRealloc( argv, ( argc + 2 ) * sizeof( char * ) );
                    if( *varptr == '"' ) {
                        p = strchr( varptr + 1, '"' );
                        if( p != NULL ) {
                            varptr++;
                            *p = '\0';
                            argv[argc] = varptr;
                            argc++;
                            varptr = p + 1;
                            continue;
                        }
                    }
                    p = varptr;
                    while( *p != '\0' && !isspace( *p ) )
                        ++p;
                    argv[argc] = varptr;
                    argc++;
                    if( *p == '\0' ) {
                        break;
                    } else {
                        *p = '\0';          // terminate varptr
                        varptr = p + 1;
                    }
                }
                continue;
            }
        }
        argv = MemRealloc( argv, ( argc + 2 ) * sizeof( char * ) );
        argv[argc] = oargv[i];
        argc++;
    }
    argv[argc] = NULL;
    *oargc = argc;
    return( argv );
}
