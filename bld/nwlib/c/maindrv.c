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
* Description:  Librarian DLL version mainline.
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#ifdef __WATCOMC__
#include <process.h>
#else
#include "clibext.h"
#endif
#ifdef IDE_PGM
#include <stdio.h>
#include <limits.h>
#include "main.h"
#endif
#include "idedrv.h"

#define AR_MODE_ENV "WLIB$AR"

#ifndef DLL_NAME
  #error DLL_NAME must be given with -d switch when DLL Driver
#else
  #define quoted( name ) # name
  #define _str(x) quoted(x)
  #define DLL_NAME_STR _str(DLL_NAME)
#endif

static IDEDRV info =
{   DLL_NAME_STR
};

int main                        // MAIN-LINE FOR DLL DRIVER
    ( int argc                  // - # args
    , char *argv[] )            // - arguments
{
    int retcode;                // - return code
#ifndef __UNIX__
    int len;
    char *cmd_line;
    char *p;
#endif

#ifndef __WATCOMC__
    _argv = argv;
    _argc = argc;
#endif
#ifndef __UNIX__
    len = _bgetcmd( NULL, 0 ) + 1;
    cmd_line = malloc( len );
    _bgetcmd( cmd_line, len );
    /* Turn on 'ar' mode by setting WLIB$AR env var */
    p = strrchr( argv[ 0 ], '\\' );
    if( p == NULL ) {
        p = argv[ 0 ];
    } else {
        ++p;
    }
    if( stricmp( p, "ar.exe" ) == 0 ) {
        putenv( AR_MODE_ENV "=ON" );
    }
    retcode = IdeDrvExecDLL( &info, cmd_line );
    free( cmd_line );
#else
    retcode = IdeDrvExecDLLArgv( &info, argc, argv );
#endif
    if( retcode != IDEDRV_ERR_INIT_EXEC ) {
        IdeDrvUnloadDLL( &info );               // UNLOAD THE DLL
    }
    return( retcode );
}
