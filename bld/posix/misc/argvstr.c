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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <stdlib.h>
#include <ctype.h>
#include "bool.h"
#include "argvstr.h"


int     ParseStr( const char *str, char **argv, char *buf )
/*********************************************************/
{
    /*
     * Returns a count of the "command line" parameters in *str.
     * Unless argv is NULL, both argv and buf are completed.
     *
     * This function ought to be fairly similar to clib(initargv@_SplitParms).
     * Parameterisation does the same as _SplitParms with historical = 0.
     */

    const char  *start;
    int         argc;
    char        *bufend;
    bool        got_quote;

    bufend = buf;
    argc = 0;
    while( *str != '\0' ) {
        got_quote = false;
        while( isspace( *str ) )
            str++;
        start = str;
        if( buf != NULL ) {
            argv[argc] = bufend;
        }
        while( ( got_quote || !isspace( *str ) ) && *str != '\0' ) {
            if( *str == '\"' ) {
                got_quote = !got_quote;
            }
            if( buf != NULL ) {
                *bufend = *str;
                bufend++;
            }
            str++;
        }
        if( start != str ) {
            argc++;
            if( buf != NULL ) {
                *bufend = '\0';
                bufend++;
            }
        }
    }
    return( argc );
}
