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
* Description:  Prototypes for dealing with Win95's crippled Win32 support.
*
****************************************************************************/


#ifndef _LIBWIN32_H_INCLUDED
#define _LIBWIN32_H_INCLUDED

#include "variety.h"

#ifndef _WINDOWS_
    #include <windows.h>
#endif


extern BOOL     __lib_CreateDirectoryW( LPCWSTR lpPathName,
                                        LPSECURITY_ATTRIBUTES lpSecurityAttributes );

extern HANDLE   __lib_CreateFileW( LPCWSTR lpFileName,
                                   DWORD dwDesiredAccess,
                                   DWORD dwShareMode,
                                   LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                                   DWORD dwCreationDisposition,
                                   DWORD dwFlagsAndAttributes,
                                   HANDLE hTemplateFile );

extern BOOL     __lib_CreateProcessW( LPWSTR lpCommandLine,
                                      BOOL bInheritHandles,
                                      LPVOID lpEnvironment,
                                      LPPROCESS_INFORMATION lpProcessInformation );

extern BOOL     __lib_DeleteFileW( LPCWSTR lpFileName );

extern HANDLE   __lib_FindFirstFileW( LPCWSTR lpFileName,
                                      LPWIN32_FIND_DATAW lpFindFileData );

extern BOOL     __lib_FindNextFileW( HANDLE hFindFile,
                                     LPWIN32_FIND_DATAW lpFindFileData );

extern DWORD    __lib_GetCurrentDirectoryW( DWORD nBufferLength,
                                            LPWSTR lpBuffer );

extern UINT     __lib_GetDriveTypeW( LPCWSTR lpRootPathName );

extern DWORD    __lib_GetFileAttributesW( LPCWSTR lpFileName );

extern DWORD    __lib_GetFullPathNameW( LPCWSTR lpFileName,
                                        DWORD nBufferLength,
                                        LPWSTR lpBuffer,
                                        LPWSTR *lpFilePart );

_WCRTLINK extern DWORD  __lib_GetModuleFileNameW( HINSTANCE hModule,
                                                  LPWSTR lpFilename,
                                                  DWORD nSize );

extern BOOL     __lib_MoveFileW( LPCWSTR lpExistingFileName,
                                 LPCWSTR lpNewFileName );

extern BOOL     __lib_RemoveDirectoryW( LPCWSTR lpPathName );

extern BOOL     __lib_SetCurrentDirectoryW( LPCWSTR lpPathName );

extern BOOL     __lib_SetEnvironmentVariableW( LPCWSTR lpName,
                                               LPCWSTR lpValue );

extern BOOL     __lib_SetFileAttributesW( LPCWSTR lpFileName,
                                          DWORD dwFileAttributes );


/*
 * Function prototypes for dealing with OS bugs.  Your source code should
 * call the __lib_ versions, which in turn will call the __fixed_ versions;
 * this allows for automatic wide/DBCS conversion if necessary and provides
 * a consistent interface.
 */

extern DWORD    __fixed_GetFileAttributesA( LPCSTR lpFileName );
extern DWORD    __fixed_GetFileAttributesW( LPCWSTR lpFileName );


/*
 * On AXP and PPC machines, we don't have to worry about Win95's crippled
 * Win32 support, since we know we'll be running under NT.  However, we
 * always have to fix OS bugs, so don't forget to remap to the corresponding
 * __fixed_ routines where appropriate.
 *
 * Note that __lib_CreateProcessW isn't remapped like the others, since
 * its parameter list is different from CreateProcessW's.
 */

#if defined(__AXP__) || defined(__PPC__)
    #define __lib_CreateDirectoryW              CreateDirectoryW
    #define __lib_CreateFileW                   CreateFileW
    #define __lib_DeleteFileW                   DeleteFileW
    #define __lib_FindFirstFileW                FindFirstFileW
    #define __lib_FindNextFileW                 FindNextFileW
    #define __lib_GetCurrentDirectoryW          GetCurrentDirectoryW
    #define __lib_GetDriveTypeW                 GetDriveTypeW
    #define __lib_GetFileAttributesW            __fixed_GetFileAttributesW
    #define __lib_GetFullPathNameW              GetFullPathNameW
    #define __lib_GetModuleFileNameW            GetModuleFileNameW
    #define __lib_MoveFileW                     MoveFileW
    #define __lib_RemoveDirectoryW              RemoveDirectoryW
    #define __lib_SetCurrentDirectoryW          SetCurrentDirectoryW
    #define __lib_SetEnvironmentVariableW       SetEnvironmentVariableW
    #define __lib_SetFileAttributesW            SetFileAttributesW
#endif

#define __lib_GetFileAttributesA                __fixed_GetFileAttributesA


#endif
