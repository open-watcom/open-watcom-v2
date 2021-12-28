/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include "commonui.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "bool.h"
#include "pmdbg.h"
#include "const.rh"
#include "intdlg.rh"
#include "stat.rh"
#include "stackt.rh"
#include "drwatlog.rh"
#include "dmptask.rh"
#include "taskctl.rh"
#include "cguimem.h"
#include "mythelp.h"
#include "types.h"
#include "dlgmod.h"
#include "font.h"
#include "segmem.h"
#include "ismod32.h"
#include "winintrf.h"
#include "di386cli.h"
#include "rcstr.grh"
#include "ldstr.h"
#include "dbgopt.rh"
#include "rvalue.h"
#include "listbox.h"


#define INT_PTR int

#define MAX_SYM_NAME    128
#define MAX_FILE_NAME   144

typedef struct {
    WORD                segnum;
    DWORD               symoff;
    WORD                linenum;
    char                name[MAX_SYM_NAME];
    char                filename[MAX_FILE_NAME];
} syminfo;

typedef struct {
    registers           regs;
    DWORD               oldEAX;
    WORD                intnumber;
    WORD                handle;
    WORD                deadtask;
} intstuff;

enum {
    LOGFL_STACK_TRACE=0,
    LOGFL_TASKS,
    LOGFL_MODULES,
    LOGFL_GDI,
    LOGFL_USER,
    LOGFL_MEM,
    LOGFL_MOD_SEGMENTS,
    LOGFL_AUTOLOG,
    LOGFL_NOTE,
    LOGFL_MAX
};

#define MAX_FNAME       144
typedef struct {
    char        flags[LOGFL_MAX+1];
    char        filename[MAX_FNAME];
    char        disasmbackup,disasmlines;
    DWORD       maxlogsize;
} loginfo;

typedef struct {
    BOOL        dump_pending;
    WORD        CS;
    DWORD       EIP;
    WORD        taskid;
    char        oldbyte;
} dumpany;

/*
 * global variables
 */
extern interrupt_struct         IntData;
extern HANDLE                   Instance;
extern HTASK                    DeadTask;
extern TASKENTRY                DTTaskEntry;
extern MODULEENTRY              DTModuleEntry;
extern HWND                     MainWindow;
extern WORD                     CharSizeX,CharSizeY;
extern char                     *AppName;
extern DWORD                    WindowsFlags;
extern loginfo                  LogInfo;
extern dumpany                  DumpAny;
extern HANDLE                   ProgramTask;
extern int                      MinAddrSpaces;
extern HWND                     DumpDialogHwnd;
extern bool                     WasFault32;
extern bool                     IsWin32App;
extern WORD                     Win32CS;
extern DWORD                    Win32InitialEIP;
extern WORD                     Win32DS;
extern bool                     FaultHandlerEntered;
extern bool                     StatShowSymbols;
extern char                     DebuggerOpts[128];
extern int                      DumpHow;
extern bool                     AlwaysRespondToDebugInChar;
extern bool                     AlertOnWarnings;
extern LBoxHdl                  *ListBox;
extern WORD                     ExceptionAction;

/*
 * function prototypes
 */

/* debug.c */
WORD NumToAddr( HMODULE modhandle, WORD num );
msg_id GetFaultString( int intnum, char *buff );

/* disasm.c */
unsigned Disassemble( ADDRESS *addr, char *buff, int addbytes );
void InstructionBackup( int cnt, ADDRESS *addr );
void PreviousInstruction( ADDRESS *addr );
void RegDrWatcomDisasmRtns( void );

/* dump.c */
void DumpTask( HWND );
void DumpATask( HWND );
void KillATask( HWND );
void DoDump( HWND );

/* heapex.c */
void DoHeapDialog( HWND );

/* drwatlog.c */
void StartLogFile( void );
void FinishLogFile( void );
void GetProfileInfo( void );
void PutProfileInfo( void );
void DoLogDialog( HWND );
void EraseLog( void );
void MakeLog( bool );

/* notify.c */
extern BOOL __export FAR PASCAL NotifyHandler( WORD, DWORD );
void HandleNotify( WORD, DWORD );

/* stack.c */
void StartStackTraceDialog( HWND wnd );

/* stat.c */
void DisplayAsmLines( HWND hwnd, ADDRESS *paddr, ADDRESS *flagaddr, int idlo, int idhi, int sbid );
void ScrollAsmDisplay( HWND hwnd, WORD wparam, ADDRESS *paddr, ADDRESS *flagaddr, int idlo, int idhi, int sbid );
void DoStatDialog( HWND );

/* sym.c */
bool FindSymbol( ADDRESS *addr, syminfo *si );
bool FindWatSymbol( ADDRESS *addr, syminfo *si, bool getsrcinfo );
bool InitSymbols( void );
void FiniSymbols( void );
bool LoadDbgInfo( void );
void UnloadDbgInfo( void );

/* win32app.c */
bool CheckIsWin32App( HANDLE );
bool DoGlobalEntryModule( GLOBALENTRY *ge, HMODULE hmod, WORD seg );
bool DoGlobalEntryHandle( GLOBALENTRY *ge, HANDLE hmem );

/* winmain.c */
#pragma aux Death __aborts;
void Death( msg_id msgid, ... );

/* winproc.c */
extern LONG __export FAR PASCAL WindowProc( HWND, UINT, WPARAM, LPARAM );

/* drwatcom.c */
void Alert( void );
void ClearAlert( void );

/* lddips.c */

void ShowDIPStatus( HWND hwnd );
bool LoadTheDips( void );
void FiniDipMsgs( void );
bool IsDip( HINSTANCE );

/* lognote.c */
void AnotateLog( HWND hwnd, HANDLE Instance, void (*fn)(char *)  );
