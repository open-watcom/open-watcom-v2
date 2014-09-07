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
* Description:  Linux debugger trap file x86 specific functions.
*
****************************************************************************/


#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include "trpimp.h"
#include "trperr.h"
#include "mad.h"
#include "madregs.h"
#include "dbg386.h"
#include "exeelf.h"
#include "lnxcomm.h"
#include "x86cpu.h"

static watch_point      wpList[ MAX_WP ];
static int              wpCount = 0;

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

static void ReadFPU( struct x86_fpu *r )
{
    user_i387_struct    regs;

    memset( r, 0, sizeof( *r ) );
    if( ptrace( PTRACE_GETFPREGS, pid, NULL, &regs ) == 0 ) {
        r->cw = regs.cwd;
        r->sw = regs.swd;
        r->tag = regs.twd;
        r->ip_err.p.offset = regs.fip;
        r->ip_err.p.segment = regs.fcs;
        r->op_err.p.offset = regs.foo;
        r->op_err.p.segment = regs.fos;
        memcpy( r->reg, regs.st_space, sizeof( r->reg ) );
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

trap_retval ReqRead_cpu( void )
{
    ReadCPU( GetOutPtr( 0 ) );
    return( sizeof( struct x86_cpu ) );
}

trap_retval ReqRead_fpu( void )
{
    ReadFPU( GetOutPtr( 0 ) );
    return( sizeof( struct x86_fpu ) );
}

trap_retval ReqRead_regs( void )
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

static void WriteFPU( struct x86_fpu *r )
{
    user_i387_struct    regs;

    regs.cwd = r->cw;
    regs.swd = r->sw;
    regs.twd = r->tag;
    regs.fip = r->ip_err.p.offset;
    regs.fcs = r->ip_err.p.segment;
    regs.foo = r->op_err.p.offset;
    regs.fos = r->op_err.p.segment;
    memcpy( regs.st_space, r->reg, sizeof( r->reg ) );
    ptrace( PTRACE_SETFPREGS, pid, NULL, &regs );
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

trap_retval ReqWrite_cpu( void )
{
    WriteCPU( GetInPtr( sizeof( write_cpu_req ) ) );
    return( 0 );
}

trap_retval ReqWrite_fpu( void )
{
    WriteFPU( GetInPtr( sizeof( write_fpu_req ) ) );
    return( 0 );
}

trap_retval ReqWrite_regs( void )
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

static void SetDR6( u_long val )
{
    ptrace( PTRACE_POKEUSER, pid, O_DEBUGREG( 6 ), (void *)val );
}

static void SetDR7( u_long val )
{
    ptrace( PTRACE_POKEUSER, pid, O_DEBUGREG(7), (void *)val );
}

static u_long SetDRn( int i, u_long linear, long type )
{
    ptrace( PTRACE_POKEUSER, pid, O_DEBUGREG( i ), (void *)linear );
    return( ( type << DR7_RWLSHIFT( i ) )
//        | ( DR7_GEMASK << DR7_GLSHIFT( i ) ) | DR7_GE
          | ( DR7_LEMASK << DR7_GLSHIFT( i ) ) | DR7_LE );
}

void ClearDebugRegs( void )
{
    int i;

    for( i = 0; i < 4; i++)
        SetDRn( i, 0, 0 );
    SetDR6( 0 );
    SetDR7( 0 );
}

int SetDebugRegs( void )
{
    int         needed,i,dr;
    u_long      dr7;
    watch_point *wp;

    needed = 0;
    for( i = 0; i < wpCount; i++)
        needed += wpList[i].dregs;
    if( needed > 4 )
        return( FALSE );
    dr  = 0;
    dr7 = 0;
    for( i = 0, wp = wpList; i < wpCount; i++, wp++ ) {
        dr7 |= SetDRn( dr, wp->linear, DRLen( wp->len ) | DR7_BWR );
        dr++;
        if( wp->dregs == 2 ) {
            dr7 |= SetDRn( dr, wp->linear+wp->len, DRLen( wp->len ) | DR7_BWR );
            dr++;
        }
    }
    SetDR7( dr7 );
    return( TRUE );
}

int CheckWatchPoints( void )
{
    u_long  value;
    int     i;

    for( i = 0; i < wpCount; i++ ) {
        ReadMem( pid, &value, wpList[i].loc.offset, sizeof( value ) );
        if( value != wpList[i].value ) {
            return( TRUE );
        }
    }
    return( FALSE );
}

trap_retval ReqSet_watch( void )
{
    set_watch_req   *acc;
    set_watch_ret   *ret;
    u_long          value;
    watch_point     *curr;
    u_long          linear;
    unsigned        i,needed;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->multiplier = 100000;
    ret->err = 1;
    if( wpCount < MAX_WP ) {
        ret->err = 0;
        curr = wpList + wpCount;
        curr->loc.segment = acc->watch_addr.segment;
        curr->loc.offset = acc->watch_addr.offset;
        ReadMem( pid, &value, acc->watch_addr.offset, sizeof( dword ) );
        curr->value = value;
        curr->len = acc->size;
        wpCount++;
        curr->linear = linear = acc->watch_addr.offset;
        curr->linear &= ~(curr->len-1);
        curr->dregs = (linear & (curr->len-1) ) ? 2 : 1;
        needed = 0;
        for( i = 0; i < wpCount; ++i ) {
            needed += wpList[ i ].dregs;
        }
        if( needed <= 4 ) ret->multiplier |= USING_DEBUG_REG;
    }
    return( sizeof( *ret ) );
}

trap_retval ReqClear_watch( void )
{
    clear_watch_req *acc;
    watch_point     *dst;
    watch_point     *src;
    int             i;

    acc = GetInPtr( 0 );
    dst = src = wpList;
    for( i = 0; i < wpCount; i++ ) {
        if( src->loc.segment != acc->watch_addr.segment
                || src->loc.offset != acc->watch_addr.offset ) {
            dst->loc.offset = src->loc.offset;
            dst->loc.segment = src->loc.segment;
            dst->value = src->value;
            dst++;
        }
        src++;
    }
    wpCount--;
    return( 0 );
}

trap_retval ReqRead_io( void )
{
    read_io_req *acc;
    void        *ret;
    trap_elen   len;

    /* Perform I/O on the target machine on behalf of the debugger.
     * Since there are no kernel APIs in Linux to do this, we just
     * enable IOPL and use regular I/O. We will bail if we can't get
     * IOPL=3, so the debugger trap file will need to be run as root
     * before it can be used for I/O access.
     */
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
#ifdef __WATCOMC__
    if( iopl( 3 ) == 0 ) {
        len = acc->len;
        switch( len ) {
        case 1:
            *((unsigned_8*)ret) = inpb( acc->IO_offset );
            break;
        case 2:
            *((unsigned_16*)ret) = inpw( acc->IO_offset );
            break;
        case 4:
            *((unsigned_32*)ret) = inpd( acc->IO_offset );
            break;
        }
    } else {
        len = 0;
    }
#else
    len = 0;
#endif
    return( len );
}

trap_retval ReqWrite_io( void )
{
    write_io_req    *acc;
    write_io_ret    *ret;
    void            *data;
    trap_elen       len;

    /* Perform I/O on the target machine on behalf of the debugger.
     * Since there are no kernel APIs in Linux to do this, we just
     * enable IOPL and use regular I/O. We will bail if we can't get
     * IOPL=3, so the debugger trap file will need to be run as root
     * before it can be used for I/O access.
     */
    acc = GetInPtr( 0 );
    data = GetInPtr( sizeof( *acc ) );
    len = GetTotalSize() - sizeof( *acc );
    ret = GetOutPtr( 0 );
#ifdef __WATCOMC__
    if( iopl( 3 ) == 0 ) {
        ret->len = len;
        switch( len ) {
        case 1:
            outpb( acc->IO_offset, *((unsigned_8*)data) );
            break;
        case 2:
            outpw( acc->IO_offset, *((unsigned_16*)data) );
            break;
        case 4:
            outpd( acc->IO_offset, *((unsigned_32*)data) );
            break;
        }
    } else {
        ret->len = 0;
    }
#else
    ret->len = 0;
#endif
    return( sizeof( *ret ) );
}

trap_retval ReqGet_sys_config( void )
{
    get_sys_config_ret  *ret;

    ret = GetOutPtr( 0 );
    ret->sys.os = MAD_OS_LINUX;

    // TODO: Detect OS version (kernel version?)!
    ret->sys.osmajor = 1;
    ret->sys.osminor = 0;

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

trap_retval ReqMachine_data( void )
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
