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
#include "opcodes.h"
#include "sysmacro.h"
#include "model.h"

extern  instruction     *SIBPossibleIndex(instruction*,name*,name**,bool*,hw_reg_set,hw_reg_set,bool*,bool*);
extern  name            *AllocRegName(hw_reg_set);
extern  name            *AllocIntConst(int);
extern  name            *ScaleIndex(name*,name*,type_length,type_class_def,type_length,int,i_flags);
extern  hw_reg_set      Low64Reg(hw_reg_set);
extern  void            FreeIns(instruction*);
extern  void            ReplaceOperand(instruction*,name*,name*);
extern  byte            *Copy(byte*,byte*,uint);

extern  block           *HeadBlock;
extern  type_length     TypeClassSize[];

typedef struct sib_info {
    struct sib_info     *next;
    name                *reg;
    name                *index;
    int                 scale;
    i_flags             flags;
    instruction         *ins;
    signed_32           offset;
} sib_info;

extern bool FoldIntoIndex( instruction * ins ) {
/**********************************************/

    name        *cons;
    bool        is_base;
    name        *new_x;
    hw_reg_set  base_reg;
    hw_reg_set  other_reg;
    hw_reg_set  tmp;
    sib_info    sib;
    bool        dies;
    bool        modifies;
    instruction *next;
    sib_info    *sib_head;
    sib_info    *curr_sib;
    bool        cons_add;

    cons_add = FALSE;
    if( ins->head.opcode == OP_LSHIFT ) {
        HW_CAsgn( base_reg, HW_EMPTY );
        cons = ins->operands[ 1 ];
        if( cons->n.class != N_CONSTANT ) return( FALSE );
        if( cons->c.const_type != CONS_ABSOLUTE ) return( FALSE );
        if( cons->c.int_value > 3 ) return( FALSE );
/*
        found SHL R1,n => R1
*/
    } else if( ins->head.opcode == OP_ADD && ins->operands[0] == ins->result ) {
        // ADD Rx,?? => Rx
        cons = ins->operands[ 1 ];
        if( cons->n.class == N_CONSTANT && cons->c.const_type == CONS_ABSOLUTE ) {
/*
        found ADD R1,c => R1
*/
            if( TypeClassSize[ ins->type_class ] != WORD_SIZE ) return( FALSE );
            cons_add = TRUE;
            HW_CAsgn( base_reg, HW_EMPTY );
        } else {
            if( cons->n.class != N_REGISTER ) return( FALSE );
            if( cons->n.size != WORD_SIZE ) return( FALSE );
            base_reg = cons->r.reg;
/*
        found ADD R1,R2 => R1
*/
            if( cons == ins->operands[ 0 ] ) {
/*
        found ADD R1,R1 => R1  <==> SHL R1,1 => R1
*/
                cons = AllocIntConst( 1 );
                HW_CAsgn( base_reg, HW_EMPTY );
            }
        }
    } else {
        return( FALSE );
    }
    other_reg = ins->operands[ 0 ]->r.reg;
    sib_head = NULL;
    dies = FALSE;
    next = ins;
    do {
        curr_sib = NULL;
        next = SIBPossibleIndex( next, ins->result, &sib.index,
                                 &is_base, base_reg, other_reg,
                                 &dies, &modifies );
        if( next == NULL ) break;
        sib.ins = next;
        sib.offset = 0;
        if( !HW_Equal( base_reg, HW_EMPTY ) && _IsTargetModel( INDEXED_GLOBALS )
         && sib.index->i.base != NULL
         && sib.index->i.base->n.class == N_MEMORY ) break;
    /*
        Hey, we found a good one as long as reg dies immediately after it and we
        don't need to save the base register slot for INDEXED GLOBALS
    */
        if( HW_Equal( base_reg, HW_EMPTY ) ) {
            sib.flags = sib.index->i.index_flags;
            if( cons_add ) {
                int value;
                sib.scale = sib.index->i.scale;
                sib.reg = sib.index->i.index;
                value = cons->c.int_value;
                if( ! is_base ) {
                    value <<= sib.scale;
                }
                sib.offset = value;
            } else {
                if( is_base ) {
                    if( sib.index->i.scale != 0 ) break;
                    sib.flags ^= ( X_HIGH_BASE | X_LOW_BASE ); /* flip base and index */
                }
                sib.scale = cons->c.int_value + sib.index->i.scale;
                if( sib.scale > 3 ) break;
                if( ins->operands[0] == ins->result ) {
                    sib.reg = sib.index->i.index;
                } else {
                    HW_Asgn( tmp, sib.index->i.index->r.reg );
                    HW_TurnOff( tmp, ins->result->r.reg );

                    // following line is to prevent turning
                    // "shl r1,n -> r2; op i -> base[ r1 + r2 ]"
                    // into something which would look like
                    // "op i -> base[ r1 + r1 * f(n) ]"
                    // which we couldn't handle because X_LOW_BASE
                    // stuff can't (currently) deal with same reg being
                    // both base and index.  BBB - May 17, 1996
                    if( HW_Ovlap( tmp, other_reg ) ) break;

                    HW_TurnOn( tmp, other_reg );
                    sib.reg = AllocRegName( tmp );
                    tmp = Low64Reg( tmp );
                    if( sib.flags & X_LOW_BASE ) {
                        if( HW_Equal( tmp, other_reg ) ) {
                            sib.flags ^= ( X_HIGH_BASE | X_LOW_BASE );
                        }
                    } else if( sib.flags & X_HIGH_BASE ) {
                        if( !HW_Equal( tmp, other_reg ) ) {
                            sib.flags ^= ( X_HIGH_BASE | X_LOW_BASE );
                        }
                    }
                }
            }
        } else { /* ADD */
            sib.flags = sib.index->i.index_flags & ~( X_HIGH_BASE | X_LOW_BASE );
            if( sib.index->i.index_flags & X_BASE ) break;
            if( sib.index->i.base && sib.index->i.base->n.class == N_TEMP ) {
                break;
            }
            sib.scale = sib.index->i.scale;
            if( sib.scale != 0 ) break;
            if( ins->operands[0] == ins->result ) {
                HW_Asgn( tmp, base_reg );
                HW_TurnOn( tmp, sib.index->i.index->r.reg );
            } else {
                HW_Asgn( tmp, sib.index->i.index->r.reg );
                HW_COnlyOn( tmp, HW_SEGS );
                HW_TurnOn( tmp, other_reg );
                HW_TurnOn( tmp, base_reg );
            }
            sib.reg = AllocRegName( tmp );
            sib.flags |= X_HIGH_BASE;
        }
        _Alloc( curr_sib, sizeof( sib_info ) );
        Copy( (byte *)&sib, (byte *)curr_sib, sizeof( sib_info ) );
        curr_sib->next = sib_head;
        sib_head = curr_sib;
    } while( !dies && !modifies );
    if( dies && curr_sib != NULL ) {
        curr_sib = sib_head;
        while( curr_sib != NULL ) {
            new_x = ScaleIndex( curr_sib->reg, curr_sib->index->i.base,
                                curr_sib->index->i.constant + curr_sib->offset,
                                curr_sib->index->n.name_class,
                                curr_sib->index->n.size, curr_sib->scale,
                                curr_sib->flags );
            ReplaceOperand( curr_sib->ins, curr_sib->index, new_x );
            curr_sib = curr_sib->next;
        }
        FreeIns( ins );
    } else {
        dies = FALSE;
    }
    while( sib_head != NULL ) {
        curr_sib = sib_head;
        sib_head = sib_head->next;
        _Free( curr_sib, sizeof( sib_info ) );
    }
    return( dies );
}
