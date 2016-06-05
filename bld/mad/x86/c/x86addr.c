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


#include "string.h"
#include "x86.h"

void            DIGENTRY MIAddrAdd( address *a, long b, mad_address_format af )
{
    unsigned    over;

    a->mach.offset += b;
    if( !BIG_SEG( *a ) ) {
        if( a->mach.offset > 0xffff ) {
            if( af != MAF_OFFSET ) {
                over = a->mach.offset >> (16 - MCSystemConfig()->huge_shift);
                a->mach.segment += (addr_seg)over;
            }
            a->mach.offset &= 0xffff;
        }
    }
}

int             DIGENTRY MIAddrComp( const address *ap, const address *bp, mad_address_format af )
{
    addr_ptr    a;
    addr_ptr    b;

    a = ap->mach;
    b = bp->mach;
    if( af == MAF_OFFSET ) {
        if( a.offset == b.offset ) return(  0 );
        if( a.offset >  b.offset ) return( +1 );
                                   return( -1 );
    }
    if( REAL_SEG( *ap ) ) {
        a.segment = (addr_seg)( a.segment + ( a.offset >> 4 ) );
        b.segment = (addr_seg)( b.segment + ( b.offset >> 4 ) );
        a.offset = a.offset & 0x0F;
        b.offset = b.offset & 0x0F;
    }
    if( a.segment == b.segment ) {
        if( a.offset == b.offset ) {
            return( 0 );
        } else if( a.offset > b.offset ) {
            return( 1 );
        } else {
            return( -1 );
        }
    } else if( a.segment > b.segment ) {
        return( 2 );
    } else {
        return( -2 );
    }
}

long            DIGENTRY MIAddrDiff( const address *a, const address *b, mad_address_format af )
{
    long        diff;
    addr_seg    seg;

    af = af;
    diff = a->mach.offset - b->mach.offset;
    if( !BIG_SEG( *a ) ) {
        seg = 0;
        if( diff < 0 ) 
            seg = b->mach.segment + (1 << MCSystemConfig()->huge_shift);
        diff &= 0xffff;
        seg = a->mach.segment - seg;
        diff += (unsigned long)(seg >> MCSystemConfig()->huge_shift) << 16;
    }
    return( diff );
}

mad_status      DIGENTRY MIAddrMap( addr_ptr *a, const addr_ptr *map, const addr_ptr *real, const mad_registers *mr )
{
    if( !REAL_SEG( GetRegIP( mr ) ) )
        return( MS_FAIL );
    /*
        we're in real mode and already have a known address. We can
        use the known address to calculate this address's mapping.
    */
    a->segment += real->segment - map->segment;
    return( MS_OK );
}

mad_status      DIGENTRY MIAddrFlat( const mad_registers *mr )
{
    if( REAL_SEG( GetRegIP( mr ) ) )
        return( MS_FAIL );
    return( MS_OK );
}

mad_status      DIGENTRY MIAddrInterrupt( const addr_ptr *a, unsigned size, const mad_registers *mr )
{
    unsigned_16         start, end;
    address             addr;

    mr = mr;
    memset( &addr, 0, sizeof( addr ) );
    addr.mach = *a;
    if( !REAL_SEG( addr ) )
        return( MS_FAIL );
    start = (unsigned_16)( ((unsigned long)a->segment << 4) + a->offset );
    end = (unsigned_16)( start + size );
    if( start < 0x400 || end < 0x400 )
        return( MS_OK );
    return( MS_FAIL );
}

unsigned AddrCharacteristics( address a )
{
    unsigned_8  character;

    MCMachineData( a, X86MD_ADDR_CHARACTERISTICS, 0, NULL, sizeof( character ), &character );
    return( character );
}
