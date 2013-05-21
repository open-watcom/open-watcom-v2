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
#include "zoiks.h"

extern  int             NumOperands(instruction*);

static  void            MoveSeg( instruction *ins, instruction *new_ins,
                                 name *op, bool save_old );

extern  void    DelSeg( instruction *ins ) {
/******************************************/

    if( NumOperands( ins ) < ins->num_operands ) {
        ins->num_operands--;
    }
}


extern  void    DupSeg( instruction *ins, instruction *new_ins ) {
/****************************************************************/

    if( ins->head.opcode == OP_BLOCK ) return;
    if( ins->num_operands <= NumOperands( ins ) ) return;
    if( new_ins->num_operands > NumOperands( new_ins ) ) return;
    new_ins->operands[ new_ins->num_operands++ ] =
            ins->operands[ ins->num_operands-1 ];
    new_ins->t.index_needs = ins->t.index_needs;
}


extern  void    DupSegOp( instruction *ins, instruction *new_ins, int i ) {
/*************************************************************************/


    MoveSeg( ins, new_ins, new_ins->operands[ i ], TRUE );
}


extern  void    DupSegRes( instruction *ins, instruction *new_ins ) {
/*******************************************************************/


    MoveSeg( ins, new_ins, new_ins->result, TRUE );
}


extern  void    MoveSegOp( instruction *ins, instruction *new_ins, int i ) {
/**************************************************************************/


    MoveSeg( ins, new_ins, new_ins->operands[ i ], FALSE );
}


extern  void    MoveSegRes( instruction *ins, instruction *new_ins ) {
/********************************************************************/


    MoveSeg( ins, new_ins, new_ins->result, FALSE );
}


static bool SegMemLoc( name *op ) {
/************************************/
/*
 * Return true if the operand COULD be the one associated with the segment */

    if( op->n.class == N_INDEXED ) return( TRUE );
    if( op->n.class != N_MEMORY ) return( FALSE );
    if( op->m.memory_type == CG_LBL ) {
        /* kluge for TLS stuff - want to be able to put fs: override on
           the __tls_array runtime label - BBB May 15, 1996 */
        if( AskIfRTLabel( op->v.symbol ) )  return( TRUE );
        return( FALSE ); /* made by Addressable */
    }
    if( op->m.memory_type == CG_CLB ) return( FALSE ); /* made by Addressable */
    return( TRUE );
}


extern  void    DelSegRes( instruction *ins ) {
/*********************************************/

    int         i;

    if( ins->num_operands <= NumOperands( ins ) ) return;
    if( !SegMemLoc( ins->result ) ) return;
    i = ins->num_operands;
    while( --i >= 0 ) {
        if( ins->result == ins->operands[ i ] ) return;
    }
    DelSeg( ins );
}


extern  void    DelSegOp( instruction *ins, int i ) {
/***************************************************/

    int         j;

    if( ins->num_operands <= NumOperands( ins ) ) return;
    if( !SegMemLoc( ins->operands[ i ] ) ) return;
    if( ins->result == ins->operands[ i ] ) return;
    j = ins->num_operands;
    while( --j >= 0 ) {
        if( i != j ) {
            if( ins->operands[ j ] == ins->operands[ i ] ) return;
        }
    }
    DelSeg( ins );
}


static  void    MoveSeg( instruction *ins, instruction *new_ins,
                         name *op, bool save_old ) {
/**************************************************/

    int         i;

    if( ins->head.opcode == OP_BLOCK ) return;
    if( ins->num_operands <= NumOperands( ins ) ) return;
    if( !SegMemLoc( op ) ) return;
    DupSeg( ins, new_ins );
    if( save_old ) return;
    i = ins->num_operands;
    while( --i >= 0 ) {
        if( op == ins->operands[ i ] ) return;
    }
    if( op == ins->result ) return;
    DelSeg( ins );
}
