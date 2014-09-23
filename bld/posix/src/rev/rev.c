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
* Description:  POSIX rev utility
*               Reverses character order of files
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "misc.h"
#include "getopt.h"
#include "argvrx.h"
#include "argvenv.h"

#define  MIN_LINE_LEN   80

char *OptEnvVar="rev";

static const char *usageMsg[] = {
    "Usage: rev [-?X] [@env] [files...]",
    "\tenv                : environment variable to expand",
    "\tfiles              : reverse character order of specified files",
    "\tOptions: -?        : display this message",
    "\t\t -X        : match files by regular expressions",
    NULL
};

/*
 * Local functions.
 */

static void reverseFile( FILE *fp )
{
    int         ch = 0;
    unsigned    os = 0;
    unsigned    size;
    char       *buff;

    size = MIN_LINE_LEN * sizeof( char );
    buff = (char *) malloc( size );

    for( ;; ) {
        if( os >= size - 1 ) {
            size += MIN_LINE_LEN * sizeof( char );
            buff  = (char *) realloc( buff, size );
        }
        ch = fgetc( fp );

        if( ch == EOF ) {
            break;
        } else if( (char) ch == '\n' ) {
            for( ; os >= 1; os-- ) {
                fputc( buff[ os - 1 ], stdout );
            }
            fputc( '\n', stdout );
        } else {
            buff[ os ] = (char) ch;
            os++;
        }
    }

    free( buff );
}

void main( int argc, char **argv )
{
    FILE       *fp;
    int         regexp;

    argv = ExpandEnv( &argc, argv );
    if( GetOpt( &argc, argv, "X", usageMsg ) == 'X' ) {
        regexp = 1;
    } else {
        regexp = 0;
    }

    argv = ExpandArgv( &argc, argv, regexp );
    argv++;
    if( argv[0] == NULL ) {
        reverseFile( stdin );
    } else {
        while( *argv != NULL ) {
            fp = fopen( *argv, "r" );
            if( fp == NULL ) {
                fprintf( stderr, "rev: cannot open input file \"%s\"\n", *argv );
            } else {
                if( argc > 2 ) {
                    fprintf( stdout, "%s:\n", *argv );
                }
                reverseFile( fp );
                fclose( fp );
            }
            argv++;
        }
    }
}
