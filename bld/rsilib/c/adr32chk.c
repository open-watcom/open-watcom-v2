/****************************************************************************
*
*                            Open Watcom Project
*
*    Copyright (c) 2011 Open Watcom Contributors. All Rights Reserved.
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*    Copyright (c) 1987-91, 1992 Rational Systems, Incorporated. All Rights Reserved.
*
* =========================================================================
*
* Description:  rsi check 32-bit address range.
*
****************************************************************************/


#include "rsi1632.h"

/* Returns 0 if range is completely invalid, 1 if partially valid,
    2 if completely valid.  Assumes, of course, that valid segments
    are contiguous and begin at zero.

    If part of the range is valid, *valid_length is set to the length
    of the valid part.
*/
int rsi_addr32_check( OFFSET32 off, SELECTOR sel, OFFSET32 for_length, OFFSET32 *valid_length )
{
    descriptor  g;
    OFFSET32    length;

    if( addr_mode == 0 ) {
        /* real or absolute address */
        if( !is_validselector( sel ) )
            return( 0 );    /* Null or beyond end of GDT */
        if( !rsi_get_descriptor( sel, &g ) )
            return( 0 );
        length = GDT32LEN( g );
        if( off > length )
            return( 0 );    /* Offset past end of segment */
        if( (sel & 3) > g.type.dpl || !g.type.present || !g.type.mustbe_1 ) {
            return( 0 );    /* Bad access bits */
        }
        --for_length;       /* to get address of the last byte looked at */
        if( off + for_length < off ) {
            if( valid_length ) {
                *valid_length = length - off + 1;
            }
            return( 1 );    /* wrapped */
        }
        if( off + for_length > length ) {
            if( valid_length ) {
                *valid_length = length - off + 1;
            }
            return( 1 );    /* beyond end */
        }
        ++for_length;
    }
    if( valid_length )
        *valid_length = for_length;
    return( 2 );
}
