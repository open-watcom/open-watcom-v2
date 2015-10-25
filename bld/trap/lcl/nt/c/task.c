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
* Description:  Win32 trap file startup and shutdown.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stdnt.h"
#include "trpimp.h"
#include "trpimpxx.h"
#include "globals.h"
#include "task.h"

// for old header files
#ifndef VER_PLATFORM_WIN32_WINDOWS
#define VER_PLATFORM_WIN32_WINDOWS 1
#endif

trap_version TRAPENTRY TrapInit( const char *parms, char *err, bool remote )
{
    trap_version    ver;
    OSVERSIONINFO   osver;
    HANDLE          dll;

    osver.dwOSVersionInfoSize = sizeof( osver );
    GetVersionEx( &osver );
#if defined( MD_x64 )
#else
    IsWin32s = FALSE;
    IsWin95 = FALSE;
    IsWinNT = FALSE;
    if( osver.dwPlatformId == VER_PLATFORM_WIN32s ) {
        IsWin32s = TRUE;
    } else if( osver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ) {
        IsWin95 = TRUE;
    } else if( osver.dwPlatformId == VER_PLATFORM_WIN32_NT ) {
        IsWinNT = TRUE;
    }
    if( IsWinNT ) {
        dll = LoadLibrary( "VDMDBG.DLL" );
        if( dll != NULL ) {
            pVDMSetThreadContext        = (LPVOID)GetProcAddress( dll, "VDMSetThreadContext" );
            pVDMModuleFirst             = (LPVOID)GetProcAddress( dll, "VDMModuleFirst" );
            pVDMModuleNext              = (LPVOID)GetProcAddress( dll, "VDMModuleNext" );
            pVDMEnumProcessWOW          = (LPVOID)GetProcAddress( dll, "VDMEnumProcessWOW" );
            pVDMProcessException        = (LPVOID)GetProcAddress( dll, "VDMProcessException" );
            pVDMGetModuleSelector       = (LPVOID)GetProcAddress( dll, "VDMGetModuleSelector" );
            pVDMGetThreadContext        = (LPVOID)GetProcAddress( dll, "VDMGetThreadContext" );
            pVDMGetThreadSelectorEntry  = (LPVOID)GetProcAddress( dll, "VDMGetThreadSelectorEntry" );
        }
        dll = LoadLibrary( "PSAPI.DLL" );
        if( dll != NULL ) {
            pGetMappedFileName          = (LPVOID)GetProcAddress( dll, "GetMappedFileNameA" );
        }
    }
#endif
    dll = LoadLibrary( "KERNEL32.DLL" );
    if( dll != NULL ) {
        pOpenThread                 = (LPVOID)GetProcAddress( dll, "OpenThread" );
        pQueryDosDevice             = (LPVOID)GetProcAddress( dll, "QueryDosDeviceA" );
        pCreateToolhelp32Snapshot   = (LPVOID)GetProcAddress( dll, "CreateToolhelp32Snapshot" );
        pModule32First              = (LPVOID)GetProcAddress( dll, "Module32First" );
        pModule32Next               = (LPVOID)GetProcAddress( dll, "Module32Next" );
    }
    //say( "base address=%8.8x", ((char*)&GetInPtr)-0x2f );
    DLLPath = LocalAlloc( LMEM_FIXED | LMEM_ZEROINIT, strlen( err ) + 1 );
    strcpy( DLLPath, err );

    StopForDLLs = TRUE;
    BreakOnKernelMessage = FALSE;
    if( *parms == 'k' ) {
        BreakOnKernelMessage = TRUE;
    }
    remote = remote;

    err[0] = 0;
    ver.major = TRAP_MAJOR_VERSION;
    ver.minor = TRAP_MINOR_VERSION;
    ver.remote = FALSE;
    return( ver );
}

void TRAPENTRY TrapFini( void )
{
}

/*
 * TrapTellHWND - inform trap file of gui debugger being used
 */
void TRAPENTRY TrapTellHWND( HWND hwnd )
{
    DebuggerWindow = hwnd;
}

#if 0
/*
 * TrapListLibs - this is called by the debugger to dump out a list
 *                of DLL's and their associated selectors
 */
int TRAPENTRY TrapListLibs( char *buff, int is_first, int want_16,
                         int want_32, int verbose, int sel )
{
    return( DoListLibs( buff, is_first, want_16, want_32, verbose, sel ) );
}
#endif
