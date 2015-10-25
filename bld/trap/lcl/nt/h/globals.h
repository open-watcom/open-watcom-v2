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
* Description:  Global definitions for Win32 trap file.
*
****************************************************************************/


#ifdef TRPGLOBINIT
#define TRPGLOBAL
#else
#define TRPGLOBAL extern
#define TRPGLOBINIT(x)
#endif

TRPGLOBAL DWORD                 DebugeePid;
TRPGLOBAL DWORD                 DebugeeTid;
TRPGLOBAL process_info          ProcessInfo;
TRPGLOBAL BOOL                  DebugeeEnded;
TRPGLOBAL WORD                  WPCount;
TRPGLOBAL DWORD                 LastExceptionCode;
TRPGLOBAL DWORD                 CurrentModule TRPGLOBINIT( 1 );
TRPGLOBAL DWORD                 ModuleTop;
TRPGLOBAL WORD                  FlatCS;
TRPGLOBAL WORD                  FlatDS;
TRPGLOBAL BOOL                  StopForDLLs;
TRPGLOBAL LPSTR                 DLLPath;
TRPGLOBAL subsystems            DebugeeSubsystem;
TRPGLOBAL msg_list              *DebugString;
TRPGLOBAL DEBUG_EVENT           DebugEvent;
TRPGLOBAL BOOL                  IsWOW;
#if !defined( MD_x64 )
TRPGLOBAL BOOL                  IsDOS;
TRPGLOBAL BOOL                  IsWin32s;
TRPGLOBAL BOOL                  IsWin95;
TRPGLOBAL BOOL                  IsWinNT;
TRPGLOBAL BOOL                  UseVDMStuff;
#endif
TRPGLOBAL char                  CurrEXEName[MAX_PATH];
TRPGLOBAL BOOL                  DidWaitForDebugEvent;
TRPGLOBAL BOOL                  Slaying;
TRPGLOBAL HWND                  DebuggerWindow;
TRPGLOBAL DWORD                 LastDebugEventTid;
TRPGLOBAL BOOL                  BreakOnKernelMessage;
TRPGLOBAL BOOL                  PendingProgramInterrupt;
TRPGLOBAL char                  *MsgPrefix TRPGLOBINIT( NULL );
TRPGLOBAL BOOL                  Supporting8ByteBreakpoints TRPGLOBINIT( 0 );    /* Start disabled */
TRPGLOBAL BOOL                  SupportingExactBreakpoints TRPGLOBINIT( 0 );    /* Start disabled */

#if defined( MD_x86 ) && defined( WOW )
TRPGLOBAL wow_info              WOWAppInfo;
//typedef void    *LPVDMCONTEXT;
#endif
#if !defined( WOW )
typedef void    *LPMODULEENTRY;
typedef void    (WINAPI *DEBUGEVENTPROC)();
typedef BOOL    (WINAPI *PROCESSENUMPROC)( DWORD, DWORD, LPARAM );
#endif

TRPGLOBAL HANDLE (WINAPI*pOpenThread)( DWORD );

TRPGLOBAL DWORD
(WINAPI *pQueryDosDevice)(
    LPCTSTR         lpDeviceName,
    LPTSTR          lpTargetPath,
    DWORD           ucchMax
);

TRPGLOBAL DWORD 
(WINAPI *pGetMappedFileName)(
    HANDLE          hProcess,
    LPVOID          lpv,
    LPTSTR          lpFilename,
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

#if !defined( MD_x64 ) && defined( WOW )
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
