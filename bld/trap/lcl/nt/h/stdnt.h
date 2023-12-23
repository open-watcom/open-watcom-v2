/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  NT trap dll internal interfaces
*
****************************************************************************/


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#include <tlhelp32.h>
#include "madconf.h"
#include "brkptcpu.h"
#include "trpimp.h"
#include "trpcomm.h"
#include "packet.h"
#include "exepe.h"
#include "exeos2.h"
#include "exedos.h"
#ifdef WOW
#if MADARCH & MADARCH_X86
#include "vdmdbg.h"
#endif
#endif


#undef GetThreadContext
#define GetThreadContext Dont_call_GetThreadContext_directly__Call_MyGetThreadContext_instead
#undef SetThreadContext
#define SetThreadContext Dont_call_SetThreadContext_directly__Call_MySetThreadContext_instead

    #define MYCONTEXT           CONTEXT
    #define MYCONTEXT_CONTROL   CONTEXT_CONTROL;

#if MADARCH & MADARCH_X64
    /*
     * position in Windows CONTEXT,
     * it is offset in FXSAVE/FXRSTOR memory structure
     */
    #define MYCONTEXT_MXCSR     24
    #define MYCONTEXT_XMM       (10 * 16)
#else
    /*
     * position in Windows CONTEXT,
     * it is offset in FXSAVE/FXRSTOR memory structure
     */
    #define MYCONTEXT_MXCSR     24
    #define MYCONTEXT_XMM       (10 * 16)
#endif

#if MADARCH & MADARCH_X86
    #define MYCONTEXT_TO_USE    (CONTEXT_FULL | CONTEXT_FLOATING_POINT \
                                    | CONTEXT_DEBUG_REGISTERS | CONTEXT_EXTENDED_REGISTERS)
#elif MADARCH & MADARCH_X64
    #define MYCONTEXT_TO_USE    (CONTEXT_FULL | CONTEXT_FLOATING_POINT | CONTEXT_DEBUG_REGISTERS)
#else
    #define MYCONTEXT_TO_USE    CONTEXT_FULL
#endif

#if MADARCH & MADARCH_X64
    /*
     * position in Windows CONTEXT,
     * it is offset in FXSAVE/FXRSTOR memory structure
     */
    #define WOW64CONTEXT_MXCSR     24
    #define WOW64CONTEXT_XMM       (10 * 16)

    #define WOW64CONTEXT_CONTROL   WOW64_CONTEXT_CONTROL;
    #define WOW64CONTEXT_TO_USE    (WOW64_CONTEXT_FULL | WOW64_CONTEXT_FLOATING_POINT \
                            | WOW64_CONTEXT_DEBUG_REGISTERS | WOW64_CONTEXT_EXTENDED_REGISTERS)
#endif

#if MADARCH & MADARCH_X64
#define RDWORD  DWORD64
#else
#define RDWORD  DWORD
#endif

#if MADARCH & (MADARCH_X86 | MADARCH_X64)
#define GET_LDT_BASE(d) \
    ((ULONG_PTR)(d).BaseLow + ((ULONG_PTR)(d).HighWord.Bytes.BaseMid << 16L) + \
    ((ULONG_PTR)(d).HighWord.Bytes.BaseHi << 24L))

#define GET_LDT_LIMIT(d) \
    ((d).HighWord.Bits.Granularity ? \
    ((1 + (ULONG_PTR)(d).LimitLow + ((ULONG_PTR)(d).HighWord.Bits.LimitHi << 16L)) << 12L) : \
    (1 + (ULONG_PTR)(d).LimitLow + ((ULONG_PTR)(d).HighWord.Bits.LimitHi << 16L)))
#endif

typedef enum {
    STATE_NONE                  = 0x0000,
    STATE_WATCH                 = 0x0001,
    STATE_WATCH_386             = 0x0002,
    STATE_EXPECTING_FAULT       = 0x0004,
    STATE_IGNORE_DEAD_THREAD    = 0x0008,
    STATE_WAIT_FOR_VDM_START    = 0x0010,
    STATE_IGNORE_DEBUG_OUT      = 0x0020,
} state_type;

typedef enum contex_type {
    CONTEXT_TYPE_STD,
    CONTEXT_TYPE_VDM,
    CONTEXT_TYPE_WOW64
} context_type;

typedef struct _MYCONTEXT {
    context_type    type;
    union {
#if MADARCH & MADARCH_X64
        WOW64_CONTEXT   wow64;
#elif defined( WOW )
  #if MADARCH & MADARCH_X86
        VDMCONTEXT      vdm;
  #endif
#endif
        CONTEXT         std;
    } u;
} _MYCONTEXT;

typedef struct {
    WORD                signature;
    union {
        pe_exe_header   pehdr;
        os2_exe_header  nehdr;
    } u;
    char                modname[16];
} header_info;

#ifdef WOW
#if MADARCH & MADARCH_X86
typedef struct {
    addr48_ptr          addr;
    DWORD               tid;
    WORD                htask;
    DWORD               hmodule;
    char                modname[MAX_MODULE_NAME+1];
} wow_info;
#endif
#endif

typedef struct thread_info {
    struct thread_info  *next;
    DWORD               tid;
    HANDLE              thread_handle;
    FARPROC             start_addr;
    FARPROC             brk_addr;
    opcode_type         old_opcode;
    unsigned            alive       :1;
    unsigned            suspended   :1;
    unsigned            is_foreign  :1;
#if MADARCH & MADARCH_X64
    unsigned            is_wow64    :1;
#elif defined( WOW )
    unsigned            is_wow      :1;
    unsigned            is_dos      :1;
#endif
} thread_info;

typedef struct process_info {
    thread_info         *thread_list;
    DWORD               pid;
    HANDLE              __unused_field;
    HANDLE              process_handle;
    LPVOID              base_addr;
} process_info;

typedef enum {
    SS_UNKNOWN          = 0x0000,
    SS_NATIVE           = 0x0001,
    SS_WINDOWS_GUI      = 0x0002,
    SS_WINDOWS_CHAR     = 0x0003,
    SS_OS2_CHAR         = 0x0005,
    SS_POSIX_CHAR       = 0x0007
} subsystems;

typedef struct msg_list {
    struct msg_list     *next;
    char                msg[1]; /* variable size */
} msg_list;

/*
 * function prototypes
 */

typedef HANDLE  (WINAPI *OPENTHREADPROC)( DWORD, BOOL, DWORD );
typedef DWORD   (WINAPI *QUERYDOSDEVICEPROC)( LPCSTR lpDeviceName, LPSTR lpTargetPath, DWORD ucchMax );
typedef DWORD   (WINAPI *GETMAPPEDFILENAMEPROC)( HANDLE hProcess, LPVOID lpv, LPSTR lpFilename, DWORD nSize );
typedef HANDLE  (WINAPI *CREATETOOLHELP32SNAPSHOTPROC)( DWORD dwFlags, DWORD th32ProcessID );
typedef BOOL    (WINAPI *MODULE32FIRSTPROC)( HANDLE hSnapshot, LPMODULEENTRY32 lpme );
typedef BOOL    (WINAPI *MODULE32NEXTPROC)( HANDLE hSnapshot, LPMODULEENTRY32 lpme );

/* accmap.c */
extern bool             FindExceptInfo( LPVOID off, LPVOID *base, DWORD *size );
extern void             FixUpDLLNames( void );
extern void             RemoveModuleFromLibList( const char *module, const char *filename );
extern bool             IsMagicalFileHandle( HANDLE h );
extern HANDLE           GetMagicalFileHandle( const char *name );
#ifdef WOW
extern void             AddProcess16( header_info * );
#endif
extern void             AddProcess( header_info * );
extern void             DelProcess( bool );
extern void             VoidProcess( void );
#ifdef WOW
extern void             AddLib16( IMAGE_NOTE *im );
#endif
extern void             AddLib( void );
extern void             DelLib( void );
extern void             FreeLibList( void );
extern bool             DoListLibs( char *buff, int is_first, int want_16, int want_32, int verbose, int sel );

/* accmem.c */
extern DWORD            WriteMemory( addr48_ptr *addr, LPVOID buff, DWORD size );
extern DWORD            ReadMemory( addr48_ptr *addr, LPVOID buff, DWORD size );

/* accmisc.c */
extern bool             GetSelectorLDTEntry( WORD sel, LDT_ENTRY *ldt );
extern bool             IsBigSel( WORD sel );
extern void             AddMessagePrefix( const char *prefix, size_t len );

/* accrun.c */
extern opcode_type      place_breakpoint_lin( HANDLE process, FARPROC base );
extern int              remove_breakpoint_lin( HANDLE process, FARPROC base, opcode_type old_opcode );
extern trap_conditions  DebugExecute( state_type state, bool *retflag, bool );
extern void             InterruptProgram( void );
extern bool             Terminate( void );

/* misc.c */
extern bool             MyGetThreadContext( thread_info *ti, MYCONTEXT *pc );
extern bool             MySetThreadContext( thread_info *ti, MYCONTEXT *pc );

/* peread.c */
extern bool             SeekRead( HANDLE handle, DWORD newpos, void *buff, WORD size );
extern bool             GetEXEHeader( HANDLE handle, header_info *hi, WORD *stack );
extern bool             GetModuleName( HANDLE fhdl, char *buff, size_t maxlen );

/* thread.c */
extern void             AddThread( DWORD tid, HANDLE th, FARPROC sa );
extern void             DeadThread( DWORD tid );
extern thread_info      *FindThread( DWORD tid );
extern void             RemoveThread( DWORD tid );
extern void             RemoveAllThreads( void );

/* accbrwat.c */
extern bool             CheckWatchPoints( void );
extern bool             SetDebugRegs( void );
extern void             ClearDebugRegs( void );
extern bool             CheckBreakPoints( void );
extern bool             FindBreak( MYCONTEXT *con, opcode_type *old_opcode );
extern bool             IsWatch( void );

/* dbgthrd.c */
extern DWORD            StartControlThread( char *name, DWORD *pid, DWORD cr_flags );
extern bool             MyWaitForDebugEvent( void );
extern void             MyContinueDebugEvent( int );
extern void             StopControlThread( void );
extern void             ProcessQueuedRepaints( void );
extern void             ParseServiceStuff( char *name,
                            char **pdll_name, char **pservice_name,
                            char **pdll_destination, char **pservice_parm );

/* accregs.c */
extern FARPROC          AdjustIP( MYCONTEXT *, int );
extern FARPROC          GetIP( MYCONTEXT * );
extern void             SetIP( MYCONTEXT *, FARPROC );

extern void             say( const char *fmt, ... );
