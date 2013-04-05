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
* Description:  Main procedure to call the linker IDE interface.
*
****************************************************************************/


#include "walloca.h"
#include "idedrv.h"
#ifdef __WATCOMC__
#include <process.h>
#else
#include "clibext.h"
#endif

#ifndef DLL_NAME
  #error DLL_NAME must be given with -d switch when DLL Driver
#else
  #define quoted( name ) # name
  #define _str(x) quoted(x)
  #define DLL_NAME_STR _str(DLL_NAME)
#endif

int main( int argc, char **argv )
{
    IDEDRV          inf;
#if !defined( __UNIX__ )
    char            *cmdline;
    int             cmdlen;
#endif
    IDEDRV_STATUS   status;

#if !defined( __UNIX__ )
    argc = argc;
    argv = argv;
#if !defined( __WATCOMC__ )
    _argv = argv;
    _argc = argc;
#endif
#endif
    status = IDEDRV_ERR_LOAD;
    IdeDrvInit( &inf, DLL_NAME_STR, NULL );
#if !defined( __UNIX__ )
    cmdline = NULL;
    cmdlen = _bgetcmd( NULL, 0 );
    if( cmdlen != 0 ) {
        cmdlen++;               // add 1 for null char
        cmdline = alloca( cmdlen );
        if( cmdline != NULL ) {
            _bgetcmd( cmdline, cmdlen );
        }
    }
    status = IdeDrvExecDLL( &inf, cmdline );
#else
    status = IdeDrvExecDLLArgv( &inf, argc, argv );
#endif
    switch( status ) {
    case IDEDRV_SUCCESS:
    case IDEDRV_ERR_RUN_EXEC:
    case IDEDRV_ERR_RUN_FATAL:
        break;
    default:
        IdeDrvPrintError( &inf );
        break;
    }
    IdeDrvUnloadDLL( &inf );
    return( status != IDEDRV_SUCCESS );
}
