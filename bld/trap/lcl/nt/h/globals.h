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


TRPGLOBAL char                  ExtensionList[] TRPGLOBINIT( ".com\0.exe\0" );
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
TRPGLOBAL BOOL                  IsWOW;
TRPGLOBAL BOOL                  IsDOS;
TRPGLOBAL BOOL                  IsWin32s;
TRPGLOBAL BOOL                  IsWin95;
TRPGLOBAL BOOL                  IsWinNT;
TRPGLOBAL DEBUG_EVENT           DebugEvent;
TRPGLOBAL BOOL                  UseVDMStuff;
TRPGLOBAL char                  CurrEXEName[MAX_PATH];
TRPGLOBAL BOOL                  DidWaitForDebugEvent;
TRPGLOBAL BOOL                  Slaying;
TRPGLOBAL HWND                  DebuggerWindow;
TRPGLOBAL DWORD                 LastDebugEventTid;
TRPGLOBAL BOOL                  BreakOnKernelMessage;
TRPGLOBAL BOOL                  PendingProgramInterrupt;
TRPGLOBAL char                  *MsgPrefix TRPGLOBINIT( NULL );

#ifdef WOW
TRPGLOBAL wow_info              WOWAppInfo;
#else
typedef void    *LPMODULEENTRY;
typedef void    *LPVDMCONTEXT;
typedef void    (WINAPI *DEBUGEVENTPROC)();
typedef BOOL    (WINAPI *PROCESSENUMPROC)( DWORD, DWORD, LPARAM );
#endif
TRPGLOBAL
HANDLE
(WINAPI*pOpenThread)(
    DWORD
);

TRPGLOBAL
BOOL
(WINAPI*pVDMModuleFirst)(
    HANDLE          hProcess,
    HANDLE          hThread,
    LPMODULEENTRY   lpModuleEntry,
    DEBUGEVENTPROC  lpEventProc,
    LPVOID          lpData
);

TRPGLOBAL
BOOL
(WINAPI*pVDMModuleNext)(
    HANDLE          hProcess,
    HANDLE          hThread,
    LPMODULEENTRY   lpModuleEntry,
    DEBUGEVENTPROC  lpEventProc,
    LPVOID          lpData
);

TRPGLOBAL
INT
(WINAPI*pVDMEnumProcessWOW)(
    PROCESSENUMPROC fp,
    LPARAM          lparam
);

TRPGLOBAL
BOOL
(WINAPI*pVDMProcessException)(
    LPDEBUG_EVENT   lpDebugEvent
    );


TRPGLOBAL
BOOL
(WINAPI*pVDMGetModuleSelector)(
    HANDLE          hProcess,
    HANDLE          hThread,
    UINT            wSegmentNumber,
    LPSTR           lpModuleName,
    LPWORD          lpSelector
);


TRPGLOBAL
BOOL
(WINAPI*pVDMGetThreadContext)(
    LPDEBUG_EVENT   lpDebugEvent,
    LPVDMCONTEXT    lpVDMContext
);

TRPGLOBAL
BOOL
(WINAPI*pVDMSetThreadContext)(
    LPDEBUG_EVENT   lpDebugEvent,
    LPVDMCONTEXT    lpVDMContext
);
#undef TRPGLOBAL
#undef TRPGLOBINIT
