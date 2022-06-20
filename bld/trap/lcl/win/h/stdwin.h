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


#define INCLUDE_TOOLHELP_H
#include <windows.h>
#include "trpimp.h"
#include "trpcomm.h"
#include "trperr.h"
#include "packet.h"
#include "winintrf.h"
#include "brkptcpu.h"


#define MAGIC_COOKIE    0x66600666L
typedef enum {
    FAULT_HIT,
    START_BP_HIT,
    TASK_LOADED,
    TASK_ENDED,
    ASYNCH_STOP,
    OUT_STR,
    GET_CHAR,
    DLL_LOAD,
    DLL_LOAD32,
} private_msg;

typedef enum {
    ACTIVE,
    WAITING_FOR_TASK_LOAD,
    LOADING_DEBUGEE,
    WAITING_FOR_BREAKPOINT,
    RUNNING_DEBUGEE,
    GOING_TO_32BIT_START,
    RELEASE_DEBUGEE,
    KILLING_DEBUGEE
} debugger_state;

#define NIL_HANDLE      -1

#define MAX_STR 512

typedef struct {
    addr48_ptr          loc;
    WORD                segment_number;
    opcode_type         old_opcode;
    char                hard_mode:1;
    char                in_use:1;
} break_point;

#define SIG_OFF         0
#define SIG_SIZE        4
#define Align4K( x ) (((x)+0xfffL) & ~0xfffL )

struct fp_state {
    unsigned char fp[108];
};

typedef struct dll_info {
    addr48_ptr          addr;
    opcode_type         old_opcode;
    bool                expecting_int1;
} dll_info;

#define SIG_OFF         0
#define SIG_SIZE        4

extern const unsigned short     __based(__segname("_CONST")) win386sig[SIG_SIZE / sizeof( short )];
extern const unsigned short     __based(__segname("_CONST")) win386sig2[SIG_SIZE / sizeof( short )];

/*
 * global variables
 */
extern dll_info                 DLLLoad;
extern unsigned_8               FPUType;
extern HWND                     DesktopWindow;
extern HINSTANCE                Instance;
extern HTASK                    DebuggerTask;
extern HTASK                    DebugeeTask;
extern HINSTANCE                DebugeeInstance;
extern DWORD                    WindowsFlags;
extern interrupt_struct         IntResult;
extern struct fp_state          FPResult;
extern volatile debugger_state  DebuggerState;
extern break_point              StopNewTask;
extern DWORD                    SystemDebugState;
extern DWORD                    TerminateCSIP;
extern HWND                     DebuggerWindow;
extern int                      ModuleTop;
extern int                      CurrentModule;
extern bool                     FaultHandlerEntered;
extern int                      SaveStdIn;
extern int                      SaveStdOut;
extern HMODULE                  DebugeeModule;
extern bool                     WasInt32;
extern BOOL                     DebugDebugeeOnly;
extern HTASK                    TaskAtFault;
extern WORD                     Win386Sig[];
extern WORD                     Win386SigRev[];
//extern FARPROC                  SubClassProcInstance;
extern bool                     HardModeRequired;
extern bool                     InputLocked;
extern bool                     ForceHardMode;
extern bool                     InSoftMode;
extern WORD                     CSAlias;
extern WORD                     SegmentToAccess;
extern BOOL                     PendingTrap;
extern event_hook_fn            *HookRtn;
extern BOOL                     IsRFX;
extern char                     OutBuff[MAX_STR];
extern int                      OutPos;
extern BOOL                     StopOnExtender;
extern BOOL                     LoadingDebugee;
extern BOOL                     TraceOn;
extern BOOL                     Debugging32BitApp;

/*
 * function prototypes
 */
/* accbrwat.c */
BOOL IsOurBreakpoint( WORD sel, DWORD off );
void ResetBreakpoints( WORD sel );
BOOL SetDebugRegs( void );
void ClearDebugRegs( void );
DWORD GetDR6( void );
BOOL CheckWatchPoints( void );
bool IsWatch( void );

/* asyhook.c */
extern void InitASynchHook( void );
extern void FiniASynchHook( void );
extern void HandleAsynch( void );

/* accmap.c */
void AddModuleLoaded( HANDLE mod, BOOL );
void AddDebugeeModule( void );
void AddAllCurrentModules( void );
BOOL HasSegAliases( void );

/* accmisc.c */
BOOL IsSegSize32( WORD seg );

/* accredir.c */
void ExecuteRedirect( void );

/* accrun.c */
void SingleStepMode( void );

/* initfini.c */
void SetInputLock( bool lock_status );

/* dbgeemsg.c */
void EnterSoftMode( void );
void ExitSoftMode( void );
//long FAR PASCAL SubClassProc( HWND hwnd, unsigned message, WORD wparam, LONG lparam );

/* dbghook.c */
void FiniDebugHook( void );
void InitDebugHook( void );

/* int.asm */
void FAR PASCAL IntHandler( void );

/* mem.c */
DWORD WriteMemory( addr48_ptr *addr, LPVOID buff, DWORD size );
DWORD ReadMemory( addr48_ptr *addr, LPVOID buff, DWORD size );

/* notify.c */
BOOL FAR PASCAL NotifyHandler( WORD id, DWORD data );

/* debug output */
extern unsigned     DbgFlags;
#ifdef DEBUG
#define OUT_BREAK       0x0001
#define OUT_ERR         0x0002
#define OUT_HOOK        0x0004
#define OUT_INIT        0x0008
#define OUT_LOAD        0x0010
#define OUT_MAP         0x0020
#define OUT_MSG         0x0040
#define OUT_REQ         0x0080
#define OUT_RUN         0x0100
#define OUT_SOFT        0x0200
#define OUT_TEMP        0x0400
extern void MyOut( unsigned f, char *, ... );
extern void MyClearScreen( void );
#define Out( a ) MyOut a
#define ClearScreen MyClearScreen
#else
#define Out( a )
#define ClearScreen()
#endif
