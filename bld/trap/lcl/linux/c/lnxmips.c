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
* Description:  Linux debugger trap file MIPS specific functions.
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
#include "exeelf.h"
#include "lnxcomm.h"

/* Implementation notes:
 *
 * - This code is little endian, for 32-bit MIPS systems. It could be extended
 *   to support 64-bit machines relatively easily, as well as big endian
 *   systems.
 *
 */

/* Macros to get at GP/FP registers based on their index; useful in loops */
#define TRANS_GPREG_32( mr, idx ) (*((unsigned_32 *)(&(mr->r0.u._32[I64LO32])) + (2 * idx)))
#define TRANS_FPREG_LO( mr, idx ) (*((unsigned_32 *)(&(mr->f0.u64.u._32[I64LO32])) + (2 * idx)))
#define TRANS_FPREG_HI( mr, idx ) (*((unsigned_32 *)(&(mr->f0.u64.u._32[I64HI32])) + (2 * idx)))

static void ReadCPU( struct mips_mad_registers *r )
{
    int         i;

    memset( r, 0, sizeof( *r ) );
    /* Read GPRs */
    for( i = 0; i < 32; ++i ) {
        TRANS_GPREG_32( r, i ) = ptrace( PTRACE_PEEKUSER, pid, (void *)i, NULL );
    }
    /* Read FPRs */
    for( i = 0; i < 16; ++i ) {
        TRANS_FPREG_LO( r, i ) = ptrace( PTRACE_PEEKUSER, pid, (void *)(FPR_BASE + i * 2), NULL );
        TRANS_FPREG_HI( r, i ) = ptrace( PTRACE_PEEKUSER, pid, (void *)(FPR_BASE + i * 2 + 1), NULL );
    }
    /* Read special registers */
    r->pc.u._32[I64LO32]  = ptrace( PTRACE_PEEKUSER, pid, (void *)PC, NULL );
    r->lo                 = ptrace( PTRACE_PEEKUSER, pid, (void *)MMLO, NULL );
    r->hi                 = ptrace( PTRACE_PEEKUSER, pid, (void *)MMHI, NULL );
    r->fpcsr              = ptrace( PTRACE_PEEKUSER, pid, (void *)FPC_CSR, NULL );
    r->fpivr              = ptrace( PTRACE_PEEKUSER, pid, (void *)FPC_EIR, NULL );

    last_eip = r->pc.u._32[I64LO32];
}

trap_retval ReqRead_cpu( void )
{
    return( 0 );
}

trap_retval ReqRead_fpu( void )
{
    return( 0 );
}

trap_retval ReqRead_regs( void )
{
    mad_registers   *mr;

    mr = GetOutPtr( 0 );
    ReadCPU( &mr->mips );
    return( sizeof( mr->mips ) );
}

static void WriteCPU( struct mips_mad_registers *r )
{
    int         i;

    /* Write special registers */
    ptrace( PTRACE_POKEUSER, pid, (void *)PC, (void *)(r->pc.u._32[I64LO32]) );
    ptrace( PTRACE_POKEUSER, pid, (void *)MMLO, (void *)r->lo );
    ptrace( PTRACE_POKEUSER, pid, (void *)MMHI, (void *)r->hi );
    /* Write GPRs */
    for( i = 0; i < 32; ++i ) {
        ptrace( PTRACE_POKEUSER, pid, (void *)i, (void *)TRANS_GPREG_32( r, i ) );
    }
    /* Write FPRs */
    for( i = 0; i < 16; ++i ) {
        ptrace( PTRACE_POKEUSER, pid, (void *)(FPR_BASE + i * 2), (void *)TRANS_FPREG_LO( r, i ) );
        ptrace( PTRACE_POKEUSER, pid, (void *)(FPR_BASE + i * 2 + 1), (void *)TRANS_FPREG_HI( r, i ) );
    }
}

trap_retval ReqWrite_cpu( void )
{
    return( 0 );
}

trap_retval ReqWrite_fpu( void )
{
    return( 0 );
}

trap_retval ReqWrite_regs( void )
{
    mad_registers   *mr;

    mr = GetInPtr( sizeof( write_regs_req ) );
    WriteCPU( &mr->mips );
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

/* We do not support I/O port access on MIPS, although if we really
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

    ret->sys.cpu = MIPS_R3000;
    ret->sys.fpu = 0;
    ret->sys.huge_shift = 3;
    ret->sys.mad = MAD_MIPS;
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

const char *const ExceptionMsgs[33] = {
    "",
    TRP_QNX_hangup,
    TRP_QNX_user_interrupt,
    TRP_QNX_quit,
    TRP_EXC_illegal_instruction,
    TRP_QNX_trap,
    TRP_QNX_abort,
    TRP_EXC_access_violation "(SIGEMT)",
    TRP_QNX_floating_point_error,
    TRP_QNX_process_killed,
    TRP_QNX_bus_error,
    TRP_EXC_access_violation "(SIGSEGV)",
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
    "", /* SIGTSTP */
    TRP_QNX_process_continued,
    "", /* SIGTTIN */
    "", /* SIGTTOU */
    "", /* SIGVTALRM */
    "", /* SIGPROF */
    "", /* SIGXCPU */
    "", /* SIGXFSZ */
    ""
};
