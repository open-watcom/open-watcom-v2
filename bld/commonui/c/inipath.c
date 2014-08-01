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
* Description:  Gets the path in which to store configuration information
*
****************************************************************************/

#include "precomp.h"
#include <direct.h>
#include <string.h>
#include "wio.h"
#ifdef __NT__
    #include <shellapi.h>
    #include <shlobj.h>
    typedef HRESULT (WINAPI *GetFolderPath)( HWND, int, HANDLE, DWORD, LPTSTR );
#endif
#include "watcom.h"

#define CONFIG_DIR "Open Watcom"


/*
 * GetConfigFilePath - get the path to the directory containing the config files
 *
 * SHFOLDER.DLL is loaded explicitly for compatability with Win95.  Calling
 * SHGetFolderPathA directly doesn't work, probably due to order of linking.
 */
void GetConfigFilePath( char *path, size_t size )
{
#ifdef __NT__
    HINSTANCE library = LoadLibrary( "shfolder.dll" );
    if( library != NULL ) {
        GetFolderPath getpath = (GetFolderPath)GetProcAddress(library, "SHGetFolderPathA");
        if (SUCCEEDED( getpath( NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, path ) ) ) {
            if( strlen( path ) + strlen( "\\" CONFIG_DIR ) + 12 < size ) {
                strcat( path, "\\" CONFIG_DIR );
                if( access( path, F_OK ) ) {    /* make sure CONFIG_DIR diretory is present */
                    mkdir( path );              /* if not, create it */
                }
            }
        }
        FreeLibrary( library );
    } else {                                    /* should only get here on old machines */
        GetWindowsDirectory( path, (UINT)size );      /* that don't have SHFOLDER.DLL */
    }
#else
    GetWindowsDirectory( path, (UINT)size );
#endif

} /* GetConfigFilePath */
