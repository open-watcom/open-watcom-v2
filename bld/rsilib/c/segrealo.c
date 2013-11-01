/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2011-2013 The Open Watcom Contributors. All Rights Reserved.
*
* =========================================================================
*
* Description:  rsi re-allocate segment.
*
****************************************************************************/


#include "rsi1632.h"

SELECTOR rsi_seg_realloc( SELECTOR sel )
{
    union _REGS     r;
    struct _SREGS   s;

    r.w.ax = 0xFFFF;
    r.w.dx = 0x0300;
    s.ds = NULL_SEL;
    s.es = sel;
    intdosx( &r, &r, &s );
    if( r.w.cflag )
        return( NULL_SEL );
    return( r.w.ax );
}
