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



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "misc.h"
#include "getopt.h"
#include "argvrx.h"
#include "argvenv.h"

char *OptEnvVar="expand";

static char *usageMsg[] = {
    "Usage: expand [-?X] [-number[,number...]] [@env] [files...]",
    "\tenv                : environment variable to expand",
    "\tfiles              : files to expand",
    "\tOptions: -?        : display this message",
    "\t\t -number   : set tab stops every 'number' columns (default 8)",
    "\t\t -# list   : set tab stops at given columns (origin 0)",
    "\t\t -X        : match files with regular expressions",
    NULL
};

/*
 * Local functions.
 */

static void expandFile( FILE *fp, unsigned tabs[] )
{
    int         ch = 0;
    unsigned    col = 0, t = 0, stop;

    stop = tabs[ 0 ];

    while( 1 ) {
        if( col >= stop ) {
            if( tabs[ t+1 ] != 0 ) {
                t++;
                stop = tabs[ t ];
            } else if( t == 0 ) {
                stop += tabs[0];
            } else {
                stop += 1;
            }
        }

        ch = fgetc( fp );

        if( ch == EOF ) {
            break;
        } else if( (char) ch == '\n' ) {
            fputc( ch, stdout );
            col = 0;
            stop = tabs[ 0 ];
            t = 0;
        } else if( (char) ch == '\t' ) {
            for( ; col < stop; col++ ) {
                fputc( ' ', stdout );
            }
        } else {
            fputc( ch, stdout );
            col++;
        }
    }
}

static int parseList( char *list, unsigned **tabs )
{
    char        *p;
    unsigned     cnt, size;
    int          num, old;

    if( strcspn( list, "0123456789," ) != 0 ) {
        return( 1 );
    }

    size = 10 * sizeof( unsigned );
    *tabs = (unsigned *) malloc( size );
    cnt = 0;
    old = 0;

    do {
        if( cnt >= size - 1 ) {
            size += 10 * sizeof( unsigned );
            *tabs = (unsigned *) realloc( *tabs, size );
        }
        p = strchr( list, ',' );
        if( p != NULL ) {
            *p = '\0';
        }
        num = atoi( list );
        if( num <= old ) {
            return( 1 );
        }
        *(*tabs + cnt) = num;
        cnt++;
        list = p + 1;
        old = num;
    } while( p != NULL );

    *(*tabs + cnt) = 0;
    return( 0 );
}

void main( int argc, char **argv )
{
    FILE       *fp;
    int         ch, arg;
    char       *list;                   // List of tab stops specified
    unsigned   *tabs;                   // List of fields to be retained.
    int         regexp;

    tabs   = NULL;
    list   = NULL;                      // Setup for realloc.
    arg    = 0;                         // Flag for finding # parameter
    regexp = 0;

    argv = ExpandEnv( &argc, argv );

    while( 1 ) {
        ch = GetOpt( &argc, argv, "#X", usageMsg );
        if( ch == -1 ) {
            break;
        } else if( ch == '#' ) {
            list = (char *) realloc( list, strlen( OptArg )*sizeof( char ) + 1);
            strcpy( list, OptArg );
            arg = 1;
        } else if( ch == 'X' ) {
            regexp = 1;
        }
    }

    if( !arg ) {
        tabs = (unsigned *) malloc( 2 * sizeof( unsigned ) );
        tabs[0] = 8;
        tabs[1] = 0;
    } else if( parseList( list, &tabs ) ) {
        free( tabs );
        free( list );
        Die( "expand: invalid tab stop specification\n" );
    }

    argv = ExpandArgv( &argc, argv, regexp );
    argv++;
    if( argv[0] == NULL ) {
        expandFile( stdin, tabs );
    } else {
        while( *argv != NULL ) {
            fp = fopen( *argv, "r" );
            if( fp == NULL ) {
                fprintf( stderr, "expand: cannot open input file \"%s\"\n", *argv );
            } else {
                if( argc > 2 ) {
                    fprintf( stdout, "%s:\n", *argv );
                }
                expandFile( fp, tabs );
                fclose( fp );
            }
            argv++;
        }
    }
    free( tabs );
    free( list );
}
