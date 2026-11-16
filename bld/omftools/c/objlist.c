/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  List module names defined in an object file.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "bool.h"
#include "watcom.h"
#include "pcobj.h"
#include "misc.h"


static void usage( void )
/***********************/
{
    printf( "Usage: objlist <list of object or library files>\n" );
}

static bool ProcFile( FILE *fp )
/******************************/
{
    unsigned_16 page_len;
    unsigned_32 offset;
    bool        ok;
    int         rc;
    bool        first;

    page_len = 0;
    ReadRecInit();
    first = true;
    ok = true;
    while( ok ) {
        offset = ftell( fp );
        rc = ReadRec( fp );
        if( rc <= 0 ) {
            if( rc == 0 )
                ok = false;
            break;
        }
        switch( GET_RECCMD() ) {
        case CMD_THEADR:
            if( first ) {
                char    name[256];

                GetNameBuf( name );
                printf( "%s\n", NamePtr );
                first = false;
            }
            break;
        case CMD_MODEND:
        case CMD_MODEND32:
            if( page_len != 0 ) {
                AlignFile( fp, page_len );
            }
            first = true;
            break;
        case LIB_HEADER_REC:
            page_len = GET_RECLEN() - 1 + 4;
            break;
        case LIB_TRAILER_REC:
            fseek( fp, 0L, SEEK_END );
            page_len = 0;
            break;
        default:
            break;
        }
    }
    ReadRecFini();
    return( ok );
}

static bool process_file_mod( const char *filename )
/**************************************************/
{
    FILE    *fp;
    bool    ok;

    fp = fopen( filename, "rb" );
    if( fp == NULL ) {
        printf( "Cannot open input file: %s.\n", filename );
        return( false );
    }
    ok = ProcFile( fp );
    fclose( fp );
    return( ok );
}

int main( int argc, char *argv[] )
/********************************/
{
    int     i;
    char    *fn;
    bool    ok;

    if( argc == 1 ) {
        usage();
        return( 1 );
    }
    ok = true;
    for( i = 1; i < argc; ++i ) {
        fn = DoWildCard( argv[i] );
        while( fn != NULL ) {
            ok &= process_file_mod( fn );
            fn = DoWildCard( NULL );
        }
        DoWildCardClose();
    }
    return( ok ? EXIT_SUCCESS : EXIT_FAILURE );
}
