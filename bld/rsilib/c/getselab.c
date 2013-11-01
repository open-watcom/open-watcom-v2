/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2011-2013 The Open Watcom Contributors. All Rights Reserved.
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
        SELECTOR    sel;
        descriptor  g;
        long        rc;

        if( (rc = DPMIAllocateLDTDescriptors( 1 )) < 0 )
            return( NULL_SEL );
        sel = rc;
        DPMIGetDescriptor( sel, &g );
        --size;
        g.lim_0_15 = size;
        g.lim_16_19 = size / 256 / 256;
        g.base_0_15 = base_addr;
        g.base_16_23 = base_addr >> 16;
        g.base_24_31 = base_addr >> 24;
        if( DPMISetDescriptor( sel, &g ) != 0 ) {
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
