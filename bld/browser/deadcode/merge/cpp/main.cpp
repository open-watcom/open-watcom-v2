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



// System includes --------------------------------------------------------

#include <stdio.h>
#include <wvlist.hpp>
#include <wstring.hpp>
#include <wfilenam.hpp>

// Project includes -------------------------------------------------------

#include "merge.h"
#include "errors.h"

const char * Version = "0.1a " __DATE__ " " __TIME__;
const char * DefaultOutput = "browse.cbr";
const char * ExeName = "merge";
const char * ModuleExt = ".mbr";
const char * MergedExt = ".cbr";

#define UNKNOWN_OPT_RET 5

void printHeader();

int main( int argc, char ** argv )
/********************************/
{
    int         i;
    WVList      list;
    ElfFile     outFile( DefaultOutput );
    char *      currOpt;

    printHeader();

    if( argc == 1 ) {
        printf( "Usage: %s [options] files [options]\n", ExeName );
        printf( "      merges one or more browser modules (%s files) into a browser database\n", ModuleExt );
        printf( "      (%s file).\n", MergedExt, DefaultOutput );
        printf( "Options:\n" );
        printf( "       ( /option is also accepted )\n" );
        printf( "        -o=<output file>   specify the merged filename -- default %s\n", DefaultOutput );
        printf( "\n" );
    } else {
        for( i = 1; i < argc; i += 1 ) {
            currOpt = argv[ i ];
            if( currOpt[ 0 ] == '/' || currOpt[ 0 ] == '-' ) {
                if( currOpt[ 1 ] != 'o' ||
                  ( currOpt[2] != '=' && currOpt[2] != '#' ) ) {

                    printf( "unknown option '%s'\n", currOpt );
                    return UNKNOWN_OPT_RET;

                } else {
                    WFileName fn( currOpt + 3 );
                    fn.absoluteTo();
                    outFile.setFileName( fn.gets() );
                }
            } else {
                outFile.addComponentFile( argv[ i ] );
            }
        }

        try {
            DwarfFileMerger merger( &outFile );
            merger.doMerge();
        }
        catch( Errors err ) {
            switch( err ) {
            case ErrOutOfMemory:
                printf( "Error:  Out of Memory\n" );
            case ErrInvalidFormat:
                printf( "Error:  Invalid file format1\n" );  // ITB should be caught earlier
            default:
                printf( "Unknown error1.\n" );
            }
        }
        catch ( ... ) {
            printf( "Unknown error2.\n" );
        }
    }

    return 0;
}

void printHeader()
/****************/
{
    printf( "WATCOM Browsing Information Merger Version %s\n", Version );

    #if DEBUG
    printf( "For INTERNAL Use Only\n" );
    #endif

    printf( "Copyright by WATCOM International Corp. 1993, All rights reserved. \n" );
    printf( "WATCOM is a trademark of WATCOM International Corp. \n" );
}
