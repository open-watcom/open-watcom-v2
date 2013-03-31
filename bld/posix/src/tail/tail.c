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
* Description:  POSIX tail utility
*               Displays tail lines of a file
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include "misc.h"
#include "getopt.h"
#include "argvrx.h"


char *OptEnvVar = "tail";

static const char *usageMsg[] = {
    "Usage: tail [-?X][-<number>] [files]",
    "\tfiles              : files to display tail lines of",
    "\tOptions: -?        : print this list",
    "\t\t -<number> : number of lines to show",
    "\t\t -X        : match files by regular expressions",
    NULL
};

static int tailCount;

#define STRING_INC      32

typedef struct {
    int         size;
    int         off;
    char        str[1];
} string;

static void addCharToString( string **p, char ch )
{
    string *s = *p;
    if( s->off == s->size ) {
        s->size += STRING_INC;
        s = MemRealloc( s, sizeof( string ) + s->size );
        *p = s;
    }
    s->str[ s->off ] = ch;
    s->off++;

}

string *newString( void )
{
    string      *tmp;

    tmp = MemAlloc( sizeof( string ) + STRING_INC );
    tmp->size = STRING_INC;
    tmp->off = 0;
    return( tmp );

}

/*
 * tailStdin - display the tail of stdin
 */
static void tailStdin( void )
{
    int         ch;
    string      **tail_list;
    int         cline;
    int         i;

    tail_list = MemAlloc( tailCount * sizeof( string * ) );

    cline = 0;
    for(;;) {
        ch = fgetchar();
        if( ch == EOF ) {
            for( i=0;i<tailCount;i++ ) {
                if( tail_list[cline] != NULL ) {
                    printf( "%s", tail_list[cline]->str );
                }
                cline = (cline+1) % tailCount;
            }
            return;
        }
        if( tail_list[ cline ] == NULL ) {
            tail_list[ cline ] = newString();
        }
        addCharToString( &tail_list[cline], ch );
        if( ch == '\n' ) {
            addCharToString( &tail_list[cline], 0 );
            cline = (cline+1) % tailCount;
            if( tail_list[cline] != NULL ) {
                tail_list[cline]->off = 0;
            }
        }

    }

}


/*
 * tailFile - display the tail of a file
 */
void tailFile( char *fname )
{
    long line;
    char ch;
    int h;
    int amt_read;
    long pos;

    h = open( fname, O_BINARY | O_RDONLY );
    if( h == -1 ) {
        Die( "Could not open file \"%s\"\n", fname );
    }
    line = 0;
    pos = lseek( h, -1L, SEEK_END );
    for(;;) {
        if( pos == -1 ) {
            pos = 0;
        } else {
            amt_read = read( h, &ch, 1 );
            if( amt_read != 1 ) {
                pos = 0;
            }
        }
        if( pos == 0 || ch == '\n' ) {
            line++;
            if( pos == 0 ) {
                pos = lseek( h, 0, SEEK_SET );
            }
            if( line == tailCount+1 || pos == 0 ) {
                for(;;) {
                    if( read( h, &ch, 1 ) != 1 ) {
                        close( h );
                        return;
                    }
                    putchar( ch );
                }
            }
        }
        pos = lseek( h, -2L, SEEK_CUR );
    }

}


void main( int argc, char *argv[] )
{
    int         ch;
    int         i;
    int         rxflag;

    tailCount = 10;
    rxflag = 0;
    for(;;) {
        ch = GetOpt( &argc, argv, "#X", usageMsg );
        if( ch == -1 ) {
            break;
        }
        switch( ch ) {
        case '#':
            tailCount = atoi( OptArg );
            break;
        case 'X':
            rxflag = 1;
            break;
        }
    }

    argv = ExpandArgv( &argc, argv, rxflag );

    if( argc == 1 ) {
        tailStdin();
    } else {
        for( i=1;i<argc;i++ ) {
            if( argc > 2 ) {
                printf( "\n%s:\n", strupr( argv[i] ) );
            }
            tailFile( argv[i] );
        }
    }
    exit( 0 );
}
