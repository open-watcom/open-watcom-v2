/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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


#include "_cgstd.h"
#include "coderep.h"
#include "zoiks.h"
#include "freelist.h"
#include "data.h"
#include "makeins.h"
#include "namelist.h"
#include "optab.h"


static  pointer         *InsFrl;

void    InitIns( void )
/********************************
 * Initialize the free list of "instruction"
 */
{
    InitFrl( &InsFrl );
}


bool    InsFrlFree( void )
/******************************
 * Free up the "instruction" free list.
 */
{
    return( FrlFreeAll( &InsFrl, INS_SIZE ) );
}


void    FreeIns( instruction *ins )
/********************************************
 * Free an instruction "ins"
 */
{
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


instruction     *NewIns( opcnt num )
/***********************************
 * Allocate a new instruction (with "num" operands) and initialize its
 * fields to sensible defaults.
 */
{
    instruction *new_ins;

    if( num > MAX_OPS_PER_INS ) {
        _Zoiks( ZOIKS_026 );
    }
    new_ins = AllocFrl( &InsFrl, INS_SIZE );
    new_ins->head.state = INS_NEEDS_WORK;
    new_ins->u2.index_needs = RL_;
    new_ins->num_operands = num;
    new_ins->result = NULL;
    memset( &new_ins->flags, 0, sizeof( new_ins->flags ) );
    new_ins->ins_flags = 0;
    new_ins->head.line_num = 0;
    new_ins->sequence = 0;
    new_ins->stk_entry = 0;
    new_ins->stk_exit = 0;
    new_ins->fp.u.stk_extra = 0;
    for( num = MAX_OPS_PER_INS; num-- > 0; ) {
        new_ins->operands[num] = NULL;
    }
    new_ins->table = NULL;
    new_ins->u.gen_table = NULL;
    new_ins->base_type_class = XX;
    new_ins->zap = (register_name *)AllocRegName( HW_EMPTY );
    return( new_ins );
}


instruction     *MakeNop( void )
/*****************************************
 * Return an initialized "NOP" instruction
 */
{
    instruction *ins;

    ins = NewIns( 0 );
    ins->head.opcode = OP_NOP;
    ins->head.state = INS_READY;
    ins->type_class = XX;
    DoNothing( ins );
    return( ins );
}


instruction     *MakeNary( opcode_defs opcode, name *left,
                                     name *right, name *result,
                                     type_class_def type_class,
                                     type_class_def base_type_class,
                                     opcnt num_operands )
/************************************************************
 * Make a N-ary instruction (OPCODE left, right => result)
 */
{
    instruction *ins;

    ins = NewIns( num_operands );
    ins->head.opcode = opcode;
    ins->type_class = type_class;
    ins->base_type_class = base_type_class;
    ins->operands[0] = left;
    ins->operands[1] = right;
    ins->result = result;
    return( ins );
}


instruction     *MakeMove( name *src, name *dst,
                                   type_class_def type_class )
/**********************************************************
 * Make a move instruction (MOV src => dst)
 */
{
    instruction *move;

    move = MakeNary( OP_MOV, src, NULL, dst, type_class, XX, 1 );
    return( move );
}


instruction     *MakeUnary( opcode_defs opcode, name *src,
                                    name *dst, type_class_def type_class )
/**********************************************************************
 * Make a unary instruction (opcode  src => dst)
 */
{
    return( MakeNary( opcode, src, NULL, dst, type_class, XX, 1 ) );
}


instruction     *MakeConvert( name *src, name *dst, type_class_def type_class,
                                      type_class_def base_type_class )
/**********************************************************************
 * Make a conversion instruction.  Convert "src" (type = "base_class") to
 * "dst" (type = "class")
 */
{
    return( MakeNary( OP_CONVERT, src, NULL, dst, type_class, base_type_class, 1 ) );
}


instruction     *MakeRound( name *src, name *dst, type_class_def type_class,
                                      type_class_def base_type_class )
/**********************************************************************
 * Make a round instruction.  Convert "src" (type = "base_class") to
 * "dst" (type = "class")
 */
{
    return( MakeNary( OP_ROUND, src, NULL, dst, type_class, base_type_class, 1 ) );
}


instruction     *MakeBinary( opcode_defs opcode, name *left,
                                     name *right, name *result,
                                     type_class_def type_class )
/************************************************************
 * Make a binary instruction (OPCODE left, right => result)
 */
{
    return( MakeNary( opcode, left, right, result, type_class, XX, 2 ) );
}


instruction     *MakeCondition( opcode_defs opcode,
                                name *left, name *right,
                                cond_dst_idx t, cond_dst_idx f,
                                type_class_def type_class )
/***************************************************************
 * Make a conditional (IF ( left opcode right ) goto "t" else goto "f")
 */
{
    instruction *cond;

    cond = MakeNary( opcode, left, right, NULL, type_class, XX, 2 );
    _SetBlockIndex( cond, t, f );
    return( cond );
}
