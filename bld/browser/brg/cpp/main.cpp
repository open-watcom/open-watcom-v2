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


#include <stdlib.h>
#include <stdio.h>
#include <direct.h>
#include <wclist.h>
#include <process.h>    // getcmd
#include <string.hpp>

#include "banner.h"
#include "cmdparse.h"
#include "death.h"
#include "dwmerger.h"
#include "elffile.h"
#include "mrfile.h"
#include "util.h"

const char * ExeName = "wbrg";
const char * ModuleExt = ".mbr";
const char * MergedExt = ".dbr";

#define ERROR_RETURN_VAL 7

static void printHeader();
static void printUsage();

#define COMMANDBUF (512)    /* size of buffer to hold command line */

int main()
//--------
{
    char cmdLine[ COMMANDBUF ];

    printHeader();

    getcmd( cmdLine );

    try {
        CommandParser prs( cmdLine, FALSE );

        if( prs.database() == NULL || prs.files()->entries() == 0 ) {
            printUsage();
        } else {
            WCValSList<String> disabled;
            DwarfFileMerger merger( prs.database(), *prs.files(), disabled );
            if( merger.upToDate() ) {
                printf( "Database %s already up-to-date\n", prs.database() );
            } else {
                merger.doMerge( prs.quiet() );
            }
        }
    } catch ( ... ) {
        fputs( "Merging aborted\n\n", stderr );
        return -1;
    }

    return 0;
}

static void printHeader()
//-----------------------
{
    puts( banner1w( "Browsing Information Merger ", _WBRG_VERSION_ ) );
    puts( banner2( "1994" ) );
    puts( banner3 );
    puts( "" );
}

static void printUsage()
//----------------------
{
    puts( "" );
    puts( "Usage:    wbrg <merger_cmd> ... <merger_cmd>" );
    puts( "" );
    puts( "          <merger_cmd> ::= database <dbr_file>" );
    puts( "                         | file <mbr_file>, <mbr_file>, ..." );
    puts( "                         | file { <mbr_file> ... <mbr_file> }" );
    puts( "                         | @ <cbr_file>" );
    puts( "          <dbr_file> is the file name of the browser database file" );
    puts( "          <mbr_file> is the file name of a browser module file" );
    puts( "          <cbr_file> is the file name of a browser command file" );
}
