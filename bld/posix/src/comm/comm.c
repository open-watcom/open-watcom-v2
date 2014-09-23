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
* Description:  POSIX comm utility
*               Compares sorted files
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "misc.h"
#include "getopt.h"
#include "argvrx.h"
#include "argvenv.h"

#define  MIN_LINE_LEN   80

#define  S_FIRST        0x01
#define  S_SECOND       0x02
#define  S_BOTH         0x04

char *OptEnvVar="comm";

static const char *usageMsg[] = {
    "Usage: comm [-?123] [@env] file1 file2",
    "\tenv                : environment variable to expand",
    "\tfile1 file2        : similarly sorted files to be compared",
    "\tOptions: -?        : display this message",
    "\t\t -1 2 3    : suppress specified output columns",
    NULL
};

typedef struct line {                       // Line of a file.
    char           *buff;
    unsigned        size;
} line;

/*
 * Local functions.
 */

static int getNextLine( FILE *fp, line *l )
{
    int         ch = 0;
    unsigned    os = 0;

    if( l->size == 0 ) {
        l->size = MIN_LINE_LEN * sizeof( char );
        l->buff = (char *) malloc( l->size );
    }

    for( ;; ) {
        if( os >= l->size - 1 ) {                   // Buffer getting small.
            l->size += MIN_LINE_LEN * sizeof( char );
            l->buff  = (char *) realloc( l->buff, l->size );
        }
        ch = fgetc( fp );

        if( ch == EOF ) {
            break;
        } else if( (char) ch == '\n' ) {
            break;
        } else {
            *(l->buff + os) = (char) ch;
            os++;
        }
    }
    *(l->buff + os) = '\0';

    return( (ch == EOF)  &&  (os == 0) );
}

static void compareFiles( FILE *fp1, FILE *fp2, char mask )
{
    line        l1 = { NULL, 0 }, l2 = { NULL, 0 };
    int         cmp = 0, i1, i2;
    char        tab1 = '\t', tab2 = '\t';

    if( mask & S_FIRST ) {
        tab1 = 0;
    }
    if( mask & S_SECOND ) {
        tab2 = 0;
    }

    for( ;; ) {
        if( cmp <= 0 ) {
            i1 = getNextLine( fp1, &l1 );
        }
        if( cmp >= 0 ) {
            i2 = getNextLine( fp2, &l2 );
        }

        if( i1  &&  i2 ) {
            break;
        } else if( i1 ) {
            cmp = 1;
        } else if( i2 ) {
            cmp = -1;
        } else {
            cmp = strcmp( l1.buff, l2.buff );
        }

        if( cmp < 0  &&  (~mask & S_FIRST) ) {
            printf( "%s\n", l1.buff );
        } else if( cmp > 0  &&  (~mask & S_SECOND) ) {
            if( tab1 ) printf( "%c", tab1 );
            printf( "%s\n", l2.buff );
        } else if( cmp == 0  &&  (~mask & S_BOTH) ) {
            if( tab1 ) printf( "%c", tab1 );
            if( tab2 ) printf( "%c", tab2 );
            printf( "%s\n", l1.buff );
        }
    }

    free( l1.buff );
    free( l2.buff );
}

void main( int argc, char **argv )
{
    FILE       *fp1, *fp2;
    int         ch;
    char        mask = 0;

    argv = ExpandEnv( &argc, argv );

    for( ;; ) {
        ch = GetOpt( &argc, argv, "123", usageMsg );
        if( ch == -1 ) {
            break;
        }
        switch( ch ) {
            case '1':
                mask |= S_FIRST;
                break;
            case '2':
                mask |= S_SECOND;
                break;
            case '3':
                mask |= S_BOTH;
                break;
        }
    }

    if( argc != 3 ) {
        Die( "%s\n", usageMsg[0] );
    } else {
        if( (fp1 = fopen( argv[1], "r" )) == NULL ) {
            Die( "comm: cannot open input file \"%s\"\n", argv[1] );
        }
        if( (fp2 = fopen( argv[2], "r" )) == NULL ) {
            Die( "comm: cannot open input file \"%s\"\n", argv[2] );
        }
        compareFiles( fp1, fp2, mask );
        fclose( fp1 );
        fclose( fp2 );
    }
}
