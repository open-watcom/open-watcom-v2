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

char *OptEnvVar="head";

static const char *usageMsg[] = {
    "Usage: head [-?X] [-<number>] [@env] [files...]",
    "\tenv                : environment variable to expand",
    "\tfiles              : files to display head lines of",
    "\tOptions: -?        : display this message",
    "\t\t -<number> : number of lines to show",
    "\t\t -X        : match files by regular expressions",
    NULL
};

unsigned head;

static void DumpHead( FILE *fp )
{
    int c;
    unsigned line;

    line = 0;
    for(;;) {
        c = fgetc( fp );
        if( c == EOF ) break;
        putchar( c );
        if( c == '\n' ) {
            ++line;
            if( line == head ) {
                break;
            }
        }
    }
    fclose( fp );
}

void main( int argc, char **argv )
/********************************/
{
    FILE        *fp;
    int         ch;
    int         rxflag;

    argv = ExpandEnv( &argc, argv );

    head = 10;
    rxflag = 0;
    for( ;; ) {
        ch = GetOpt( &argc, argv, "#X", usageMsg );
        if( ch == -1 ) {
            break;
        }
        switch( ch ) {
        case '#':
            head = atoi( OptArg );
            break;
        case 'X':
            rxflag = 1;
            break;
        }
    }

    argv = ExpandArgv( &argc, argv, rxflag );

    argv++;
    if( argv[0] == NULL ) {
        DumpHead( stdin );
    } else {
        while( *argv != NULL ) {
            fp = fopen( *argv, "r" );
            if( fp == NULL ) {
                Die( "%s: could not open\n", *argv );
            }
            if( argc > 2 ) {
                printf( "%s:\n", *argv );
            }
            DumpHead( fp );
            argv++;
        }
    }
}
