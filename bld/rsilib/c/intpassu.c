/****************************************************************************
*
*                            Open Watcom Project
*
*    Copyright (c) 2011 Open Watcom Contributors. All Rights Reserved.
*
* =========================================================================
*
* Description:  rsi set interrupt pass up.
*
****************************************************************************/


#include "rsi1632.h"

int rsi_int_passup( int int_no )
{
    unsigned char   FarPtr pui;
    int             i;

    pui = _d16info.PassupIntP.pv;
    for( i = 0; i < MAX_PASSUP_INDEX; ++i, pui += PASSUP_INDEX_INC ) {
        if( pui[3] == 0 || pui[3] == int_no ) {
            pui[3] = int_no;
            if( rsi_rm_set_vector( int_no, MK_FP( FP_SEG( _d16info.D16ToPMP ), FP_OFF( pui ) ) ) ) {
                return( 1 );
            }
            pui[3] = 0;
            break;
        }
    }
    return( 0 );
}
