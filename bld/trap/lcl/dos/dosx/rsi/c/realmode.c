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
#include <dos.h>
#include "dos16new.h"

#define _DBG( x ) // printf x; fflush( stdout )

typedef struct {
        short           real;
        short           prot;
} map;

#define NONE            -1
#define MAPPINGS        3

static map Mappings[ MAPPINGS ] = {{NONE,NONE},{NONE,NONE},{NONE,NONE}};
static int Loser = 0;

void far * ToPM( long linear_addr )
{
    char        far *pm;
    static long dummy;
    int         i;
    short       real;
    short       offset;

    real = ( linear_addr >> 4 ) & 0xF000;
    offset = linear_addr;
    for( i = 0; i < MAPPINGS; ++i ) {
        if( Mappings[ i ].real == real ) {
            return( MK_FP( Mappings[ i ].prot, offset ) );
        }
    }
    pm = D16ProtectedPtr( MK_FP( real, 0 ), 0 );
    if( pm == 0 ) {
        _DBG(( "Can't get PM pointer for %ld\n", linear_addr ));
        pm = (char far *)&dummy;
    }
    for( i = 0; i < MAPPINGS; ++i ) {
        if( Mappings[ i ].real == NONE ) {
            Mappings[ i ].real = real;
            Mappings[ i ].prot = FP_SEG( pm );
            return( pm+offset );
        }
    }
    D16SegCancel( MK_FP( Mappings[ Loser ].prot, 0 ) );
    Mappings[ Loser ].real = real;
    Mappings[ Loser ].prot = FP_SEG( pm );
    ++Loser;
    if( Loser == MAPPINGS ) Loser = 0;
    return( pm+offset );
}

long GetDosLong( long linear_addr )
{
    _DBG(( "GetDosLong %8.8lx\n", *(long far *)ToPM( linear_addr ) ));
    return( *(long far *)ToPM( linear_addr ) );
}

char GetDosByte( long linear_addr )
{
//  _DBG(( "GetDosByte %2.2x\n", *(char far *)ToPM( linear_addr ) ));
    return( *(char far *)ToPM( linear_addr ) );
}

void PutDosByte( long linear_addr, char c )
{
//  _DBG(( "PutDosByte\n"));
    *(char far *)ToPM( linear_addr ) = c;
}

void PutDosLong( long linear_addr, long l )
{
//  _DBG(( "PutDosLong\n"));
    *(long far *)ToPM( linear_addr ) = l;
}

void CallRealMode( long dos_addr )
{
    D16REGS     regs;

    regs.ds = regs.es = FP_SEG( dos_addr );/* the trap file runs tiny -zu */
    _DBG(( "Calling RealMode\n"));
    D16rmRCall( regs.ds, FP_OFF( dos_addr ), &regs, &regs );
    _DBG(( "Back from RealMode\n"));
}
