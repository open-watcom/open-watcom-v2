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
#include "bool.h"
#include "stdui.h"
#include "uiextrn.h"
#include "tixparse.h"
#include "tixsupp.h"
#include "trie.h"
#include "doparse.h"


/***************************************************************************
****************************************************************************
*****                                                                   ****
***** NOTE: if adding new features to the .TIX files, don't forget to   ****
***** update the parsing routines in BUILDESC.C                         ****
*****                                                                   ****
****************************************************************************
***************************************************************************/

extern struct _console_ctrl *UIConCtrl;

char            ti_char_map[256];

static tix_status init_tix_scanner( const char *termname )
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
            if( strstr( termname, "qnx" ) != 0 ) {
                in_file = ti_fopen( "qnx.tix" );
            } else if( strstr( termname, "ansi" ) != 0 || strcmp( termname, "xterm" ) == 0 ) {
                in_file = ti_fopen( "ansi.tix" );
            }
        }
    }
    if( in_file == NULL ) {
        rc = TIX_NOFILE;
        if( UIConCtrl == NULL ) {
            in_file = ti_fopen( "default.tix" );
            if( in_file != NULL ) {
                rc = TIX_DEFAULT;
            }
        }
    }
    return( rc );
}

static void close_tix_scanner( void )
{
    if( in_file != NULL ) {
        fclose( in_file );
    }
}

tix_status ti_read_tix( const char *termname )
/********************************************/
{
    int         i;
    tix_status  ret;

    memset( _ti_alt_map, 0, sizeof( _ti_alt_map ) );

    for( i = 0; i < sizeof( ti_char_map ); i++ )
        ti_char_map[i] = i;

    ret = init_tix_scanner( termname );
    switch( ret ) {
    case TIX_NOFILE:
        if( UIConCtrl != NULL )
            return( ret );
        return( ui_tix_missing( termname ) ? TIX_OK : TIX_FAIL );
    }
    if( !do_parse() )
        ret = TIX_FAIL;
    close_tix_scanner();
    return( ret );
}
