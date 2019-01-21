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


#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

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
    #include "wterm.h"
#endif


#include "uidef.h"
#include "uiextrn.h"
#include "tixparse.h"
#include "tixsupp.h"
#include "trie.h"
#include "doparse.h"


char            ti_char_map[256][4];

static tix_status init_tix_scanner( const char *termname )
/********************************************************/
{
    char        *tix_name;
    size_t      len;
    tix_status  rc;

    in_file = NULL;
    rc = TIX_OK;
    if( *termname != '\0' ) {
        len = strlen( termname ) + 5;
        tix_name = uimalloc( len );
        strcpy( tix_name, termname );
        strcat( tix_name, ".tix" );
        in_file = ti_fopen( tix_name );
        uifree( tix_name );
        if( in_file == NULL ) {
            if( strstr( termname, "ansi" ) != 0 ) {
                in_file = ti_fopen( "ansi.tix" );
            } else if( strstr( termname, "xterm" ) != 0 ) {
                in_file = ti_fopen( "xterm.tix" );
                if( in_file == NULL ) {
                    in_file = ti_fopen( "ansi.tix" );
                }
            }
        }
    }
    if( in_file == NULL ) {
        rc = TIX_NOFILE;
        in_file = ti_fopen( "default.tix" );
        if( in_file != NULL ) {
            rc = TIX_DEFAULT;
        }
    }
    return( rc );
}

static void close_tix_scanner( void )
/***********************************/
{
    if( in_file != NULL ) {
        fclose( in_file );
        in_file = NULL;
    }
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
    unsigned char       c;
    int                 i;
    char                esc_str[3];

    esc_str[0] = _ESC_CHAR;
    esc_str[1] = 'A';
    esc_str[2] = '\0';
    for( i = 0; i < sizeof( default_tix ) / sizeof( default_tix[0] ); i++ ) {
        c = default_tix[i].vt100;
        if( (c & 0x80) == 0 )
            c = set_ti_alt_map( i, c );
        ti_char_map[i][0] = c;
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
        if( alt_num_keys[i] ) {
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


tix_status ti_read_tix( const char *termname )
/********************************************/
{
    int         i;
    tix_status  ret;
    const char  *s;
    int         utf8_mode = 0;

    memset( _ti_alt_map, 0, sizeof( _ti_alt_map ) );

    for( i = 0; i < sizeof( ti_char_map ) / sizeof( ti_char_map[0] ); i++ )
        ti_char_map[i][0] = i;

    ret = init_tix_scanner( termname );
    switch( ret ) {
    case TIX_NOFILE:
        do_default();
        ret = TIX_DEFAULT;
        break;
    case TIX_OK:
    case TIX_DEFAULT:
        if( !do_parse() )
            ret = TIX_FAIL;
        close_tix_scanner();
        break;
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
    /* do not play with utf8 mode setting: all VT are already configured
       as needed. With this code on there is a problem with line drawing
       on the linux console (framebuffer mode) */

    if( strncmp( termname, "linux", 5 ) == 0 ) {
        /* force UTF-8 mode if the locale is set that way; *
         * we may be on a new VT on the Linux console      */
        if( utf8_mode ) {
            uiwritec( _ESC "%G" );
            /* use UTF-8 characters instead of ACS */
            for( i = 0; i < sizeof( default_tix ) / sizeof( default_tix[0] ); i++ ) {
                wctomb( ti_char_map[i], default_tix[i].unicode );
            }
        } else {
            uiwritec( _ESC "%@" );
        }
    }
#endif

    if( strncmp( termname, "xterm", 5 ) == 0 ) {
        /* special xterm keys available in recent xterms */
        TrieAdd( EV_CTRL_CURSOR_UP,     _ESC "[1;5A" );
        TrieAdd( EV_CTRL_CURSOR_DOWN,   _ESC "[1;5B" );
        TrieAdd( EV_CTRL_CURSOR_RIGHT,  _ESC "[1;5C" );
        TrieAdd( EV_CTRL_CURSOR_LEFT,   _ESC "[1;5D" );
        TrieAdd( EV_CTRL_HOME,          _ESC "[1;5H" );
        TrieAdd( EV_CTRL_END,           _ESC "[1;5F" );
        TrieAdd( EV_CTRL_PAGE_UP,       _ESC "[5;5~" );
        TrieAdd( EV_CTRL_PAGE_DOWN,     _ESC "[6;5~" );

        /* slightly older xterms report these sequences... */
        TrieAdd( EV_CTRL_CURSOR_UP,     _ESC "[O5A" );
        TrieAdd( EV_CTRL_CURSOR_DOWN,   _ESC "[O5B" );
        TrieAdd( EV_CTRL_CURSOR_RIGHT,  _ESC "[O5C" );
        TrieAdd( EV_CTRL_CURSOR_LEFT,   _ESC "[O5D" );
        TrieAdd( EV_CTRL_HOME,          _ESC "[O5H" );
        TrieAdd( EV_CTRL_END,           _ESC "[O5F" );

        /* Red Hat 8 xterm has yet different sequences. Does
         * not differentiate between Home/End and Ctrl + Home/End,
         * but does for PgUp/PgDn (same codes as newer xterms above)
         */
        TrieAdd( EV_CTRL_CURSOR_UP,     _ESC "O5A" );
        TrieAdd( EV_CTRL_CURSOR_DOWN,   _ESC "O5B" );
        TrieAdd( EV_CTRL_CURSOR_RIGHT,  _ESC "O5C" );
        TrieAdd( EV_CTRL_CURSOR_LEFT,   _ESC "O5D" );
    }
    return( ret );
}
