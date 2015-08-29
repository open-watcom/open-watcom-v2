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


#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "bool.h"

#define _IN_ICMASK      // for ic.h to include everything

#include "wcpp.h"
#include "icodes.h"

static const char *ic_names[] = {
    #define IC( code, type, mask ) __STR( code )
    #include "ic.h"
    #undef IC
};

static const char *ic_masks[] = {
    #define IC( code, type, mask ) mask
    #include "ic.h"
    #undef IC
};

static const char *usageMsg[] = {
    "icmask <ic-h> <use-ic-source> <use-ic-source> ...",
    "where:",
    "    <ic-h> is ic.h",
    "    <use-ic-source> is source code that selectively reads IC codes",
    NULL
};

#define BUFF_SIZE 1024

#define NO_IDX  ((unsigned)-1)

static char *file;
static unsigned line;
static FILE *src_c;
static char buff[BUFF_SIZE];

typedef struct name NAME;
struct name {
    NAME        *next;
    char        name[1];
};
static NAME *icMaskTable[IC_END + 1];
static NAME *icPreProcTable[IC_END + 1];

static void addName( NAME **t, unsigned ic, char *name )
{
    NAME *m;
    size_t len;

    for( m = t[ic]; m != NULL; m = m->next ) {
        if( strcmp( m->name, name ) == 0 ) {
            return;
        }
    }
    len = strlen( name );
    m = malloc( sizeof( *m ) + len );
    m->next = t[ic];
    t[ic] = m;
    strcpy( m->name, name );
}

static void fail( char *msg, ... )
{
    va_list args;

    if( line ) {
        fprintf( stderr, "error on line %u of %s\n", line, file );
    }
    va_start( args, msg );
    vfprintf( stderr, msg, args );
    va_end( args );
    exit( EXIT_FAILURE );
}

static void dumpUsage( void )
{
    const char **p;

    for( p = usageMsg; *p != '\0'; ++p ) {
        fprintf( stderr, "%s\n", *p );
    }
}

static char *skipNonSpace( char *p )
{
    while( *p && !isspace( *p ) ) {
        ++p;
    }
    return( p );
}

static char *skipMacroName( char *p )
{
    if( *p && ( isalpha( *p ) || *p == '_' )) {
        ++p;
    }
    while( *p && ( isalnum( *p ) || *p == '_') ) {
        ++p;
    }
    return( p );
}

static char *skipSpace( char *p )
{
    while( *p && isspace( *p ) ) {
        ++p;
    }
    return( p );
}

static unsigned whatICAmI( char *p )
{
    char        *s;
    char        c;
    unsigned    i;

    s = p;
    while( *p && ( isalnum( *p ) || *p == '_' ) ) {
        ++p;
    }
    c = *p;
    *p = '\0';
    for( i = 0; i <= IC_END; ++i ) {
        if( strcmp( ic_names[i], s ) == 0 ) {
            *p = c;
            return( i );
        }
    }
    return( NO_IDX );
}

static void processIC_H( char *fname )
{
    char *p;
    char *ok;
    char *ic_start;
    unsigned ic_idx;
    FILE *ic_h;

    file = fname;
    ic_h = fopen( fname, "r" );
    if( ic_h == NULL )
        fail( "cannot open '%s' for input\n", fname );
    line = 0;
    ic_idx = NO_IDX;
    for( ;; ) {
        ok = fgets( buff, sizeof( buff ), ic_h );
        if( ok == NULL )
            break;
        ++line;
        p = skipSpace( buff );
        if( memcmp( p, "//", 2 ) == 0 ) {
            continue;
        }
        ic_start = strstr( p, "IC(" );
        if( ic_start != NULL ) {
            p = skipNonSpace( ic_start );
            p = skipSpace( p );
            ic_idx = whatICAmI( p );
            if( ic_idx == NO_IDX )
                fail( "cannot find IC '%s'\n", p );
            continue;
        }
        if( ic_idx != NO_IDX ) {
            if( buff[0] == '#' ) {
                addName( icPreProcTable, ic_idx, buff );
            }
        }
    }
    fclose( ic_h );
}

static void outputIMASK_H( int argc, char **argv )
{
    NAME *m;
    NAME *n;
    int i;
    FILE *out_h;

    // output name cannot match icmask.* because makefile execs del icmask.*
    out_h = fopen( "ic_mask.gh", "w" );
    if( out_h == NULL )
        fail( "cannot open 'icmask.gh' for output\n" );
    fprintf( out_h, "/* generated by ICMASK reading" );
    while( argc-- > 0 ) {
        fprintf( out_h, " %s", *argv );
        ++argv;
    }
    fprintf( out_h, " */\n" );
    for( i = 0; i <= IC_END; ++i ) {
        m = icMaskTable[i];
        if( m == NULL ) {
            if( ic_masks[i] == NULL ) {
                fprintf( out_h, "0, /* %s */\n", ic_names[i] );
            } else {
                fprintf( out_h, "0|%s, /* %s */\n", ic_masks[i], ic_names[i] );
            }
        } else {
            for( ; m != NULL; m = n ) {
                n = m->next;
                if( n == NULL ) {
                    break;
                }
                fprintf( out_h, "ICOPM_%s|", m->name );
            }
            if( ic_masks[i] == NULL ) {
                fprintf( out_h, "ICOPM_%s, /* %s */\n", m->name, ic_names[i] );
            } else {
                fprintf( out_h, "ICOPM_%s|%s, /* %s */\n", m->name, ic_masks[i], ic_names[i] );
            }
        }
        for( m = icPreProcTable[i]; m != NULL; m = m->next ) {
            fprintf( out_h, "%s", m->name );
        }
    }
    fclose( out_h );
}

static void scanSource( char *fname )
{
    char *mask_id;
    char *ok;
    char *t;
    char *p;
    char *s;
    char c;
    int begin_found;
    unsigned ic_idx;

    file = fname;
    line = 0;
    begin_found = 0;
    mask_id = NULL;
    for(;;) {
        ok = fgets( buff, sizeof(buff), src_c );
        if( ok == NULL ) break;
        ++line;
        if( begin_found ) {
            t = strstr( buff, "// ICMASK END" );
            if( t != NULL ) {
                begin_found = 0;
                continue;
            }
            p = skipSpace( buff );
            if( memcmp( p, "case", 4 ) == 0 ) {
                p = skipNonSpace( p );
                p = skipSpace( p );
                if( memcmp( p, "IC_", 3 ) == 0 ) {
                    ic_idx = whatICAmI( p );
                    if( ic_idx == NO_IDX )
                        fail( "cannot find IC '%s'\n", p );
                    addName( icMaskTable, ic_idx, mask_id );
                }
            }
        } else {
            t = strstr( buff, "// ICMASK BEGIN" );
            if( t != NULL ) {
                p = skipNonSpace( t );
                p = skipSpace( p );
                p = skipNonSpace( p );
                p = skipSpace( p );
                p = skipNonSpace( p );
                p = skipSpace( p );
                s = p;
                p = skipMacroName( p );
                c = *p;
                *p = '\0';
                mask_id = strdup( s );
                *p = c;
                begin_found = 1;
            }
        }
    }
    if( begin_found ) {
        fail( "// ICMASK BEGIN found without matching // ICMASK END\n" );
    }
}

static int ParseVariable( const char *var, char **argv, char *buf )
/*****************************************************************/
{
    /*
     * Returns a count of the "command line" parameters in *var.
     * Unless argv is NULL, both argv and buf are completed.
     *
     * This function ought to be fairly similar to clib(initargv@_SplitParms).
     * Parameterisation does the same as _SplitParms with historical = 0.
     */

    const char  *start;
    int         argc;
    char        *bufend;
    char        *bufstart;
    bool        got_quote;
    bool        output_data;

    output_data = ( buf != NULL ) && ( argv != NULL );
    bufend = buf;
    argc = 0;
    for( ;; ) {
        got_quote = false;
        while( isspace( *var ) && *var != '\0' )
            var++;
        start = var;
        if( output_data ) {
            bufstart = bufend;
        }
        if( *var == '-' ) {
            if( output_data ) {
                *bufend++ = *var;
            }
            var++;
        }
        while( ( got_quote || !isspace( *var ) ) && *var != '\0' ) {
            if( *var == '\"' ) {
                got_quote = !got_quote;
            }
            if( output_data ) {
                *bufend++ = *var;
            }
            var++;
        }
        if( start != var ) {
            if( output_data ) {
                *bufend++ = '\0';
                argv[argc] = bufstart;
            }
            argc++;
        }
        if( *var == '\0' ) {
            break;
        }
    }
    return( argc );
}

int indirEnvOrFile( char *name, char ***args )
{
    /*
     * Pass nofilenames and analysis of getenv(name) into argc and argv
     * to doScanParams. Return view on usability of data. (true is usable.)
     */
    int                 argc;
    size_t              argvsize;
    size_t              argbufsize;
    const char          *optstring;
    size_t              varlen;         // size to hold name copy.
    char                fbuf[512];

    optstring = getenv( name );
    if( optstring == NULL ) {
        FILE *fh;

        fh = fopen( name, "rt" );
        if( fh == NULL ) {
            *args = NULL;
            return( 0 );
        }
        fgets( fbuf, sizeof( fbuf ), fh );
        fclose( fh );
        optstring = fbuf;
    }
    argc = ParseVariable( optstring, NULL, NULL );  // count parameters.
    argbufsize = strlen( optstring ) + 1 + argc;    // inter-parameter spaces map to 0
    argvsize = argc * sizeof( char * );             // sizeof argv[argc+1]
    varlen = strlen( name ) + 1;                    // Copy taken to detect recursion.
    *args = (char **)malloc( argbufsize + argvsize + varlen );
    ParseVariable( optstring, *args, (char *)( *args + argc ) );
    return( argc );
}

int main( int argc, char **argv )
{
    char **args;
    int  i;

    if( argc == 1 || *argv[1] != '@' && argc == 2 ) {
        dumpUsage();
        fail( "invalid arguments\n" );
    }
    args = argv + 1;
    argc = argc - 1;
    if( *args[0] == '@' ) {
        argc = indirEnvOrFile( args[0] + 1, &args );
    }
    processIC_H( args[0] );
    for( i = 1; i < argc; ++i ) {
        src_c = fopen( args[i], "r" );
        if( src_c == NULL )
            fail( "cannot open '%s' for input\n", args[i] );
        scanSource( args[i] );
        fclose( src_c );
    }
    outputIMASK_H( argc, args );
    return( EXIT_SUCCESS );
}
