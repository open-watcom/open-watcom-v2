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
#include "opcodes.h"
#include "s37sib.def"
#include "cgmem.h"
#include "makeins.h"

extern  name            *AllocRegName(hw_reg_set);
extern  name            *ScaleIndex(name*,name*,type_length,type_class_def,type_length,int,i_flags);
extern  hw_reg_set      HighReg(hw_reg_set);
extern  bool            IsRXInstruction(instruction*);
extern  bool            IsIndexReg(hw_reg_set,type_class_def,bool);
extern  instruction     *SIBPossibleIndex(instruction*,name*,name**,bool*,hw_reg_set,hw_reg_set,bool*,bool*);
extern  void            ReplaceOperand(instruction*,name*,name*);
extern  byte            *Copy(byte*,byte*,uint);

extern  block   *HeadBlock;

typedef struct sib_info {
    struct sib_info     *next;
    name                *reg;
    name                *index;
    i_flags             flags;
    instruction         *ins;
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

    if( ins->head.opcode == OP_ADD ) {
        cons = ins->operands[ 1 ];
        if( cons->n.class != N_REGISTER ) return( FALSE );
        if( cons->n.size != WORD_SIZE ) return( FALSE );
        base_reg = cons->r.reg;
        if( !IsIndexReg( base_reg, WD, 0 ) ) return( FALSE );
/*
        found ADD R1,R2 => R1
*/
        if( cons == ins->operands[ 0 ] ) {
/*
        found ADD R1,R1 => R1  <==> SHL R1,1 => R1
*/
            return( FALSE );
        }
    } else {
        return( FALSE );
    }
    dies = FALSE;
    sib_head = NULL;
    other_reg = ins->operands[ 0 ]->r.reg;
    next = ins;
    do {
        curr_sib = NULL;
        next = SIBPossibleIndex( next, ins->result, &sib.index,
                                 &is_base, base_reg, other_reg,
                                 &dies, &modifies );
        if( next == NULL ) break;
        sib.ins = next;
        if( !IsRXInstruction( next ) ) break;
        if( sib.index->i.index_flags & X_BASE ) break;
        if( HasTrueBase( sib.index ) && sib.index->i.base->n.class == N_TEMP ) {
            break;
        }
        tmp = base_reg;
        HW_TurnOn( tmp, sib.index->i.index->r.reg );
        sib.reg = AllocRegName( tmp );
        sib.flags = sib.index->i.index_flags & ~( X_HIGH_BASE | X_LOW_BASE );
        tmp = HighReg( sib.reg->r.reg );
        if( HW_Equal( tmp, base_reg ) ) {
            sib.flags |= X_HIGH_BASE;
        } else {
            sib.flags |= X_LOW_BASE;
        }
        curr_sib = CGAlloc( sizeof( sib_info ) );
        Copy( (byte *)&sib, (byte *)curr_sib, sizeof( sib_info ) );
        curr_sib->next = sib_head;
        sib_head = curr_sib;
    } while( !dies && !modifies );
    if( dies && curr_sib != NULL ) {
        curr_sib = sib_head;
        while( curr_sib != NULL ) {
            new_x = ScaleIndex( curr_sib->reg, curr_sib->index->i.base,
                                curr_sib->index->i.constant,
                                curr_sib->index->n.name_class,
                                curr_sib->index->n.size, 0,
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
        CGFree( curr_sib );
    }
    return( dies );
}
