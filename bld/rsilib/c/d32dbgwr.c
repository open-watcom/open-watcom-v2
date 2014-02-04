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
* Description:  rsi write 32-bit memory.
*
****************************************************************************/


#include "rsi1632.h"

int D32DebugWrite( OFFSET32 off, SELECTOR sel, int translate, void FarPtr from, unsigned short len )
{
    Fptr32      fp;
    OFFSET32    new_len;

    fp.sel = sel;
    fp.off = off;
    if( translate )
        D32Relocate( &fp );

    if( rsi_addr32_check( fp.off, fp.sel, len, &new_len ) == MEMBLK_INVALID )
        return( 1 );
    return( poke32( fp.off, fp.sel, from, (unsigned short)new_len ) );
}
