/****************************************************************************
*
*                            Open Watcom Project
*
*    Copyright (c) 2011 Open Watcom Contributors. All Rights Reserved.
*
* =========================================================================
*
* Description:  rsi descriptor manipulation functions.
*
****************************************************************************/


#include "rsi1632.h"

int rsi_get_descriptor( SELECTOR sel, descriptor FarPtr g )
{
    descriptor FarPtr  p;

    if( USESDPMI() ) {
        return( DPMIGetDescriptor( sel, g ) == 0 );
    } else {
        p = makeptr( gdt_sel, sel & SelMask );
        *g = *p;
        return( p->type.accessed != 0 );
    }
}

int rsi_set_descriptor( SELECTOR sel, descriptor FarPtr g )
{
    descriptor FarPtr  p;

    if( USESDPMI() ) {
        return( DPMISetDescriptor( sel, g ) == 0 );
    } else {
        p = makeptr( gdt_sel, sel & SelMask );
        *p = *g;
        return( sel != NULL_SEL );
    }
}

