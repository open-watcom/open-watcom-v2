/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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


BOOL __lib_SetEnvironmentVariableW( LPCWSTR lpName, LPCWSTR lpValue )
/*******************************************************************/
{
    if( WIN32_IS_NT ) {                                 /* NT */
        return( SetEnvironmentVariableW( lpName, lpValue ) );
    } else {                                            /* Win95 or Win32s */
        char *          mbName;
        char *          mbValue;
        BOOL            osrc;

        /*** Prepare to call the OS ***/
        mbName = __lib_cvt_wcstombs( lpName );
        if( mbName == NULL ) {
            return( 0 );
        }

        if( lpValue == NULL ) {
            mbValue = NULL;
        } else {
            mbValue = __lib_cvt_wcstombs( lpValue );
            if( mbValue == NULL ) {
                lib_free( mbName );
                return( 0 );
            }
        }

        /*** Call the OS ***/
        osrc = SetEnvironmentVariableA( mbName, mbValue );
        lib_free( mbName );
        if( mbValue != NULL )
            lib_free( mbValue );
        return( osrc );
    }
}
