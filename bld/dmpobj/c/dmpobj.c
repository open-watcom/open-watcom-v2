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
* Description:  OMF file dump utility.
*
****************************************************************************/


#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "banner.h"

#include "dmpobj.h"

bool Descriptions;
bool InterpretComent;
bool quiet;
bool TranslateIndex;

void leave( int rc )
{
    OutputSetFH( stdout );
    OutputFini();
    exit( rc );
}

static void ShowProductInfo( void )
{
    if( quiet == FALSE ) {
        Output( banner1w( "OMF Dump Utility", BAN_VER_STR ) CRLF );
        Output( banner2a() CRLF );
        Output( banner3 CRLF );
        Output( banner3a CRLF );
    }
}

static void usage( void )
{
    ShowProductInfo();

    Output( "Usage: dmpobj [options] objfile[" OBJSUFFIX "]..." CRLF );
    Output( "Options:" CRLF );
    Output( "-l\t\tProduce listing file" CRLF );
    Output( "-d\t\tPrint descriptive titles for some output" CRLF );
    Output( "-t\t\tPrint names for some index values and list at end" CRLF );
    Output( "-c\t\tDump COMENT records without interpretation" CRLF );
    Output( "-i\t\tOriginal Intel OMF-86 format" CRLF );
    Output( "-q\t\tQuiet, don't show product info" CRLF );
    Output( "-r\t\tProvide raw dump of records as well" CRLF );
    Output( "-rec=xxx\tProvide dump of selected record type" CRLF );
    Output( "\t\t  (by number or by symbolic name)" CRLF );
    leave( 1 );
}

int main( int argc, char **argv )
/*******************************/
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
    TranslateIndex = FALSE;
    list_file = FALSE;
    is_intel = FALSE;
    quiet = FALSE;
    for( i = 1; i < argc; ++i ) {
        if( argv[i][0] == '-' ) {
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
                if( strnicmp( argv[i] + 1, "rec=", 4 ) == 0 ) {
                    if( rec_count < 10 ) {
                        if( isdigit( argv[i][5] ) ) {
                            rec_type[ rec_count++ ] = atoi( argv[i] + 5 );
                        } else {
                            rec_type[ rec_count++ ] = RecNameToNumber( argv[i] + 5 );
                        }
                    } else {
                        Output( "Maximum 10 record type allowed." CRLF );
                        OutputFini();
                    }
                } else {
                    DumpRaw = TRUE;
                }
                break;
            case 'i':
                is_intel = FALSE;
                break;
            case 'q':
                quiet = TRUE;
                break;
            case 't':
                TranslateIndex = TRUE;
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

    ShowProductInfo();
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
    return( 0 );  // for the less intelligent compilers
}
