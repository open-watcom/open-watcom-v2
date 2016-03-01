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
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "tixparse.h"

typedef enum {
    TT_CODE,
    TT_STRING,
    TT_EOF,
} tix_token;

static FILE     *in_file= NULL;
static FILE     *out_file= NULL;


static void tix_error( const char *str )
{
    fprintf( stderr, "\nError: %s\n", str );
}


static void trieAdd( unsigned code, const char *input )
{
    fprintf( out_file, "0x%2.2x, 0x%2.2x,    ", code & 0xff, code >> 8 );
    for( ;; ) {
        fprintf( out_file, "0x%2.2x, ", *input );
        if( *input == '\0' )
            break;
        ++input;
    }
    fprintf( out_file, "\n" );
}

static tix_token get_tix_token( char *buff )
{
    int         c;
    char        *p;
    char        *end;
    unsigned    num;
    char        endc;

    for( ;; ) {
        c = getc( in_file );
        if( c == EOF ) return( TT_EOF );
        if( c == '#' ) {
            /* eat a comment */
            for( ;; ) {
                c = getc( in_file );
                if( c == EOF ) return( TT_EOF );
                if( c == '\n' ) break;
            }
        }
        if( !isspace( c ) ) break;
    }
    p = buff;
    if( c == '\'' || c == '\"' ) {
        /* collect a string */
        endc = c;
        for( ;; ) {
            c = getc( in_file );
            if( c == EOF ) break;
            if( c == '\r' ) break;
            if( c == '\n' ) break;
            if( c == endc ) break;
            if( c == '\\' ) {
                c = getc( in_file );
                if( c == EOF ) break;
                switch( c ) {
                case 'a':
                    c = '\a';
                    break;
                case 'b':
                    c = '\b';
                    break;
                case 'e':
                    c = '\x1b';
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
                        if( c == EOF ) break;
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
            *p++ = c;
        }
        *p = '\0';
        return( TT_CODE );
    } else {
        /* collect a string or number */
        for( ;; ) {
            *p++ = c;
            c = getc( in_file );
            if( c == EOF ) break;
            if( isspace( c ) ) break;
            if( c == '#' ) {
                ungetc( c, in_file );
                break;
            }
        }
        *p = '\0';
        num = strtoul( buff, &end, 0 );
        if( end != p ) return( TT_STRING );
        buff[0] = num & 0xff;
        buff[1] = num >> 8;
        return( TT_CODE );
    }
}

static int get_tix_code( char *buff )
{
    if( get_tix_token( buff ) != TT_CODE ) {
        tix_error( "expecting code" );
        return( -1 );
    }
    return( buff[0] + (buff[1] << 8) );
}

static const char acs_default[] =
        "q-x|l.m`k.j\'n+w-v-t|u|~*+>,<-^.vO#f`g?a#h#";

static char find_acs_map( char c, const char *acs )
{
    for( ;; ) {
        if( acs[0] == '\0' ) break;
        if( acs[0] == c ) return( acs[1] );
        ++acs;
        if( acs[0] == '\0' ) break;
        ++acs;
    }
    return( '\0' );
}

static tix_status do_parse()
{
    char        buff[80];
    char        input[80];
    tix_token   tok;
    int         code;
    char        c;

    tok = get_tix_token( buff );
    for( ;; ) {
        if( tok == TT_EOF ) break;
        if( tok != TT_STRING ) {
            tix_error( "expecting directive" );
            return( TIX_FAIL );
        }
        if( stricmp( buff, "display" ) == 0 ) {
            code = get_tix_code( buff );
            if( code == -1 ) return( 0 );
            tok = get_tix_token( buff );
            if( tok == TT_EOF ) break;
            if( tok == TT_STRING ) {
                if( stricmp( buff, "alt" ) != 0 ) {
                    tix_error( "expecting alt" );
                    return( TIX_FAIL );
                }
                tok = get_tix_token( buff );
                if( tok == TT_EOF ) break;
            }
            if( tok != TT_CODE ) {
                tix_error( "expecting display code" );
                return( TIX_FAIL );
            }
            tok = get_tix_token( buff );
        } else if( stricmp( buff, "key" ) == 0 ) {
            code = get_tix_code( buff );
            if( code == -1 ) return( TIX_FAIL );
            input[0] = '\0';
            for( ;; ) {
                tok = get_tix_token( buff );
                if( tok != TT_CODE ) break;
                strcat( input, buff );
            }
            trieAdd( code, input );
        } else {
            tix_error( "unknown directive" );
            return( TIX_FAIL );
        }
    }
    return( TIX_OK );
}

int main( int argc, char *argv[] )
{
    if( argc != 3 ) {
        fprintf( stderr, "Usage: buildesc <in-tix-file> <out-c-file>\n" );
        return( 1 );
    }
    in_file = fopen( argv[1], "r" );
    if( in_file == NULL ) {
        fprintf( stderr, "Can't open %s: %s\n", argv[0], strerror( errno ) );
        return( 1 );
    }

    out_file = fopen( argv[2], "w" );
    if( out_file == NULL ) {
        fprintf( stderr, "Can't open %s: %s\n", argv[0], strerror( errno ) );
        return( 1 );
    }
    fprintf( out_file, "\n\n\t// File generated by BUILDESC.C\n\n" );
    fprintf( out_file, "const char ConEscapes[] = {\n" );
    if( do_parse() == TIX_FAIL ) return( 1 );
    fprintf( out_file, "};\n" );
    return( 0 );
}
