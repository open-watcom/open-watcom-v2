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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include "wresall.h"
#include "types.h"
#include "global.h"
#include "param.h"
#include "cmpfiles.h"
#include "trmemcvr.h"
#include "pathgrp2.h"

#include "clibext.h"


static const char * const HelpText[] = {
    "Usages: %s {options} filename1 filename2",
    "Options: (-option is also accepted)",
    "/?   - print this help message",
    "/q   - operated quietly",
    "/c   - continue checking resources after a mismatch",
    "/n   - don't print \"Number of types (resources) differ.\" message",
    NULL
};

static void PrintUsageMessage( const char * progpath )
/****************************************************/
{
    int         linenum;
    pgroup2     pg;

    _splitpath2( progpath, pg.buffer, NULL, NULL, &pg.fname, NULL );
    strlwr( pg.fname );

    fprintf( stdout, HelpText[ 0 ], pg.fname );
    fputc( '\n', stdout );

    for (linenum = 1; HelpText[ linenum ] != NULL; linenum++) {
        fputs( HelpText[ linenum ], stdout );
        fputc( '\n', stdout );
    }
}

int main( int argc, char * argv[] )
/*********************************/
{
    bool    oktocont;
    int     retcode;

    TRMemOpen();

    oktocont = ScanParams( argc, argv );

    if (CmdLineParms.PrintHelp) {
        PrintUsageMessage( argv[0] );
    }

    if (oktocont) {
        retcode = CompareFiles();
    } else {
        retcode = -1;
    }

    TRMemClose();

    return( retcode );
}
