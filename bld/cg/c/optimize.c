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
* Description:  Instruction reordering and dead code elimination.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "cgdefs.h"
#include "zoiks.h"
#include "data.h"
#include "x87.h"
#include "makeins.h"
#include "redefby.h"
#include "regalloc.h"


extern  bool            PropagateMoves(void);
extern  instruction_id  Renumber(void);
extern  bool            SideEffect(instruction*);
extern  conflict_node*  FindConflictNode(name*,block*,instruction*);
extern  void            FreeConflicts(void);
extern  void            FindReferences(void);
extern  void            MakeConflicts(void);
extern  void            MakeLiveInfo(void);
extern  void            FreeJunk(block*);

typedef enum {
    MOVEABLE        = 0x01,
    SWAPABLE        = 0x02,
    SIDE_EFFECT     = 0x04,
    PUSHABLE        = 0x08
} opcode_attr;

static  opcode_attr OpAttrs[LAST_OP - FIRST_OP + 1] = {
/********************************
    define the attributes of any given opcode
*/
    #define PICK(e,i,d1,d2,ot,pnum,attr)  attr,
    #define ONLY_INTERNAL_CGOPS
    #include "cgops.h"
    #undef ONLY_INTERNAL_CGOPS
    #undef PICK
};


#define _IsIns( ins, attr )        ( OpAttrs[ins->head.opcode] & attr )
#define _IsntIns( ins, attr )      ( _IsIns( ins, attr ) == false )


static  bool    ReDefinesOps( instruction *of, instruction *ins ) {
/******************************************************************
    return true if instruction "ins" redefines any of the operands
    or the result of instruction "of"
*/

    int         i;

    for( i = of->num_operands; i-- > 0; ) {
        if( of->operands[i]->n.class == N_REGISTER )
            return( true );
        if( _IsReDefinedBy( ins, of->operands[i] ) ) {
            return( true );
        }
    }
    if( of->result != NULL ) {
        if( of->result->n.class == N_REGISTER )
            return( true );
        if( _IsReDefinedBy( ins, of->result ) ) {
            return( true );
        }
    }
    return( false );
}


static  bool    CanReorder( instruction *try, instruction *after ) {
/*******************************************************************
    return try if "try" could be moved past instruction "after"
*/

    if( ReDefinesOps( try, after ) ) return( false );
    if( ReDefinesOps( after, try ) ) return( false );
    return( true );
}


static  instruction     *CanMoveAfter( instruction *ins ) {
/**********************************************************
    Decide whether we can/should push down instruction "ins". If we can,
    return a pointer to the instruction we could put it after.
*/

    instruction *next;

    if( _IsntIns( ins, PUSHABLE ) ) return( NULL );
    if( ins->operands[0] != ins->result ) return( NULL );
    if( ins->operands[1]->n.class != N_CONSTANT ) return( NULL );
    if( _IsFloating( ins->type_class ) ) return( NULL );
    if( _IsFloating( ins->base_type_class ) ) return( NULL );
    for( next = ins->head.next; next != NULL; next = next->head.next ) {
        if( _IsntIns( next, SWAPABLE ) ) break;
        if( CanReorder( ins, next ) == false ) break;
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

    /* move backwards through instructions*/
    for( ins = blk->ins.hd.prev; ins->head.opcode != OP_BLOCK; ins = prev ) {
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
    }
}


extern  bool    SameThing( name *x, name *y )
/********************************************
    returns true if "x" and "y" are the same thing. IE: N_MEMORY
    names which are associated with the same front end symbol or
    N_TEMP names which are aliases of each other. Two temporaries
    which are "stackable" are considered to be the same thing
    since they may (probably will) both end up on the 8087 stack.
*/
{
    if( x == y ) return( true );
    if( x == NULL || y == NULL ) return( false );
    if( FPIsStack( x ) && FPIsStack( y ) ) return( true );
    if( x->n.class == N_MEMORY
     && y->n.class == N_MEMORY
     && x->v.symbol == y->v.symbol ) return( true );
    if( x->n.class == N_TEMP && y->n.class == N_TEMP ) {
        if( x->t.v.id != y->t.v.id ) return( false );
        if( !( x->t.temp_flags & ALIAS )
         && !( y->t.temp_flags & ALIAS ) ) return( false );
        return( true );
    }
    return( false );
}


extern  void    DeadTemps( void )
/********************************
    Using the information contained in each N_TEMP, delete any instruction
    whose result is a temporary that isnt used again. This is only a very
    crude hack of dead code, since it only gets temporaries which are local
    to a basic block and are only defined. Its designed to get rid of
    dummy instructions generated to preserve the value of an assignment
    operator. For example, *x = *y generates:
        MOV     [y] => [x]
        MOV     [x] => temp     <- this usually doesn't get used later
*/
{
    block       *blk;
    instruction *ins;
    instruction *next;

    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = next ) {
            next = ins->head.next;
            if( SideEffect( ins ) == false
             && _IsntIns( ins, SIDE_EFFECT )
             && ins->result != NULL
             && ins->result->n.class == N_TEMP
             && ( ins->result->v.usage & (USE_ADDRESS|HAS_MEMORY|USE_WITHIN_BLOCK|USE_IN_ANOTHER_BLOCK) ) == 0 ) {
                FreeIns( ins );
            }
        }
    }
}


static  bool    IsDeadIns( block *blk, instruction *ins, instruction *next )
/*****************************************************************************
    returns true if an instruction is assigning to a name which is not
    live immediately following instruction "ins".
*/
{
    conflict_node       *conf;
    name                *op;

    op = ins->result;
    if( op == NULL ) return( false );
//  if( op->n.class != N_TEMP ) return( false );
    if( op->v.usage & USE_ADDRESS ) return( false );
    if( SideEffect( ins ) ) return( false );
    conf = FindConflictNode( op, blk, ins );
    if( conf == NULL ) return( false );
    if( _LBitEmpty( conf->id.within_block )
     && _GBitEmpty( conf->id.out_of_block ) ) {
         return( false );
     }
    if( _LBitOverlap( conf->id.within_block, next->head.live.within_block ) ) {
        return( false );
    }
    if( _GBitOverlap( conf->id.out_of_block, next->head.live.out_of_block ) ) {
        return( false );
    }
    return( true );
}


extern  void    AxeDeadCode( void )
/******************************
    This is like DeadTemps, but it uses the live information in order
    to discover whether an instruction is assigning to a variable that
    dies immediately following. If we change anything here, we redo
    the live information (that's not very expensive).

    delete instructions which assign to temporaries which will not be
    used again before they are reassigned (ie: are not live after the instruction */
{
    block               *blk;
    instruction         *ins;
    instruction         *next;
    instruction         *kill;
    bool                change;

/* Reuse field, it's useless for killed instruction */
#define _INS_KILL_LINK( ins )  ( ins )->u2.cse_link

    for(;;) {
        kill = NULL;
        change = false;
        for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
            for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = next ) {
                next = ins->head.next;
                if( IsDeadIns( blk, ins, next ) ) {
                    ins->result = NULL;
                    change = true;
                    if( _IsntIns( ins, SIDE_EFFECT ) ) {
                       /*
                        * 2005-05-18 RomanT
                        * Do not free instruction immediately, or conflict
                        * edges will point to nowhere. Add them to kill list.
                        */
                        _INS_KILL_LINK( ins ) = kill;
                        kill = ins;
                    } else if( _OpIsCondition( ins->head.opcode ) ) {
                        _SetBlockIndex( ins, NO_JUMP, NO_JUMP );
                    }
                }
            }
        }
        if( change == false ) break;
        FreeConflicts();
        /* Now it's safe to free instructions without problems with edges */
        while ( kill ) {
            next = _INS_KILL_LINK( kill );
            FreeIns( kill );
            kill = next;
        }
        NullConflicts(EMPTY);
        FindReferences();
        MakeConflicts();
        MakeLiveInfo();
    }
}


extern  void    DeadInstructions( void )
/***********************************
    This is called after register allocation. It frees up any
    instructions whose generate table says G_NO (don't generate me).
*/
{
    block       *blk;

    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        FreeJunk( blk );
    }
}


extern  void    PushPostOps( void )
/**********************************
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

    Perform instruction optimizations */
{
    block       *blk;

    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        PushInsForward( blk );
    }
    PropagateMoves();
    Renumber();
}
