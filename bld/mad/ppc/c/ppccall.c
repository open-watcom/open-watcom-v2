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
* Description:  PowerPC call stack unwinding. Inspired by AXP version.
*
****************************************************************************/


#include <stddef.h>
#include <string.h>
#include "ppc.h"
#include "madregs.h"

/* Implementation Notes:
 *
 * This code is designed to work with the SVR4 PowerPC ABI. Support for
 * other ABIs is likely to require changes.
 *
 * The stack unwinding code was heavily 'inspired' by the Alpha AXP version,
 * as the PowerPC architecture is far closer to AXP than to x86. There is
 * however one very significant difference. The AXP code relies on procedure
 * descriptors - the ABI defines a machinery which given an execution
 * address, determines the start and end of a function. PowerPC has no such
 * thing, hence a bit more heuristic approach is required. When unwinding
 * code that has symbols, the debugger will help us by guessing (with very
 * high degree of accuracy) the function entry point.
 */


mad_string              DIGENTRY MICallStackGrowsUp( void )
{
    return( MS_FAIL );
}

const mad_string        *DIGENTRY MICallTypeList( void )
{
    static const mad_string list[] = { MAD_MSTR_NIL };

    return( list );
}

mad_status      DIGENTRY MICallBuildFrame( mad_string call, address ret, address rtn, mad_registers const *in, mad_registers *out )
{
    call = call;
    out->ppc = in->ppc;
    //NYI: 64 bit
    out->ppc.lr.u._32[I64LO32] = ret.mach.offset;
    out->ppc.iar.u._32[I64LO32] = rtn.mach.offset;
    return( MS_OK );
}

const mad_reg_info      *DIGENTRY MICallReturnReg( mad_string call, address rtn )
{
    call = call; rtn = rtn;

    return( &RegList[IDX_r3].info );
}

const mad_reg_info      **DIGENTRY MICallParmRegList( mad_string call, address rtn )
{
    static const mad_reg_info *list[] = {
        &RegList[IDX_r3].info, &RegList[IDX_r4].info, &RegList[IDX_r5].info,
        &RegList[IDX_r6].info, &RegList[IDX_r7].info, &RegList[IDX_r8].info,
        &RegList[IDX_r9].info, &RegList[IDX_r10].info,
        NULL };

    call = call; rtn = rtn;

    return( list );
}

#define NO_OFF  (~(addr_off)0)

unsigned        DIGENTRY MICallUpStackSize( void )
{
    return( sizeof( mad_call_up_data ) );
}

mad_status      DIGENTRY MICallUpStackInit( mad_call_up_data *cud, const mad_registers *mr )
{
    cud->lr = mr->ppc.lr.u._32[I64LO32];
    cud->sp = mr->ppc.u1.sp.u._32[I64LO32];
    cud->fp = mr->ppc.r31.u._32[I64LO32];    // NYI: can float around
    cud->first_frame = TRUE;
    return( MS_OK );
}

static int GetAnOffset( addr_off in, addr_off *off )
{
    address     a;
    int         rc;

    memset( &a, 0, sizeof( a ) );
    a.mach.offset = in;
    rc = MCReadMem( a, sizeof( *off ), off ) == sizeof( *off );
    if( rc ) {
        CONV_BE_32( *off );     // NYI: dynamic endian switching
    }
    return( rc );
}

/* The stack pointer (sp) is r1 by convention, and frame pointer (fp)
 * is r31 if applicable. Because there are no push and pop instructions,
 * the sp value typically doesn't change during the lifetime of a function.
 * These assignments are defined by the SVR4 ABI and most other PowerPC
 * operating systems use these assignments as well. This is the typical
 * stack layout as defined by the SVR4 ABI:
 *
 *          |                   |   High address
 *          |    LR save word   |
 *          +-------------------+
 *          |     Back chain    | <-+
 *          +-------------------+   |
 *          |   FPR save area   |   |
 *          +-------------------+   |
 *          |   GPR save area   |   |
 *          +-------------------+   |
 *          |    CR save area   |   |
 *          +-------------------+   |
 *          |  Local variables  |   |
 *          +-------------------+   |
 *          |   Dynamic area    |   |
 *          +-------------------+   |
 *          |  Callee arguments |   |
 *          +-------------------+   |
 *          |    LR save word   |   |
 *          +-------------------+   |
 *  sp ->   |     Back chain    | --+
 *          +-------------------+
 *
 * Notes:
 *  - Most of the stack items are optional except for the back chain.
 *  - The stack must be quadword (ie. 16-byte) aligned.
 *  - The LR save word just above where sp points is for the callee
 *    (if there is one), not for the current function!
 *  - The Link Register itself may not hold return address of current
 *    function, but rather that of a callee.
 *  - The back chain and sp must be updated atomically, either with
 *    stwu or stwux instruction.
 *
 * Once we've figured out the current frame's sp and lr values, we're
 * home dry because the back chain and LR save word values must be valid.
 * Determining current frame's sp and lr may not be so simple though, because:
 * - Stack frame may not have been fully established if we're right in
 *   the middle of a function's prologue.
 * - For functions with dynamic stack allocation (ie. using alloca()),
 *   sp can't be used to unwind stack, fp must be used instead.
 * - Return address may be either in lr or on the stack.
 * - If we don't know where the current function starts, we're going to
 *   have serious trouble figuring out what the return address is.
 */

mad_status      DIGENTRY MICallUpStackLevel( mad_call_up_data *cud,
                                const address *start,
                                unsigned rtn_characteristics,
                                long return_disp,
                                const mad_registers *in,
                                address *execution,
                                address *frame,
                                address *stack,
                                mad_registers **out )
{
    mad_disasm_data     dd;
    mad_status          ms;
    address             curr;
    addr_off            prev_lr_off;
    addr_off            prev_fp_off;
    addr_off            frame_size;
    addr_off            frame_start;
    addr_off            proc_end;
    dis_register        lr_save_gpr;

    return_disp = return_disp;

    rtn_characteristics = rtn_characteristics;
    in = in;
    *out = NULL;
    if( cud->lr == 0 ) return( MS_FAIL );
    if( cud->sp == 0 ) return( MS_FAIL );

    frame_size = 0;
    frame_start = cud->sp;
    prev_lr_off = NO_OFF;
    prev_fp_off = NO_OFF;
    curr = *execution;
    curr.mach.offset = start->mach.offset;
    /* Assume prolog no larger than 16 instructions; this might not be enough */
    proc_end = start->mach.offset + 64;
    if( curr.mach.offset == 0 ) return( MS_FAIL );
    lr_save_gpr = -1;
    for( ;; ) {
        if( curr.mach.offset >= execution->mach.offset ) break;
        if( curr.mach.offset >= proc_end ) break;
        ms = DisasmOne( &dd, &curr, 0 );
        if( ms != MS_OK ) return( ms );
        if( curr.mach.offset == start->mach.offset + sizeof( unsigned_32 ) ) {
            /* first instruction is usually 'stwu sp, -framesize(sp)' */
            /* NYI: it could be stwux, and it needn't be the first instruction */
            if( dd.ins.type != DI_PPC_stwu ) return( MS_FAIL );
            frame_size = -dd.ins.op[1].value;
        }
        switch( dd.ins.type ) {
        /* track fp saves */
        case DI_PPC_stw:
        case DI_PPC_stwu:
            if( dd.ins.op[0].base == DR_PPC_r31 ) {
                prev_fp_off = dd.ins.op[1].value;
            }
            if( dd.ins.op[0].base == lr_save_gpr ) {
                prev_lr_off = dd.ins.op[1].value;
                lr_save_gpr = -1;
            }
            break;
        /* track lr saves (those have go through a scratch GPR) */
        case DI_PPC_mfspr:
            if( dd.ins.op[1].value == 8 ) {
                lr_save_gpr = dd.ins.op[0].base;
            }
            break;
        /* track moves from sp to fp */
        case DI_PPC_or:
            /* look for 'mr r31, sp' */
            if( (dd.ins.op[0].base == DR_PPC_r31)
                && (dd.ins.op[1].base == DR_PPC_r1)
                && (dd.ins.op[2].base == DR_PPC_r1) ) {
                frame_start = cud->fp;
            }
            break;
        }
    }
    if( frame_start == 0 ) return( MS_FAIL );
    if( cud->first_frame ) {
        cud->first_frame = FALSE;
        cud->sp = frame_start + frame_size;
        cud->fp = frame_start + frame_size;
        if( prev_lr_off != NO_OFF ) {
            if( !GetAnOffset( frame_start + prev_lr_off, &cud->lr ) ) return( MS_FAIL );
        }   /* else return address in lr is still valid */
    } else {
        if( !GetAnOffset( frame_start, &cud->sp ) ) return( MS_FAIL );
        if( !GetAnOffset( cud->sp + sizeof( unsigned_32 ), &cud->lr ) ) return( MS_FAIL );
        if( !GetAnOffset( frame_start + prev_fp_off, &cud->fp ) ) return( MS_FAIL );
    }
    if( cud->lr == 0 ) return( MS_FAIL );
    if( cud->sp <= frame_start ) return( MS_FAIL );
    stack->mach.offset = cud->sp;
    execution->mach.offset = cud->lr;
//    if( VariableFrame( execution->mach.offset ) ) {
//        frame->mach.offset = cud->fp;
//    } else {
        frame->mach.offset = cud->sp;
//    }
    return( MS_OK );
}
