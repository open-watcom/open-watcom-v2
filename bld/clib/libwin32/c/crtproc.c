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


#include "variety.h"
#include <mbstring.h>
#include <windows.h>
#include "liballoc.h"
#include "libwin32.h"
#include "osver.h"

/*
 * The Win32 CreateProcessW actually has a few other parameters, but the
 * C library doesn't really use them.  Instead of bloating the executables
 * with conversion code that won't ever be executed, always supply the
 * values that would be passed in anyway.
 */

BOOL __lib_CreateProcessW( LPWSTR lpCommandLine,
                           BOOL bInheritHandles,
                           LPVOID lpEnvironment,
                           LPPROCESS_INFORMATION lpProcessInformation )
/*********************************************************************/
{
    if( WIN32_IS_NT ) {                                 /* NT */
        STARTUPINFOW    startupInfo;

        memset( &startupInfo, 0, sizeof( startupInfo ) );
        startupInfo.wShowWindow = SW_NORMAL;
        return( CreateProcessW( NULL, lpCommandLine, NULL, NULL,
                                bInheritHandles, CREATE_UNICODE_ENVIRONMENT,
                                lpEnvironment, NULL, &startupInfo,
                                lpProcessInformation ) );
    } else {                                            /* Win95 or Win32s */
        char *          mbCommandLine;
        char *          mbEnvironment;
        BOOL            osrc;
        STARTUPINFOA    mbStartupInfo;
        size_t          cvt;
        size_t          len;
        size_t          size;
        wchar_t *       wp;
        char *          p;

        /*** Convert lpCommandLine to MBCS ***/
        len = wcslen( lpCommandLine ) * MB_CUR_MAX + 1;
        mbCommandLine = lib_malloc( len );
        if( mbCommandLine == NULL ) {
            return( FALSE );
        }
        cvt = wcstombs( mbCommandLine, lpCommandLine, len );
        if( cvt == (size_t)-1 ) {
            lib_free( mbCommandLine );
            return( FALSE );
        }

        /*** Determine the size of a MBCS version of the environment ***/
        size = 0;
        wp = lpEnvironment;
        while( *wp != L'\0' ) {
            len = wcslen( wp ) + 1;
            size += len;
            wp += len;
        }
        size *= MB_CUR_MAX;
        size++;                 /* for terminating null byte */

        /*** Create a MBCS version of the environment ***/
        mbEnvironment = lib_malloc( size );     /* allocate some room */
        if( mbEnvironment == NULL ) {
            lib_free( mbCommandLine );
            return( FALSE );
        }
        wp = lpEnvironment;                     /* convert the strings */
        p = mbEnvironment;
        while( *wp != L'\0' ) {
            cvt = wcstombs( p, wp, size );
            if( cvt != (size_t)-1 ) {
                p += cvt;
                *p = '\0';
                p++;
                size -= cvt + 1;
            } else {
                /* just ignore this string */
            }
            wp += wcslen( wp ) + 1;
        }
        *p = '\0';                      /* terminate with empty string */

        /*** Call the OS ***/
        memset( &mbStartupInfo, 0, sizeof( mbStartupInfo ) );
        mbStartupInfo.wShowWindow = SW_NORMAL;
        osrc = CreateProcessA( NULL, mbCommandLine, NULL, NULL,
                               bInheritHandles, 0, mbEnvironment, NULL,
                               &mbStartupInfo, lpProcessInformation );
        lib_free( mbCommandLine );
        lib_free( mbEnvironment );

        return( osrc );
    }
}
