/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2018 The Open Watcom Contributors. All Rights Reserved.
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
#include "score.h"

#if 0
static  void    RegDelete( score *scoreboard, int index )
/*******************************************************/
{
    score       *scoreitem;
    int         half;
    list_head   **free_heads;

    scoreitem = &scoreboard[index];
    if( scoreitem->next_reg != scoreitem ) {
        free_heads = (list_head **)&scoreboard[ScoreCount];
        scoreitem->list = *free_heads;
        *free_heads = (list_head *)**free_heads;
        *scoreitem->list = NULL;
    }
    scoreitem->prev_reg->next_reg = scoreitem->next_reg;
    scoreitem->next_reg->prev_reg = scoreitem->prev_reg;
    scoreitem->next_reg = scoreitem;
    scoreitem->prev_reg = scoreitem;
    scoreitem->generation = 0;
    half = ScoreList[index]->low;
    if( half != NO_INDEX ) {
        RegDelete( scoreboard, half );
    }
    half = ScoreList[index]->high;
    if( half != NO_INDEX ) {
        RegDelete( scoreboard, half );
    }
}
#endif

void    RegInsert( score *scoreboard, int dst_idx, int src_idx )
/**************************************************************/
{
    score       *dst;
    score       *src;
    score       *next;
    byte        gen_num;
    list_head   **free_heads;

    src = &scoreboard[src_idx];
    dst = &scoreboard[dst_idx];
    ScoreFreeList( dst );
    free_heads = (list_head **)&scoreboard[ScoreCount];
    *dst->list = (list_head)*free_heads;
    *free_heads = dst->list;
    next = dst;
    do {
        next->list = src->list;
        next = next->next_reg;
    } while( next != dst );
    gen_num = 0;
    next = src;
    do {
        ++gen_num;
        next = next->next_reg;
    } while( next != src );
    dst->generation = gen_num;
    src->prev_reg->next_reg = dst;
    dst->prev_reg->next_reg = src;
    next = src->prev_reg;
    src->prev_reg = dst->prev_reg;
    dst->prev_reg = next;
}


bool    RegsEqual( score *scoreboard, int i1, int i2 )
/****************************************************/
{
    return( scoreboard[i1].list == scoreboard[i2].list );
}


static void     MergeDown( score *scoreboard, int dst_idx, int src_idx )
/**********************************************************************/
{
    int dst_hi;
    int src_hi;
    int dst_lo;
    int src_lo;

    dst_hi = ScoreList[dst_idx]->high;
    src_hi = ScoreList[src_idx]->high;
    if( dst_hi != NO_INDEX && src_hi != NO_INDEX ) {
        if( !RegsEqual( scoreboard, src_hi, dst_hi ) ) {
            RegInsert( scoreboard, dst_hi, src_hi );
            MergeDown( scoreboard, dst_hi, src_hi );
        }
    }
    src_lo = ScoreList[src_idx]->low;
    dst_lo = ScoreList[dst_idx]->low;
    if( dst_lo != NO_INDEX && src_lo != NO_INDEX ) {
        if( !RegsEqual( scoreboard, src_lo, dst_lo ) ) {
            RegInsert( scoreboard, dst_lo, src_lo );
            MergeDown( scoreboard, dst_lo, src_lo );
        }
    }
}


static  void    MergeUp( score *scoreboard, int dst_idx, int src_idx )
/********************************************************************/
{
    int dst_hi;
    int src_hi;
    int dst_lo;
    int src_lo;

    dst_hi = ScoreList[dst_idx]->high_of;
    src_hi = ScoreList[src_idx]->high_of;
    dst_lo = ScoreList[dst_idx]->low_of;
    src_lo = ScoreList[src_idx]->low_of;
    if( dst_hi != NO_INDEX && src_hi != NO_INDEX ) {
        dst_lo = ScoreList[dst_hi]->low;
        src_lo = ScoreList[src_hi]->low;
        if( dst_lo != NO_INDEX && src_lo != NO_INDEX
         && RegsEqual( scoreboard, dst_lo, src_lo )
         && !RegsEqual( scoreboard, dst_hi, src_hi ) ) {
            RegInsert( scoreboard, dst_hi, src_hi );
            MergeUp( scoreboard, dst_hi, src_hi );
        }
    } else if( dst_lo != NO_INDEX && src_lo != NO_INDEX ) {
        dst_hi = ScoreList[dst_lo]->high;
        src_hi = ScoreList[src_lo]->high;
        if( dst_hi != NO_INDEX && src_hi != NO_INDEX
         && RegsEqual( scoreboard, dst_hi, src_hi )
         && !RegsEqual( scoreboard, dst_lo, src_lo ) ) {
            RegInsert( scoreboard, dst_lo, src_lo );
            MergeUp( scoreboard, dst_lo, src_lo );
        }
    }
}


void    RegKill( score *scoreboard, hw_reg_set regs )
/***************************************************/
{
    score_reg   *entry;
    score_list  *curr_list;
    score_list  **owner;
    int         i;
    list_head   **free_heads;

    if( !HW_CEqual( regs, HW_EMPTY ) ) {
        entry = *ScoreList;
        free_heads = (list_head **)&scoreboard[ScoreCount];
        for( i = ScoreCount; i > 0; --i ) {
            if( HW_Ovlap( entry->reg, regs ) ) {
                if( scoreboard->list != NULL ) {
                    if( scoreboard->next_reg == scoreboard ) {
                        ScoreFreeList( scoreboard );
                    } else {
                        scoreboard->list = *free_heads;
                        *free_heads = (list_head *)**free_heads;
                        *scoreboard->list = NULL;
                    }
                }
                scoreboard->prev_reg->next_reg = scoreboard->next_reg;
                scoreboard->next_reg->prev_reg = scoreboard->prev_reg;
                scoreboard->next_reg = scoreboard;
                scoreboard->prev_reg = scoreboard;
                scoreboard->generation = 0;
            } else {
                for( owner = scoreboard->list; (curr_list = *owner) != NULL; ) {
                    if( curr_list->info.index_reg != NO_INDEX
                      && HW_Ovlap( ScoreList[curr_list->info.index_reg]->reg, regs ) ) {
                        *owner = curr_list->next;
                        FreeScListEntry( curr_list );
                    } else {
                        owner = &curr_list->next;
                    }
                }
            }
            ++entry;
            ++scoreboard;
        }
    }
}


void    RegAdd( score *scoreboard, int dst_idx, int src_idx )
/***********************************************************/
/* NB: it is important that dst_idx has just become equal to src_idx*/
/*     NOT vice-versa. Ie: we just did a  MOV R(src_idx) ==> R(dst_idx)*/
/*     or equivalent*/
{
    if( ScoreList[dst_idx]->size != ScoreList[src_idx]->size )
        return;
    if( !ScAddOk( ScoreList[dst_idx]->reg, ScoreList[src_idx]->reg ) )
        return;
    if( RegsEqual( scoreboard, dst_idx, src_idx ) )
        return;
    MergeDown( scoreboard, dst_idx, src_idx );
    MergeUp( scoreboard, dst_idx, src_idx );
    RegInsert( scoreboard, dst_idx, src_idx );
}
