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
* Description:  Trap file for QNX Neutrino interactive debugging.
*
****************************************************************************/


#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <spawn.h>
#include <confname.h>
#include <sys/stat.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>
#include <sys/resource.h>
#include "digcpu.h"
#include "trpimp.h"
#include "trperr.h"
#include "mad.h"
#include "madregs.h"
#include "ntocomm.h"
#include "x86cpu.h"


typedef unsigned short  USHORT;
typedef unsigned long   ULONG;

process_info            ProcInfo;

//#define MAX_WATCHES     32
//struct _watch_struct    WatchPoints[MAX_WATCHES];
//static int              WatchCount = 0;

static pid_t            OrigPGrp;

unsigned nto_node( void )
{
    unsigned    node;

    if( ND_NODE_CMP( ProcInfo.node, ND_LOCAL_NODE ) == 0 ) {
        return( ND_LOCAL_NODE );
    }
    node = netmgr_strtond( ProcInfo.procfs_path, 0 );
    if( node == -1 ) {
        dbg_print(( "QNX node lost!\n" ));
    }
    return( node );
}


static bool get_nto_version( unsigned_8 *major, unsigned_8 *minor )
{
    char    buf[32];

    *major = *minor = 0;
    if( confstr( _CS_RELEASE, buf, sizeof( buf ) ) ) {
        char    *s = buf;

        *major = atoi( s );
        s = strchr( s, '.' );
        if( s ) {
            *minor = atoi( ++s );
            return( TRUE );
        }
    }
    return( FALSE );
}


trap_retval TRAP_CORE( Get_sys_config )( void )
{
    get_sys_config_ret  *ret;

    ret = GetOutPtr( 0 );
    ret->os = DIG_OS_QNX;

    get_nto_version( &ret->osmajor, &ret->osminor );

    ret->cpu = X86CPUType();
    if( HAVE_EMU ) {
        ret->fpu = X86_EMU;
    } else {
        ret->fpu = ret->cpu & X86_CPU_MASK;
    }
    ret->huge_shift = 3;
    ret->arch = DIG_ARCH_X86;
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Checksum_mem )( void )
{
    unsigned char       buf[256];
    addr_off            offv;
    size_t              len;
    size_t              want;
    size_t              i;
    size_t              got;
    ULONG               sum;
    checksum_mem_req    *acc;
    checksum_mem_ret    *ret;

    sum = 0;
    if( ProcInfo.pid && !ProcInfo.at_end ) {
        acc = GetInPtr( 0 );
        dbg_print(( "mem checksum at %08x, %d bytes\n",
                (unsigned)acc->in_addr.offset, acc->len ));
        offv = acc->in_addr.offset;
        want = sizeof( buf );
        for( len = acc->len; len > 0; len -= want ) {
            if( want > len )
                want = len;
            got = ReadMemory( ProcInfo.procfd, offv, buf, want );
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
    if( ProcInfo.pid != 0 && !ProcInfo.at_end ) {
#ifdef DEBUG_MEM
        dbg_print(( "mem read at %08x, %d bytes\n", (unsigned)acc->mem_addr.offset, acc->len ));
#endif
        return( ReadMemory( ProcInfo.procfd, acc->mem_addr.offset, GetOutPtr( 0 ), acc->len ) );
    }
    return( 0 );
}


trap_retval TRAP_CORE( Write_mem )( void )
{
    write_mem_req   *acc;
    write_mem_ret   *ret;
    size_t          len;

    acc = GetInPtr( 0 );
    CONV_LE_32( acc->mem_addr.offset );
    CONV_LE_16( acc->mem_addr.segment );
    ret = GetOutPtr( 0 );
    len = GetTotalSizeIn() - sizeof( *acc );
    ret->len = 0;
    if( ProcInfo.pid && !ProcInfo.at_end ) {
#ifdef DEBUG_MEM
        dbg_print(( "mem write at %08x, %d bytes\n",
                    (unsigned)acc->mem_addr.offset, len ));
#endif
        ret->len = WriteMemory( ProcInfo.procfd, acc->mem_addr.offset,
                                GetInPtr( sizeof( *acc ) ), len );
    }
    CONV_LE_16( ret->len );
    return( sizeof( *ret ) );}


trap_retval TRAP_CORE( Read_io )( void )
/*
 * TODO: implement I/O port writes (if possible)
 */
{
#if 0
    read_io_req *acc;
    void        *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    switch( acc->len ) {
    case 1:
        break;
    case 2:
        break;
    case 4:
        break;
    default:
        return( 0 );
    }
    return( acc->len );
#else
    return( 0 );
#endif
}


trap_retval TRAP_CORE( Write_io )( void )
/*
 * TODO: implement I/O port writes (if possible)
 */
{
    write_io_ret    *ret;
#if 0
    write_io_req    *acc;
    void            *data;
    size_t          len;

    acc  = GetInPtr( 0 );
    data = GetInPtr( sizeof( *acc ) );
    len  = GetTotalSizeIn() - sizeof( *acc );
    switch( len ) {
    case 1:
        break;
    case 2:
        break;
    case 4:
        break;
    default:
        len = 0;
        break;
    }
    ret  = GetOutPtr( 0 );
    ret->len = len;
#else
    ret  = GetOutPtr( 0 );
    ret->len = 0;
#endif
    return( sizeof( *ret ) );
}


static void ReadCPU( struct x86_cpu *r )
{
    debug_greg_t    qnx_regs;
    int             regsize;

    memset( r, 0, sizeof( *r ) );
    if( devctl( ProcInfo.procfd, DCMD_PROC_GETGREG, &qnx_regs, sizeof( *r ), &regsize ) == EOK ) {
        r->eax = qnx_regs.x86.eax;
        r->ebx = qnx_regs.x86.ebx;
        r->ecx = qnx_regs.x86.ecx;
        r->edx = qnx_regs.x86.edx;
        r->esi = qnx_regs.x86.esi;
        r->edi = qnx_regs.x86.edi;
        r->ebp = qnx_regs.x86.ebp;
        r->esp = qnx_regs.x86.esp;
        r->eip = qnx_regs.x86.eip;
        r->efl = qnx_regs.x86.efl;
        r->cs  = qnx_regs.x86.cs;
        r->ss  = qnx_regs.x86.ss;
#ifdef __SEGMENTS__
        r->ds  = qnx_regs.x86.ds;
        r->es  = qnx_regs.x86.es;
        r->fs  = qnx_regs.x86.fs;
        r->gs  = qnx_regs.x86.gs;
#endif
    }
}

static void ReadFPU( struct x86_fpu *r )
{
    unsigned    regsize;

    memset( r, 0, sizeof( *r ) );
    regsize = sizeof( *r );
    devctl( ProcInfo.procfd, DCMD_PROC_GETFPREG, r, regsize, &regsize );
}

trap_retval TRAP_CORE( Read_regs )( void )
{
    mad_registers       *mr;

    mr = GetOutPtr( 0 );

    ReadCPU( &mr->x86.cpu );
    ReadFPU( &mr->x86.u.fpu );
    return( sizeof( mr->x86 ) );
}

static void WriteCPU( struct x86_cpu *r )
{
    debug_greg_t    qnx_regs;

    qnx_regs.x86.eax = r->eax;
    qnx_regs.x86.ebx = r->ebx;
    qnx_regs.x86.ecx = r->ecx;
    qnx_regs.x86.edx = r->edx;
    qnx_regs.x86.esi = r->esi;
    qnx_regs.x86.edi = r->edi;
    qnx_regs.x86.ebp = r->ebp;
    qnx_regs.x86.esp = r->esp;
    qnx_regs.x86.eip = r->eip;
    qnx_regs.x86.efl = r->efl;
    qnx_regs.x86.cs = r->cs;
    qnx_regs.x86.ss = r->ss;
#ifdef __SEGMENTS__
    qnx_regs.x86.ds = r->ds;
    qnx_regs.x86.es = r->es;
    qnx_regs.x86.fs = r->fs;
    qnx_regs.x86.gs = r->gs;
#endif
    devctl( ProcInfo.procfd, DCMD_PROC_SETGREG, &qnx_regs, sizeof( qnx_regs ), 0 );
}

static void WriteFPU( struct x86_fpu *r )
{
    if( ProcInfo.pid != 0 ) {
        devctl( ProcInfo.procfd, DCMD_PROC_SETFPREG, r, sizeof( X86_FSAVE_REGISTERS ), 0 );
    }
}

trap_retval TRAP_CORE( Write_regs )( void )
{
    mad_registers       *mr;

    mr = GetInPtr( sizeof( write_regs_req ) );
    WriteCPU( &mr->x86.cpu );
    WriteFPU( &mr->x86.u.fpu );
    return( 0 );
}

static int SplitParms( char *p, const char **args, size_t len )
/**************************************************************
 * Break up program arguments passed in as a single string into
 * individual components. Useful for passing argv style array to
 * exec().
 */
{
    int     i;
    char    endc;

    i = 0;
    if( len == 1 )
        goto done;
    for( ;; ) {
        for( ;; ) {
            if( len == 0 )
                goto done;
            if( *p != ' ' && *p != '\t' )
                break;
            ++p;
            --len;
        }
        if( len == 0 )
            goto done;
        if( *p == '"' ) {
            --len;
            ++p;
            endc = '"';
        } else {
            endc = ' ';
        }
        if( args != NULL ) {
            args[i] = p;
        }
        ++i;
        for( ;; ) {
            if( len == 0 )
                goto done;
            if( *p == endc || *p == '\0' || ( endc == ' ' && *p == '\t' ) ) {
                /*
                 * if output array is not specified then source string
                 * is not changed and it calculates number of parameters only
                 * as soon as output array is specified then source is modified
                 */
                if( args != NULL ) {
                    *p = '\0';
                }
                ++p;
                --len;
                if( len == 0 )
                    goto done;
                break;
            }
            ++p;
            --len;
        }
    }
done:
    return( i );
}

static pid_t proc_attach( pid_t pid, int *pfd, pthread_t *ptid )
{
    procfs_status       status;
    struct sigevent     event;
    char                path[PATH_MAX];
    int                 ctl_fd;

    snprintf( path, sizeof( path ) - 1, "%s/%d/as", ProcInfo.procfs_path, pid );
    dbg_print(( "attaching to pid %d, procfs path '%s'\n", pid, path ));
    ctl_fd = open( path, O_RDWR );
    if( ctl_fd == -1 ) {
        dbg_print(("failed to open proc file '%s'\n", path));
        return( 0 );
    }

    if( devctl( ctl_fd, DCMD_PROC_STOP, &status, sizeof( status ), 0 ) != EOK ) {
        dbg_print(("failed to stop process\n"));
        return( 0 );
    }

    /* Define a sigevent for process stopped notification. The OS will deliver
     * SIGUSR1 to us whenever the debuggee stops. We block SIGUSR1 and only
     * unblock it temporarily when expecting a notification.
     */
    event.sigev_notify = SIGEV_SIGNAL_THREAD;
    event.sigev_signo  = SIGUSR1;
    event.sigev_code   = 0;
    event.sigev_value.sival_ptr = NULL;
    event.sigev_priority = -1;
    devctl( ctl_fd, DCMD_PROC_EVENT, &event, sizeof( event ), 0 );

    if( devctl( ctl_fd, DCMD_PROC_STATUS, &status, sizeof( status ), 0 ) == EOK
      && status.flags & _DEBUG_FLAG_STOPPED ) {
        dbg_print(( "debuggee stopped - sending SIGCONT to pid %d\n", pid ));
        SignalKill( nto_node(), pid, 0, SIGCONT, 0, 0 );
    }
    *pfd = ctl_fd;
    *ptid = status.tid;
    return( pid );
}

static void proc_detach( char *args )
{
    int     sig = 0;

    dbg_print(( "detaching from current process (pid %d)\n", ProcInfo.pid ));
    if( args ) {
        sig = atoi( args );
    }

    if( sig ) {
        SignalKill( nto_node(), ProcInfo.pid, 0, sig, 0, 0 );
    }
    close( ProcInfo.procfd );
    ProcInfo.procfd = -1;
    ProcInfo.pid    = 0;
}

static pid_t RunningProc( const char *name, const char **name_ret )
{
    pid_t       pid;
    char        ch;
    const char  *ptr;

    ptr = name;
//    ptr = CollectNid( name, strlen( name ), nid );

    for( ;; ) {
        ch = *ptr;
        if( ch != ' ' && ch != '\t' )
            break;
        ++ptr;
    }
    if( name_ret != NULL ) {
        *name_ret = ptr;
    }
    pid = 0;
    for( ;; ) {
        ch = *ptr;
        if( ch < '0' || ch > '9' )
            break;
        pid = ( pid * 10 ) + ( ch - '0' );
        ++ptr;
    }
    if( *ptr != '\0') {
        return( 0 );
    }
    return( pid );
}


static int nto_breakpoint( addr_off addr, int type, int size )
{
    procfs_break    brk;

    brk.type = type;
    brk.addr = addr;
    brk.size = size;
    if( devctl( ProcInfo.procfd, DCMD_PROC_BREAK, &brk, sizeof( brk ), 0 ) != EOK ) {
        dbg_print(( "failed to manipulate breakpoint!\n" ));
        return( 1 );
    } else {
        return( 0 );
    }
}


static bool setup_rdebug( void )
{
    if( GetLdInfo( ProcInfo.procfd, ProcInfo.dynsec_va, &ProcInfo.rdebug_va, &ProcInfo.ld_bp_va ) ) {
        struct r_debug      rdebug;

        dbg_print(( "rdebug at %08x, ld breakpoint at %08x\n",
                    (unsigned)ProcInfo.rdebug_va, (unsigned)ProcInfo.ld_bp_va ));
        ReadMemory( ProcInfo.procfd, ProcInfo.rdebug_va, &rdebug, sizeof( rdebug ) );
        AddLibs( ProcInfo.procfd, (addr_off)rdebug.r_map );
        ProcInfo.have_rdebug = TRUE;

        /* Set a breakpoint in dynamic linker. That way we can be
         * informed on dynamic library load/unload events.
         */
        dbg_print(( "Setting ld breakpoint at %08x\n", (unsigned)ProcInfo.ld_bp_va ));
        nto_breakpoint( ProcInfo.ld_bp_va, _DEBUG_BREAK_EXEC, 0 );
        return( TRUE );
    }
    return( FALSE );
}


/* Translate pid to the corresponding executable's pathname. */
static size_t pid_to_name( pid_t pid, char *exe_name, size_t len )
{
    int                 procfd;
    char                buf[PATH_MAX];
    procfs_debuginfo    *dbg_info;

    *exe_name = '\0';
    snprintf( buf, sizeof( buf ) - 1, "%s/%d/as", ProcInfo.procfs_path, pid );
    procfd = open( buf, O_RDONLY );
    if( procfd != -1 ) {
        dbg_info = (procfs_debuginfo *)buf;
        dbg_info->path[0] = '\0';

        if( devctl( procfd, DCMD_PROC_MAPDEBUG_BASE, dbg_info, sizeof( buf ), 0 ) == EOK ) {
            if( strncmp( dbg_info->path, "./", 2 ) && (*dbg_info->path != '/') ) {
                // Bug in QNX? Initial '/' seems to be missing sometimes.
                strcpy( exe_name, "/" );
            }
            strncat( exe_name, dbg_info->path, len - 16 );
        }
        close( procfd );
    }
    return( strlen( exe_name ) + 1 );
}


trap_retval TRAP_CORE( Prog_load )( void )
{
    const char              **args;
    char                    *parms;
    char                    *parm_start;
    int                     i;
    char                    exe_name[PATH_MAX];
    const char              *name;
    pid_t                   save_pgrp;
    sigset_t                sig_set;
    prog_load_req           *acc;
    prog_load_ret           *ret;
    size_t                  len;
    int                     fds[3];
    struct inheritance      inherit;
    char                    *p;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    ProcInfo.sig    = -1;
    ProcInfo.tid    = -1;
    ProcInfo.at_end = FALSE;
    ProcInfo.fork   = FALSE;
    ProcInfo.have_rdebug = FALSE;
    ProcInfo.rdebug_va = 0;
    ProcInfo.dynsec_va = 0;
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
    ProcInfo.pid = RunningProc( args[0], &name );
    if( ProcInfo.pid != 0 ) {
        ProcInfo.loaded_proc = FALSE;
    } else {
        args[0] = name;
        FindFilePath( DIG_FILETYPE_EXE, args[0], exe_name );
        save_pgrp = getpgrp();
        setpgid( 0, OrigPGrp );

        /* Clear pending SIGUSR1 signals but don't change behaviour */
        signal( SIGUSR1, signal( SIGUSR1, SIG_IGN ) );
        sigemptyset( &sig_set );
        sigaddset( &sig_set, SIGUSR1 );
        sigprocmask( SIG_UNBLOCK, &sig_set, NULL );

        memset( &inherit, 0, sizeof( inherit ) );
        if( ND_NODE_CMP( ProcInfo.node, ND_LOCAL_NODE ) != 0 ) {
            inherit.nd = nto_node();
            inherit.flags |= SPAWN_SETND;
            inherit.flags &= ~SPAWN_EXEC;
        }
        inherit.flags |= SPAWN_SETGROUP | SPAWN_HOLD;
        inherit.pgroup = SPAWN_NEWPGROUP;

        /* Only inherit standard handles; don't let child inherit everything. */
        fds[0] = STDIN_FILENO;
        fds[1] = STDOUT_FILENO;
        fds[2] = STDERR_FILENO;

        dbg_print(( "attempting to spawn '%s'", exe_name ));
        ProcInfo.pid = spawnp( exe_name, 3, fds, &inherit, args,
              !ND_NODE_CMP( ProcInfo.node, ND_LOCAL_NODE) ? dbg_environ : NULL );
        /* Block SIGUSR1 again */
        sigprocmask( SIG_BLOCK, &sig_set, NULL );
        dbg_print(( ": received pid %d\n", ProcInfo.pid ));
        setpgid( 0, save_pgrp );
        ProcInfo.loaded_proc = TRUE;
    }
    ret->flags = 0;
    if( ProcInfo.pid != -1 ) {
        // Tell debugger to ignore segment values when comparing pointers
        ret->flags |= LD_FLAG_IS_BIG | LD_FLAG_IGNORE_SEGMENTS;
        ProcInfo.pid = proc_attach( ProcInfo.pid, &ProcInfo.procfd, &ProcInfo.tid );
        if( ProcInfo.pid == 0 ) {
            if( ProcInfo.loaded_proc ) {
                SignalKill( nto_node(), ProcInfo.pid, 0, SIGKILL, 0, 0 );
            }
            goto fail;
        }
        if( !ProcInfo.loaded_proc ) {
            pid_to_name( ProcInfo.pid, exe_name, sizeof( exe_name ) );
            dbg_print(( "attached to '%s'\n", exe_name ));
            ret->flags |= LD_FLAG_IS_STARTED;
        }
        ret->task_id = ProcInfo.pid;
        ProcInfo.dynsec_va = GetDynSection( exe_name );
        AddProcess( exe_name, ProcInfo.dynsec_va );
        if( ProcInfo.dynsec_va ) {
            setup_rdebug();
        }
        errno = 0;
    }
fail:
    ret->err = errno;
    if( ret->err != 0 ) {
        proc_detach( NULL );
    }
    // It might be better to use handle 1 for the debuggee
    ret->mod_handle = 0;
    CONV_LE_32( ret->err );
    CONV_LE_32( ret->task_id );
    CONV_LE_32( ret->mod_handle );
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Prog_kill )( void )
{
    prog_kill_ret       *ret;

    dbg_print(( "killing current process (pid %d)\n", ProcInfo.pid ));
    if( ProcInfo.pid ) {
        if( ProcInfo.loaded_proc && !ProcInfo.at_end ) {
            SignalKill( nto_node(), ProcInfo.pid, 0, SIGKILL, 0, 0 );
        }
        proc_detach( NULL );
    }
    ProcInfo.sig      = -1;
    ProcInfo.at_end   = FALSE;
    ProcInfo.save_in  = -1;
    ProcInfo.save_out = -1;
    ret = GetOutPtr( 0 );
    ret->err = 0;
    CONV_LE_32( ret->err );
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Set_break )( void )
{
    set_break_req       *acc;
    set_break_ret       *ret;

    acc = GetInPtr( 0 );
    CONV_LE_32( acc->break_addr.offset );
    CONV_LE_16( acc->break_addr.segment );
    ret = GetOutPtr( 0 );
    dbg_print(( "setting breakpoint at %04x:%08x\n", acc->break_addr.segment,
               (unsigned)acc->break_addr.offset ));
    nto_breakpoint( acc->break_addr.offset, _DEBUG_BREAK_EXEC, 0 );
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Clear_break )( void )
{
    clear_break_req      *acc;

    acc = GetInPtr( 0 );
    CONV_LE_32( acc->break_addr.offset );
    CONV_LE_16( acc->break_addr.segment );
    dbg_print(( "clearing breakpoint at %04x:%08x\n", acc->break_addr.segment,
               (unsigned)acc->break_addr.offset ));
    nto_breakpoint( acc->break_addr.offset, _DEBUG_BREAK_EXEC, -1 );
    return( 0 );
}


static int nto_watchpoint( int addr, int size, int type )
{
    procfs_break    brk;

    switch( type ) {
    case 1:         /* Read */
        brk.type = _DEBUG_BREAK_RD;
        break;
    case 2:         /* Read/Write */
        brk.type = _DEBUG_BREAK_RW;
        break;
    default:        /* Modify */
        /* FIXME: brk.type = _DEBUG_BREAK_RWM gives EINVAL for some reason.  */
        brk.type = _DEBUG_BREAK_RW;
    }
    brk.type |= _DEBUG_BREAK_HW;    /* Always ask for HW watchpoint */
    brk.addr = addr;
    brk.size = size;

    if( devctl( ProcInfo.procfd, DCMD_PROC_BREAK, &brk, sizeof( brk ), 0 ) != EOK ) {
        dbg_print(( "Failed to manipulate hardware watchpoint\n" ));
        return( -1 );
    }
    return( 0 );
}


trap_retval TRAP_CORE( Set_watch )( void )
{
    set_watch_req   *acc;
    set_watch_ret   *ret;

    acc = GetInPtr( 0 );
    CONV_LE_32( acc->break_addr.offset );
    CONV_LE_16( acc->break_addr.segment );
    dbg_print(( "setting watchpoint %d bytes at %04x:%08x\n", acc->size,
               acc->watch_addr.segment, (unsigned)acc->watch_addr.offset ));
    ret = GetOutPtr( 0 );
    ret->multiplier = 1000;
    ret->err = 1;       // failure
    if( nto_watchpoint( acc->watch_addr.offset, acc->size, 1 ) == 0 ) {
        /* Succeeded */
        ret->err = 0;   // OK
        ret->multiplier |= USING_DEBUG_REG;
    }
    CONV_LE_32( ret->err );
    CONV_LE_32( ret->multiplier );
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Clear_watch )( void )
{
    clear_watch_req *acc;

    acc = GetInPtr( 0 );
    CONV_LE_32( acc->watch_addr.offset );
    CONV_LE_16( acc->watch_addr.segment );
    dbg_print(( "clearing watchkpoint at %04x:%08x\n", acc->watch_addr.segment,
               (unsigned)acc->watch_addr.offset ));
    nto_watchpoint( acc->watch_addr.offset, -1, 1 );
    return( 0 );
}


/* Run if user hit Ctrl-C twice */
static void KillRunHandler( int sig )
{
    signal( sig, KillRunHandler );
    dbg_print(( "sending SIGKILL to debuggee (pid %d)\n", ProcInfo.pid ));
    /* Be brutal */
    SignalKill( nto_node(), ProcInfo.pid, 0, SIGKILL, 0, 0 );
}


/* Run if user hit Ctrl-C once */
static void RunHandler( int sig )
{
    /* If this doesn't work, try more severe methods */
    signal( sig, KillRunHandler );
    dbg_print(( "sending SIGINT to debuggee (pid %d)\n", ProcInfo.pid ));
    SignalKill( nto_node(), ProcInfo.pid, 0, SIGINT, 0, 0 );
}


static void Resume( int step )
{
    // TODO: set current thread here?

    ProcInfo.run.flags = _DEBUG_RUN_FAULT | _DEBUG_RUN_TRACE | _DEBUG_RUN_ARM;
    if( step ) {
        ProcInfo.run.flags |= _DEBUG_RUN_STEP;
    }

    sigemptyset( (sigset_t *)&ProcInfo.run.fault );

    sigaddset( (sigset_t *)&ProcInfo.run.fault, FLTBPT );
    sigaddset( (sigset_t *)&ProcInfo.run.fault, FLTTRACE );
    sigaddset( (sigset_t *)&ProcInfo.run.fault, FLTILL );
    sigaddset( (sigset_t *)&ProcInfo.run.fault, FLTPRIV );
    sigaddset( (sigset_t *)&ProcInfo.run.fault, FLTBOUNDS );
    sigaddset( (sigset_t *)&ProcInfo.run.fault, FLTIOVF );
    sigaddset( (sigset_t *)&ProcInfo.run.fault, FLTIZDIV );
    sigaddset( (sigset_t *)&ProcInfo.run.fault, FLTFPE );
    sigaddset( (sigset_t *)&ProcInfo.run.fault, FLTPAGE );

    // TODO: figure out which signals we should really stop on
    sigemptyset( &ProcInfo.run.trace );

#if 1
    sigaddset( (sigset_t *)&ProcInfo.run.trace, SIGINT );
    sigaddset( (sigset_t *)&ProcInfo.run.trace, SIGQUIT );
    sigaddset( (sigset_t *)&ProcInfo.run.trace, SIGTRAP );
    sigaddset( (sigset_t *)&ProcInfo.run.trace, SIGFPE );
    sigaddset( (sigset_t *)&ProcInfo.run.trace, SIGSEGV );
    sigaddset( (sigset_t *)&ProcInfo.run.trace, SIGSTOP );
    sigaddset( (sigset_t *)&ProcInfo.run.trace, SIGCHLD );
#endif

//    sigfillset( &ProcInfo.run.trace );

    ProcInfo.run.flags |= _DEBUG_RUN_CLRSIG | _DEBUG_RUN_CLRFLT;

    if( devctl( ProcInfo.procfd, DCMD_PROC_RUN, &ProcInfo.run, sizeof( ProcInfo.run ), 0 ) != EOK ) {
        dbg_print(( "failed to run debuggee!\n" ));
    }
}


static trap_conditions RunIt( unsigned step )
{
    void                (*old)();
    trap_conditions     conditions;
    sigset_t            sig_set;
    siginfo_t           info;
    procfs_status       status;

    conditions = COND_NONE;

    Resume( step );

    sigemptyset( &sig_set );
    sigaddset( &sig_set, SIGUSR1 );

    devctl( ProcInfo.procfd, DCMD_PROC_STATUS, &status, sizeof( status ), 0 );
    while( !(status.flags & _DEBUG_FLAG_ISTOP) ) {
        dbg_print(( "nothing interesting yet: flags = %x\n", status.flags) );
        old = signal( SIGINT, RunHandler );
        sigwaitinfo( &sig_set, &info );
        signal( SIGINT, old );
        devctl( ProcInfo.procfd, DCMD_PROC_STATUS, &status, sizeof( status ), 0 );
    }
    dbg_print(( "interesting: flags = %x, why = %x, tid=%d\n", status.flags, status.why, status.tid ));

    /* It is unclear how to detect that thread state changed (threads added, deleted, etc.).
     * We'll just report thread change every time we stop, unless we're single stepping.
     */
    if( (status.tid != ProcInfo.tid) || !(status.flags & _DEBUG_FLAG_SSTEP) ) {
        // If current tid is zero, the process ended. We're not interested.
        if( status.tid ) {
            ProcInfo.tid = status.tid;
            conditions |= COND_THREAD | COND_THREAD_EXTRA;
        }
    }

    if( status.flags & _DEBUG_FLAG_SSTEP ) {
        conditions |= COND_TRACE;
    } else if( status.flags & _DEBUG_FLAG_TRACE_EXEC ) {
        conditions |= COND_BREAK;
    } else if( status.flags & (_DEBUG_FLAG_TRACE_RD | _DEBUG_FLAG_TRACE_WR) ) {
        conditions |= COND_BREAK;
    } else if( status.flags & _DEBUG_FLAG_ISTOP ) {
        switch( status.why ) {
        case _DEBUG_WHY_SIGNALLED:
        case _DEBUG_WHY_FAULTED:
        case _DEBUG_WHY_JOBCONTROL:
            dbg_print(( "stopped on signal: %d\n", status.info.si_signo ));
            if( status.info.si_signo == SIGINT ) {
                conditions |= COND_USER;
            } else {
                ProcInfo.sig = status.info.si_signo;
                conditions |= COND_EXCEPTION;
            }
            break;
        case _DEBUG_WHY_TERMINATED: {
            int     wait_val = 0;

            waitpid( ProcInfo.pid, &wait_val, WNOHANG );
            dbg_print(( "debuggee terminated (pid %d), status collected (%d)\n",
                        ProcInfo.pid, wait_val ));
            ProcInfo.at_end = TRUE;
            conditions |= COND_TERMINATE;
            break;
        }
        case _DEBUG_WHY_REQUESTED:
            /* We are assuming a requested stop is due to a SIGINT.  */
            conditions |= COND_USER;
            break;
        }
    }
    /* Check the dynamic linker breakpoint. */
    if( (conditions & COND_BREAK) && (status.ip == ProcInfo.ld_bp_va) ) {
        conditions &= ~COND_BREAK;
        conditions |= COND_LIBRARIES;
        ProcessLdBreakpoint( ProcInfo.procfd, ProcInfo.rdebug_va );
    }
    return( conditions );
}


static unsigned ProgRun( bool step )
{
    debug_greg_t        regs;
    prog_go_ret         *ret;
    int                 regsize;

    ret = GetOutPtr( 0 );
    memset( ret, 0, sizeof( *ret ) );
    if( ProcInfo.at_end ) {
        dbg_print(( "process terminated - nothing to do\n" ));
        ret->conditions = COND_TERMINATE;
    } else if( step ) {
        dbg_print(( "about to step\n" ));
        ret->conditions = RunIt( 1 );
    } else {
        dbg_print(( "about to run\n" ));
        ret->conditions = RunIt( 0 );
    }
    if( (ret->conditions & COND_TERMINATE) == 0 ) {
        memset( &regs, 0, sizeof( regs ) );
        devctl( ProcInfo.procfd, DCMD_PROC_GETGREG, &regs, sizeof( regs ), &regsize );
        ret->program_counter.offset  = regs.x86.eip;
        ret->program_counter.segment = regs.x86.cs;
        ret->stack_pointer.offset  = regs.x86.esp;
        ret->stack_pointer.segment = regs.x86.ss;

        /* If debuggee has dynamic section, try getting the r_debug struct
         * every time the debuggee stops. The r_debug data may not be available
         * immediately after the debuggee process loads.
         */
        if( !ProcInfo.have_rdebug && ProcInfo.dynsec_va ) {
            if( setup_rdebug() ) {
                ret->conditions |= COND_LIBRARIES;
            }
        }
    }
    dbg_print(( "stopped at %04x:%08x because of %x\n", ret->program_counter.segment,
               (unsigned)ret->program_counter.offset, ret->conditions ));

    // Note: Some trap files always set COND_CONFIG here. This should only be
    // necessary if we were switching between 32-bit and 16-bit code and the like.
    // It should not be needed for QNX Neutrino.
    CONV_LE_32( ret->stack_pointer.offset );
    CONV_LE_16( ret->stack_pointer.segment );
    CONV_LE_32( ret->program_counter.offset );
    CONV_LE_16( ret->program_counter.segment );
    CONV_LE_16( ret->conditions );
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Prog_step )( void )
{
    return( ProgRun( TRUE ) );
}


trap_retval TRAP_CORE( Prog_go )( void )
{
    return( ProgRun( FALSE ) );
}


trap_retval TRAP_CORE( Redirect_stdin )( void  )
{
    redirect_stdin_ret *ret;

    // TODO: implement if possible
    ret = GetOutPtr( 0 );
    ret->err = 1;
    CONV_LE_32( ret->err );
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Redirect_stdout )( void  )
{
    redirect_stdout_ret *ret;

    // TODO: implement if possible
    ret = GetOutPtr( 0 );
    ret->err = 1;
    CONV_LE_32( ret->err );
    return( sizeof( *ret ) );
}


trap_retval TRAP_FILE( file_to_fullpath )( void )
{
    pid_t               pid;
    size_t              len;
    const char          *name;
    char                *fullname;
    file_string_to_fullpath_req *acc;
    file_string_to_fullpath_ret *ret;

/*     -- convert string in acc->name to full path (searchpath style)
 *        in acc->info (MAXIMUM len acc->info_len).
 */
    pid  = 0;
    acc  = GetInPtr( 0 );
    name = GetInPtr( sizeof( *acc ) );
    ret  = GetOutPtr( 0 );
    ret->err = 0;
    fullname = GetOutPtr( sizeof( *ret ) );
    if( acc->file_type == DIG_FILETYPE_EXE ) {
        pid = RunningProc( name, &name );
    }
    if( pid != 0 ) {
        /* We aren't told how big the output buffer is! */
        len = pid_to_name( pid, fullname, 512 );
        dbg_print(( "pid %d -> name '%s'\n", pid, name ));
    } else {
        len = FindFilePath( acc->file_type, name, fullname );
    }
    if( len == 0 ) {
        ret->err = ENOENT;      /* File not found */
    }
    CONV_LE_32( ret->err );
    return( sizeof( *ret ) + len + 1 );
}


trap_retval TRAP_CORE( Get_message_text )( void )
{
    static const char *const ExceptionMsgs[] = {
        "",
        TRP_QNX_hangup,
        TRP_QNX_user_interrupt,
        TRP_QNX_quit,
        TRP_EXC_illegal_instruction,
        TRP_QNX_trap,
        //"I/O trap",
        TRP_QNX_abort,
        TRP_QNX_emt,
        TRP_QNX_floating_point_error,
        TRP_QNX_process_killed,
        TRP_QNX_bus_error,
        TRP_EXC_access_violation,
        TRP_QNX_sys,
        TRP_QNX_broken_pipe,
        TRP_QNX_alarm,
        TRP_QNX_process_termination,
        TRP_QNX_user_signal_1,
        TRP_QNX_user_signal_2,
        TRP_QNX_child_stopped,
        TRP_QNX_power_fail,
        TRP_QNX_winch,
        TRP_QNX_urgent,
        TRP_QNX_poll,
        TRP_QNX_process_stopped,
        "",
        TRP_QNX_process_continued,
        TRP_QNX_device_ready,
        "",
        ""
    };
    get_message_text_ret        *ret;
    char                        *err_txt;

    ret = GetOutPtr( 0 );
    err_txt = GetOutPtr( sizeof( *ret ) );
    if( ProcInfo.fork ) {
        ProcInfo.fork = FALSE;
        strcpy( err_txt, TRP_QNX_PROC_FORK );
        ret->flags = MSG_NEWLINE | MSG_WARNING;
    } else {
        if( ProcInfo.sig == -1 ) {
            err_txt[0] = '\0';
        } else if( ProcInfo.sig > ( (sizeof( ExceptionMsgs ) / sizeof( char * ) - 1) ) ) {
            strcpy( err_txt, TRP_EXC_unknown );
        } else {
            strcpy( err_txt, ExceptionMsgs[ProcInfo.sig] );
        }
        ProcInfo.sig = -1;
        ret->flags = MSG_NEWLINE | MSG_ERROR;
    }
    return( sizeof( *ret ) + strlen( err_txt ) + 1 );
}


trap_retval TRAP_CORE( Machine_data )( void )
{
    machine_data_req    *acc;
    machine_data_ret    *ret;
    machine_data_spec   *data;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->cache_start = 0;
    ret->cache_end = ~(addr_off)0;
    if( acc->info_type == X86MD_ADDR_CHARACTERISTICS ) {
        data = GetOutPtr( sizeof( *ret ) );
        data->x86_addr_flags = X86AC_BIG;
        return( sizeof( *ret ) + sizeof( data->x86_addr_flags ) );
    }
    return( sizeof( *ret ) );
}


trap_retval TRAP_THREAD( get_next )( void )
{
    thread_get_next_req *req;
    thread_get_next_ret *ret;
    procfs_status       status;

    req = GetInPtr( 0 );
    CONV_LE_32( req->thread );
    ret = GetOutPtr( 0 );

    status.tid = req->thread + 1;
    ret->state = 0;
    if( ProcInfo.pid && !ProcInfo.at_end ) {
        if( devctl( ProcInfo.procfd, DCMD_PROC_TIDSTATUS, &status, sizeof( status ), 0 ) != EOK ) {
            dbg_print(( "failed to get thread status (tid %ld)\n", req->thread + 1 ));
            ret->thread = 0;
        } else {
            ret->thread = status.tid;
            ret->state  = status.tid_flags & _NTO_TF_FROZEN ? THREAD_FROZEN : THREAD_THAWED;
        }
    } else {
        /* If the debuggee isn't running, pretend there is one thread */
        ret->thread = req->thread ? 0 : 1;
    }
    dbg_print(( "next thread %ld (in %ld), state %d\n", ret->thread, req->thread, ret->state ));
    CONV_LE_32( ret->thread );
    return( sizeof( *ret ) );
}


trap_retval TRAP_THREAD( set )( void )
{
    thread_set_req      *req;
    thread_set_ret      *ret;
    pthread_t           tid;

    req = GetInPtr( 0 );
    CONV_LE_32( req->thread );
    tid = req->thread;
    ret = GetOutPtr( 0 );
    ret->err = 0;
    ret->old_thread = ProcInfo.tid;
    dbg_print(( "setting thread %d (currently %d)\n", tid, ProcInfo.tid ));
    if( tid ) {
        if( devctl( ProcInfo.procfd, DCMD_PROC_CURTHREAD, &tid, sizeof( tid ), 0 ) != EOK ) {
            dbg_print(( "failed to set current thread to %d\n", tid ));
            ret->err = EINVAL;
        } else {
            ProcInfo.tid = tid;
        }
    }
    CONV_LE_32( ret->old_thread );
    CONV_LE_32( ret->err );
    return( sizeof( *ret ) );
}


trap_retval TRAP_THREAD( freeze )( void )
{
    thread_freeze_req   *req;
    thread_freeze_ret   *ret;
    pthread_t           tid;

    ret = GetOutPtr( 0 );
    ret->err = 0;
    req = GetInPtr( 0 );
    CONV_LE_32( req->thread );
    tid = req->thread;
    dbg_print(( "freezing thread %d\n", tid ));
    if( tid ) {
        /* If debuggee isn't running, do nothing but pretend it worked */
        if( ProcInfo.pid && !ProcInfo.at_end ) {
            if( devctl( ProcInfo.procfd, DCMD_PROC_FREEZETHREAD, &tid, sizeof( tid ), 0 ) != EOK ) {
                dbg_print(( "failed to freeze thread %d\n", tid ));
                ret->err = EINVAL;
            }
        }
    } else {
        ret->err = EINVAL;
    }
    CONV_LE_32( ret->err );
    return( sizeof( *ret ) );
}


trap_retval TRAP_THREAD( thaw )( void )
{
    thread_thaw_req     *req;
    thread_thaw_ret     *ret;
    pthread_t           tid;

    ret = GetOutPtr( 0 );
    ret->err = 0;
    req = GetInPtr( 0 );
    CONV_LE_32( req->thread );
    tid = req->thread;
    dbg_print(( "thawing thread %d\n", tid ));
    if( tid ) {
        /* If debuggee isn't running, do nothing but pretend it worked */
        if( ProcInfo.pid && !ProcInfo.at_end ) {
            if( devctl( ProcInfo.procfd, DCMD_PROC_THAWTHREAD, &tid, sizeof( tid ), 0 ) != EOK ) {
                dbg_print(( "failed to thaw thread %d\n", tid ));
                ret->err = EINVAL;
            }
        }
    } else {
        ret->err = EINVAL;
    }
    CONV_LE_32( ret->err );
    return( sizeof( *ret ) );
}


/* see <sys/state.h> */
static char     *tstate_desc[STATE_MAX] = {
    "DEAD",
    "RUNNING",
    "READY",
    "STOPPED",
    "SEND",
    "RECEIVE",
    "REPLY",
    "STACK",
    "WAITTHREAD",
    "WAITPAGE",
    "SIGSUSPEND",
    "SIGWAITINFO",
    "NANOSLEEP",
    "MUTEX",
    "CONDVAR",
    "JOIN",
    "INTR",
    "SEM",
    "WAITCTX",
    "NET_SEND",
    "NET_REPLY"
};

trap_retval TRAP_THREAD( get_extra )( void )
{
    thread_get_extra_req    *req;
    char                    *ret;
    procfs_status           status;

    req = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret[0] = '\0';
    if( req->thread ) {
        status.tid = req->thread;
        if( devctl( ProcInfo.procfd, DCMD_PROC_TIDSTATUS, &status, sizeof( status ), 0 ) != EOK ) {
            dbg_print(( "failed to get extra for thread %ld\n", req->thread ));
            strcpy( ret, "error" );
        } else {
            if( tstate_desc[status.state] ) {
                strcpy( ret, tstate_desc[status.state] );
            } else {
                strcpy( ret, "unknown" );
            }
        }
    } else {
        strcpy( ret, "QNX Thread State" );
    }
    return( strlen( ret ) + 1 );
}


trap_version TRAPENTRY TrapInit( const char *parms, char *err, bool remote )
{
    trap_version    ver;
    sigset_t        sig_set;

    /* unused parameters */ (void)parms; (void)remote;

    /* We use SIGUSR1 to gain control after blocking wait for a process. */
    sigemptyset( &sig_set );
    sigaddset( &sig_set, SIGUSR1 );
    sigprocmask( SIG_BLOCK, &sig_set, NULL );

    ProcInfo.save_in = -1;
    ProcInfo.save_out = -1;
    ProcInfo.node = ND_LOCAL_NODE;
    strcpy( ProcInfo.procfs_path, "/proc" );
    err[0] = '\0';      /* all ok */
    ver.major = TRAP_VERSION_MAJOR;
    ver.minor = TRAP_VERSION_MINOR;
    ver.remote = FALSE;
    OrigPGrp = getpgrp();
    return( ver );
}


void TRAPENTRY TrapFini( void )
{
}
