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

#include "ctokens.h"
#include "xlat.gh"

char *Tokens[] = {

#define pick(token,string,class) string
#include "ctokens.h"


void main( int argc, char **argv )
{
    FILE *ofp;
    int i;

    if( argc != 2 ) {
        puts( "usage: umktable <output-file>" );
        exit(1);
    }
    ofp = fopen( argv[1], "w" );
    if( !ofp ) {
        puts( "cannot open output file" );
        exit(1);
    }
    i = T_LAST_TOKEN;
    if( i > 255 ) {
        puts( "too many tokens!" );
        exit(1);
    }
    if( i > 200 ) {
        puts( "/* over 200 tokens! */" );
    }
    for( i = 0; i < 256; ++i ) {
        toYACC[i] = 0;
    }
    createTable();
    fprintf( ofp, "static const unsigned char YYFAR yytranslate[257] = {\n" );
    for( i = 0; i < 256; ++i ) {
        fprintf( ofp, "%3u,", toYACC[i] );
        if( i < T_LAST_TOKEN ) {
            fprintf( ofp, "\t/* %s */", Tokens[ i ] );
        }
        fprintf( ofp, "\n" );
    }
    fprintf( ofp, "  0 };\n" );
    fclose( ofp );
    exit(0);
}
