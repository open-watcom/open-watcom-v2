/****************************************************************************
*
*                            Open Watcom Project
*
*    Copyright (c) 2011 Open Watcom Contributors. All Rights Reserved.
*
* =========================================================================
*
* Description:  rsi create data selector (writeable) alias for code segment.
*
****************************************************************************/


#include "rsi1632.h"

SELECTOR rsi_sel_data_alias( SELECTOR csel )
{
    if( USESDPMI() ) {
        long    rc;

        if( (rc = DPMICreateCodeSegmentAliasDescriptor( csel )) < 0 )
            return( NULL_SEL );
        return( rc );
    } else {
        union _REGS     r;

        r.w.ax = 0xFF00;
        r.w.bx = csel;
        r.w.dx = 0x0500;
        intdos( &r, &r );
        if( r.w.cflag )
            return( NULL_SEL );
        return( r.w.ax );
    }
}
