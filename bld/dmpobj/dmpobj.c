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


#include <ctype.h>
#include <stdlib.h>

#include "dmpobj.h"

bool Descriptions;
bool InterpretComent;

void leave( int rc )
{
    OutputSetFH( stdout );
    OutputFini();
    exit( rc );
}

static void usage( void )
{
    Output( "Usage: dmpobj [options] objfile[" OBJSUFFIX "]..." CRLF );
    Output( "Options:" CRLF );
    Output( "/l\tProduce listing file" CRLF );
    Output( "/d\tPrint descriptive titles for some output" CRLF );
    Output( "/c\tDump COMENT records without interpretation" CRLF );
    Output( "/i\tOriginal Intel OMF-86 format" CRLF );
    Output( "/r\tProvide raw dump of records as well" CRLF );
    leave( 1 );
}

void main( int argc, char **argv )
/********************************/
{
    FILE        *fp;
    char        drive[ _MAX_DRIVE ];
    char        dir[ _MAX_DIR ];
    char        fname[ _MAX_FNAME ];
    char        ext[ _MAX_EXT ];
    char        file[ _MAX_PATH ];
    char        *fn;
    int         i;
    bool        list_file;
    FILE        *fh;
    bool        is_intel;

    OutputInit();
    OutputSetFH( stdout );

    Descriptions = FALSE;
    InterpretComent = TRUE;
    list_file = FALSE;
    is_intel = FALSE;
    for( i = 1; i < argc; ++i ) {
        if( argv[i][0] == '/' ) {
            switch( tolower( argv[i][1] ) ) {
            case 'l':
                list_file = TRUE;
                break;
            case 'd':
                Descriptions = TRUE;
                break;
            case 'c':
                InterpretComent = FALSE;
                break;
            case 'r':
                DumpRaw = TRUE;
                break;
            case 'i':
                is_intel = FALSE;
                break;
            default:
                usage();
            }
        } else {
            break;
        }
    }
    if( i == argc ) {
        usage();
    }

    for( ; i < argc; ++i ) {
        _splitpath( argv[i], drive, dir, fname, ext );
        if( ext[0] == 0 ) {
            _makepath( file, drive, dir, fname, OBJSUFFIX );
            fn = file;
        } else {
            fn = argv[i];
        }
        fp = fopen( fn, "rb" );
        if( fp == NULL ) {
            Output( "Cannot open '%s' for reading" CRLF, fn );
            leave( 20 );
        }
        if( list_file ) {
            _makepath( file, drive, dir, fname, LSTSUFFIX );
            fh = fopen( file, "w" );
            if( fh == NULL ) {
                Output( "Cannot open '%s' for writing" CRLF, file );
                leave( 20 );
            }
            OutputSetFH( fh );
        }
        ProcFile( fp, is_intel );
        fclose( fp );
        OutputSetFH( stdout );  /* does fclose() if necessary */
    }
    leave( 0 );
}
