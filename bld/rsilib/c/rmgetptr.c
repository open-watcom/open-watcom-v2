/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2011-2022 The Open Watcom Contributors. All Rights Reserved.
*
* =========================================================================
*
* Description:  rsi get real-mode far pointer.
*
****************************************************************************/


#include "rsi1632.h"

void FarPtr rsi_get_rm_ptr( void FarPtr p )
{
    descriptor  g;
    unsigned    seg;
    unsigned    off;

    if( rsi_get_descriptor( _FP_SEG( p ), &g ) ) {
        seg = GET_DESC_BASE( g ) >> 4;
        off = _FP_OFF( p ) + (g.base_15_0 & 0x0F);
        if( off < _FP_OFF( p ) ) {
            seg++;
            off -= 0x10;
        }
        return( makeptr( seg, off ) );
    }
    return( NULL_PTR );
}
