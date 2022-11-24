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
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "bool.h"
#include "misc.h"
#include "getopt.h"
#include "argvrx.h"
#include "argvenv.h"


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

int main( int argc, char **argv )
/*******************************/
{
    FILE        *fp;
    int         ch;
    bool        rxflag;
    int         i;
    char        **argv1;

    argv1 = ExpandEnv( &argc, argv, "HEAD" );

    head = 10;
    rxflag = false;
    for( ;; ) {
        ch = GetOpt( &argc, argv1, "#X", usageMsg );
        if( ch == -1 ) {
            break;
        }
        switch( ch ) {
        case '#':
            head = atoi( OptArg );
            break;
        case 'X':
            rxflag = true;
            break;
        }
    }

    argv = ExpandArgv( &argc, argv1, rxflag );
    if( argc < 2 ) {
        DumpHead( stdin );
    } else {
        for( i = 1; i < argc; i++ ) {
            fp = fopen( argv[i], "r" );
            if( fp == NULL ) {
                Die( "%s: could not open\n", argv[i] );
            }
            if( argc > 2 ) {
                printf( "%s:\n", argv[i] );
            }
            DumpHead( fp );
        }
    }
    MemFree( argv );
    MemFree( argv1 );

    return( 0 );
}
