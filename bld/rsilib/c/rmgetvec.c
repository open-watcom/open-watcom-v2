/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2011-2022 The Open Watcom Contributors. All Rights Reserved.
*
* =========================================================================
*
* Description:  rsi get real-mode interrupt vector.
*
****************************************************************************/


#include "rsi1632.h"

bool rsi_rm_get_vector( int int_no, void FarPtr FarPtr vectp )
{
    void FarPtr     FarPtr vect;

    if( USESDPMI() ) {
        *vectp = DPMIGetRealModeInterruptVector( int_no );
    } else {
        vect = makeptr( mem0_sel, 0 );
        *vectp = vect[int_no];
    }
    return( true );
}
