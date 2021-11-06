/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
#include <stddef.h>
#include <mbstring.h>
#include <windows.h>
#include "liballoc.h"
#include "libwin32.h"
#include "osver.h"
#include "cvtwc2mb.h"


UINT __lib_GetDriveTypeW( LPCWSTR lpRootPathName )
/************************************************/
{
    if( WIN32_IS_NT ) {                                 /* NT */
        return( GetDriveTypeW( lpRootPathName ) );
    } else {                                            /* Win95 or Win32s */
        char            *mbRootPathName;
        UINT            osrc;

        /*** Prepare to call the OS ***/
        mbRootPathName = __lib_cvt_wcstombs( lpRootPathName );
        if( mbRootPathName == NULL ) {
            return( FALSE );
        }

        /*** Call the OS ***/
        osrc = GetDriveTypeA( mbRootPathName );
        lib_free( mbRootPathName );
        return( osrc );
    }
}
