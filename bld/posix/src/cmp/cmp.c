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
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "getopt.h"
#include "misc.h"
#include "argvenv.h"

char *usageTxt[] = {
    "Usage:\tcmp [-?lsx] [@env] file1 file2 [seek1 [seek2]]",
    "\tenv         : environment variable to expand",
    "\tfile1 file2 : files to compare. One of the files may be `-' for",
    "\t\t      the standard input.",
    "\tOptions: -? : print this list",
    "\t\t -l : Keep going after finding a difference",
    "\t\t -s : Say nothing, return an error status",
    "\t\t -x : Display differences in hex",
    NULL
};

char *OptEnvVar = "cmp";

int     flagKeepGoing;
int     flagSayNothing;
int     flagSayItInHex;

#define BUFFER_SIZE     16384
char buffer[ 2 ][ BUFFER_SIZE ];


int cmp( int fh[2], char *names[2], long offs[2] ) {

    int         i;
    const char  *fmt;
    int         bytes_read[2];
    size_t      amt;

    for(;;) {
        bytes_read[0] = read( fh[0], &buffer[0][0], BUFFER_SIZE );
        if( bytes_read[0] == -1 ) {
            Die( "error reading %s: %s\n", names[0], strerror( errno ) );
        }
        bytes_read[1] = read( fh[1], &buffer[1][0], BUFFER_SIZE );
        if( bytes_read[1] == -1 ) {
            Die( "error reading %s: %s\n", names[1], strerror( errno ) );
        }
        amt = bytes_read[0] < bytes_read[1] ? bytes_read[0] : bytes_read[1];
        if( memcmp( &buffer[0][0], &buffer[1][0], amt ) ) {
            for( i = 0; i < amt; ++i ) {
                if( buffer[0][i] != buffer[1][i] ) {
                    if( !flagSayNothing ) {
                        if( offs[0] == offs[1] ) {
                            if( flagSayItInHex ) {
                                fmt = "offset %08lx: %s=%02x, %s=%02x\n";
                            } else {
                                fmt = "offset %lu: %s=%u, %s=%u\n";
                            }
                            printf( fmt, offs[0]+i, names[0], buffer[0][i],
                                                    names[1], buffer[1][i] );
                        } else {
                            if( flagSayItInHex ) {
                                fmt = "offset %08lx %s=%02x, offset %08lx %s=%02x\n";
                            } else {
                                fmt = "offset %lu %s=%u, offset %lu %s=%u\n";
                            }
                            printf( fmt, offs[0]+i, names[0], buffer[0][i],
                                         offs[1]+i, names[1], buffer[1][i] );
                        }
                    }
                    if( !flagKeepGoing ) return( 1 );
                }
            }
        }
        offs[0] += bytes_read[0];
        offs[1] += bytes_read[1];
        if( bytes_read[0] != bytes_read[1] ) {
            if( !flagSayNothing ) {
                if( flagSayItInHex ) {
                    fmt = "%s ends prematurely at offset %08lx\n";
                } else {
                    fmt = "%s ends prematurely at offset %u\n";
                }
                if( bytes_read[0] < bytes_read[1] ) {
                    printf( fmt, names[0], offs[0] );
                } else {
                    printf( fmt, names[1], offs[1] );
                }
            }
            return( 1 );
        }
        if( bytes_read[0] == 0 ) break;
    }
    return( 0 );
}

void main( int argc, char **argv ) {

    int         ch;
    long        offs[2];
    int         i;
    int         fh[2];

    argv = ExpandEnv( &argc, argv );

    for(;;) {
        ch = GetOpt( &argc, argv, "lsx", usageTxt );
        if( ch == -1 ) break;
        switch( ch ) {
        case 'l':
            flagKeepGoing = 1;
            break;
        case 's':
            flagSayNothing = 1;
            break;
        case 'x':
            flagSayItInHex = 1;
            break;
        }
    }
    if( argc < 3 || argc > 5 ) {
        Quit( usageTxt, "invalid number of arguments\n" );
    }
    offs[0] = offs[1] = 0;
    if( argc > 3 ) {
        offs[0] = strtol( argv[3], NULL, 0 );
        if( argc > 4 ) {
            offs[1] = strtol( argv[4], NULL, 0 );
        }
    }
    ++argv;             // don't care about argv[0]
    for( i = 0; i < 2; ++i ) {
        if( argv[i][0] == '-' && argv[i][1] == 0 ) {
            if( i == 1 && fh[0] == STDIN_FILENO ) {
                Die( "cmp: only one argument can be stdin\n" );
            }
            fh[i] = STDIN_FILENO;
            argv[i] = "(stdin)";
        } else {
            fh[i] = open( argv[i], O_RDONLY | O_BINARY );
            if( fh[i] == -1 ) {
                Die( "unable to open %s: %s\n", argv[i], strerror( errno ) );
            }
        }
        if( offs[i] ) {
            if( lseek( fh[i], offs[i], SEEK_SET ) == -1L ) {
                Die( "error seeking to %s in %s: %s\n", argv[i+2], argv[i],
                            strerror( errno ) );
            }
        }
    }

    exit( cmp( fh, argv, offs ) );
}
