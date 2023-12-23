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
* Description:  rsi read 32-bit memory.
*
****************************************************************************/


#include "rsi1632.h"


bool D32DebugRead( addr48_ptr FarPtr addr, bool translate, void FarPtr to, size_t len )
/*
 * Return value: true if error.
 */
{
    addr48_ptr  fp;
    size_t      new_len;
    int         check;

    if( len == 0 )
        return( false );

    fp.segment = addr->segment;
    fp.offset = addr->offset;

    if( translate )
        D32Relocate( &fp );
    /*
     * If the range is at least partially invalid, we fill the buffer with
     * 0xFF (which will show through after any real data has been read).
     *
     * If the range is partially valid, we clip the address range to fit
     * and read the memory.
     */
    check = rsi_addr32_check( fp.offset, fp.segment, len, &new_len );
    if( check == MEMBLK_INVALID ) {
        far_setmem( to, len, 0xFF );
        return( true );
    } else {
        if( check != MEMBLK_VALID ) {
            far_setmem( (unsigned char FarPtr)to + new_len, len - new_len, 0xFF );
        }
        page_fault = false;
        peek32( fp.offset, fp.segment, new_len, to );
        /*
         * If a page fault occurred while reading the range, recurse until
         * we either read without getting a page fault, or reach a 1-byte
         * region that causes a page fault.  In the latter case, we can
         * just return the byte as 0xFF.
         */
        if( page_fault ) {
            page_fault = false;

            if( new_len == 1 ) {
                *(unsigned char FarPtr)to = 0xFF;
            } else {
                len = ( new_len >> 1 );
                D32DebugRead( &fp, false, to, len );
                fp.offset += len;
                D32DebugRead( &fp, false, (unsigned char FarPtr)to + len, new_len - len );
            }
        }
        return( false );
    }
}
