/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
#include "globals.h"
#include "trpld.h"
#include "trpsys.h"


trap_version TRAPENTRY TrapInit( const char *parms, char *err, bool remote )
{
    trap_version    ver;
    HANDLE          dll;

    /* unused parameters */ (void)remote;

#if MADARCH & MADARCH_X64
    dll = LoadLibrary( "PSAPI.DLL" );
    if( dll != NULL ) {
        pGetMappedFileName = (GETMAPPEDFILENAMEPROC)GetProcAddress( dll, "GetMappedFileNameA" );
    }
#else
    IsWinNT = false;
    IsWin32s = false;
    IsWin95 = false;
    {
        DWORD   osver = GetVersion();
        if( osver < 0x80000000 ) {
            IsWinNT = true;
        } else if( LOBYTE( LOWORD( osver ) ) < 4 ) {
            IsWin32s = true;
        } else {
            IsWin95 = true;
        }
    }
    if( IsWinNT ) {
  #ifdef WOW
        dll = LoadLibrary( "VDMDBG.DLL" );
        if( dll != NULL ) {
            pVDMModuleFirst             = (VDMMODULEFIRSTPROC)GetProcAddress( dll, "VDMModuleFirst" );
            pVDMModuleNext              = (VDMMODULENEXTPROC)GetProcAddress( dll, "VDMModuleNext" );
            pVDMEnumProcessWOW          = (VDMENUMPROCESSWOWPROC)GetProcAddress( dll, "VDMEnumProcessWOW" );
            pVDMProcessException        = (VDMPROCESSEXCEPTIONPROC)GetProcAddress( dll, "VDMProcessException" );
            pVDMGetModuleSelector       = (VDMGETMODULESELECTORPROC)GetProcAddress( dll, "VDMGetModuleSelector" );
            pVDMGetThreadContext        = (VDMGETTHREADCONTEXTPROC)GetProcAddress( dll, "VDMGetThreadContext" );
            pVDMSetThreadContext        = (VDMSETTHREADCONTEXTPROC)GetProcAddress( dll, "VDMSetThreadContext" );
            pVDMGetThreadSelectorEntry  = (VDMGETTHREADSELECTORENTRYPROC)GetProcAddress( dll, "VDMGetThreadSelectorEntry" );
        }
  #endif
        dll = LoadLibrary( "PSAPI.DLL" );
        if( dll != NULL ) {
            pGetMappedFileName = (GETMAPPEDFILENAMEPROC)GetProcAddress( dll, "GetMappedFileNameA" );
        }
    }
#endif
    dll = LoadLibrary( "KERNEL32.DLL" );
    if( dll != NULL ) {
        pOpenThread                 = (OPENTHREADPROC)GetProcAddress( dll, "OpenThread" );
        pQueryDosDevice             = (QUERYDOSDEVICEPROC)GetProcAddress( dll, "QueryDosDeviceA" );
        pCreateToolhelp32Snapshot   = (CREATETOOLHELP32SNAPSHOTPROC)GetProcAddress( dll, "CreateToolhelp32Snapshot" );
        pModule32First              = (MODULE32FIRSTPROC)GetProcAddress( dll, "Module32First" );
        pModule32Next               = (MODULE32NEXTPROC)GetProcAddress( dll, "Module32Next" );
    }
//    say( "base address=%8.8x", ((char*)&GetInPtr)-0x2f );
    DLLPath = LocalAlloc( LMEM_FIXED | LMEM_ZEROINIT, strlen( err ) + 1 );
    strcpy( DLLPath, err );

    BreakOpcode = BRKPOINT;

    StopForDLLs = true;
    BreakOnKernelMessage = false;
    if( *parms == 'k' ) {
        BreakOnKernelMessage = true;
    }
    err[0] = 0;
    ver.major = TRAP_VERSION_MAJOR;
    ver.minor = TRAP_VERSION_MINOR;
    ver.remote = false;
    return( ver );
}

void TRAPENTRY TrapFini( void )
{
}

void TRAPENTRY_FUNC( InfoFunction )( HWND hwnd )
/***********************************************
 * inform trap file of gui debugger being used
 */
{
    DebuggerWindow = hwnd;
}

void TRAPENTRY_FUNC( InterruptProgram )( void )
{
    InterruptProgram();
}

bool TRAPENTRY_FUNC( Terminate )( void )
{
    return( Terminate() );
}

#if 0
int TRAPENTRY_FUNC( ListLibs )( char *buff, int is_first, int want_16,
                         int want_32, int verbose, int sel )
/*********************************************************************
 * this is called by the debugger to dump out a list
 * of DLL's and their associated selectors
 */
{
    return( DoListLibs( buff, is_first, want_16, want_32, verbose, sel ) );
}
#endif
