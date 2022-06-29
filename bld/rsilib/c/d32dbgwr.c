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
* Description:  rsi write 32-bit memory.
*
****************************************************************************/


#include "rsi1632.h"

bool D32DebugWrite( addr48_ptr FarPtr addr, bool translate, void FarPtr from, size_t len )
{
    addr48_ptr  fp;
    OFFSET32    new_len;

    fp.segment = addr->segment;
    fp.offset = addr->offset;
    if( translate )
        D32Relocate( &fp );

    if( rsi_addr32_check( fp.offset, fp.segment, len, &new_len ) == MEMBLK_INVALID )
        return( true );
    return( poke32( fp.offset, fp.segment, from, (size_t)new_len ) );
}
