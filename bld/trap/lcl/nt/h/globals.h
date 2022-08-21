/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Global definitions for Win32 trap file.
*
****************************************************************************/


#ifdef TRPGLOBINIT
#define TRPGLOBAL
#else
#define TRPGLOBAL extern
#define TRPGLOBINIT(x)
#endif


#if defined( WOW )
  #if defined( MD_x86 )
//typedef void    *LPVDMCONTEXT;
  #endif
#else
typedef void    *LPMODULEENTRY;
typedef void    *LPVDMCONTEXT;
typedef void    *LPVDMLDT_ENTRY;
typedef void    (WINAPI *DEBUGEVENTPROC)();
typedef BOOL    (WINAPI *PROCESSENUMPROC)( DWORD, DWORD, LPARAM );
#endif

TRPGLOBAL DWORD         DebugeePid;
TRPGLOBAL DWORD         DebugeeTid;
TRPGLOBAL process_info  ProcessInfo;
TRPGLOBAL bool          DebugeeEnded;
TRPGLOBAL DWORD         LastExceptionCode;
TRPGLOBAL DWORD         CurrentModule TRPGLOBINIT( 1 );
TRPGLOBAL DWORD         ModuleTop;
TRPGLOBAL WORD          FlatCS;
TRPGLOBAL WORD          FlatDS;
TRPGLOBAL bool          StopForDLLs;
TRPGLOBAL LPSTR         DLLPath;
TRPGLOBAL subsystems    DebugeeSubsystem;
TRPGLOBAL msg_list      *DebugString;
TRPGLOBAL DEBUG_EVENT   DebugEvent;
TRPGLOBAL bool          IsWOW;
#if !defined( MD_x64 )
TRPGLOBAL bool          IsDOS;
TRPGLOBAL bool          IsWin32s;
TRPGLOBAL bool          IsWin95;
TRPGLOBAL bool          IsWinNT;
TRPGLOBAL bool          UseVDMStuff;
#endif
TRPGLOBAL char          CurrEXEName[MAX_PATH];
TRPGLOBAL bool          DidWaitForDebugEvent;
TRPGLOBAL bool          Slaying;
TRPGLOBAL HWND          DebuggerWindow;
TRPGLOBAL DWORD         LastDebugEventTid;
TRPGLOBAL bool          BreakOnKernelMessage;
TRPGLOBAL bool          PendingProgramInterrupt;
TRPGLOBAL char          *MsgPrefix TRPGLOBINIT( NULL );
TRPGLOBAL bool          Supporting8ByteBreakpoints TRPGLOBINIT( false );    /* Start disabled */
TRPGLOBAL bool          SupportingExactBreakpoints TRPGLOBINIT( false );    /* Start disabled */
TRPGLOBAL opcode_type   BreakOpcode;

#if defined( WOW )
  #if defined( MD_x86 )
TRPGLOBAL wow_info      WOWAppInfo;
  #endif
#endif

#if !defined( MD_x64 )
TRPGLOBAL HANDLE
(WINAPI*pOpenThread)(
    DWORD
);

TRPGLOBAL DWORD
(WINAPI *pQueryDosDevice)(
    LPCSTR          lpDeviceName,
    LPSTR           lpTargetPath,
    DWORD           ucchMax
);

TRPGLOBAL DWORD
(WINAPI *pGetMappedFileName)(
    HANDLE          hProcess,
    LPVOID          lpv,
    LPSTR           lpFilename,
    DWORD           nSize
);

TRPGLOBAL HANDLE
(WINAPI *pCreateToolhelp32Snapshot)(
    DWORD           dwFlags,
    DWORD           th32ProcessID
);

TRPGLOBAL BOOL
(WINAPI *pModule32First)(
    HANDLE          hSnapshot,
    LPMODULEENTRY32 lpme
);

TRPGLOBAL BOOL
(WINAPI *pModule32Next)(
    HANDLE          hSnapshot,
    LPMODULEENTRY32 lpme
);

TRPGLOBAL BOOL
(WINAPI*pVDMModuleFirst)(
    HANDLE          hProcess,
    HANDLE          hThread,
    LPMODULEENTRY   lpModuleEntry,
    DEBUGEVENTPROC  lpEventProc,
    LPVOID          lpData
);

TRPGLOBAL BOOL
(WINAPI*pVDMModuleNext)(
    HANDLE          hProcess,
    HANDLE          hThread,
    LPMODULEENTRY   lpModuleEntry,
    DEBUGEVENTPROC  lpEventProc,
    LPVOID          lpData
);

TRPGLOBAL INT
(WINAPI*pVDMEnumProcessWOW)(
    PROCESSENUMPROC fp,
    LPARAM          lparam
);

TRPGLOBAL BOOL
(WINAPI*pVDMProcessException)(
    LPDEBUG_EVENT   lpDebugEvent
    );


TRPGLOBAL BOOL
(WINAPI*pVDMGetModuleSelector)(
    HANDLE          hProcess,
    HANDLE          hThread,
    UINT            wSegmentNumber,
    LPSTR           lpModuleName,
    LPWORD          lpSelector
);


TRPGLOBAL BOOL
(WINAPI*pVDMGetThreadContext)(
    LPDEBUG_EVENT   lpDebugEvent,
    LPVDMCONTEXT    lpVDMContext
);

TRPGLOBAL BOOL
(WINAPI*pVDMSetThreadContext)(
    LPDEBUG_EVENT   lpDebugEvent,
    LPVDMCONTEXT    lpVDMContext
);

TRPGLOBAL BOOL
(WINAPI*pVDMGetThreadSelectorEntry)(
    HANDLE          hProcess,
    HANDLE          hThread,
    WORD            wSelector,
    LPVDMLDT_ENTRY  lpSelectorEntry
);
#endif

#undef TRPGLOBAL
#undef TRPGLOBINIT
