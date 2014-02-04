/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2011-2013 The Open Watcom Contributors. All Rights Reserved.
* Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
* Copyright (c) 1987-1992 Rational Systems, Incorporated. All Rights Reserved.
*
* =========================================================================
*
* Description:  rsi check 32-bit address range.
*
****************************************************************************/


#include "rsi1632.h"

/* Returns 
    MEMBLK_INVALID if range is completely invalid, 
    MEMBLK_PARTIAL if partially valid,
    MEMBLK_VALID if completely valid.  Assumes, of course, that valid segments
    are contiguous and begin at zero.

    If part of the range is valid, *valid_length is set to the length
    of the valid part.
*/
int rsi_addr32_check( OFFSET32 off, SELECTOR sel, OFFSET32 for_length, OFFSET32 *valid_length )
{
    descriptor  g;
    OFFSET32    limit;

    if( addr_mode == 0 && for_length != 0 ) {
        /* real or absolute address */
        if( !is_validselector( sel ) )
            return( MEMBLK_INVALID );    /* Null or beyond end of GDT */
        if( !rsi_get_descriptor( sel, &g ) )
            return( MEMBLK_INVALID );
        limit = GDT32LIMIT( g );
        if( off > limit )
            return( MEMBLK_INVALID );    /* Offset past end of segment */
        if( (sel & 3) > g.type.dpl || !g.type.present || !g.type.mustbe_1 ) {
            return( MEMBLK_INVALID );    /* Bad access bits */
        }
        --for_length;       /* to get address of the last byte looked at */
        if( off + for_length < off ) {
            if( valid_length ) {
                *valid_length = limit - off + 1;
            }
            return( MEMBLK_PARTIAL );    /* wrapped */
        }
        if( off + for_length > limit ) {
            if( valid_length ) {
                *valid_length = limit - off + 1;
            }
            return( MEMBLK_PARTIAL );    /* beyond end */
        }
        ++for_length;
    }
    if( valid_length != NULL )
        *valid_length = for_length;
    return( MEMBLK_VALID );
}
