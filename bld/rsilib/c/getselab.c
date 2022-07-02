/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2011-2022 The Open Watcom Contributors. All Rights Reserved.
*
* =========================================================================
*
* Description:  rsi create selector to absolute address range.
*
****************************************************************************/


#include "rsi1632.h"

SELECTOR rsi_sel_new_absolute( long base_addr, unsigned size )
{
    if( USESDPMI() ) {
        descriptor  g;
        long        sel;

        sel = DPMIAllocateLDTDescriptors( 1 );
        if( sel < 0 )
            return( NULL_SEL );
        DPMIGetDescriptor( sel, &g );
        --size;
        g.limit_15_0 = size;
        g.limit_19_16 = size / 256 / 256;
        g.base_15_0 = base_addr;
        g.base_23_16 = base_addr >> 16;
        g.base_31_24 = base_addr >> 24;
        if( DPMISetDescriptor( sel, &g ) ) {
            DPMIFreeLDTDescriptor( sel );
            return( NULL_SEL );
        }
        return( sel );
    } else {
        union _REGS     r;
        struct _SREGS   s;

        r.h.ah = 0xFF;
        r.h.al = base_addr >> 16;
        r.w.bx = base_addr;
        r.w.cx = size;
        r.w.dx = 0x0A00;
        s.ds = NULL_SEL;
        s.es = NULL_SEL;
        intdosx( &r, &r, &s );
        return( s.es );
    }
}
