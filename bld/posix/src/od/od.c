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
#include <fcntl.h>
#include <io.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include "misc.h"
#include "getopt.h"
#include "argvrx.h"
#include "argvenv.h"
#include "clibext.h"

#define  M_BYTES        0x01            // Binary output modes.
#define  M_WORDS        0x02
#define  M_DWORDS       0x04

#define  OCT_OFFSET     0
#define  DEC_OFFSET     1

#define  MAX_BYTES      16              // Number of bytes to read in

char *OptEnvVar="od";

static const char *usageMsg[] = {
    "Usage: od [-?bcdDhoOsSxX] [@env] [file] [[+]offset[.][b]]",
    "\tenv                : environment variable to expand",
    "\tfile               : name of file to dump",
    "\toffset             : starting octal offset for file dump",
    "\t                       + indicates offset when no file given",
    "\t                       . indicates offset is decimal",
    "\t                       b indicates offset in blocks",
    "\tOptions: -?        : display this message",
    "\t\t -h        : hexadecimal bytes",
    "\t\t -b        : octal bytes",
    "\t\t -c        : ASCII bytes",
    "\t\t -d        : unsigned decimal words",
    "\t\t -D        : unsigned decimal longs (double words)",
    "\t\t -o        : unsigned octal words",
    "\t\t -O        : unsigned octal longs (double words)",
    "\t\t -s        : signed decimal words",
    "\t\t -S        : signed decimal longs (double words)",
    "\t\t -x        : unsigned hexadecimal words",
    "\t\t -X        : unsigned hexadecimal longs (double words)",
    NULL
};

typedef struct format {
    long int    offset;             // initial file offset
    unsigned    osfmt : 1;          // offset format ( dec. = ON, oct. = OFF )

    unsigned    b_asc : 1;          // b = bytes, w = words, dw = double words
    unsigned    b_hex : 1;
    unsigned    b_oct : 1;          // asc = ascii
    unsigned    w_dec : 1;          // dec = unsigned decimal
    unsigned    w_hex : 1;          // hex = hexadecimal
    unsigned    w_oct : 1;          // oct = octal
    unsigned    w_sgn : 1;          // sgn = signed decimal
    unsigned    dw_dec : 1;
    unsigned    dw_oct : 1;
    unsigned    dw_sgn : 1;
    unsigned    dw_hex : 1;
} format;

/*
 * Local functions.
 */

static void printBinary( char *ptr, unsigned size, int mode, char *fmtstr,
                            char *print_os )
{
    long        val;                        // temp to hold dump values
    int         i;

    if( *print_os == 0 ) {
        fprintf( stdout, "%10s", "" );      // pad start of line with blanks
    } else {
        *print_os = 0;
    }

    if( mode == M_BYTES ) {
        for( i = 0; i < size; i++, ptr++ ) {
            fprintf( stdout, fmtstr, *ptr );
        }
    } else if( mode == M_WORDS ) {
        for( i = 0; i < size; i += 2, ptr += 2 ) {
            val  = (long) *(ptr + 1) << 8L;
            val += (long) *ptr;
            fprintf( stdout, fmtstr, val );
        }
    } else if( mode == M_DWORDS ) {
        for( i = 0; i < size; i += 4, ptr += 4 ) {
            val  = (long) *(ptr + 3) << 24L;
            val += (long) *(ptr + 2) << 16L;
            val += (long) *(ptr + 1) << 8L;
            val += (long) *ptr;
            fprintf( stdout, fmtstr, val );
        }
    }
    fprintf( stdout, "\n" );                    // go to new line.
}

static void printText( char *data, unsigned size, char *print_os )
{
    int     i;

    if( *print_os == 0 ) {
        fprintf( stdout, "%10s", "" );      // pad start of line with blanks
    } else {
        *print_os = 0;
    }

    for( i = 0; i < MAX_BYTES  &&  i < size; i++ ) {
        switch( data[i] ) {
            case '\0':
                fprintf( stdout, "  \\0" );
                break;
            case '\f':
                fprintf( stdout, "  \\f" );
                break;
            case '\n':
                fprintf( stdout, "  \\n" );
                break;
            case '\r':
                fprintf( stdout, "  \\r" );
                break;
            case '\t':
                fprintf( stdout, "  \\t" );
                break;
            case '\b':
                fprintf( stdout, "  \\b" );
                break;
            default:
                if( isascii( data[i] )  &&  isprint( data[i] ) ) {
                    fprintf( stdout, " %3c", data[i] );
                } else {
                    fprintf( stdout, " %03o", (unsigned) data[i] );
                }
        }
    }
    fprintf( stdout, "\n" );
}

static void flushLine( char *data, unsigned size, format *fmt )
{
    char         print_os;

    if( size == 0 ) {
        return;
    }

    if( fmt->osfmt == DEC_OFFSET ) {
        fprintf( stdout, "%010ld", fmt->offset );
    } else {
        fprintf( stdout, "%010lo", fmt->offset );
    }

    print_os = 1;                       // the offset has been printed

    if( fmt->w_oct ) {
        printBinary( data, size, M_WORDS, " %07o", &print_os );
    }
    if( fmt->dw_oct ) {
        printBinary( data, size, M_DWORDS, "     %011lo", &print_os );
    }
    if( fmt->w_dec ) {
        printBinary( data, size, M_WORDS, "   %5u", &print_os );
    }
    if( fmt->dw_dec ) {
        printBinary( data, size, M_DWORDS, " %15lu", &print_os );
    }
    if( fmt->w_sgn ) {
        printBinary( data, size, M_WORDS, "   %5d", &print_os );
    }
    if( fmt->dw_sgn ) {
        printBinary( data, size, M_DWORDS, " %15ld", &print_os );
    }
    if( fmt->w_hex ) {
        printBinary( data, size, M_WORDS, "    %04x", &print_os );
    }
    if( fmt->dw_hex ) {
        printBinary( data, size, M_DWORDS, "        %08lx", &print_os );
    }
    if( fmt->b_hex ) {
        printBinary( data, size, M_BYTES, "  %02x", &print_os );
    }
    if( fmt->b_asc ) {
        printText( data, size, &print_os );
    }
    if( fmt->b_oct ) {
        printBinary( data, size, M_BYTES, " %03o", &print_os );
    }
}

static void dumpFile( FILE *fp, format *fmt )
{
    int         ch;
    char       *buf;
    char       *prv;

    int         repeat = 0;
    short int   sz     = 0;

    buf = (char *) malloc( MAX_BYTES );
    prv = (char *) malloc( MAX_BYTES );

    fseek( fp, fmt->offset, SEEK_SET );

    while( 1 ) {
        ch  = fgetc( fp );

        if( ch != EOF ) {
            buf[ sz ] = (char) ch;
        } else {
            flushLine( buf, sz, fmt );
            break;
        }

        sz++;

        if( isatty( fileno( fp ) )  &&  ch == '\n' ) {
            flushLine( buf, sz, fmt );
            repeat = 0;
            fmt->offset += (long int) sz;
            sz = 0;
            memset( buf, 0, MAX_BYTES );
        } else if( sz >= MAX_BYTES ) {
            if( repeat != 0  &&  memcmp( buf, prv, MAX_BYTES ) == 0 ) {
                if( repeat != 2 ) {
                    fprintf( stdout, "*\n" );
                    repeat = 2;
                }
            } else {
                flushLine( buf, MAX_BYTES, fmt );
                memcpy( prv, buf, MAX_BYTES );
                repeat = 1;
            }
            fmt->offset += (long int) MAX_BYTES;
            sz = 0;
            memset( buf, 0, MAX_BYTES );
        }
    }

    fmt->offset += (long int) sz;

    if( fmt->osfmt == DEC_OFFSET ) {
        fprintf( stdout, "%010ld\n", fmt->offset );
    } else {
        fprintf( stdout, "%010lo\n", fmt->offset );
    }
}

static void parseOffset( char *str, format *fmt )
{
    long int    mult = 1L;
    char       *p, *fs = "%lo";

    if( *str == '+' ) {                         // ignore the '+'
        str++;
    }

    for( p = str; isdigit( *p ); p++ );         // skip by number.

    if( *p == '.' ) {                           // decimal option specified
        fs = "%ld";
        fmt->osfmt = DEC_OFFSET;
        *p = '\0';
        p++;
    }
    if( *p == 'b' ) {
        mult = 512L;                            // block option specified
        *p = '\0';
        p++;
    }

    if( *p != '\0' ) {
        fmt->offset = -1L;                      // error occurred
    } else {
        sscanf( str, fs, &fmt->offset );        // scan and multiply by given
        fmt->offset *= mult;                    // block size (default 1)
    }
}

void main( int argc, char **argv )
{
    FILE       *fp;
    int         ch;

    format      fmt     = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    char        fmtset  = 0;

    argv = ExpandEnv( &argc, argv );

    while( 1 ) {
        ch = GetOpt( &argc, argv, "bcdDhoOsSxX", usageMsg );
        if( ch == -1 ) {
            break;
        }
        if( strchr( "bcdDhoOsSxX", ch ) != NULL ) {
            fmtset = 1;
            switch( ch ) {                      // switch to set format type
                case 'h':
                    fmt.b_hex = 1;
                    break;
                case 'b':
                    fmt.b_oct = 1;
                    break;
                case 'c':
                    fmt.b_asc = 1;
                    break;
                case 'd':
                    fmt.w_dec = 1;
                    break;
                case 'D':
                    fmt.dw_dec = 1;
                    break;
                case 'o':
                    fmt.w_oct = 1;
                    break;
                case 'O':
                    fmt.dw_oct = 1;
                    break;
                case 's':
                    fmt.w_sgn = 1;
                    break;
                case 'S':
                    fmt.dw_sgn = 1;
                    break;
                case 'x':
                    fmt.w_hex = 1;
                    break;
                case 'X':
                    fmt.dw_hex = 1;
                    break;
            }
        }
    }
    if( !fmtset ) {
        fmt.w_oct = 1;                          // set default (octal words)
    }

    argv++;
    if( *argv == NULL || **argv == '+' ) {
        if( *argv != NULL ) {
            parseOffset( *argv, &fmt );         // get specified offset
            if( fmt.offset < 0 ) {
                Die( "od: invalid offset\n" );  // error
            }
        }
        setmode( STDIN_FILENO, O_BINARY );      // switch stdin to binary mode
        dumpFile( stdin, &fmt );
    } else {
        if( argc == 3 ) {
            parseOffset( *(argv + 1), &fmt );   // get specified offset
            if( fmt.offset < 0 ) {
                Die( "od: invalid offset\n" );  // error
            }
        }
        if( (fp = fopen( *argv, "rb" )) == NULL ) {
            Die( "od: cannot open input file \"%s\"\n", *argv );
        }
        dumpFile( fp, &fmt );
        fclose( fp );
    }
}
