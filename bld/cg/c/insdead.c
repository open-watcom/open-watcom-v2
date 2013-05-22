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
* Description:  Identify and eliminate dead (useless) instructions.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "data.h"
#include "x87.h"
#include "makeins.h"


extern  void            FreeAName(name*);
extern  block           *TailBlocks(void);
extern  bool            BreakExists(void);
extern  bool            IsVolatile(name*);
extern  void            DoNothing(instruction*);
extern  bool            DoesSomething(instruction*);


static  void    InitVisitedTemps( void )
/***************************************
    Mark all N_TEMP and N_MEMORY names as Not visited. If NO_OPTIMIZATION
    is on, mark them all as visited.
*/
{
    name        *op;
    name        *alias;

    op = Names[ N_TEMP ];
    while( op != NULL ) {
        op->t.temp_flags &= ~VISITED;
        op = op->n.next_name;
    }
    op = Names[ N_TEMP ];
    while( op != NULL ) {
        if( op->v.usage & (USE_ADDRESS|VAR_VOLATILE) ) {
            alias = op;
            do {
                alias->t.temp_flags |= VISITED;
                alias = alias->t.alias;
            } while( alias != op );
        }
        op = op->n.next_name;
    }
    if( _IsModel( NO_OPTIMIZATION ) ) {
        op = Names[ N_TEMP ];
        while( op != NULL ) {
            if( _FrontEndTmp( op ) ) {
                op->t.temp_flags |= VISITED;
            }
            op = op->n.next_name;
        }
    }
    if( BlockByBlock || _IsModel ( NO_OPTIMIZATION ) ) {
        op = Names[ N_TEMP ];
        while( op != NULL ) {
            if( op->v.usage & USE_IN_ANOTHER_BLOCK ) {
                op->t.temp_flags |= VISITED;
            }
            op = op->n.next_name;
        }
    }
}


static  bool            FreeUselessIns( block *tail, bool just_the_loop,
                                        bool in_regalloc )
/***********************************************************************
    Free any instructions which have not been marked
    with the INS_VISITED bit. See below for the setting
    of this bit.
*/
{
    block       *blk;
    instruction *ins;
    instruction *prev;
    bool        change;

    change = FALSE;
    blk = tail;
    while( blk != NULL ) {
        ins = blk->ins.hd.prev;
        if( just_the_loop && !( blk->class & IN_LOOP ) ) {
            while( ins->head.opcode != OP_BLOCK ) {
                ins->ins_flags &= ~INS_VISITED;
                ins = ins->head.prev;
            }
        } else {
            while( ins->head.opcode != OP_BLOCK ) {
                prev = ins->head.prev;
                if( ( ins->ins_flags & INS_VISITED ) == 0 ) {
                    change = TRUE;
                    if( in_regalloc ) {
                        DoNothing( ins );
                    } else {
                        FreeIns( ins );
                    }
                } else {
                    ins->ins_flags &= ~INS_VISITED;
                }
                ins = prev;
            }
        }
        blk = blk->prev_block;
    }
    return( change );
}


static  void            FreeUnVisitedTemps( void )
/*************************************************
    Free any temps which have not been marked VISITED. They are
    useless. See below for setting of this bit.
*/
{
    name        *op;
    name        **owner;

    owner = &Names[ N_TEMP ];
    for( ;; ) {
        op = *owner;
        if( op == NULL ) break;
        if( op->t.temp_flags & VISITED ) {
            op->t.temp_flags &= ~VISITED;
            owner = &op->n.next_name;
        } else {
            *owner = op->n.next_name;
            FreeAName( op );
        }
    }
}


extern  bool            VolatileIns(instruction *ins)
/****************************************************
    Does the instruction access/define a volatile variable?
    This is a utility routine for any module to use.
*/
{
    int         i;

    i = ins->num_operands;
    while( --i >= 0 ) {
        if( IsVolatile( ins->operands[ i ] ) ) return( TRUE );
    }
    if( ins->result != NULL ) {
        if( IsVolatile( ins->result ) ) return( TRUE );
    }
    return( FALSE );
}


extern  bool            SideEffect(instruction* ins)
/***************************************************
    Is an instruction a side effect instruction, such as one
    that changes the 8087 stack or a SUB with a following SBB.
    This is a utility routine for any module to use.
*/
{
    if( ins->head.opcode == OP_PUSH ) return( TRUE );
    if( ins->head.opcode == OP_POP ) return( TRUE );
    if( ins->ins_flags & INS_CC_USED
        && ins->head.opcode != OP_MOV ) return( TRUE );
    if( FPSideEffect( ins ) ) return( TRUE );
    return( VolatileIns( ins ) );
}


static  bool    MarkUseful( name *op )
/*************************************
    Mark an operand as useful. If the operand is an indexed name,
    its base and index become useful as well.
*/
{
    bool        change;
    name        *alias;

    change = FALSE;
    if( op->n.class == N_TEMP ) {
        if( ( op->t.temp_flags & VISITED ) == 0 ) {
            change = TRUE;
            alias = op;
            do {
                alias->t.temp_flags |= VISITED;
                alias = alias->t.alias;
            } while( alias != op );
        }
    } else if( op->n.class == N_INDEXED ) {
        change |= MarkUseful( op->i.index );
        if( op->i.base != NULL ) {
            change |= MarkUseful( op->i.base );
        }
    } else if( op->n.class == N_CONSTANT ) {
        if( op->c.const_type == CONS_TEMP_ADDR ) {
            change |= MarkUseful( op->c.value );
        }
    }
    return( change );
}


static  bool    MarkOpsUseful( instruction *ins )
/************************************************
    We have decided that an instruction is useful, therefore we must
    mark all of its operands as useful
*/
{
    int         i;
    bool        change;

    change = FALSE;
    ins->ins_flags |= INS_VISITED;
    i = ins->num_operands;
    while( --i >= 0 ) {
        change |= MarkUseful( ins->operands[ i ] );
    }
    return( change );
}


static  bool    CheckUseful( instruction *ins )
/**********************************************
    Mark an instruction INS_VISITED if it is useful. A useful
    instruction is one that causes a branch, defines memory, a register
    or a VISITED operand. We return TRUE whenver an instruction gets
    marked INS_VISITED that wasn't before.
*/
{
    name        *res;
    opcode_defs opcode;
    bool        change;

    opcode = ins->head.opcode;
    change = FALSE;
    res = ins->result;
    if( !_OpIsJump( opcode )
     && opcode != OP_PUSH
     && opcode != OP_POP
     && opcode != OP_NOP
     && opcode != OP_CALL_INDIRECT
     && opcode != OP_CALL ) {
        if( SideEffect( ins ) ) {
            if( res != NULL ) {
                change |= MarkUseful( res );
            }
            change |= MarkOpsUseful( ins );
            return( change );
        }
        if( res != NULL ) {
            if( res->n.class == N_MEMORY || res->n.class == N_REGISTER ) {
                change |= MarkOpsUseful( ins );
                return( change );
            }
            if( res->n.class == N_INDEXED ) {
                change |= MarkUseful( res );
                change |= MarkOpsUseful( ins );
                return( change );
            }
            if( res->n.class == N_TEMP ) {
                if( res->t.temp_flags & VISITED ) {
                    change |= MarkOpsUseful( ins );
                    return( change );
                }
            }
        }
    } else {
        if( res != NULL ) {
            change |= MarkUseful( res );
        }
        change |= MarkOpsUseful( ins );
    }
    return( change );
}


static  void            FindUsefulIns( block * tail, bool just_the_loop,
                                        bool in_regalloc )
/***********************************************************************
    This goes around calling CheckUseful until no more instructions
    are found to be useful. Initially, instructions which affect
    branches, or define memory/registers are useful.
    The operands of a useful instruction are marked VISITED.
    On subsequent passes, instructions which define VISITED names
    are also marked useful and their operands are marked VISITED.
    Iterating, we find out every useful instruction in the routine,
    and anything not marked useful gets killed.

*/
{
    bool        change;
    block       *blk;
    instruction *ins;

    if( just_the_loop ) {
        blk = tail;
        while( blk != NULL ) {
            if( !( blk->class & IN_LOOP ) ) {
                ins = blk->ins.hd.prev;
                while( ins->head.opcode != OP_BLOCK ) {
                    MarkOpsUseful( ins );
                    if( ins->result != NULL ) MarkUseful( ins->result );
                    ins = ins->head.prev;
                }
            }
            blk = blk->prev_block;
        }
    }
    do {
        change = FALSE;
        blk = tail;
        while( blk != NULL ) {
            if( !just_the_loop || ( blk->class & IN_LOOP ) ) {
                ins = blk->ins.hd.prev;
                while( ins->head.opcode != OP_BLOCK ) {
                    if( !in_regalloc || DoesSomething( ins ) ) {
                        change |= CheckUseful( ins );
                    }
                    ins = ins->head.prev;
                }
            }
            blk = blk->prev_block;
        }
    } while( change );
}


static  bool    RemoveUselessStuff( bool just_the_loop, bool in_regalloc )
/************************************************************************
    This routine removes useless instructions from the routine. Note
    that if BreakExists(), there are some extra blocks hanging off
    BlockList that we must consider in our analysis. This is for FORTRAN
    when we are generating partial routines. HeadBlock points to what
    we really want to generate, BlockList points the the rest. We discover
    which instruction are useless by marking all useful instructions
    in the program
*/
{
    block       *tail;
    bool        change;

    InitVisitedTemps();
    FindUsefulIns( BlockList, just_the_loop, in_regalloc );
    tail = TailBlocks();
    if( tail != NULL ) {
        FindUsefulIns( tail, just_the_loop, in_regalloc );
    }
    change = FreeUselessIns( BlockList, just_the_loop, in_regalloc );
    if( tail != NULL ) {
        change |= FreeUselessIns( tail, just_the_loop, in_regalloc );
    }
    if( !in_regalloc ) FreeUnVisitedTemps();
    return( change );
}


static bool DoInsDead( bool just_the_loop, bool in_regalloc )
/***********************************************************/
{
    if( BlockByBlock && !BreakExists() ) return( FALSE );
    return( RemoveUselessStuff( just_the_loop, in_regalloc ) );
}


extern  bool    InsDead( void )
/******************************
    Remove any dead or useless instructions in the program we can find.
*/
{
    return( DoInsDead( FALSE, FALSE ) );
}

extern  bool    RegInsDead( void )
/*********************************
    Remove any dead or useless instructions in the program we can find.
*/
{
    return( DoInsDead( FALSE, TRUE ) );
}

extern  bool    LoopInsDead( void )
/*********************************/
{
    return( DoInsDead( TRUE, FALSE ) );
}
