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
* Description:  Real mode related routines used by protected mode servers.
*
****************************************************************************/


#if defined( DOS4G )

#include <stdio.h>
#include <dos.h>
#include "dos16new.h"

#ifdef DEBUG_TRAP
#define _DBG( x ) printf x; fflush( stdout )
#else
#define _DBG( x )
#endif

typedef struct {
    short   real;
    short   prot;
} map;

#define NONE            -1
#define MAX_MAPPINGS    sizeof(Mappings_pool)/sizeof(Mappings_pool[0])

static map  Mappings_pool[] = {
    {NONE,NONE},
    {NONE,NONE},
    {NONE,NONE},
    {NONE,NONE}
};

static int  Loser = 0;
static long dummy = 0;

void far * RMLinToPM( unsigned long linear_addr, int pool )
/***********************************************************/
{
    char        far *pm;
    int         i;
    short       real;
    short       offset;

    real = ( linear_addr >> 4 ) & 0xF000;
    offset = linear_addr;
    if( pool ) {
        for( i = 0; i < MAX_MAPPINGS; ++i ) {
            if( Mappings_pool[ i ].real == real ) {
                return( MK_FP( Mappings_pool[ i ].prot, offset ) );
            }
        }
    }
    pm = D16ProtectedPtr( MK_FP( real, 0 ), 0 );
    if( pm == 0 ) {
        _DBG(( "Can't get PM pointer for %ld\n", linear_addr ));
        pm = (char far *)&dummy;
    }
    if( pool ) {
        for( i = 0; i < MAX_MAPPINGS; ++i ) {
            if( Mappings_pool[ i ].real == NONE ) {
                Mappings_pool[ i ].real = real;
                Mappings_pool[ i ].prot = FP_SEG( pm );
                return( pm+offset );
            }
        }
        D16SegCancel( MK_FP( Mappings_pool[ Loser ].prot, 0 ) );
        Mappings_pool[ Loser ].real = real;
        Mappings_pool[ Loser ].prot = FP_SEG( pm );
        ++Loser;
        if( Loser == MAX_MAPPINGS ) {
            Loser = 0;
        }
    }
    return( pm+offset );
}

void CallRealMode( unsigned long dos_addr )
{
    D16REGS     regs;

    regs.ds = regs.es = FP_SEG( dos_addr );/* the trap file runs tiny -zu */
    _DBG(( "Calling RealMode\n"));
    D16rmRCall( regs.ds, FP_OFF( dos_addr ), &regs, &regs );
    _DBG(( "Back from RealMode\n"));
}

#elif defined( CAUSEWAY )

#include "dpmi.h"

extern int _CallRealMode( rm_call_struct far *regs );
#pragma aux _CallRealMode = \
        "mov    ax,0ff02h" \
        "int    0x31" \
        "sbb    eax,eax" \
        parm [ es edi ] value [ eax ];

void CallRealMode( unsigned long dos_addr )
{
    rm_call_struct  regs;

    /* the trap file runs tiny -zu */
    regs.ds = regs.es = regs.cs = dos_addr >> 16;
    regs.ip = dos_addr & 0xFFFF;
    _CallRealMode( &regs );
}


#else

extern int _CallRealMode( unsigned long dos_addr );
#pragma aux _CallRealMode = \
        "xor    ecx,ecx" \
        "mov    ax,0250eh" \
        "int    0x21" \
        "sbb    eax,eax" \
        parm [ ebx ] modify [ecx] value [ eax ];

void CallRealMode( unsigned long dos_addr )
{
    _CallRealMode( dos_addr );
}

#endif

