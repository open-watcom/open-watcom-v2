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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <stddef.h>
#include "stdwin.h"
#include "trpsys.h"
#include "dbgrmsg.h"


const unsigned short    __based(__segname("_CONST")) win386sig[SIG_SIZE / sizeof( short )] = { 0xDEAD,0xBEEF };
const unsigned short    __based(__segname("_CONST")) win386sig2[SIG_SIZE / sizeof( short )] = { 0xBEEF,0xDEAD };

dll_info                DLLLoad;
BOOL                    TraceOn;
HTASK                   DebuggerTask;
HTASK                   DebugeeTask;
HINSTANCE               DebugeeInstance;
interrupt_struct        IntResult;
volatile debugger_state DebuggerState = ACTIVE;
break_point             StopNewTask;
DWORD                   SystemDebugState;
unsigned_8              FPUType;
volatile appl_action    AppMessage;
DWORD                   TerminateCSIP;
HWND                    DebuggerWindow;
bool                    FaultHandlerEntered;
int                     SaveStdIn = NIL_HANDLE;
int                     SaveStdOut = NIL_HANDLE;
bool                    WDebug386 = false;
HMODULE                 DebugeeModule;
BOOL                    DebugDebugeeOnly;
HTASK                   TaskAtFault;
bool                    InSoftMode;
WORD                    CSAlias;
WORD                    SegmentToAccess;
BOOL                    PendingTrap;
event_hook_fn           *HookRtn;
bool                    HardModeRequired;
bool                    ForceHardMode;
bool                    InputLocked = false;
char                    OutBuff[MAX_STR];
int                     OutPos;
BOOL                    StopOnExtender;
BOOL                    LoadingDebugee;
BOOL                    Debugging32BitApp;
