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

char *OptEnvVar="split";

static char *usageMsg[] = {
    "Usage: split [-?] [-number] [@env] file [prefix]",
    "\tenv                : environment variable to expand",
    "\tfile               : name of file to split",
    "\tprefix             : characters prepended to output file names",
    "\tOptions: -?        : display this message",
    "\t\t -number   : number of lines per output file (default 1000)",
    NULL
};

/*
 * Local functions.
 */

static int splitFile( FILE *fp, int lines, char *prefix )
{
    int             ch;
    FILE           *out  = NULL;
    char           *file = NULL;
    char            more = 1;
    unsigned        cnt  = 0;
    unsigned char   e1   = 'a';
    unsigned char   e2   = 'a';

    file = (char *) malloc( (strlen( prefix ) + 2) * sizeof( char ) + 1 );

    while( 1 ) {
        ch = fgetc( fp );

        if( ch == EOF ) {                   // look for end of file.
            break;
        } else if( cnt == 0  &&  more ) {

            if( out != NULL ) {
                fclose( out );
            }

            sprintf( file, "%s%c%c", prefix, e1, e2 );  // create and open
            out = fopen( file, "w" );                   // next split file

            if( out != NULL ) {                         // check for error
                if( e2 == 'z' ) {                       // increment suffix
                    if( e1 != 'z' ) {
                        e1++;
                        e2 = 'a';
                    } else {
                        more = 0;
                    }
                } else {
                    e2++;
                }
                cnt = 1;                                // current line = 1
            } else {
                free( file );                           // error case
                return( 1 );
            }
        }
        fputc( ch, out );                               // output to file
        if( ch == '\n' ) {
            cnt++;
            if( cnt > lines ) {                         // time to switch files
                cnt = 0;
            }
        }
    }
    fclose( out );
    free( file );
    return( 0 );
}

void main( int argc, char **argv )
{
    FILE       *fp;
    int         ch;

    char       *prefix = "x";
    int         lines  = 1000;

    argv = ExpandEnv( &argc, argv );

    while( 1 ) {
        ch = GetOpt( &argc, argv, "#", usageMsg );
        if( ch == -1 ) {
            break;
        } else if( ch == '#' ) {
            lines = atoi( OptArg );
        }
    }

    if( argc == 1  ||  argc > 3 ) {
        Die( "%s\n", usageMsg[0] );
    } else {
        if( argc == 3 ) {
            prefix = argv[2];
        }
        if( lines <= 0 ) {
            Die( "split: invalid number of lines per file\n" );
        }
        if( !strcmp( argv[1], "-" ) ) {
            if( splitFile( stdin, lines, prefix ) ) {
                Die( "split: cannot open output file\n" );
            }
        } else {
            fp = fopen( argv[1], "r" );
            if( fp == NULL ) {
                Die( "split: cannot open input file \"%s\"\n", argv[1] );
            }
            if( splitFile( fp, lines, prefix ) ) {
                Die( "split: cannot open output file\n" );
            }
            fclose( fp );
        }
    }
}
