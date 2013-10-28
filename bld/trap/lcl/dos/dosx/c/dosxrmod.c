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

//#define DEBUG_TRAP  1
#include "trapdbg.h"
#include <stdio.h>
#include "rsi1632.h"

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

void __far *RMLinToPM( unsigned long linear_addr, int pool )
/********************************************************/
{
    int         i;
    short       real;
    short       offset;
	SELECTOR    pm_sel;

    real = ( linear_addr >> 4 ) & 0xF000;
    offset = linear_addr;
    if( pool ) {
        for( i = 0; i < MAX_MAPPINGS; ++i ) {
            if( Mappings_pool[ i ].real == real ) {
                return( MK_FP( Mappings_pool[ i ].prot, offset ) );
            }
        }
    }
    pm_sel = rsi_sel_new_absolute( linear_addr & 0x000F0000, 0 );
    if( pm_sel == NULL_SEL ) {
        _DBG_Write( "Can't get PM pointer for " );
        _DBG_Write32( linear_addr );
        _DBG_NewLine();
        pm_sel = FP_SEG( &dummy );
    }
    if( pool ) {
        for( i = 0; i < MAX_MAPPINGS; ++i ) {
            if( Mappings_pool[ i ].real == NONE ) {
                Mappings_pool[ i ].real = real;
                Mappings_pool[ i ].prot = pm_sel;
                return( MK_FP( pm_sel, offset ) );
            }
        }
        rsi_sel_free( Mappings_pool[ Loser ].prot );
        Mappings_pool[ Loser ].real = real;
        Mappings_pool[ Loser ].prot = pm_sel;
        ++Loser;
        if( Loser == MAX_MAPPINGS ) {
            Loser = 0;
        }
    }
    return( MK_FP( pm_sel, offset ) );
}

void CallRealMode( unsigned long dos_addr )
{
    D16REGS     regs;

    regs.ds = regs.es = FP_SEG( dos_addr ); /* the trap file runs tiny -zu */
    _DBG_Writeln( "Calling RealMode" );
    rsi_rm_far_call( (void __far *)( dos_addr ), &regs, &regs );
    _DBG_Writeln( "Back from RealMode" );
}

#elif defined( CAUSEWAY )

#include "dpmi.h"

extern int _CallRealMode( rm_call_struct __far *regs );
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

