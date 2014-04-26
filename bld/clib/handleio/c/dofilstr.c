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


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "iomode.h"
#include "filestr.h"


/*
 * Parse the C_FILE_INFO environment variable created by the parent process.
 * If C_FILE_INFO isn't in the environment, don't do anything; if it is,
 * create the POSIX-level file handles accordingly.
 */
_WCRTLINK void __F_NAME(__ParsePosixHandleStr,__wParsePosixHandleStr)( void )
/***************************************************************************/
{
    CHAR_TYPE *         envp;
    CHAR_TYPE *         p;
    int                 posixHandle, osHandle, mode;
    int                 len;
    CHAR_TYPE           buf[9];

    /*** Get the environment variable ***/
    envp = __F_NAME(getenv,_wgetenv)( STRING( "C_FILE_INFO" ) );
    if( envp == NULL )  return;

    /*** Process the items, one by one ***/
    while( *envp != NULLCHAR ) {
        p = envp;

        /*** Extract the three data fields ***/
        p = __F_NAME(strchr,wcschr)( p, STRING( ':' ) );      /* POSIX handle field */
        len = p - envp;
        __F_NAME(strncpy,wcsncpy)( buf, envp, len );
        buf[len] = NULLCHAR;
        posixHandle = (int)__F_NAME(strtol,wcstol)( buf, NULL, 16 );
        p++;

        envp = p;                                   /* OS handle field */
        p = __F_NAME(strchr,wcschr)( p, STRING( ':' ) );
        len = p - envp;
        __F_NAME(strncpy,wcsncpy)( buf, envp, len );
        buf[len] = NULLCHAR;
        osHandle = (int)__F_NAME(strtol,wcstol)( buf, NULL, 16 );
        p++;

        envp = p;                                   /* file mode field */
        p = __F_NAME(strchr,wcschr)( p, STRING( '*' ) );
        len = p - envp;
        __F_NAME(strncpy,wcsncpy)( buf, envp, len );
        buf[len] = NULLCHAR;
        mode = (int)__F_NAME(strtol,wcstol)( buf, NULL, 16 );
        p++;

        /*** Create the corresponding file ***/
        __setOSHandle( posixHandle, (HANDLE)osHandle );
        __SetIOMode( posixHandle, mode );

        envp = p;
    }

    /*** Delete the environment variable ***/
    __F_NAME(putenv,_wputenv)( STRING( "C_FILE_INFO=" ) );
}
