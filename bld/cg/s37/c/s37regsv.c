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



#include "standard.h"
#include "coderep.h"
#include "conflict.h"
#include "model.h"
#include "savings.h"
#include "procdef.h"

#include "s37regsv.def"

extern  savings         Save;
extern  hw_reg_set      GivenRegisters;
extern  bool            BlockByBlock;

extern  void            SetCost(save_def*,save_def);
extern  void            SetLoopCost(uint);
extern  void            AdjTimeSize(uint*,uint*);
extern  hw_reg_set      MustSaveRegs();

#define COST( s, t ) ( ( (s)*size + (t)*time ) / TOTAL_WEIGHT )

extern  void    InitWeights( uint size ) {
/*****************************************
    Set up a structure describing the savings/costs involved
    with operations like loads, stores, saving a memory reference, etc.
    "size" is the importance of code size (vs. speed) expressed
    as a percentage between 0 and 100.
*/

    uint        time;


    AdjTimeSize( &time, &size );
    SetLoopCost( time );

/*   Indexing    - save load of index register*/

    Save.index_save     = WORD_SIZE * COST( 4,4 );

/*   Loads       - cost move memory to register instruction*/

    SetCost( Save.load_cost, COST( 4,4 ) );

/*   Stores      - cost move register to memory instruction*/

    SetCost( Save.store_cost, COST( 4,4 ) );

/*   Uses        - replace memory reference with register reference*/

    SetCost( Save.use_save, COST( 3,3 ) );

/*   Definitions - save move register to memory*/

    SetCost( Save.def_save, COST( 3,3 ) );

/*   Prologs    - push a register*/

    SetCost( Save.push_cost, COST( 0,2 ) );

/*   Epilogs    - pop a register*/

    SetCost( Save.pop_cost, COST( 0,2 ) );

    Save.push_cost[ FS ] = Save.store_cost[ FS ];
    Save.pop_cost[ FS ] = Save.store_cost[ FS ];
    Save.push_cost[ FD ] = Save.store_cost[ FD ];
    Save.pop_cost[ FD ] = Save.store_cost[ FD ];
}


extern  bool    WorthProlog( conflict_node *conf, hw_reg_set reg ) {
/*******************************************************************
    decide if the savings associated with giving conflict "conf"
    is worth the cost incurred by generating a prolog to
    save and restore register "reg"
*/

    save_def    cost;
    hw_reg_set  tmp;
    type_class_def      class;

    class = conf->name->n.name_class;

    if( _Is( conf, NEVER_TOO_GREEDY ) ) return( TRUE );
    tmp = MustSaveRegs();
    if( BlockByBlock || HW_Ovlap( reg, GivenRegisters )
    || !HW_Ovlap( reg, tmp ) ) {
        cost = 0;
    } else {
        cost = Save.push_cost[ class ] + Save.pop_cost[ class ];
    }
    return( cost <= conf->savings );
}

extern  void    ConstSavings() {
/******************************/
}
