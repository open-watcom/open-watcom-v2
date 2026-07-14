/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Browsing Information Merger main module.
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <direct.h>
#include <wclist.h>
#include <process.h>
#include <string.hpp>

#include "banner.h"
#include "cmdparse.h"
#include "death.h"
#include "dwmerger.h"
#include "elffile.h"
#include "mrfile.h"
#include "util.h"


#define ERROR_RETURN_VAL 7

const char * ExeName = "wbrg";
const char * ModuleExt = ".mbr";
const char * MergedExt = ".dbr";

static void printHeader();
static void printUsage();

int main()
//--------
{
    int     cmd_len;
    char    *cmd_line;

    printHeader();

    cmd_len = _bgetcmd( NULL, 0 ) + 1;
    cmd_line = new char[cmd_len];
    _bgetcmd( cmd_line, cmd_len );

    try {
        CommandParser prs( cmd_line, false );
        delete[] cmd_line;

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
    puts(
        banner1t( "Browsing Information Merger" ) "\n"
        banner1v( _WBRG_VERSION_ ) "\n"
        banner2 "\n"
        banner2a( 1994 ) "\n"
        banner3 "\n"
        banner3a "\n"
    );
}

static void printUsage()
//----------------------
{
    puts(
        "\n"
        "Usage:    wbrg <merger_cmd> ... <merger_cmd>" "\n"
        "\n"
        "    <merger_cmd> ::= database <dbr_file>" "\n"
        "                   | file <mbr_file>, <mbr_file>, ..." "\n"
        "                   | file { <mbr_file> ... <mbr_file> }" "\n"
        "                   | @ <cbr_file>" "\n"
        "    <dbr_file> is the file name of the browser database file" "\n"
        "    <mbr_file> is the file name of a browser module file" "\n"
        "    <cbr_file> is the file name of a browser command file"
    );
}
