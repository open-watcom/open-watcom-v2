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


/*
 Description:
 ============
 This is the import library side of the version verification. This module
 must be added to the import library and can be added to the dll (although
 that copy of it will never be used).

*/

#include <windows.h>
#include <string.h>
#include "version.h"

#define _glue( a, b ) a ## b
#define _xglue( a, b ) _glue( a, b )
#define _str(a) # a
#define _xstr(a) _str(a)

#define _func_name( func ) _xglue( DLL_PREFIX, func )

#ifdef __WATCOMC__
#pragma off (unreferenced);
#endif

static char VersionDllProc[] = _xstr( _func_name( version_dll_num ) );
static char VersionStr[] = "Version Verification: "
                            _xstr( DLL_FILE_NAME )
                            " expected version "
                            _xstr( DLL_VERSION );

extern unsigned _func_name( version_import )( void )
/**************************************************/
{
    return( DLL_VERSION );
}

extern unsigned _func_name( version_dll )( void )
/***********************************************/
{
    HANDLE      lib;
    FARPROC     ver_dll_num;
    unsigned    ver;

    strupr( VersionDllProc );

    lib = LoadLibrary( _xstr( DLL_FILE_NAME ) );
#if defined( __WINDOWS__ )
    if( lib < (HANDLE)32 ) {
#else
    if( lib == NULL ) {
#endif
        /* couldn't load the dll so the version is 0 */
        ver = 0;
    } else {
        ver_dll_num = GetProcAddress( lib, VersionDllProc );
        if( ver_dll_num == NULL ) {
            ver = 0;
        } else {
            ver = ((unsigned(*)(void))(*ver_dll_num))();
        }
        FreeLibrary( lib );
    }

    return( ver );
}

extern int _func_name( version_check )( void )
/********************************************/
{
    return( _func_name( version_dll )() == _func_name( version_import )() );
}
