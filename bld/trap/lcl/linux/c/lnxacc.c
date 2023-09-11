/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Main Linux specific debugger trap file OS access module.
*
****************************************************************************/

/* TODO:
   * complete watchpoints
   * combine global into a struct (like the QNX trap file) to make it a little
     clearer what is global and what not.
   * implement thread support
   * implement corefile post-mortem support
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include "madconf.h"
#include "trpimp.h"
#include "trpcomm.h"
#include "trperr.h"
#include "mad.h"
#include "madregs.h"
#include "lnxcomm.h"
#include "lnxpath.h"
#include "brkptcpu.h"


addr_seg                flatCS;
addr_seg                flatDS;
pid_t                   pid;
long                    orig_eax;
long                    last_eip;

static pid_t            OrigPGrp;
static bool             attached;
static int              last_sig;
static bool             at_end;
static struct r_debug   rdebug;         /* Copy of debuggee's r_debug (if present) */
static struct r_debug   *dbg_rdebug;    /* Ptr to r_debug in debuggee's space */
static bool             have_rdebug;    /* Flag indicating valid r_debug */
static Elf32_Dyn        *dbg_dyn;       /* VA of debuggee's dynamic section (if present) */
static opcode_type      BreakOpcode;
static opcode_type      old_ld_opcode;

trap_retval TRAP_CORE( Checksum_mem )( void )
{
    unsigned char       buf[256];
    addr_off            offv;
    size_t              len;
    size_t              want;
    size_t              i;
    size_t              got;
    u_long              sum;
    checksum_mem_req    *acc;
    checksum_mem_ret    *ret;

    sum = 0;
    if( pid != 0 ) {
        want = sizeof( buf );
        acc = GetInPtr( 0 );
        offv = acc->in_addr.offset;
        for( len = acc->len; len > 0; len -= want ) {
            if( want > len )
                want = len;
            got = ReadMemory( pid, offv, buf, want );
            for( i = 0; i < got; ++i )
                sum += buf[i];
            if( got != want ) {
                break;
            }
            offv += want;
        }
    }
    ret = GetOutPtr( 0 );
    ret->result = sum;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Read_mem )( void )
{
    read_mem_req    *acc;

    acc = GetInPtr( 0 );
    CONV_LE_32( acc->mem_addr.offset );
    CONV_LE_16( acc->mem_addr.segment );
    CONV_LE_16( acc->len );
    return( ReadMemory( pid, acc->mem_addr.offset, GetOutPtr( 0 ), acc->len ) );
}

trap_retval TRAP_CORE( Write_mem )( void )
{
    write_mem_req   *acc;
    write_mem_ret   *ret;

    acc = GetInPtr( 0 );
    CONV_LE_32( acc->mem_addr.offset );
    CONV_LE_16( acc->mem_addr.segment );
    ret = GetOutPtr( 0 );
    ret->len = WriteMemory( pid, acc->mem_addr.offset, GetInPtr( sizeof( *acc ) ),
                                GetTotalSizeIn() - sizeof( *acc ) );
    CONV_LE_16( ret->len );
    return( sizeof( *ret ) );
}

#if MADARCH & MADARCH_X86
static bool GetFlatSegs( addr_seg *cs, addr_seg *ds )
{
    user_regs_struct    regs;

    if( ptrace( PTRACE_GETREGS, pid, NULL, &regs ) == 0 ) {
        *cs = regs.cs;
        *ds = regs.ds;
        return( true );
    }
    return( false );
}
#endif

static pid_t RunningProc( const char *name, const char **name_ret )
{
    pid_t       pidd;
    char        ch;
    const char  *start;

    start = name;

    for( ;; ) {
        ch = *name;
        if( ch != ' ' && ch != '\t' )
            break;
        ++name;
    }
    if( name_ret != NULL )
        *name_ret = name;
    pidd = 0;
    for( ;; ) {
        ch = *name;
        if( ch < '0' || ch > '9' )
            break;
        pidd = ( pidd * 10 ) + ( ch - '0' );
        ++name;
    }
    if( *name != '\0' )
        return( 0 );
    return( pidd );
}

static size_t GetExeNameFromPid( pid_t pid, char *buffer, size_t max_len )
{
    char        procfile[24];
    size_t      len;

    sprintf( procfile, "/proc/%d/exe", pid );
    len = readlink( procfile, buffer, max_len - 1 );
    if( len == -1 ) {
        len = 0;
    }
    buffer[len] = '\0';
    return( len );
}

trap_retval TRAP_CORE( Prog_load )( void )
{
    const char                  **args;
    char                        *parms;
    char                        *parm_start;
    int                         i;
    char                        exe_name[PATH_MAX];
    const char                  *name;
    pid_t                       save_pgrp;
    prog_load_req               *acc;
    prog_load_ret               *ret;
    size_t                      len;
    int                         status;
    char                        *p;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    last_sig = -1;
    have_rdebug = false;
    dbg_dyn = NULL;
    at_end = false;
    parms = parm_start = GetInPtr( sizeof( *acc ) );
    len = GetTotalSizeIn() - sizeof( *acc );
    if( acc->true_argv ) {
        i = 1;
        while( len-- > 0 ) {
            if( *parms++ == '\0' ) {
                i++;
            }
        }
        args = alloca( i * sizeof( *args ) );
        parms = parm_start;
        len = GetTotalSizeIn() - sizeof( *acc );
        i = 1;
        while( len-- > 0 ) {
            if( *parms++ == '\0' ) {
                args[i++] = parms;
            }
        }
    } else {
        while( --len, *parms++ != '\0' )
            {}
        i = SplitParms( parms, NULL, len ) + 2;
        args = alloca( i * sizeof( *args ) + len );
        p = memcpy( (void *)( args + i ), parms, len );
        SplitParms( p, args + 1, len );
    }
    args[0] = parm_start;
    args[i - 1] = NULL;
    attached = true;
    pid = RunningProc( args[0], &name );
    if( pid == 0 || ptrace( PTRACE_ATTACH, pid, NULL, NULL ) == -1 ) {
        attached = false;
        args[0] = name;
        if( FindFilePath( DIG_FILETYPE_EXE, args[0], exe_name ) == 0 )
            goto fail;
        save_pgrp = getpgrp();
        setpgid( 0, OrigPGrp );
        pid = fork();
        if( pid == -1 )
            return( 0 );
        if( pid == 0 ) {
            if( ptrace( PTRACE_TRACEME, 0, NULL, NULL ) < 0 ) {
                exit( 1 );
            }
            execve( exe_name, (const char **)args, (const char **)dbg_environ );
            exit( 1 ); /* failsafe */
        }
        setpgid( 0, save_pgrp );
    } else if( pid ) {
        GetExeNameFromPid( pid, exe_name, PATH_MAX );
    }
    ret->flags = 0;
    ret->mod_handle = 0;
    if( ( pid != -1 ) && ( pid != 0 ) ) {
        int status;

        ret->task_id = pid;
        ret->flags |= LD_FLAG_IS_PROT | LD_FLAG_IS_BIG;
        /* wait until it hits _start (upon execve) or
           gives us a SIGSTOP (if attached) */
        if( waitpid( pid, &status, 0 ) < 0 )
            goto fail;
        if( !WIFSTOPPED( status ) )
            goto fail;
        if( attached ) {
            ret->flags |= LD_FLAG_IS_STARTED;
            if( WSTOPSIG( status ) != SIGSTOP ) {
                goto fail;
            }
        } else {
            if( WSTOPSIG( status ) != SIGTRAP ) {
                goto fail;
            }
        }

#if MADARCH & MADARCH_X86
        if( !GetFlatSegs( &flatCS, &flatDS ) ) {
            goto fail;
        }
#endif

        dbg_dyn = GetDebuggeeDynSection( exe_name );
        AddProcess();
        errno = 0;
    }
    ret->err = errno;
    if( ret->err != 0 ) {
        pid = 0;
    }
    CONV_LE_32( ret->err );
    CONV_LE_32( ret->task_id );
    CONV_LE_32( ret->mod_handle );
    return( sizeof( *ret ) );
fail:
    if( pid != 0 && pid != -1 ) {
        if( attached ) {
            ptrace( PTRACE_DETACH, pid, NULL, NULL );
            attached = false;
        } else {
            ptrace( PTRACE_KILL, pid, NULL, NULL );
            waitpid( pid, &status, 0 );
        }
    }
    pid = 0;
    CONV_LE_32( ret->err );
    CONV_LE_32( ret->task_id );
    CONV_LE_32( ret->mod_handle );
    return( 0 );
}

trap_retval TRAP_CORE( Prog_kill )( void )
{
    prog_kill_ret   *ret;

    if( pid != 0 && !at_end ) {
        if( attached ) {
            ptrace( PTRACE_DETACH, pid, NULL, NULL );
            attached = false;
        } else {
            int status;

            ptrace( PTRACE_KILL, pid, NULL, NULL );
            waitpid( pid, &status, 0 );
        }
    }
    DelProcess();
    at_end = false;
    pid = 0;
    ret = GetOutPtr( 0 );
    ret->err = 0;
    CONV_LE_32( ret->err );
    return( sizeof( *ret ) );
}

static opcode_type place_breakpoint( addr_off offv )
{
    opcode_type old_opcode;

    ReadMemory( pid, offv, &old_opcode, sizeof( old_opcode ) );
    WriteMemory( pid, offv, &BreakOpcode, sizeof( BreakOpcode ) );
    return( old_opcode );
}

static void remove_breakpoint( addr_off offv, opcode_type old_opcode )
{
    WriteMemory( pid, offv, &old_opcode, sizeof( old_opcode ) );
}

trap_retval TRAP_CORE( Set_break )( void )
{
    set_break_req   *acc;
    set_break_ret   *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    CONV_LE_32( acc->break_addr.offset );
    CONV_LE_16( acc->break_addr.segment );
    ret->old = place_breakpoint( acc->break_addr.offset );
    Out( "ReqSet_break at " );
    OutNum( acc->break_addr.offset );
    Out( " (was " );
    OutNum( ret->old );
    Out( ")\n" );
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Clear_break )( void )
{
    clear_break_req *acc;

    acc = GetInPtr( 0 );
    CONV_LE_32( acc->break_addr.offset );
    CONV_LE_16( acc->break_addr.segment );
    remove_breakpoint( acc->break_addr.offset, acc->old );
    Out( "ReqClear_break at " );
    OutNum( acc->break_addr.offset );
    Out( " (setting to " );
    OutNum( acc->old );
    Out( ")\n" );
    return( 0 );
}

typedef void (*sighandler_t)(int);
static sighandler_t setsig( int sig, sighandler_t handler )
{
    struct sigaction sa, old_sa;

    sa.sa_flags = SA_RESTART;
    sigemptyset( &sa.sa_mask );
    sa.sa_handler = handler;
    sigaction( sig, &sa, &old_sa );
    return old_sa.sa_handler;
}

static trap_elen ProgRun( bool step )
{
    static int          ptrace_sig = 0;
    static int          ld_state = 0;
    user_regs_struct    regs;
    int                 status;
    prog_go_ret         *ret;
    void                (*old)(int);
    bool                debug_continue;

    if( pid == 0 )
        return( 0 );
    ret = GetOutPtr( 0 );

    if( at_end ) {
        ptrace_sig = 0;
        ret->conditions = COND_TERMINATE;
        goto end;
    }

    /* we only want child-generated SIGINTs now */
    do {
        old = setsig( SIGINT, SIG_IGN );
        if( step ) {
            Out( "PTRACE_SINGLESTEP\n" );
            if( ptrace( PTRACE_SINGLESTEP, pid, NULL, (void *)ptrace_sig ) == -1 ) {
                perror( "PTRACE_SINGLESTEP" );
            }
        } else {
            Out( "PTRACE_CONT\n" );
            if( ptrace( PTRACE_CONT, pid, NULL, (void *)ptrace_sig ) == -1 ) {
                perror( "PTRACE_CONT" );
            }
        }
        waitpid( pid, &status, 0 );
        setsig( SIGINT, old );

#if MADARCH & MADARCH_X86
        ptrace( PTRACE_GETREGS, pid, NULL, &regs );
#elif MADARCH & MADARCH_PPC
        regs.eip = ptrace( PTRACE_PEEKUSER, pid, REGSIZE * PT_NIP, NULL );
        regs.esp = ptrace( PTRACE_PEEKUSER, pid, REGSIZE * PT_R1, NULL );
#elif MADARCH & MADARCH_MIPS
        regs.eip = ptrace( PTRACE_PEEKUSER, pid, (void *)PC, NULL );
        regs.esp = ptrace( PTRACE_PEEKUSER, pid, (void *)29, NULL );
#endif
        Out( " eip " );
        OutNum( regs.eip );
        Out( "\n" );

        debug_continue = false;
        if( WIFSTOPPED( status ) ) {
            switch( ( ptrace_sig = WSTOPSIG( status ) ) ) {
            case SIGSEGV:
            case SIGILL:
            case SIGFPE:
            case SIGABRT:
            case SIGBUS:
            case SIGQUIT:
            case SIGSYS:
                last_sig = ptrace_sig;
                ret->conditions = COND_EXCEPTION;
                ptrace_sig = 0;
                break;
            case SIGINT:
                ret->conditions = COND_USER;
                ptrace_sig = 0;
                break;
            case SIGTRAP:
                ret->conditions = step ? COND_TRACE : COND_BREAK;
                Out( "sigtrap\n" );
                ptrace_sig = 0;
                break;
            default:
                /* For signals that we do not wish to handle, we need
                 * to continue the debuggee until we get a signal
                 * that we need to handle
                 */
                Out( "Unknown signal " );
                OutNum( ptrace_sig );
                Out( "\n" );
                debug_continue = true;
                break;
            }
        } else if( WIFEXITED( status ) ) {
            Out( "WIFEXITED\n" );
            at_end = true;
            ret->conditions = COND_TERMINATE;
            ptrace_sig = 0;
            goto end;
        }
    } while( debug_continue );

    if( ret->conditions == COND_BREAK ) {
#if MADARCH & MADARCH_X86
        if( regs.eip == rdebug.r_brk + sizeof( opcode_type ) ) {
#elif MADARCH & (MADARCH_PPC | MADARCH_MIPS)
        if( regs.eip == rdebug.r_brk ) {
#endif
            int         psig = 0;
            void        (*oldsig)(int);

            /* The dynamic linker breakpoint was hit, meaning that
             * libraries are being loaded or unloaded. This gets a bit
             * tricky because we must restore the original code that was
             * at the breakpoint and execute it, but we still want to
             * keep the breakpoint.
             */
            remove_breakpoint( rdebug.r_brk, old_ld_opcode );
            ReadMemory( pid, (addr_off)dbg_rdebug, &rdebug, sizeof( rdebug ) );
            Out( "ld breakpoint hit, state is " );
            switch( rdebug.r_state ) {
            case RT_ADD:
                Out( "RT_ADD\n" );
                ld_state = RT_ADD;
                AddOneLib( rdebug.r_map );
                break;
            case RT_DELETE:
                Out( "RT_DELETE\n" );
                ld_state = RT_DELETE;
                break;
            case RT_CONSISTENT:
                Out( "RT_CONSISTENT\n" );
                if( ld_state == RT_DELETE )
                    DelOneLib( rdebug.r_map );
                ld_state = RT_CONSISTENT;
                break;
            default:
                Out( "error!\n" );
                break;
            }
            regs.orig_eax = -1;
#if MADARCH & MADARCH_X86
            regs.eip--;
            ptrace( PTRACE_SETREGS, pid, NULL, &regs );
#endif
            oldsig = setsig( SIGINT, SIG_IGN );
            ptrace( PTRACE_SINGLESTEP, pid, NULL, (void *)psig );
            waitpid( pid, &status, 0 );
            setsig( SIGINT, oldsig );
            /*
             * put back ld breakpoint
             */
            place_breakpoint( rdebug.r_brk );
            ret->conditions = COND_LIBRARIES;
        } else {
#if MADARCH & MADARCH_X86
            Out( "decrease eip(sigtrap)\n" );
            regs.orig_eax = -1;
            regs.eip--;
            ptrace( PTRACE_SETREGS, pid, NULL, &regs );
#endif
        }
    }
    orig_eax = regs.orig_eax;
    last_eip = regs.eip;
    ret->program_counter.offset = regs.eip;
    ret->program_counter.segment = regs.cs;
    ret->stack_pointer.offset = regs.esp;
    ret->stack_pointer.segment = regs.ss;
    ret->conditions |= COND_CONFIG;

    /* If debuggee has dynamic section, try getting the r_debug struct
     * every time the debuggee stops. The r_debug data may not be available
     * immediately after the debuggee process loads.
     */
    if( !have_rdebug && (dbg_dyn != NULL) ) {
        if( Get_ld_info( pid, dbg_dyn, &rdebug, &dbg_rdebug ) ) {
            AddInitialLibs( rdebug.r_map );
            have_rdebug = true;
            ret->conditions |= COND_LIBRARIES;

            /* Set a breakpoint in dynamic linker. That way we can be
             * informed on dynamic library load/unload events.
             */
            old_ld_opcode = place_breakpoint( rdebug.r_brk );
            Out( "Setting ld breakpoint at " );
            OutNum( rdebug.r_brk );
            Out( " old opcode was " );
            OutNum( old_ld_opcode );
            Out( "\n" );
        }
    }
 end:
    CONV_LE_32( ret->stack_pointer.offset );
    CONV_LE_16( ret->stack_pointer.segment );
    CONV_LE_32( ret->program_counter.offset );
    CONV_LE_16( ret->program_counter.segment );
    CONV_LE_16( ret->conditions );
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Prog_step )( void )
{
    return( ProgRun( true ) );
}

trap_retval TRAP_CORE( Prog_go )( void )
{
    return( ProgRun( false ) );
}

trap_retval TRAP_CORE( Redirect_stdin )( void  )
{
    redirect_stdin_ret *ret;

    ret = GetOutPtr( 0 );
    ret->err = 1;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Redirect_stdout )( void  )
{
    redirect_stdout_ret *ret;

    ret = GetOutPtr( 0 );
    ret->err = 1;
    return( sizeof( *ret ) );
}

trap_retval TRAP_FILE( file_to_fullpath )( void )
{
    file_string_to_fullpath_req *acc;
    file_string_to_fullpath_ret *ret;
    size_t                      len;
    const char                  *name;
    char                        *fullname;
    pid_t                       pidd;

    acc = GetInPtr( 0 );
    name = GetInPtr( sizeof( *acc ) );
    fullname = GetOutPtr( sizeof( *ret ) );
    pidd = 0;
    if( acc->file_type == DIG_FILETYPE_EXE ) {
        pidd = RunningProc( name, &name );
    }
    if( pidd != 0 ) {
        len = GetExeNameFromPid( pidd, fullname, PATH_MAX );
    } else {
        len = FindFilePath( acc->file_type, name, fullname );
    }
    ret = GetOutPtr( 0 );
    ret->err = 0;
    if( len == 0 ) {
        ret->err = ENOENT;      /* File not found */
    }
    CONV_LE_32( ret->err );
    return( sizeof( *ret ) + len + 1 );
}

trap_retval TRAP_CORE( Get_message_text )( void )
{
    get_message_text_ret    *ret;
    char                    *err_txt;

    ret = GetOutPtr( 0 );
    err_txt = GetOutPtr( sizeof( *ret ) );
    if( last_sig == -1 ) {
        err_txt[0] = '\0';
    } else if( last_sig > (sizeof( ExceptionMsgs ) / sizeof( char * ) - 1) ) {
        strcpy( err_txt, TRP_EXC_unknown );
    } else {
        strcpy( err_txt, ExceptionMsgs[last_sig] );
        last_sig = -1;
        ret->flags = MSG_NEWLINE | MSG_ERROR;
    }
    return( sizeof( *ret ) + strlen( err_txt ) + 1 );
}

trap_version TRAPENTRY TrapInit( const char *parms, char *err, bool remote )
{
    trap_version ver;

    /* unused parameters */ (void)parms; (void)remote;

    err[0] = '\0'; /* all ok */
    ver.major = TRAP_VERSION_MAJOR;
    ver.minor = TRAP_VERSION_MINOR;
    ver.remote = false;
    OrigPGrp = getpgrp();
    BreakOpcode = BRKPOINT;

    return( ver );
}

void TRAPENTRY TrapFini( void )
{
}
