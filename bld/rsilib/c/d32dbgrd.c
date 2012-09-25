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
* Description:  rsi read 32-bit memory.
*
****************************************************************************/


#include "rsi1632.h"

/* Return value: not used.
*/
int D32DebugRead( OFFSET32 off, SELECTOR sel, int translate, char far *to, unsigned n )
{
    Fptr32      fp;
    OFFSET32    new_n;
    int         check;

    if( n == 0 )
        return( 0 );

    fp.sel = sel;
    fp.off = off;

    if( translate )
        D32Relocate( &fp );

    /* If the range is at least partially invalid, we fill the buffer with
        0xFF (which will show through after any real data has been read).

        If the range is partially valid, we clip the address range to fit
        and read the memory.
    */
    check = rsi_addr32_check( fp.off, fp.sel, (OFFSET32) n, &new_n );

    if( check <= 1 )
        far_setmem( to, n, 0xFF );
    if( check >= 1 ) {
        page_fault = 0;
        n = (unsigned short)new_n;
        peek32( fp.off, fp.sel, to, n );

        /* If a page fault occurred while reading the range, recurse until
            we either read without getting a page fault, or reach a 1-byte
            region that causes a page fault.  In the latter case, we can
            just return the byte as 0xFF.
        */
        if( page_fault ) {
            page_fault = 0;

            if( n == 1 ) {
                *to = 0xFF;
            } else {
                check = ( n >> 1 );
                D32DebugRead( off, sel, 0, to, check );
                D32DebugRead( off + check, sel, 0, to + check, n - check );
            }
        }
    }
    return( 0 );
}
