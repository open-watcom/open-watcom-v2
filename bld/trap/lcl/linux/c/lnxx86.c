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
* Description:  Linux debugger trap file x86 specific functions.
*
****************************************************************************/


#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include "digcpu.h"
#include "trpimp.h"
#include "trpcomm.h"
#include "trperr.h"
#include "mad.h"
#include "madregs.h"
#include "dbg386.h"
#include "lnxcomm.h"
#include "x86cpu.h"


#define MAX_WATCHES 32

/* Structure used internally to set hardware watch points */
typedef struct {
    uint_64     value;
    dword       linear;
    addr48_ptr  addr;
    word        size;
    word        dregs;
} watch_point;

static watch_point  WatchPoints[MAX_WATCHES];
static int          WatchCount = 0;

static void ReadCPU( struct x86_cpu *r )
{
    user_regs_struct    regs;

    memset( r, 0, sizeof( *r ) );
    if( ptrace( PTRACE_GETREGS, pid, NULL, &regs ) == 0 ) {
        last_eip = regs.eip;
        orig_eax = regs.orig_eax;
        r->eax = regs.eax;
        r->ebx = regs.ebx;
        r->ecx = regs.ecx;
        r->edx = regs.edx;
        r->esi = regs.esi;
        r->edi = regs.edi;
        r->ebp = regs.ebp;
        r->esp = regs.esp;
        r->eip = regs.eip;
        r->efl = regs.eflags;
        r->cs = regs.cs;
        r->ds = regs.ds;
        r->ss = regs.ss;
        r->es = regs.es;
        r->fs = regs.fs;
        r->gs = regs.gs;
    }
}

static void ReadFPUXMM( struct x86_fpu *r, struct x86_xmm *x )
{
    user_fxsr_struct    regs;
    int                 i;

    memset( r, 0, sizeof( *r ) );
    memset( x, 0, sizeof( *x ) );
    if( ptrace( PTRACE_GETFPXREGS, pid, NULL, &regs ) == 0 ) {
        r->cw = regs.cwd;
        r->sw = regs.swd;
        r->tag = regs.twd;
        r->ip_err.p.offset = regs.fip;
        r->ip_err.p.segment = regs.fcs;
        r->op_err.p.offset = regs.foo;
        r->op_err.p.segment = regs.fos;
        for( i = 0; i < 8; i++ )
            memcpy( &r->reg[i], &regs.st_space[i], sizeof( r->reg[0] ) );
        memcpy( x->xmm, regs.xmm_space, sizeof( x->xmm ) );
        x->mxcsr = regs.mxcsr;
    }
}

trap_retval TRAP_CORE( Read_regs )( void )
{
    mad_registers   *mr;

    mr = GetOutPtr( 0 );
    ReadCPU( &mr->x86.cpu );
    ReadFPUXMM( &mr->x86.u.fpu, &mr->x86.xmm );
    return( sizeof( mr->x86 ) );
}

static void WriteCPU( struct x86_cpu *r )
{
    user_regs_struct    regs;

    /* the kernel uses an extra register orig_eax
       If orig_eax >= 0 then it will check eax for
       certain values to see if it needs to restart a
       system call.
       If it restarts a system call then it will set
       eax=orig_eax and eip-=2.
       If orig_eax < 0 then eax is used as is.
    */

    regs.eax = r->eax;
    regs.ebx = r->ebx;
    regs.ecx = r->ecx;
    regs.edx = r->edx;
    regs.esi = r->esi;
    regs.edi = r->edi;
    regs.ebp = r->ebp;
    regs.esp = r->esp;
    regs.eip = r->eip;
    if( regs.eip != last_eip ) {
        /* eip is actually changed! This means that
           the orig_eax value does not make sense;
           set it to -1 */
        orig_eax = -1;
        last_eip = regs.eip;
    }
    regs.orig_eax = orig_eax;
    regs.eflags = r->efl;
    regs.cs = r->cs;
    regs.ds = r->ds;
    regs.ss = r->ss;
    regs.es = r->es;
    regs.fs = r->fs;
    regs.gs = r->gs;
    ptrace( PTRACE_SETREGS, pid, NULL, &regs );
}

static void WriteFPUXMM( struct x86_fpu *r, struct x86_xmm *x )
{
    user_fxsr_struct    regs;
    int                 i;

    memset( &regs, 0, sizeof( regs ) );
    if( ptrace( PTRACE_GETFPXREGS, pid, NULL, &regs ) == 0 ) {
        regs.cwd = r->cw;
        regs.swd = r->sw;
        regs.twd = r->tag;
        regs.fip = r->ip_err.p.offset;
        regs.fcs = r->ip_err.p.segment;
        regs.foo = r->op_err.p.offset;
        regs.fos = r->op_err.p.segment;
        for( i = 0; i < 8; i++ )
            memcpy( &regs.st_space[i], &r->reg[i], sizeof( r->reg[0] ) );
        memcpy( regs.xmm_space, x->xmm, sizeof( x->xmm ) );
        regs.mxcsr = x->mxcsr;
        ptrace( PTRACE_SETFPXREGS, pid, NULL, &regs );
    }
}

trap_retval TRAP_CORE( Write_regs )( void )
{
    mad_registers   *mr;

    mr = GetInPtr( sizeof( write_regs_req ) );
    WriteCPU( &mr->x86.cpu );
    WriteFPUXMM( &mr->x86.u.fpu, &mr->x86.xmm );
    return( 0 );
}

u_long GetDR6( void )
{
    u_long  val;

    val = ptrace( PTRACE_PEEKUSER, pid, O_DEBUGREG( 6 ), &val );
    return( val );
}

static void SetDR6( dword val )
{
    ptrace( PTRACE_POKEUSER, pid, O_DEBUGREG( 6 ), (void *)val );
}

static void SetDR7( dword val )
{
    ptrace( PTRACE_POKEUSER, pid, O_DEBUGREG(7), (void *)val );
}

static dword SetDRn( int i, dword linear, word type )
{
    ptrace( PTRACE_POKEUSER, pid, O_DEBUGREG( i ), (void *)linear );
    return( ( (dword)type << DR7_RWLSHIFT( i ) )
//        | ( DR7_GEMASK << DR7_GLSHIFT( i ) )
          | ( DR7_LEMASK << DR7_GLSHIFT( i ) ) );
}

void ClearDebugRegs( void )
{
    int i;

    for( i = 0; i < 4; i++)
        SetDRn( i, 0, 0 );
    SetDR6( 0 );
    SetDR7( 0 );
}

static int DRegsCount( void )
{
    int     needed;
    int     i;

    needed = 0;
    for( i = 0; i < WatchCount; i++ ) {
        needed += WatchPoints[i].dregs;
    }
    return( needed );
}

int SetDebugRegs( void )
{
    int         i;
    int         j;
    int         dr;
    dword       dr7;
    watch_point *wp;
    dword       linear;
    word        size;
    word        type;

    if( DRegsCount() > 4 )
        return( false );
    dr  = 0;
    dr7 = /* DR7_GE | */ DR7_LE;
    for( wp = WatchPoints, i = WatchCount; i-- > 0; wp++ ) {
        linear = wp->linear;
        size = wp->size;
        if( size == 8 )
            size = 4;
        type = DRLen( size ) | DR7_BWR;
        for( j = 0; j < wp->dregs; j++ ) {
            dr7 |= SetDRn( dr, linear, type );
            dr++;
            linear += size;
        }
    }
    SetDR7( dr7 );
    return( true );
}

#if 0
bool CheckWatchPoints( void )
{
    watch_point *wp;
    int         i;
    uint_64     value;

    for( wp = WatchPoints, i = WatchCount; i-- > 0; wp++ ) {
        value = 0;
        ReadMemory( pid, wp->addr.offset, &value, wp->size );
        if( wp->value != value ) {
            return( true );
        }
    }
    return( false );
}
#endif

static word GetDRInfo( word segment, dword offset, word size, dword *plinear )
{
    word    dregs;
    dword   linear;

    /* unused parameters */ (void)segment;

    linear = offset;
    dregs = 1;
    if( size == 8 ) {
        size = 4;
        dregs++;
    }
    if( linear & ( size - 1 ) )
        dregs++;
    if( plinear != NULL )
        *plinear = linear & ~( size - 1 );
    return( dregs );
}

trap_retval TRAP_CORE( Set_watch )( void )
{
    set_watch_req   *acc;
    set_watch_ret   *ret;
    watch_point     *wp;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->multiplier = 100000;
    ret->err = 1;       // failure
    if( WatchCount < MAX_WATCHES ) {
        ret->err = 0;   // OK
        wp = WatchPoints + WatchCount;
        wp->addr.segment = acc->watch_addr.segment;
        wp->addr.offset = acc->watch_addr.offset;
        wp->size = acc->size;
        wp->value = 0;
        ReadMemory( pid, wp->addr.offset, &wp->value, wp->size );

        wp->dregs = GetDRInfo( wp->addr.segment, wp->addr.offset, wp->size, &wp->linear );

        WatchCount++;
        if( DRegsCount() <= 4 ) {
            ret->multiplier |= USING_DEBUG_REG;
        }
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Clear_watch )( void )
{
    clear_watch_req *acc;
    watch_point     *dst;
    watch_point     *src;
    int             i;

    acc = GetInPtr( 0 );
    dst = src = WatchPoints;
    for( i = 0; i < WatchCount; i++ ) {
        if( src->addr.segment != acc->watch_addr.segment
          || src->addr.offset != acc->watch_addr.offset ) {
            *dst = *src;
            dst++;
        }
        src++;
    }
    WatchCount--;
    return( 0 );
}

trap_retval TRAP_CORE( Read_io )( void )
/*
 * Perform I/O on the target machine on behalf of the debugger.
 * Since there are no kernel APIs in Linux to do this, we just
 * enable IOPL and use regular I/O. We will bail if we can't get
 * IOPL=3, so the debugger trap file will need to be run as root
 * before it can be used for I/O access.
 */
{
#ifdef __WATCOMC__
    read_io_req *acc;
    void        *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    if( iopl( 3 ) == 0 ) {
        switch( acc->len ) {
        case 1:
            *(unsigned_8 *)ret = inpb( acc->IO_offset );
            break;
        case 2:
            *(unsigned_16 *)ret = inpw( acc->IO_offset );
            break;
        case 4:
            *(unsigned_32 *)ret = inpd( acc->IO_offset );
            break;
        default:
            return( 0 );
        }
        return( acc->len );
    }
#endif
    return( 0 );
}

trap_retval TRAP_CORE( Write_io )( void )
/*
 * Perform I/O on the target machine on behalf of the debugger.
 * Since there are no kernel APIs in Linux to do this, we just
 * enable IOPL and use regular I/O. We will bail if we can't get
 * IOPL=3, so the debugger trap file will need to be run as root
 * before it can be used for I/O access.
 */
{
    write_io_ret    *ret;
#ifdef __WATCOMC__
    write_io_req    *acc;
    void            *data;
    size_t          len;

    acc = GetInPtr( 0 );
    data = GetInPtr( sizeof( *acc ) );
    len = 0;
    if( iopl( 3 ) == 0 ) {
        len = GetTotalSizeIn() - sizeof( *acc );
        switch( len ) {
        case 1:
            outpb( acc->IO_offset, *(unsigned_8 *)data );
            break;
        case 2:
            outpw( acc->IO_offset, *(unsigned_16 *)data );
            break;
        case 4:
            outpd( acc->IO_offset, *(unsigned_32 *)data );
            break;
        default:
            len = 0;
            break;
        }
    }
    ret = GetOutPtr( 0 );
    ret->len = len;
#else
    ret = GetOutPtr( 0 );
    ret->len = 0;
#endif
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Get_sys_config )( void )
{
    get_sys_config_ret  *ret;

    ret = GetOutPtr( 0 );
    ret->os = DIG_OS_LINUX;

    // TODO: Detect OS version (kernel version?)!
    ret->osmajor = 1;
    ret->osminor = 0;

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

const char *const ExceptionMsgs[33] = {
    "",
    TRP_QNX_hangup,
    TRP_QNX_user_interrupt,
    TRP_QNX_quit,
    TRP_EXC_illegal_instruction,
    TRP_QNX_trap,
    TRP_QNX_abort,
    TRP_QNX_bus_error,
    TRP_QNX_floating_point_error,
    TRP_QNX_process_killed,
    TRP_QNX_user_signal_1,
    TRP_EXC_access_violation "(SIGSEGV)",
    TRP_QNX_user_signal_2,
    TRP_QNX_broken_pipe,
    TRP_QNX_alarm,
    TRP_QNX_process_termination,
    TRP_EXC_floating_point_stack_check,
    TRP_QNX_child_stopped,
    TRP_QNX_process_continued,
    TRP_QNX_process_stopped,
    "", /* sigtstp */
    "", /* sigttin */
    "", /* sigttou */
    TRP_QNX_urgent,
    "", /* sigxcpu */
    "", /* sigxfsz */
    "", /* sigvtalarm */
    "", /* sigprof */
    TRP_QNX_winch,
    TRP_QNX_poll,
    TRP_QNX_power_fail,
    TRP_QNX_sys,
    ""
};
