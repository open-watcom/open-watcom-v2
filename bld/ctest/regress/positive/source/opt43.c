/*
 *  Severely stripped down version of bld/cg/c/dominate.c
 *  that can run the register allocator into a corner. Regalloc
 *  would get stuck in an endless loop generating infinite stream
 *  of moves to temps because it was running out of registers.
 *  Solution was to use R_MOVOP1REG instead of R_MOVOP1TEMP in the
 *  appropriate table.
 *  If this code builds, register allocation worked.
 */

#include "fail.h"

typedef unsigned        char    bool;

typedef unsigned a_bit_set;

typedef struct dom_bit_set {
    a_bit_set   _0;
    a_bit_set   _1;
    a_bit_set   _2;
    a_bit_set   _3;
    a_bit_set   _4;
    a_bit_set   _5;
    a_bit_set   _6;
    a_bit_set   _7;
} dom_bit_set;

typedef struct dominator_info {
    dom_bit_set         id;
    dom_bit_set         post_dominator;
    dom_bit_set         dominator;
} dominator_info;

typedef struct block            *block_pointer;

typedef struct block_edge {
        block_pointer           destination;
        struct block            *source;
        struct block_edge       *next_source;
} block_edge;

typedef struct block {
        struct block            *next_block;
        struct block            *prev_block;
        struct block            *loop_head;
        struct block_edge       *input_edges;
        dominator_info          dom;
        union {
            struct block        *alter_ego;
            struct block        *next;
        } v;
        unsigned                targets;
        struct block_edge       edge[ 1 ];
} block;

static block    *HeadBlock;
static block    *ReturnBlock;

// Stop the cg from being too clever
volatile int    vol;

bool test( int cond )
{
    return( cond == vol );
}

bool CalcDominatorInfo( int cond )
{
    block       *blk;
    block_edge  *edge;
    int         i;
    dom_bit_set predecessors;
    dom_bit_set successors;
    dom_bit_set full_set;
    dom_bit_set old_dominator;
    dom_bit_set temp_bits;
    bool        change;
    bool        have_info;

    have_info =  ((bool)0) ;
    if( test( cond ) ) {
        have_info =  ((bool)1) ;
         { (full_set)._0 = ~0; (full_set)._1 = ~0; (full_set)._2 = ~0; (full_set)._3 = ~0; (full_set)._4 = ~0; (full_set)._5 = ~0; (full_set)._6 = ~0; (full_set)._7 = ~0; } ;
        for( blk = HeadBlock; blk !=  0L ; blk = blk->next_block ) {
             { (blk->dom.dominator)._0 = (full_set)._0; (blk->dom.dominator)._1 = (full_set)._1; (blk->dom.dominator)._2 = (full_set)._2; (blk->dom.dominator)._3 = (full_set)._3; (blk->dom.dominator)._4 = (full_set)._4; (blk->dom.dominator)._5 = (full_set)._5; (blk->dom.dominator)._6 = (full_set)._6; (blk->dom.dominator)._7 = (full_set)._7; } ;
             { (blk->dom.post_dominator)._0 = (full_set)._0; (blk->dom.post_dominator)._1 = (full_set)._1; (blk->dom.post_dominator)._2 = (full_set)._2; (blk->dom.post_dominator)._3 = (full_set)._3; (blk->dom.post_dominator)._4 = (full_set)._4; (blk->dom.post_dominator)._5 = (full_set)._5; (blk->dom.post_dominator)._6 = (full_set)._6; (blk->dom.post_dominator)._7 = (full_set)._7; } ;
        }
         { (HeadBlock->dom.dominator)._0 = (HeadBlock->dom.id)._0; (HeadBlock->dom.dominator)._1 = (HeadBlock->dom.id)._1; (HeadBlock->dom.dominator)._2 = (HeadBlock->dom.id)._2; (HeadBlock->dom.dominator)._3 = (HeadBlock->dom.id)._3; (HeadBlock->dom.dominator)._4 = (HeadBlock->dom.id)._4; (HeadBlock->dom.dominator)._5 = (HeadBlock->dom.id)._5; (HeadBlock->dom.dominator)._6 = (HeadBlock->dom.id)._6; (HeadBlock->dom.dominator)._7 = (HeadBlock->dom.id)._7; } ;
         { (ReturnBlock->dom.post_dominator)._0 = (ReturnBlock->dom.id)._0; (ReturnBlock->dom.post_dominator)._1 = (ReturnBlock->dom.id)._1; (ReturnBlock->dom.post_dominator)._2 = (ReturnBlock->dom.id)._2; (ReturnBlock->dom.post_dominator)._3 = (ReturnBlock->dom.id)._3; (ReturnBlock->dom.post_dominator)._4 = (ReturnBlock->dom.id)._4; (ReturnBlock->dom.post_dominator)._5 = (ReturnBlock->dom.id)._5; (ReturnBlock->dom.post_dominator)._6 = (ReturnBlock->dom.id)._6; (ReturnBlock->dom.post_dominator)._7 = (ReturnBlock->dom.id)._7; } ;
        do {
            change =  ((bool)0) ;
            for( blk = HeadBlock; blk !=  0L ; blk = blk->next_block ) {
                if( blk != HeadBlock ) {
                     { (old_dominator)._0 = (blk->dom.dominator)._0; (old_dominator)._1 = (blk->dom.dominator)._1; (old_dominator)._2 = (blk->dom.dominator)._2; (old_dominator)._3 = (blk->dom.dominator)._3; (old_dominator)._4 = (blk->dom.dominator)._4; (old_dominator)._5 = (blk->dom.dominator)._5; (old_dominator)._6 = (blk->dom.dominator)._6; (old_dominator)._7 = (blk->dom.dominator)._7; } ;
                     { (predecessors)._0 = (full_set)._0; (predecessors)._1 = (full_set)._1; (predecessors)._2 = (full_set)._2; (predecessors)._3 = (full_set)._3; (predecessors)._4 = (full_set)._4; (predecessors)._5 = (full_set)._5; (predecessors)._6 = (full_set)._6; (predecessors)._7 = (full_set)._7; } ;
                    for( edge = blk->input_edges; edge !=  0L ; edge = edge->next_source ) {
                         { (temp_bits)._0 = (full_set)._0; (temp_bits)._1 = (full_set)._1; (temp_bits)._2 = (full_set)._2; (temp_bits)._3 = (full_set)._3; (temp_bits)._4 = (full_set)._4; (temp_bits)._5 = (full_set)._5; (temp_bits)._6 = (full_set)._6; (temp_bits)._7 = (full_set)._7; } ;
                         { (temp_bits)._0 &= ~(edge->source->dom.dominator)._0; (temp_bits)._1 &= ~(edge->source->dom.dominator)._1; (temp_bits)._2 &= ~(edge->source->dom.dominator)._2; (temp_bits)._3 &= ~(edge->source->dom.dominator)._3; (temp_bits)._4 &= ~(edge->source->dom.dominator)._4; (temp_bits)._5 &= ~(edge->source->dom.dominator)._5; (temp_bits)._6 &= ~(edge->source->dom.dominator)._6; (temp_bits)._7 &= ~(edge->source->dom.dominator)._7; } ;
                         { (predecessors)._0 &= ~(temp_bits)._0; (predecessors)._1 &= ~(temp_bits)._1; (predecessors)._2 &= ~(temp_bits)._2; (predecessors)._3 &= ~(temp_bits)._3; (predecessors)._4 &= ~(temp_bits)._4; (predecessors)._5 &= ~(temp_bits)._5; (predecessors)._6 &= ~(temp_bits)._6; (predecessors)._7 &= ~(temp_bits)._7; } ;
                    }
                     { (predecessors)._0 |= (blk->dom.id)._0; (predecessors)._1 |= (blk->dom.id)._1; (predecessors)._2 |= (blk->dom.id)._2; (predecessors)._3 |= (blk->dom.id)._3; (predecessors)._4 |= (blk->dom.id)._4; (predecessors)._5 |= (blk->dom.id)._5; (predecessors)._6 |= (blk->dom.id)._6; (predecessors)._7 |= (blk->dom.id)._7; } ;
                     { (blk->dom.dominator)._0 = (predecessors)._0; (blk->dom.dominator)._1 = (predecessors)._1; (blk->dom.dominator)._2 = (predecessors)._2; (blk->dom.dominator)._3 = (predecessors)._3; (blk->dom.dominator)._4 = (predecessors)._4; (blk->dom.dominator)._5 = (predecessors)._5; (blk->dom.dominator)._6 = (predecessors)._6; (blk->dom.dominator)._7 = (predecessors)._7; } ;
                    if( ! ( ( (blk->dom.dominator)._0 == (old_dominator)._0 ) && ( (blk->dom.dominator)._1 == (old_dominator)._1 ) && ( (blk->dom.dominator)._2 == (old_dominator)._2 ) && ( (blk->dom.dominator)._3 == (old_dominator)._3 ) && ( (blk->dom.dominator)._4 == (old_dominator)._4 ) && ( (blk->dom.dominator)._5 == (old_dominator)._5 ) && ( (blk->dom.dominator)._6 == (old_dominator)._6 ) && ( (blk->dom.dominator)._7 == (old_dominator)._7 ) )  ) change =  ((bool)1) ;
                }
                if( blk != ReturnBlock ) {
                     { (old_dominator)._0 = (blk->dom.post_dominator)._0; (old_dominator)._1 = (blk->dom.post_dominator)._1; (old_dominator)._2 = (blk->dom.post_dominator)._2; (old_dominator)._3 = (blk->dom.post_dominator)._3; (old_dominator)._4 = (blk->dom.post_dominator)._4; (old_dominator)._5 = (blk->dom.post_dominator)._5; (old_dominator)._6 = (blk->dom.post_dominator)._6; (old_dominator)._7 = (blk->dom.post_dominator)._7; } ;
                     { (successors)._0 = (full_set)._0; (successors)._1 = (full_set)._1; (successors)._2 = (full_set)._2; (successors)._3 = (full_set)._3; (successors)._4 = (full_set)._4; (successors)._5 = (full_set)._5; (successors)._6 = (full_set)._6; (successors)._7 = (full_set)._7; } ;
                    for( i = 0, edge = &blk->edge[ 0 ]; i < blk->targets; i++, edge++ ) {
                         { (temp_bits)._0 = (full_set)._0; (temp_bits)._1 = (full_set)._1; (temp_bits)._2 = (full_set)._2; (temp_bits)._3 = (full_set)._3; (temp_bits)._4 = (full_set)._4; (temp_bits)._5 = (full_set)._5; (temp_bits)._6 = (full_set)._6; (temp_bits)._7 = (full_set)._7; } ;
                         { (temp_bits)._0 &= ~(edge->destination->dom.post_dominator)._0; (temp_bits)._1 &= ~(edge->destination->dom.post_dominator)._1; (temp_bits)._2 &= ~(edge->destination->dom.post_dominator)._2; (temp_bits)._3 &= ~(edge->destination->dom.post_dominator)._3; (temp_bits)._4 &= ~(edge->destination->dom.post_dominator)._4; (temp_bits)._5 &= ~(edge->destination->dom.post_dominator)._5; (temp_bits)._6 &= ~(edge->destination->dom.post_dominator)._6; (temp_bits)._7 &= ~(edge->destination->dom.post_dominator)._7; } ;
                         { (successors)._0 &= ~(temp_bits)._0; (successors)._1 &= ~(temp_bits)._1; (successors)._2 &= ~(temp_bits)._2; (successors)._3 &= ~(temp_bits)._3; (successors)._4 &= ~(temp_bits)._4; (successors)._5 &= ~(temp_bits)._5; (successors)._6 &= ~(temp_bits)._6; (successors)._7 &= ~(temp_bits)._7; } ;
                    }
                     { (successors)._0 |= (blk->dom.id)._0; (successors)._1 |= (blk->dom.id)._1; (successors)._2 |= (blk->dom.id)._2; (successors)._3 |= (blk->dom.id)._3; (successors)._4 |= (blk->dom.id)._4; (successors)._5 |= (blk->dom.id)._5; (successors)._6 |= (blk->dom.id)._6; (successors)._7 |= (blk->dom.id)._7; } ;
                     { (blk->dom.post_dominator)._0 = (successors)._0; (blk->dom.post_dominator)._1 = (successors)._1; (blk->dom.post_dominator)._2 = (successors)._2; (blk->dom.post_dominator)._3 = (successors)._3; (blk->dom.post_dominator)._4 = (successors)._4; (blk->dom.post_dominator)._5 = (successors)._5; (blk->dom.post_dominator)._6 = (successors)._6; (blk->dom.post_dominator)._7 = (successors)._7; } ;
                    if( ! ( ( (blk->dom.post_dominator)._0 == (old_dominator)._0 ) && ( (blk->dom.post_dominator)._1 == (old_dominator)._1 ) && ( (blk->dom.post_dominator)._2 == (old_dominator)._2 ) && ( (blk->dom.post_dominator)._3 == (old_dominator)._3 ) && ( (blk->dom.post_dominator)._4 == (old_dominator)._4 ) && ( (blk->dom.post_dominator)._5 == (old_dominator)._5 ) && ( (blk->dom.post_dominator)._6 == (old_dominator)._6 ) && ( (blk->dom.post_dominator)._7 == (old_dominator)._7 ) )  ) change = ((bool)1) ;
                }
            }
        } while( change );
    }
    return( have_info );
}

int main(void)
{
    _PASS;
}
