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


#define MAX_PROC_NAME           50

#include <stdlib.h>
#include "windows.h"
#include "listbox.h"
#include "font.h"
#include "procctl.h"
#include "types.h"
#include "stat.h"
#include "dlgmod.h"
#include "reg.h"
#include "machtype.h"
#include "mem.h"
#include "rcstr.h"
#include "ldstr.h"
#include "rvalue.h"


#define MAX_CMDLINE             256

#define LISTBOX_1               200

#define TOTAL_MEM_STR   "All Images"
#define MEM_WALKER_HEADER       \
  "  Base   AllocBase   Size   Prot   State    Object    Image"
// ******** ********  ******** **** ********** ********* **************

#define DR_NEW_INST             ( WM_USER )
#define DR_TASK_LIST_CHANGE     ( WM_USER + 1 )
#define DR_DEBUG_EVENT          ( WM_USER + 2 )

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
    BOOL                log_restarts;
    BOOL                log_chains;
    BOOL                log_events;
    DWORD               asm_cnt;
    DWORD               asm_bkup;
    DWORD               max_flen;
} LogInfo;

typedef struct {
    DEBUG_EVENT         *dbinfo;
    int                 rc;
    int                 action;
    CONTEXT             context;
    ProcNode            *procinfo;
    ThreadNode          *threadinfo;
    ModuleNode          *module;
    BOOL                got_dbginfo;
}ExceptDlgInfo;

typedef struct {
    BOOL        auto_attatch;
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

extern BOOL                     StatShowSymbols;
extern struct interrupt_struct  IntData;

/* drproc.c */
LONG CALLBACK MainWindowProc( HWND hwnd, UINT msg, UINT wparam, LONG lparam );
void ClearAlert( void );
void Alert( void );

/* handler.c */
DWORD DebugEventHandler( DEBUG_EVENT *dbinfo );

/* param.c */
void ProcessCommandLine( char *cmdline );

/* taskctl.c */
BOOL InitProcessCtl( void );
void CallProcCtl( DWORD event, void *info,
                  void (*hdler)(ProcAttatchInfo *) );

/* proclist.c */
ProcNode *FindProcess( DWORD process );
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
void MapAddress( addr_ptr *addr, ModuleNode *mod );
ModuleNode *ModuleFromAddr( ProcNode *proc, void *addr );
ModuleNode *GetFirstModule( ProcNode *procinfo );
ModuleNode *GetNextModule( ModuleNode *modinfo );

/* autoget.c */
void InitAutoAttatch( void );

/* profile.c */
void GetProfileInfo( void );
void PutProfileInfo( void );

/* fault.c */
int HandleException( DEBUG_EVENT *dbinfo );
void FormatException( char *buf, DWORD code );

/* stat.c */
void SetProcessInfo( HANDLE hdl, DWORD procid );
void DoStatDialog( HWND hwnd );

/* disasm.c */
void RegDrWatcomDisasmRtns( void );
void SetDisasmInfo( HANDLE hdl, ModuleNode *mod );
void InstructionBackup( int cnt, ADDRESS *addr );
void PreviousInstruction( ADDRESS *addr );
unsigned Disassemble( ADDRESS *addr, char *buff, int addbytes );

/* log.c */
void EraseLog( void );
void ViewLog( void );
void MakeLog( ExceptDlgInfo *faultinfo );
void SetLogOptions( HWND hwnd );
void CheckLogSize( void );

/* lognote.c */
void AnotateLog( HWND hwnd, HANDLE Instance, void (*fn)(char *)  );

/* thrdctl.c */
BOOL CALLBACK ThreadCtlProc( HWND hwnd, UINT msg, UINT wparam, DWORD lparam );
BOOL GetRetCode( HWND parent, RetCodeTypes type, DWORD id, DWORD *rc );
BOOL ParseNumeric( char *buf, BOOL signed_val, DWORD *val );

/* reg.c */
BOOL RefreshInfo( void );
BOOL RefreshCostlyInfo( void );
BOOL GetProcessInfo( DWORD pid, ProcStats *info );
BOOL GetNextProcess( ProcList *info, ProcPlace *place, BOOL first );
BOOL GetNextThread( ThreadList *info, ThreadPlace *place,
                    DWORD pid, BOOL first );
BOOL GetThreadInfo( DWORD pid, DWORD tid, ThreadStats *info );
void FreeModuleList( char **ptr, DWORD cnt );
char **GetModuleList( DWORD pid, DWORD *cnt );
void InitReg( void );
BOOL GetMemInfo( DWORD procid, MemInfo *info );
BOOL GetImageMemInfo( DWORD procid, char *imagename, MemByType *imageinfo );


/* sym.c */
BOOL InitDip( void );
BOOL GetLineNum( ModuleNode *mod, DWORD offset, char *fname,
                 DWORD bufsize, DWORD *line );
BOOL GetSymbolName( ModuleNode *mod,DWORD offset, char *name, DWORD *symoff );
void MapAddress( addr_ptr *addr, ModuleNode *mod );
BOOL LoadDbgInfo( ModuleNode *mod );
void UnloadDbgInfo( ModuleNode *mod );

/* memory.c */
void DoMemDlg( HWND hwnd, DWORD procid );

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
RVALUE FindWatSymbol( ADDRESS *addr, syminfo *si, int getsrcinfo );

/* lddips.c */
void ShowDIPStatus( HWND hwnd );
BOOL LoadTheDips( void );
void FiniDipMsgs( void );
