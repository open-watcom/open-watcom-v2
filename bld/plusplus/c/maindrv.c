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


#include "plusplus.h"

#include <limits.h>
#ifdef __WATCOMC__
#include <process.h>
#endif

#include "memmgr.h"
#include "idedrv.h"

#ifdef wpp_drv
#   ifndef DLL_NAME
#      error DLL_NAME must be given with -d switch when DLL Driver
#      define NO_MO_COMPILING
#   else
#      define quoted( name ) # name
#      define _str(x) quoted(x)
#      define DLL_NAME_STR _str(DLL_NAME)
#   endif
#else
#   define DLL_NAME_STR "WPP"
#endif

#ifndef NO_MO_COMPILING

#if defined(__DOS__) || defined(__OS2__) || defined(__NT__)
#if ! defined(wpp_drv)
#define RAW_CMDLINE
#endif
#endif

#if defined(__UNIX__)
#define USE_ARGV
#endif

static IDEDRV info =
{   DLL_NAME_STR
};

#ifndef USE_ARGV
static char cmd_line[ 1024*8 ]; // - a buffer
#endif

int main(                       // MAIN-LINE FOR DLL DRIVER
    #if !defined(RAW_CMDLINE)
    int argc,                   // - arg count
    char **argv                 // - arg.s
    #endif
    )
{
    int retcode;                // - return code

    #if defined(USE_ARGV)
        retcode = IdeDrvExecDLLArgv( &info, argc, argv );
    #elif defined(wpp_drv)
        argc = argc;
        argv = argv;
        getcmd( cmd_line );
        retcode = IdeDrvExecDLL( &info, cmd_line );
    #elif defined(RAW_CMDLINE)
        size_t  len;
        char    *lcl_argv[2];

        len = _bgetcmd( NULL, INT_MAX );
        lcl_argv[1] = NULL;
        if( len >= sizeof( cmd_line ) ) {
            lcl_argv[0] = CMemAlloc( len + 1 );
        } else {
            lcl_argv[0] = cmd_line;
        }
        _bgetcmd( lcl_argv[0], len + 1 );
        retcode = IdeDrvExecDLL( &info, cmd_line );
        if( len >= sizeof( cmd_line ) ) {
            CMemFree( lcl_argv[0] );
        }
    #else
        argc = argc;
        cmd_line[0] = cmd_line[0];
        retcode = IdeDrvExecDLL( &info, &argv[1] );
    #endif
    switch( retcode ) {
      case IDEDRV_SUCCESS :
      case IDEDRV_ERR_RUN :
      case IDEDRV_ERR_RUN_EXEC :
      case IDEDRV_ERR_RUN_FATAL :
        break;
      default :
        retcode = IdeDrvPrintError( &info );
        break;
    }
    return retcode;
}


#endif  // NO_MO_COMPILING
