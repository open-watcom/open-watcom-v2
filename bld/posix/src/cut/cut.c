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
* Description:  POSIX cut utility
*               Cuts files
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

char *OptEnvVar="cut";

static const char *usageMsg[] = {
    "Usage: cut -f|c list [-d char] [-?Xs] [@env] [files...]",
    "\tenv                : environment variable to expand",
    "\tfiles              : files to cut",
    "\tlist               : comma separated numeric list",
    "\tOptions: -?        : display this message",
    "\t\t -f list   : retain listed fields",
    "\t\t -d char   : field delimiter character [default is the tab]",
    "\t\t -s        : suppress lines containing no field separator",
    "\t\t -c list   : retain listed columns",
    "\t\t -X        : match files by regular expressions",
    NULL
};

typedef enum mode {                         // Character selection mode.
    UNDEF,
    FIELD,
    CHAR
} mode;

typedef struct node {                       // Node type of list tree.
    struct node    *left;
    struct node    *right;
    int             v;
} node;

typedef struct line {                       // Line of a file.
    char           *buff;
    unsigned        size;
} line;

/*
 * Local functions.
 */

static void  treeFree( node *n )
{
    if( n->right != NULL ) {
        treeFree( n->left );
        treeFree( n->right );
    } else {
        free( n );
    }
}

static node *treeNode( int val, node *l, node *r )
{
    node        *n;

    n = (node *) malloc( sizeof( node ) );

    n->left  = l;
    n->right = r;
    n->v     = val;

    return( n );
}

static void treePlace( node *n, int val )
{
    node        temp;

    while( 1 ) {
        if( n->left == NULL  ||  n->left->v > val ) {
            n->v = val;
            break;
        } else {
            temp = *n;
            *n = *n->right;
            free( temp.right );
            free( temp.left );

            if( val <= n->v ) {
                break;
            }
        }
    }
}

static void treeInsert( node *n, int low, int high )
{
    if( n->right == NULL ) {

        n->left  = treeNode( low, NULL, NULL );
        n->right = treeNode( high, NULL, NULL );

    } else if( low > n->right->v + 1 ) {

        treeInsert( n->right, low, high );

    } else if( low >= n->left->v ) {

        if( high > n->right->v ) {
            treePlace( n->right, high );
        }

    } else if( high < n->left->v - 1 ) {

        n->right = treeNode( high, n->left, n->right );
        n->left  = treeNode( low, NULL, NULL );

    } else {
        n->left->v = low;
        if( high > n->right->v ) {
            treePlace( n->right, high );
        }
    }
}

static int parseList( char *list, node *head )
{
    char        *p, *op, *arg;
    int          low, high;

    op = list;

    while( p != NULL ) {
        p = strchr( op, ',' );
        if( p != NULL ) {
            *p = '\0';
        }

        arg = strchr( op, '-' );
        if( arg != NULL ) {
            *arg = '\0';
            arg++;
        } else {
            arg = op;
        }

        low = atoi( op );
        high = atoi( arg );

        if( high < low  ||  low == 0 ) {        // There was a range error.
            return( 1 );
        }
        treeInsert( head, low, high );          // Stuff data into tree.
        op = p + 1;
    }
    return( 0 );
}

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

static void getFields( line *ln, node *list, char delim, char supp )
{
    char        *p, *op;
    int          f;
    int          low, high, fld;

    p  = strchr( ln->buff, delim );

    if( p == NULL  &&  !supp ) {
        fprintf( stdout, "%s\n", ln->buff );
    } else if( p != NULL ) {
        f = 0;
        op = ln->buff;
        fld = 0;
        while( list->right != NULL  &&  p != NULL ) {
            low = list->left->v - 1;
            high = list->right->v - 1;

            while( fld <= high ) {
                if( fld >= low ) {
                    if( f ) {
                        fprintf( stdout, "%c", delim );
                    }
                    f = 1;
                    if( p == NULL ) {
                        fprintf( stdout, "%s", op );
                        break;
                    } else {
                        *p = '\0';
                        fprintf( stdout, "%s", op );
                    }
                } else if( p == NULL ) {
                    break;
                }
                op = p + 1;
                p = strchr( op, delim );
                fld++;
            }
            list = list->right;
        }
        if( f ) {
            fprintf( stdout, "\n" );
        }
    }
}

static void getChars( line *ln, node *list )
{
    unsigned    low = 0, high, len;

    len = strlen( ln->buff );

    for( ; list->right != NULL  &&  low < len; list = list->right ) {
        low  = list->left->v - 1;
        high = list->right->v - 1;

        for( ; low <= high  &&  low < len; low++ ) {
            fputc( *(ln->buff + low), stdout );
        }
    }
    fputc( '\n', stdout );
}

static void cutFile( FILE *fp, node *list, mode m, char delim, char supp )
{
    line        ln;

    ln.size = 0;
    ln.buff = NULL;

    while( !getNextLine( fp, &ln ) ) {
        if( m == CHAR ) {
            getChars( &ln, list );
        } else if( m == FIELD ) {
            getFields( &ln, list, delim, supp );
        }
    }

    free( ln.buff );
}

void main( int argc, char **argv )
{
    FILE       *fp;
    int         ch;
    node       *head;                   // Head pointer for list tree.
    char       *list = NULL;            // List of fields to be retained.
    mode        m = UNDEF;
    char        delim = '\t';
    char        suppress = 0;
    char        regexp = 0;             // Don't to reg.exp. file matching.

    argv = ExpandEnv( &argc, argv );

    while( 1 ) {
        ch = GetOpt( &argc, argv, "f:c:d:sX", usageMsg );
        if( ch == -1 ) {
            break;
        }
        switch( ch ) {
            case 'f':
                m = FIELD;
                list = (char *) realloc( list, strlen(OptArg)*sizeof(char) + 1);
                strcpy( list, OptArg );
                break;
            case 'c':
                m = CHAR;
                list = (char *) realloc( list, strlen(OptArg)*sizeof(char) + 1);
                strcpy( list, OptArg );
                break;
            case 'd':
                delim = *OptArg;
                break;
            case 's':
                suppress = 1;
                break;
            case 'X':
                regexp = 1;
                break;
        }
    }

    if( m == UNDEF ) {
        Die( "%s\n", usageMsg[0] );
    }

    argv = ExpandArgv( &argc, argv, regexp );
    head = treeNode( 0, NULL, NULL );

    if( parseList( list, head ) ) {
        treeFree( head );
        free( list );
        Die( "cut: invalid list or range\n" );
    }

    argv++;
    if( argv[0] == NULL ) {
        cutFile( stdin, head, m, delim, suppress );
    } else {
        while( *argv != NULL ) {
            fp = fopen( *argv, "r" );
            if( fp == NULL ) {
                fprintf( stderr,"cut: cannot open input file \"%s\"\n", *argv );
            } else {
                if( argc > 2 ) {
                    fprintf( stdout, "%s:\n", *argv );
                }
                cutFile( fp, head, m, delim, suppress );
                fclose( fp );
            }
            argv++;
        }
    }
    treeFree( head );
    free( list );
}
