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

#include "awk.h"

unsigned ctr = 20000;

gsub os2_subs[] = {
    { "&incvarup", "OS2_INCLUDE" },
    { "&wclcmdup", "wcl386" },
    { "&", "&amp." },
    { ":", "\&colon." },
    { NULL, NULL },
};

int main( int argc, char **argv )
{
    FILE *ifp;
    FILE *ofp;
    char *check;

    if( argc != 3 ) {
        fail( "mkos2hlp <gml-file> <hlp-file>" );
    }
    ifp = fopen( argv[1], "r" );
    if( ifp == NULL ) {
        fail( "cannot open input file '%s'", argv[1] );
    }
    ofp = fopen( argv[2], "w" );
    if( ofp == NULL ) {
        fail( "cannot open input file '%s'", argv[2] );
    }
    fprintf( ofp, ":userdoc.\n" );
    for(;;) {
        check = fgets( ibuff, sizeof(ibuff), ifp );
        if( check == NULL ) break;
        make_fields();
        if( match( field[0], ":MSGLVL", ":eMSGLVL", ".np", NULL ) ) {
            // do zip
        } else if( match( field[0], ":cmt", NULL ) ) {
            field[0] = ".*";
            print_fields( ofp );
            fputc( '\n', ofp );
        } else if( match( field[0], ":MSGSYM", NULL ) ) {
            fprintf( ofp, ".* %s %s\n", field[0], field[1] );
            fprintf( ofp, ".*\n" );
        } else if( match( field[0], ":ANSI", NULL ) ) {
            fprintf( ofp, ".* %s %s\n", field[0], field[1] );
            fprintf( ofp, ".*\n" );
        } else if( match( field[0], ":EXTWARN", NULL ) ) {
            fprintf( ofp, ".* %s %s\n", field[0], field[1] );
            fprintf( ofp, ".*\n" );
        } else if( match( field[0], ":INFO", NULL ) ) {
            fprintf( ofp, ".* %s %s\n", field[0], field[1] );
            fprintf( ofp, ".*\n" );
        } else if( match( field[0], ":WARNING", NULL ) ) {
            fprintf( ofp, ".* %s %s\n", field[0], field[1] );
            fprintf( ofp, ".*\n" );
        } else if( match( field[0], ":MSGTXT", NULL ) ) {
            field[0] = "";
            unmake_fields();
            substitutions( os2_subs );
            fprintf( ofp, ":h1 res=%d. ", ctr );
            print_fields( ofp );
            fputc( '\n', ofp );
            ++ctr;
            fprintf( ofp, ":hp2. " );
            print_fields( ofp );
            fprintf( ofp, ":ehp2.\n" );
            fprintf( ofp, ":p.\n" );
        } else if( match( field[0], ":errbad", ":errgood", NULL ) ) {
            fprintf( ofp, ":xmp.\n" );
        } else if( match( field[0], ":eerrbad", ":eerrgood", NULL ) ) {
            fprintf( ofp, ":exmp.\n" );
        } else if( match( field[0], ".kw", ".id", ".us", ".ev", NULL ) ) {
            field[0] = "";
            unmake_fields();
            substitutions( os2_subs );
            fprintf( ofp, ":hp3.%s:ehp3.\n", ibuff );
        } else {
            substitutions( os2_subs );
            fprintf( ofp, "%s", ibuff );
        }
    }
    fprintf( ofp, ":euserdoc.\n" );
    fclose( ifp );
    fclose( ofp );
    return( EXIT_SUCCESS );
}
