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
#include "trpimp.h"
#include "trperr.h"
#include "mad.h"
#include "madregs.h"
#include "ntocomm.h"
#include "x86cpu.h"


typedef unsigned short  USHORT;
typedef unsigned long   ULONG;

static pid_t            OrigPGrp;

process_info        ProcInfo;

//#define MAX_WP  32
//struct _watch_struct    WatchPoints[ MAX_WP ];
short               WatchCount = 0;


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


unsigned ReqGet_sys_config( void )
{
    get_sys_config_ret  *ret;

    ret = GetOutPtr( 0 );
    ret->sys.os = OS_QNX;

    get_nto_version( &ret->sys.osmajor, &ret->sys.osminor );

    ret->sys.cpu = X86CPUType();
    if( HAVE_EMU ) {
        ret->sys.fpu = X86_EMU;
    } else {
        ret->sys.fpu = ret->sys.cpu & X86_CPU_MASK;
    }
    ret->sys.huge_shift = 3;
    ret->sys.mad = MAD_X86;
    return( sizeof( *ret ) );
}


unsigned ReqChecksum_mem( void )
{
    char                buf[256];
    addr_off            offv;
    USHORT              length;
    USHORT              size;
    int                 i;
    USHORT              amount;
    ULONG               sum;
    checksum_mem_req    *acc;
    checksum_mem_ret    *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    dbg_print(( "mem checksum at %08x, %d bytes\n", 
                (unsigned)acc->in_addr.offset, acc->len ));
    sum = 0;
    if( ProcInfo.pid && !ProcInfo.at_end ) {
        length = acc->len;
        offv = acc->in_addr.offset;
        for( ;; ) {
            if( length == 0 )
                break;
            size = (length > sizeof( buf )) ? sizeof( buf ) : length;
            amount = ReadMem( ProcInfo.procfd, buf, offv, size );
            for( i = amount; i != 0; --i )
                sum += buf[ i - 1 ];
            offv += amount;
            length -= amount;
            if( amount != size ) break;
        }
    }
    ret->result = sum;
    return( sizeof( *ret ) );
}


unsigned ReqRead_mem( void )
{
    read_mem_req    *acc;
    unsigned        len = 0;

    acc = GetInPtr( 0 );
    CONV_LE_32( acc->mem_addr.offset );
    CONV_LE_16( acc->mem_addr.segment );
    CONV_LE_16( acc->len );
    if( ProcInfo.pid && !ProcInfo.at_end ) {
#ifdef DEBUG_MEM
        dbg_print(( "mem read at %08x, %d bytes\n", (unsigned)acc->mem_addr.offset, acc->len ));
#endif
        len = ReadMem( ProcInfo.procfd, GetOutPtr( 0 ), acc->mem_addr.offset, acc->len );
    }
    return( len );
}


unsigned ReqWrite_mem( void )
{
    write_mem_req   *acc;
    write_mem_ret   *ret;
    unsigned        len;

    acc = GetInPtr( 0 );
    CONV_LE_32( acc->mem_addr.offset );
    CONV_LE_16( acc->mem_addr.segment );
    ret = GetOutPtr( 0 );
    len = GetTotalSize() - sizeof( *acc );
    ret->len = 0;
    if( ProcInfo.pid && !ProcInfo.at_end ) {
#ifdef DEBUG_MEM
        dbg_print(( "mem write at %08x, %d bytes\n", (unsigned)acc->mem_addr.offset, len ));
#endif
        ret->len = WriteMem( ProcInfo.procfd, GetInPtr( sizeof( *acc ) ), acc->mem_addr.offset, len );
    }
    CONV_LE_16( ret->len );
    return( sizeof( *ret ) );}


unsigned ReqRead_io( void )
{
    read_io_req *acc;
    void        *ret;
    unsigned    len;

// TODO: implement I/O port reads (if possible)
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    len = 0;
    return( len );
}


unsigned ReqWrite_io( void )
{
    write_io_req    *acc;
    write_io_ret    *ret;
    void            *data;
    unsigned        len;

// TODO: implement I/O port writes (if possible)
    acc  = GetInPtr( 0 );
    data = GetInPtr( sizeof( *acc ) );
    len  = GetTotalSize() - sizeof( *acc );
    ret  = GetOutPtr( 0 );
    ret->len = 0;
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

unsigned ReqRead_cpu( void )
{
    ReadCPU( GetOutPtr( 0 ) );
    return( sizeof( struct x86_cpu ) );
}

unsigned ReqRead_fpu( void )
{
    ReadFPU( GetOutPtr( 0 ) );
    return( sizeof( struct x86_fpu ) );
}

unsigned ReqRead_regs( void )
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

unsigned ReqWrite_cpu( void )
{
    WriteCPU( GetInPtr( sizeof( write_cpu_req ) ) );
    return( 0 );
}

unsigned ReqWrite_fpu( void )
{
    WriteFPU( GetInPtr( sizeof( write_fpu_req ) ) );
    return( 0 );
}

unsigned ReqWrite_regs( void )
{
    mad_registers       *mr;

    mr = GetInPtr( sizeof( write_regs_req ) );
    WriteCPU( &mr->x86.cpu );
    WriteFPU( &mr->x86.u.fpu );
    return( 0 );
}

static int SplitParms( char *p, char *args[], unsigned len )
{
    int     i;
    char    endc;

    i = 0;
    if( len == 1 ) goto done;
    for( ;; ) {
        for( ;; ) {
            if( len == 0 ) goto done;
            if( *p != ' ' && *p != '\t' ) break;
            ++p;
            --len;
        }
        if( len == 0 ) goto done;
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
            if( len == 0 ) goto done;
            if( *p == endc
             || *p == '\0'
             || (endc == ' ' && *p == '\t' ) ) {
                if( args != NULL ) {
                    *p = '\0';  //NYI: not a good idea, should make a copy
                }
                ++p;
                --len;
                if( len == 0 ) goto done;
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

static pid_t RunningProc( char *name, char **name_ret )
{
    pid_t       pid;
    char        ch;
    char        *start;

    start = name;
//    name = CollectNid( name, strlen( name ), nid );

    for( ;; ) {
        ch = *name;
        if( ch != ' ' && ch != '\t' ) break;
        ++name;
    }
    if( name_ret != NULL ) {
        *name_ret = name;
    }
    pid = 0;
    for( ;; ) {
        if( *name < '0' || *name > '9' ) break;
        pid = (pid * 10) + (*name - '0');
        ++name;
    }
    if( *name != '\0') {
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
    }
    else {
        return( 0 );
    }
}


static bool setup_rdebug( void )
{
    if( GetLdInfo( ProcInfo.procfd, ProcInfo.dynsec_va, &ProcInfo.rdebug_va, &ProcInfo.ld_bp_va ) ) {
        struct r_debug      rdebug;

        dbg_print(( "rdebug at %08x, ld breakpoint at %08x\n", 
                    (unsigned)ProcInfo.rdebug_va, (unsigned)ProcInfo.ld_bp_va ));
        ReadMem( ProcInfo.procfd, &rdebug, ProcInfo.rdebug_va, sizeof( rdebug ) );
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


unsigned ReqProg_load( void )
{
    char                    **args;
    char                    *parms;
    char                    *parm_start;
    int                     i;
    char                    exe_name[PATH_MAX];
    char                    *name;
    pid_t                   save_pgrp;
    sigset_t                sig_set;
    prog_load_req           *acc;
    prog_load_ret           *ret;
    unsigned                len;
    int                     fds[3];
    struct inheritance      inherit;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    ProcInfo.sig    = -1;
    ProcInfo.tid    = -1;
    ProcInfo.at_end = FALSE;
    ProcInfo.fork   = FALSE;
    ProcInfo.have_rdebug = FALSE;
    ProcInfo.rdebug_va = 0;
    ProcInfo.dynsec_va = 0;
    parms = (char *)GetInPtr( sizeof( *acc ) );
    parm_start = parms;
    len = GetTotalSize() - sizeof( *acc );
    if( acc->true_argv ) {
        i = 1;
        for( ;; ) {
            if( len == 0 ) break;
            if( *parms == '\0' ) {
                i++;
            }
            ++parms;
            --len;
        }
        args = alloca( i * sizeof( *args ) );
        parms = parm_start;
        len = GetTotalSize() - sizeof( *acc );
        i = 1;
        for( ;; ) {
            if( len == 0 ) break;
            if( *parms == '\0' ) {
                args[ i++ ] = parms + 1;
            }
            ++parms;
            --len;
        }
        args[i - 1] = NULL;
    } else {
        while( *parms != '\0' ) {
            ++parms;
            --len;
        }
        ++parms;
        --len;
        i = SplitParms( parms, NULL, len );
        args = alloca( (i + 2) * sizeof( *args ) );
        args[ SplitParms( parms, &args[1], len ) + 1 ] = NULL;
    }
    args[0] = parm_start;
    ProcInfo.pid = RunningProc( args[0], &name );
    if( ProcInfo.pid != 0 ) {
        ProcInfo.loaded_proc = FALSE;
    } else {
        args[0] = name;
        if( FindFilePath( TRUE, args[0], exe_name ) == 0 ) {
            exe_name[0] = '\0';
        }
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


unsigned ReqProg_kill( void )
{
    prog_kill_ret       *ret;

    ret = GetOutPtr( 0 );
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
    ret->err = 0;
    CONV_LE_32( ret->err );
    return( sizeof( *ret ) );
}


unsigned ReqSet_break( void )
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


unsigned ReqClear_break( void )
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


static int nto_watchpoint( int addr, int len, int type )
{
    procfs_break    brk;
    
    switch( type ) {
    case 1:			/* Read */
        brk.type = _DEBUG_BREAK_RD;
        break;
    case 2:			/* Read/Write */
        brk.type = _DEBUG_BREAK_RW;
        break;
    default:		/* Modify */
        /* FIXME: brk.type = _DEBUG_BREAK_RWM gives EINVAL for some reason.  */
        brk.type = _DEBUG_BREAK_RW;
    }
    brk.type |= _DEBUG_BREAK_HW;	/* Always ask for HW watchpoint */
    brk.addr = addr;
    brk.size = len;
    
    if( devctl( ProcInfo.procfd, DCMD_PROC_BREAK, &brk, sizeof( brk ), 0 ) != EOK ) {
        dbg_print(( "Failed to manipulate hardware watchpoint\n" ));
        return( -1 );
    }
    return( 0 );
}


unsigned ReqSet_watch( void )
{
    set_watch_req   *acc;
    set_watch_ret   *ret;

    acc = GetInPtr( 0 );
    CONV_LE_32( acc->break_addr.offset );
    CONV_LE_16( acc->break_addr.segment );
    ret = GetOutPtr( 0 );
    ret->multiplier = 1000;
    ret->err = 1;
    dbg_print(( "setting watchpoint %d bytes at %04x:%08x\n", acc->size,
               acc->watch_addr.segment, (unsigned)acc->watch_addr.offset ));
    if( nto_watchpoint( acc->watch_addr.offset, acc->size, 1 ) == 0 ) {
        /* Succeeded */
        ret->err = 0;
        ret->multiplier |= USING_DEBUG_REG;
    }
    CONV_LE_32( ret->err );
    CONV_LE_32( ret->multiplier );
    return( sizeof( *ret ) );
}


unsigned ReqClear_watch( void )
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


static int RunIt( unsigned step )
{
    void                (*old)();
    int                 ret;
    sigset_t            sig_set;
    siginfo_t           info;
    procfs_status       status;

    ret = 0;

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
            ret |= COND_THREAD | COND_THREAD_EXTRA;
        }
    }

    if( status.flags & _DEBUG_FLAG_SSTEP ) {
        ret |= COND_TRACE;
    } else if( status.flags & _DEBUG_FLAG_TRACE_EXEC ) {
        ret |= COND_BREAK;
    } else if( status.flags & (_DEBUG_FLAG_TRACE_RD | _DEBUG_FLAG_TRACE_WR) ) {
        ret |= COND_BREAK;
    } else if( status.flags & _DEBUG_FLAG_ISTOP ) {
        switch( status.why ) {
        case _DEBUG_WHY_SIGNALLED:
        case _DEBUG_WHY_FAULTED:
        case _DEBUG_WHY_JOBCONTROL:
            dbg_print(( "stopped on signal: %d\n", status.info.si_signo ));
            if( status.info.si_signo == SIGINT ) {
                ret |= COND_USER;
            } else {
                ProcInfo.sig = status.info.si_signo;
                ret |= COND_EXCEPTION;
            }
            break;
        case _DEBUG_WHY_TERMINATED: {
            int     wait_val = 0;

            waitpid( ProcInfo.pid, &wait_val, WNOHANG );
            dbg_print(( "debuggee terminated (pid %d), status collected (%d)\n", 
                        ProcInfo.pid, wait_val ));
            ProcInfo.at_end = TRUE;
            ret |= COND_TERMINATE;
            break;
        }
        case _DEBUG_WHY_REQUESTED:
            /* We are assuming a requested stop is due to a SIGINT.  */
            ret |= COND_USER;
            break;
        }
    }
    /* Check the dynamic linker breakpoint. */
    if( (ret & COND_BREAK) && (status.ip == ProcInfo.ld_bp_va) ) {
        ret &= ~COND_BREAK;
        ret |= COND_LIBRARIES;
        ProcessLdBreakpoint( ProcInfo.procfd, ProcInfo.rdebug_va );
    }
    return( ret );
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
    if( !(ret->conditions & COND_TERMINATE) ) {
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


unsigned ReqProg_step( void )
{
    return( ProgRun( TRUE ) );
}


unsigned ReqProg_go( void )
{
    return( ProgRun( FALSE ) );
}


unsigned ReqRedirect_stdin( void  )
{
    redirect_stdin_ret *ret;

    // TODO: implement if possible
    ret = GetOutPtr( 0 );
    ret->err = 1;
    CONV_LE_32( ret->err );
    return( sizeof( *ret ) );
}


unsigned ReqRedirect_stdout( void  )
{
    redirect_stdout_ret *ret;

    // TODO: implement if possible
    ret = GetOutPtr( 0 );
    ret->err = 1;
    CONV_LE_32( ret->err );
    return( sizeof( *ret ) );
}


unsigned ReqFile_string_to_fullpath( void )
{
    pid_t              pid;
    bool               exe;
    int                len;
    char               *name;
    char               *fullname;
    file_string_to_fullpath_req *acc;
    file_string_to_fullpath_ret *ret;

/*     -- convert string in acc->name to full path (searchpath style)
 *        in acc->info (MAXIMUM len acc->info_len).
 */
    pid  = 0;
    acc  = GetInPtr( 0 );
    name = GetInPtr( sizeof( *acc ) );
    ret  = GetOutPtr( 0 );
    fullname = GetOutPtr( sizeof( *ret ) );
    exe = ( acc->file_type == TF_TYPE_EXE ) ? TRUE : FALSE;
    if( exe ) {
        pid = RunningProc( name, &name );
    }
    if( pid != 0 ) {
        /* We aren't told how big the output buffer is! */
        len = pid_to_name( pid, fullname, 512 );
        dbg_print(( "pid %d -> name '%s'\n", pid, name ));
    } else {
        len = FindFilePath( exe, name, fullname );
    }
    if( len == 0 ) {
        ret->err = ENOENT;      /* File not found */
    } else {
        ret->err = 0;
    }
    CONV_LE_32( ret->err );
    return( sizeof( *ret ) + len + 1 );
}


unsigned ReqGet_message_text( void )
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


unsigned ReqAddr_info( void )
{
    addr_info_req       *acc;
    addr_info_ret       *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->is_big = TRUE;
    return( sizeof( *ret ) );
}


unsigned ReqMachine_data( void )
{
    machine_data_req    *acc;
    machine_data_ret    *ret;
    unsigned_8          *data;
    
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    data = GetOutPtr( sizeof( *ret ) );
    ret->cache_start = 0;
    ret->cache_end = ~(addr_off)0;
    *data = X86AC_BIG;
    return( sizeof( *ret ) + sizeof( *data ) );
}


unsigned ReqThread_get_next( void )
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


unsigned ReqThread_set( void )
{
    thread_set_req      *req;
    thread_set_ret      *ret;
    pthread_t           tid;

    req = GetInPtr( 0 );
    CONV_LE_32( req->thread );
    ret = GetOutPtr( 0 );
    tid = req->thread;
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


unsigned ReqThread_freeze( void )
{
    thread_freeze_req   *req;
    thread_freeze_ret   *ret;
    pthread_t           tid;

    req = GetInPtr( 0 );
    CONV_LE_32( req->thread );
    ret = GetOutPtr( 0 );
    tid = req->thread;
    ret->err = 0;
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


unsigned ReqThread_thaw( void )
{
    thread_thaw_req     *req;
    thread_thaw_ret     *ret;
    pthread_t           tid;

    req = GetInPtr( 0 );
    CONV_LE_32( req->thread );
    ret = GetOutPtr( 0 );
    tid = req->thread;
    ret->err = 0;
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

unsigned ReqThread_get_extra( void )
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


trap_version TRAPENTRY TrapInit( char *parm, char *err, bool remote )
{
    trap_version    ver;
    sigset_t        sig_set;

    parm = parm;
    remote = remote;

    /* We use SIGUSR1 to gain control after blocking wait for a process. */
    sigemptyset( &sig_set );
    sigaddset( &sig_set, SIGUSR1 );
    sigprocmask( SIG_BLOCK, &sig_set, NULL );

    ProcInfo.save_in = -1;
    ProcInfo.save_out = -1;
    ProcInfo.node = ND_LOCAL_NODE;
    strcpy( ProcInfo.procfs_path, "/proc" );
    err[0] = '\0';      /* all ok */
    ver.major = TRAP_MAJOR_VERSION;
    ver.minor = TRAP_MINOR_VERSION;
    ver.remote = FALSE;
    OrigPGrp = getpgrp();
    return( ver );
}


void TRAPENTRY TrapFini( void )
{
}
