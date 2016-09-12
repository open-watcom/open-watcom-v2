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


#include "axp.h"

void MADIMPENTRY( AddrAdd )( address *a, long b, mad_address_format af )
{
    af = af;

    a->mach.offset += b;
}

int MADIMPENTRY( AddrComp )( const address *ap, const address *bp, mad_address_format af )
{
    af = af;

    if( ap->mach.offset == bp->mach.offset ) return(  0 );
    if( ap->mach.offset >  bp->mach.offset ) return( +1 );
                                             return( -1 );
}

long MADIMPENTRY( AddrDiff )( const address *a, const address *b, mad_address_format af )
{
    long        diff;

    af = af;

    diff = a->mach.offset - b->mach.offset;
    return( diff );
}

mad_status MADIMPENTRY( AddrMap )( addr_ptr *a, const addr_ptr *map, const addr_ptr *real, const mad_registers *mr )
{
    a = a; map = map; real = real; mr = mr;

    return( MS_FAIL );
}

mad_status MADIMPENTRY( AddrFlat )( const mad_registers *mr )
{
    mr = mr;

    return( MS_FAIL );
}

mad_status MADIMPENTRY( AddrInterrupt )( const addr_ptr *a, unsigned size, const mad_registers *mr )
{
    a = a; size = size; mr = mr;

    return( MS_FAIL );
}
