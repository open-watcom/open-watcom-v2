/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2017 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Parse the .tix terminal description file.
*
****************************************************************************/


#ifndef BUILDDESC

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include "wterm.h"
#include "uidef.h"
#include "trie.h"
#include "uiintern.h"
#include "uiextrn.h"
#include "tixparse.h"
#include "doparse.h"

#include "clibext.h"
#endif

#define TC_ERROR    ((unsigned)-1)

#define CODE2BUFF( __b, __c )   __b[0] = __c & 0xff; __b[1] = (__c >> 8) & 0xff;
#define BUFF2CODE( __b )        (*(unsigned char *)__b + (*(unsigned char *)(__b + 1) << 8 ))

typedef enum {
    TT_CODE,
    TT_STRING,
    TT_EOF,
} tix_token;

FILE            *in_file = NULL;

#ifndef BUILDDESC

// macros for getting/setting bits in alt-char map
#define ti_alt_map_set( x ) ( _ti_alt_map[( x ) / 8] |= ( 1 << ( ( x ) % 8 ) ) )

unsigned char   _ti_alt_map[32];

#if defined( __QNX__ )
static const char acs_default[] = "q-x|l.m`k.j\'n+w-v-t|u|~*+>,<-^.vO#f`g?a#h#";
#else
static const char acs_default[] = "q-x|l+m+k+j+n+w+v+t+u+~o+>,<-^.v0#f\\g#a:h#";
#endif

static char find_acs_map( char c, const char *acs )
{
    char    ch;

    while( (ch = *acs++) != '\0' ) {
        if( ch == c )
            return( *acs );
        if( *acs++ == '\0' ) {
            break;
        }
    }
    return( '\0' );
}

char set_ti_alt_map( unsigned i, char c )
{
    char    cmap;

    cmap = find_acs_map( c, acs_chars );
    if( cmap != '\0' ) {
        ti_alt_map_set( i );
    } else {
        cmap = find_acs_map( c, acs_default );
        if( cmap == '\0' ) {
            cmap = c;
            ti_alt_map_set( i );
        }
    }
    return( cmap );
}
#endif

static void tix_error( const char *str )
{
#ifdef BUILDDESC
    fprintf( stderr, "\nError: %s\n", str );
#else
    uiwritec( "\nError in " );
    uiwrite( GetTermType() );
    uiwritec( ": " );
    uiwrite( str );
    uiwritec( "\n" );
#endif
}

static tix_token get_tix_token( char *buff )
{
    int         c;
    char        *p;
    char        *end;
    unsigned    num;
    int         endc;

    for( ;; ) {
        c = getc( in_file );
        if( c == EOF )
            return( TT_EOF );
        if( c == '#' ) {
            /* eat a comment */
            for( ;; ) {
                c = getc( in_file );
                if( c == EOF )
                    return( TT_EOF );
                if( c == '\n' ) {
                    break;
                }
            }
        }
        if( !isspace( c ) ) {
            break;
        }
    }
    p = buff;
    if( c == '\'' || c == '\"' ) {
        /* collect a string */
        endc = c;
        for( ;; ) {
            c = getc( in_file );
            if( c == EOF )
                break;
            if( c == '\r' )
                break;
            if( c == '\n' )
                break;
            if( c == endc )
                break;
            if( c == '\\' ) {
                c = getc( in_file );
                if( c == EOF )
                    break;
                switch( c ) {
                case 'a':
                    c = '\a';
                    break;
                case 'b':
                    c = '\b';
                    break;
                case 'e':
                    c = '\x1B';
                    break;
                case 'f':
                    c = '\f';
                    break;
                case 'n':
                    c = '\n';
                    break;
                case 'r':
                    c = '\r';
                    break;
                case 't':
                    c = '\t';
                    break;
                case 'v':
                    c = '\b';
                    break;
                case 'x':
                    num = 0;
                    for( ;; ) {
                        c = getc( in_file );
                        if( c == EOF )
                            break;
                        if( isdigit( c ) ) {
                            c = c - '0';
                        } else if( c >= 'A' && c <= 'F' ) {
                            c = c - 'A' + 10;
                        } else if( c >= 'a' && c <= 'f' ) {
                            c = c - 'a' + 10;
                        } else {
                            ungetc( c, in_file );
                        }
                        num = (num << 8) + c;
                    }
                    c = num;
                    break;
                }
            }
            *p++ = (char)c;
        }
        *p = '\0';
    } else {
        /* collect a string or number */
        for( ;; ) {
            *p++ = (char)c;
            c = getc( in_file );
            if( c == EOF )
                break;
            if( isspace( c ) )
                break;
            if( c == '#' ) {
                ungetc( c, in_file );
                break;
            }
        }
        *p = '\0';
        num = strtoul( buff, &end, 0 );
        if( end != p )
            return( TT_STRING );
        CODE2BUFF( buff, num );
    }
    return( TT_CODE );
}

static unsigned get_tix_code( char *buff )
{
    if( get_tix_token( buff ) != TT_CODE ) {
        tix_error( "expecting code" );
        return( TC_ERROR );
    }
    return( BUFF2CODE( buff ) );
}

bool do_parse( void )
{
    char        buff[80];
    char        input[80];
    tix_token   tok;
    unsigned    code;

    tok = get_tix_token( buff );
    while( tok == TT_STRING ) {
        if( stricmp( buff, "display" ) == 0 ) {
            code = get_tix_code( buff );
            if( code == TC_ERROR )
                return( false );
            tok = get_tix_token( buff );
            if( tok == TT_STRING ) {
                if( stricmp( buff, "alt" ) != 0 ) {
                    tix_error( "expecting alt" );
                    return( false );
                }
                tok = get_tix_token( buff );
                if( tok == TT_STRING ) {
                    tix_error( "expecting display code" );
                    return( false );
#ifdef BUILDDESC
                }
#else
                } else if( tok == TT_CODE ) {
                    ti_char_map[code][0] = set_ti_alt_map( code, buff[0] );
                }
            } else if( tok == TT_CODE ) {
                ti_char_map[code][0] = buff[0];
#endif
            }
            if( tok == TT_EOF )
                break;
            tok = get_tix_token( buff );
        } else if( stricmp( buff, "key" ) == 0 ) {
            code = get_tix_code( buff );
            if( code == TC_ERROR )
                return( false );
            input[0] = '\0';
            while( (tok = get_tix_token( buff )) == TT_CODE ) {
                strcat( input, buff );
            }
            TrieAdd( code, input );
        } else {
            tix_error( "unknown directive" );
            return( false );
        }
    }
    if( tok == TT_CODE ) {
        tix_error( "expecting directive" );
        return( false );
    }
    return( true );
}
