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


/********************************************************************
    CGUIPP - CGUI C Preprocessor
    ============================

when            who             why
====            ===             ===
90-nov-23       djp             created.
93-apr-27       John Dahms      double up backslashes in the #line directive

    This program preprocesses CGUI C files containing the following special
    directives: _IF_CGUI, _IF_NOT_CGUI, _ELSE_CGUI, _ENDIF_CGUI.

*********************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

enum {
    _IF_CGUI,
    _IF_NOT_CGUI,
    _ELSE_CGUI,
    _ENDIF_CGUI
};

enum {
    WANT_CGUI,
    DONT_WANT_CGUI
};

static char *Token_strs[]={
    "_IF_CGUI",
    "_IF_NOT_CGUI",
    "_ELSE_CGUI",
    "_ENDIF_CGUI"
};

static char *Sub_strs[4][2]={
    { "if( Cgui_active ) { ",           "#ifdef CGUI " },
    { "if( !Cgui_active ) { ",          "#ifndef CGUI " },
    { "} else { ",                      "#else " },
    { "} ",                             "#endif " }
};


main(
/***/

    int                 argc,
    char                *argv[]
) {
    FILE                *in_file;
    FILE                *out_file;
    char                buf[500];
    int                 is_cgui;
    int                 token;
    char                *match;
    char                tmp;
    char                *src;
    char                *dst;

    if( argc != 4 ) {
        printf( "format: cguipp [in file] [out file] [CGUI|NOT_CGUI]\n" );
        exit( -1 );
    }

    if( strcmp( argv[3], "CGUI" ) == 0 ) {
        is_cgui = WANT_CGUI;
    } else {
        is_cgui = DONT_WANT_CGUI;
    }

    in_file = fopen( argv[1], "r" );
    if( in_file == NULL ) {
        printf( "could not open input file:%s\n", argv[1] );
        exit( -1 );
    }

    out_file = fopen( argv[2], "w" );
    if( out_file == NULL ) {
        printf( "could not open output file:%s\n", argv[2] );
        exit( -1 );
    }

    src = argv[1];
    dst = buf;
    while( *src ) {
        if( *src == '\\' ) {
            *dst++ = '\\';
        }
        *dst++ = *src++;
    }
    *dst = '\0';
    fprintf( out_file, "#line 1 \"%s\"\n", buf );

    while( NULL != fgets( buf, 500, in_file ) ) {
        match = NULL;
        for( token = 0; token < 4; ++token ) {
            match = strstr( buf, Token_strs[token] );
            if( match != NULL ) {
                break;
            }
        }
        if( match == NULL ) {
            fputs( buf, out_file );
        } else {
            tmp = *match;
            *match = '\0';
            fputs( buf, out_file );
            fputs( Sub_strs[token][is_cgui], out_file );
            match += strlen( Token_strs[token] );
            fputs( match, out_file );
        }
    }

    fclose( in_file );
    fclose( out_file );
}
