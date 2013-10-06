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
* Description:  Instruction block/range dump.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "dump.h"
#include "data.h"

extern  void            DumpNL(void);
extern  void            DumpPtr(pointer);
extern  void            DumpIns(instruction*);
extern  void            DumpLineNum(instruction*);
extern  void            DumpInt(int);
extern  void            DumpInsList(block*);
extern  void            DumpInstrsOnly(block*);
extern  char            *AskName(pointer,cg_class);
extern  void            DumpSymList(name*);
extern  void            Dump8h(unsigned_32);


static void DumpBlkFlags( block *blk )
/************************************/
{
    if( blk->class & RETURN ) {
        DumpLiteral( "Ret " );
    } else {
        DumpLiteral( "----" );
    }
    if( blk->class & JUMP ) {
        DumpLiteral( "Jmp " );
    } else {
        DumpLiteral( "----" );
    }
    if( blk->class & CONDITIONAL ) {
        DumpLiteral( "Cond " );
    } else {
        DumpLiteral( "-----" );
    }
    if( blk->class & SELECT ) {
        DumpLiteral( "Sel " );
    } else {
        DumpLiteral( "----" );
    }
    if( blk->class & ITERATIONS_KNOWN ) {
        DumpLiteral( "Itr " );
    } else {
        DumpLiteral( "----" );
    }
    if( blk->class & BIG_LABEL ) {
        DumpLiteral( "LBL " );
    } else {
        DumpLiteral( "----" );
    }
    if( blk->class & CALL_LABEL ) {
        DumpLiteral( "LCall " );
    } else {
        DumpLiteral( "------" );
    }
    if( blk->class & LABEL_RETURN ) {
        DumpLiteral( "LRet " );
    } else {
        DumpLiteral( "-----" );
    }
    if( blk->class & RETURNED_TO ) {
        DumpLiteral( "RetTo " );
    } else {
        DumpLiteral( "------" );
    }
    if( blk->class & LOOP_HEADER ) {
        DumpLiteral( "LupHd " );
    } else {
        DumpLiteral( "------" );
    }
    if( blk->class & IN_LOOP ) {
        DumpLiteral( "InLup " );
    } else {
        DumpLiteral( "------" );
    }
    if( blk->class & LOOP_EXIT ) {
        DumpLiteral( "LupEx " );
    } else {
        DumpLiteral( "------" );
    }
    if( blk->class & BLOCK_VISITED ) {
        DumpLiteral( "Vst " );
    } else {
        DumpLiteral( "----" );
    }
    if( blk->class & BLOCK_MARKED ) {
        DumpLiteral( "Mrk " );
    } else {
        DumpLiteral( "----" );
    }
    if( blk->class & UNKNOWN_DESTINATION ) {
        DumpLiteral( "Dst? " );
    } else {
        DumpLiteral( "-----" );
    }
    if( blk->class & MULTIPLE_EXITS ) {
        DumpLiteral( "2Exits " );
    } else {
        DumpLiteral( "-------" );
    }
    DumpNL();
}


extern  void    DumpRefs( name *op )
/**********************************/
{
    block       *blk;
    instruction *ins;
    int         dsize;
    int         i;

    blk = HeadBlock;
    while( blk != NULL ) {
        ins = blk->ins.hd.next;
        while( ins->head.opcode != OP_BLOCK ) {
            dsize = 0;
            i = ins->num_operands;
            while( --i >= 0 ) {
                if( ins->operands[ i ] == op ) {
                    DumpLiteral( "OP(" );
                    DumpInt( i + 1 );
                    DumpLiteral( ") " );
                    dsize += 6;
                } else if( ins->operands[ i ]->n.class == N_INDEXED ) {
                    if( ins->operands[ i ]->i.index == op ) {
                        DumpLiteral( "IX(" );
                        DumpInt( i + 1 );
                        DumpLiteral( ") " );
                        dsize += 6;
                    } else if( ins->operands[ i ]->i.base == op ) {
                        if( ins->operands[ i ]->i.index_flags & X_FAKE_BASE ) {
                            DumpLiteral( "FB(" );
                        } else {
                            DumpLiteral( "BA(" );
                        }
                        DumpInt( i + 1 );
                        DumpLiteral( ") " );
                        dsize += 6;
                    }
                }
            }
            if( ins->result == op ) {
                DumpLiteral( "RES " );
                dsize += 4;
            } else if( ins->result != NULL ) {
                if( ins->result->i.index == op ) {
                    DumpLiteral( "IX(" );
                    DumpInt( i + 1 );
                    DumpLiteral( ") " );
                    dsize += 4;
                } else if( ins->result->i.base == op ) {
                    if( ins->result->i.index_flags & X_FAKE_BASE ) {
                        DumpLiteral( "FB(" );
                    } else {
                        DumpLiteral( "BA(" );
                    }
                    DumpInt( i + 1 );
                    DumpLiteral( ") " );
                    dsize += 4;
                }
            }
            if( dsize ) {
                while( ++dsize < 23 ) {
                    DumpLiteral( " " );
                }
                DumpIns( ins );
            }
            ins = ins->head.next;
        }
        blk = blk->next_block;
    }
}


static  void    DumpBlkLabel( block *b )
/**************************************/
{
    if( b->label != NULL ) {
        DumpLiteral( " L" );
        DumpPtr( b->label );
        DumpLiteral( " " );
        DumpXString( AskName( AskForLblSym( b->label ), CG_FE ) );
        if( b->edge[ 0 ].flags & BLOCK_LABEL_DIES ) {
            DumpLiteral( " label dies" );
        }
    }
}


static  bool    FindBlock( block *b )
/***********************************/
{
    block       *blk;

    blk = HeadBlock;
    for( ;; ) {
        if( blk == NULL ) return( FALSE );
        if( blk == b ) return( TRUE );
        blk = blk->next_block;
    }
}

extern  void    DumpBlkId( block *b )
/***********************************/
{
    DumpLiteral( "Block " );
    DumpInt( b->id );
    DumpLiteral( "(" );
    DumpInt( b->gen_id );
    DumpLiteral( ")" );
}

static  void    DumpInputs( block *b )
/************************************/
{
    int         i;
    block_edge  *edge;

    DumpPtr( b->input_edges );
    DumpLiteral( "           Origins: " );
    if( b->input_edges != NULL ) {
        i = 0;
        edge = b->input_edges;
        for(;;) {
            if( FindBlock( edge->source ) ) {
                DumpBlkId( edge->source );
            } else {
                DumpLiteral( "?" );
            }
            edge = edge->next_source;
            if( edge == NULL ) break;
            DumpLiteral( ", " );
            ++ i;
            if( ( i & 7 ) == 0 ) {
                DumpNL();
                DumpLiteral( "         " );
            }
        }
    }
    DumpNL();
}


extern  void    DumpLBit( local_bit_set *bit )
/********************************************/
{
    _LBitIter( Dump8h, (*bit) );
}


extern  void    DumpGBit( global_bit_set *bit )
/*********************************************/
{
    _GBitIter( Dump8h, (*bit) );
}


static  void    DumpDataFlo( block *b )
/*************************************/
{
    if( b->dataflow != NULL ) {
        DumpLiteral( "IN   " );
        DumpGBit( &b->dataflow->in );
        DumpLiteral( " OUT  " );
        DumpGBit( &b->dataflow->out );
        DumpNL();
        DumpLiteral( "DEF  " );
        DumpGBit( &b->dataflow->def );
        DumpLiteral( " USE  " );
        DumpGBit( &b->dataflow->use );
        DumpNL();
        DumpLiteral( "LOAD " );
        DumpGBit( &b->dataflow->need_load );
        DumpLiteral( " STOR " );
        DumpGBit( &b->dataflow->need_store );
        DumpNL();
    }
}


static  void    DumpGotos( block *b, bool all )
/*********************************************/
{
    int         i;

    DumpPtr( &b->edge[ 0 ] );
    DumpLiteral( "           Destinations: " );
    if( b->targets > 0 ) {
        if( ( b->edge[ 0 ].flags & DEST_LABEL_DIES ) && all ) {
            DumpLiteral( "(kills) " );
        }
        if( b->edge[ 0 ].flags & DEST_IS_BLOCK ) {
            DumpBlkId( b->edge[ 0 ].destination.u.blk );
        } else {
            DumpLiteral( "L" );
            DumpPtr( b->edge[ 0 ].destination.u.lbl );
        }
        i = 0;
        while( ++i < b->targets ) {
            DumpLiteral( ", " );
            if( ( b->edge[ i ].flags & DEST_LABEL_DIES ) && all ) {
                DumpLiteral( "(kills) " );
            }
            if( b->edge[ i ].flags & DEST_IS_BLOCK ) {
                DumpBlkId( b->edge[ i ].destination.u.blk );
            } else {
                DumpLiteral( "L" );
                DumpPtr( b->edge[ i ].destination.u.lbl );
            }
        }
    }
    DumpNL();
}


extern  void    DumpLoops( void )
/*******************************/
{
    block       *blk;

    blk = HeadBlock;
    while( blk != NULL ) {
        DumpBlkId( blk );
        if( blk->class & LOOP_HEADER ) {
            DumpLiteral( " Is loop header." );
        } else {
            DumpLiteral( "                " );
        }
        if( blk->loop_head != NULL ) {
            DumpLiteral( " Has loop header " );
            DumpBlkId( blk->loop_head );
        }
        DumpNL();
        blk = blk->next_block;
    }
}


extern  void    DumpFlowGraph( block *blk )
/*****************************************/
{
    interval_def        *head;
    interval_def        *curr;
    int                 i;

    DumpLiteral( "Interval graph" );
    DumpNL();
    head = blk->u.interval;
    while( head->parent != NULL ) {
        head = head->parent;
    }
    curr = head;
    for(;;) {
        if( curr->level == 0 ) {
            DumpBlkId( curr->first_block );
            DumpLiteral( " loop depth " );
            DumpInt( curr->first_block->depth );
            DumpNL();
            for(;;) {
                if( curr->next_sub_int != NULL ) break;
                if( curr == head ) break;
                curr = curr->parent;
            }
            i = head->level;
            while( i > curr->level ) {
                DumpLiteral( "|     " );
                -- i;
            }
            if( i > 0 ) {
                for(;;) {
                    DumpLiteral( "End   " );
                    if( -- i == 0 ) break;
                }
                DumpNL();
                i = head->level;
                while( i > curr->level ) {
                    DumpLiteral( "|     " );
                    -- i;
                }
            }
            if( curr == head ) break;
            curr = curr->next_sub_int;
        } else {
            DumpLiteral( "Start " );
            curr = curr->sub_int;
        }
    }
    DumpNL();
}


extern  void    DumpSymTab( void )
/********************************/
{
    int         class;

    class = N_CONSTANT;
    for( ;; ) {
        if( Names[ class ] != NULL ) {
            DumpNL();
            switch( class ) {
            case N_CONSTANT:
                DumpLiteral( "Constants" );
                break;
            case N_MEMORY:
                DumpLiteral( "Memory" );
                break;
            case N_TEMP:
                DumpLiteral( "Temporaries" );
                break;
            case N_REGISTER:
                DumpLiteral( "Registers" );
                break;
            case N_INDEXED:
                DumpLiteral( "Indexed" );
                break;
            }
            DumpSymList( Names[ class ] );
            DumpNL();
        }
        if( ++ class > N_INDEXED ) break;
    }
}

extern  void    DumpEdge( block_num i, block_edge *edge )
/*******************************************************/
{
    DumpLiteral( "\n\tEdge " );
    DumpInt( i );
    DumpLiteral( "(" );
    DumpPtr( edge );
    DumpLiteral( ")\n\t\tDestination:\t" );
    DumpPtr( edge->destination );
    DumpLiteral( "\n\t\tSource:\t" );
    DumpPtr( edge->source );
    DumpLiteral( "\n\t\tNext Source:\t" );
    DumpPtr( edge->next_source );
    DumpLiteral( "\n\t\tFlags:\t" );
    DumpInt( edge->flags );
    DumpNL();
}

extern  void    DumpEdges( block *b )
/***********************************/
{
    block_edge  *edge;
    block_num   i;

    DumpBlkId( b );
    for( i = 0; i < b->targets; i++ ) {
        edge = &b->edge[ i ];
        DumpEdge( i, edge );
    }
}

extern  void    DumpInputEdges( block *b )
/****************************************/
{
    block_edge  *edge;
    block_num   i;

    i = 0;
    for( edge = b->input_edges; edge != NULL; edge = edge->next_source ) {
        DumpEdge( i++, edge );
    }
}

static  void    DumpBlkI( void )
/******************************/
{
    block       *blk;

    blk = HeadBlock;
    while( blk != NULL ) {
        DumpBlkFlags( blk );
        DumpLineNum( (instruction *)&blk->ins );
        DumpPtr( blk );
        DumpLiteral( " " );
        DumpBlkId( blk );
        if( blk->label != NULL ) {
            DumpLiteral( " L:" );
            DumpPtr( blk->label );
        }
        DumpNL();
        if( !_DBitEmpty( blk->dom.id ) ) {
            DumpLiteral( "DOM: id  = " );
            _DBitIter( Dump8h, blk->dom.id );
            DumpNL();
            DumpLiteral( "DOM: dom = " );
            _DBitIter( Dump8h, blk->dom.dominator );
            DumpNL();
            DumpLiteral( "DOM: post= " );
            _DBitIter( Dump8h, blk->dom.post_dominator );
            DumpNL();
        }
        DumpInputs( blk );
        DumpInstrsOnly( blk );
        DumpGotos( blk, FALSE );
        blk = blk->next_block;
    }
}


extern  void    DumpRange(int first,int last)
/*******************************************/
{
    first=first;last=last;
    DumpBlkI();
}

extern  void    DumpABlk( block *b )
/**********************************/
{
    DumpPtr( b );
    DumpLiteral( " " );
    DumpBlkId( b );
    DumpBlkLabel( b );
    DumpLiteral( " Depth " );
    DumpInt( b->depth );
    DumpNL();
    DumpBlkFlags( b );
    DumpInputs( b );
    DumpDataFlo( b );
    DumpInsList( b );
    DumpGotos( b, TRUE );
    DumpNL();
}

extern  void    DumpBlock( block *b )
/***********************************/
{
    while( b != NULL ) {
        DumpABlk( b );
        b = b->next_block;
    }
    DumpLiteral( "-------------------------------" );
    DumpNL();
}

extern  void    DumpBlk( void )
/*****************************/
{
    DumpBlock( HeadBlock );
}
