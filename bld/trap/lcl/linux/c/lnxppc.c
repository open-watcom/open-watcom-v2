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
* Description:  Linux debugger trap file PowerPC specific functions.
*
****************************************************************************/


#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <asm/ptrace.h>
#include "trpimp.h"
#include "trperr.h"
#include "mad.h"
#include "madregs.h"
#include "lnxcomm.h"

/* Implementation notes:
 *
 * - This code is big endian, for 32-bit PowerPC systems. It could be extended
 *   to support 64-bit machines relatively easily.
 *
 * - Some PowerPC systems/CPUs/kernels appear to have problems setting/clearing
 *   breakpoints. This could very well be caused by cache coherency problems;
 *   the I-cache and D-cache are not guaranteed to be consistent. We should
 *   be able to manually force the caches to be synced.
 */

/* Macros to get at GP/FP registers based on their index; useful in loops */
#define TRANS_GPREG_32( mr, idx ) (*((unsigned_32 *)(&(mr->r0.u._32[I64LO32])) + (2 * idx)))
#define TRANS_FPREG_LO( mr, idx ) (*((unsigned_32 *)(&(mr->f0.u64.u._32[I64LO32])) + (2 * idx)))
#define TRANS_FPREG_HI( mr, idx ) (*((unsigned_32 *)(&(mr->f0.u64.u._32[I64HI32])) + (2 * idx)))

static void ReadCPU( struct ppc_mad_registers *r )
{
    int         i;

    memset( r, 0, sizeof( *r ) );
    /* Read GPRs */
    for( i = 0; i < 32; ++i ) {
        TRANS_GPREG_32( r, i ) = ptrace( PTRACE_PEEKUSER, pid, i * REGSIZE, 0 );
    }
    /* Read FPRs */
    for( i = 0; i < 32; ++i ) {
        TRANS_FPREG_HI( r, i ) = ptrace( PTRACE_PEEKUSER, pid, (PT_FPR0 + i * 2) * REGSIZE, 0 );
        TRANS_FPREG_LO( r, i ) = ptrace( PTRACE_PEEKUSER, pid, (PT_FPR0 + i * 2 + 1) * REGSIZE, 0 );
    }
    /* Read SPRs */
    r->iar.u._32[I64LO32] = ptrace( PTRACE_PEEKUSER, pid, PT_NIP * REGSIZE, 0 );
    r->msr.u._32[I64LO32] = ptrace( PTRACE_PEEKUSER, pid, PT_MSR * REGSIZE, 0 );
    r->ctr.u._32[I64LO32] = ptrace( PTRACE_PEEKUSER, pid, PT_CTR * REGSIZE, 0 );
    r->lr.u._32[I64LO32]  = ptrace( PTRACE_PEEKUSER, pid, PT_LNK * REGSIZE, 0 );
    r->xer                = ptrace( PTRACE_PEEKUSER, pid, PT_XER * REGSIZE, 0 );
    r->cr                 = ptrace( PTRACE_PEEKUSER, pid, PT_CCR * REGSIZE, 0 );
    r->fpscr              = ptrace( PTRACE_PEEKUSER, pid, PT_FPSCR * REGSIZE, 0 );
    last_eip = r->iar.u._32[I64LO32];
}

trap_retval ReqRead_regs( void )
{
    mad_registers   *mr;

    mr = GetOutPtr( 0 );
    ReadCPU( &mr->ppc );
    return( sizeof( mr->ppc ) );
}

static void WriteCPU( struct ppc_mad_registers *r )
{
    int         i;

    /* Write SPRs */
    ptrace( PTRACE_POKEUSER, pid, PT_NIP * REGSIZE, (void *)(r->iar.u._32[I64LO32]) );
    ptrace( PTRACE_POKEUSER, pid, PT_MSR * REGSIZE, (void *)(r->msr.u._32[I64LO32]) );
    ptrace( PTRACE_POKEUSER, pid, PT_CTR * REGSIZE, (void *)(r->ctr.u._32[I64LO32]) );
    ptrace( PTRACE_POKEUSER, pid, PT_LNK * REGSIZE, (void *)(r->lr.u._32[I64LO32]) );
    ptrace( PTRACE_POKEUSER, pid, PT_CCR * REGSIZE, (void *)r->cr );
    ptrace( PTRACE_POKEUSER, pid, PT_XER * REGSIZE, (void *)r->xer );
    /* Write GPRs */
    for( i = 0; i < 32; ++i ) {
        ptrace( PTRACE_POKEUSER, pid, i * REGSIZE, (void *)TRANS_GPREG_32( r, i ) );
    }
    /* Write FPRs */
    for( i = 0; i < 32; ++i ) {
        ptrace( PTRACE_POKEUSER, pid, (PT_FPR0 + i * 2) * REGSIZE, TRANS_FPREG_HI( r, i ) );
        ptrace( PTRACE_POKEUSER, pid, (PT_FPR0 + i * 2 + 1) * REGSIZE, TRANS_FPREG_LO( r, i ) );
    }
    ptrace( PTRACE_POKEUSER, pid, PT_FPSCR * REGSIZE, (void *)(r->fpscr) );
}

trap_retval ReqWrite_regs( void )
{
    mad_registers   *mr;

    mr = GetInPtr( sizeof( write_regs_req ) );
    WriteCPU( &mr->ppc );
    return( 0 );
}

trap_retval ReqSet_watch( void )
{
    set_watch_req   *acc;
    set_watch_ret   *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->multiplier = 100000;
    ret->err = 1;
    return( sizeof( *ret ) );
}

trap_retval ReqClear_watch( void )
{
    clear_watch_req *acc;

    acc = GetInPtr( 0 );
    return( 0 );
}

/* We do not support I/O port access on PowerPC, although if we really
 * wanted to, we could access memory mapped ISA/PCI I/O ports on systems
 * where those are provided. Would require root privileges.
 */
trap_retval ReqRead_io( void )
{
    read_io_req *acc;
    void        *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    return( 0 );
}

trap_retval ReqWrite_io( void )
{
    write_io_req    *acc;
    write_io_ret    *ret;
    void            *data;

    acc = GetInPtr( 0 );
    data = GetInPtr( sizeof( *acc ) );
    ret = GetOutPtr( 0 );
    ret->len = 0;
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

    ret->sys.cpu = PPC_604;
    ret->sys.fpu = 0;
    ret->sys.huge_shift = 3;
    ret->sys.mad = MAD_PPC;
    CONV_LE_16( ret->sys.mad );
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
    *data = 0;
    return( sizeof( *ret ) + sizeof( *data ) );
}

// TODO: fix up ordering/contents
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
