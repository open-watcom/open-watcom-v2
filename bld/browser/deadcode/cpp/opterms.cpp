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
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <watcom.h>
#include <wstring.hpp>
#include "util.h"
#include "opterms.h"
#include "opgram.h"

// typedefs

typedef struct {
    char *          name;
    int             unique;
    int             token;
} tokenInfo;

typedef struct {
    tokenInfo * table;
    char *      name;
    LookForType type;
    int         num_elem;
} tokenTable;

#define DEFTOKEN( name, tag ) { name, 0, tag },


// static functions

static int  compNodes( const void * left, const void * right );
static int  samelen( char * a, char * b );
static void setupTokenTable( tokenTable * table );
static void showUnique( char * name, int unique );

static inline int maxi( int a, int b ) { return a > b ? a : b; }
static inline int mini( int a, int b ) { return a < b ? a : b; }

// static global tables

static tokenInfo Directives[] =
{
    #include "opdirect.h"
    { NULL, 0, 0 }
};
const numDirectives (sizeof(Directives) / sizeof(tokenInfo) - 1);

static tokenInfo Options[] =
{
    #include "opopts.h"
    { NULL, 0, 0 }
};
const numOptions (sizeof(Options) / sizeof(tokenInfo) - 1);

static tokenTable TokenTables[] = {
    { Directives, "directive", LF_Directive, numDirectives },
    { Options,    "option",    LF_Option,    numOptions },
    { NULL,       0 }
};

/*
 * define some special, one character non-terminals.  These cause a flush.
 * value returned is the character constant.
 */
char * Special = "=,@!~\"{}";

char * StartComments = "#";
char * EndComments = "\n";

// externally available functions

extern void terminalSetup( void )
/*******************************/
{
    for( int i = 0; TokenTables[ i ].table != NULL; i += 1 ) {
        setupTokenTable( &TokenTables[ i ] );
    }
}

extern int tryToken( LookForType lf, char * buf )
/***********************************************/
{
    int         i;
    tokenInfo * fnd = NULL;
    int         count = strlen( buf );

    for( int cnt = 0; TokenTables[ cnt ].table != NULL; cnt += 1 ) {
        tokenTable * table = &TokenTables[ cnt ];

        if( table->type & lf ) {
            for( i = 0; i < table->num_elem; i += 1 ) {
                if( !strnicmp( table->table[ i ].name, buf, count ) &&
                    count >= table->table[ i ].unique ) {
                        fnd = &table->table[ i ];
                        break;
                }
            }
        }
    }

    if( fnd != NULL ) {
        return fnd->token;
    } else {
        return -1;
    }
}

extern char * tryAmbig( LookForType lf, char * buf )
/**************************************************/
{
    int          i;
    int          count = strlen( buf );
    tokenTable * table;

    for( int cnt = 0; TokenTables[ cnt ].table != NULL; cnt += 1 ) {
         table = &TokenTables[ cnt ];

        if( table->type & lf ) {
            for( i = 0; i < table->num_elem; i += 1 ) {
                if( !strnicmp( table->table[ i ].name, buf, count ) ) break;
            }

            if( i < table->num_elem ) {
                WString ambigs;

                ambigs.concat( table->table[ i ].name );
                i += 1;
                while( i < table->num_elem
                         && !strnicmp( table->table[ i ].name, buf, count ) ) {
                    ambigs.concat( " or " );
                    ambigs.concat( table->table[ i ].name );
                    i += 1;
                }

                return WBRStrDup( ambigs );
            }
        }
    }

    return NULL;
}

extern char * findTokenString( int token )
/****************************************/
{
    int i;

    for( i = 0; Options[ i ].name != NULL; i += 1 ) {
        if( Options[ i ].token == token ) {
            return( Options[ i ].name );
        }
    }

    for( i = 0; Directives[ i ].name != NULL; i += 1 ) {
        if( Directives[ i ].token == token ) {
            return( Directives[ i ].name );
        }
    }

    return( NULL );
}

// static functions

#pragma warning 14 9    // Warning! W014: no reference to symbol 'showUnique'
/*
 * change a string to mixed case, capitals showing characters needed to
 * make it unique.
 */
static void showUnique( char * name, int unique )
/***********************************************/
{
    strlwr( name );
    while( unique > 0 ) {
        unique -= 1;
        name[ unique ] = (char) toupper( name[ unique ] );
    }
}

/*
 * find the number of characters the same between two strings,
 * or the strlen of the shorter
 */

static int samelen( char * a, char * b )
/**************************************/
{
    int i = 0;

    while( *a != '\0' && *b != '\0' && toupper( *a++ ) == toupper( *b++ ) ) {
        i += 1;
    }

    return i;
}

/*
 * compare two tokenInfo nodes
 */
static int compNodes( const void * left, const void * right )
/***********************************************************/
{
    return strcmp( ((tokenInfo *)left)->name, ((tokenInfo *)right)->name );
}

/*
 * setup one of the token tables
 */
static void setupTokenTable( tokenTable * table )
/***********************************************/
{
    int i;
    int unique;
    int prev;
    int next;

    qsort( table->table, table->num_elem, sizeof(tokenInfo), compNodes );

    for( i = 0; i < table->num_elem; i += 1 ) {
        unique = 0;

        if( i > 0 ) {
            prev = samelen( table->table[i - 1].name, table->table[i].name );
        } else {
            prev = 0;
        }

        unique = maxi( unique, prev );

        if( i < table->num_elem - 1 ) {
            next = samelen( table->table[i + 1].name, table->table[i].name );
        } else {
            next = 0;
        }

        unique = maxi( unique, next );

        table->table[ i ].unique = mini( unique + 1,
                                        strlen( table->table[ i ].name ));

        #if 0   // this modifies a static string, and doesn't work with /zc
        showUnique( table->table[ i ].name, table->table[ i ].unique );
        #endif
    }
}
