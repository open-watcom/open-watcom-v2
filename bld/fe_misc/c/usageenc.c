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
#include <string.h>
#include "lsspec.h"
#include "encodlng.h"

struct opts{
    FILE  *in;
    FILE  *out;
}Opt;

void fatal( char *m ) {
    printf( "fatal: %s\n", m );
    exit( EXIT_FAILURE );
}


static void CopyFile(  FILE *ifp, FILE *ofp)
{
//put in usage header
//remove '\n' and replace with nullchar
//end with nullchar
    auto LocaleUsage usage_header;
    int c;

    memset( &usage_header, 0, sizeof( usage_header ) );
    usage_header.header.code = LS_Usage;
    usage_header.header.signature = LS_Usage_SIG;
    fwrite( &usage_header, sizeof( usage_header ), 1, ofp );
    for(;;) {
        c = fgetc( ifp );
        if( c == EOF ) break;
        if( c == '\n' ){
            fputc( '\0', ofp );
        }else{
            fputc( c, ofp );
        }
    }
    fputc( '\0', ofp );
}

static FILE *InitFile( char *n, char *m ) {
    FILE *f;
    f = fopen( n, m );
    if( f == NULL ) {
        fatal( "cannot open file" );
    }
    return( f );
}
static void ProcessOptions( char **argv ) {
    Opt.in = InitFile( *argv, "r" );
    ++argv;
    Opt.out = InitFile( *argv, "wb" );
    ++argv;
    if( *argv ) {
        fatal( "invalid argument" );
    }
}

int main( int argc, char **argv )
{

    if( argc != 3 ) {
        fatal( "usage: nulline <infile> <outfile>" );
    }
    ProcessOptions( &argv[1] );
    CopyFile( Opt.in, Opt.out);
    fclose( Opt.in );
    fclose( Opt.out );
    return( EXIT_SUCCESS );
}
