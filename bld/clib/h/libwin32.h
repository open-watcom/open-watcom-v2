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
    #define __lib_CreateProcessW(a,b,c,d,e)     CreateProcessW(NULL,a,NULL,NULL,b,CREATE_UNICODE_ENVIRONMENT,c,NULL,d,e)
#else
extern BOOL __lib_CreateProcessW( LPWSTR lpCommandLine, BOOL bInheritHandles, LPVOID lpEnvironment,
                                      LPSTARTUPINFOW startupInfo, LPPROCESS_INFORMATION lpProcessInformation );
#endif
#define __lib_CreateProcessA(a,b,c,d,e)         CreateProcessA(NULL,a,NULL,NULL,b,0,c,NULL,d,e)

#ifdef __WIDECHAR__
    #define __lib_CreateProcess                 __lib_CreateProcessW
#else
    #define __lib_CreateProcess                 __lib_CreateProcessA
#endif

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

#else

extern BOOL     __lib_CreateDirectoryW( LPCWSTR lpPathName,
                                        LPSECURITY_ATTRIBUTES lpSecurityAttributes );

extern HANDLE   __lib_CreateFileW( LPCWSTR lpFileName,
                                   DWORD dwDesiredAccess,
                                   DWORD dwShareMode,
                                   LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                                   DWORD dwCreationDisposition,
                                   DWORD dwFlagsAndAttributes,
                                   HANDLE hTemplateFile );

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

extern DWORD    __lib_GetModuleFileNameW( HINSTANCE hModule,
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

#endif

#define __lib_CreateDirectoryA                  CreateDirectoryA
#define __lib_CreateFileA                       CreateFileA
#define __lib_DeleteFileA                       DeleteFileA
#define __lib_FindFirstFileA                    FindFirstFileA
#define __lib_FindNextFileA                     FindNextFileA
#define __lib_GetCurrentDirectoryA              GetCurrentDirectoryA
#define __lib_GetDriveTypeA                     GetDriveTypeA
#define __lib_GetFileAttributesA                __fixed_GetFileAttributesA
#define __lib_GetFullPathNameA                  GetFullPathNameA
#define __lib_GetModuleFileNameA                GetModuleFileNameA
#define __lib_MoveFileA                         MoveFileA
#define __lib_RemoveDirectoryA                  RemoveDirectoryA
#define __lib_SetCurrentDirectoryA              SetCurrentDirectoryA
#define __lib_SetEnvironmentVariableA           SetEnvironmentVariableA
#define __lib_SetFileAttributesA                SetFileAttributesA


#ifdef __WIDECHAR__
    #define __lib_CreateDirectory               __lib_CreateDirectoryW
    #define __lib_CreateFile                    __lib_CreateFileW
    #define __lib_DeleteFile                    __lib_DeleteFileW
    #define __lib_FindFirstFile                 __lib_FindFirstFileW
    #define __lib_FindNextFile                  __lib_FindNextFileW
    #define __lib_GetCurrentDirectory           __lib_GetCurrentDirectoryW
    #define __lib_GetDriveType                  __lib_GetDriveTypeW
    #define __lib_GetFileAttributes             __lib_GetFileAttributesW
    #define __lib_GetFullPathName               __lib_GetFullPathNameW
    #define __lib_GetModuleFileName             __lib_GetModuleFileNameW
    #define __lib_MoveFile                      __lib_MoveFileW
    #define __lib_RemoveDirectory               __lib_RemoveDirectoryW
    #define __lib_SetCurrentDirectory           __lib_SetCurrentDirectoryW
    #define __lib_SetEnvironmentVariable        __lib_SetEnvironmentVariableW
    #define __lib_SetFileAttributes             __lib_SetFileAttributesW
#else
    #define __lib_CreateDirectory               __lib_CreateDirectoryA
    #define __lib_CreateFile                    __lib_CreateFileA
    #define __lib_DeleteFile                    __lib_DeleteFileA
    #define __lib_FindFirstFile                 __lib_FindFirstFileA
    #define __lib_FindNextFile                  __lib_FindNextFileA
    #define __lib_GetCurrentDirectory           __lib_GetCurrentDirectoryA
    #define __lib_GetDriveType                  __lib_GetDriveTypeA
    #define __lib_GetFileAttributes             __lib_GetFileAttributesA
    #define __lib_GetFullPathName               __lib_GetFullPathNameA
    #define __lib_GetModuleFileName             __lib_GetModuleFileNameA
    #define __lib_MoveFile                      __lib_MoveFileA
    #define __lib_RemoveDirectory               __lib_RemoveDirectoryA
    #define __lib_SetCurrentDirectory           __lib_SetCurrentDirectoryA
    #define __lib_SetEnvironmentVariable        __lib_SetEnvironmentVariableA
    #define __lib_SetFileAttributes             __lib_SetFileAttributesA
#endif

#endif
