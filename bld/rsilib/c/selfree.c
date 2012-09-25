/****************************************************************************
*
*                            Open Watcom Project
*
*    Copyright (c) 2011 Open Watcom Contributors. All Rights Reserved.
*
* =========================================================================
*
* Description:  rsi free selector.
*
****************************************************************************/


#include "rsi1632.h"

int rsi_sel_free( SELECTOR sel )
{
    if( USESDPMI() ) {
        DPMIFreeLDTDescriptor( sel );
    } else {
        union _REGS     r;

        r.w.ax = 0xFF00;
        r.w.bx = sel;
        r.w.dx = 0x0700;
        intdos( &r, &r );
    }
    return( 1 );
}
