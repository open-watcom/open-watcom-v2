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
#include "opcodes.h"
#include "cgdefs.h"

extern    block         *HeadBlock;
extern    conflict_node *ConfList;

extern  bool            PropagateMoves();
extern  instruction_id  Renumber();
extern  bool            FPIsStack(name*);
extern  bool            SideEffect(instruction*);
extern  void            FreeIns(instruction*);
extern  conflict_node*  FindConflictNode(name*,block*,instruction*);
extern  void            FreeConflicts();
extern  void            NullConflicts(var_usage);
extern  void            FindReferences();
extern  void            MakeConflicts();
extern  void            MakeLiveInfo();
extern  bool            ReDefinedBy(instruction*,name*);
extern  void            FreeJunk(block*);

typedef enum {
        MOVEABLE        = 0x01,
        SWAPABLE        = 0x02,
        SIDE_EFFECT     = 0x04,
        PUSHABLE        = 0x08
} opcode_attr;

static  opcode_attr OpAttrs[LAST_OP-FIRST_OP+1] = {
/********************************
    define the attributes of any given opcode
*/
        FALSE,                          /* OP_NOP,*/
/* NOP's are always where they are for a good reason*/
                                        /*   binary operators*/

        PUSHABLE,                       /* OP_ADD,*/
        MOVEABLE,                       /* OP_EXT_ADD,*/
        PUSHABLE,                       /* OP_SUB,*/
        MOVEABLE,                       /* OP_EXT_SUB,*/
        MOVEABLE +      SWAPABLE,       /* OP_MUL,*/
        MOVEABLE +      SWAPABLE,       /* OP_EXT_MUL,*/
        MOVEABLE +      SWAPABLE,       /* OP_DIV,*/
        MOVEABLE +      SWAPABLE,       /* OP_MOD,*/
        MOVEABLE +      SWAPABLE,       /* OP_AND,*/
        MOVEABLE +      SWAPABLE,       /* OP_OR,*/
        MOVEABLE +      SWAPABLE,       /* OP_XOR,*/
        MOVEABLE +      SWAPABLE,       /* OP_RSHIFT,*/
        MOVEABLE +      SWAPABLE,       /* OP_LSHIFT,*/
        MOVEABLE +      SWAPABLE,       /* OP_POW */
        MOVEABLE +      SWAPABLE,       /* OP_P5FDIV */
        MOVEABLE +      SWAPABLE,       /* OP_ATAN2 */
        MOVEABLE +      SWAPABLE,       /* OP_FMOD */
        MOVEABLE +      SWAPABLE,       /* OP_NEGATE,*/
        MOVEABLE +      SWAPABLE,       /* OP_COMPLEMENT,*/
        MOVEABLE +      SWAPABLE,       /* OP_LOG */
        MOVEABLE +      SWAPABLE,       /* OP_COS */
        MOVEABLE +      SWAPABLE,       /* OP_SIN */
        MOVEABLE +      SWAPABLE,       /* OP_TAN */
        MOVEABLE +      SWAPABLE,       /* OP_SQRT */
        MOVEABLE +      SWAPABLE,       /* OP_FABS */
        MOVEABLE +      SWAPABLE,       /* OP_ACOS */
        MOVEABLE +      SWAPABLE,       /* OP_ASIN */
        MOVEABLE +      SWAPABLE,       /* OP_ATAN */
        MOVEABLE +      SWAPABLE,       /* OP_COSH */
        MOVEABLE +      SWAPABLE,       /* OP_EXP */
        MOVEABLE +      SWAPABLE,       /* OP_LOG10 */
        MOVEABLE +      SWAPABLE,       /* OP_SINH */
        MOVEABLE +      SWAPABLE,       /* OP_TANH */
        MOVEABLE +      SWAPABLE,       /* OP_PTR_TO_NATIVE  */
        MOVEABLE +      SWAPABLE,       /* OP_PTR_TO_FORIEGN  */
        FALSE,                          /* OP_SLACK */

        MOVEABLE +      SWAPABLE,       /* OP_CONVERT,*/
        MOVEABLE +      SWAPABLE,       /* OP_LA,*/
        MOVEABLE +      SWAPABLE,       /* OP_CAREFUL_LA*/
        MOVEABLE +      SWAPABLE,       /* OP_ROUND,*/

        MOVEABLE +      SWAPABLE,       /* OP_MOV,*/

                                        /*   Calling operators*/

        SIDE_EFFECT,                    /* OP_CALL_INDIRECT,*/
                        SWAPABLE,       /* OP_PUSH,*/
                        SWAPABLE,       /* OP_POP,*/
        FALSE,                          /* OP_PARM_DEF,*/

                                        /*   Select statement operator*/

        FALSE,                          /* OP_SELECT*/

/*   Condition code setting instructions (no result, third operand is a label)*/

        SIDE_EFFECT,                    /* OP_BIT_TEST_TRUE,*/
        SIDE_EFFECT,                    /* OP_BIT_TEST_FALSE,*/
        SIDE_EFFECT,                    /* OP_CMP_EQUAL,*/
        SIDE_EFFECT,                    /* OP_CMP_NOT_EQUAL,*/
        SIDE_EFFECT,                    /* OP_CMP_GREATER,*/
        SIDE_EFFECT,                    /* OP_CMP_LESS_EQUAL,*/
        SIDE_EFFECT,                    /* OP_CMP_LESS,*/
        SIDE_EFFECT,                    /* OP_CMP_GREATER_EQUAL,*/

/*   Operator whose argument is a label*/

        SIDE_EFFECT,                    /* OP_CALL,*/

        SIDE_EFFECT,                    /* OP_SET_EQUAL,*/
        SIDE_EFFECT,                    /* OP_SET_NOT_EQUAL,*/
        SIDE_EFFECT,                    /* OP_SET_GREATER,*/
        SIDE_EFFECT,                    /* OP_SET_LESS_EQUAL,*/
        SIDE_EFFECT,                    /* OP_SET_LESS,*/
        SIDE_EFFECT,                    /* OP_SET_GREATER_EQUAL,*/

        FALSE,                          /* OP_DEBUG_INFO */
        FALSE,                          /* OP_CHEAP_NOP */

        FALSE,                          /* OP_LOAD_UNALIGNED */
        FALSE,                          /* OP_STORE_UNALIGNED */
        FALSE,                          /* OP_EXTRACT_LOW */
        FALSE,                          /* OP_EXTRACT_HIGH */
        FALSE,                          /* OP_INSERT_LOW */
        FALSE,                          /* OP_INSERT_HIGH */
        FALSE,                          /* OP_MASK_LOW */
        FALSE,                          /* OP_MASK_HIGH */
        FALSE,                          /* OP_ZAP */
        FALSE,                          /* OP_ZAP_NOT */

        FALSE,                          /* OP_STK_ALLOC */
        FALSE,                          /* OP_VA_START */

/* add opcodes here */

        FALSE,                          /* OP_SLACK */
        FALSE,                          /* OP_SLACK */
        FALSE,                          /* OP_SLACK */
        FALSE,                          /* OP_SLACK */
        FALSE,                          /* OP_SLACK */
        FALSE,                          /* OP_SLACK */
        FALSE,                          /* OP_SLACK */
        FALSE,                          /* OP_SLACK */

        FALSE                           /* OP_BLOCK */
    };


#define _IsIns( ins, attr )        ( OpAttrs[ ins->head.opcode ] & attr )
#define _IsntIns( ins, attr )      ( _IsIns( ins, attr ) == FALSE )


extern  void    PushPostOps() {
/******************************
    This routine tries to push add and subtract instructions
    forward in the basic block. The reason for this is that
    it untangles post increment code allowing for copy propagation
    and code elimination. Take *x++ = *y++ for example

    Before:                     After:                  With copy propagation:

    MOV x => t1                 MOV x => t1             MOV x => t1 (useless)
    MOV y => t2                 MOV y => t2             MOV y => t2 (useless)
    ADD x, 1 => x               MOV [t2] => [t1]        MOV [y] => [x]
    ADD y, 1 => y               ADD x,1 => x            ADD x,1 => x
    MOV [t2] => [t1]            ADD y,1 => y            ADD y,1 => y
*/

/*    Perform instruction optimizations*/

    block       *blk;

    blk = HeadBlock;
    while( blk != NULL ) {
        PushInsForward( blk );
        blk = blk->next_block;
    }
    PropagateMoves();
    Renumber();
}


static  bool    ReDefinesOps( instruction *of, instruction *ins ) {
/******************************************************************
    return true if instruction "ins" redefines any of the operands
    or the result of instruction "of"
*/

    int         i;

    i = of->num_operands;
    while( --i >= 0 ) {
        if( of->operands[ i ]->n.class == N_REGISTER ) return( TRUE );
        if( ReDefinedBy( ins, of->operands[ i ] ) ) return( TRUE );
    }
    if( of->result != NULL ) {
        if( of->result->n.class == N_REGISTER ) return( TRUE );
        if( ReDefinedBy( ins, of->result ) ) return( TRUE );
    }
    return( FALSE );
}


static  bool    CanReorder( instruction *try, instruction *after ) {
/*******************************************************************
    return try if "try" could be moved past instruction "after"
*/

    if( ReDefinesOps( try, after ) ) return( FALSE );
    if( ReDefinesOps( after, try ) ) return( FALSE );
    return( TRUE );
}


static  instruction     *CanMoveAfter( instruction *ins ) {
/**********************************************************
    Decide whether we can/should push down instruction "ins". If we can,
    return a pointer to the instruction we could put it after.
*/

    instruction *next;

    if( _IsntIns( ins, PUSHABLE ) ) return( NULL );
    if( ins->operands[ 0 ] != ins->result ) return( NULL );
    if( ins->operands[ 1 ]->n.class != N_CONSTANT ) return( NULL );
    if( _IsFloating( ins->type_class ) ) return( NULL );
    if( _IsFloating( ins->base_type_class ) ) return( NULL );
    next = ins->head.next;
    for(;;) {
        if( _IsntIns( next, SWAPABLE ) ) break;
        if( CanReorder( ins, next ) == FALSE ) break;
        next = next->head.next;
        if( next == NULL ) break;
    }
    if( next == ins->head.next ) return( NULL );
    return( next );
}


static  void    PushInsForward( block *blk ) {
/*********************************************
    Try pushing down each instruction in the program.
*/

    instruction *ins;
    instruction *prev;
    instruction *next;

    ins = blk->ins.hd.prev;            /* move backwards through instructions*/
    for(;;) {
        if( ins->head.opcode == OP_BLOCK ) break;
        prev = ins->head.prev;

        next = CanMoveAfter( ins );
        if( next != NULL ) {

            /*   Remove 'ins' from its current position*/

            ins->head.next->head.prev = ins->head.prev;
            ins->head.prev->head.next = ins->head.next;

            /*   Link 'ins' before 'next'*/

            ins->head.prev = next->head.prev;
            next->head.prev = ins;
            ins->head.prev->head.next = ins;
            ins->head.next = next;
        }
        ins = prev;
    }
}


extern  bool    SameThing( name *x, name *y ) {
/**********************************************
    returns TRUE if "x" and "y" are the same thing. IE: N_MEMORY
    names which are associated with the same front end symbol or
    N_TEMP names which are aliases of each other. Two temporaries
    which are "stackable" are considered to be the same thing
    since they may (probably will) both end up on the 8087 stack.
*/

    if( x == y ) return( TRUE );
    if( x == NULL || y == NULL ) return( FALSE );
    if( FPIsStack( x ) && FPIsStack( y ) ) return( TRUE );
    if( x->n.class == N_MEMORY
     && y->n.class == N_MEMORY
     && x->v.symbol == y->v.symbol ) return( TRUE );
    if( x->n.class == N_TEMP && y->n.class == N_TEMP ) {
        if( x->t.v.id != y->t.v.id ) return( FALSE );
        if( !( x->t.temp_flags & ALIAS )
         && !( y->t.temp_flags & ALIAS ) ) return( FALSE );
        return( TRUE );
    }
    return( FALSE );
}


extern  void    DeadTemps() {
/****************************
    Using the information contained in each N_TEMP, delete any instruction
    whose result is a temporary that isnt used again. This is only a very
    crude hack of dead code, since it only gets temporaries which are local
    to a basic block and are only defined. Its designed to get rid of
    dummy instructions generated to preserve the value of an assignment
    operator. For example, *x = *y generates:
        MOV     [y] => [x]
        MOV     [x] => temp     <- this usually doesn't get used later
*/

    block       *blk;
    instruction *ins;
    instruction *next;

    blk = HeadBlock;
    while( blk != NULL ) {
        ins = blk->ins.hd.next;
        while( ins->head.opcode != OP_BLOCK ) {
            next = ins->head.next;
            if( SideEffect( ins ) == FALSE
             && _IsntIns( ins, SIDE_EFFECT )
             && ins->result != NULL
             && ins->result->n.class == N_TEMP
             && ( ins->result->v.usage
               &(USE_ADDRESS|HAS_MEMORY|USE_WITHIN_BLOCK|USE_IN_ANOTHER_BLOCK) )
               == EMPTY ) {
                FreeIns( ins );
            }
            ins = next;
        }
        blk = blk->next_block;
    }
}


static  bool    IsDeadIns( block *blk, instruction *ins, instruction *next ) {
/*****************************************************************************
    returns TRUE if an instruction is assigning to a name which is not
    live immediately following instruction "ins".
*/

    conflict_node       *conf;
    name                *op;

    if( SideEffect( ins ) ) return( FALSE );
    op = ins->result;
    if( op == NULL ) return( FALSE );
//  if( op->n.class != N_TEMP ) return( FALSE );
    if( op->v.usage & USE_ADDRESS ) return( FALSE );
    conf = FindConflictNode( op, blk, ins );
    if( conf == NULL ) return( FALSE );
    if( _LBitEmpty( conf->id.within_block )
     && _GBitEmpty( conf->id.out_of_block ) ) {
         return( FALSE );
     }
    if( _LBitOverlap( conf->id.within_block, next->head.live.within_block ) ) {
        return( FALSE );
    }
    if( _GBitOverlap( conf->id.out_of_block, next->head.live.out_of_block ) ) {
        return( FALSE );
    }
    return( TRUE );
}


extern  void    AxeDeadCode() {
/******************************
    This is like DeadTemps, but it uses the live information in order
    to discover whether an instruction is assigning to a variable that
    dies immediately following. If we change anything here, we redo
    the live information (that's not very expensive).
*/

/* delete instructions which assign to temporaries which will not be*/
/* used again before they are reassigned (ie: are not live after the instruction*/

    block               *blk;
    instruction         *ins;
    instruction         *next;
    bool                change;

    for(;;) {
        change = FALSE;
        blk = HeadBlock;
        while( blk != NULL ) {
            ins = blk->ins.hd.next;
            while( ins->head.opcode != OP_BLOCK ) {
                next = ins->head.next;
                if( IsDeadIns( blk, ins, next ) ) {
                    ins->result = NULL;
                    change = TRUE;
                    if( _IsntIns( ins, SIDE_EFFECT ) ) {
                        FreeIns( ins );
                    } else if( _OpIsCondition( ins->head.opcode ) ) {
                        _SetBlockIndex( ins, NO_JUMP, NO_JUMP );
                    }
                }
                ins = next;
            }
            blk = blk->next_block;
        }
        if( change == FALSE ) break;
        FreeConflicts();
        NullConflicts(EMPTY);
        FindReferences();
        MakeConflicts();
        MakeLiveInfo();
    }
}

extern  void    DeadInstructions() {
/***********************************
    This is called after register allocation. It frees up any
    instructions whose generate table says G_NO (don't generate me).
*/

    block       *blk;

    blk = HeadBlock;
    while( blk != NULL ) {
        FreeJunk( blk );
        blk = blk->next_block;
    }
}
