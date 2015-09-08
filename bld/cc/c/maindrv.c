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


#ifdef __WATCOMC__
#include <process.h>
#endif
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include "idedrv.h"
#include "walloca.h"
#include "watcom.h"

#include "clibext.h"


#ifndef DLL_NAME
  #error DLL_NAME must be given with -d switch when DLL Driver
#else
  #define quoted( name ) # name
  #define _str(x) quoted(x)
  #define DLL_NAME_STR _str(DLL_NAME)
#endif

int main( int argc, char* argv[] ) {
/**********************************/
    IDEDRV  info;
#ifndef __UNIX__
    char    *cmdline;
    int     cmdlen;
#endif
    int     retcode;                 // - return code

#ifndef __WATCOMC__
    _argc = argc;
    _argv = argv;
#endif
    IdeDrvInit( &info, DLL_NAME_STR, NULL );
    retcode = IDEDRV_ERR_RUN_FATAL;

#ifndef __UNIX__
    argc = argc;
    argv = argv;
    cmdline = NULL;
    cmdlen = _bgetcmd( NULL, 0 );
    if( cmdlen != 0 ) {
        cmdlen++;               // add 1 for null char
        cmdline = alloca( cmdlen );
        if( cmdline != NULL ) {
            _bgetcmd( cmdline, cmdlen );
        }
    }
    retcode = IdeDrvExecDLL( &info, cmdline );
#else
    retcode = IdeDrvExecDLLArgv( &info, argc, argv );
#endif
    switch( retcode ) {
    case IDEDRV_SUCCESS :
    case IDEDRV_ERR_RUN :
    case IDEDRV_ERR_RUN_EXEC :
    case IDEDRV_ERR_RUN_FATAL :
        break;
    default:
        retcode = IdeDrvPrintError( &info );
        break;
    }
//    IdeDrvUnloadDLL( &info );
    return( retcode != IDEDRV_SUCCESS );
}
