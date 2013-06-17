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
* Description:  Viper INI file access routines.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <io.h>
#if defined( __WINDOWS__ ) || defined( __NT__ )
    #include <windows.h>
    #ifdef __NT__
        #include <shlobj.h>
        typedef HRESULT (WINAPI *GetFolderPath)(HWND, int, HANDLE, DWORD, LPTSTR);
    #endif
#endif
#include "inifile.hpp"

#define IDE_INI_FILENAME        "watcom.ini"
#define IDE_INI_DIR             "Open Watcom"


#if defined( __OS2__ )
    /* On OS/2, closing a profile is potentially expensive operation
     * (buffer flushes and whatnot). Therefore we cache the ini handle
     * to minimize the profile open/close operations.
     */
    IniFile::IniFile()
    {
        char    buff[FILENAME_MAX];
        char    *p = getenv( "USER_INI" );

        strcpy( buff, p );
        for( p = buff + strlen(buff) - 1; p >= buff; p-- ) {
            if( *p == '\\' ) {
                *p = 0;
                break;
            }
        }
        strcat( buff, "\\" IDE_INI_FILENAME);
        _handle = PrfOpenProfile( NULL, (PSZ)buff );
    }
    
    IniFile::~IniFile( )
    {
        PrfCloseProfile( _handle );
    }
    
    int IniFile::read( const char *section, const char *key, const char *deflt,
                  char *buffer, int len )
    {
        return( PrfQueryProfileString( _handle, (PSZ)section, (PSZ)key, (PSZ)deflt,
                                       buffer, len ) );
    }
    
    int IniFile::write( const char *section, const char *key, const char *string)
    {
        return( PrfWriteProfileString( _handle, (PSZ)section, (PSZ)key, (PSZ)string ) );
    }

#elif defined( __WINDOWS__ ) || defined( __NT__ )
    /*
     * shfolder.dll is loaded explicitly for compatability with Win98 -- calling
     * SHGetFolderPathA directly doesn't work, probably due to order of linking
    */
    IniFile::IniFile()
    {
        char path[FILENAME_MAX];
#ifdef __NT__
        HINSTANCE library = LoadLibrary( "shfolder.dll" );
        if ( library ) {
            GetFolderPath getpath = (GetFolderPath)GetProcAddress(library, "SHGetFolderPathA");
            if( SUCCEEDED( getpath( NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, path ) ) ) {
                if( strlen( path ) + strlen( "\\" IDE_INI_DIR ) + 12 < FILENAME_MAX) {
                    strcat( path, "\\" IDE_INI_DIR);
                    if( access(path, F_OK) )    // make sure CONFIG_DIR diretory is present
                        mkdir( path );          // if not, create it
                }
            }
            FreeLibrary( library );
        }
        else                                    // should only get here on old machines
            GetWindowsDirectory( path, FILENAME_MAX );  // that don't have shfolder.dll
#else
        GetWindowsDirectory( path, FILENAME_MAX );
#endif
        strcat( path, "\\" IDE_INI_FILENAME );
        _path = path;
    }
    
    int IniFile::read( const char *section, const char *key, const char *deflt,
                  char *buffer, int len )
    {
        return( GetPrivateProfileString( section, key, deflt, buffer, len, _path) );
    }
    
    int IniFile::write( const char *section, const char *key, const char *string)
    {
        return( WritePrivateProfileString( section, key, string, _path) );
    }

#ifdef __WATCOMC__
    // Complain about defining trivial destructor inside class
    // definition only for warning levels above 8 
#pragma warning 657 9
#endif

   IniFile::~IniFile( ) { }
    
#else
    #error UNSUPPORTED OS
#endif

