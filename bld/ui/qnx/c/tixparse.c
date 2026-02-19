/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2022 The Open Watcom Contributors. All Rights Reserved.
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
#include <sys/uio.h>
#include <unistd.h>
#include "wterm.h"
#include "stdui.h"
#include "tixparse.h"
#include "tixsupp.h"
#include "trie.h"
#include "uiintern.h"
#include "uiextrn.h"
#include "doparse.h"


/***************************************************************************
****************************************************************************
*****                                                                   ****
***** NOTE: if adding new features to the .TIX files, don't forget to   ****
***** update the parsing routines in BUILDESC.C                         ****
*****                                                                   ****
****************************************************************************
***************************************************************************/

// macros for getting/setting bits in alt-char map
#define ti_alt_map_set( x ) ( _ti_alt_map[( x ) / 8] |= ( 1 << ( ( x ) % 8 ) ) )

extern struct _console_ctrl *UIConCtrl;

unsigned char   _ti_alt_map[32];
char            ti_char_map[256][MB_LEN_MAX];

static const char acs_default[] = "q-x|l.m`k.j\'n+w-v-t|u|~*+>,<-^.vO#f`g?a#h#";

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

static char set_ti_alt_map( unsigned i, char c )
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

static tix_status init_tix_scanner( const char *termname, FILE **in_file )
{
    char        *tix_name;
    size_t      len;
    tix_status  rc;
    FILE        *fp;

    fp = NULL;
    rc = TIX_OK;
    if( *termname != '\0' ) {
        len = strlen( termname ) + 5;
        tix_name = uimalloc( len );
        strcpy( tix_name, termname );
        strcat( tix_name, ".tix" );
        fp = ti_fopen( tix_name );
        uifree( tix_name );
        if( fp == NULL ) {
            if( strstr( termname, "qnx" ) != 0 ) {
                fp = ti_fopen( "qnx.tix" );
            } else if( strstr( termname, "ansi" ) != 0 || strcmp( termname, "xterm" ) == 0 ) {
                fp = ti_fopen( "ansi.tix" );
            }
        }
    }
    if( fp == NULL ) {
        rc = TIX_NOFILE;
        if( UIConCtrl == NULL ) {
            fp = ti_fopen( "default.tix" );
            if( fp != NULL ) {
                rc = TIX_DEFAULT;
            }
        }
    }
    *in_file = fp;
    return( rc );
}

static void close_tix_scanner( FILE *in_file )
{
    if( in_file != NULL ) {
        fclose( in_file );
    }
}

void ti_map_display_code( char c, bool alt_map )
{
    if( alt_map ) {
        ti_char_map[TT_CODE][0] = set_ti_alt_map( TT_CODE, c );
    } else {
        ti_char_map[TT_CODE][0] = c;
    }
    ti_char_map[TT_CODE][1] = 0;
}

void tix_error( const char *str )
{
    uiwritec( "\nError in " );
    uiwrite( GetTermType() );
    uiwritec( ": " );
    uiwrite( str );
    uiwritec( "\n" );
}

tix_status ti_read_tix( const char *termname )
/********************************************/
{
    int         i;
    tix_status  ret;
    FILE        *in_file;

    memset( _ti_alt_map, 0, sizeof( _ti_alt_map ) );

    for( i = 0; i < sizeof( ti_char_map ) / sizeof( ti_char_map[0] ); i++ ) {
        ti_char_map[i][0] = i;
        ti_char_map[i][1] = 0;
    }
    ret = init_tix_scanner( termname, &in_file );
    switch( ret ) {
    case TIX_NOFILE:
        if( UIConCtrl != NULL )
            return( ret );
        return( ui_tix_missing( termname ) ? TIX_OK : TIX_FAIL );
    }
    if( !do_parse( in_file ) )
        ret = TIX_FAIL;
    close_tix_scanner( in_file );
    return( ret );
}
