/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "conflict.h"
#include "pattern.h"
#include "model.h"
#include "savings.h"

extern  void            CalcLoadStore(conflict_node*);
extern  name            *DeAlias(name*);

savings         Save;


extern  void    AdjTimeSize( uint *time, uint *size ) {
/*****************************************************/

    /*   Adjust size to be between 0 and TOTAL_WEIGHT*/

    if( *size > 100 ) {
        *size = TOTAL_WEIGHT;
    } else {
        *size = *size * TOTAL_WEIGHT / 100;
    }
    *time = TOTAL_WEIGHT - *size;
}


extern  void    SetLoopCost( uint time ) {
/****************************************/

    save_def    loop_weight;
    int         i;

    loop_weight         = ( LOOP_FACTOR*time ) / TOTAL_WEIGHT;
    if( loop_weight == 0 ) {
        loop_weight = 1;
    }
    Save.loop_weight[ 0 ] = 1;
    i = 1;
    for(;;) {
        Save.loop_weight[ i ] = Save.loop_weight[ i-1 ]*loop_weight;
        ++ i;
        if( i > MAX_LOOP ) break;
    }
}


extern  void    SetCost( save_def *array, save_def cost ) {
/*********************************************************/

    save_def    cost2;

    cost2 = cost;
    #if _TARGET & _TARG_IAPX86
        cost2 *= 2;
    #endif
    array[ U1 ] = cost;
    array[ I1 ] = cost;
    array[ U2 ] = cost;
    array[ I2 ] = cost;
    array[ U4 ] = cost2;
    array[ I4 ] = cost2;
    array[ U8 ] = cost;         // FIXME - not sure about these...
    array[ I8 ] = cost;
    array[ CP ] = cost2;
    array[ PT ] = cost2;
    array[ FS ] = cost;
    array[ FD ] = cost;
    array[ FL ] = cost;
    array[ XX ] = cost;
}


extern  save_def        Weight( save_def value, block *blk ) {
/************************************************************/

    if( blk->depth < MAX_LOOP ) {
        value *= Save.loop_weight[  blk->depth  ];
    } else {
        value *= Save.loop_weight[  MAX_LOOP  ];
    }
    return( value );
}


extern  void    CalcSavings( conflict_node *conf ) {
/**************************************************/

/* NB: <regsave> relies on the fact that "conf" is a parm to this routine*/

    save_def            block_save;
    save_def            block_cost;
    save_def            cost;
    save_def            save;
#include "savcache.h"

    if( ( conf->name->v.usage & USE_IN_ANOTHER_BLOCK )
     && ( conf->name->v.usage & ( NEEDS_MEMORY | USE_ADDRESS ) ) ) {
        CalcLoadStore( conf );
    }
    if( _Is( conf, CONFLICT_ON_HOLD ) ) {
        conf->savings = 0;
    } else if( conf->available == 0
         && _Isnt( conf, ( NEEDS_INDEX | NEEDS_SEGMENT ) ) ) {
        conf->savings = 0;
    } else if( conf->ins_range.first == conf->ins_range.last ) {
        conf->savings = 0;             /* don't try - the world screws up*/
    } else {
        cost = 0;
        save = 0;
        block_cost = 0;
        block_save = 0;
        opnd = conf->name;

#undef   _InRegAssgn
#include "savcode.h"

        if( save <= cost ) {
            conf->savings = 0;
        } else if( save - cost < MAX_SAVE ) {
            conf->savings = save - cost;
        } else {
            conf->savings = MAX_SAVE - 1;
        }
    }
}
