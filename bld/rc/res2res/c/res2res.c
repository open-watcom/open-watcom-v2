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
* Description:  Watcom resource converter.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bool.h"
#include "param.h"
#include "convert.h"
#include "trmemcvr.h"

static void PrintUsageMesage( const char * progpath )
{
    char    progname[ _MAX_FNAME ];

    _splitpath( progpath, NULL, NULL, progname, NULL );
    strlwr( progname );

    printf( "Usage: %s {options} file1 [file2]\n", progname );
    puts( "Converts a Microsoft .RES file to a Watcom .RES "
                "file or vice versa." );
#ifdef __UNIX__
    puts( "valid options:" );
#else
    puts( "valid options:   (/option also accepted)" );
#endif
    puts( "-q  - operate quietly" );
    puts( "-k  - keep name tables (default is not to)" );
    puts( "If  file2  is not given  file1  will be replaced." );
}

int main( int argc, const char * argv[] )
/***************************************/
{
    int     error;          /* 1 if an error has occured, 0 otherwise */
    bool    contok;         /* true if OK to continue after scanning arg */

    TRMemOpen();

    contok = ScanParams( argc, argv );
    if( CmdLineParms.PrintHelp ) {
        PrintUsageMesage( argv[0] );
    }

    if( contok ) {
        error = ConvertFiles();
    } else {
        error = 1;
    }

    TRMemClose();

    return( error );
}
