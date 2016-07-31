/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2016 The Open Watcom Contributors. All Rights Reserved.
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
#include "stack.h"
#include "makeins.h"
#include "foldins.h"
#include "convins.h"
#include "data.h"
#include "namelist.h"
#include "regalloc.h"
#include "rgtbl.h"
#include "split.h"
#include "insutil.h"
#include "optab.h"


static  block           *Head;
static  block           *(*Next)(block*);

static  int     CountOps( instruction *ins, name *cons ) {
/********************************************************/

    int num_operands;

    switch( ins->head.opcode ) {
    case OP_LSHIFT:
    case OP_RSHIFT:
        num_operands = 1;
        break;
    case OP_ADD:
    case OP_SUB:
        if( IsStackReg( ins->result ) ) {
            num_operands = 0;
        } else if( cons->c.lo.int_value == 1 ) {
            num_operands = 1;
        } else {
            num_operands = ins->num_operands;
        }
        break;
    case OP_CALL:
    case OP_CALL_INDIRECT:
        /* operands of a call are special, and never should use CONST_TEMP's */
        num_operands = 0;
        break;
    default:
        num_operands = ins->num_operands;
        break;
    }
    return( num_operands );
}


static  type_class_def  FindMaxClass( name *cons, int *prefs ) {
/******************************************************************

    Run through the instructions and find the maximimum class of
    all instructions in which "cons" is used.
*/

    block               *blk;
    instruction         *ins;
    int                 i;
    int                 class;
    int                 num_operands;

    class = -1;
    *prefs = 0;
    for( blk = Head; blk != NULL; blk = Next( blk ) ) {
        for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = ins->head.next ) {
            num_operands = CountOps( ins, cons );
            for( i = 0; i < num_operands; ++i ) {
                if( ins->operands[i] == cons ) {
                    ++*prefs;
                    if( (int)ins->type_class >= class ) {
                        class = _OpClass( ins );
                    }
                }
            }
        }
    }
    if( class == -1 )
        class = XX;
    // return( Unsigned[ class ] );
    // why? BBB - June 28, 1995
    return( class );
}


static  bool    ReplaceConst( name *cons, name *temp, type_class_def tmp_class )
/*******************************************************************************

     Replace all occurences of "cons" with "temp".  As a hack, we
     never replace the operands of a shift, since it's always
     better to do a shift by a constant on both the 370 and x86
     architectures.
*/
{
    block               *blk;
    instruction         *ins;
    int                 i;
    type_class_def      ins_class;
    bool                change;
    int                 num_operands;

    tmp_class = tmp_class;
    change = false;
    for( blk = Head; blk != NULL; blk = Next( blk ) ) {
        for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = ins->head.next ) {
            ins_class = Unsigned[ _OpClass( ins ) ];
            num_operands = CountOps( ins, cons );
            for( i = 0; i < num_operands; ++i ) {
                if( ins->operands[i] == cons  ) {
                    if( ins_class == temp->n.name_class ) {
                        ins->operands[i] = temp;
                        change = true;
                    } else {
#if ( _TARGET & _TARG_AXP ) == 0 && ( _TARG_MEMORY & _TARG_LOW_FIRST )
                        if( _IsIntegral( ins_class ) && _IsIntegral( tmp_class ) ) {
                            ins->operands[i] = TempOffset( temp, 0, ins_class );
                            change = true;
                        }
#endif
                    }
                }
            }
        }
    }
    return( change );
}


extern  void    ConstToTemp( block *pre, block *head, block*(*next)(block*) ) {
/******************************************************************************

    Turn constant referneces into temp references so that they can
    get register allocated.
*/

    name                *cons;
    name                *temp;
    type_class_def      class;
    int                 num_refs;

    Head = head;
    Next = next;
    for( cons = Names[ N_CONSTANT ]; cons != NULL; cons = cons->n.next_name ) {
        if( cons->c.const_type == CONS_TEMP_ADDR ) continue;
        class = FindMaxClass( cons, &num_refs );
        if( class == XX ) continue;
        if( head == HeadBlock && num_refs < 2 ) continue;
        temp = AllocTemp( class );
        temp->t.temp_flags |= CONST_TEMP;
        temp->v.symbol = cons;
        if( !ReplaceConst( cons, temp, class ) ) continue;
        temp->v.usage |= USE_IN_ANOTHER_BLOCK;
        SuffixIns( pre->ins.hd.prev, MakeMove( cons, temp, class ) );
    }
}


extern  void            MemConstTemp( conflict_node *conf ) {
/************************************************************

    We've decided to put a CONST_TEMP into memory. Go through the
    instruction list and change all occurences back into the original
    constant reference. Also, do the same for any other CONST_TEMP
    temporaries whose only purpose is to define this one.
*/

    block               *blk;
    instruction         *ins;
    int                 i;
    name                *other;
    name                *temp;
    instruction         *folded;

    temp = conf->name;
    other = NULL;
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = ins->head.next ) {
            for( i = 0; i < ins->num_operands; ++i ) {
                if( ins->operands[i]->n.class != N_TEMP ) continue;
                if( DeAlias( ins->operands[i] ) != temp ) continue;
                ins->head.state = INS_NEEDS_WORK;
                ins->operands[i] = ins->operands[i]->v.symbol;
                if( ins->head.opcode == OP_CONVERT ) {
                    ins = rDOCVT( ins );
                }
            }
            if( ins->result != NULL && ins->result->n.class == N_TEMP ) {
                if( DeAlias( ins->result ) == temp ) {
                    other = ins->operands[0];
                    ins->head.opcode = OP_NOP;
                    ins->num_operands = 0;
                    ins->result = NULL;
                    DoNothing( ins );
                }
            }
            folded = FoldIns( ins );
            if( folded != NULL ) {
                ins = folded;
            }
        }
    }
    if( other == NULL ) return;
    if( !_ConstTemp( other ) ) return;
    if( other->v.conflict == NULL ) return;
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = ins->head.next ) {
            for( i = 0; i < ins->num_operands; ++i ) {
                if( ins->operands[i]->n.class != N_TEMP ) continue;
                if( DeAlias( ins->operands[i] ) == other ) return;
            }
        }
    }
    SafeRecurseCG( (func_sr)InMemory, other->v.conflict );
}
