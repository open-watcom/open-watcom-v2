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


BOOL __lib_FindNextFileW( HANDLE hFindFile,
                          LPWIN32_FIND_DATAW lpFindFileData )
/***********************************************************/
{
    if( WIN32_IS_NT ) {                                 /* NT */
        return( FindNextFileW( hFindFile, lpFindFileData ) );
    } else {                                            /* Win95 or Win32s */
        BOOL                osrc;
        WIN32_FIND_DATAA    mbFindFileData;
        size_t              cvt;

        /*** Call the OS ***/
        osrc = FindNextFileA( hFindFile, &mbFindFileData );
        if( osrc == FALSE ) {
            return( FALSE );
        }

        /*** Convert the WIN32_FIND_DATAA info to WIN32_FIND_DATAW info ***/
        lpFindFileData->dwFileAttributes = mbFindFileData.dwFileAttributes;
        lpFindFileData->ftCreationTime = mbFindFileData.ftCreationTime;
        lpFindFileData->ftLastAccessTime = mbFindFileData.ftLastAccessTime;
        lpFindFileData->ftLastWriteTime = mbFindFileData.ftLastWriteTime;
        lpFindFileData->nFileSizeHigh = mbFindFileData.nFileSizeHigh;
        lpFindFileData->nFileSizeLow = mbFindFileData.nFileSizeLow;
        lpFindFileData->dwReserved0 = mbFindFileData.dwReserved0;
        lpFindFileData->dwReserved1 = mbFindFileData.dwReserved1;
        cvt = mbstowcs( lpFindFileData->cFileName, mbFindFileData.cFileName,
                        MAX_PATH );
        if( cvt == (size_t)-1 ) {
            return( FALSE );
        }
        cvt = mbstowcs( lpFindFileData->cAlternateFileName,
                        mbFindFileData.cAlternateFileName,
                        MAX_PATH );
        if( cvt == (size_t)-1 ) {
            return( FALSE );
        }

        return( osrc );
    }
}
