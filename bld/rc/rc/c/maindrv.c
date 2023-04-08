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
* Description:  resource compiler mainline
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#ifdef __WATCOMC__
    #include <process.h>
#endif
#include "idedrv.h"

#include "clibext.h"


#ifndef DLL_NAME
    #error      DLL_NAME must be given with -d switch when DLL Driver
#endif

#define quoted(name)    # name
#define _str(x)         quoted(x)
#define DLL_NAME_STR    _str(DLL_NAME)


int main( int argc, char *argv[] ) 
/********************************/
{
    int         retcode;
    IDEDRV      info;
#ifndef __UNIX__
    int         cmdlen;
    char        *cmdline;
#endif

#if !defined( __WATCOMC__ )
    _argv = argv;
    _argc = argc;
#elif !defined( __UNIX__ )
    /* unused parameters */ (void)argc; (void)argv;
#endif

    IdeDrvInit( &info, DLL_NAME_STR, NULL );
#ifdef __UNIX__
    retcode = IdeDrvExecDLLArgv( &info, argc, argv );
#else
    cmdlen = _bgetcmd( NULL, 0 ) + 1;
    cmdline = malloc( cmdlen );
    if( cmdline != NULL )
        _bgetcmd( cmdline, cmdlen );
    retcode = IdeDrvExecDLL( &info, cmdline );
    free( cmdline );
#endif
    switch( retcode ) {
    case IDEDRV_SUCCESS:
    case IDEDRV_ERR_RUN:
    case IDEDRV_ERR_RUN_EXEC:
    case IDEDRV_ERR_RUN_FATAL:
        break;
    default:
        IdeDrvPrintError( &info );
        break;
    }
    IdeDrvUnloadDLL( &info );
    return( retcode == IDEDRV_SUCCESS ? EXIT_SUCCESS : EXIT_FAILURE );
}
