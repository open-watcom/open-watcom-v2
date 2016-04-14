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
#include "data.h"
#include "savings.h"

extern  savings         Save;

extern  hw_reg_set      MustSaveRegs();
extern  void            SetCost(save_def*,save_def);
extern  void            SetLoopCost(uint);
extern  void            AdjTimeSize(uint*,uint*);
extern  conflict_node   *NameConflict(instruction*,name*);
extern  save_def        Weight(save_def,block*);

static  save_def        MaxConstSave;

#define COST( s, t ) ( ( (s)*size + (t)*time ) / TOTAL_WEIGHT )
#define _NEXTCON( c ) (*(conflict_node**)&((c)->tree))

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
/*   Loads       - cost move memory to register instruction*/
/*   Stores      - cost move register to memory instruction*/
/*   Uses        - replace memory reference with register reference*/
/*   Definitions - save move register to memory*/
/*   Prologs    - push a register*/
/*   Epilogs    - pop a register*/


    SetCost( Save.load_cost, COST( 4,1 ) );
    SetCost( Save.store_cost, COST( 4,1 ) );
    SetCost( Save.use_save, COST( 4,1 ) );
    SetCost( Save.def_save, COST( 4,1 ) );
    SetCost( Save.push_cost, COST( 4,1 ) );
    SetCost( Save.pop_cost, COST( 4,1 ) );
    Save.index_save     = Save.load_cost[ WD ];
}


extern  bool    WorthProlog( conflict_node *conf, hw_reg_set reg ) {
/*******************************************************************
    decide if the savings associated with giving conflict "conf"
    is worth the cost incurred by generating a prolog to
    save and restore register "reg"
*/

#if 0
    save_def            cost;
    save_def            savings;
    hw_reg_set          must_save;
    type_class_def      class;
    name                *op;

    class = conf->name->n.name_class;
    must_save = MustSaveRegs();
    if( BlockByBlock || HW_Ovlap( reg, GivenRegisters ) ||
       !HW_Ovlap( reg, must_save ) ) {
        cost = 0;
    } else {
        cost = Save.pop_cost[ class ] + Save.push_cost[ class ];
    }
    op = conf->name;
    savings = conf->savings;
    if( _ConstTemp( op ) ) {
        /* adjust for the initial load */
        cost += Weight( Save.load_cost[ class ] + Save.def_save[ class ],
                        conf->start_block );
        /* Adjust by a fudge factor */
        savings /= LOOP_FACTOR;
    } else {
        savings -= MaxConstSave;
    }
    return( savings >= cost );
#else
    reg = reg;
    conf = conf;
    return( true );
#endif
}


extern  void            ConstSavings() {
/**************************************

    Ensure constants are cached last by making sure that all
    "real" conflict nodes have higher savings than all conflict
    nodes for temps holding a constant value.  Make sure as well,
    that all "outer" loop constant temporary conflicts inherit the
    savings of any inner conflict that they define.
*/

    conflict_node       *conf;
    conflict_node       *other;
    block               *blk;
    instruction         *ins;

    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = ins->head.next ) {
            if( ins->head.opcode != OP_MOV ) continue;
            if( !_ConstTemp(ins->operands[0]) ) continue;
            if( !_ConstTemp(ins->result) ) continue;
            conf = NameConflict( ins, ins->operands[0] );
            if( conf == NULL ) continue;
            other = NameConflict( ins, ins->result );
            if( other == NULL ) continue;
            if( _Isnt( conf, SAVINGS_JUST_CALCULATED ) ) continue;
            if( _Isnt( other, SAVINGS_CALCULATED ) ) continue;
            _NEXTCON( conf ) = other;
        }
    }
    for( conf = ConfList; conf != NULL; conf = conf->next_conflict ) {
        if( _Is( conf, SAVINGS_JUST_CALCULATED ) && _ConstTemp( conf->name ) ) {
            for( other=_NEXTCON(conf); other != NULL; other=_NEXTCON(other) ) {
                conf->savings += other->savings;
                if( _Is( other, CONF_VISITED ) ) break;
            }
            _SetTrue( conf, CONF_VISITED ); /* already summed down the list */
        }
    }
    for( conf = ConfList; conf != NULL; conf = conf->next_conflict ) {
        _NEXTCON( conf ) = NULL;
        _SetFalse( conf, CONF_VISITED );
    }
    MaxConstSave = 0;
    for( conf = ConfList; conf != NULL; conf = conf->next_conflict ) {
        if( !_ConstTemp( conf->name ) ) continue;
        _SetFalse( conf, SAVINGS_JUST_CALCULATED );
        if( _Isnt( conf, SAVINGS_CALCULATED ) ) continue;
        if( conf->savings < MaxConstSave ) continue;
        MaxConstSave = conf->savings;
    }
    for( conf = ConfList; conf != NULL; conf = conf->next_conflict ) {
        if( _ConstTemp( conf->name ) ) continue;
        if( _Isnt( conf, SAVINGS_JUST_CALCULATED ) ) continue;
        _SetFalse( conf, SAVINGS_JUST_CALCULATED );
        if( conf->savings != 0 ) {
            conf->savings += MaxConstSave;
        }
    }
}
