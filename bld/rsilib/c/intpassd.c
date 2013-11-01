/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2011-2013 The Open Watcom Contributors. All Rights Reserved.
*
* =========================================================================
*
* Description:  rsi set interrupt pass down.
*
****************************************************************************/


#include "rsi1632.h"

int rsi_int_passdown( int int_no )
{
    if( USESDPMI() ) {
        union _REGS     r;

        r.w.ax = 0xFFFF;
        r.w.dx = 0x1600;
        intdos( &r, &r );
        return( DPMISetPMInterruptVector( int_no, makeptr( r.w.ax, int_no ) ) == 0 );
    } else {
        int     old_intf;
        IDT     FarPtr idt;

        idt = makeptr( idt_sel, 0 );
        old_intf = reset_intflag();
        idt[int_no].idtsel = kernelx_sel;
        idt[int_no].idtoffset = _d16info.passdn0 + ( ( _d16info.passdn255 - _d16info.passdn0 ) / 255 ) * int_no;
        set_intflag( old_intf );
        return( 1 );
    }
}
