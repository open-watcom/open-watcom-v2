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
#include "types.h"
#include "param.h"
#include "swchar.h"

struct Res2ResParams CmdLineParms;

static int ScanMultiOptArg( const char * arg )
{
    int    contok;

    contok = TRUE;

    for ( ; *arg != '\0' && contok; arg++) {
        switch (*arg) {
        case 'q':
            CmdLineParms.Quiet = TRUE;
            break;
        case 'k':
            CmdLineParms.KeepNameTable = TRUE;
            break;
        case '?':
            CmdLineParms.PrintHelp = TRUE;
            contok = FALSE;
            break;
        default:
            fprintf( stderr, "Unknow option: %c", *arg );
            contok = FALSE;
            break;
        }
    }
    return( contok );
}

static int ScanOptionsArg( const char * arg )
{
    int    contok;

    contok = TRUE;

    switch (*arg) {
    case '\0':
        fputs( "No options specified after option charater.\n", stderr );
        contok = FALSE;
        break;
    default:            /* option that could have others with it */
        contok = ScanMultiOptArg( arg ) && contok;
        break;
    }

    return( contok );
}

static void MakeOutFilename( void )
{
    strcpy( CmdLineParms.OutFileName, CmdLineParms.InFileName );
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
    CmdLineParms.PrintHelp = FALSE;
    CmdLineParms.Quiet = FALSE;
    CmdLineParms.KeepNameTable = FALSE;
    CmdLineParms.InFileName[0] = '\0';
    CmdLineParms.OutFileName[0] = '\0';
}

int ScanParams( int argc, const char * argv[] )
/**********************************************/
{
    int     switchchar;
    int     nofilenames;    /* number of filename parms read so far */
    int    contok;          /* continue with main execution */
    int     currarg;

    nofilenames = 0;
    contok = TRUE;
    switchchar = _dos_switch_char();
    DefaultParms();

    for (currarg = 1; currarg < argc && contok; currarg++) {
        if (*argv[ currarg ] == switchchar || *argv[ currarg ] == '-') {
            contok = ScanOptionsArg( argv[ currarg ] + 1 ) && contok;
        } else if (*argv[ currarg ] == '?') {
            CmdLineParms.PrintHelp = TRUE;
            contok = FALSE;
        } else if (nofilenames == 0) {
            strncpy( CmdLineParms.InFileName, argv[ currarg ], _MAX_PATH );
            nofilenames++;
        } else if (nofilenames == 1) {
            strncpy( CmdLineParms.OutFileName, argv[ currarg ], _MAX_PATH );
            nofilenames++;
        } else {
            fprintf( stderr, "Too many arguments %s.\n", argv[ currarg ] );
            contok = FALSE;
        }
    }

    if (contok) {
        switch (nofilenames) {
        case 0:
            fputs( "Filename required.\n", stderr );
            contok = FALSE;
            break;
        case 1:
            CheckExtention( CmdLineParms.InFileName );
            MakeOutFilename();
            break;
        default:
            CheckExtention( CmdLineParms.InFileName );
            CheckExtention( CmdLineParms.OutFileName );
            break;
        }
    }

    return( contok );
}
