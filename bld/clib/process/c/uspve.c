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
* Description:  Implementation of _wspawnve().
*
****************************************************************************/


#ifdef __NT__

// NT can handle wide character strings directly, so don't bother converting
// path, arguments, and environment to MBCS.
#define __WIDECHAR__
#define UNICODE
#include "spve.c"


#else


#include "variety.h"
#include <process.h>
#include <stdlib.h>
#include "rtdata.h"
#include "exwc2mb.h"


_WCRTLINK int _wspawnve( int mode, const wchar_t *path,
/**********************************************************/
    const wchar_t *const argv[], const wchar_t *const envp[] )
{
    char *              mbPath;
    char **             mbArgv;
    char **             mbEnvp;
    int                 rc;
    int                 sameEnv = 0;

    /*** Convert wide strings to MBCS ***/
    if( envp == NULL || envp == (const wchar_t **)_RWD_wenviron ) {
        sameEnv = 1;
        rc = __exec_wide_to_mbcs( path, argv, NULL, &mbPath, &mbArgv, NULL );
        mbEnvp = _RWD_environ;
    } else {
        rc = __exec_wide_to_mbcs( path, argv, envp, &mbPath, &mbArgv, &mbEnvp );
    }
    if( rc == 0 )  return( -1 );

    /*** Call MBCS version, free converted strings, and return ***/
    rc = spawnve( mode, mbPath, (const char **)mbArgv, (const char **)mbEnvp );
    if( sameEnv ) {
        __exec_wide_to_mbcs_cleanup( mbPath, mbArgv, NULL );
    } else {
        __exec_wide_to_mbcs_cleanup( mbPath, mbArgv, mbEnvp );
    }
    return( rc );
}

#endif
