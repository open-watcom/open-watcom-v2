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


#include "wdebug.h"
#include "stdwin.h"
BYTE                    DLLLoadSaveByte;
WORD                    DLLLoadIP;
WORD                    DLLLoadCS;
BOOL                    DLLLoadExpectingInt1;
BOOL                    TraceOn;
HTASK                   DebuggerTask;
HTASK                   DebugeeTask;
HINSTANCE               DebugeeInstance;
struct interrupt_struct IntResult;
volatile debugger_state DebuggerState=ACTIVE;
const char __based(__segname("_CONST")) ExtensionList[] = { ".com\0.exe\0" };
break_point             StopNewTask;
DWORD                   SystemDebugState;
unsigned_8              FPUType;
volatile restart_opts   AppMessage;
DWORD                   TerminateCSIP;
HWND                    DebuggerWindow;
BOOL                    FaultHandlerEntered;
int                     SaveStdIn=NIL_HANDLE;
int                     SaveStdOut=NIL_HANDLE;
BOOL                    WDebug386;
HMODULE                 DebugeeModule;
void                    (FAR PASCAL *DoneWithInterrupt)( LPVOID );
int                     (FAR PASCAL *GetDebugInterruptData)( LPVOID );
void                    (FAR PASCAL *ResetDebugInterrupts32)( void );
int                     (FAR PASCAL *SetDebugInterrupts32)( void );
void                    (FAR PASCAL *DebuggerIsExecuting)( int );
BOOL                    DebugDebugeeOnly;
HANDLE                  TaskAtFault;
BOOL                    InSoftMode;
WORD                    CSAlias;
WORD                    SegmentToAccess;
BOOL                    PendingTrap;
LPVOID                  HookRtn;
WORD                    HardModeRequired;
WORD                    ForceHardMode;
BOOL                    InputLocked;
char                    OutBuff[MAX_STR];
int                     OutPos;
BOOL                    StopOnExtender;
BOOL                    LoadingDebugee;
BOOL                    Debugging32BitApp;
