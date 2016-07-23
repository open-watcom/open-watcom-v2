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


#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "getopt.h"
#include "misc.h"
#include "argvrx.h"

char *OptEnvVar = "fold";

const char *usageTxt[] = {
    "Usage: fold [-?X] [-<width>] [files]",
    "\tfiles             : files to fold",
    "\tOptions: -?       : print this list",
    "\t\t -<width> : maximum length of output lines",
    "\t\t -X       : match files by regular expressions",
    NULL
};

size_t  lineWidth;
char    *lineBuffer;

static void fold( FILE *fh ) {

    int         ch;
    int         offs;

    offs = 0;
    for(;;) {
        ch = fgetc( fh );
        if( ch == EOF ) break;
        if( ch == '\n' || offs == lineWidth ) {
            lineBuffer[ offs ] = 0;
            puts( lineBuffer );
            offs = 0;
        }
        if( ch != '\n' ) {
            lineBuffer[ offs++ ] = ch;
        }
    }
    if( offs > 0 ) {
        lineBuffer[ offs ] = 0;
        puts( lineBuffer );
    }
}


void main( int argc, char **argv ) {

    int         ch;
    FILE        *fh;
    int         i;
    int         rxflag;

    lineWidth = 70;
    rxflag = 0;
    for(;;) {
        ch = GetOpt( &argc, argv, "#", usageTxt );
        if( ch == -1 ) break;
        if( ch == 'X' ) {
            rxflag = 1;
        }
        if( ch == '#' ) {
            lineWidth = strtoul( OptArg, NULL, 10 );
            if( lineWidth == 0 ) {
                Die( "width must be positive\n" );
            }
        }
    }
    lineBuffer = malloc( lineWidth+1 );
    if( lineBuffer == NULL ) {
        Die( "not enough memory to hold a line\n" );
    }

    argv = ExpandArgv( &argc, argv, rxflag );

    if( argc < 2 ) {
        fold( stdin );
    } else {
        for( i = 1; i < argc; ++i ) {
            fh = fopen( argv[ i ], "rt" );
            if( fh == NULL ) {
                Die( "error opening %s: %s\n", argv[i], strerror( errno ) );
            }
            fold( fh );
            fclose( fh );
        }
    }
}
