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
* Description:  Strength reduction of multiplication operations.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "data.h"
#include "makeins.h"

extern  name            *AllocIntConst(int);
extern  name            *AllocTemp(type_class_def);
extern  void            PrefixIns(instruction*,instruction*);
extern  int             SubCost(void);
extern  int             AddCost(void);
extern  int             MulCost(unsigned_32);
extern  int             ShiftCost( int );
extern  uint_32         CountBits( uint_32 );

#define MAXOPS  20

typedef enum {
    DO_SHL,
    DO_SUB,
    DO_ADD,
    DO_XFR
} op_type;

typedef struct op {
    op_type     op;
    char        cnt;
} op;

static  op      Ops[MAXOPS];


static  int     Factor( unsigned_32 num, int *cost )
/**************************************************/
{
    int         shlcnt;
    int         i;
    int         j;
    unsigned    num_oprs;
    unsigned_32 test;
    unsigned_32 pow2;

    *cost = 0;
    i = MAXOPS;
    if( num == 0 ) return( i );
    if( num == 0xFFFFFFFF ) return( i );

    test = num >> 1;
    do {
        pow2 = 1;
        shlcnt = 0;
        while( test ) {
            pow2 <<= 1;
            shlcnt++;
            test >>= 1;
        }
        while( pow2 != 1 ) {
            if( ( num % ( pow2-1) ) == 0 ) {
                test = pow2-1;
            } else if( num % (pow2+1) == 0 ) {
                test = pow2+1;
            } else {
                test = 0;
            }
            if( test ) {
                if( CountBits( num / test )+2 <= CountBits( num ) ) {

                    num /= test;

                    if( --i < 0 ) return( MAXOPS );
                    Ops[i].op = test == pow2-1 ? DO_SUB : DO_ADD;

                    if( --i < 0 ) return( MAXOPS );
                    Ops[i].op = DO_SHL;
                    Ops[i].cnt= shlcnt;

                    if( --i < 0 ) return( MAXOPS );
                    Ops[i].op = DO_XFR;
                    break;
                }
            }
            pow2 >>= 1;
            shlcnt--;
        }
    } while( pow2 != 1 );

    shlcnt = 0;
    for( ;; ) {
        while( !( num & 1 ) ) {
            ++shlcnt;
            num >>= 1;
        }
        if( shlcnt != 0 ) {
            if( --i < 0 ) return( MAXOPS );
            Ops[i].op = DO_SHL;
            Ops[i].cnt= shlcnt;
        }
        if( num == 1 ) break;
        if( --i < 0 ) return( MAXOPS );
        shlcnt = 0;
        if( ( num & 3 ) == 1 ) {
            Ops[i].op = DO_ADD;
            num >>= 1;
            shlcnt = 1;
        } else {
            Ops[i].op = DO_SUB;
            ++num;
        }
    }

    /* Now estimate the cost of the alternate instruction sequence. */
    for( j = i; j < MAXOPS; ++j ) {
        switch( Ops[j].op ) {
        case DO_SHL:
            *cost += ShiftCost( Ops[j].cnt );
            break;
        case DO_ADD:
            *cost += AddCost();
            break;
        case DO_SUB:
            *cost += SubCost();
            break;
        default:
            *cost += 1;
            break;
        }
    }

    /*
        Bump up cost estimate to allow for the fact that we're going to have
        [a lot] more instructions with the shift and add method.
    */
    num_oprs = MAXOPS - i + 1;
    *cost += num_oprs;
    return( i );
}

static  instruction     *CheckMul( instruction *ins )
/***************************************************/
{
    signed_32           rhs;
    int                 i;
    bool                neg;
    instruction         *new_ins;
    name                *orig;
    name                *temp;
    type_class_def      class;
    int                 cost;

    rhs = ins->operands[1]->c.int_value;
    neg = FALSE;
    class = ins->type_class;
    if( class == SW && rhs < 0 ) {
        rhs = -rhs;
        neg = TRUE;
    }
    i = Factor( rhs, &cost );
    if( MulCost( rhs ) <= cost ) return( ins );
    if( i == MAXOPS ) return( ins );
    orig = AllocTemp( class );
    new_ins = MakeMove( ins->operands[0], orig, class );
    PrefixIns( ins, new_ins );
    temp = AllocTemp( class );
    new_ins = MakeMove( orig, temp, class );
    PrefixIns( ins, new_ins );
    for( ; i < MAXOPS; ++i ) {
        switch( Ops[i].op ) {
        case DO_XFR:
            new_ins = MakeUnary( OP_MOV, temp, orig, class );
            break;
        case DO_ADD:
            new_ins = MakeBinary( OP_ADD, temp, orig, temp, class );
            break;
        case DO_SUB:
            new_ins = MakeBinary( OP_SUB, temp, orig, temp, class );
            break;
        case DO_SHL:
            new_ins = MakeBinary( OP_LSHIFT, temp,
                                  AllocIntConst( Ops[i].cnt ), temp, class );
            break;
        }
        PrefixIns( ins, new_ins );
    }
    if( neg ) {
        new_ins = MakeUnary( OP_NEGATE, temp, ins->result, class );
    } else {
        new_ins = MakeMove( temp, ins->result, class );
    }
    PrefixIns( ins, new_ins );
    FreeIns( ins );
    return( new_ins );
}


extern  void    MulToShiftAdd( void )
/***********************************/
{
    block       *blk;
    instruction *ins;
    instruction *next;
    name        *op;

    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        for( ins = blk->ins.hd.next;
            ins->head.opcode != OP_BLOCK; ins = ins->head.next ) {
            if( ins->head.opcode != OP_MUL ) continue;
            if( ins->operands[0]->n.class == N_CONSTANT ) {
                op = ins->operands[0];
                if( op->c.const_type == CONS_ABSOLUTE ) {
                    ins->operands[0] = ins->operands[1];
                    ins->operands[1] = op;
                }
            }
            op = ins->operands[1];
            switch( op->n.class ) {
            case N_TEMP:
                if( !(op->t.temp_flags & CONST_TEMP) ) continue;
                ins->operands[1] = op->v.symbol;
                /* fall through */
            case N_CONSTANT:
                if( ins->type_class != WD && ins->type_class != SW ) continue;
                if( op->c.const_type != CONS_ABSOLUTE ) continue;
                next = CheckMul( ins );
                if( next == ins ) ins->operands[1] = op; /*nothing happened*/
                ins = next;
            }
        }
    }
}
