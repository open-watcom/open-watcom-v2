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
* Description:  POSIX egrep/fgrep utility
*               Performs grep function
*
****************************************************************************/


#include <io.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "regexp.h"
#include "getopt.h"
#include "argvrx.h"
#include "argvenv.h"
                                        // Search flags:
#define  M_SEARCH_INVERT        0x01    //      - invert sense of search
#define  M_SEARCH_EXACT         0x02    //      - pattern must match entire line
#define  M_SEARCH_IGNORE        0x04    //      - case insensitive match
#define  M_PREFIX_LINE          0x08    //      - precede output by line number
#define  M_SUPPRESS_ERROR       0x10    //      - suppress error messages

#define  IOBUF_MIN              2048    // size to hold a line

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

#ifdef FGREP
char *OptEnvVar = "fgrep";
#else
char *OptEnvVar = "egrep";
#endif

static const char *usageMsg[] = {
#ifdef FGREP
    "Usage: fgrep [-?Xcilnsvx] [-e pattern] [-f patfile] [@env] [pattern] [files...]",
#else
    "Usage: egrep [-?Xcilnsvx] [-e pattern] [-f patfile] [@env] [pattern] [files...]",
#endif
    "\tenv                 : environment variable to expand",
    "\tfiles               : files to search",
    "\tpattern             : pattern to search for",
    "\tOptions: -?         : display this message",
    "\t\t -c         : only display count of matched lines",
    "\t\t -e pattern : search for pattern (multiple patterns)",
    "\t\t -f patfile : search for patterns in pattern file",
    "\t\t -i         : perform case insensitive search",
    "\t\t -l         : only display names of files containing match",
    "\t\t -n         : precede each matched line with line number",
    "\t\t -s         : suppress error messages",
    "\t\t -v         : only display lines not matching a pattern",
    "\t\t -X         : match files by regular expressions",
    "\t\t -x         : require patterns to match entire line",
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

static  unsigned char    CharTrans[ ALPHA_SIZE ] = {
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

static  regexp     *ePatterns[ MAX_SEARCH_STR ];
static  char       *fPatterns[ MAX_SEARCH_STR ];
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
    unsigned    ui;

    for( ui = 0; ui < PatCount; ui++ ) {
        free( ePatterns[ ui ] );
    }
}

static void errorExit( const char *msg, ... )
{
    va_list arglist;

    if( (Flags & M_SUPPRESS_ERROR) == 0 ) {
        printf( "%s: ", OptEnvVar );
        va_start( arglist, msg );
        vprintf( msg, arglist );
        va_end( arglist );
        printf( "%s", "\n" );
    }
    free( IObuffer );
    freePatterns( );
    exit( EXIT_FAILURE );
}

static int searchBuffer( char *buf )
{
    unsigned         ui;

#ifdef FGREP
    if( Flags & M_SEARCH_EXACT ) {
        for( ui = 0; ui < PatCount; ui++ )
            if( strcmp( fPatterns[ ui ], buf ) == 0 )
                return( 1 );
    } else {
        for( ui = 0; ui < PatCount; ui++ ) {
            char       *s = buf;
            char const *p = fPatterns[ ui ];
            char const  first = *p++;
            
            for( ;; ) {
                if( CharTrans[ *(unsigned char *)s ] == first ) {
                    char       * const next = ++s;
                    
                    for( ; ; s++, p++ ) {
                        if( *p == '\0' )
                            return( 1 );
                        if( CharTrans[ *(unsigned char *)s ] != *p ) {
                            if( *s == '\0' ) {
                                goto outer;
                            } else if( CharExist[ CharTrans[ *(unsigned char *)s ] ] == 0 ) {
                                s++;
                            } else {
                                s = next;
                            }
                            break;
                        }
                    }
                    p = fPatterns[ ui ] + 1;
                } else if( *s == '\0' ) {
                    break;
                } else {
                    s++;
                }
            }
            outer: ;
        }
    }
#else
    for( ui = 0; ui < PatCount; ui++ )
        if( RegExec( ePatterns[ ui ], buf, 1 ) )
            return( 1 );
#endif
    return( 0 );
}

static char *getNextLine( int in, int newfile, const char *filename, unsigned lineno )
{
    static int          rd;
    static char *       offset;
    static char *       start;
    static char *       endbuf;
    static int          finalread;
    static int          done = 0;

    if( done ) {
        done = 0;
        return( NULL );
    }
    if( newfile ) {
        if( -1 == ( rd = read( in, IObuffer, IObsize ) ) ) errorExit( "I/O error" );
        start = offset = IObuffer;
        endbuf = IObuffer + rd;
        finalread = ( (size_t) rd < IObsize );
    }
    for( ; ; ) {
        size_t const    len = (unsigned int) ( ( endbuf - start ) - ( offset - start ) );
        char * const    lf = memchr( offset, '\n', len );
        char * const    line = start;

        if( lf != NULL ) {
            if( lf > IObuffer && lf[-1] == '\r' ) lf[-1] = '\0';
            *lf = '\0';
            offset = start = lf + 1;
            return( line );
        }
        if( finalread ) {
            endbuf[1] = '\0';
            done = 1;
            return( start );
        }
        if( len >= IObsize ) {
            free( IObuffer );
            if( ( IObuffer = malloc( IObsize += IObsize >> 1 ) ) == 0
            ||  -1 == lseek( in, -rd, SEEK_CUR ) )
                errorExit( "line too long: len (%lu) >= IObsize (%lu) at \"%s\":%u",
                    (unsigned long) len, (unsigned long) IObsize, filename, lineno );
            return getNextLine( in, newfile, filename, lineno );
        }
        memmove( IObuffer, offset, len );
        start = IObuffer;
        offset = IObuffer + len;
        if( -1 == ( rd = read( in, offset, IObsize - len ) ) ) errorExit( "I/O error" );
        finalread = ( (size_t) rd < IObsize - len );
        endbuf = offset + rd;
    }
}

static unsigned searchFile( const char *filename, int in, int numfile )
{
    char        *line;
    char         match;

    char const   invert  = ((Flags & M_SEARCH_INVERT) != 0);
    unsigned     lineno  = 1;
    unsigned     matches = 0;
    int          new = 1;

    while( ( line = getNextLine( in, new, filename, lineno ) ) != NULL ) {      // returns offset into IObuffer
        new  = 0;
        match = (char) searchBuffer( line );
        if( match ^ invert ) {
            matches++;
            if( Omode == OUT_LINES ) {
                if( numfile > 1 ) {
                    printf( "%s: ", filename );
                }
                if( Flags & M_PREFIX_LINE ) {
                    printf( "%u: ", lineno );
                }
                printf( "%s\n", line );
            } else if( Omode == OUT_FILES ) {
                printf( "%s\n", filename );
                break;
            }
        }
        lineno++;
    }
    return( matches );
}

static void parsePatterns( void )
{
    char * const        *pat;
    char                *p;

    for( pat = fPatterns; *pat != NULL; pat++ ) {
        for( p = *pat; *p; p++ ) {
            *p = CharTrans[ *(unsigned char *)p ];
            CharExist[ *(unsigned char *)p ] = 1;
        }
    }
}

static void insertPattern( const char *pat )
{
    if( PatCount >= MAX_SEARCH_STR ) {
        errorExit( "too many search patterns" );
    }
#ifdef FGREP
    fPatterns[ PatCount ] = strdup( pat );
#else
    ePatterns[ PatCount ] = RegComp( (char *) pat );
    if( ePatterns[ PatCount ] == NULL ) {
        errorExit( "error forming regular expression" );
    }
#endif
    PatCount++;
}

static void readPatternFile( const char *filename )
{
    FILE               *fp = fopen( filename, "r" );
    char               *cr;
    unsigned const      patstart = PatCount;

    if( fp == NULL )
        errorExit( "cannot open pattern file \"%s\"", filename );
    while( fgets( IObuffer, IObsize, fp ) ) {
        if( ( cr = strrchr( IObuffer, '\n' ) ) == NULL )
            errorExit( "invalid pattern \"%s\" in \"%s\"", IObuffer, filename );
        *cr = '\0';
        insertPattern( IObuffer );
    }
    (void) fclose( fp );
    if( patstart == PatCount )
        errorExit( "No pattern in \"%s\"", filename );
}

static void changeTransTable( void )
{
    int         ch;

    for( ch = 'A'; ch <= 'Z'; ch++ ) {      // Change uppers to lowers in
        CharTrans[ ch ] |= 0x20;            // translation table
    }
}

static void handle_options( int *pargc, char **argv, int *prematch )
{
    int         ch;             // switch chars

    while( -1 != ( ch = GetOpt( pargc, argv, (char *) "ce:f:ilnsvXx", usageMsg ) ) ) {
        switch( ch ) {
            case 'c':
                Omode = OUT_COUNT;
                break;
            case 'e':
                insertPattern( OptArg );
                break;
            case 'f':
                readPatternFile( OptArg );
                break;
            case 'i':
                Flags |= M_SEARCH_IGNORE;
                CaseIgnore = TRUE;
                changeTransTable( );
                break;
            case 'l':
                Omode = OUT_FILES;
                break;
            case 'n':
                Flags |= M_PREFIX_LINE;
                break;
            case 's':
                Flags |= M_SUPPRESS_ERROR;
                break;
            case 'v':
                Flags |= M_SEARCH_INVERT;
                break;
            case 'X':
                *prematch = 1;
                break;
            case 'x':
                Flags |= M_SEARCH_EXACT;
                break;
            default: /* Do nothing */
                break;
        }
    }
}

int main( int argc, char **argv )
{
    int         rematch = 0;    // regexp file matching is OFF
    unsigned    matches = 0;    // number of matches

    if( ( IObuffer = malloc( IObsize = IOBUF_MIN ) ) == NULL )
        errorExit( "insufficient memory for file buffer" );
    CaseIgnore  = FALSE;        // case sensitive match by default
    argv = ExpandEnv( &argc, argv );
    handle_options( &argc, argv, &rematch );

    if( PatCount == 0 ) {
        if( argc <= 1 )
            errorExit( "%s", usageMsg[0] );
        argc--, argv++;
        insertPattern( *argv );
    }

    parsePatterns( );
    argv = ExpandArgv( &argc, argv, rematch );

    if( *++argv == NULL ) {
        (void) setmode( STDIN_FILENO, O_BINARY );
        matches = searchFile( "stdin", STDIN_FILENO, 1 );
    } else {
        while( *argv != NULL ) {
            int const   in = open( *argv, O_BINARY | O_RDONLY );        // input file handle

            if( in == -1 ) {
                if( !(Flags & M_SUPPRESS_ERROR) )
                    fprintf( stderr, "%s: cannot open input file \"%s\"\n",
                        OptEnvVar, *argv );
            } else {
                matches += searchFile( *argv, in, argc - 1 );
                close( in );
            }
            argv++;
        }
    }
    if( Omode == OUT_COUNT )
        printf( "%u\n", matches );
    free( IObuffer );
    freePatterns( );

    return( matches == 0 );
}
