/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2011-2013 The Open Watcom Contributors. All Rights Reserved.
*
* =========================================================================
*
* Description:  rsi call real-mode far function.
*
****************************************************************************/


#include "rsi1632.h"

int rsi_rm_far_call( void FarPtr far_proc, D16REGS FarPtr inr, D16REGS FarPtr outr )
{
    void FarPtr vect;
    int         retval;

    rsi_rm_get_vector( RMCALL_INT, &vect );
    rsi_rm_set_vector( RMCALL_INT, far_proc );
    retval = _d16info.D16rmInP( RMCALL_INT, inr, outr );
    rsi_rm_set_vector( RMCALL_INT, vect );
    return( retval );
}
