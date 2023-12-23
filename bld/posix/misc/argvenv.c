/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
#include "argvstr.h"
#include "fnutils.h"


char **ExpandEnv( int *oargc, char *oargv[], const char *env_name )
{
    int         argc;
    int         i;
    int         n;
    char        *envstr;
    char        *p;
    char        **argv;
    size_t      str_len;

    argc = 0;
    str_len = 0;
    /*
     * if defined extra environment variable then
     * put it on begining of arguments list
     */
    if( env_name != NULL ) {
        envstr = getenv( env_name );
        if( envstr != NULL ) {
            n = ParseStr( envstr, NULL, NULL );
            str_len += strlen( envstr ) + n;
            argc += n;
        }
    }
    /*
     * process arguments list and expand
     * "@<env variable name>" items if it exists
     */
    argc++; // argv[0]
    for( i = 1; i < *oargc; i++ ) {
        if( *oargv[i] == '@' ) {
            envstr = getenv( oargv[i] + 1 );
            if( envstr != NULL ) {
                n = ParseStr( envstr, NULL, NULL );
                str_len += strlen( envstr ) + n;
                argc += n;
                continue;
            }
        }
        argc++;
    }

    /*
     * allocate new arguments list
     * after arguments list new strings are added
     */
    argv = MemAlloc( ( argc + 1 ) * sizeof( char * ) + str_len );
    p = (char *)( argv + argc + 1 );
    /*
     * copy reference for argv[0] as first
     */
    argv[0] = oargv[0];
    argc = 1;
    /*
     * copy extra environment variable in front of arguments list
     * but after argv[0]
     */
    if( env_name != NULL ) {
        envstr = getenv( env_name );
        if( envstr != NULL ) {
            n = ParseStr( envstr, &argv[argc], p );
            p += strlen( envstr ) + n;
            argc += n;
        }
    }
    /*
     * copy arguments list items and expand
     * "@<env variable name>" items
     */
    for( i = 1; i < *oargc; i++ ) {
        if( *oargv[i] == '@' ) {
            envstr = getenv( oargv[i] + 1 );
            if( envstr != NULL ) {
                n = ParseStr( envstr, &argv[argc], p );
                p += strlen( envstr ) + n;
                argc += n;
                continue;
            }
        }
        argv[argc] = oargv[i];
        argc++;
    }
    /*
     * add last NULL item
     */
    argv[argc] = NULL;
    *oargc = argc;
    return( argv );
}
