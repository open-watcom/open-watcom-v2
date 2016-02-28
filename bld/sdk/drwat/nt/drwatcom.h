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


#include <stdlib.h>
#include "commonui.h"
#include "listbox.h"
#include "font.h"
#include "procctl.h"
#include "stat.h"
#include "dlgmod.h"
#include "reg.h"
#include "machtype.h"
#include "mem.h"
#include "rcstr.gh"
#include "ldstr.h"
#include "rvalue.h"
#include "madregs.h"

#define MAX_CMDLINE             256

#define LISTBOX_1               200

#define TOTAL_MEM_STR   "All Images"
#define MEM_WALKER_HEADER       \
  "  Base   AllocBase   Size   Prot   State    Object    Image"
// ******** ********  ******** **** ********** ********* **************

//#define DR_NEW_INST           ( WM_USER )
#define DR_TASK_LIST_CHANGE     ( WM_USER + 19 )
#define DR_DEBUG_EVENT          ( WM_USER + 20 )

#define CHILD_L_CLICK           ( WM_USER + 21 )
#define REG_STRING_DESELECTED   ( WM_USER + 22 )
#define REG_STRING_SELECTED     ( WM_USER + 23 )
#define UPDATE_REG_LIST         ( WM_USER + 24 )
#define HIDE_REG_LIST           ( WM_USER + 25 )
#define UNHIDE_REG_LIST         ( WM_USER + 26 )
#define CHILD_R_CLICK           ( WM_USER + 27 )
#define STAT_FOREGROUND         ( WM_USER + 28 )
#define STAT_MAD_NOTIFY         ( WM_USER + 29 )


#define MAX_SYM_NAME    128
#define MAX_FILE_NAME   144

typedef struct {
    DWORD               type;
    union {
        DWORD           pid;
        char            *path;
    }                   info;
    void                (*errhdler)( void * );
} ProcAttatchInfo;

typedef struct {
    DWORD               action;
    DEBUG_EVENT         dbginfo;
}CommunicationBuffer;

typedef struct {
    WORD                segnum;
    DWORD               symoff;
    WORD                linenum;
    char                name[MAX_SYM_NAME];
    char                filename[MAX_FILE_NAME];
} syminfo;

typedef struct {
    char                logname[_MAX_PATH];
    char                editor[_MAX_PATH];
    char                *editor_cmdline;
    BOOL                log_process;
    BOOL                log_stacktrace;
    BOOL                log_modules;
    BOOL                log_mem_manager;
    BOOL                log_mem_dmp;
    BOOL                query_notes;
    BOOL                autolog;
    DWORD               asm_cnt;
    DWORD               asm_bkup;
    DWORD               max_flen;
} LogInfo;

typedef struct {
    DEBUG_EVENT         *dbinfo;
    int                 rc;
    int                 action;
    mad_registers       *regs;
    address             init_ip;
    ProcNode            *procinfo;
    ThreadNode          *threadinfo;
    ModuleNode          *module;
    BOOL                got_dbginfo;
}ExceptDlgInfo;

typedef struct {
    BOOL        auto_attatch;
    WORD        exception_action;
    BOOL        continue_exception;
}ConfigInfo;

typedef struct {
    MEMORY_BASIC_INFORMATION    mbi;
    char                        *modname;
    char                        *objname;
    char                        data[1];  /* dynamic array */
}MemListItem;

typedef struct {
    DWORD                       allocated;
    DWORD                       used;
    MemListItem                 **data;
}MemListData;

typedef enum {
    RETCD_PROCESS,
    RETCD_THREAD
}RetCodeTypes;

extern HANDLE           Instance;
extern HWND             MainHwnd;
extern LBoxHdl          *MainLBox;
extern char             *AppName;
extern LogInfo          LogData;
extern HWND             ThreadDlg;
extern ConfigInfo       ConfigData;

extern BOOL             StatShowSymbols;
extern HWND             StatHdl;
extern system_config    SysConfig;
extern HANDLE           ProcessHdl;

/* drproc.c */
LONG CALLBACK MainWindowProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
void ClearAlert( void );
void Alert( void );

/* handler.c */
DWORD DebugEventHandler( DEBUG_EVENT *dbinfo );

/* param.c */
void ProcessCommandLine( char *cmdline );

/* taskctl.c */
BOOL InitProcessCtl( void );
void CallProcCtl( DWORD event, void *info, void (*hdler)(void *) );

/* proclist.c */
ProcNode *FindProcess( DWORD process );
void GetProcName( DWORD process, char *name );
void AddThread( DWORD procid, DWORD threadid, HANDLE threadhdl );
void AddProcess( DWORD procid, HANDLE prochdl, DWORD threadid,
                 HANDLE threadhdl );
ProcNode *FindProcess( DWORD process );
ThreadNode *FindThread( ProcNode *procnode, DWORD threadid );
void RemoveThread( DWORD process, DWORD threadid );
void RemoveProcess( DWORD process );
void DisplayProcList( void );
void AddProcessName( DWORD procid, char *name );
ProcNode *GetNextOwnedProc( ProcNode *cur );
void AddModule( DWORD procid, HANDLE fhdl, DWORD base, char *name );
void RemoveModule( DWORD procid, DWORD base );
//void MapAddress( addr_ptr *addr, ModuleNode *mod );
ModuleNode *ModuleFromAddr( ProcNode *proc, void *addr );
ModuleNode *GetFirstModule( ProcNode *procinfo );
ModuleNode *GetNextModule( ModuleNode *modinfo );

/* autoget.c */
void InitAutoAttatch( void );

/* profile.c */
void GetProfileInfo( void );
void PutProfileInfo( void );

/* fault.c */
ExceptDlgInfo * FaultGetExceptDlgInfo( HWND fault );
int HandleException( DEBUG_EVENT *dbinfo );
void FormatException( char *buf, DWORD code );

/* stat.c */
mad_registers * StatGetMadRegisters( HWND stat );
void SetProcessInfo( HANDLE hdl, DWORD procid );
int DoStatDialog( HWND hwnd );

/* disasm.c */
void SetDisasmInfo( HANDLE hdl, ModuleNode *mod );
int InstructionFoward(int cnt, address *addr);
int InstructionBackward( int cnt, address *addr);
void Disassemble( address *addr, char *buff, int addbytes,unsigned max );
BOOL AllocMadDisasmData(void);
void DeAllocMadDisasmData(void);

/* log.c */
void EraseLog( void );
void ViewLog( void );
void MakeLog( ExceptDlgInfo *faultinfo );
void SetLogOptions( HWND hwnd );
void CheckLogSize( void );

/* lognote.c */
void AnotateLog( HWND hwnd, HANDLE Instance, void (*fn)(char *)  );

/* thrdctl.c */
BOOL CALLBACK ThreadCtlProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
BOOL GetRetCode( HWND parent, RetCodeTypes type, DWORD id, DWORD *rc );
BOOL ParseNumeric( char *buf, BOOL signed_val, DWORD *val );

/* reg.c */
BOOL RefreshInfo( void );
void RefreshCostlyInfo( void );
BOOL GetProcessInfo( DWORD pid, ProcStats *info );
BOOL GetNextProcess( ProcList *info, ProcPlace *place, BOOL first );
BOOL GetNextThread( ThreadList *info, ThreadPlace *place,
                    DWORD pid, BOOL first );
BOOL GetThreadInfo( DWORD pid, DWORD tid, ThreadStats *info );
void FreeModuleList( char **ptr, DWORD cnt );
char **GetModuleList( DWORD pid, DWORD *cnt );
void InitReg( void );
#ifndef CHICAGO
BOOL GetMemInfo( DWORD procid, MemInfo *info );
BOOL GetImageMemInfo( DWORD procid, char *imagename, MemByType *imageinfo );
#endif

/* sym.c */
BOOL InitDip( void );
BOOL GetLineNum( address *addr, char *fname, DWORD bufsize, DWORD *line );
BOOL GetSymbolName( address *addr, char *name, DWORD *symoff );
BOOL LoadDbgInfo( ModuleNode *mod );
void UnloadDbgInfo( ModuleNode *mod );

/* memory.c */
#ifndef CHICAGO
void DoMemDlg( HWND hwnd, DWORD procid );
#endif

/* memview.c */
BOOL RegisterMemWalker( void );
void WalkMemory( HWND parent, HANDLE hdl, DWORD procid );
void FormatMemListEntry( char *buf, MemListItem *item );
void RefreshMemList( DWORD procid, HANDLE prochdl, MemListData *proclist );
void FreeMemList( MemListData *info );

/* pefile.c */
BOOL GetSegmentList( ModuleNode *node );
char *GetModuleName( HANDLE fhdl );
BOOL GetModuleSize( HANDLE fhdl, DWORD *size );
ObjectInfo *GetModuleObjects( HANDLE fhdl, DWORD *num_objects );

/* disasm.c */
RVALUE FindWatSymbol( address *addr, syminfo *si, int getsrcinfo );

/* lddips.c */
void ShowDIPStatus( HWND hwnd );
BOOL LoadTheDips( void );
void FiniDipMsgs( void );
