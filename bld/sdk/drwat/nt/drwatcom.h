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


#if defined( _M_IX86 )
#define MD_x86
#elif defined __AXP__
#define MD_axp
#endif

#include "commonui.h"
#include <winperf.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "bool.h"
#include "font.h"
#include "procctl.h"
#include "stat.rh"
#include "dlgmod.h"
#include "reg.h"
#include "machtype.h"
#include "cguimem.h"
#include "rcstr.grh"
#include "ldstr.h"
#include "rvalue.h"
#include "madregs.h"
#include "listbox.h"


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

/*
 * Windows NT must use OS loader handle for I/O file function for DLL/multi-thread support
 * For DIG client functions appropriate Windows HANDLE oriented I/O functions must be used
 * !!! ISO or POSIX functions must not be used !!!
 *
 * below is appropriate macros for mapping between ISO stream file handle and Windows file handle
 */
#define WH2FP(wh)       ((FILE *)(wh))
#define FP2WH(fp)       ((HANDLE)(fp))

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

extern bool             StatShowSymbols;
extern HWND             StatHdl;
extern system_config    SysConfig;
extern HANDLE           ProcessHdl;

/* drproc.c */
extern LONG         CALLBACK MainWindowProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
extern void         ClearAlert( void );
extern void         Alert( void );

/* handler.c */
extern DWORD        DebugEventHandler( DEBUG_EVENT *dbinfo );

/* param.c */
extern void         ProcessCommandLine( char *cmdline );

/* taskctl.c */
extern BOOL         InitProcessCtl( void );
extern void         CallProcCtl( DWORD event, void *info, void (*hdler)(void *) );

/* proclist.c */
extern ProcNode     *FindProcess( DWORD process );
extern void         GetProcName( DWORD process, char *name );
extern void         AddThread( DWORD procid, DWORD threadid, HANDLE threadhdl );
extern void         AddProcess( DWORD procid, HANDLE prochdl, DWORD threadid, HANDLE threadhdl );
extern ProcNode     *FindProcess( DWORD process );
extern ThreadNode   *FindThread( ProcNode *procnode, DWORD threadid );
extern void         RemoveThread( DWORD process, DWORD threadid );
extern void         RemoveProcess( DWORD process );
extern void         DisplayProcList( void );
extern void         AddProcessName( DWORD procid, char *name );
extern ProcNode     *GetNextOwnedProc( ProcNode *cur );
extern void         AddModule( DWORD procid, FILE *fp, DWORD base, char *name );
extern void         RemoveModule( DWORD procid, DWORD base );
//void MapAddress( addr_ptr *addr, ModuleNode *mod );
extern ModuleNode   *ModuleFromAddr( ProcNode *proc, void *addr );
extern ModuleNode   *GetFirstModule( ProcNode *procinfo );
extern ModuleNode   *GetNextModule( ModuleNode *modinfo );

/* autoget.c */
extern void         InitAutoAttatch( void );

/* profile.c */
extern void         GetProfileInfo( void );
extern void         PutProfileInfo( void );

/* fault.c */
extern ExceptDlgInfo *FaultGetExceptDlgInfo( HWND fault );
extern int          HandleException( DEBUG_EVENT *dbinfo );
extern void         FormatException( char *buf, DWORD code );

/* stat.c */
extern mad_registers *StatGetMadRegisters( HWND stat );
extern void         SetProcessInfo( HANDLE hdl, DWORD procid );
extern int          DoStatDialog( HWND hwnd );

/* disasm.c */
extern void         SetDisasmInfo( HANDLE hdl, ModuleNode *mod );
extern int          InstructionFoward(int cnt, address *addr);
extern int          InstructionBackward( int cnt, address *addr);
extern void         Disassemble( address *addr, char *buff, int addbytes,unsigned max );
extern bool         AllocMadDisasmData(void);
extern void         DeAllocMadDisasmData(void);

/* drwatlog.c */
extern void         EraseLog( void );
extern void         ViewLog( void );
extern void         MakeLog( ExceptDlgInfo *faultinfo );
extern void         SetLogOptions( HWND hwnd );
extern void         CheckLogSize( void );

/* lognote.c */
extern void         AnotateLog( HWND hwnd, HANDLE Instance, void (*fn)(char *)  );

/* thrdctl.c */
extern BOOL         CALLBACK ThreadCtlProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
extern BOOL         GetRetCode( HWND parent, RetCodeTypes type, DWORD id, DWORD *rc );
extern BOOL         ParseNumeric( char *buf, BOOL signed_val, DWORD *val );

/* reg.c */
extern BOOL         RefreshInfo( void );
extern void         RefreshCostlyInfo( void );
extern BOOL         GetProcessInfo( DWORD pid, ProcStats *info );
extern BOOL         GetNextProcess( ProcList *info, ProcPlace *place, BOOL first );
extern BOOL         GetNextThread( ThreadList *info, ThreadPlace *place, DWORD pid, BOOL first );
extern BOOL         GetThreadInfo( DWORD pid, DWORD tid, ThreadStats *info );
extern void         FreeModuleList( char **ptr, DWORD cnt );
extern char         **GetModuleList( DWORD pid, DWORD *cnt );
extern void         InitReg( void );
#ifndef CHICAGO
extern BOOL         GetMemInfo( DWORD procid, MemInfo *info );
extern BOOL         GetImageMemInfo( DWORD procid, char *imagename, MemByType *imageinfo );
#endif

/* sym.c */
extern bool         InitDip( void );
extern bool         GetLineNum( address *addr, char *fname, DWORD bufsize, DWORD *line );
extern bool         GetSymbolName( address *addr, char *name, DWORD *symoff );
extern bool         LoadDbgInfo( ModuleNode *mod );
extern void         UnloadDbgInfo( ModuleNode *mod );

/* memory.c */
#ifndef CHICAGO
extern void         DoMemDlg( HWND hwnd, DWORD procid );
#endif

/* memview.c */
extern bool         RegisterMemWalker( void );
extern void         WalkMemory( HWND parent, HANDLE hdl, DWORD procid );
extern void         FormatMemListEntry( char *buf, MemListItem *item );
extern void         RefreshMemList( DWORD procid, HANDLE prochdl, MemListData *proclist );
extern void         FreeMemList( MemListData *info );

/* pefile.c */
extern BOOL         GetSegmentList( ModuleNode *node );
extern char         *GetModuleName( FILE *fp );
extern BOOL         GetModuleSize( FILE *fp, DWORD *size );
extern ObjectInfo   *GetModuleObjects( FILE *fp, DWORD *num_objects );

/* disasm.c */
extern bool         FindWatSymbol( address *addr, syminfo *si, bool getsrcinfo );

/* lddips.c */
extern void         ShowDIPStatus( HWND hwnd );
extern bool         LoadTheDips( void );
extern void         FiniDipMsgs( void );
