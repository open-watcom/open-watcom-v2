/****************************************************************************
*
*                            Open Watcom Project
*
*    Copyright (c) 2011 Open Watcom Contributors. All Rights Reserved.
*
* =========================================================================
*
* Description:  rsi set real-mode interrupt.
*
****************************************************************************/


#include "rsi1632.h"

int rsi_rm_set_vector( int int_no, void FarPtr v )
{
    int             old_intf;
    void FarPtr     FarPtr vect;

    if( USESDPMI() ) {
        return( DPMISetRealModeInterruptVector( int_no, v ) == 0 );
    } else {
        vect = makeptr( mem0_sel, 0 );
        old_intf = reset_intflag();
        vect[int_no] = v;
        set_intflag( old_intf );
        return( 1 );
    }
}

