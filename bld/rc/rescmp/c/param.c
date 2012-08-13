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


#include <stdio.h>
#include <string.h>
#include "global.h"
#include "param.h"
#include "types.h"
#include "swchar.h"

static bool ScanMultiOptArg( const char * arg )
{
    bool    contok;

    contok = true;

    for ( ; *arg != '\0' && contok; arg++) {
        switch (*arg) {
        case 'n':
            CmdLineParms.NoCounts = true;
            break;
        case 'c':
            CmdLineParms.CheckAll = true;
            break;
        case 'q':
            CmdLineParms.Quiet = true;
            break;
        case '?':
            CmdLineParms.PrintHelp = true;
            contok = false;
            break;
        default:
            printf( "Error: unknown option %c\n", *arg );
            contok = false;
            break;
        }
    }
    return( contok );
}

static bool ScanOptionsArg( const char * arg )
{
    bool    contok;

    contok = true;

    switch (*arg) {
    case '\0':
        puts( "Error: no option specifed after option character" );
        contok = false;
        break;
    default:            /* option that could have others with it */
        contok = ScanMultiOptArg( arg ) && contok;
        break;
    }

    return( contok );
}

static void CheckExtention( char * path )
{
    char    name[ _MAX_FNAME ];
    char    drive[ _MAX_DRIVE ];
    char    dir[ _MAX_DIR ];
    char    ext[ _MAX_EXT ];

    _splitpath( path, drive, dir, name, ext );
    if (*ext == '\0') {
        _makepath( path, drive, dir, name, "RES" );
    }
}

static void DefaultParms( void )
{
    CmdLineParms.CheckAll = 0;
    CmdLineParms.PrintHelp = 0;
    CmdLineParms.Quiet = 0;
    CmdLineParms.FileName1[0] = '\0';
    CmdLineParms.FileName2[0] = '\0';
}

bool ScanParams( int argc, const char * argv[] )
/**********************************************/
{
    int     switchchar;
    int     nofilenames;    /* number of filename parms read so far */
    bool    contok;         /* continue with main execution */
    int     currarg;

    nofilenames = 0;
    contok = true;
    switchchar = _dos_switch_char();
    DefaultParms();

    for (currarg = 1; currarg < argc && contok; currarg++) {
        if (*argv[ currarg ] == switchchar || *argv[ currarg ] == '-') {
            contok = ScanOptionsArg( argv[ currarg ] + 1 ) && contok;
        } else if (*argv[ currarg ] == '?') {
            CmdLineParms.PrintHelp = true;
            contok = false;
        } else if (nofilenames == 0) {
            strncpy( CmdLineParms.FileName1, argv[ currarg ], _MAX_PATH );
            nofilenames++;
        } else if (nofilenames == 1) {
            strncpy( CmdLineParms.FileName2, argv[ currarg ], _MAX_PATH );
            nofilenames++;
        } else {
            puts( "Error: Too many arguments on command line" );
            contok = false;
        }
    }

    if (contok) {
        switch (nofilenames) {
        case 0:
        case 1:
            puts( "Error: two filenames required." );
            contok = false;
            break;
        default:
            CheckExtention( CmdLineParms.FileName1 );
            CheckExtention( CmdLineParms.FileName2 );
            break;
        }
    }

    return( contok );
}
