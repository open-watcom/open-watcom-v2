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
#include <stdlib.h>
#include <windows.h>
#include "liballoc.h"
#include "libwin32.h"

/*
 * Since the multibyte stuff isn't initialized when this module is called
 * from the startup code, call MultiByteToWideChar directly.  Since we tell
 * the OS to use the system default code page during conversion, this
 * module will have limited use outside the startup code... which at present
 * is the only place it is used.
 *
 * Note that we can't use osver.h either, since _osbuild and _osmajor are
 * also uninitialized at this point.
 *
 * This function is exported (_WCRTLINK'ed) so that the startup code works
 * when using the DLL version of the library.
 */

_WCRTLINK DWORD __lib_GetModuleFileNameW( HINSTANCE hModule,
                                          LPWSTR lpFilename,
                                          DWORD nSize )
/**********************************************************/
{
    if( HIWORD( GetVersion() ) < 0x8000 ) {             /* NT */
        return( GetModuleFileNameW( hModule, lpFilename, nSize ) );
    } else {                                            /* Win95 or Win32s */
        char *          mbFileName;
        DWORD           osrc;
        size_t          cvt;

        /*** Allocate some memory ***/
        mbFileName = lib_malloc( _MAX_PATH * MB_CUR_MAX );
        if( mbFileName == NULL ) {
            return( 0 );
        }

        /*** Call the OS ***/
        osrc = GetModuleFileNameA( hModule, mbFileName, _MAX_PATH*MB_CUR_MAX );
        if( osrc == 0 ) {
            lib_free( mbFileName );
            return( 0 );
        }

        /*** Convert returned pathname ***/
        cvt = MultiByteToWideChar( CP_OEMCP, MB_PRECOMPOSED, mbFileName,
                                   -1, lpFilename, nSize );
        lib_free( mbFileName );
        if( cvt == 0 )  return( 0 );
        lpFilename[nSize-1] = L'\0';    /* ensure null-terminated */

        /*** Clean up and go home ***/
        return( wcslen( lpFilename ) ); /* return string length */
    }
}
