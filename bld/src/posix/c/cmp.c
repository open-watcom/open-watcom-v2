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
* Description:  Compare binary files and report differences.
*
****************************************************************************/


#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"


static const char *usage_text[] = {
    "Usage:\tcmp [-?lsx] file1 file2 [seek1 [seek2]]",
    "\tfile1 file2 : files to compare. One of the files may be `-' for",
    "\t\t      the standard input.",
    "\tOptions:",
    "\t\t -? : print this list",
    "\t\t -l : display all differences (in octal)",
    "\t\t -s : say nothing, return an error status",
    "\t\t -x : display all differences (in hex)",
    NULL
};

static int  keep_going;
static int  say_nothing;
static int  say_it_in_hex;
static int  say_it_in_oct;
static int  count_lines;

#define BUFFER_SIZE     8192
static char buffer[2][BUFFER_SIZE];


static int cmp( int fh[2], char *names[2], long offs[2] )
/*******************************************************/
{
    int         i;
    const char  *fmt;
    int         bytes_read[2];
    size_t      amt;
    long        line_num = 1;
    int         files_differ = 0;

    for( ;; ) {
        bytes_read[0] = read( fh[0], &buffer[0][0], BUFFER_SIZE );
        if( bytes_read[0] == -1 ) {
            util_die( "cmp: error reading %s: %s\n", names[0], strerror( errno ) );
        }
        bytes_read[1] = read( fh[1], &buffer[1][0], BUFFER_SIZE );
        if( bytes_read[1] == -1 ) {
            util_die( "cmp: error reading %s: %s\n", names[1], strerror( errno ) );
        }
        amt = bytes_read[0] < bytes_read[1] ? bytes_read[0] : bytes_read[1];
        if( memcmp( &buffer[0][0], &buffer[1][0], amt ) ) {
            for( i = 0; i < amt; ++i ) {
                if( count_lines && buffer[0][i] == '\n' ) {
                    ++line_num;
                }
                if( buffer[0][i] != buffer[1][i] ) {
                    if( !say_nothing ) {
                        if( offs[0] == offs[1] ) {
                            if( say_it_in_hex ) {
                                printf( "%08lx: %02x %02x\n", offs[0] + i,
                                        buffer[0][i], buffer[1][i] );
                            } else if( say_it_in_oct ) {
                                printf( "%d %o %o\n", offs[0] + i + 1,
                                        buffer[0][i], buffer[1][i] );
                            } else {
                                printf( "%s %s differ: char %ld, line %ld\n",
                                        names[0], names[1], offs[0] + i + 1, line_num );
                            }
                        } else {
                            if( say_it_in_hex ) {
                                fmt = "offset %08lx %s=%02x, offset %08lx %s=%02x\n";
                            } else if( say_it_in_oct ) {
                                fmt = "offset %08lx %s=%o, offset %08lx %s=%o\n";
                            } else {
                                fmt = "offset %lu %s=%u, offset %lu %s=%u\n";
                            }
                            printf( fmt, offs[0] + i, names[0], buffer[0][i],
                                         offs[1] + i, names[1], buffer[1][i] );
                        }
                    }
                    if( !keep_going ) {
                        return( 1 );
                    }
                    files_differ = 1;
                }
            }
        } else if( count_lines ) {
            for( i = 0; i < amt; ++i ) {
                if( buffer[0][i] == '\n' ) {
                    ++line_num;
                }
            }
        }
        offs[0] += bytes_read[0];
        offs[1] += bytes_read[1];
        if( bytes_read[0] != bytes_read[1] ) {
            if( !say_nothing ) {
                if( say_it_in_hex ) {
                    fmt = "cmp: EOF on %s at offset %08lx\n";
                } else {
                    fmt = "cmp: EOF on %s at offset %lu\n";
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
    return( files_differ );
}


int main( int argc, char **argv )
/*******************************/
{
    int         ch;
    long        offs[2];
    int         i;
    int         fh[2];

    while( (ch = getopt( argc, argv, ":lsx" )) != -1 ) {
        switch( ch ) {
        case 'l':
            keep_going = 1;
            say_it_in_oct = 1;
            break;
        case 's':
            say_nothing = 1;
            break;
        case 'x':
            keep_going = 1;
            say_it_in_hex = 1;
            break;
        case '?':
            util_quit( usage_text, NULL );
        }
    }
    if( !(say_it_in_hex || say_it_in_oct || say_nothing) ) {
        /* Only count lines if we have to */
        count_lines = 1;
    }

    /* Skip option arguments */
    argc = argc - optind + 1;
    argv += optind - 1;
    if( argc < 3 || argc > 5 ) {
        util_quit( usage_text, "cmp: invalid number of arguments\n" );
    }
    offs[0] = offs[1] = 0;
    if( argc > 3 ) {
        offs[0] = strtol( argv[3], NULL, 0 );
        if( argc > 4 ) {
            offs[1] = strtol( argv[4], NULL, 0 );
        }
    }
    ++argv;             /* Skip argv[0] */
    for( i = 0; i < 2; ++i ) {
        if( argv[i][0] == '-' && argv[i][1] == 0 ) {
            if( i == 1 && fh[0] == STDIN_FILENO ) {
                util_die( "cmp: only one argument can be stdin\n" );
            }
            fh[i] = STDIN_FILENO;
            argv[i] = "(stdin)";
        } else {
            fh[i] = open( argv[i], O_RDONLY | O_BINARY );
            if( fh[i] == -1 ) {
                util_die( "cmp: unable to open %s: %s\n", argv[i], strerror( errno ) );
            }
        }
        if( offs[i] ) {
            if( lseek( fh[i], offs[i], SEEK_SET ) == -1L ) {
                util_die( "cmp: error seeking to %s in %s: %s\n", argv[i+2], argv[i],
                           strerror( errno ) );
            }
        }
    }

    return( cmp( fh, argv, offs ) );
}
