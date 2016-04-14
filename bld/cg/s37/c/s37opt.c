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
#include "makeins.h"

extern  void            PrefixIns(instruction*,instruction*);
extern  void            SuffixIns(instruction*,instruction*);
extern  name            *AllocTemp(type_class_def);
extern  name            *AllocIntConst(int);
extern  void            FindReferences(void);

extern  block           *HeadBlock;
extern  name            *Names[];


static bool WorthAConversion( name *temp )
/*****************************************

     NYI
*/
{
    temp = temp;
    return( true );
}


static instruction *AndResult( instruction *ins, type_class_def class )
/**********************************************************************

    And the result of "ins" with 255 or 65535 to clear out the high
    order junk.
*/
{
    signed_32           constant;
    instruction         *new_ins;

    if( class == U1 ) {
        constant = 0xFF;
    } else if( class == U2 ) {
        constant = 0xFFFF;
    } else {
        return( ins );
    }
    new_ins = MakeBinary( OP_AND, ins->result, AllocIntConst( constant ),
                          ins->result, SW );
    SuffixIns( ins, new_ins );
    return( new_ins );
}


static void ConvertOtherOperands( instruction *ins, name *temp )
/***************************************************

    Change ins to an integer instruction, and convert any other operands
    to integer.
*/
{
    instruction *new_ins;
    int         i;

    ins->table = NULL;
    i = ins->num_operands;
    while( --i >= 0 ) {
        if( ins->operands[i] != temp &&
            ins->operands[i]->n.class != N_CONSTANT ) {
            new_ins = MakeConvert( ins->operands[i], AllocTemp( SW ),
                                   SW, ins->operands[i]->n.name_class );
            ins->operands[i] = new_ins->result;
            PrefixIns( ins, new_ins );
        }
    }
    if( ins->result != NULL && ins->result != temp ) {
        new_ins = MakeConvert( AllocTemp( SW ), ins->result,
                               ins->result->n.name_class, SW );
        ins->result = new_ins->operands[0];
        SuffixIns( ins, new_ins );
    }
}


static bool ConvertInsToInt( instruction *ins, name *temp )
/**********************************************************

    Convert ins to an integer instruction if "temp" is involved. Return true
    if we changed something.
*/
{
    int         i;

    if( ins->head.opcode == OP_CONVERT || ins->head.opcode == OP_MOV ) {
        if( ins->operands[0] == temp ) {
            ins->head.opcode = OP_CONVERT;
            ins->base_type_class = SW;
            ins->table = NULL;
            return( true );
        }
        if( ins->result == temp ) {
            ins->head.opcode = OP_CONVERT;
            ins->table = NULL;
            ins->type_class = SW;
            return( true );
        }
    } else {
        i = ins->num_operands;
        while( --i >= 0 ) {
            if( ins->operands[i] == temp ) {
                ins->type_class = SW;
                ConvertOtherOperands( ins, temp );
                return( true );
            }
        }
        if( ins->result == temp ) {
            ins->type_class = SW;
            ConvertOtherOperands( ins, temp );
            return( true );
        }
    }
    return( false );
}


static void FixConverts()
/************************

    We may have created some convert from type X to type X instructions.
    Make them into move instructions, to give later optimizations an
    easier time of it.
*/
{
    block       *blk;
    instruction *ins;

    blk = HeadBlock;
    while( blk != NULL ) {
        ins = blk->ins.hd.next;
        while( ins->head.opcode != OP_BLOCK ) {
            if( ins->head.opcode == OP_CONVERT ) {
                if( ins->type_class == ins->base_type_class ) {
                    ins->head.opcode = OP_MOV;
                }
            }
            ins = ins->head.next;
        }
        blk = blk->next_block;
    }
}

static bool ConvertToInt( name *temp )
/*************************************

    Do the actual word to convert temp to an integer
*/
{
    block               *blk;
    instruction         *ins;
    type_class_def      class;
    bool                change;

    blk = HeadBlock;
    change = false;
    temp->n.name_class = SW;
    temp->n.size = WORD_SIZE;
    while( blk != NULL ) {
        ins = blk->ins.hd.next;
        while( ins->head.opcode != OP_BLOCK ) {
             class = ins->type_class;
             change |= ConvertInsToInt( ins, temp );
             if( ins->result == temp ) {
                 ins = AndResult( ins, class );
             }
             ins = ins->head.next;
        }
        blk = blk->next_block;
    }
    return( change );
}


static bool ConvertIfPossible( name *temp )
/******************************************

    Convert temp to an integer if we can.
*/
{
    if( ( temp->v.usage & USE_ADDRESS+USE_MEMORY+VAR_VOLATILE+NEEDS_MEMORY ) ) {
        return( false );
    }
    if( temp->t.temp_flags & ALIAS ) return( false );
    if( temp->t.alias != temp ) return( false );
    switch( temp->n.name_class ) {
    case I1:
    case I2:
        return( ConvertToInt( temp ) );
    case U1:
    case U2:
        if( !WorthAConversion( temp ) ) return( false );
        return( ConvertToInt( temp ) );
    }
    return( false );
}


extern bool CharsAndShortsToInts()
/***********************************

    Convert char and short variables to integer.
*/
{
    name        *temp;
    bool        change;

    change = false;
    for( temp = Names[N_TEMP]; temp != NULL; temp = temp->n.next_name ) {
        change |= ConvertIfPossible( temp );
    }
    FixConverts();
    if( change ) {
        FindReferences();
    }
    return( change );
}
