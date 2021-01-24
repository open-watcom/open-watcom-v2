/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
#include "wnoret.h"
#if defined( __UNIX__ ) && defined( __WATCOMC__ )
  #if ( __WATCOMC__ < 1300 )
    // fix for OW 1.9
    #include <limits.h>
  #endif
#endif
#include "pathgrp2.h"

#include "clibext.h"


#if defined( __UNIX__ )
    #define OBJSUFFIX   "o"
#else
    #define OBJSUFFIX   "obj"
#endif
#define LSTSUFFIX       "lst"

bool Descriptions;
bool InterpretComent;
bool quiet;
bool TranslateIndex;

void leave( int rc )
{
    OutputSetFH( stdout );
    OutputFini();
    exit( rc );
    // never return
}

static void ShowProductInfo( void )
{
    if( !quiet ) {
        Output( banner1w( "OMF Dump Utility", BAN_VER_STR ) CRLF );
        Output( banner2 CRLF );
        Output( banner2a( 1984 ) CRLF );
        Output( banner3 CRLF );
        Output( banner3a CRLF );
    }
}

static void usage( void )
{
    ShowProductInfo();

    Output( "Usage: dmpobj [options] objfile[." OBJSUFFIX "]..." CRLF );
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
}

int main( int argc, char **argv )
/*******************************/
{
    FILE        *fp;
    pgroup2     pg;
    char        file[_MAX_PATH];
    char        *fn;
    int         i;
    bool        list_file;
    FILE        *fh;
    bool        is_intel;

    OutputInit();
    OutputSetFH( stdout );

    Descriptions = false;
    InterpretComent = true;
    TranslateIndex = false;
    list_file = false;
    is_intel = false;
    quiet = false;
    for( i = 1; i < argc; ++i ) {
        if( argv[i][0] == '-' ) {
            switch( tolower( argv[i][1] ) ) {
            case 'l':
                list_file = true;
                break;
            case 'd':
                Descriptions = true;
                break;
            case 'c':
                InterpretComent = false;
                break;
            case 'r':
                if( strnicmp( argv[i] + 1, "rec=", 4 ) == 0 ) {
                    if( rec_count < 10 ) {
                        if( isdigit( argv[i][5] ) ) {
                            rec_type[rec_count++] = atoi( argv[i] + 5 );
                        } else {
                            rec_type[rec_count++] = RecNameToNumber( argv[i] + 5 );
                        }
                    } else {
                        Output( "Maximum 10 record type allowed." CRLF );
                        OutputFini();
                    }
                } else {
                    DumpRaw = true;
                }
                break;
            case 'i':
                is_intel = false;
                break;
            case 'q':
                quiet = true;
                break;
            case 't':
                TranslateIndex = true;
                break;
            default:
                usage();
                leave( 1 );
                // never return
            }
        } else {
            break;
        }
    }
    if( i == argc ) {
        usage();
        leave( 1 );
        // never return
    }

    ShowProductInfo();
    for( ; i < argc; ++i ) {
        _splitpath2( argv[i], pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
        if( pg.ext[0] == 0 ) {
            _makepath( file, pg.drive, pg.dir, pg.fname, OBJSUFFIX );
            fn = file;
        } else {
            fn = argv[i];
        }
        fp = fopen( fn, "rb" );
        if( fp == NULL ) {
            Output( "Cannot open '%s' for reading" CRLF, fn );
            leave( 20 );
            // never return
        }
        if( list_file ) {
            _makepath( file, pg.drive, pg.dir, pg.fname, LSTSUFFIX );
            fh = fopen( file, "w" );
            if( fh == NULL ) {
                Output( "Cannot open '%s' for writing" CRLF, file );
                leave( 20 );
                // never return
            }
            OutputSetFH( fh );
        }
        ProcFile( fp, is_intel );
        fclose( fp );
        OutputSetFH( stdout );  /* does fclose() if necessary */
    }
    leave( 0 );
    // never return
    // next is for compilers not supporting "no return" function modifier
    NO_RETURN_FAKE( return( 0 ) );
}
