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


BOOL __lib_SetEnvironmentVariableW( LPCWSTR lpName, LPCWSTR lpValue )
/*******************************************************************/
{
    if( WIN32_IS_NT ) {                                 /* NT */
        return( SetEnvironmentVariableW( lpName, lpValue ) );
    } else {                                            /* Win95 or Win32s */
        char *          mbName;
        char *          mbValue;
        BOOL            osrc;
        size_t          cvt;
        size_t          len;

        /*** Allocate some memory ***/
        len = wcslen( lpName ) * MB_CUR_MAX + 1;
        mbName = lib_malloc( len );
        if( mbName == NULL ) {
            return( FALSE );
        }
        if( lpValue == NULL ) {
            mbValue = NULL;
        } else {
            len = wcslen( lpValue ) * MB_CUR_MAX + 1;
            mbValue = lib_malloc( len );
            if( mbValue == NULL ) {
                lib_free( mbName );
                return( FALSE );
            }
        }

        /*** Prepare to call the OS ***/
        cvt = wcstombs( mbName, lpName, len );
        if( cvt == (size_t)-1 ) {
            lib_free( mbName );
            if( mbValue != NULL )  lib_free( mbValue );
            return( FALSE );
        }
        if( mbValue != NULL ) {
            cvt = wcstombs( mbValue, lpValue, len );
            if( cvt == (size_t)-1 ) {
                lib_free( mbValue );
                return( FALSE );
            }
        }

        /*** Call the OS ***/
        osrc = SetEnvironmentVariableA( mbName, mbValue );
        lib_free( mbName );
        if( mbValue != NULL )  lib_free( mbValue );
        return( osrc );
    }
}
