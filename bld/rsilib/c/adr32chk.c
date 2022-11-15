/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2011-2022 The Open Watcom Contributors. All Rights Reserved.
* Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
* Copyright (c) 1987-1992 Rational Systems, Incorporated. All Rights Reserved.
*
* =========================================================================
*
* Description:  rsi check 32-bit address range.
*
****************************************************************************/


#include "rsi1632.h"


int rsi_addr32_check( OFFSET32 off, SELECTOR sel, size_t for_length, size_t *valid_length )
/*
 * Returns:
 *   MEMBLK_INVALID     if range is completely invalid
 *   MEMBLK_PARTIAL     if partially valid
 *   MEMBLK_VALID       if completely valid
 * Assumes, of course, that valid segments are contiguous and begin at zero.
 *
 * If part of the range is valid, *valid_length is set to the length
 * of the valid part.
 */
{
    descriptor  g;
    OFFSET32    limit;
    int         rc;

    rc = MEMBLK_VALID;
    if( addr_mode == 0 && for_length > 0 ) {
        /*
         * real or absolute address
         */
        if( !is_validselector( sel ) ) {
            /*
             * Null or beyond end of GDT
             */
            return( MEMBLK_INVALID );
        }
        if( !rsi_get_descriptor( sel, &g ) )
            return( MEMBLK_INVALID );
        limit = GET_DESC_LIMIT( g );
        if( off > limit ) {
            /*
             * Offset past end of segment
             */
            return( MEMBLK_INVALID );
        }
        if( (sel & 3) > g.u1.flags.dpl || !g.u1.flags.present || !g.u1.flags.nonsystem ) {
            /*
             * Bad access bits
             */
            return( MEMBLK_INVALID );
        }
        if( off + for_length - 1 < off          /* wrapped */
          || off + for_length - 1 > limit ) {   /* beyond end */
            for_length = (size_t)( limit - off + 1 );
            rc = MEMBLK_PARTIAL;
        }
    }
    if( valid_length != NULL )
        *valid_length = for_length;
    return( rc );
}
