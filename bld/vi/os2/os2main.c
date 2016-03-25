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
* Description:  Mainline for OS/2 version of vi.
*
****************************************************************************/


#include "vi.h"
#ifndef __OS2V2__
    #include <malloc.h>
#endif
#include "stack.h"

/*
 * getEXEName - look up full path to exe
 */
static void getEXEName( char *name )
{
    char path[_MAX_PATH], drive[_MAX_DRIVE], dir[_MAX_DIR];
    char fname[_MAX_FNAME], ext[_MAX_EXT];
    char tmppath[_MAX_PATH];

    _splitpath( name, drive, dir, fname, ext );
    if( ext[0] != '\0' ) {
        EXEName = name;
    } else {
        _makepath( tmppath, drive, dir, fname, ".exe" );
        _searchenv( tmppath, "PATH", path );
        EXEName = DupString( path );
    }

} /* getEXEName */

void main( int argc, char *argv[] )
{
    InitMem();

    argc = argc;
    getEXEName( argv[0] );
#ifdef __OS2V2__
    VarAddGlobalStr( "OS", "os2v2" );
#else
    VarAddGlobalStr( "OS", "os2" );
#endif
    InitialStack();
    Comspec = getenv( "COMSPEC" );
    InitializeEditor();
    FinalStack();
    EditMain();

    FiniMem();

} /* main */
