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
#include "regexp.h"
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

#define  MAX_SEARCH_STR         256     // Maximum number of search strings.

typedef enum outmode {                  // Output modes:
    OUT_FILES,                          //      - output file names
    OUT_COUNT,                          //      - output count of matching lines
    OUT_LINES                           //      - output matching lines
} outmode;

/**********************************************************
 * Global Data.
 **********************************************************/

char *OptEnvVar = "egrep";

static char *usageMsg[] = {
    "Usage: egrep [-?Xcilnsvx] [-e pattern] [-f patfile] [@env] [pattern]"
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

static  regexp     *Patterns[ MAX_SEARCH_STR ];
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

static int searchBuffer( char *buf )
{
    int         i;

    for( i = 0; i < PatCount; i++ ) {
        if( RegExec( Patterns[ i ], buf, 1 ) ) {
            return( 1 );
        }
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
                errorExit( "fgrep: line too long\n" );
            }
            memmove( IObuffer, offset, len );
            start = IObuffer;
            offset = IObuffer + len;
            rd = read( in, offset, IObsize - len );
            if( rd == -1 ) {
                errorExit( "fgrep: I/O error\n" );
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
    unsigned     lineno  = 1;
    unsigned     matches = 0;
    int          new = 1;

    while( 1 ) {
        line = getNextLine( in, new );      // returns offset into IObuffer
        new  = 0;

        if( line == NULL ) {
            break;
        } else {
            match = searchBuffer( line );
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

static void insertPattern( char *pat )
{
    if( PatCount >= MAX_SEARCH_STR ) {
        errorExit( "egrep: too many search patterns\n" );
    } else {
        Patterns[ PatCount ] = RegComp( pat );
        if( Patterns[ PatCount ] == NULL ) {
            errorExit( "egrep: error forming regular expression\n" );
        }
        PatCount++;
    }
}

static void readPatternFile( char *filename ) {

    FILE       *fp;
    char       *cr;
    char       *res;
    char        buff[ 512 ];

    fp = fopen( filename, "r" );
    if( fp == NULL ) {
        Die( "egrep: cannot open pattern file ""%s""\n", filename );
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
            Die( "egrep: invalid pattern ""%s""\n", buff );
        }
        if( res == NULL ) {
            break;
        }
    }
}

int main( int argc, char **argv )
{
    int         in;             // input file handle
    int         ch;             // switch chars

    int         rematch = 0;    // regexp file matching is OFF
    unsigned    matches = 0;    // number of matches

    CaseIgnore  = FALSE;        // case sensitive match by default

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
                CaseIgnore = TRUE;
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
        errorExit( "egrep: insufficient memory for file buffer\n" );
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

    argv = ExpandArgv( &argc, argv, rematch );
    argv++;

    if( *argv == NULL ) {
        setmode( STDIN_FILENO, O_BINARY );
        matches = searchFile( "stdin", STDIN_FILENO, 1 );
    } else {
        while( *argv != NULL ) {
            in = open( *argv, O_BINARY | O_RDONLY );
            if( in == -1 ) {
                if( (Flags & M_SUPPRESS_ERROR) == 0 ) {
                    fprintf( stderr, "egrep: cannot open input file \"%s\"\n",
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
