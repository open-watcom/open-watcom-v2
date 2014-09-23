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
* Description:  POSIX basename utility
*               Determines base portion of a pathname
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "misc.h"
#include "getopt.h"
#include "argvrx.h"
#include "argvenv.h"

char *OptEnvVar = "basename";

static const char *usageMsg[] = {
    "Usage: basename [-?] [@env] filename [suffix]",
    "\tenv                : environment variable to expand",
    "\tfilename           : name of file to strip",
    "\tsuffix             : suffix to be stripped",
    "\tOptions: -?        : display this message",
    NULL
};

void main( int argc, char **argv )
{
    char        *fname, *src, *ext;
    size_t      len;

    argv = ExpandEnv( &argc, argv );

    GetOpt( &argc, argv, "", usageMsg );

    if( argc < 2  ||  argc > 3 ) {
        Die( "%s\n", usageMsg[0] );
    } else {
        len = strlen( argv[1] );
        fname  = (char *)malloc( 2 * len * sizeof( char ) + 1 );

        _splitpath( argv[1], NULL, NULL, fname, fname + len );
        strcat( fname, fname + len );

        if( argc == 3 ) {
            src = fname + strlen( fname );
            ext = argv[2] + strlen( argv[2] );

            for( ; *src == *ext && ext >= argv[2]; src--, ext-- );

            if( ext < argv[2] ) {
                *(src + 1) = '\0';
            }
        }
        fprintf( stdout, "%s\n", fname );

        free( fname );
    }
}
