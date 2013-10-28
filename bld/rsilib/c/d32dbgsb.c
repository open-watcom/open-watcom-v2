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
* Description:  rsi set 32-bit break-point.
*
****************************************************************************/


#include "rsi1632.h"

void D32DebugSetBreak( OFFSET32 off, SELECTOR sel, int translate, unsigned char FarPtr to, unsigned char FarPtr from )
{
    Fptr32  fp;
    char    temp[4];

    if( translate ) {
        fp.sel = sel;
        fp.off = off;
        D32Relocate( &fp );
        sel = fp.sel;
        off = fp.off;
    }
    peek32( off, sel, temp, 1 );

    /* Don't set a breakpoint if there's already one there, or we lose
            the previously saved byte.
    */
    if( *temp != *to ) {
        *from = *temp;
        poke32( off, sel, to, 1 );
    }
}
