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
#include "score.h"

extern  void            FreeScListEntry(score_list*);
extern  void            ScoreFreeList(score*);
extern  bool            ScAddOk(hw_reg_set,hw_reg_set);

extern  int             ScoreCount;
extern  score_reg       **ScoreList;

static  void    RegDelete( score *sc, int index ) {
/*************************************************/

    score       *scoreboard;
    int         half;
    list_head   **free_heads;

    scoreboard = &sc[ index ];
    if( scoreboard->next_reg != scoreboard ) {
        free_heads = (list_head **)&sc[ ScoreCount ];
        scoreboard->list = *free_heads;
        *free_heads = (list_head *)**free_heads;
        *scoreboard->list = NULL;
    }
    scoreboard->prev_reg->next_reg = scoreboard->next_reg;
    scoreboard->next_reg->prev_reg = scoreboard->prev_reg;
    scoreboard->next_reg = scoreboard;
    scoreboard->prev_reg = scoreboard;
    scoreboard->generation = 0;
    half = ScoreList[  index  ]->low;
    if( half != NO_INDEX ) {
        RegDelete( sc, half );
    }
    half = ScoreList[  index  ]->high;
    if( half != NO_INDEX ) {
        RegDelete( sc, half );
    }
}


extern  bool    RegsEqual( score *sc, int i1, int i2 ) {
/******************************************************/

    return( sc[ i1 ].list == sc[ i2 ].list );
}

extern  void    RegAdd( score *sc, int dst_idx, int src_idx ) {
/*************************************************************/

/* NB: it is important that dst_idx has just become equal to src_idx*/
/*     NOT vice-versa. Ie: we just did a  MOV R(src_idx) ==> R(dst_idx)*/
/*     or equivalent*/

    if( ScoreList[ dst_idx ]->size != ScoreList[ src_idx ]->size ) return;
    if( !ScAddOk( ScoreList[ dst_idx ]->reg, ScoreList[ src_idx ]->reg ) )
        return;
    if( RegsEqual( sc, dst_idx, src_idx ) ) return;
    MergeDown( sc, dst_idx, src_idx );
    MergeUp( sc, dst_idx, src_idx );
    RegInsert( sc, dst_idx, src_idx );
}


static  void    MergeDown( score *sc, int dst_idx, int src_idx ) {
/****************************************************************/

    int dst_hi;
    int src_hi;
    int dst_lo;
    int src_lo;

    dst_hi = ScoreList[ dst_idx ]->high;
    src_hi = ScoreList[ src_idx ]->high;
    if( dst_hi != NO_INDEX && src_hi != NO_INDEX ) {
        if( !RegsEqual( sc, src_hi, dst_hi ) ) {
            RegInsert( sc, dst_hi, src_hi );
            MergeDown( sc, dst_hi, src_hi );
        }
    }
    src_lo = ScoreList[ src_idx ]->low;
    dst_lo = ScoreList[ dst_idx ]->low;
    if( dst_lo != NO_INDEX && src_lo != NO_INDEX ) {
        if( !RegsEqual( sc, src_lo, dst_lo ) ) {
            RegInsert( sc, dst_lo, src_lo );
            MergeDown( sc, dst_lo, src_lo );
        }
    }
}


static  void    MergeUp( score *sc, int dst_idx, int src_idx ) {
/**************************************************************/

    int dst_hi;
    int src_hi;
    int dst_lo;
    int src_lo;

    dst_hi = ScoreList[ dst_idx ]->high_of;
    src_hi = ScoreList[ src_idx ]->high_of;
    dst_lo = ScoreList[ dst_idx ]->low_of;
    src_lo = ScoreList[ src_idx ]->low_of;
    if( dst_hi != NO_INDEX && src_hi != NO_INDEX ) {
        dst_lo = ScoreList[ dst_hi ]->low;
        src_lo = ScoreList[ src_hi ]->low;
        if( dst_lo != NO_INDEX && src_lo != NO_INDEX
         && RegsEqual( sc, dst_lo, src_lo )
         && !RegsEqual( sc, dst_hi, src_hi ) ) {
            RegInsert( sc, dst_hi, src_hi );
            MergeUp( sc, dst_hi, src_hi );
        }
    } else if( dst_lo != NO_INDEX && src_lo != NO_INDEX ) {
        dst_hi = ScoreList[ dst_lo ]->high;
        src_hi = ScoreList[ src_lo ]->high;
        if( dst_hi != NO_INDEX && src_hi != NO_INDEX
         && RegsEqual( sc, dst_hi, src_hi )
         && !RegsEqual( sc, dst_lo, src_lo ) ) {
            RegInsert( sc, dst_lo, src_lo );
            MergeUp( sc, dst_lo, src_lo );
        }
    }
}


extern  void    RegInsert( score *sc, int dst_idx, int src_idx ) {
/****************************************************************/

    score       *dst;
    score       *src;
    score       *next;
    byte        gen_num;
    list_head   **free_heads;

    src = &sc[ src_idx ];
    dst = &sc[ dst_idx ];
    ScoreFreeList( dst );
    free_heads = (list_head **)&sc[ ScoreCount ];
    *dst->list = (list_head)*free_heads;
    *free_heads = dst->list;
    next = dst;
    for(;;) {
        next->list = src->list;
        next = next->next_reg;
        if( next == dst ) break;
    }
    next = src;
    gen_num = 0;
    for(;;) {
        next = next->next_reg;
        ++gen_num;
        if( next == src ) break;
    }
    dst->generation = gen_num;
    src->prev_reg->next_reg = dst;
    dst->prev_reg->next_reg = src;
    next = src->prev_reg;
    src->prev_reg = dst->prev_reg;
    dst->prev_reg = next;
}


extern  void    RegKill( score *scoreboard, hw_reg_set regs ) {
/*************************************************************/

    score_reg   *entry;
    score_list  *curr_list;
    score_list  **owner;
    int         i;
    list_head   **free_heads;

    if( !HW_CEqual( regs, HW_EMPTY ) ) {
        entry = *ScoreList;
        i = ScoreCount;
        free_heads = (list_head **)&scoreboard[ ScoreCount ];
        while( -- i >= 0 ) {
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
                owner = scoreboard->list;
                for(;;) {
                    curr_list = *owner;
                    if( curr_list == NULL ) break;
                    if( curr_list->info.index_reg != NO_INDEX
                     && HW_Ovlap( ScoreList[ curr_list->info.index_reg ]->reg,
                                  regs ) ) {
                        *owner = curr_list->next;
                        FreeScListEntry( curr_list );
                    } else {
                        owner = &curr_list->next;
                    }
                }
            }
            ++ entry;
            ++ scoreboard;
        }
    }
}
