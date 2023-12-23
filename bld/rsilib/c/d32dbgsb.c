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
* Description:  rsi set 32-bit break-point.
*
****************************************************************************/


#include "rsi1632.h"


void D32DebugSetBreak( addr48_ptr FarPtr addr, bool translate, opcode_type FarPtr to, opcode_type FarPtr from )
{
    addr48_ptr  fp;
    opcode_type temp;

    fp.segment = addr->segment;
    fp.offset = addr->offset;
    if( translate ) {
        D32Relocate( &fp );
    }
    peek32( fp.offset, fp.segment, sizeof( temp ), &temp );
    /*
     * Don't set a breakpoint if there's already one there, or we lose
     *     the previously saved byte.
     */
    if( temp != *to ) {
        *from = temp;
        poke32( fp.offset, fp.segment, sizeof( *to ), to );
    }
}
