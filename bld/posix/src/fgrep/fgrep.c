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
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <io.h>
#include <fcntl.h>
#include <limits.h>
#include "misc.h"
#include "getopt.h"
#include "argvrx.h"
#include "argvenv.h"
                                        // Search flags:
#define  M_SEARCH_INVERT        0x01    //      - invert sense of search
#define  M_SEARCH_EXACT         0x02    //      - pattern must match entire line
#define  M_SEARCH_IGNORE        0x04    //      - case insensitive match
#define  M_PREFIX_LINE          0x08    //      - precede output by line number
#define  M_SUPPRESS_ERROR       0x10    //      - suppress error messages

#define  IOBUF_MIN              1024
#if defined( __386__ )
    #define IOBUF_SIZE          65536
#else
    #define IOBUF_SIZE          32768
#endif

#define  ALPHA_SIZE             256     // Size of alphabet
#define  MAX_SEARCH_STR         256     // Maximum number of search strings.

typedef enum outmode {                  // Output modes:
    OUT_FILES,                          //      - output file names
    OUT_COUNT,                          //      - output count of matching lines
    OUT_LINES                           //      - output matching lines
} outmode;

/**********************************************************
 * Global Data.
 **********************************************************/

char *OptEnvVar = "fgrep";

static char *usageMsg[] = {
    "Usage: fgrep [-?Xcilnsvx] [-e pattern] [-f patfile] [@env] [pattern]"
    " [files...]",
    "\tenv                 : environment variable to expand",
    "\tfiles               : files to search",
    "\tpattern             : pattern to search for",
    "\tOptions: -?         : display this message",
    "\t\t -e pattern : search for pattern (multiple patterns)",
    "\t\t -f patfile : search for patterns in pattern file",
    "\t\t -v         : only display lines not matching a pattern",
    "\t\t -x         : require patterns to match entire line",
    "\t\t -i         : perform case insensitive search",
    "\t\t -c         : only display count of matched lines",
    "\t\t -l         : only display names of files containing match",
    "\t\t -n         : precede each matched line with line number",
    "\t\t -s         : suppress error messages",
    "\t\t -X         : match files by regular expressions",
    NULL
};

static  char    CharExist[ ALPHA_SIZE ] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static  char    CharTrans[ ALPHA_SIZE ] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
    0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
    0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
    0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
    0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
    0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
    0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
    0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
    0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
    0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
    0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
    0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
    0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
    0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
    0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
    0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

static  char       *Patterns[ MAX_SEARCH_STR ];
static  unsigned    PatCount = 0;

static  size_t      IObsize  = 0;
static  char       *IObuffer = NULL;

static  int         Flags = 0;              // search flags
static  outmode     Omode = OUT_LINES;      // output mode

/**********************************************************
 * Local Functions.
 **********************************************************/

static void freePatterns( void )
{
    int         i;

    for( i = 0; i < PatCount; i++ ) {
        free( Patterns[ i ] );
    }
}

static void errorExit( char *msg )
{
    if( (Flags & M_SUPPRESS_ERROR) == 0 ) {
        printf( "%s\n", msg );
    }
    free( IObuffer );
    freePatterns( );
    exit( EXIT_FAILURE );
}


static int exactSearchBuffer( char *srch )
{
    int         i;

    for( i = 0; i < PatCount; i++ ) {
        if( strcmp( Patterns[ i ], srch ) == 0 ) {
            return( 1 );
        }
    }
    return( 0 );
}

static int searchBuffer( char *srch )
{
    int         i;
    char       *s, *p, *next;
    char        first;

    for( i = 0; i < PatCount; i++ ) {
        s = srch;
        p = Patterns[ i ];
        first = *p;
        p++;
        for( ; ; ) {
            if( CharTrans[ *s ] == first ) {
                s++;
                next = s;
                for( ; ; s++, p++ ) {
                    if( *p == '\0' ) {
                        return( 1 );
                    } else if( CharTrans[ *s ] != *p ) {
                        if( *s == '\0' ) {
                            goto outer;
                        } else if( CharExist[ CharTrans[ *s ] ] == 0 ) {
                            s++;
                            break;
                        } else {
                            s = next;
                            break;
                        }
                    }
                }
                p = Patterns[ i ] + 1;
            } else if( *s == '\0' ) {
                break;
            } else {
                s++;
            }
        }
        outer: ;
    }
    return( 0 );
}

static char *getNextLine( int in, int newfile )
{
    int                         rd;
    unsigned int                len;
    char *                      lf;
    char *                      line;

    static char *               offset;
    static char *               start;
    static char *               endbuf;
    static int                  finalread;
    static int                  done = 0;

    if( done ) {
        done = 0;
        return( NULL );
    } else if( newfile ) {
        rd = read( in, IObuffer, IObsize );
        if( rd == -1 ) {
            errorExit( "fgrep: I/O error\n" );
        }
        start  = IObuffer;
        offset = start;
        endbuf = IObuffer + rd;
        if( rd < IObsize ) {
            finalread = 1;
        } else {
            finalread = 0;
        }
    }

    for( ; ; ) {
        len = (unsigned int)( endbuf - start );
        lf = memchr( offset, '\n', len );
        if( lf != NULL ) {
            if( lf > IObuffer  &&  lf[-1] == '\r' )  lf[-1] = '\0';
            *lf = '\0';
            line = start;
            start = lf + 1;
            offset = start;
            return( line );
        } else if( finalread ) {
            endbuf[1] = '\0';
            done = 1;
            return( start );
        } else {
            if( len >= IObsize ) {
                errorExit( "fgrep: line too long" );
            }
            memmove( IObuffer, offset, len );
            start = IObuffer;
            offset = IObuffer + len;
            rd = read( in, offset, IObsize - len );
            if( rd == -1 ) {
                errorExit( "fgrep: I/O error" );
            } else if( rd < IObsize - len ) {
                finalread = 1;
            } else {
                finalread = 0;
            }
            endbuf = offset + rd;
        }
    }
}

static unsigned searchFile( char *filename, int in, int numfile )
{
    char        *line;
    char         match;

    char         invert  = ((Flags & M_SEARCH_INVERT) != 0);
    char         exact   = ((Flags & M_SEARCH_EXACT) != 0);
    unsigned     lineno  = 1;
    unsigned     matches = 0;
    int          new = 1;

    while( 1 ) {
        line = getNextLine( in, new );      // returns offset into IObuffer
        new  = 0;

        if( line == NULL ) {
            break;
        } else {
            if( exact ) {
                match = exactSearchBuffer( line );
            } else {
                match = searchBuffer( line );
            }
            if( match ^ invert ) {
                if( Omode == OUT_LINES ) {
                    if( numfile > 1 ) {
                        printf( "%s: ", filename );
                    }
                    if( Flags & M_PREFIX_LINE ) {
                        printf( "%u: ", lineno );
                    }
                    printf( "%s\n", line );
                } else if( Omode == OUT_COUNT ) {
                    matches++;
                } else if( Omode == OUT_FILES ) {
                    printf( "%s\n", filename );
                    break;
                }
            }
            lineno++;
        }
    }
    return( matches );
}

static void parsePatterns( void )
{
    char       **pat;
    char        *p;

    pat = Patterns;

    while( *pat != NULL ) {
        p = *pat;
        while( *p ) {
            *p = CharTrans[ *p ];
            CharExist[ *p ] = 1;
            p++;
        }
        pat++;
    }
}

static void insertPattern( char *pat )
{
    if( PatCount >= MAX_SEARCH_STR ) {
        errorExit( "fgrep: too many search patterns" );
    } else {
        Patterns[ PatCount ] = strdup( pat );
        PatCount++;
    }
}

static void readPatternFile( char *filename ) {

    FILE       *fp;
    char       *cr;
    char       *res;
    char        buff[ 256 ];

    fp = fopen( filename, "r" );
    if( fp == NULL ) {
        Die( "fgrep: cannot open pattern file ""%s""\n", filename );
    }
    while( 1 ) {
        res = fgets( buff, 256, fp );
        cr = strrchr( buff, '\n' );
        if( cr  ||  res == NULL ) {
            if( cr ) {
                *cr = '\0';
            }
            if( strlen( buff ) > 0 ) {
                insertPattern( buff );
                PatCount++;
            }
        } else {
            Die( "fgrep: invalid pattern ""%s""\n", buff );
        }
        if( res == NULL ) {
            break;
        }
    }
}

static void changeTransTable( void )
{
    char        ch;

    for( ch = 'A'; ch <= 'Z'; ch++ ) {      // Change uppers to lowers in
        CharTrans[ ch ] |= 0x20;            // translation table
    }
}

int main( int argc, char **argv )
{
    int         in;             // input file handle
    int         ch;             // switch chars

    int         rematch = 0;    // regexp file matching is OFF
    unsigned    matches = 0;    // number of matches

    argv = ExpandEnv( &argc, argv );

    while( 1 ) {
        ch = GetOpt( &argc, argv, "Xcilnsvxe:f:", usageMsg );
        if( ch == -1 ) {
            break;
        }
        switch( ch ) {
            case 'v':
                Flags |= M_SEARCH_INVERT;
                break;
            case 'x':
                Flags |= M_SEARCH_EXACT;
                break;
            case 'i':
                Flags |= M_SEARCH_IGNORE;
                changeTransTable( );
                break;
            case 'n':
                Flags |= M_PREFIX_LINE;
                break;
            case 's':
                Flags |= M_SUPPRESS_ERROR;
                break;
            case 'c':
                Omode = OUT_COUNT;
                break;
            case 'l':
                Omode = OUT_FILES;
                break;
            case 'e':
                insertPattern( OptArg );
                break;
            case 'f':
                readPatternFile( OptArg );
                break;
            case 'X':
                rematch = 1;
                break;
        }
    }

    for( IObsize = IOBUF_SIZE; IObsize >= IOBUF_MIN; IObsize /= 2 ) {
        if( IObuffer = (char *) malloc( IObsize ) ) break;
    }
    if( IObuffer == NULL ) {
        errorExit( "fgrep: insufficient memory for file buffer\n" );
    }

    if( PatCount == 0 ) {
        if( argc <= 1 ) {
            Die( "%s\n", usageMsg[0] );
        } else {
            argv++;
            argc--;
            insertPattern( *argv );
        }
    }

    parsePatterns( );

    argv = ExpandArgv( &argc, argv, rematch );
    argv++;

    if( *argv == NULL ) {
        setmode( STDIN_FILENO, O_BINARY );
        matches = searchFile( "stdin", STDIN_FILENO, 1 );
    } else {
        while( *argv != NULL ) {
            in = open( *argv, O_BINARY | O_RDONLY );
            if( in == -1 ) {
                if( !(Flags & M_SUPPRESS_ERROR) ) {
                    fprintf( stderr, "fgrep: cannot open input file \"%s\"\n",
                                *argv );
                }
            } else {
                matches += searchFile( *argv, in, argc - 1 );
                close( in );
            }
            argv++;
        }
    }

    if( Omode == OUT_COUNT ) {
        printf( "%u\n", matches );
    }
    free( IObuffer );
    freePatterns( );

    if( matches ) {
        return( 0 );
    } else {
        return( 1 );
    }
}
