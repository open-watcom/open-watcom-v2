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
#include "stdnt.h"
#include "madregs.h"

// position in Windows CONTEXT, 
// it is offset in FXSAVE/FXRSTOR memory structure
#define CONTEXT_MXCSR    24
#define CONTEXT_XMM      (10 * 16) 

#if defined( MD_x86 )
static void ReadCPU( struct x86_cpu *r, CONTEXT *con )
{
    r->eax = con->Eax;
    r->ebx = con->Ebx;
    r->ecx = con->Ecx;
    r->edx = con->Edx;
    r->esi = con->Esi;
    r->edi = con->Edi;
    r->esp = con->Esp;
    r->ebp = con->Ebp;
    r->eip = con->Eip;
    r->efl = con->EFlags;
    r->ds = con->SegDs;
    r->cs = con->SegCs;
    r->es = con->SegEs;
    r->ss = con->SegSs;
    r->fs = con->SegFs;
    r->gs = con->SegGs;
}

static void WriteCPU( struct x86_cpu *r, CONTEXT *con )
{
    con->Eax = r->eax;
    con->Ebx = r->ebx;
    con->Ecx = r->ecx;
    con->Edx = r->edx;
    con->Esi = r->esi;
    con->Edi = r->edi;
    con->Esp = r->esp;
    con->Ebp = r->ebp;
    con->Eip = r->eip;
    con->EFlags = r->efl;
    con->SegDs = r->ds;
    con->SegCs = r->cs;
    con->SegEs = r->es;
    con->SegSs = r->ss;
    con->SegFs = r->fs;
    con->SegGs = r->gs;
}
#endif

unsigned ReqRead_cpu( void )
{
#if defined( MD_x86 )
    trap_cpu_regs   *regs;
    CONTEXT         con;
    thread_info     *ti;

    regs = GetOutPtr( 0 );

    memset( regs, 0, sizeof( *regs ) );
    if( DebugeePid ) {
        ti = FindThread( DebugeeTid );
        MyGetThreadContext( ti, &con );
        ReadCPU( ( void * ) regs, &con );
    }
    return( sizeof( *regs ) );
#else
    return( 0 );
#endif
}

unsigned ReqRead_fpu( void )
{
#if defined( MD_x86 )
    CONTEXT         con;
    read_fpu_ret    *ret;
    thread_info     *ti;

    ret = GetOutPtr( 0 );

    memset( ret, 0, sizeof( *ret ) );
    if( DebugeePid ) {
        ti = FindThread( DebugeeTid );
        MyGetThreadContext( ti, &con );
        memcpy( ret, &con.FloatSave, sizeof( *ret ) );
    }
    return( sizeof( *ret ) );
#else
    return( 0 );
#endif
}

unsigned ReqWrite_cpu( void )
{
#if defined( MD_x86 )
    CONTEXT         con;
    thread_info     *ti;
    trap_cpu_regs   *regs;

    if( DebugeePid == 0 ) {
        return( 0 );
    }
    regs = GetInPtr( sizeof( write_cpu_req ) );

    ti = FindThread( DebugeeTid );
    MyGetThreadContext( ti, &con );
    WriteCPU( ( void * )regs, &con );
    MySetThreadContext( ti, &con );
#endif
    return( 0 );
}

unsigned ReqWrite_fpu( void )
{
#if defined( MD_x86 )
    trap_fpu_regs   *fpu;
    CONTEXT         con;
    thread_info     *ti;

    if( DebugeePid == 0 ) {
        return( 0 );
    }
    fpu = GetInPtr( sizeof( write_fpu_req ) );
    ti = FindThread( DebugeeTid );
    MyGetThreadContext( ti, &con );
    memcpy( &con.FloatSave, fpu, sizeof( *fpu ) );
    MySetThreadContext( ti, &con );
#endif
    return( 0 );
}

unsigned ReqRead_regs( void )
{
    mad_registers   _WCUNALIGNED *mr;
    CONTEXT         con;
    thread_info     *ti;

    mr = GetOutPtr( 0 );

#if defined( MD_x86 )
    memset( mr, 0, sizeof( mr->x86 ) );
#elif defined( MD_axp )
    memset( mr, 0, sizeof( mr->axp ) );
#elif defined( MD_ppc )
    memset( mr, 0, sizeof( mr->ppc ) );
#else
    #error ReqRead_regs not configured
#endif
    if( DebugeePid ) {
        ti = FindThread( DebugeeTid );
        MyGetThreadContext( ti, &con );
#if defined( MD_x86 )
        ReadCPU( &mr->x86.cpu, &con );
        memcpy( &mr->x86.fpu, &con.FloatSave, sizeof( mr->x86.fpu ) );
        memcpy( mr->x86.xmm.xmm, &con.ExtendedRegisters[ CONTEXT_XMM ],
            sizeof( mr->x86.xmm.xmm ) );
        mr->x86.xmm.mxcsr = con.ExtendedRegisters[ CONTEXT_MXCSR ];
#elif defined( MD_axp )
        memcpy( &mr->axp.r, &con, sizeof( mr->axp.r ) );
        mr->axp.pal.nt.fir      = *( unsigned_64 * ) & con.Fir;
        mr->axp.pal.nt.softfpcr = *( unsigned_64 * ) & con.SoftFpcr;
        mr->axp.pal.nt.psr      = con.Psr;
        mr->axp.active_pal      = PAL_nt;
#elif defined( MD_ppc )
        memcpy( &mr->ppc.f0, &con.Fpr0, sizeof( double )* 32 );
        mr->ppc.r0.u._32[0] = con.Gpr0;
        mr->ppc.r1.u._32[0] = con.Gpr1;
        mr->ppc.r2.u._32[0] = con.Gpr2;
        mr->ppc.r3.u._32[0] = con.Gpr3;
        mr->ppc.r4.u._32[0] = con.Gpr4;
        mr->ppc.r5.u._32[0] = con.Gpr5;
        mr->ppc.r6.u._32[0] = con.Gpr6;
        mr->ppc.r7.u._32[0] = con.Gpr7;
        mr->ppc.r8.u._32[0] = con.Gpr8;
        mr->ppc.r9.u._32[0] = con.Gpr9;
        mr->ppc.r10.u._32[0] = con.Gpr10;
        mr->ppc.r11.u._32[0] = con.Gpr11;
        mr->ppc.r12.u._32[0] = con.Gpr12;
        mr->ppc.r13.u._32[0] = con.Gpr13;
        mr->ppc.r14.u._32[0] = con.Gpr14;
        mr->ppc.r15.u._32[0] = con.Gpr15;
        mr->ppc.r16.u._32[0] = con.Gpr16;
        mr->ppc.r17.u._32[0] = con.Gpr17;
        mr->ppc.r18.u._32[0] = con.Gpr18;
        mr->ppc.r19.u._32[0] = con.Gpr19;
        mr->ppc.r20.u._32[0] = con.Gpr20;
        mr->ppc.r21.u._32[0] = con.Gpr21;
        mr->ppc.r22.u._32[0] = con.Gpr22;
        mr->ppc.r23.u._32[0] = con.Gpr23;
        mr->ppc.r24.u._32[0] = con.Gpr24;
        mr->ppc.r25.u._32[0] = con.Gpr25;
        mr->ppc.r26.u._32[0] = con.Gpr26;
        mr->ppc.r27.u._32[0] = con.Gpr27;
        mr->ppc.r28.u._32[0] = con.Gpr28;
        mr->ppc.r29.u._32[0] = con.Gpr29;
        mr->ppc.r30.u._32[0] = con.Gpr30;
        mr->ppc.r31.u._32[0] = con.Gpr31;

        mr->ppc.lr.u._32[0] = con.Lr;
        mr->ppc.ctr.u._32[0] = con.Ctr;
        mr->ppc.iar.u._32[0] = con.Iar;
        mr->ppc.msr.u._32[0] = con.Msr;
        mr->ppc.cr = con.Cr;
        mr->ppc.xer = con.Xer;
        mr->ppc.fpscr = *( unsigned_32 * ) & con.Fpscr; //NYI: is this right?
#else
        #error ReqRead_regs not configured
#endif
    }
#if defined( MD_x86 )
    return( sizeof( mr->x86 ) );
#elif defined( MD_axp )
    return( sizeof( mr->axp ) );
#elif defined( MD_ppc )
    return( sizeof( mr->ppc ) );
#else
    #error ReqRead_regs not configured
#endif
}

unsigned ReqWrite_regs( void )
{
    CONTEXT         con;
    thread_info     *ti;
    mad_registers   _WCUNALIGNED *mr;

    if( DebugeePid == 0 ) {
        return( 0 );
    }
    mr = GetInPtr( sizeof( write_regs_req ) );

    ti = FindThread( DebugeeTid );
    MyGetThreadContext( ti, &con );
#if defined( MD_x86 )
    WriteCPU( &mr->x86.cpu, &con );
    memcpy( &con.FloatSave, &mr->x86.fpu, sizeof( mr->x86.fpu ) );
    memcpy( &con.ExtendedRegisters[ CONTEXT_XMM ], 
            mr->x86.xmm.xmm, sizeof( mr->x86.xmm.xmm ) );
    con.ExtendedRegisters[ CONTEXT_MXCSR ] = mr->x86.xmm.mxcsr;
#elif defined( MD_axp )
    memcpy( &con, &mr->axp.r, sizeof( mr->axp.r ) );
    *( unsigned_64 * ) & con.Fir            = mr->axp.pal.nt.fir;
    *( unsigned_64 * ) & con.SoftFpcr       = mr->axp.pal.nt.softfpcr;
    con.Psr                             = mr->axp.pal.nt.psr;
#elif defined( MD_ppc )
    memcpy( &con.Fpr0, &mr->ppc.f0, sizeof( double )* 32 );
    con.Gpr0 = mr->ppc.r0.u._32[0];
    con.Gpr1 = mr->ppc.r1.u._32[0];
    con.Gpr2 = mr->ppc.r2.u._32[0];
    con.Gpr3 = mr->ppc.r3.u._32[0];
    con.Gpr4 = mr->ppc.r4.u._32[0];
    con.Gpr5 = mr->ppc.r5.u._32[0];
    con.Gpr6 = mr->ppc.r6.u._32[0];
    con.Gpr7 = mr->ppc.r7.u._32[0];
    con.Gpr8 = mr->ppc.r8.u._32[0];
    con.Gpr9 = mr->ppc.r9.u._32[0];
    con.Gpr10 = mr->ppc.r10.u._32[0];
    con.Gpr11 = mr->ppc.r11.u._32[0];
    con.Gpr12 = mr->ppc.r12.u._32[0];
    con.Gpr13 = mr->ppc.r13.u._32[0];
    con.Gpr14 = mr->ppc.r14.u._32[0];
    con.Gpr15 = mr->ppc.r15.u._32[0];
    con.Gpr16 = mr->ppc.r16.u._32[0];
    con.Gpr17 = mr->ppc.r17.u._32[0];
    con.Gpr18 = mr->ppc.r18.u._32[0];
    con.Gpr19 = mr->ppc.r19.u._32[0];
    con.Gpr20 = mr->ppc.r20.u._32[0];
    con.Gpr21 = mr->ppc.r21.u._32[0];
    con.Gpr22 = mr->ppc.r22.u._32[0];
    con.Gpr23 = mr->ppc.r23.u._32[0];
    con.Gpr24 = mr->ppc.r24.u._32[0];
    con.Gpr25 = mr->ppc.r25.u._32[0];
    con.Gpr26 = mr->ppc.r26.u._32[0];
    con.Gpr27 = mr->ppc.r27.u._32[0];
    con.Gpr28 = mr->ppc.r28.u._32[0];
    con.Gpr29 = mr->ppc.r29.u._32[0];
    con.Gpr30 = mr->ppc.r30.u._32[0];
    con.Gpr31 = mr->ppc.r31.u._32[0];

    con.Lr = mr->ppc.lr.u._32[0];
    con.Ctr = mr->ppc.ctr.u._32[0];
    con.Iar = mr->ppc.iar.u._32[0];
    con.Msr = mr->ppc.msr.u._32[0];
    con.Cr = mr->ppc.cr;
    con.Xer = mr->ppc.xer;
    *( unsigned_32 * ) & con.Fpscr = mr->ppc.fpscr; //NYI: is this right?
#else
    #error ReqWrite_regs not configured
#endif
    MySetThreadContext( ti, &con );
    return( 0 );
}

DWORD AdjustIP( CONTEXT *con, int adjust )
{
#if defined( MD_x86 )
    con->Eip += adjust;
    return( con->Eip );
#elif defined( MD_axp )
    //NYI: 64 bit
    ( ( unsigned_64 * ) & con->Fir )->u._32[0] += adjust;
    return( ( ( unsigned_64 * ) & con->Fir )->u._32[0] );
#elif defined( MD_ppc )
    con->Iar += adjust;
    return( con->Iar );
#else
    #error AdjustIP not configured
#endif
}

void SetIP( CONTEXT *con, DWORD new )
{
#if defined( MD_x86 )
    con->Eip = new;
#elif defined( MD_axp )
    //NYI: 64 bit
    ( ( unsigned_64 * ) & con->Fir )->u._32[0] = new;
#elif defined( MD_ppc )
    con->Iar = new;
#else
    #error AdjustIP not configured
#endif
}
