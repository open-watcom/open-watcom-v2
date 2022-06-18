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

void D32DebugSetBreak( OFFSET32 off, SELECTOR sel, int translate, opcode_type FarPtr to, opcode_type FarPtr from )
{
    Fptr32      fp;
    opcode_type temp;

    if( translate ) {
        fp.sel = sel;
        fp.off = off;
        D32Relocate( &fp );
        sel = fp.sel;
        off = fp.off;
    }
    peek32( off, sel, &temp, sizeof( temp ) );

    /* Don't set a breakpoint if there's already one there, or we lose
            the previously saved byte.
    */
    if( temp != *to ) {
        *from = temp;
        poke32( off, sel, to, sizeof( *to ) );
    }
}
