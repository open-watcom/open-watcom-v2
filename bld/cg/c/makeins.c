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


#include <string.h>
#include "cgstd.h"
#include "coderep.h"
#include "zoiks.h"
#include "freelist.h"
#include "data.h"
#include "makeins.h"
#include "namelist.h"


extern  void            DoNothing(instruction*);

static  pointer         *InsFrl;

extern  void    InitIns( void ) {
/********************************
    Initialize the free list of "instruction"
*/

    InitFrl( &InsFrl );
}


extern  bool    InsFrlFree( void ) {
/******************************
    Free up the "instruction" free list.
*/

    return( FrlFreeAll( &InsFrl, INS_SIZE ) );
}


extern  void    FreeIns( instruction *ins ) {
/********************************************
    Free an instruction "ins"
*/
    instruction         *next;

#if 0  /* Debugging code for integrity of conflict edges */
    conflict_node       *conf;

    for (conf = ConfList; conf != NULL; conf = conf->next_conflict) {
        if (conf->ins_range.first == ins || conf->ins_range.last == ins) {
            Zoiks(ZOIKS_050);
        }
    }
#endif

    next = ins->head.next;
    if( next->head.opcode != OP_BLOCK && next->head.line_num == 0 ) {
        next->head.line_num = ins->head.line_num;
    }

    ins->head.prev->head.next = next;
    next->head.prev = ins->head.prev;
    FrlFreeSize( &InsFrl, (pointer *)ins, INS_SIZE );
}


extern  instruction     *NewIns( int num ) {
/*******************************************
    Allocate a new instruction (with "num" operands) and initialize its
    fields to sensible defaults.
*/

    instruction *new;

    if( num > MAX_OPS_PER_INS ) {
        _Zoiks( ZOIKS_026 );
    }
    new = AllocFrl( &InsFrl, INS_SIZE );
    new->head.state = INS_NEEDS_WORK;
    new->t.index_needs = RL_;
    new->num_operands = num;
    new->result = NULL;
    memset( &new->flags, 0, sizeof( new->flags ) );
    new->ins_flags = 0;
    new->head.line_num = 0;
    new->sequence = 0;
    new->stk_entry = 0;
    new->stk_exit = 0;
    new->s.stk_extra = 0;
    for( num = MAX_OPS_PER_INS; num-- > 0; ) {
        new->operands[num] = NULL;
    }
    new->table = NULL;
    new->u.gen_table = NULL;
    new->base_type_class = XX;
    new->zap = (register_name *) AllocRegName( HW_EMPTY );
    return( new );
}


extern  instruction     *MakeNop( void ) {
/*****************************************
    Return an initialized "NOP" instruction
*/

    instruction *ins;

    ins = NewIns( 0 );
    ins->head.opcode = OP_NOP;
    ins->head.state = INS_READY;
    ins->type_class = XX;
    DoNothing( ins );
    return( ins );
}


extern  instruction     *MakeNary( opcode_defs opcode, name *left,
                                     name *right, name *result,
                                     type_class_def class,
                                     type_class_def base_class,
                                     int num_operands ) {
/************************************************************
    Make a N-ary instruction (OPCODE left, right => result)
*/

    instruction *ins;

    ins = NewIns( num_operands );
    ins->head.opcode = opcode;
    ins->type_class = class;
    ins->base_type_class = base_class;
    ins->operands[ 0 ] = left;
    ins->operands[ 1 ] = right;
    ins->result = result;
    return( ins );
}


extern  instruction     *MakeMove( name *src, name *dst,
                                   type_class_def class ) {
/**********************************************************
    Make a move instruction (MOV src => dst)
*/

    instruction *move;

    move = MakeNary( OP_MOV, src, NULL, dst, class, XX, 1 );
    return( move );
}


extern  instruction     *MakeUnary( opcode_defs op, name *src,
                                    name *dst, type_class_def class ) {
/**********************************************************************
    Make a unary instruction (op  src => dst)
*/

    return( MakeNary( op, src, NULL, dst, class, XX, 1 ) );
}


extern  instruction     *MakeConvert( name *src, name *dst, type_class_def class,
                                      type_class_def base_class ) {
/**********************************************************************
    Make a conversion instruction.  Convert "src" (type = "base_class") to
    "dst" (type = "class")
*/

    return( MakeNary( OP_CONVERT, src, NULL, dst, class, base_class, 1 ) );
}


extern  instruction     *MakeRound( name *src, name *dst, type_class_def class,
                                      type_class_def base_class ) {
/**********************************************************************
    Make a round instruction.  Convert "src" (type = "base_class") to
    "dst" (type = "class")
*/

    return( MakeNary( OP_ROUND, src, NULL, dst, class, base_class, 1 ) );
}


extern  instruction     *MakeBinary( opcode_defs opcode, name *left,
                                     name *right, name *result,
                                     type_class_def class ) {
/************************************************************
    Make a binary instruction (OPCODE left, right => result)
*/

    return( MakeNary( opcode, left, right, result, class, XX, 2 ) );
}


extern  instruction     *MakeCondition( opcode_defs opcode, name *left,
                                        name *right, int t, int f,
                                        type_class_def class ) {
/***************************************************************
    Make a conditional (IF ( left opcode right ) goto "t" else goto "f")
*/

    instruction *cond;

    cond = MakeNary( opcode, left, right, NULL, class, XX, 2 );
    _SetBlockIndex( cond, t, f );
    return( cond );
}
