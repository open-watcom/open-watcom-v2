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
#include <ctype.h>
#include <dos.h>
#include "wdebug.h"
#include "stdwin.h"
#include "winacc.h"
#include "mad.h"
#include "madregs.h"

/*
 * The structure used for CPU registers here - IntResult - is set and
 * read from in fault.c
 */

static void ReadCPU( struct x86_cpu *r )
{
    memset( r, 0, sizeof( *r ) );
    if( DebugeeTask != NULL ) {
        r->eax = IntResult.EAX;
        r->ebx = IntResult.EBX;
        r->ecx = IntResult.ECX;
        r->edx = IntResult.EDX;
        r->esi = IntResult.ESI;
        r->edi = IntResult.EDI;
        r->esp = IntResult.ESP;
        r->ebp = IntResult.EBP;
        r->eip = IntResult.EIP;
        r->efl = IntResult.EFlags;
        r->ds = IntResult.DS;
        r->cs = IntResult.CS;
        r->es = IntResult.ES;
        r->ss = IntResult.SS;
        r->fs = IntResult.FS;
        r->gs = IntResult.GS;
        if( !IsSegSize32( IntResult.SS ) ) {
            r->esp = (DWORD) (WORD) r->esp;
            r->ebp = (DWORD) (WORD) r->ebp;
        }
    }
}

static void ReadFPU( struct x86_fpu *r )
{
    memset( r, 0, sizeof( *r ) );
    if( DebugeeTask != NULL ) {
        memcpy( r, &FPResult, sizeof( *r ) );
    }
}

static void WriteCPU( struct x86_cpu *r )
{
    if( DebugeeTask != NULL ) {
        IntResult.EAX = r->eax;
        IntResult.EBX = r->ebx;
        IntResult.ECX = r->ecx;
        IntResult.EDX = r->edx;
        IntResult.ESI = r->esi;
        IntResult.EDI = r->edi;
        IntResult.ESP = r->esp;
        IntResult.EBP = r->ebp;
        IntResult.EIP = r->eip;
        IntResult.EFlags = r->efl;
        IntResult.DS = r->ds;
        IntResult.CS = r->cs;
        IntResult.ES = r->es;
        IntResult.SS = r->ss;
        IntResult.FS = r->fs;
        IntResult.GS = r->gs;
    }
}

static void WriteFPU( struct x86_fpu *r )
{
    if( DebugeeTask != NULL ) {
        memcpy( &FPResult, r, sizeof( *r ) );
    }
}

unsigned ReqRead_cpu()
{
    ReadCPU( GetOutPtr( 0 ) );
    return( sizeof( struct x86_cpu ) );
}

unsigned ReqRead_fpu()
{
    ReadFPU( GetOutPtr( 0 ) );
    return( sizeof( struct x86_fpu ) );
}

unsigned ReqRead_regs( void )
{
    mad_registers       *mr;

    mr = GetOutPtr( 0 );

    ReadCPU( &mr->x86.cpu );
    ReadFPU( &mr->x86.fpu );
    return( sizeof( mr->x86 ) );
}

unsigned ReqWrite_cpu()
{
    WriteCPU( GetInPtr( sizeof( write_cpu_req ) ) );
    return( 0 );
}

unsigned ReqWrite_fpu()
{
    WriteFPU( GetInPtr( sizeof( write_fpu_req ) ) );
    return( 0 );
}

unsigned ReqWrite_regs( void )
{
    mad_registers       *mr;

    mr = GetInPtr( sizeof( write_regs_req ) );
    WriteCPU( &mr->x86.cpu );
    WriteFPU( &mr->x86.fpu );
    return( 0 );
}
