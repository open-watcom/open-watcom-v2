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


#include <stdlib.h>

#include "drwatcom.h"
#include "mad.h"
#include "madcli.h"
#include "mem.h"
#include "madsys1.h"


#if defined( _M_IX86 )
void GetSysConfig(void)
{
    SYSTEM_INFO sys;

    GetSystemInfo( &sys );
    switch( sys.dwProcessorType ) {
    case PROCESSOR_INTEL_386:
        SysConfig.cpu = X86_386;
        SysConfig.fpu = X86_387;
        break;
    case PROCESSOR_INTEL_486:
        SysConfig.cpu = X86_486;
        SysConfig.fpu = X86_487;
        break;
    case PROCESSOR_INTEL_PENTIUM:
        SysConfig.cpu = X86_586;
        SysConfig.fpu = X86_587;
        break;
    }
    SysConfig.os = MAD_OS_NT;
    SysConfig.mad = MAD_X86;
    SysConfig.osminor = _osminor;
    SysConfig.osmajor = _osmajor;
    SysConfig.huge_shift = 3;
}

void LoadMADRegisters( mad_registers *MadRegisters, HANDLE threadhdl )
{
    CONTEXT context;
    context.ContextFlags = CONTEXT_FULL | CONTEXT_FLOATING_POINT;
    GetThreadContext( threadhdl, &context );
    MadRegisters->x86.cpu.eax = context.Eax;
    MadRegisters->x86.cpu.ebx = context.Ebx;
    MadRegisters->x86.cpu.ecx = context.Ecx;
    MadRegisters->x86.cpu.edx = context.Edx;
    MadRegisters->x86.cpu.esi = context.Esi;
    MadRegisters->x86.cpu.edi = context.Edi;
    MadRegisters->x86.cpu.ebp = context.Ebp;
    MadRegisters->x86.cpu.esp = context.Esp;
    MadRegisters->x86.cpu.eip = context.Eip;
    MadRegisters->x86.cpu.efl = context.EFlags;
    MadRegisters->x86.cpu.cr0 = 0;
    MadRegisters->x86.cpu.cr2 = 0;
    MadRegisters->x86.cpu.cr3 = 0;
    MadRegisters->x86.cpu.ds = context.SegDs;
    MadRegisters->x86.cpu.es = context.SegEs;
    MadRegisters->x86.cpu.ss = context.SegSs;
    MadRegisters->x86.cpu.cs = context.SegCs;
    MadRegisters->x86.cpu.fs = context.SegFs;
    memcpy( &( MadRegisters->x86.u.fpu ), &( context.FloatSave ), sizeof( MadRegisters->x86.u.fpu ) );
    MADRegistersHost( MadRegisters );
}

void StoreMADRegisters( mad_registers *MadRegisters, HANDLE threadhdl )
{
    CONTEXT context;

    MADRegistersTarget(MadRegisters);
    context.ContextFlags=CONTEXT_FULL|CONTEXT_FLOATING_POINT;
    context.Eax = MadRegisters->x86.cpu.eax;
    context.Ebx = MadRegisters->x86.cpu.ebx;
    context.Ecx = MadRegisters->x86.cpu.ecx;
    context.Edx = MadRegisters->x86.cpu.edx;
    context.Esi = MadRegisters->x86.cpu.esi;
    context.Edi = MadRegisters->x86.cpu.edi;
    context.Ebp = MadRegisters->x86.cpu.ebp;
    context.Esp = MadRegisters->x86.cpu.esp;
    context.Eip = MadRegisters->x86.cpu.eip;
    context.EFlags = MadRegisters->x86.cpu.efl;
    context.SegDs = MadRegisters->x86.cpu.ds;
    context.SegEs = MadRegisters->x86.cpu.es;
    context.SegSs = MadRegisters->x86.cpu.ss;
    context.SegCs = MadRegisters->x86.cpu.cs;
    context.SegFs = MadRegisters->x86.cpu.fs;
    memcpy(&(context.FloatSave),&(MadRegisters->x86.u.fpu),min(sizeof(context.FloatSave),sizeof(MadRegisters->x86.u.fpu)));
    SetThreadContext( threadhdl, &context );
}

#elif defined __AXP__
void GetSysConfig( void )
{
    SysConfig.cpu = 0;
    SysConfig.fpu = 0;
    SysConfig.mad = MAD_AXP;
    SysConfig.osminor = _osminor;
    SysConfig.osmajor = _osmajor;
    SysConfig.huge_shift = 0;

}

void LoadMADRegisters(mad_registers *MadRegisters, HANDLE threadhdl )
{
    CONTEXT context;

    context.ContextFlags = CONTEXT_FULL;
    GetThreadContext( threadhdl, &context );
    memcpy( &( MadRegisters->axp.r[AR_f0] ), &( context.FltF0 ), sizeof( axpreg ) * 32 );
    memcpy( &( MadRegisters->axp.r[AR_v0] ), &( context.IntV0 ), sizeof( axpreg ) * 32 );
    memcpy( &( MadRegisters->axp.r[AR_fpcr] ), &( context.Fpcr ), sizeof( axpreg ) );
    memcpy( &( MadRegisters->axp.pal.nt.fir ), &( context.Fir ), sizeof( unsigned_64 ) );
    memcpy( &( MadRegisters->axp.pal.nt.softfpcr ), &( context.SoftFpcr ), sizeof( unsigned_64 ) );
    memcpy( &( MadRegisters->axp.pal.nt.psr ), &( context.Psr ), sizeof( unsigned_32 ) );
    MadRegisters->axp.active_pal = PAL_nt;
    MADRegistersHost( MadRegisters );

}

void StoreMADRegisters( mad_registers *MadRegisters, HANDLE threadhdl )
{
    CONTEXT context;

    MADRegistersTarget( MadRegisters );
    context.ContextFlags = CONTEXT_FULL;
    memcpy( &( context.FltF0 ), &( MadRegisters->axp.r[AR_f0] ), sizeof( axpreg ) * 32 );
    memcpy( &( context.IntV0 ), &( MadRegisters->axp.r[AR_v0] ), sizeof( axpreg ) * 32 );
    memcpy( &( context.Fpcr ), &( MadRegisters->axp.r[AR_fpcr] ), sizeof( axpreg ) );
    memcpy( &( context.Fir ), &( MadRegisters->axp.pal.nt.fir ), sizeof( unsigned_64 ) );
    memcpy( &( context.SoftFpcr ), &( MadRegisters->axp.pal.nt.softfpcr ), sizeof( unsigned_64 ) );
    memcpy( &( MadRegisters->axp.pal.nt.psr ), &( context.Psr ), sizeof( unsigned_32 ) );
    SetThreadContext( threadhdl, &context );
}
#else
#error hardware support not written
#endif
