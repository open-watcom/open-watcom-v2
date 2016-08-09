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


#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "getopt.h"
#include "misc.h"
#include "argvrx.h"

char *OptEnvVar = "STRINGS";

#define STATIC  static

STATIC const char *usageTxt[] = {
    "Usage: strings [-?xX] [-num] [files...]",
    "\tfiles         : files to display the printable strings from",
    "\tOptions: -?   : display this message",
    "\t\t -x   : print offsets in hex",
    "\t\t -X   : match files by regular expression",
    "\t\t -num : print only strings at least 'num' bytes in length",
    "\t\t        (default is 4 bytes)",
    NULL
};


STATIC  unsigned        threshold;
STATIC  char            *thresholdBuf;
STATIC  int             gimmeHexOffsets;


STATIC void doStrings( FILE *fp ) {

    static char errmsg[] = "error writing stdout: %s\n";
    int         ch;
    unsigned    amt_printable;
    unsigned long offset;

    offset = 0;
    for(;;) {
        amt_printable = 0;
        for(;;) {
            ch = fgetc( fp );
            ++offset;
            if( ch == EOF ) break;
            if( isprint( ch ) ) {
                thresholdBuf[ amt_printable ] = ch;
                ++amt_printable;
                if( amt_printable == threshold ) break;
            } else {
                amt_printable = 0;
            }
        }
        if( ch == EOF ) break;
        thresholdBuf[ amt_printable ] = 0;
        if( gimmeHexOffsets ) {
            if( printf( "% 8x: %s", offset, thresholdBuf ) < 0 ) {
                Die( errmsg, strerror( errno ) );
            }
        } else {
            if( fputs( thresholdBuf, stdout ) == EOF ) {
                Die( errmsg, strerror( errno ) );
            }
        }
        for(;;) {
            ch = fgetc( fp );
            ++offset;
            if( ch == EOF ) break;
            if( !isprint( ch ) ) break;
            if( putchar( ch ) == EOF ) {
                Die( errmsg, strerror( errno ) );
            }
        }
        if( putchar( '\n' ) == EOF ) {
            Die( errmsg, strerror( errno ) );
        }
        if( ch == EOF ) break;
    }
}


void main( int argc, char **argv ) {

    int         rxflag;
    int         ch;
    int         i;
    FILE        *fp;

    rxflag = 0;
    threshold = 4;
    gimmeHexOffsets = 0;
    for(;;) {
        ch = GetOpt( &argc, argv, "#xX", usageTxt );
        if( ch == -1 ) break;
        switch( ch ) {
        case '#':
            threshold = strtoul( OptArg, NULL, 0 );
            if( threshold == 0 ) {
                Quit( usageTxt, "'num' must be positive\n" );
            }
            break;
        case 'x':
            gimmeHexOffsets = 1;
            break;
        case 'X':
            rxflag = 1;
            break;
        }
    }

    thresholdBuf = malloc( threshold + 1 );
    if( thresholdBuf == NULL ) {
        Die( "not enough memory\n" );
    }

    argv = ExpandArgv( &argc, argv, rxflag );

    if( argc == 1 ) {
        /* FIXME: stdin is in TEXT mode... */
        doStrings( stdin );
    } else {
        for( i = 1; i < argc; ++i ) {
            if( argv[i][0] == '-' && argv[i][1] == 0 ) {
                doStrings( stdin );
            } else {
                fp = fopen( argv[i], "rb" );
                if( fp == NULL ) {
                    Die( "unable to open %s: %s\n", argv[i], strerror( errno ) );
                }
                doStrings( fp );
                fclose( fp );
            }
        }
    }
    exit( 0 );
}
