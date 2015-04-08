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
#include <termios.h>
#include <term.h>

#include <sys/uio.h>
#include <unistd.h>

#include "stdui.h"
#include "qnxuiext.h"

#include "tixparse.h"
#include "trie.h"

/***************************************************************************
****************************************************************************
*****                                                                   ****
***** NOTE: if adding new features to the .TIX files, don't forget to   ****
***** update the parsing routines in BUILDESC.C                         ****
*****                                                                   ****
****************************************************************************
***************************************************************************/

extern char ui_tix_path[];
extern int ui_tix_missing( const char *name );

typedef enum {
    TT_CODE,
    TT_STRING,
    TT_EOF,
} tix_token;

static FILE     *in_file= NULL;

char            ti_char_map[256];
unsigned char   _ti_alt_map[32];

static void tix_error( const char *str )
{
    static char header[]= "\nError in ";

#define uiwrite( s ) write( UIConHandle, s, strlen( s ) );

    write( UIConHandle, header, sizeof( header )-1 );
    uiwrite( __cur_term->_termname );
    uiwrite( ": " );
    uiwrite( str );
    uiwrite( "\n" );
}

#define TIX_PATH_NAME   "/usr/watcom/tix/"
#define TIX_PATH_LEN    ( sizeof( TIX_PATH_NAME ) )

static FILE *ti_fopen( const char *fnam )
{
    FILE        *res;
    char        *fpath;
    char        *homeDir;
    unsigned    size;
    unsigned    len;

    if( fnam == NULL || fnam[0] == '\0' ) {
        return( NULL );
    }

    // first look in current directory
    res= fopen( fnam, "r" );
    if( res != NULL )
        return( res );

    // if it's not there, look in the user's home directory
    homeDir= getenv( "HOME" );
    if( homeDir == NULL )
        homeDir = "";
    size = strlen( homeDir ) + 1;
    len = strlen( ui_tix_path );
    if( len > size )
        size = len;
    fpath = alloca( size + strlen( fnam ) + 1 );
    if( fpath == NULL )
        return( NULL );

    if( homeDir[0] != '\0' ) {
        strcpy( fpath, homeDir );
        strcat( fpath, "/" );
        strcat( fpath, fnam );

        res= fopen( fpath, "r" );
        if( res!=NULL ) {
            return( res );
        }
    }

    // finally, look in /usr/watcom/tix/<name>
    strcpy( fpath, ui_tix_path );
    strcat( fpath, fnam );
    res= fopen( fpath, "r" );
    return( res );
}

static tix_status init_tix_scanner( const char *name, bool use_default )
{
    char        tix_name[19];

    strcpy( tix_name, name );
    strcat( tix_name, ".tix" );
    in_file = ti_fopen( tix_name );
    if( in_file != NULL ) return( TIX_OK );
    if( strstr( name, "qnx" ) != 0 ) {
        in_file = ti_fopen( "qnx.tix" );
    } else if( strstr( name, "ansi" ) != 0 || strcmp( name, "xterm" ) == 0 ) {
        in_file = ti_fopen( "ansi.tix" );
    }
    if( in_file != NULL ) return( TIX_OK );
    if( use_default ) {
        in_file = ti_fopen( "default.tix" );
        if( in_file != NULL ) return( TIX_DEFAULT );
    }
    return( TIX_NOFILE );
}

static void close_tix_scanner( void )
{
    if( in_file != NULL ) {
        fclose( in_file );
    }
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

static tix_status do_parse( void )
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
                c = find_acs_map( buff[0], acs_chars );
                if( c != '\0' ) {
                    ti_alt_map_set( code );
                } else {
                    c = find_acs_map( buff[0], acs_default );
                    if( c == '\0' ) {
                        c = buff[0];
                        ti_alt_map_set( code );
                    }
                }
                buff[0] = c;
            }
            if( tok != TT_CODE ) {
                tix_error( "expecting display code" );
                return( TIX_FAIL );
            }
            ti_char_map[ code ] = buff[0];
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
            TrieAdd( code, input );
        } else {
            tix_error( "unknown directive" );
            return( TIX_FAIL );
        }
    }
    return( TIX_OK );
}

tix_status ti_read_tix( bool use_default )
/***************************************/
{
    int         i;
    int         ret;

    memset( _ti_alt_map, 0, sizeof( _ti_alt_map ) );

    for( i = 0; i < sizeof( ti_char_map ); i++ ) ti_char_map[i]=i;

    ret = init_tix_scanner( __cur_term->_termname, use_default );
    switch( ret ) {
    case TIX_FAIL:
        return( ret );
    case TIX_NOFILE:
        if( !use_default ) return( ret );
        return( ui_tix_missing( __cur_term->_termname ) ? TIX_OK : TIX_FAIL );
    }
    if( do_parse() == TIX_FAIL ) ret = TIX_FAIL;
    close_tix_scanner();
    return( ret );
}
