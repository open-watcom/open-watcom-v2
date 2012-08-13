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
* Description:  Resource dump utility.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trmemcvr.h"
#include "dmpfile.h"
#include "param.h"
#include "global.h"

static char * HelpText[] = {
    "Usage: %s {options} filename",
#ifndef __UNIX__
    "Options: (/option is also accepted)",
#endif
    "-?         - print this help message",
    "-c         - dump contents of resources",
    "-n=name    - print only resources with this name",
    "-t=type    - print only resources with this type",
    NULL
};

static void PrintUsageMessage( const char * progpath )
/****************************************************/
{
    int     linenum;
    char    progfname[ _MAX_FNAME ];

    _splitpath( progpath, NULL, NULL, progfname, NULL );
    strlwr( progfname );

    fprintf( stdout, HelpText[ 0 ], progfname );
    fputc( '\n', stdout );

    for (linenum = 1; HelpText[ linenum ] != NULL; linenum++) {
        fputs( HelpText[ linenum ], stdout );
        fputc( '\n', stdout );
    }
}


int main( int argc, const char * argv[] )
/*********************************/
{
    int     contok;
    int     retcode;

    TRMemOpen();

    contok = ScanParams( argc, argv );

    if (CmdLineParms.PrintHelp) {
        PrintUsageMessage( argv[0] );
    }

    if (contok) {
        retcode = DumpFile();
    } else {
        retcode = 1;
    }

    FreeParams();

    switch (retcode) {
    case 1:         /* reading parameters */
        break;
    case 2:         /* reading input file */
        puts( "Error reading input file" );
        break;
    }

    TRMemClose();

    return( retcode );
}
