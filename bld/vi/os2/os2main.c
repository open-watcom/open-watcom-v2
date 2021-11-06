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
* Description:  Mainline for OS/2 version of vi.
*
****************************************************************************/


#include "vi.h"
#include "stack.h"
#include "pathgrp2.h"


/*
 * getEXEName - look up full path to exe
 */
static void getEXEName( char *name )
{
    pgroup2     pg;
    char        path[_MAX_PATH];
    char        tmppath[_MAX_PATH];

    _splitpath2( name, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    if( pg.ext[0] != '\0' ) {
        EXEName = name;
    } else {
        _makepath( tmppath, pg.drive, pg.dir, pg.fname, "exe" );
        _searchenv( tmppath, "PATH", path );
        EXEName = DupString( path );
    }

} /* getEXEName */

void main( int argc, char *argv[] )
{
    InitMem();

    argc = argc;
    getEXEName( argv[0] );
#if defined(_M_I86)
    GlobVarAddStr( GLOBVAR_OS, "os2" );
#else
    GlobVarAddStr( GLOBVAR_OS, "os2v2" );
#endif
    InitialStack();
    Comspec = getenv( "COMSPEC" );
    InitializeEditor();
    FinalStack();
    EditMain();

    FiniMem();

} /* main */
