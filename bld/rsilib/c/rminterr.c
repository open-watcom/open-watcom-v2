/****************************************************************************
*
*                            Open Watcom Project
*
*    Copyright (c) 2011 Open Watcom Contributors. All Rights Reserved.
*
* =========================================================================
*
* Description:  rsi call real-mode interrupt.
*
****************************************************************************/


#include "rsi1632.h"

int rsi_rm_interrupt( int int_no, D16REGS FarPtr inr, D16REGS FarPtr outr )
{
    return( _d16info.D16rmInP( int_no, inr, outr ) );
}
