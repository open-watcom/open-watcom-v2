/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  MIPS MAD address processing.
*
****************************************************************************/


#include "mips.h"


void MADIMPENTRY( AddrAdd )( address *a, long b, mad_address_format af )
{
    /* unused parameters */ (void)af;

    a->mach.offset += b;
}

int MADIMPENTRY( AddrComp )( address const *ap, address const *bp, mad_address_format af )
{
    /* unused parameters */ (void)af;

    if( ap->mach.offset == bp->mach.offset )
        return(  0 );
    if( ap->mach.offset >  bp->mach.offset )
        return( +1 );
    return( -1 );
}

long MADIMPENTRY( AddrDiff )( address const *a, address const *b, mad_address_format af )
{
    long        diff;

    /* unused parameters */ (void)af;

    diff = a->mach.offset - b->mach.offset;
    return( diff );
}

mad_status MADIMPENTRY( AddrMap )( addr_ptr *a, addr_ptr const *map, addr_ptr const *real, mad_registers const *mr )
{
    /* unused parameters */ (void)a; (void)map; (void)real; (void)mr;

    return( MS_FAIL );
}

mad_status MADIMPENTRY( AddrFlat )( mad_registers const *mr )
{
    /* unused parameters */ (void)mr;

    return( MS_FAIL );
}

mad_status MADIMPENTRY( AddrInterrupt )( addr_ptr const *a, unsigned size, mad_registers const *mr )
{
    /* unused parameters */ (void)a; (void)size; (void)mr;

    return( MS_FAIL );
}
