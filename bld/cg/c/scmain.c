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
* Description:  Scoreboard entire routine.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "score.h"
#include "freelist.h"
#include "cfloat.h"
#include "spawn.h"
#include "memout.h"
#include "data.h"
#include "utils.h"
#include "stack.h"
#include "nullprop.h"
#include "generate.h"


extern  void            MakeLiveInfo(void);
extern  mem_out_action  SetMemOut(mem_out_action);
extern  void            UpdateLive(instruction*,instruction*);


extern  void    ScoreInit( void )
/*******************************/
{
    InitFrl( &ScListFrl );
}

extern  void    ScoreFini( void )
/*******************************/
{
}


static  void    ScoreSeed( block *blk, block *son, unsigned index )
/******************************************************************
    If block ends with a single comparison for == or !=, then the
    one of the sons (true index for ==, false index for !=) knows
    that the two operands are in fact equal.
*/
{
    instruction     *cmp;
    unsigned        which;

    if( !_IsBlkAttr( blk, BLK_CONDITIONAL ) )
        return;
    if( _IsBlkAttr( blk, BLK_MULTIPLE_EXITS ) )
        return;
    for( cmp = blk->ins.hd.prev; cmp->head.opcode == OP_NOP; ) {
        cmp = cmp->head.prev;
    }
    switch( cmp->head.opcode ) {
    case OP_CMP_EQUAL:
        which = _TrueIndex( cmp );
        break;
    case OP_CMP_NOT_EQUAL:
        which = _FalseIndex( cmp );
        break;
    default:
        return;
    }
    if( which != index ) return;
    ScoreMakeEqual( son->cc, cmp->operands[0], cmp->operands[1] );
}


static  void    CopyList( score *frm, score *to,
                          list_head **sc_heads, int i )
/*****************************************************/
{
    score_list  *first;
    score_list  *new;
    score       *next;

    if( to[i].list == NULL ) {
        to[i].list = *sc_heads;
        *sc_heads = (list_head *)**sc_heads;
        *to[i].list = NULL;
        for( next = to[i].next_reg; next->list == NULL; next = next->next_reg ) {
            next->list = next->prev_reg->list;
        }
    }
    if( *to[i].list == NULL ) {
        next = to[i].next_reg;
        for( first = *frm[i].list; first != NULL; first = first->next ) {
            new = NewScListEntry();
            Copy( &first->info, &new->info, sizeof( score_info ) );
            new->next = *next->list;
            *next->list = new;
        }
    }
}


static  void    ScoreCopy( score *other_sc, score *sc )
/*****************************************************/
{
    list_head   **sc_heads;
    int         i;

    FreeScoreBoard( sc );
    sc_heads = (list_head **)&sc[ScoreCount];
    i = ScoreCount;
    for( ;; ) {
        --i;
        sc[i].next_reg = &sc[other_sc[i].next_reg->index];
        sc[i].prev_reg = &sc[other_sc[i].prev_reg->index];
        sc[i].generation = other_sc[i].generation;
        sc[i].list = NULL;
        *sc_heads = (list_head *)sc_heads + 1;
        ++sc_heads;
        if( i == 0 ) break;
    }
    *sc_heads = NULL;
    i = ScoreCount;
    sc_heads = (list_head **)&sc[ScoreCount];
    for( ;; ) {
        --i;
        CopyList( other_sc, sc, sc_heads, i );
        if( i == 0 ) break;
    }
}


static  void *ScoreDescendants( block *blk )
/******************************************/
{
    block_num   i;
    block       *son;
    hw_reg_set  regs;

    for( i = blk->targets; i-- > 0; ) {
        son = blk->edge[i].destination.u.blk;
        if( ( son->inputs == 1 ) && !_IsBlkVisited( son ) ) {
            son->cc = ScAlloc( ScoreCount * ( sizeof( score ) + sizeof( list_head ) ) + sizeof( list_head ) );
            ScoreClear( son->cc );
            for(;;) {
                ScoreCopy( blk->cc, son->cc );
                ScoreSeed( blk, son, i );
                if( !DoScore( son ) ) break;
                UpdateLive( son->ins.hd.next, son->ins.hd.prev );
            }
            _MarkBlkVisited( son );
            _MarkBlkMarked( son );
            SafeRecurseCG( (func_sr)ScoreDescendants, son );
            FreeScoreBoard( son->cc );
            ScFree( son->cc );
            son->cc = NULL;
        }
    }
    HW_CAsgn( regs, HW_EMPTY );
    for( i = blk->targets; i-- > 0; ) {
        son = blk->edge[i].destination.u.blk;
        if( _IsBlkMarked( son ) ) {
            HW_TurnOn( regs, son->ins.hd.next->head.live.regs );
            _MarkBlkUnMarked( son );
        }
    }
    HW_TurnOn( blk->ins.hd.live.regs, regs );
    UpdateLive( blk->ins.hd.next, blk->ins.hd.prev );
    return( NULL );
}


static  void    InitZero( void )
/******************************/
/* Must be allocd. Could be modified by ScoreAssign but that's ok */
/* since it will just set offset to 0 */
{
    ScZero = ScAlloc( sizeof( score_info ) );
    ScZero->class     = N_CONSTANT;
    ScZero->offset    = 0;
    ScZero->symbol.p  = NULL;
    ScZero->index_reg = NO_INDEX;
    ScZero->base      = NULL;
}


static  void    ScoreRoutine( void )
/**********************************/
{
    block       *blk;
//    bool        change;

    ScoreCalcList();
    if( ScoreCount != 0 ) {
        InitZero();
        _MarkBlkAllUnVisited();
        MakeLiveInfo();
//        change = false;
        for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
            if( !_IsBlkVisited( blk ) ) {
                blk->cc = ScAlloc( ScoreCount * ( sizeof( score ) + sizeof( list_head ) ) + sizeof( list_head ) );
                ScoreClear( blk->cc );
                for( ;; ) {
                    FreeScoreBoard( blk->cc );
                    ScInitRegs( blk->cc );
                    if( !DoScore( blk ) ) break;
                    UpdateLive( blk->ins.hd.next, blk->ins.hd.prev );
                }
                _MarkBlkVisited( blk );
                ScoreDescendants( blk );
                FreeScoreBoard( blk->cc );
                ScFree( blk->cc );
                blk->cc = NULL;
            }
        }
    }
}


static  void    CleanUp( void )
/*****************************/
{
    block       *blk;

    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        FreeScoreBoard( blk->cc );
        ScFree( blk->cc );
        blk->cc = NULL;
    }
    ScFree( ScoreList );
    ScFree( ScZero );
}


static  void    ConstSizes( void )
/********************************/
{
    name        *cons;

    for( cons = Names[N_CONSTANT]; cons != NULL; cons = cons->n.next_name ) {
        if( cons->c.const_type == CONS_ABSOLUTE ) {
            if ( CFIsU16( cons->c.value ) ) {
                cons->n.size = 2;
            } else {
                cons->n.size = 4;
            }
        }
    }
}


extern  void    Score( void )
/***************************/
{
    mem_out_action      old_memout;
    block               *blk;

    ConstSizes();
    old_memout = SetMemOut( MO_SUICIDE );
    ScZero = NULL;               /* in case of Suicide*/
    ScoreList = NULL;
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        blk->cc = NULL;
    }
    if( Spawn( &ScoreRoutine ) != 0 ) {
        ProcMessage( MSG_SCOREBOARD_DIED );
    }
    CleanUp();
    SetMemOut( old_memout );
}
