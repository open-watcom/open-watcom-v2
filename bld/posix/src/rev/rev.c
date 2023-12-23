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
* Description:  POSIX rev utility
*               Reverses character order of files
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "bool.h"
#include "misc.h"
#include "getopt.h"
#include "argvrx.h"
#include "argvenv.h"


#define  MIN_LINE_LEN   80

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
    buff = (char *)MemAlloc( size );

    for( ;; ) {
        if( os >= size - 1 ) {
            size += MIN_LINE_LEN * sizeof( char );
            buff  = (char *)MemRealloc( buff, size );
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

    MemFree( buff );
}

int main( int argc, char **argv )
{
    FILE    *fp;
    bool    regexp;
    int     i;
    char    **argv1;

    argv1 = ExpandEnv( &argc, argv, "REV" );

    regexp = ( GetOpt( &argc, argv1, "X", usageMsg ) == 'X' );

    argv = ExpandArgv( &argc, argv1, regexp );
    if( argc < 2 ) {
        reverseFile( stdin );
    } else {
        for( i = 1; i < argc; i++ ) {
            fp = fopen( argv[i], "r" );
            if( fp == NULL ) {
                fprintf( stderr, "rev: cannot open input file \"%s\"\n", argv[i] );
            } else {
                if( argc > 2 ) {
                    fprintf( stdout, "%s:\n", argv[i] );
                }
                reverseFile( fp );
                fclose( fp );
            }
        }
    }
    MemFree( argv );
    MemFree( argv1 );

    return( 0 );
}
