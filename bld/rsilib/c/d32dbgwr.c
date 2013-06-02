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
* Description:  rsi write 32-bit memory.
*
****************************************************************************/


#include "rsi1632.h"

int D32DebugWrite( OFFSET32 off, SELECTOR sel, int translate, void far *from, unsigned n )
{
    Fptr32      fp;
    OFFSET32    new_n;
    int         check;

    fp.sel = sel;
    fp.off = off;
    if( translate )
        D32Relocate( &fp );

    check = rsi_addr32_check( fp.off, fp.sel, (OFFSET32)n, &new_n );

    if( check >= 1 )
        return( poke32( fp.off, fp.sel, from, (unsigned)new_n ) );
    return( 0 );
}
