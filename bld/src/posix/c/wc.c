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
* Description:  Counts lines, words, characters in input.
*
****************************************************************************/


#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "util.h"


static int              char_flag;
static int              line_flag;
static int              mult_flag;
static int              word_flag;
static unsigned long    total_chars;
static unsigned long    total_lines;
static unsigned long    total_words;

static const char *usage_text[] = {
    "Usage: wc [-?lw] [-c|-m] [file...]",
    "\tfile        : file to count words/lines/characters in",
    "\tOptions: -? : display this message",
    "\t\t -c : count bytes",
    "\t\t -m : count characters",
    "\t\t -l : count lines",
    "\t\t -w : count words",
    NULL
};


static void print_line( unsigned long lines, unsigned long words,
                        unsigned long chars, const char *name )
/***************************************************************/
{
    int     need_space = 0;

    if( line_flag ) {
        printf( "%lu", lines );
        need_space = 1;
    }
    if( word_flag ) {
        if( need_space ) {
            putchar( ' ' );
            need_space = 0;
        }
        printf( "%lu", words );
        need_space = 1;
    }
    if( char_flag ) {
        if( need_space ) {
            putchar( ' ' );
            need_space = 0;
        }
        printf( "%lu", chars );
    }
    if( name != NULL ) {
        printf( " %s\n", name );
    } else {
        putchar( '\n' );
    }
}


static void do_wc( FILE *fh, const char *name )
/*********************************************/
{
    int                 ch;
    int                 in_word = 0;
    unsigned long       lines = 0;
    unsigned long       words = 0;
    unsigned long       chars = 0;

    for( ;; ) {
        ch = fgetc( fh );
        if( ch == EOF ) break;
        if( mult_flag ) {
            /* Currently not implemented. Needs to take multi-byte
             * characters into account.
             */
            ++chars;
        } else {
            ++chars;
        }
        if( isspace( ch ) ) {
            if( in_word ) {
                in_word = 0;
                ++words;
            }
        } else {
            in_word = 1;
        }
        if( ch == '\n' ) {
            ++lines;
        }
    }
    if( in_word ) {
        ++words;
    }
    total_lines += lines;
    total_words += words;
    total_chars += chars;
    print_line( lines, words, chars, name );
}


int main( int argc, char **argv )
/*******************************/
{
    int         i, ch;
    FILE        *fh;
    int         more_than_one;

    while( (ch = getopt( argc, argv, ":clmw" )) != -1 ) {
        switch( ch ) {
        case 'c':
            char_flag = 1;
            break;
        case 'l':
            line_flag = 1;
            break;
        case 'm':
            char_flag = 1;
            mult_flag = 1;
            break;
        case 'w':
            word_flag = 1;
            break;
        case '?':
            util_quit( usage_text, NULL );
        }
    }
    /* Skip option arguments and argv[0] */
    argc = argc - optind;
    argv += optind;

    if( !word_flag && !line_flag && !char_flag ) {
        word_flag = line_flag = char_flag = 1;
    }
    if( argc == 0 ) {
        do_wc( stdin, NULL );
    } else {
        i = 0;
        more_than_one = 0;
        for( ;; ) {
            fh = fopen( argv[i], "r" );
            if( fh == NULL ) {
                fprintf( stderr, "wc: error opening '%s' for read: %s\n",
                    argv[i], strerror( errno ) );
                return( EXIT_FAILURE );
            }
            do_wc( fh, argv[i] );
            fclose( fh );
            ++i;
            if( i == argc ) break;
            more_than_one = 1;
        }
        if( more_than_one ) {
            print_line( total_lines, total_words, total_chars, "total" );
        }
    }
    return( EXIT_SUCCESS );
}
