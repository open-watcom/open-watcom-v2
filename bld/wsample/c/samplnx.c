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
* Description:  Linux performance sampling core. See Linux debugger trap
*               file for additional reference.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include "sample.h"
#include "wmsg.h"
#include "smpstuff.h"
// Helper functions shared with debugger
#if defined( __386__ )
    #define MD_x86
#elif defined( __PPC__ )
    #define MD_ppc
#else
    #error Unsupported CPU architecture
#endif
#include "exeelf.h"
#include "lnxcomm.h"
#include "cpuglob.h"

#if 0
    #define dbg_printf( ... ) printf( __VA_ARGS__ )
#else
    #define dbg_printf( ... )
#endif

#define BUFF_SIZE   2048

#ifndef __WATCOMC__
extern char             **environ;
#endif

static void             CodeLoad( char *name, u_long addr, samp_block_kinds kind );

static unsigned         SleepTime;
static unsigned         *SampleIndexP;
static unsigned         *SampleCountP;
static samp_block       **SamplesP;
static samp_block       **CallGraphP;
static unsigned         MaxThread = 1;
static pid_t            SamplePid = 0;  // pid to attach to
static pid_t            Pid;
static bool             Attached;
static pid_t            OrigPGrp;
static int              HaveRdebug;     // flag indicating valid r_debug
static struct r_debug   Rdebug;         // copy of child's r_debug (if present)
static struct r_debug   *DbgRdebug;     // address of r_debug in child's space
static Elf32_Dyn        *DbgDyn;        // VA of child's dynamic section (if present)
static volatile bool    TimerTicked;
static unsigned short   FlatSeg = 1;    // hardcoded value, no real segments
static opcode_type      saved_opcode;

static seg_offset       CommonAddr;

typedef struct lli {
    addr_off    offset;
    addr_off    dbg_dyn_sect;
    addr_off    code_size;
    char        newly_unloaded : 1;
    char        newly_loaded : 1;
    char        filename[257]; // TODO: This should really be dynamic!
} lib_load_info;

static lib_load_info    *ModuleInfo;
static int              ModuleTop;


/*
 * The following routines that keep track of loaded shared libraries were
 * stolen from the trap file, but for the profiler's use we can simplify
 * them a lot.
 */


/* Find a shared lib entry in the list */
static lib_load_info *FindLib( addr_off dynsection )
{
    unsigned    i;

    for( i = 0; i < ModuleTop; ++i ) {
        if( ModuleInfo[i].dbg_dyn_sect == dynsection )
            return( &ModuleInfo[i] );
    }
    return( NULL );
}


static struct link_map *FindLibInLinkMap( pid_t pid, struct link_map *first_lmap, addr_off dyn_base )
{
    struct link_map     lmap;
    struct link_map     *dbg_lmap;

    dbg_lmap = first_lmap;
    while( dbg_lmap != NULL ) {
        if( !GetLinkMap( pid, dbg_lmap, &lmap ) )
            break;
        if( (addr_off)lmap.l_ld == dyn_base )
            return( dbg_lmap );
        dbg_lmap = lmap.l_next;
    }
    return( NULL );
}


static void InitLibMap( void )
{
    lib_load_info       *lli;

    ModuleInfo = malloc( sizeof( lib_load_info ) );
    memset( ModuleInfo, 0, sizeof( lib_load_info ) );
    ModuleTop = 1;

    lli = &ModuleInfo[0];

    lli->offset = 0;    // main executable won't be relocated
    lli->filename[0] = '\0';
}


static void FiniLibMap( void )
{
    unsigned    i;

    for( i = 0; i < ModuleTop; ++i ) {
        ModuleInfo[i].offset = 0;
        ModuleInfo[i].dbg_dyn_sect = 0;
        ModuleInfo[i].code_size = 0;
    }
}


static void DoAddLib( pid_t pid, struct link_map *lmap )
{
    lib_load_info       *lli;

    // inefficient code, but not called very often
    ModuleTop++;
    lli = malloc( ModuleTop * sizeof( lib_load_info ) );
    memset( lli, 0, ModuleTop * sizeof( lib_load_info ) );
    memcpy( lli, ModuleInfo, (ModuleTop - 1) * sizeof( lib_load_info ) );
    free( ModuleInfo );
    ModuleInfo = lli;
    lli = &ModuleInfo[ModuleTop - 1];

    lli->offset = lmap->l_addr;
    lli->dbg_dyn_sect = (addr_off)lmap->l_ld;
    dbg_strcpy( pid, lli->filename, lmap->l_name );
    lli->offset = lmap->l_addr;

    dbg_printf( "Added library, ofs/dyn = %p/%p'%s'\n", lmap->l_addr, (addr_off)lmap->l_ld, lli->filename );

    // no name, not interested
    if( lli->filename[0] != '\0' )
        CodeLoad( lli->filename, lmap->l_addr, SAMP_CODE_LOAD );
}


static void DoRemoveLib( addr_off dynsection )
{
    unsigned    i;

    for( i = 0; i < ModuleTop; ++i ) {
        if( ModuleInfo[i].dbg_dyn_sect == dynsection ) {
            dbg_printf( "Deleting library '%s'\n", ModuleInfo[i].filename );
            ModuleInfo[i].offset = 0;
            ModuleInfo[i].dbg_dyn_sect = 0;
            ModuleInfo[i].code_size = 0;
            break;
        }
    }
}


/*
 * AddOneLib - called when dynamic linker is adding a library. Unfortunately
 * we don't get told which library, so we just have to zip through the list
 * until we find one we don't know about yet.
 */
static int AddLibrary( pid_t pid, struct link_map *first_lmap )
{
    struct link_map     lmap;
    struct link_map     *dbg_lmap;
    int                 count = 0;
    lib_load_info       *lli;

    dbg_lmap = first_lmap;
    while( dbg_lmap != NULL ) {
        if( !GetLinkMap( pid, dbg_lmap, &lmap ) )
            break;
        lli = FindLib( (addr_off)lmap.l_ld );
        if( lli == NULL ) {
            DoAddLib( pid, &lmap );
            ++count;
        }
        dbg_lmap = lmap.l_next;
    }
    return( count );
}

/*
 * DelOneLib - called when dynamic linker is deleting a library. Unfortunately
 * we don't get told which library, so we just have to zip through our list
 * until we find out which one is suddenly missing.
 */
static int RemoveLibrary( pid_t pid, struct link_map *first_lmap )
{
    int                 count = 0;
    int                 i;
    addr_off            dyn_base;

    for( i = 0; i < ModuleTop; ++i ) {
        dyn_base = ModuleInfo[i].dbg_dyn_sect;
        if( dyn_base != 0 ) {
            if( FindLibInLinkMap( pid, first_lmap, dyn_base ) == NULL ) {
                DoRemoveLib( dyn_base );
            }
        }
    }
    return( count );
}

unsigned NextThread( unsigned tid )
{
    if( tid == MaxThread )
        return( 0 );
    Samples = SamplesP[ tid ];
    SampleIndex = SampleIndexP[ tid ];
    if( CallGraphMode ) {
        CallGraph = CallGraphP[ tid ];
        SampleCount = SampleCountP[ tid ];
    }
    return( tid + 1 );
}


void SetPid( char **cmd )
{
    SamplePid = GetNumber( 1, INT_MAX, cmd, 10 );
}


void InitTimerRate( void )
{
    SleepTime = 10;     // default to 10 milliseconds
}


void SetTimerRate( char **cmd )
{
    SleepTime = GetNumber( 1, 1000, cmd, 10 );
}


unsigned long TimerRate( void )
{
    return( 1000 * SleepTime );
}


unsigned SafeMargin( void )
{
    return( Ceiling - 10 );
}


int VersionCheck( void )
{
    return( TRUE );
}


static void GrowArrays( unsigned tid )
{
    unsigned    max;

    max = MaxThread;
    SamplesP = realloc( SamplesP, tid * sizeof( void * ) );
    SampleIndexP = realloc( SampleIndexP, tid * sizeof( void * ) );
    if( CallGraphMode ) {
        CallGraphP = realloc( CallGraphP, tid * sizeof( void * ) );
        SampleCountP = realloc( SampleCountP, tid * sizeof( void * ) );
    }
    while( max < tid ) {
        AllocSamples( max + 1 );
        SamplesP[ max ] = Samples;
        SampleIndexP[ max ] = SampleIndex;
        if( CallGraphMode ) {
            CallGraphP[ max ] = CallGraph;
            SampleCountP[ max ] = SampleCount;
        }
        ++max;
    }
    MaxThread = max;
}

void RecordSample( unsigned offset, unsigned tid )
{
    samp_block  *old_samples;
    unsigned    old_sample_index;
    unsigned    old_sample_count;

    if( tid > MaxThread ) {
        GrowArrays( tid );
    }
    --tid;
    LastSampleIndex = SampleIndexP[ tid ];
    if( SampleIndexP[ tid ] == 0 ) {
        SamplesP[ tid ]->pref.tick = CurrTick;
        if( CallGraphMode ) {
            CallGraphP[ tid ]->pref.tick = CurrTick;
        }
    }
    ++CurrTick;
    SamplesP[ tid ]->d.sample.sample[ SampleIndexP[ tid ] ].offset = offset;
    SamplesP[ tid ]->d.sample.sample[ SampleIndexP[ tid ] ].segment = FlatSeg;
    SampleIndexP[ tid ]++;
    if( CallGraphMode ) {
        SampleCountP[ tid ]++;
    }
    if( CallGraphMode && tid == 0 ) {
        old_sample_count = SampleCount;
        old_samples = Samples;                  /* since RecordCGraph isn't */
        old_sample_index = SampleIndex;         /* used to threads, we fool */
        Samples = SamplesP[ tid ];              /* it into storing the info */
        SampleIndex = SampleIndexP[ tid ];      /* in the right place by    */
        SampleCount = SampleCountP[ tid ];
        RecordCGraph();                         /* changing its pointers    */
        SamplesP[ tid ] = Samples;              /* and restoring them later */
        SampleIndexP[ tid ] = SampleIndex;
        SampleCountP[ tid ] = SampleCount;
        Samples = old_samples;
        SampleIndex = old_sample_index;
        SampleCount = old_sample_count;
    }
    if( SampleIndexP[ tid ] >= Margin ) {
        StopAndSave();
    }
}


void GetCommArea( void )
{
    if( CommonAddr.segment == 0 ) {     /* can't get the common region yet */
        Comm.cgraph_top = 0;
        Comm.top_ip = 0;
        Comm.top_cs = 0;
        Comm.pop_no = 0;
        Comm.push_no = 0;
        Comm.in_hook = 1;               /* don't record this sample */
    } else {
        ReadMem( Pid, &Comm, CommonAddr.offset, sizeof( Comm ) );
    }
}


void ResetCommArea( void )
{
    if( CommonAddr.segment != 0 ) {     /* reset common variables */
        Comm.pop_no = 0;
        Comm.push_no = 0;
        WriteMem( Pid, &Comm.pop_no, CommonAddr.offset + 11, 2 * sizeof( short ) );
    }
}


void GetNextAddr( void )
{
    struct {
        unsigned long   ptr;
        seg             cs;
        off             ip;
    } stack_entry;

    if( CommonAddr.segment == 0 ) {
        CGraphOff = 0;
        CGraphSeg = 0;
    } else {
        ReadMem( Pid, &stack_entry, Comm.cgraph_top, sizeof( stack_entry ) );
        CGraphOff = stack_entry.ip;
        CGraphSeg = stack_entry.cs;
        Comm.cgraph_top = stack_entry.ptr;
    }
}


void StopProg( void )
{
}


static void CodeLoad( char *name, u_long addr, samp_block_kinds kind )
{
    seg_offset  ovl;

    ovl.offset = 0;
    ovl.segment = 0;
    WriteCodeLoad( ovl, name, kind );
    /* fake the address map - no segments here */
    WriteAddrMap( 1, FlatSeg, addr );
}


static void InternalError( char *str )
{
    Output( MsgArray[MSG_SAMPLE_2 - ERR_FIRST_MESSAGE] );
    Output( str );
    Output( "\n" );
    _exit( -1 );
}


#if defined( MD_x86 )

/* Handle profiler marks (hardcoded breakpoints with a string) */
static int ProcessMark( pid_t pid, user_regs_struct *regs )
{
    if( (regs->edx & 0xffff) != 0 ) {   /* this is a mark */
        char            buff[BUFF_SIZE];
        int             len = 0;
        seg_offset      where;

        /* read the mark string char by char */
        for( ;; ) {
            if( len >= (BUFF_SIZE - 1) )
                break;
            ReadMem( pid, &buff[len], regs->eax + len, 1 );
            if( buff[len] == '\0' )
                break;
            ++len;
        }
        buff[len] = '\0';
        where.segment = FlatSeg;
        where.offset = regs->eip;
        WriteMark( buff, where );
        return( TRUE );
    } else {
        dbg_printf( "hardcoded breakpoint was not a mark!\n" );
        return( FALSE );
    }
}

#endif

/* Handle breakpoints - we expect to see breakpoints in the dynamic linker
 * (which we set ourselves) as well as profiler marks (embedded in the
 * profiled application's code).
 */
static int ProcessBreakpoint( pid_t pid, u_long ip )
{
    static int  ld_state = RT_CONSISTENT;   // This ought to be per-pid
    int         ptrace_sig = 0;

#if defined( MD_x86 )
    user_regs_struct    regs;

    // on x86, when breakpoint was hit the EIP points to the next
    // instruction, so we must be careful
    ptrace( PTRACE_GETREGS, pid, NULL, &regs );

    if( ip == Rdebug.r_brk + sizeof( opcode_type ) ) {
#elif defined( MD_ppc )
    if( ip == Rdebug.r_brk ) {
#endif
        opcode_type         brk_opcode = BRKPOINT;
        int                 status;
        int                 ret;

        /* The dynamic linker breakpoint was hit, meaning that
         * libraries are being loaded or unloaded. This gets a bit
         * tricky because we must restore the original code that was
         * at the breakpoint and execute it, but we still want to
         * keep the breakpoint.
         */
        if( WriteMem( pid, &saved_opcode, Rdebug.r_brk, sizeof( saved_opcode ) ) != sizeof( saved_opcode ) )
            printf( "WriteMem() #1 failed\n" );
        ReadMem( pid, &Rdebug, (addr_off)DbgRdebug, sizeof( Rdebug ) );
        dbg_printf( "ld breakpoint hit, state is " );
        switch( Rdebug.r_state ) {
        case RT_ADD:
            dbg_printf( "RT_ADD\n" );
            AddLibrary( pid, Rdebug.r_map );
            ld_state = RT_ADD;
            break;
        case RT_DELETE:
            dbg_printf( "RT_DELETE\n" );
            ld_state = RT_DELETE;
            break;
        case RT_CONSISTENT:
            dbg_printf( "RT_CONSISTENT\n" );
            if( ld_state == RT_DELETE )
                RemoveLibrary( pid, Rdebug.r_map );
            ld_state = RT_CONSISTENT;
            break;
        default:
            dbg_printf( "error!\n" );
            break;
        }
#if defined( MD_x86 )
        regs.eip--;
        ptrace( PTRACE_SETREGS, pid, NULL, &regs );
#endif
        // unset breakpoint, single step, re-set breakpoint
        if( ptrace( PTRACE_SINGLESTEP, pid, NULL, (void *)ptrace_sig ) < 0 )
            perror( "ptrace()" );
        do {    // have to loop since SIGALRM can interrupt us here
            ret = waitpid( pid, &status, 0 );
        } while( (ret < 0) && (errno == EINTR) );
        if( ret == -1)
            perror( "waitpid()" );
        if( WriteMem( pid, &brk_opcode, Rdebug.r_brk, sizeof( brk_opcode ) ) != sizeof( brk_opcode ) )
            dbg_printf( "WriteMem() #2 failed with errno %d for pid %d, %d bytes (at %p)!\n", errno, pid, sizeof( brk_opcode ), Rdebug.r_brk );
        return( TRUE ); // indicate this was our breakpoint
    } else {
        dbg_printf( "Not an ld breakpoint, assuming mark\n" );
#if defined( MD_x86 )
        if( ProcessMark( pid, &regs ) )
            return( TRUE );
#endif
    }
    return( FALSE );
}


/*
 * Real time signal (SIGALRM) handler. All we really need to do is wake up
 * periodically to interrupt waitpid(), the signal handler need not do much
 * at all.
 */
static void alarm_handler( int signo )
{
    TimerTicked = TRUE;
}


/* Install periodic real time alarm signal */
static void InstSigHandler( int msec_period )
{
    struct sigaction    sigalrm;
    struct itimerval    timer;

    sigalrm.sa_handler = (void *)alarm_handler;
    sigemptyset( &sigalrm.sa_mask );
    sigalrm.sa_flags = 0;

    sigaction( SIGALRM, &sigalrm, NULL );

    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = msec_period * 1000;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = msec_period * 1000;

    if( setitimer( ITIMER_REAL, &timer, NULL ) ) {
        InternalError( MsgArray[MSG_SAMPLE_6 - ERR_FIRST_MESSAGE] );
    }
}


/*
 * Main sampler loop. We run the profiled application under the control of
 * ptrace(). The sampler installs a SIGALRM handler which ticks at the desired
 * rate. Whenever the SIGALRM interrupts our own waitpid(), we send a SIGSTOP
 * to the profiled app and when the child app notifies us of the SIGSTOP, we
 * remember the current execution point and continue the profiled app. Note
 * that we may miss some ticks but this is not a problem - the ticks don't
 * even need to be regular to provide usable results.
 */
static void SampleLoop( pid_t pid )
{
    static int          ptrace_sig = 0;
    static int          do_cont = TRUE;
    int                 status;
    user_regs_struct    regs;
    bool                sample_continue = TRUE;
    int                 ret;
    opcode_type         brk_opcode = BRKPOINT;

    TimerTicked = FALSE;
    InstSigHandler( SleepTime );

    do {
        if( do_cont && ptrace( PTRACE_CONT, pid, NULL, (void *)ptrace_sig ) == -1)
            perror( "ptrace()" );
        ret = waitpid( pid, &status, 0 );
        if( (ret < 0) && (errno == EINTR) ) {
            /* did we get woken up by SIGALRM? */
            if( TimerTicked ) {
                TimerTicked = FALSE;
                /* interrupt child process - next waitpid() will see this */
                kill( pid, SIGSTOP );
            } else {
                dbg_printf( "who the hell interrupted waitpid()?\n" );
            }
            do_cont = FALSE;
            continue;
        }
        if( ret < 0 )
            perror( "waitpid()" );
        do_cont = TRUE;

        /* record current execution point */
#if defined( MD_x86 )
        ptrace( PTRACE_GETREGS, pid, NULL, &regs );
#elif defined( MD_ppc )
        regs.eip = ptrace( PTRACE_PEEKUSER, pid, REGSIZE * PT_NIP, NULL );
#endif
        if( WIFSTOPPED( status ) ) {
            /* If debuggee has dynamic section, try getting the r_debug struct
             * every time the child process stops. The r_debug data may not be
             * available immediately after the child process first loads.
             */
            if( !HaveRdebug && (DbgDyn != NULL) ) {
                if( Get_ld_info( pid, DbgDyn, &Rdebug, &DbgRdebug ) ) {

                    AddLibrary( pid, Rdebug.r_map );
                    HaveRdebug = TRUE;

                    /* Set a breakpoint in dynamic linker. That way we can be
                     * informed on dynamic library load/unload events.
                     */
                    ReadMem( pid, &saved_opcode, Rdebug.r_brk, sizeof( saved_opcode ) );
                    dbg_printf( "setting ld breakpoint at %p, old opcode was %X\n", Rdebug.r_brk, saved_opcode );
                    WriteMem( pid, &brk_opcode, Rdebug.r_brk, sizeof( brk_opcode ) );
                }
            }

            sample_continue = FALSE;
            switch( (ptrace_sig = WSTOPSIG( status )) ) {
            case SIGSEGV:
                dbg_printf( "SIGSEGV at %p\n", regs.eip );
                sample_continue = TRUE;
                break;
            case SIGILL:
                dbg_printf( "SIGILL at %p\n", regs.eip );
                sample_continue = TRUE;
                break;
            case SIGABRT:
                dbg_printf( "SIGABRT at %p\n", regs.eip );
                sample_continue = TRUE;
                break;
            case SIGINT:
                dbg_printf( "SIGINT at %p\n", regs.eip );
                sample_continue = TRUE;
                break;
            case SIGTRAP:
                dbg_printf( "SIGTRAP at %p\n", regs.eip );
                if( ProcessBreakpoint( pid, regs.eip ) ) {
                    // don't pass on SIGTRAP if we expected this breakpoint
                    ptrace_sig = 0;
                }
                sample_continue = TRUE;
                break;
            case SIGSTOP:
                /* presumably we were behind this SIGSTOP */
                RecordSample( regs.eip, 1 );
                ptrace_sig = 0;
                sample_continue = TRUE;
                break;
            default:
                /* all other signals get passed down to the child and we let
                 * the child handle them (or not handle and die)
                 */
                sample_continue = TRUE;
                break;
            }
        } else if( WIFEXITED( status ) ) {
            dbg_printf( "WIFEXITED pid %d\n", pid );
            report();
            sample_continue = FALSE;
        } else if( WIFSIGNALED( status ) ) {
            dbg_printf( "WIFSIGNALED pid %d\n", pid );
            report();
            sample_continue = FALSE;
        }
    } while( sample_continue );
}


static int GetExeNameFromPid( pid_t pid, char *buffer, int max_len )
{
    char        procfile[24];
    int         len;

    sprintf( procfile, "/proc/%d/exe", pid );
    len = readlink( procfile, buffer, max_len );
    if( len < 0 )
        len = 0;
    buffer[len] = '\0';
    return( len );
}


void StartProg( char *cmd, char *prog, char *full_args, char *dos_args )
{
    char            exe_name[PATH_MAX];
    pid_t           save_pgrp;
    pid_t           pid;
    int             status;

    MaxThread = 0;
    GrowArrays( 1 );
    HaveRdebug = FALSE;
    DbgDyn = NULL;
    OrigPGrp = getpgrp();
    Attached = TRUE;
    pid = Pid = SamplePid;

    /* allow attaching to existing process by pid */
    if( pid == 0 || ptrace( PTRACE_ATTACH, pid, NULL, NULL ) == -1 ) {
        int         num_args;
        size_t      len;
        char        **argv;

        Attached = FALSE;

        /* massage 'full_args' into argv format */
        len = strlen( full_args );
        num_args = SplitParms( full_args, NULL, len );
        argv = alloca( (num_args + 2)  * sizeof( *argv ) );
        argv[SplitParms( full_args, &argv[1], len ) + 1] = NULL;
        argv[0] = prog;

        Output( MsgArray[MSG_SAMPLE_1 - ERR_FIRST_MESSAGE] );
        Output( prog );
        Output( "\n" );

        save_pgrp = getpgrp();
        setpgid( 0, OrigPGrp );
        pid = fork();
        if( pid == -1 )
            InternalError( MsgArray[MSG_SAMPLE_3 - ERR_FIRST_MESSAGE] );
        if( pid == 0 ) {
            int     rc;

            if( ptrace( PTRACE_TRACEME, 0, NULL, NULL ) < 0 ) {
                InternalError( MsgArray[MSG_SAMPLE_4 - ERR_FIRST_MESSAGE] );
            }
            dbg_printf( "executing '%s'\n", prog );
            for( rc = 0; argv[rc] != NULL; ++rc )
                dbg_printf( "argv[%d] = '%s'\n", rc, argv[rc] );

            rc = execve( prog, (char const * const *)argv, (char const * const *)environ );
            dbg_printf( "execve() failed, returned %d\n", rc );
            InternalError( MsgArray[MSG_SAMPLE_3 - ERR_FIRST_MESSAGE] );  // failsafe
        }
        setpgid( 0, save_pgrp );
        strcpy( exe_name, prog );
    } else if( pid ) {
        GetExeNameFromPid( pid, exe_name, PATH_MAX );
        Output( MsgArray[MSG_SAMPLE_1 - ERR_FIRST_MESSAGE] );
        Output( exe_name );
        Output( "\n" );
    }

    if( (pid != -1) && (pid != 0) ) {
        /* wait until it hits _start (upon execve) or
           gives us a SIGSTOP (if attached) */
        if( waitpid( pid, &status, 0 ) < 0 )
            goto fail;
        if( !WIFSTOPPED( status ) )
            goto fail;
        if( Attached ) {
            if( WSTOPSIG( status ) != SIGSTOP )
                goto fail;
        } else {
            if( WSTOPSIG( status ) != SIGTRAP )
                goto fail;
        }

        DbgDyn = GetDebuggeeDynSection( exe_name );
        errno = 0;
    }
    if( errno != 0 ) {
        pid = 0;
    }
    else {
        /* record information about main executable and initialize shared
         * library tracking
         */
        InitLibMap();
        CodeLoad( exe_name, 0, SAMP_MAIN_LOAD );
        SampleLoop( pid );
        FiniLibMap();
    }
    return;

fail:
    if( pid != 0 && pid != -1 ) {
        if( Attached ) {
            ptrace( PTRACE_DETACH, pid, NULL, NULL );
            Attached = FALSE;
        } else {
            ptrace( PTRACE_KILL, pid, NULL, NULL );
            waitpid( pid, &status, 0 );
        }
    }
    InternalError( MsgArray[MSG_SAMPLE_5 - ERR_FIRST_MESSAGE] );
}


void SysDefaultOptions( void )
{
}


void SysParseOptions( char c, char **cmd )
{
    char buff[2];

    switch( c ) {
    case 'r':
        SetTimerRate( cmd );
        break;
    case 'p':
        SetPid( cmd );
        break;
    default:
        Output( MsgArray[MSG_INVALID_OPTION - ERR_FIRST_MESSAGE] );
        buff[0] = c;
        buff[1] = '\0';
        Output( buff );
        Output( "\n" );
        fatal();
        break;
    }
}
