/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  POSIX wc utility
*               Counts lines, words, characters on input
*
****************************************************************************/


#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bool.h"
#include "getopt.h"
#include "argvrx.h"

char *OptEnvVar="wc";

bool    line_flag = false;
bool    word_flag = false;
bool    char_flag = false;

unsigned long total_lines;
unsigned long total_words;
unsigned long total_chars;

static const char *usageMsg[] = {
    "Usage: wc [-?Xclw] [files]",
    "\tfiles       : files to count words/lines/characters in",
    "\tOptions: -? : display this message",
    "\t\t -c : count characters",
    "\t\t -w : count words",
    "\t\t -l : count lines",
    "\t\t -X : match files by regular expressions",
    NULL
};

static void PrintLine( unsigned long lines,
                unsigned long words,
                unsigned long chars,
                const char *name )
{
    if( line_flag ) {
        printf( "%8lu", lines );
    }
    if( word_flag ) {
        printf( "%8lu", words );
    }
    if( char_flag ) {
        printf( "%8lu", chars );
    }
    if( name != NULL ) {
        printf( "   %s\n", name );
    } else {
        putchar( '\n' );
    }
}


static void DoWC( FILE *fh, const char *name )
{
    int                 ch;
    bool                in_word;
    unsigned long       lines = 0;
    unsigned long       words = 0;
    unsigned long       chars = 0;

    in_word = false;
    for(;;) {
        ch = fgetc( fh );
        if( ch == EOF )
            break;
        ++chars;
        if( isspace( ch ) ) {
            if( in_word ) {
                in_word = false;
                ++words;
            }
        } else {
            in_word = true;
        }
        if( ch == '\n' ) {
            ++lines;
        }
    }
    if( in_word )
        ++words;
    total_lines += lines;
    total_words += words;
    total_chars += chars;
    PrintLine( lines, words, chars, name );
}


int main( int argc, char **argv )
{
    int         i,ch;
    FILE        *fh;
    bool        more_than_one;
    bool        rxflag;

    rxflag = false;
    while( (ch = GetOpt( &argc, argv, "Xwlc", usageMsg )) != -1 ) {
        switch( ch ) {
        case 'w':
            word_flag = true;
            break;
        case 'l':
            line_flag = true;
            break;
        case 'c':
            char_flag = true;
            break;
        case 'x':
            rxflag = true;
            break;
        }
    }

    argv = ExpandArgv( &argc, argv, rxflag );

    if( !word_flag && !line_flag && !char_flag ) {
        word_flag = line_flag = char_flag = true;
    }
    if( argc == 1 ) {
        DoWC( stdin, NULL );
    } else {
        i = 1;
        more_than_one = false;
        for(;;) {
            fh = fopen( argv[ i ], "r" );
            if( fh == NULL ) {
                fprintf( stderr, "error opening %s for read: %s\n",
                    argv[i], strerror( errno ) );
                exit( 1 );
            }
            DoWC( fh, argv[ i ] );
            fclose( fh );
            ++i;
            if( i == argc )
                break;
            more_than_one = true;
        }
        if( more_than_one ) {
            PrintLine( total_lines, total_words, total_chars, "--Total" );
        }
    }
    return( 0 );
}
