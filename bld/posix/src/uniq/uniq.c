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
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "misc.h"
#include "getopt.h"
#include "argvrx.h"
#include "argvenv.h"

#define  MIN_LINE_LEN   80

#define  MODE_UNIQUE    0x01
#define  MODE_REPEAT    0x02
#define  MODE_COUNT     0x04

char *OptEnvVar="uniq";

static char *usageMsg[] = {
    "Usage: uniq [-?ud] [-c] [-number] [+number] [@env] [input [output]]",
    "\tenv                : environment variable to expand",
    "\tinput              : name of input file",
    "\toutput             : name of output file",
    "\tOptions: -?        : display this message",
    "\t\t -u        : display lines which are not repeated",
    "\t\t -d        : display only one copy of lines which are repeated",
    "\t\t -c        : display all lines, without repetition, along with",
    "\t\t           : the number of times each line occurred",
    "\t\t -number   : skip number fields before comparison",
    "\t\t +number   : skip number characters in field before comparison",
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

    while( 1 ) {
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

static int  compareLines( line *ln1, line *ln2, int fld, int chr )
{
    char        *p1;
    char        *p2;
    int          fcnt;
    int          cnt;

    for( fcnt = 0, p1 = ln1->buff; *p1 != '\0'  &&  fcnt < fld; p1++ ) {
        if( *p1 == ' '  ||  *p1 == '\t' ) {
            fcnt++;
            for( ; *p1 == ' '  ||  *p1 == '\t' ; p1++ );
            p1--;
        }
    }
    for( fcnt = 0, p2 = ln2->buff; *p2 != '\0'  &&  fcnt < fld; p2++ ) {
        if( *p2 == ' '  ||  *p2 == '\t' ) {
            fcnt++;
            for( ; *p2 == ' '  ||  *p2 == '\t' ; p2++ );
            p2--;
        }
    }

    for( cnt = 0; *p1 != '\0'  &&  cnt < chr; p1++, cnt++ );
    for( cnt = 0; *p2 != '\0'  &&  cnt < chr; p2++, cnt++ );

    if( *p1 == '\0'  &&  *p2 == '\0' ) {
        return( 0 );
    } else if( *p1 == '\0'  ||  *p2 == '\0' ) {
        return( 1 );
    } else {
        return( strcmp( p1, p2 ) );
    }
}

static void copyLines( line *dest, line *src )
{
    if( src->size > dest->size ) {
        dest->size = src->size;
        dest->buff = (char *) realloc( dest->buff, dest->size * sizeof(char) );
    }
    strcpy( dest->buff, src->buff );
}

static void displayUniq( FILE *in, FILE *out, int mode, int fldos, int chros )
{
    line        ln1 = { NULL, 0 }, ln2 = { NULL, 0 };
    int         count  = 0;
    int         done   = 0;
    int         cmp    = 0;

    done = getNextLine( in, &ln1 );

    while( !done ) {
        done = getNextLine( in, &ln2 );

        if( done ) {
            cmp = 1;
        } else {
            cmp = compareLines( &ln1, &ln2, fldos, chros );
        }

        if( !cmp ) {
            count++;
        } else {
            if( mode & MODE_UNIQUE  &&  count == 0 ) {
                fprintf( out, "%s\n", ln1.buff );
            }
            if( mode & MODE_REPEAT  &&  count != 0 ) {
                fprintf( out, "%s\n", ln1.buff );
            }
            if( mode & MODE_COUNT ) {
                fprintf( out, "    %d %s\n", count + 1, ln1.buff );
            }
            count = 0;
            copyLines( &ln1, &ln2 );
        }
    }

    free( ln1.buff );
    free( ln2.buff );
}

static int isNumber( char *s )
{
    if( *s == '\0' ) {
        return( 0 );
    }

    for( ; *s != '\0'; s++ ) {
        if( !isdigit( *s ) ) {
            return( 0 );
        }
    }
    return( 1 );
}

void main( int argc, char **argv )
{
    FILE       *in, *out;
    int         ch;

    int         mode   = 0;
    int         fld_os = 0;
    int         chr_os = 0;

    argv = ExpandEnv( &argc, argv );

    while( 1 ) {
        ch = GetOpt( &argc, argv, "#udc", usageMsg );
        if( ch == -1 ) {
            break;
        }
        switch( ch ) {
            case 'u':
                mode &= ~MODE_COUNT;            // turn off counting mode
                mode |= MODE_UNIQUE;
                break;
            case 'd':
                mode &= ~MODE_COUNT;            // turn off counting mode
                mode |= MODE_REPEAT;
                break;
            case 'c':
                mode  = MODE_COUNT;
                break;
            case '#':
                if( !isNumber( OptArg ) ) {
                    Die( "uniq: invalid field offset\n" );
                }
                fld_os = atoi( OptArg );
                break;
        }
    }
    if( mode == 0 ) {
        mode = MODE_UNIQUE | MODE_REPEAT;
    }

    argv++;
    argc--;
    if( *argv != NULL  &&  **argv == '+' ) {
        if( !isNumber( *argv + 1 ) ) {
            Die( "uniq: invalid character offset\n" );
        }
        chr_os = atoi( *argv + 1 );
        argv++;
        argc--;
    }
    if( *argv == NULL ) {
        displayUniq( stdin, stdout, mode, fld_os, chr_os );
    } else {
        if( (in = fopen( *argv, "r" )) == NULL ) {
            Die( "uniq: cannot open input file \"%s\"\n", *argv );
        }
        argv++;
        if( *argv != NULL ) {
            if( (out = fopen( *argv, "w" )) == NULL ) {
                Die( "uniq: cannot open output file \"%s\"\n", *argv );
            }
            displayUniq( in, out, mode, fld_os, chr_os );
            fclose( out );
        } else {
            displayUniq( in, stdout, mode, fld_os, chr_os );
        }
        fclose( in );
    }
}
