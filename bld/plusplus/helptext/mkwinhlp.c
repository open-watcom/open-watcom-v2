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
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "awk.h"

unsigned ctr;
int have_text;
int in_example;
int blank_line;
int in_msg;
char *err_code;

gsub win_subs[] = {
    { "&incvarup", "DOS_INCLUDE" },
    { "&wclcmdup", "wcl386" },
    //{ "\\", "\\\\" },
    { "{", "\\{" },
    { "}", "\\}" },
    { NULL, NULL },
};

void flush_text( FILE *fp )
{
    if( have_text ) {
        fprintf( fp, "\n" );
        have_text = 0;
    }
}

void flush_example( FILE *fp )
{
    have_text = 1;
    blank_line = 0;
    if( in_example ) {
        flush_text( fp );
    }
}

void emit_blank_line( FILE *fp )
{
    if( ! blank_line ) {
        fprintf( fp, "\n" );
        blank_line = 1;
    }
}

void concat_text( FILE *fp )
{
    if( have_text && ! in_example ) {
        fprintf( fp, " " );
    }
}

void emit_mono( FILE *fp )
{
    size_t len = strlen( ibuff );
    if( len ) {
        if( ibuff[len-1] == '\\' ) {
            ibuff[len] = ' ';
            ibuff[len+1] = '\0';
        }
    }
    fprintf( fp, "{\\f4 %s}", ibuff );
}

void remove_tag()
{
    field[0] = "";
    unmake_fields();
    // remove first blank after null field
    strcpy( ibuff, ibuff+1 );
}

void emit_index( FILE *fp, FILE *hfp, char *code, char *text )
{
    fprintf( fp, "%c%s%c%s\n", 247, code, 247, text );
    ++ctr;
    fprintf( hfp, "#define %s %d\n", code, ctr );
}

int main( int argc, char **argv )
{
    FILE *ifp;
    FILE *ofp;
    FILE *hfp;
    char *check;
    size_t len;

    if( argc != 3 ) {
        fail( "mkwinhlp <gml-file> <hlp-file>" );
    }
    ifp = fopen( argv[1], "r" );
    if( ifp == NULL ) {
        fail( "cannot open input file '%s'", argv[1] );
    }
    ofp = fopen( argv[2], "w" );
    if( ofp == NULL ) {
        fail( "cannot open output file '%s'", argv[2] );
    }
    hfp = fopen( "winhelp.gh", "w" );
    if( hfp == NULL ) {
        fail( "cannot open output file '%s'", "winhelp.gh" );
    }
    err_code = strdup( "" );
    fprintf( hfp, "\n" );
    for(;;) {
        check = fgets( ibuff, sizeof(ibuff), ifp );
        if( check == NULL ) break;
        len = strlen(ibuff);
        if( len ) {
            // remove '\n' on the end
            ibuff[len-1] = '\0';
        }
        make_fields();
        if( field[0] == NULL ) {
            if( in_msg ) {
                emit_blank_line( ofp );
            }
        } else if( match( field[0], ":MSGLVL", ":eMSGLVL", ".np", NULL ) ) {
            // nothing
        } else if( match( field[0], ":cmt", NULL ) ) {
            // nothing
        } else if( match( field[0], ":MSGSYM", NULL ) ) {
            in_msg = 0;
            free( err_code );
            err_code = strdup( field[1] );
        } else if( match( field[0], ":MSGTXT", NULL ) ) {
            remove_tag();
            flush_text( ofp );
            substitutions( win_subs );
            emit_index( ofp, hfp, err_code, ibuff );
            blank_line = 0;
            in_msg = 1;
        } else if( match( field[0], ":errbad", ":errgood", NULL ) ) {
            flush_text( ofp );
            emit_blank_line( ofp );
            in_example = 1;
        } else if( match( field[0], ":eerrbad", ":eerrgood", NULL ) ) {
            emit_blank_line( ofp );
            in_example = 0;
        } else if( match( field[0], ".kw", ".id", ".us", ".ev", NULL ) ) {
            remove_tag();
            concat_text( ofp );
            substitutions( win_subs );
            emit_mono( ofp );
            flush_example( ofp );
        } else if( a_tag( field[0] ) ) {
            // do zip
        } else {
            concat_text( ofp );
            substitutions( win_subs );
            if( in_example ) {
                emit_mono( ofp );
            } else {
                fprintf( ofp, "%s", ibuff );
            }
            flush_example( ofp );
        }
    }
    flush_text( ofp );
    emit_index( ofp, hfp, "index", "Index" );
    fclose( hfp );
    fclose( ifp );
    fclose( ofp );
    return( EXIT_SUCCESS );
}
