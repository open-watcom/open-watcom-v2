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
#include "widechar.h"
#include <string.h>
#include <windows.h>
#include "libwin32.h"


/*
 * Apparently GetFileAttributes 3.51 sometimes gets confused when the
 * file in question is on a FAT drive.  Since FindFirstFile seems to
 * work, use it instead.
 */

#ifdef __WIDECHAR__
 DWORD __fixed_GetFileAttributesW( LPCWSTR lpFileName )
#else
 DWORD __fixed_GetFileAttributesA( LPCSTR lpFileName )
#endif
/*****************************************************/
{
    HANDLE                  handle;
    #ifdef __WIDECHAR__
        WIN32_FIND_DATAW    finddata;
    #else
        WIN32_FIND_DATAA    finddata;
    #endif

    /*** Fail if the filename contains a wildcard ***/
    #ifdef __WIDECHAR__
        if( wcschr( lpFileName, L'*' ) != NULL  ||
            wcschr( lpFileName, L'?' ) != NULL ) {
            return( 0xFFFFFFFF );
        }
    #else
        if( strchr( lpFileName, '*' ) != NULL  ||
            strchr( lpFileName, '?' ) != NULL ) {
            return( 0xFFFFFFFF );
        }
    #endif

    /*** Ok, use FindFirstFile to get the file attribute ***/
    #ifdef __WIDECHAR__
        handle = __lib_FindFirstFileW( lpFileName, &finddata );
    #else
        handle = FindFirstFileA( lpFileName, &finddata );
    #endif
    if( handle == INVALID_HANDLE_VALUE ) {
        return( 0xFFFFFFFF );
    } else {
        FindClose( handle );
    }
    return( finddata.dwFileAttributes );
}
