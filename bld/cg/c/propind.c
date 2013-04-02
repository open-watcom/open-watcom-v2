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
#include "zoiks.h"
#include "typedef.h"
#include "model.h"
#include "cgdefs.h"

extern bool PeepOptBlock(block *, bool);
extern void SuffixIns(instruction *,instruction *);
extern bool SameThing(name *,name *);
extern name *ScaleIndex(name *,name *,type_length ,type_class_def ,type_length ,int ,i_flags );
extern bool ReDefinedBy(instruction *,name *);
extern void RemoveIns( instruction *);

extern block            *HeadBlock;

static  byte    OpRefs( name *op, name *ref ) {
/*********************************************/
    byte        refs;

    if( SameThing( op, ref ) ) return( 1 );
    refs = 0;
    if( op->n.class == N_INDEXED ) {
        if( SameThing( op->i.base, ref ) ) refs++;
        if( SameThing( op->i.index, ref ) ) refs++;
    }
    return( refs );
}

static  byte    NumRefs( instruction *ins, name *op ) {
/******************************************************
    Return the number of references to op found in ins.
*/
    byte        refs;
    byte        i;

    refs = 0;
    for( i = 0; i < ins->num_operands; i++ ) {
        refs += OpRefs( ins->operands[ i ], op );
    }
    if( ins->result != NULL ) {
        refs += OpRefs( ins->result, op );
    }
    return( refs );
}

static  bool    Adjusted( name **pop, name *res, signed_32 c ) {
/*************************************************************/
    name        *op;

    op = *pop;
    if( op->n.class == N_INDEXED ) {
        if( SameThing( op->i.index, res ) ) {
            *pop = ScaleIndex( op->i.index, op->i.base, op->i.constant + c,
                op->n.name_class, op->n.size, op->i.scale, op->i.index_flags );
            return( TRUE );
        }
    }
    return( FALSE );
}


static  bool    AdjustIndex( instruction *ins, name *res, signed_32 c ) {
/************************************************************************
    If any operands or results of ins refer to res in an index, adjust the
    constant by the given amount and return TRUE.
*/
    byte        i;

    if( NumRefs( ins, res ) != 1 ) return( FALSE );
    for( i = 0; i < ins->num_operands; i++ ) {
        if( Adjusted( &ins->operands[ i ], res, c ) ) return( TRUE );
    }
    if( ins->result != NULL ) {
        if( Adjusted( &ins->result, res, c ) ) return( TRUE );
    }
    return( FALSE );
}


static  bool    DoProp( block *blk ) {
/*************************************
    Propagate constants used in ADD or SUB instructions into
    indexes.
*/
    instruction *ins;
    instruction *next;
    name        *op;

    ins = blk->ins.hd.next;
    while( ins->head.opcode != OP_BLOCK ) {
        if( ins->head.opcode == OP_ADD || ins->head.opcode == OP_SUB ) {
            op = ins->operands[ 1 ];

            /*
             * 2004-11-04  RomanT
             *
             *    Improper swapping of operands confuses codegen which
             * expecting to see known types on left and right side of ins.
             *    I took check for _IsPointer from "foldins.c" which
             * performing similar tasks. It fixed far pointers in "-ot -ol+"
             * mode. If problems will appear again, paragraph below can be
             * safely commented out (with possible cost of code quality).
             */

            if( ins->head.opcode == OP_ADD &&
                !_IsPointer( ins->type_class ) &&  /* 2004-11-04  RomanT */
                ( op->n.class != N_CONSTANT ||
                op->c.const_type != CONS_ABSOLUTE ) ) {
                if( ins->operands[ 0 ]->n.class != N_CONSTANT ||
                    ins->operands[ 0 ]->c.const_type == CONS_ABSOLUTE ) {
                    // don't move scary constant (ADDRESS) over to right
                    // BBB - May 14, 1997
                    ins->operands[ 1 ] = ins->operands[ 0 ];
                    ins->operands[ 0 ] = op;
                    op = ins->operands[ 1 ];
                }
            }

            if( op->n.class == N_CONSTANT &&
                op->c.const_type == CONS_ABSOLUTE &&
                ins->result == ins->operands[ 0 ] ) {
                next = ins->head.next;
                while( next->head.opcode != OP_BLOCK ) {
                    if( ReDefinedBy( next, ins->result ) ) break;
                    if( AdjustIndex( next, ins->result, op->c.int_value ) ) {
                        RemoveIns( ins );
                        SuffixIns( next, ins );
                        PeepOptBlock( blk, FALSE );
                        return( TRUE );
                    }
                    if( NumRefs( next, ins->result ) != 0 ) break;
                    next = next->head.next;
                }
            }
        }
        ins = ins->head.next;
    }
    return( FALSE );
}

void            PropIndexes( block *blk ) {
/******************************************
    See above.
*/
    while( DoProp( blk ) );
}
