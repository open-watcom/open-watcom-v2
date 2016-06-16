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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include "trmemcvr.h"
#include "wresall.h"
#include "global.h"
#include "param.h"
#include "types.h"
#include "swchar.h"
#include "wresdefn.h"

static int CheckPredefType( const char * restype )
{
    if (!stricmp( restype, "CURSOR" )) {
        return( RESOURCE2INT( RT_CURSOR ) );
    } else if (!stricmp( restype, "BITMAP" )) {
        return( RESOURCE2INT( RT_BITMAP ) );
    } else if (!stricmp( restype, "ICON" )) {
        return( RESOURCE2INT( RT_ICON ) );
    } else if (!stricmp( restype, "MENU" )) {
        return( RESOURCE2INT( RT_MENU ) );
    } else if (!stricmp( restype, "DIALOG" )) {
        return( RESOURCE2INT( RT_DIALOG ) );
    } else if (!stricmp( restype, "STRING" )) {
        return( RESOURCE2INT( RT_STRING ) );
    } else if (!stricmp( restype, "FONTDIR" )) {
        return( RESOURCE2INT( RT_FONTDIR ) );
    } else if (!stricmp( restype, "FONT" )) {
        return( RESOURCE2INT( RT_FONT ) );
    } else if (!stricmp( restype, "ACCELERATOR" )) {
        return( RESOURCE2INT( RT_ACCELERATOR ) );
    } else if (!stricmp( restype, "RCDATA" )) {
        return( RESOURCE2INT( RT_RCDATA ) );
    } else if (!stricmp( restype, "ERRTABLE" )) {
        return( RESOURCE2INT( RT_ERRTABLE ) );
    } else if (!stricmp( restype, "GROUP_CURSOR" )) {
        return( RESOURCE2INT( RT_GROUP_CURSOR ) );
    } else if (!stricmp( restype, "GROUP_ICON" )) {
        return( RESOURCE2INT( RT_GROUP_ICON ) );
    } else if (!stricmp( restype, "NAMETABLE" )) {
        return( RESOURCE2INT( RT_NAMETABLE ) );
    } else {
        return( 0 );
    }
}

static bool ScanMultiOptArg( const char * arg )
{
    bool    contok;

    contok = true;

    for ( ; *arg != '\0' && contok; arg++) {
        switch (*arg) {
        case 'c':
            CmdLineParms.DumpContents = true;
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
    int     predeftype;

    contok = true;

    switch (*arg) {
    case '\0':
        puts( "Error: no option specifed after option character" );
        contok = false;
        break;
    case 'n':
        arg++;
        if (*arg != '=') {
            puts( "Missing = after option n" );
            contok = false;
        } else {
            arg++;
            if (isdigit( *arg )) {
                CmdLineParms.Name = WResIDFromNum( atoi( arg ) );
            } else {
                CmdLineParms.Name = WResIDFromStr( arg );
            }
        }
        break;
    case 't':
        arg++;
        if (*arg != '=') {
            puts( "Missing = after option t" );
            contok = false;
        } else {
            arg++;
            if (isdigit( *arg )) {
                CmdLineParms.Type = WResIDFromNum( atoi( arg ) );
            } else {
                predeftype = CheckPredefType( arg );
                if (predeftype == 0) {
                    CmdLineParms.Type = WResIDFromStr( arg );
                } else {
                    CmdLineParms.Type = WResIDFromNum( predeftype );
                }
            }
        }
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
    CmdLineParms.PrintHelp = 0;
    CmdLineParms.DumpContents = false;
    CmdLineParms.Name = NULL;
    CmdLineParms.Type = NULL;
    CmdLineParms.FileName[0] = '\0';
}

void FreeParams( void )
/*********************/
{
    if (CmdLineParms.Name != NULL) {
        TRMemFree( CmdLineParms.Name );
    }
    if (CmdLineParms.Type != NULL) {
        TRMemFree( CmdLineParms.Type );
    }
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
            strncpy( CmdLineParms.FileName, argv[ currarg ], _MAX_PATH );
            nofilenames++;
        } else {
            puts( "Error: Too many arguments on command line" );
            contok = false;
        }
    }

    if (contok) {
        switch (nofilenames) {
        case 0:
            puts( "Error: filename required." );
            contok = false;
            break;
        default:
            CheckExtention( CmdLineParms.FileName );
            break;
        }
    }

    return( contok );
}
