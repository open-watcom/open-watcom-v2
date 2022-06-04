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
* Description:  Internal header file for the Linux standard debugger
*               trap file.
*
****************************************************************************/

#ifndef _LINUXCOMM_H
#define _LINUXCOMM_H

#include "machtype.h"
#include "exeelf.h"

//#define DEBUG_OUT

/* Use 4-byte packing for compatibility with the default packing used by GCC */

#include "pushpck4.h"

/* Options set using PTRACE_SETOPTIONS */
#define PTRACE_O_TRACESYSGOOD   0x00000001

#if defined( MD_x86 )

/* This defines the structure used to read and write the entire
 * set of general purpose CPU registers using sys_ptrace().
 */
typedef struct {
    long    ebx;
    long    ecx;
    long    edx;
    long    esi;
    long    edi;
    long    ebp;
    long    eax;
    int     ds;
    int     es;
    int     fs;
    int     gs;
    long    orig_eax;
    long    eip;
    int     cs;
    long    eflags;
    long    esp;
    int     ss;
} user_regs_struct;

/* This defines the structure used to read and write all the floating
 * point registers using sys_ptrace().
 */
typedef struct {
    long    cwd;
    long    swd;
    long    twd;
    long    fip;
    long    fcs;
    long    foo;
    long    fos;
    long    st_space[20];   /* 8*10 bytes for each FP-reg = 80 bytes */
} user_i387_struct;

/* This defines the structure used to read and write all the extended
 * floating point registers using sys_ptrace().
 */
typedef struct {
    u_short cwd;
    u_short swd;
    u_short twd;
    u_short fop;
    long    fip;
    long    fcs;
    long    foo;
    long    fos;
    long    mxcsr;
    long    reserved;
    long    st_space[32];   /* 8*16 bytes for each FP-reg = 128 bytes */
    long    xmm_space[32];  /* 8*16 bytes for each XMM-reg = 128 bytes */
    long    padding[56];
} user_fxsr_struct;

/* This defines the user structure that we can read and write using the
 * sys_ptrace() system call. This allows us to adjust the CPU registers,
 * FPU registers, debug registers and find out information about the
 * running process in memory. This is also the structure that heads up
 * a core dump when the kernel dumps core for a faulting process.
 */
typedef struct {
  user_regs_struct  regs;           /* Where the registers are actually stored */
  int               u_fpvalid;      /* True if math co-processor being used. Not yet used. */
  user_i387_struct  i387;           /* Math Co-processor registers. */
  u_long            u_tsize;        /* Text segment size (pages). */
  u_long            u_dsize;        /* Data segment size (pages). */
  u_long            u_ssize;        /* Stack segment size (pages). */
  u_long            start_code;     /* Starting virtual address of text. */
  u_long            start_stack;    /* Starting virtual address of stack area (bottom). */
  long              signal;         /* Signal that caused the core dump. */
  int               reserved;       /* No longer used */
  void              *u_ar0;         /* Used by gdb to help find the values for */
  user_i387_struct  *u_fpstate;     /* Math Co-processor pointer. */
  u_long            magic;          /* To uniquely identify a core file */
  char              u_comm[32];     /* User command that was responsible */
  int               u_debugreg[8];  /* Hardware debug registers */
} user_struct;

/* Define macros to get the offset of debug registers in user structure */

#define O_DEBUGREG(r)   (void *)offsetof(user_struct,u_debugreg[r])

/* Structure used internally to set hardware watch points */

typedef struct {
    addr48_ptr  loc;
    u_long      value;
    u_long      linear;
    u_short     len;
    u_short     dregs;
} watch_point;


#define MAX_WATCHES     32

#endif

#if defined( MD_ppc )

typedef struct {
    u_long eax;
    u_long eip;
    u_long orig_eax;
    u_long cs;
    u_long ss;
    u_long esp;
} user_regs_struct;

#ifndef _PPC_PTRACE_H
struct pt_regs {
    unsigned long gpr[32];
    unsigned long nip;
    unsigned long msr;
    unsigned long orig_gpr3;        /* Used for restarting system calls */
    unsigned long ctr;
    unsigned long link;
    unsigned long xer;
    unsigned long ccr;
    unsigned long mq;               /* 601 only (not used at present) */
                                    /* Used on APUS to hold IPL value. */
    unsigned long trap;             /* Reason for being here */
    unsigned long dar;              /* Fault registers */
    unsigned long dsisr;
    unsigned long result;           /* Result of a system call */
};
#endif

typedef struct user {
    struct pt_regs  regs;           /* entire machine state */
    size_t          u_tsize;        /* text size (pages) */
    size_t          u_dsize;        /* data size (pages) */
    size_t          u_ssize;        /* stack size (pages) */
    unsigned long   start_code;     /* text starting address */
    unsigned long   start_data;     /* data starting address */
    unsigned long   start_stack;    /* stack starting address */
    long            signal;         /* signal causing core dump */
    struct regs *   u_ar0;          /* help gdb find registers */
    unsigned long   magic;          /* identifies a core file */
    char            u_comm[32];     /* user command name */
} user_struct;

#define REGSIZE         sizeof( unsigned long )
#define PTRACE_SETREGS  13

#endif

#if defined( MD_mps )

typedef struct {
    unsigned long eax;
    unsigned long eip;
    unsigned long orig_eax;
    unsigned long cs;
    unsigned long ss;
    unsigned long esp;
} user_regs_struct;

/* from /usr/include/asm/reg.h */
#define EF_SIZE                 180

/* from /usr/include/asm/user.h */
typedef struct user {
    unsigned long   regs[EF_SIZE/4+64];     /* integer and fp regs */
    size_t          u_tsize;                /* text size (pages) */
    size_t          u_dsize;                /* data size (pages) */
    size_t          u_ssize;                /* stack size (pages) */
    unsigned long   start_code;             /* text starting address */
    unsigned long   start_data;             /* data starting address */
    unsigned long   start_stack;            /* stack starting address */
    long            signal;                 /* signal causing core dump */
#if 0
    struct regs *   u_ar0;                  /* help gdb find registers */
#else
    void *          u_ar0;
#endif
    unsigned long   magic;                  /* identifies a core file */
    char            u_comm[32];             /* user command name */
} user_struct;

#define REGSIZE         sizeof( unsigned long )

#define FPR_BASE        32
#define PC              64
#define MMHI            67
#define MMLO            68
#define FPC_CSR         69
#define FPC_EIR         70

#endif

/* Rendezvous structure for communication between the dynamic linker and
 * the debugger. If executable's .dynamic section contains a DT_DEBUG element,
 * the dynamic linker sets it to point to this structure.
 */

struct r_debug {
    int                 r_version;    /* Protocol version */
    struct link_map     *r_map;       /* Start of loaded objects list */
    Elf32_Addr          r_brk;        /* Address to set a breakpoint at */
    enum {
        RT_CONSISTENT,                /* Mapping change is complete */
        RT_ADD,                       /* Loading new shared object */
        RT_DELETE                     /* Removing a shared object */
    }                   r_state;      /* Current state of dynamic linker */
    Elf32_Addr          r_ldbase;     /* Dynamic linker's load base */
};

/* Structure describing a loaded shared object - part of a doubly linked list */
struct link_map {
    Elf32_Addr          l_addr;       /* Offset from object's va to load base */
    char                *l_name;      /* Name (absolute path) of shared object */
    Elf32_Dyn           *l_ld;        /* Shared object's dynamic section */
    struct link_map     *l_next;      /* Next entry in chain */
    struct link_map     *l_prev;      /* Previous entry in chain */
};

#if defined( __WATCOMC__ ) && defined( __386__ )

/* Direct I/O port access functions */

void outpb(u_long port,u_char val);
#pragma aux outpb = \
        "out  dx,al"    \
    __parm              [__edx] [__al] \
    __value             \
    __modify __exact    []

void outpw(u_long port,u_short val);
#pragma aux outpw = \
        "out  dx,ax"    \
    __parm              [__edx] [__ax] \
    __value             \
    __modify __exact    []

void outpd(u_long port,u_long val);
#pragma aux outpd = \
        "out  dx,eax"   \
    __parm              [__edx] [__eax] \
    __value             \
    __modify __exact    []

u_char inpb(u_long port);
#pragma aux inpb = \
        "in   al,dx"    \
    __parm              [__edx] \
    __value             [__al] \
    __modify __exact    [__al]

u_short inpw(u_long port);
#pragma aux inpw = \
        "in   ax,dx"    \
    __parm              [__edx] \
    __value             [__ax] \
    __modify __exact    [__ax]

u_long inpd(u_long port);
#pragma aux inpd = \
        "in   eax,dx"   \
    __parm              [__edx] \
    __value             [__eax] \
    __modify __exact    [__eax]

#endif

/* Internal helper functions */

extern unsigned     TryOnePath( const char *, struct stat *, const char *, char * );
extern unsigned     FindFilePath( dig_filetype, const char *, char * );
extern u_long       GetDR6( void );
extern void         ClearDebugRegs( void );
extern int          SetDebugRegs( void );
extern int          CheckWatchPoints( void );
extern int          GetLinkMap( pid_t pid, struct link_map *, struct link_map * );
extern int          AddInitialLibs( struct link_map * );
extern int          AddOneLib( struct link_map * );
extern int          DelOneLib( struct link_map * );
extern void         AddProcess( void );
extern void         DelProcess( void );
extern void         print_msg( const char *format, ... );
extern char         *StrCopyDst( const char *src, char *dst );

/* Utility functions shared with execution sampler */
extern unsigned     ReadMem( pid_t pid, void *ptr, addr_off offv, unsigned size );
extern unsigned     WriteMem( pid_t pid, void *ptr, addr_off offv, unsigned size );
extern Elf32_Dyn    *GetDebuggeeDynSection( const char *exe_name );
extern int          Get_ld_info( pid_t pid, Elf32_Dyn *dbg_dyn, struct r_debug *debug_ptr, struct r_debug **dbg_rdebug_ptr );
extern char         *dbg_strcpy( pid_t pid, char *, const char * );
extern int          SplitParms( char *p, const char **args, unsigned len );

/* Copy of parent's environment */
#if !defined( BUILTIN_TRAP_FILE )
extern char         **dbg_environ;
#else
#define dbg_environ environ
#endif

/* Global variables */
extern pid_t    pid;
extern long     orig_eax;
extern long     last_eip;

/* Table of signal descriptions */
extern const char *const ExceptionMsgs[33];

/* Internal debugging functions */

#ifdef DEBUG_OUT
extern void Out( const char *str );
extern void OutNum( unsigned long i );
#else
#define Out( x )
#define OutNum( x )
#endif

/* Global trap file vairables */

extern u_short          flatCS;
extern u_short          flatDS;
extern pid_t            pid;

#include "poppck.h"

#endif  /* _LINUXCOMM_H */
