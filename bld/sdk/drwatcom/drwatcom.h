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


#define _WINDOWS_16_
#include <windows.h>
#include <toolhelp.h>
#include "pmdbg.h"
#include "const.h"
#include "intdlg.h"
#include "stat.h"
#include "stackt.h"
#include "log.h"
#include "dmptask.h"
#include "taskctl.h"
#include "mem.h"
#include "mythelp.h"
#include "types.h"
#include "dlgmod.h"
#include "font.h"
#include "seginfo.h"
#include "segmem.h"
#include "ismod32.h"
#include "dbg386.h"
#include "memwnd.h"
#include "listbox.h"
#include "rcstr.h"
#include "ldstr.h"
#include "dbgopt.h"
#include "rvalue.h"


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
    LOGFL_LOG_IF_RESTART,
    LOGFL_AUTOLOG,
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
extern struct interrupt_struct  IntData;
extern HANDLE                   Instance;
extern WORD                     DeadTask;
extern TASKENTRY                DTTaskEntry;
extern MODULEENTRY              DTModuleEntry;
extern HWND                     MainWindow;
extern WORD                     CharSizeX,CharSizeY;
extern char                     *AppName;
extern BOOL                     WDebug386;
extern DWORD                    WindowsFlags;
extern loginfo                  LogInfo;
extern dumpany                  DumpAny;
extern HANDLE                   ProgramTask;
extern int                      MinAddrSpaces;
extern HWND                     DumpDialogHwnd;
extern BOOL                     WasFault;
extern BOOL                     IsWin32App;
extern WORD                     Win32CS;
extern DWORD                    Win32InitialEIP;
extern WORD                     Win32DS;
extern BOOL                     FaultHandlerEntered;
extern BOOL                     StatShowSymbols;
extern char                     DebuggerOpts[128];
extern int                      DumpHow;
extern BOOL                     AlwaysRespondToDebugInChar;
extern BOOL                     AlertOnWarnings;
extern LBoxHdl                  *ListBox;

/*
 * function prototypes
 */
extern void FAR PASCAL DirectedYield( HANDLE );

/* debug.c */
WORD NumToAddr( HANDLE modhandle, WORD num );
DWORD GetFaultString( int intnum, char *buff );

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

/* inth.asm */
void FAR PASCAL IntHandler( void );

/* log.c */
void StartLogFile( void );
void FinishLogFile( void );
void GetProfileInfo( void );
void PutProfileInfo( void );
void DoLogDialog( HWND );
void MakeLog( BOOL );

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
BOOL FindSymbol( ADDRESS *addr, syminfo *si );
void SymFileClose( void );
RVALUE FindWatSymbol( ADDRESS *addr, syminfo *si, int getsrcinfo );
BOOL InitSymbols( void );
void FiniSymbols( void );
BOOL LoadDbgInfo( void );

/* win32app.c */
BOOL CheckIsWin32App( HANDLE );
BOOL DoGlobalEntryModule( GLOBALENTRY *ge, HANDLE hmod, WORD seg );
BOOL DoGlobalEntryHandle( GLOBALENTRY *ge, HANDLE hmem );

/* winmain.c */
#pragma aux Death aborts;
void Death( DWORD msgid, ... );

/* winproc.c */
extern LONG __export FAR PASCAL WindowProc( HWND, int, WORD, LONG );

/* drwatcom.c */
void Alert( void );
void ClearAlert( void );

/* lddips.c */

void ShowDIPStatus( HWND hwnd );
BOOL LoadTheDips( void );
void FiniDipMsgs( void );
