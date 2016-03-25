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

DWORD __lib_GetFullPathNameW( LPCWSTR lpFileName, DWORD nBufferLength,
                              LPWSTR lpBuffer, LPWSTR *lpFilePart )
/********************************************************************/
{
    if( WIN32_IS_NT ) {                                 /* NT */
        return( GetFullPathNameW( lpFileName, nBufferLength, lpBuffer, lpFilePart ) );
    } else {                                            /* Win95 or Win32s */
        char *          mbFileName;
        char *          mbBuffer;
        char *          mbFilePart;
        char *          p;
        size_t          len;
        DWORD           osrc;
        size_t          cvt;

        /*** Allocate some memory ***/
        len = wcslen( lpFileName ) * MB_CUR_MAX + 1;
        mbFileName = lib_malloc( len );
        if( mbFileName == NULL ) {
            return( 0 );
        }
        mbBuffer = lib_malloc( _MAX_PATH * MB_CUR_MAX );
        if( mbBuffer == NULL ) {
            lib_free( mbFileName );
            return( 0 );
        }

        /*** Prepare to call the OS ***/
        cvt = wcstombs( mbFileName, lpFileName, len );
        if( cvt == (size_t)-1 ) {
            lib_free( mbFileName );
            lib_free( mbBuffer );
            return( 0 );
        }

        /*** Call the OS ***/
        osrc = GetFullPathNameA( mbFileName, _MAX_PATH*MB_CUR_MAX,
                                 mbBuffer, &mbFilePart );
        lib_free( mbFileName );
        if( osrc == 0 ) {
            lib_free( mbBuffer );
            return( 0 );
        }

        /*** If the buffer is too small, return required length ***/
        len = _mbslen( (unsigned char *)mbBuffer ) + 1;
        if( len > nBufferLength ) {
            lib_free( mbBuffer );
            return( len );
        }

        /*** Convert returned pathname ***/
        cvt = mbstowcs( lpBuffer, mbBuffer, nBufferLength );
        if( cvt == (size_t)-1 ) {
            lib_free( mbBuffer );
            return( 0 );
        }

        /*** Convert file part pointer ***/
        p = mbBuffer;
        for( len=0; p<mbFilePart; len++ ) {
            p = (char *)_mbsinc( (unsigned char *)p );
        }
        *lpFilePart = lpBuffer + len;

        /*** Clean up and go home ***/
        lib_free( mbBuffer );
        return( wcslen( lpBuffer ) );   /* return string length */
    }
}
