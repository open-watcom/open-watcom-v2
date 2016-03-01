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
* Description:  Parse the .tix terminal description file.
*
****************************************************************************/


#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#if defined(__WATCOMC__)
    #include <sys/ioctl.h>
#elif !defined( HP )
    #include <termios.h>
#else
    #include <stdarg.h>
#endif

#if defined(__SUNPRO_CC) || defined(__SUNPRO_C)
    // curses.h when using SUNPRO compiler has its own typedef for bool
    #define bool        curses_bool
    #include <curses.h>
    #include <term.h>
    #undef bool
#else
    #include <curses.h>
#include <term.h>
#endif

#include <unistd.h>

#include "stdui.h"
#include "unxuiext.h"
#include "tixparse.h"
#include "trie.h"


#define TC_ERROR    ((unsigned)-1)

typedef enum {
    TT_CODE,
    TT_STRING,
    TT_EOF
} tix_token;

extern int      ui_tix_missing( const char *name );

extern char     ui_tix_path[];

char            ti_char_map[256][4];
unsigned char   _ti_alt_map[32];

static FILE     *in_file = NULL;

static void tix_error( const char *str )
/**************************************/
{
    const char  *term;

    term = GetTermType();
    uiwritec( "\nError in " );
    uiwrite( term );
    uiwritec( ": " );
    uiwrite( str );
    uiwritec( "\n" );
}

FILE *ti_fopen( const char *fnam )
/********************************/
{
    FILE        *res;
    const char  *homeDir;
    char        fpath[FILENAME_MAX + 1];

    if( fnam == NULL || fnam[0] == '\0' ) {
        return( NULL );
    }

    // first look in current directory
    res = fopen( fnam, "r" );
    if( res != NULL ) {
        return( res );
    }

    // if it's not there, look in the user's home directory
    homeDir = getenv( "HOME" );
    if( homeDir != NULL && homeDir[0] != '\0' ) {
        strcpy( fpath, homeDir );
        strcat( fpath, "/" );
        strcat( fpath, fnam );

        res = fopen( fpath, "r" );
        if( res != NULL ) {
            return( res );
        }
    }

    // finally, look in /usr/watcom/tix/<name>
//    strcpy( fpath, TIX_PATH_NAME );
    strcpy( fpath, ui_tix_path );
    strcat( fpath, fnam );
    res = fopen( fpath, "r" );
    return( res );
}

static int init_tix_scanner( const char *name )
/*********************************************/
{
    char        tix_name[19];


    if( name != NULL ) {
        if( *name != '\0' ) {
            strcpy( tix_name, name );
            strcat( tix_name, ".tix" );
            in_file = ti_fopen( tix_name );
            if( in_file != NULL ) {
                return( 1 );
            }
        }
        if( strstr( name, "qnx" ) != 0 ) {
            in_file = ti_fopen( "qnx.tix" );
            if( in_file != NULL ) {
                return( 1 );
            }
        } else if( strstr( name, "ansi" ) != 0 ) {
            in_file = ti_fopen( "ansi.tix" );
            if( in_file != NULL ) {
                return( 1 );
            }
        } else if( strstr( name, "xterm" ) != 0 ) {
            in_file = ti_fopen( "xterm.tix" );
            if( in_file != NULL )
                return( 1 );
            in_file = ti_fopen( "ansi.tix" );
            if( in_file != NULL ) {
                return( 1 );
            }
        }
    }
    in_file = ti_fopen( "default.tix" );
    return( in_file != NULL );
}

static void close_tix_scanner( void )
/***********************************/
{
    if( in_file != NULL ) {
        fclose( in_file );
        in_file = NULL;
    }
}

static tix_token get_tix_token( char *buff )
/******************************************/
{
    int         c;
    char        *p;
    char        *end;
    unsigned    num;
    char        endc;

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
            *p++ = c;
        }
        *p = '\0';
        return( TT_CODE );
    } else {
        /* collect a string or number */
        for( ;; ) {
            *p++ = c;
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
        buff[0] = num & 0xff;
        buff[1] = num >> 8;
        return( TT_CODE );
    }
}

static unsigned get_tix_code( char *buff )
/****************************************/
{
    if( get_tix_token( buff ) != TT_CODE ) {
        tix_error( "expecting code" );
        return( TC_ERROR );
    }
    return( *(unsigned char *)buff + ( *(unsigned char *)( buff + 1 ) << 8 ) );
}

static const char acs_default[] = "q-x|l+m+k+j+n+w+v+t+u+~o+>,<-^.v0#f\\g#a:h#";

static char find_acs_map( char c, const char *acs )
/*************************************************/
{
    if( acs != NULL ) {
        while( acs[0] != '\0' ) {
            if( acs[0] == c )
                return( acs[1] );
            ++acs;
            if( acs[0] == '\0' )
                break;
            ++acs;
        }
    }
    return( '\0' );
}

static int do_parse( void )
/*************************/
{
    char        buff[80];
    char        input[80];
    tix_token   tok;
    unsigned    code;
    char        c;

    tok = get_tix_token( buff );
    while( tok != TT_EOF ) {
        if( tok != TT_STRING ) {
            tix_error( "expecting directive" );
            return( 0 );
        }
        if( strcasecmp( buff, "display" ) == 0 ) {
            code = get_tix_code( buff );
            if( code == TC_ERROR )
                return( 0 );
            tok = get_tix_token( buff );
            if( tok == TT_EOF )
                break;
            if( tok == TT_STRING ) {
                if( strcasecmp( buff, "alt" ) != 0 ) {
                    tix_error( "expecting alt" );
                    return( 0 );
                }
                tok = get_tix_token( buff );
                if( tok == TT_EOF )
                    break;
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
                return( 0 );
            }
            ti_char_map[code][0] = buff[0];
            tok = get_tix_token( buff );
        } else if( strcasecmp( buff, "key" ) == 0 ) {
            code = get_tix_code( buff );
            if( code == TC_ERROR )
                return( 0 );
            input[0] = '\0';
            while( (tok = get_tix_token( buff )) == TT_CODE ) {
                strcat( input, buff );
            }
            TrieAdd( code, input );
        } else {
            tix_error( "unknown directive" );
        }
    }
    return( 1 );
}

struct charmap {
    unsigned char vt100;
    unsigned short unicode;
};

static const struct charmap default_tix[] = {
    /* keep zero to handle casestrings */
    {0, 0},

    /* single line box drawing */
    {'m', 0x2514}, /* UI_LLCORNER  */
    {'j', 0x2518}, /* UI_LRCORNER  */
    {'l', 0x250c}, /* UI_ULCORNER  */
    {'k', 0x2510}, /* UI_URCORNER  */
    {'q', 0x2500}, /* UI_HLINE     */
    {'x', 0x2502}, /* UI_VLINE     */
    {'w', 0x252c}, /* UI_TTEE      */
    {'u', 0x2524}, /* UI_RTEE      */
    {'t', 0x251c}, /* UI_LTEE      */

    /* double line box drawing */
    {'m', 0x255a}, /* UI_DLLCORNER */
    {'j', 0x255d}, /* UI_DLRCORNER */
    {'l', 0x2554}, /* UI_DULCORNER */
    {'k', 0x2557}, /* UI_DURCORNER */
    {'q', 0x2550}, /* UI_DHLINE    */
    {'x', 0x2551}, /* UI_DVLINE    */

    /* triangles */
    {'.', 0x25bc}, /* UI_DPOINT    */ // 0x2193
    {',', 0x25c4}, /* UI_LPOINT    */ // 0x2190
    {'+', 0x25ba}, /* UI_RPOINT    */ // 0x2192
    {'-', 0x25b2}, /* UI_UPOINT    */ // 0x2191

    /* arrows */
    {'.', 0x2193}, /* UI_DARROW    */
    {'/', 0x2195}, /* UI_UDARROW   */

    /* boxes */
    {0xa0,0x2584}, /* UI_DBLOCK    */
    {'0', 0x258c}, /* UI_LBLOCK    */
    {0xa0,0x2590}, /* UI_RBLOCK    */
    {'0', 0x2580}, /* UI_UBLOCK    */
    {'a', 0x2591}, /* UI_CKBOARD   */
    {'h', 0x2592}, /* UI_BOARD     */
    {'0', 0x2588}, /* UI_BLOCK     */

    /* misc */
    {'h', 0x25a0}, /* UI_SQUARE    */
    {'*', 0x221a}, /* UI_ROOT      */
    {'=', 0x2261}, /* UI_EQUIVALENT*/
};

static const char alt_keys[] = "QWERTYUIOP[]\r\0ASDFGHJKL;'`\0\\ZXCVBNM,./";
static const char alt_num_keys[] = "1234567890-=";

/* use above table if no .tix file is found */
static int do_default( void )
/***************************/
{
    unsigned char       c, cmap;
    int                 i;
    char                esc_str[3];

    esc_str[0] = '\033';
    esc_str[1] = 'A';
    esc_str[2] = '\0';
    for( i = 0; i < sizeof( default_tix ) / sizeof( default_tix[0] ) ; i ++ ) {
        cmap = c = default_tix[i].vt100;
        if( (c & 0x80) == 0 ) {
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
        }
        ti_char_map[i][0] = cmap;
    }
    for( i = 0; i < sizeof( alt_keys ); i++ ) {
        if( alt_keys[i] ) {
            esc_str[1] = alt_keys[i];
            TrieAdd( 0x110 + i, esc_str );
            if( alt_keys[i] >= 'A' && alt_keys[i] <= 'Z' ) {
                esc_str[1] += 0x20;
                TrieAdd( 0x110 + i, esc_str );
            }
        }
    }
    for( i = 0; i < sizeof( alt_num_keys ); i++ ) {
        if ( alt_num_keys[i] ) {
            esc_str[1] = alt_num_keys[i];
            TrieAdd( 0x178 + i, esc_str );
        }
    }
    /* sticky function key ^F */
    TrieAdd( 0xff0, "\6");
    /* sticky ALT ^A */
    TrieAdd( 0xff3, "\1");
    return( 1 );
}


int ti_read_tix( const char *termname )
/*************************************/
{
    int         i;
    int         ret;
    const char  *s;
    int         utf8_mode = 0;

    memset( _ti_alt_map, 0, sizeof( _ti_alt_map ) );

    for( i = 0; i < sizeof( ti_char_map ) / sizeof( ti_char_map[0] ); i++ )
        ti_char_map[i][0] = i;

    if( !init_tix_scanner( termname ) ) {
        ret = do_default();
    } else {
        ret = do_parse();
        close_tix_scanner();
    }

    if( ( (s = getenv( "LC_ALL" )) != NULL && *s != '\0' ) ||
         ( (s = getenv( "LC_CTYPE" )) != NULL && *s != '\0' ) ||
         ( (s = getenv( "LANG" )) != NULL && *s != '\0' ) ) {
        if( strstr( s, "UTF" ) || strstr( s, "utf" ) ) {
            utf8_mode = 1;
        }
    }

    if( utf8_mode ) {
        /* handle at least iso-8859-1 for now */
        for( i = 0xa0; i < 0x100; i++ ) {
            wctomb( ti_char_map[i], i );
        }
    }

#if 0
    /* do not play woth utf8 mode setting: all VT are already configured
       as needed. With this code on there is a problem with line drawing
       on the linux console (framebuffer mode) */

    if( strncmp( termname, "linux", 5 ) == 0 ) {
        /* force UTF-8 mode if the locale is set that way; *
         * we may be on a new VT on the Linux console      */
        if ( utf8_mode ) {
            uiwritec( "\033%G" );
            /* use UTF-8 characters instead of ACS */
            for( i = 0; i < sizeof( default_tix ) / sizeof( default_tix[0] ); i++ ) {
                wctomb( ti_char_map[i], default_tix[i].unicode );
            }
        } else {
            uiwritec( "\033%@" );
        }
    }
#endif

    if( strncmp( termname, "xterm", 5 ) == 0 ) {
        /* special xterm keys available in recent xterms */
        TrieAdd( EV_CTRL_CURSOR_UP, "\033[1;5A" );
        TrieAdd( EV_CTRL_CURSOR_DOWN, "\033[1;5B" );
        TrieAdd( EV_CTRL_CURSOR_RIGHT, "\033[1;5C" );
        TrieAdd( EV_CTRL_CURSOR_LEFT, "\033[1;5D" );
        TrieAdd( EV_CTRL_HOME, "\033[1;5H" );
        TrieAdd( EV_CTRL_END, "\033[1;5F" );
        TrieAdd( EV_CTRL_PAGE_UP, "\033[5;5~" );
        TrieAdd( EV_CTRL_PAGE_DOWN, "\033[6;5~" );

        /* slightly older xterms report these sequences... */
        TrieAdd( EV_CTRL_CURSOR_UP, "\033[O5A" );
        TrieAdd( EV_CTRL_CURSOR_DOWN, "\033[O5B" );
        TrieAdd( EV_CTRL_CURSOR_RIGHT, "\033[O5C" );
        TrieAdd( EV_CTRL_CURSOR_LEFT, "\033[O5D" );
        TrieAdd( EV_CTRL_HOME, "\033[O5H" );
        TrieAdd( EV_CTRL_END, "\033[O5F" );

        /* Red Hat 8 xterm has yet different sequences. Does
         * not differentiate between Home/End and Ctrl + Home/End,
         * but does for PgUp/PgDn (same codes as newer xterms above)
         */
        TrieAdd( EV_CTRL_CURSOR_UP, "\033O5A" );
        TrieAdd( EV_CTRL_CURSOR_DOWN, "\033O5B" );
        TrieAdd( EV_CTRL_CURSOR_RIGHT, "\033O5C" );
        TrieAdd( EV_CTRL_CURSOR_LEFT, "\033O5D" );
    }
    return( ret );
}
