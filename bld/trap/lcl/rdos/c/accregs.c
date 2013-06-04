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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdrdos.h"
#include "madregs.h"
#include "debug.h"

trap_retval ReqRead_cpu( void )
{
    trap_cpu_regs           *regs;
    struct TDebug           *obj;
    struct TDebugThread     *thread;

    regs = GetOutPtr( 0 );
    memset( regs, 0, sizeof( *regs ) );

    obj = GetCurrentDebug();
    if (obj) {
        thread = obj->CurrentThread;
        if( thread ) {
            regs->EAX = thread->Eax;
            regs->EBX = thread->Ebx;
            regs->ECX = thread->Ecx;
            regs->EDX = thread->Edx;
            regs->ESI = thread->Esi;
            regs->EDI = thread->Edi;
            regs->ESP = thread->Esp;
            regs->EBP = thread->Ebp;
            regs->EIP = thread->Eip;
            regs->EFL = thread->Eflags;
            regs->CR3 = thread->Cr3;
            regs->DS = thread->Ds;
            regs->CS = thread->Cs;
            regs->ES = thread->Es;
            regs->SS = thread->Ss;
            regs->FS = thread->Fs;
            regs->GS = thread->Gs;
        }
    }
            
    return( sizeof( *regs ) );
}

trap_retval ReqRead_fpu( void )
{
    trap_fpu_regs           *fpu;
    struct TDebug           *obj;
    struct TDebugThread     *thread;

    fpu = GetOutPtr( 0 );
    memset( fpu, 0, sizeof( *fpu ) );

    obj = GetCurrentDebug();
    if (obj) {
        thread = obj->CurrentThread;
        if( thread ) {
            fpu->control = thread->MathControl;
            fpu->status = thread->MathStatus;
            fpu->tag = thread->MathTag;
            fpu->ip_err[0] = thread->MathEip;
            fpu->ip_err[1] = thread->MathCs;
            fpu->op_err[0] = thread->MathDataOffs;
            fpu->op_err[1] = thread->MathDataSel;
            memcpy( fpu->reg, thread->St, 8 * 10 );
        }
    }

    return( sizeof( *fpu ) );
}

trap_retval ReqWrite_cpu( void )
{
    trap_cpu_regs           *regs;
    struct TDebug           *obj;
    struct TDebugThread     *thread;

    regs = GetInPtr( sizeof( write_cpu_req ) );

    obj = GetCurrentDebug();
    if (obj) {
        thread = obj->CurrentThread;
        if( thread ) {
            thread->Eax = regs->EAX;
            thread->Ebx = regs->EBX;
            thread->Ecx = regs->ECX;
            thread->Edx = regs->EDX;
            thread->Esi = regs->ESI;
            thread->Edi = regs->EDI;
            thread->Esp = regs->ESP;
            thread->Ebp = regs->EBP;
            thread->Eip = regs->EIP;
            thread->Eflags = regs->EFL;
            thread->Ds = regs->DS;
            thread->Cs = regs->CS;
            thread->Es = regs->ES;
            thread->Ss = regs->SS;
            thread->Fs = regs->FS;
            thread->Gs = regs->GS;

            WriteRegs( thread );
        }
    }
            
    return( 0 );
}

trap_retval ReqWrite_fpu( void )
{
    trap_fpu_regs           *fpu;
    struct TDebug           *obj;
    struct TDebugThread     *thread;

    fpu = GetInPtr( sizeof( write_fpu_req ) );

    obj = GetCurrentDebug();
    if (obj) {
        thread = obj->CurrentThread;
        if( thread ) {
            thread->MathControl = fpu->control;
            thread->MathStatus = fpu->status;
            thread->MathTag = fpu->tag;
            thread->MathEip = fpu->ip_err[0];
            thread->MathCs = fpu->ip_err[1];
            thread->MathDataOffs = fpu->op_err[0];
            thread->MathDataSel = fpu->op_err[1];
            memcpy( thread->St, fpu->reg, 8 * 10 );

            WriteRegs( thread );
        }
    }

    return( 0 );
}

trap_retval ReqRead_regs( void )
{
    mad_registers           _WCUNALIGNED *mr;
    struct TDebug           *obj;
    struct TDebugThread     *thread;

    mr = GetOutPtr( 0 );
    memset( mr, 0, sizeof( mr->x86 ) );

    obj = GetCurrentDebug();
    if (obj) {
        thread = obj->CurrentThread;
        if( thread ) {
            mr->x86.cpu.eax = thread->Eax;
            mr->x86.cpu.ebx = thread->Ebx;
            mr->x86.cpu.ecx = thread->Ecx;
            mr->x86.cpu.edx = thread->Edx;
            mr->x86.cpu.esi = thread->Esi;
            mr->x86.cpu.edi = thread->Edi;
            mr->x86.cpu.esp = thread->Esp;
            mr->x86.cpu.ebp = thread->Ebp;
            mr->x86.cpu.eip = thread->Eip;
            mr->x86.cpu.efl = thread->Eflags;
            mr->x86.cpu.cr3 = thread->Cr3;
            mr->x86.cpu.ds = thread->Ds;
            mr->x86.cpu.cs = thread->Cs;
            mr->x86.cpu.es = thread->Es;
            mr->x86.cpu.ss = thread->Ss;
            mr->x86.cpu.fs = thread->Fs;
            mr->x86.cpu.gs = thread->Gs;

            mr->x86.u.fpu.cw = thread->MathControl;
            mr->x86.u.fpu.sw = thread->MathStatus;
            mr->x86.u.fpu.tag = thread->MathTag;
            mr->x86.u.fpu.ip_err.p.offset = thread->MathEip;
            mr->x86.u.fpu.ip_err.p.segment = thread->MathCs;
            mr->x86.u.fpu.op_err.p.offset = thread->MathDataOffs;
            mr->x86.u.fpu.op_err.p.segment = thread->MathDataSel;
            memcpy( mr->x86.u.fpu.reg, thread->St, 8 * 10 );

        }
    }

    return( sizeof( mr->x86 ) );
}

trap_retval ReqWrite_regs( void )
{
    mad_registers           _WCUNALIGNED *mr;
    struct TDebug           *obj;
    struct TDebugThread     *thread;

    mr = GetInPtr( sizeof( write_regs_req ) );

    obj = GetCurrentDebug();
    if (obj) {
        thread = obj->CurrentThread;
        if( thread ) {
            thread->Eax = mr->x86.cpu.eax;
            thread->Ebx = mr->x86.cpu.ebx;
            thread->Ecx = mr->x86.cpu.ecx;
            thread->Edx = mr->x86.cpu.edx;
            thread->Esi = mr->x86.cpu.esi;
            thread->Edi = mr->x86.cpu.edi;
            thread->Esp = mr->x86.cpu.esp;
            thread->Ebp = mr->x86.cpu.ebp;
            thread->Eip = mr->x86.cpu.eip;
            thread->Eflags = mr->x86.cpu.efl;
            thread->Ds = mr->x86.cpu.ds;
            thread->Cs = mr->x86.cpu.cs;
            thread->Es = mr->x86.cpu.es;
            thread->Ss = mr->x86.cpu.ss;
            thread->Fs = mr->x86.cpu.fs;
            thread->Gs = mr->x86.cpu.gs;

            thread->MathControl = mr->x86.u.fpu.cw;
            thread->MathStatus = mr->x86.u.fpu.sw;
            thread->MathTag = mr->x86.u.fpu.tag;
            thread->MathEip = mr->x86.u.fpu.ip_err.p.offset;
            thread->MathCs = mr->x86.u.fpu.ip_err.p.segment;
            thread->MathDataOffs = mr->x86.u.fpu.op_err.p.offset;
            thread->MathDataSel = mr->x86.u.fpu.op_err.p.segment;
            memcpy( thread->St, mr->x86.u.fpu.reg, 8 * 10 );

            WriteRegs( thread );
        }
    }
    return( 0 );
}
