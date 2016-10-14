/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Register scoreboarding on a basic block.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "score.h"
#include "zerobits.h"
#include "makeins.h"
#include "blips.h"
#include "insdead.h"
#include "optab.h"


extern  bool            UnChangeable(instruction*);
extern  void            UpdateLive(instruction*,instruction*);

void    FreeJunk( block *blk )
/*************************************
    Free instructions which aren't going to be generated.

*/
{
    instruction *ins;
    instruction *next;

    for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = next ) {
        next = ins->head.next;
        if( !DoesSomething( ins ) && !SideEffect( ins )
         && ins->head.opcode < FIRST_OP_WITH_LABEL
         && ins->head.opcode != OP_NOP ) { /*% there for zap info*/
            FreeIns( ins );
        }
    }
}

static bool StupidMove( score *sc, instruction *ins )
/***************************************************/
{
    score_info  info;
    int         dst_index;

    if( ins->head.opcode != OP_MOV ) return( false );
    if( ins->result->n.class != N_REGISTER ) return( false );
    dst_index = ins->result->r.reg_index;
    if( ins->operands[0]->n.class == N_REGISTER ) {
        if( !RegsEqual( sc, ins->operands[0]->r.reg_index, dst_index )  ) {
            return( false );
        }
    } else {
        ScoreInfo( &info, ins->operands[0] );
        if( !ScoreEqual( sc, dst_index, &info ) ) {
            return( false );
        }
    }
    /*
        We've seen a condition already in the
        block, but we've got a move instruction
        that assigns a value to the result register that the
        register already has. Kill the sucker.
    */
    DoNothing( ins );
    return( true );
}


static  bool    RemDeadCode( block *blk )
/****************************************
    This removes any instructions in "blk" which assign to a register which
    dies immediately following that instruction and has no side effects.
    Returns true if any instructions were killed, in which case the
    live information must be updated.
*/
{
    bool        change;
    name        *result;
    instruction *ins;
    instruction *next;

    change = false;
    for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = next ) {
        next = ins->head.next;
        result = ins->result;
        /* if result is a register and it dies after this instruction*/
        if( !_OpIsCall( ins->head.opcode )
         && UnChangeable( ins ) == false
         && SideEffect( ins ) == false
         && result != NULL
         && ScConvert( ins ) == false
         && result->n.class == N_REGISTER
         && !HW_Ovlap( ins->head.next->head.live.regs, result->r.reg ) ) {
            FreeIns( ins );
            change = true;
        }
    }
    return( change );
}

bool    DoScore( block *blk )
/************************************
    Do register scoreboarding on a basic block. Remember which
    registers contain which values as we run through the block and
    then use that information to try to replace memory references
    with register references.
*/
{
    instruction *next;
    score       *scoreboard;
    name        *dst;
    bool        change;
    bool        had_condition;
    score_info  info;
    instruction *ins;
    hw_reg_set  tmp;

    change = false;
    for( ;; ) {
        SCBlip();
        while( RemDeadCode( blk ) ) {
            UpdateLive( blk->ins.hd.next, blk->ins.hd.prev );
            change = true;
        }
        if( RegThrash( blk ) == false ) break;
        change = true;
    }
    scoreboard = blk->cc;
    had_condition = false;
    for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = next ) {
        ScoreSegments( scoreboard );
        /* May all intel designers rot in hell forever and ever, amen*/
        if( _OpIsCondition( ins->head.opcode ) && ins->result == NULL ) {
            if( had_condition ) {
                _MarkBlkAttr( blk, BLK_MULTIPLE_EXITS );
            }
            had_condition = true;
        }
        if( ScoreZero( scoreboard, &ins ) ) {
            change = true;
            UpdateLive( blk->ins.hd.next, blk->ins.hd.prev );
        }
        if( ins->head.opcode == OP_BLOCK ) break;
        /* ScoreZero freed the last instr!*/
        next = ins->head.next;
        dst = ins->result;
        if( UnChangeable( ins ) ) {
            if( dst != NULL ) {
                ScoreInfo( &info, dst );
                ScoreKillInfo( scoreboard, dst, &info, HW_EMPTY );
                RegKill( scoreboard, ins->zap->reg );
            }
        } else if( DoesSomething( ins ) ) {
            if( ins->head.opcode == OP_CALL || ins->head.opcode == OP_CALL_INDIRECT ) {
                MemChanged( scoreboard,
                    ( ins->flags.call_flags & CALL_WRITES_NO_MEMORY ) == 0 );
                RegKill( scoreboard, ins->zap->reg );
                if( dst != NULL && dst->n.class != N_REGISTER ) {
                    ScoreInfo( &info, dst );
                    ScoreKillInfo( scoreboard, dst, &info, HW_EMPTY );
                }
            } else {
                if( FindRegOpnd( scoreboard, ins ) ) {
                    change = true;
                    UpdateLive( blk->ins.hd.next, blk->ins.hd.prev );
                }
                if( ins->head.opcode == OP_MOV && had_condition == false ) {
                    if( ScoreMove( scoreboard, ins ) ) {
                        change = true;
                        UpdateLive( blk->ins.hd.next, blk->ins.hd.prev );
                    }
                    if( next->head.prev == ins ) {
                        RegKill( scoreboard, ins->zap->reg );
                    }
                } else if( ins->head.opcode == OP_LA && had_condition == false ) {
                    if( ScoreLA( scoreboard, ins ) ) {
                        change = true;
                        UpdateLive( blk->ins.hd.next, blk->ins.hd.prev );
                    }
                    if( next->head.prev == ins ) {
                        RegKill( scoreboard, ins->zap->reg );
                    }
                } else if( dst == NULL ) {
                    RegKill( scoreboard, ins->zap->reg );
                } else if( StupidMove( scoreboard, ins ) ) {
                    change = true;
                } else {
                    if( dst->n.class == N_REGISTER ) {
                        tmp = ins->zap->reg;
                        HW_TurnOn( tmp, dst->r.reg );
                        RegKill( scoreboard, tmp );
                    } else {
                        ScoreInfo( &info, dst );
                        ScoreKillInfo( scoreboard, dst, &info, HW_EMPTY );
                        RegKill( scoreboard, ins->zap->reg );
                    }
                    if( had_condition == false ) {
                        ScZeroCheck( scoreboard, ins );
                    }
                }
            }
        } else {
            if( ins->head.opcode == OP_NOP && dst != NULL ) {
                ScoreInfo( &info, dst );
                ScoreKillInfo( scoreboard, dst, &info, HW_EMPTY );
            }
            RegKill( scoreboard, ins->zap->reg );
        }
    }
    return( change );
}


byte    HasZero( score *sc, name *n )
/********************************************
    given a scoreboard "sc", determine if name "n" is equal to
    zero or has any portions which are equal to zero. This is recursive
    since a regisiter like EAX on the 386 has pieces AX, AH, and AL.
*/
{
    byte        bits;
    int         i;
    int         hi;
    int         lo;

    bits = 0;
    if( n->n.class == N_CONSTANT ) {
        if( n->c.const_type == CONS_ABSOLUTE ) {
            if( n->c.lo.int_value == 0 ) {
                bits = LO_HALF | HI_HALF;
            } else if( n->n.size == 2 ) {
                if( ( n->c.lo.int_value & 0x00ff ) == 0 ) {
                    bits |= LO_HALF;
                }
                if( ( n->c.lo.int_value & 0xff00 ) == 0 ) {
                    bits |= HI_HALF;
                }
            } else if( n->n.size == 4 ) {
                if( ( n->c.lo.int_value & 0x0000ffff ) == 0 ) {
                    bits |= LO_HALF;
                }
                if( ( n->c.lo.int_value & 0xffff0000 ) == 0 ) {
                    bits |= HI_HALF;
                }
            }
        }
    } else if( n->n.class == N_REGISTER ) {
        i  = n->r.reg_index;
        if( ScoreLookup( &sc[i], ScZero ) ) {
            bits |= LO_HALF | HI_HALF;
        } else {
            hi = ScoreList[i]->high;
            lo = ScoreList[i]->low;
            if( hi != NO_INDEX && lo != NO_INDEX ) {
                if( _IsZero( HasZero( sc, ScoreList[lo]->reg_name ) ) ) {
                    bits |= LO_HALF;
                }
                if( _IsZero( HasZero( sc, ScoreList[hi]->reg_name ) ) ) {
                    bits |= HI_HALF;
                }
            }
        }
    }
    if( n->n.size == 1 ) {
        bits |= IS_BYTE;
    }
    return( bits );
}
