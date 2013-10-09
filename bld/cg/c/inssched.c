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
* Description:  Instruction reordering for better pipeline utilization.
*
****************************************************************************/


#include "cgstd.h"
#include <limits.h>
#include "coderep.h"
#include "indvars.h"
#include "cgmem.h"
#include "spawn.h"
#include "memout.h"
#include "freelist.h"
#include "sched.h"
#include "pattern.h"
#include "zoiks.h"
#include "data.h"
#include "x87.h"


extern  void            ProcMessage(msg_class);
extern  mem_out_action  SetMemOut(mem_out_action);
extern  pointer         SafeRecurse(pointer(*)(),pointer);
extern  ret_maybe       ReDefinedBy(instruction *, name *);
extern  instruction_id  Renumber(void);
extern  hw_reg_set      StackReg(void);
extern  FU_entry        *FUEntry(instruction *);
extern  int             CountIns(block*);
extern  bool            VisibleToCall(instruction *,name *, bool);
extern  bool            IsSegReg(hw_reg_set);
extern  bool            DoesSomething(instruction*);
extern  name            *ScaleIndex(name*,name*,type_length,type_class_def,type_length,int,i_flags);
extern  hw_reg_set      LowReg(hw_reg_set);
extern  hw_reg_set      HighReg(hw_reg_set);
extern  void            ClearInsBits( instruction_flags );
extern  hw_reg_set      FullReg( hw_reg_set );

#define DEPS_IN_BLOCK    20

typedef struct dep_list_block {
    struct dep_list_block      *next;
    unsigned                    used;
    struct dep_list_entry      entries[DEPS_IN_BLOCK];
} dep_list_block;

typedef enum {
    DEP_NONE,
    DEP_OP,
    DEP_INDEX
} dep_type;


data_dag                *DataDag;   /* global for dump routines */
static pointer          *DepFrl;
static dep_list_block   *CurrDepBlock;
static block            *SBlock;


extern  bool    SchedFrlFree( void )
/***********************************
    Free the instruction schedulers dependancy link lists.
*/
{
    return( FrlFreeAll( &DepFrl, sizeof( dep_list_block ) ) );
}

static dep_list_entry *AllocDep( void )
/**************************************
    Allocate one dependancy link structure.
*/
{
    dep_list_block  *new;

    if( CurrDepBlock == NULL || CurrDepBlock->used >= DEPS_IN_BLOCK ) {
        new = AllocFrl( &DepFrl, sizeof( dep_list_block ) );
        new->next = CurrDepBlock;
        new->used = 0;
        CurrDepBlock = new;
    }
    return( &CurrDepBlock->entries[ CurrDepBlock->used++ ] );
}


static unsigned InsStallable( instruction *ins )
/***********************************************
    Determine how stallable is an instruction is. The more stallable resources
    an instruction uses, and the earlier in the instruction pipeline those
    resources are used, the more likely the instruction is going to have to
    wait due to a dependancy. This is used for breaking ties when we actually
    schedule the instructions in a block.
*/
{
    unsigned stallable;
    int i;

    stallable = 0;
    for( i = ins->num_operands - 1; i >= 0; --i ) {
        switch( ins->operands[i]->n.class ) {
        case N_INDEXED:
            stallable += 3;
            break;
        case N_REGISTER:
            stallable += 2;
            break;
        case N_MEMORY:
            stallable += 1;
            break;
        }
    }
    if( ins->result != NULL && ins->result->n.class == N_INDEXED ) {
        stallable += 3;
    }
    return( stallable );
}

static void InitDag( void )
/**************************
    Allocate the data dependancy dag list, and initialize the fields
    in it.
*/
{
    data_dag    *dag;
    data_dag    *head;
    instruction *ins;

    head = NULL;
    for( ins = SBlock->ins.hd.next; ins->head.opcode != OP_BLOCK;
         ins = ins->head.next ) {
        ins->ins_flags &= ~INS_INDEX_ADJUST;
        switch( ins->head.opcode ) {
        case OP_ADD:
        case OP_SUB:
            if( ins->operands[0] == ins->result
                && ins->operands[0]->n.class == N_REGISTER
                && ins->operands[1]->n.class == N_CONSTANT
                && ins->operands[1]->c.const_type == CONS_ABSOLUTE
                && (ins->type_class == WD || ins->type_class == SW) ) {
                /*
                    We have an instruction of the form:
                        ADD REG, CONST => REG
                    These can slide past instructions who just use REG
                    as an index register, as long as we adjust the
                    index offset by CONST. This gives the scheduler a
                    little more freedom to move things around.
                */
                ins->ins_flags |= INS_INDEX_ADJUST;
            }
            break;
        }
        dag = CGAlloc( sizeof( data_dag ) );
        dag->ins = ins;
        dag->height = 0;
        dag->anc_count = 0;
        dag->deps = NULL;
        dag->visited = FALSE;
        dag->stallable = InsStallable( ins );
        dag->prev = head;
        head = dag;
    }
    DataDag = head;
}

static bool StackOp( instruction *ins )
/**************************************
    Does this instruction explicitly or implicitly use the machine's stack
    pointer.
*/
{
    int     i;
    hw_reg_set  sp;
    name        *op;

    switch( ins->head.opcode ) {
    case OP_CALL:
    case OP_CALL_INDIRECT:
    case OP_PUSH:
    case OP_POP:
        return( TRUE );
    }
    sp = StackReg();
    for( i = ins->num_operands-1; i >= 0; --i ) {
        op = ins->operands[i];
        if( op->n.class == N_INDEXED ) op = op->i.index;
        if(op->n.class == N_REGISTER && HW_Ovlap(sp,op->r.reg)) return( TRUE );
    }
    op = ins->result;
    if( op == NULL ) return( FALSE );
    if( op->n.class == N_INDEXED ) op = op->i.index;
    if( op->n.class == N_REGISTER && HW_Ovlap(sp,op->r.reg) ) return( TRUE );
    return( FALSE );
}


static  bool    ReallyDefinedBy( instruction *ins_i, instruction *ins_j,
                                 name *op, bool ins_linked )
/**********************************************************************/
{
    ret_maybe   redefd;
    instruction *ins;

    redefd = ReDefinedBy( ins_i, op );
    if( redefd != MAYBE || !ins_linked ) return( redefd );
    _INS_NOT_BLOCK( ins_i );
    _INS_NOT_BLOCK( ins_j );
    if( ins_i->id > ins_j->id ) {
        ins = ins_i;
        ins_i = ins_j;
        ins_j = ins;
    }
    while( ins_i != ins_j ) {
        if( ReDefinedBy( ins_i, op->i.index ) ) return( TRUE );
        ins_i = ins_i->head.next;
    }
    return( FALSE );
}

static bool OkToSlide( instruction *ins, name *op )
/**************************************************
    Is it OK to slide an INDEX_ADJUST instruction past an index name?
*/
{
    #if  _TARGET & (_TARG_80386 | _TARG_IAPX86 )
        int             i;
    #endif

    ins = ins;
    if( op->i.base != NULL ) return( TRUE );
    if( op->i.constant != 0 ) return( TRUE );
    if( OptForSize >= 50 ) return( FALSE );
    #if  _TARGET & (_TARG_80386 | _TARG_IAPX86 )
        /* bad news to add a displacement on an instruction that also
           has a constant operand (takes an extra clock) */
        for( i = ins->num_operands-1; i >= 0; --i ) {
            if( ins->operands[i]->n.class == N_CONSTANT ) return( FALSE );
        }
    #endif
    return( TRUE );
}

static bool HiddenDependancy( instruction *ins, name *op )
/*********************************************************
    Look out for cheesy hidden dependencies. These can come
    about, for instance, when two instructions modify
    AL and AH - these cause a stall.
*/
{
    int         i;
    hw_reg_set  full;

    if( FPStackIns( ins ) ) return( FALSE );
    if( op->n.class != N_REGISTER ) return( FALSE );
    full = FullReg( op->r.reg );
    op = ins->result;
    for( i = 0; i < ins->num_operands; i++ ) {
        if( op == NULL ) continue;
        if( op->n.class != N_REGISTER ) continue;
        if( HW_Ovlap( full, op->r.reg ) ) return( TRUE );
        op = ins->operands[ i ];
    }
    return( FALSE );
}

static dep_type CheckOneOp( instruction *ins_i, instruction *ins_j,
                        name *op, bool ins_linked )
/******************************************************************
    Check one op for redefinition
*/
{
    if( op->n.class == N_INDEXED && op->i.index != NULL ) {
        if( ins_linked
            && (ins_i->ins_flags & INS_INDEX_ADJUST)
            && HW_Ovlap( ins_i->result->r.reg, op->i.index->r.reg )
            && OkToSlide( ins_j, op ) ) {
            return(DEP_NONE);
        }
        if( ReDefinedBy( ins_i, op->i.index ) ) {
            return( DEP_INDEX );
        }
    }
    if( ReallyDefinedBy( ins_i, ins_j, op, ins_linked ) ) {
        if( !FPStackReg( op ) ) return( DEP_OP );
        if( ins_i->sequence == ins_j->sequence ) return( DEP_OP );
    }
    if( HiddenDependancy( ins_i, op ) ) {
        return( DEP_OP );
    }
    return( DEP_NONE );
}


static dep_type DataDependant( instruction *ins_i,
                               instruction *ins_j, bool ins_linked )
/************************************************************************
    Does instruction i redefine any of the operands or result of
    instruction j? And if so, how?
*/
{
    int             k;
    dep_type        ret;

    ret = DEP_NONE;
    for( k = ins_j->num_operands-1; k >= 0; --k ) {
        /* first operand of OP_LA can't be dependant unless N_INDEXED */
        if( k == 0
            && ins_j->head.opcode == OP_LA
            && ins_j->operands[0]->n.class != N_INDEXED ) continue;
        ret = CheckOneOp( ins_i, ins_j, ins_j->operands[k], ins_linked );
        if( ret != DEP_NONE ) return( ret );
    }
    if( ins_j->result != NULL ) {
        ret = CheckOneOp( ins_i, ins_j, ins_j->result, ins_linked );
        if( ret != DEP_NONE ) return( ret );
    }
    if( _IsModel( FORTRAN_ALIASING ) && _OpIsCall( ins_j->head.opcode ) ) {
        if( !ins_linked ) return( DEP_OP );
        /*
            All NOP's following a call instruction with a result indicate
            variables which might have been modified by the call (for FORTRAN).
            Therefore, they must be made dependant on the call.
        */
        for( ;; ) {
            ins_j = ins_j->head.next;
            if( ins_j->head.opcode == OP_BLOCK ) break;
            if( _OpIsCall( ins_j->head.opcode ) ) break;
            if( ins_j->head.opcode == OP_NOP && ins_j->result != NULL ) {
                ret = DataDependant( ins_i, ins_j, ins_linked );
                if( ret != DEP_NONE ) break;
            }
        }
    }
    return( ret );
}

static bool ImplicitDependancy( instruction *imp, instruction *ins )
/*******************************************************************

    Does instruction 'imp' modify a implicitly used (segment) register
    and, if so, does 'ins' use it?

    Also, does 'imp' modify a register which may be used by ins
    in a non-obvious manner (ie modify BL and ins uses BH).

    NOTE: This routine could be a little bit smarter than it is. The
        only segment registers that are a problem are DS and SS and only
        then if the memory operand is not being overriden.
        P.S. ES might be a problem in FLAT model.
*/
{
    name        *op;
    int         i;

    op = imp->result;
    if( op == NULL ) return( FALSE );
    if( op->n.class != N_REGISTER ) return( FALSE );
    if( !IsSegReg( op->r.reg ) ) return( FALSE );
    if( _OpIsCall( ins->head.opcode ) ) return( TRUE );
    for( i = ins->num_operands-1; i >= 0; --i ) {
        op = ins->operands[i];
        switch( op->n.class ) {
        case N_MEMORY:
        case N_INDEXED:
            return( TRUE );
        }
    }
    op = ins->result;
    if( op != NULL ) {
        switch( op->n.class ) {
        case N_MEMORY:
        case N_INDEXED:
            return( TRUE );
        }
    }
    return( FALSE );
}


extern bool InsOrderDependant( instruction *ins_i, instruction *ins_j )
/*********************************************************************/
{
    if( ins_j->head.opcode == OP_NOP
        && ins_j->result == NULL
        && !DoesSomething( ins_j ) ) return( TRUE );
    if( _OpIsJump( ins_i->head.opcode ) ) return( TRUE );
    if( _OpIsCall( ins_i->head.opcode ) ) {
        if( !( ins_i->flags.call_flags & CALL_READS_NO_MEMORY )
         && ins_j->result != NULL
         && VisibleToCall( ins_i, ins_j->result, FALSE ) ) return( TRUE );
         if( FPStackIns( ins_j ) ) return( TRUE );
    }
    if( StackOp( ins_i ) && StackOp( ins_j ) ) return( TRUE );
    if( FPStackIns( ins_i ) ) {
        if( FPStackIns( ins_j ) ) {
            if( ins_i->sequence == ins_j->sequence ) return( TRUE );
            if( FPInsIntroduced( ins_j ) ) return( TRUE );
            if( FPInsIntroduced( ins_i ) ) return( TRUE );
        }
        if( _OpIsCall( ins_j->head.opcode ) ) return( TRUE );
    }
    if( DataDependant( ins_i, ins_j, TRUE ) != DEP_NONE ) return( TRUE );
    if( DataDependant( ins_j, ins_i, TRUE ) != DEP_NONE ) return( TRUE );
    if( ImplicitDependancy( ins_i, ins_j ) ) return( TRUE );
    if( ImplicitDependancy( ins_j, ins_i ) ) return( TRUE );
    return( FALSE );
}


static  bool    MultiIns( instruction *ins )
/*******************************************
    Is ins part of a two instruction sequence, like SUB, SBB?
*/
{
    switch( ins->head.opcode ) {
    case OP_ADD:
    case OP_SUB:
    case OP_MUL:
    case OP_NEGATE:    /* used in 4-byte negate sequences on i86 */
        if( ins->ins_flags & INS_CC_USED ) return( TRUE );
    }
    return( FALSE );
}

static  bool    InsUsesCC( instruction *ins )
/********************************************
    Does 'ins' uses the condition codes set from a previous instruction?
*/
{
    switch( ins->head.opcode ) {
    case OP_EXT_ADD:
    case OP_EXT_SUB:
    case OP_EXT_MUL:
        return( TRUE );
    }
    if( _OpIsCondition( ins->head.opcode ) ) {
        if( ins->u.gen_table->generate == G_NO ) {
            /*
                Any conditional whose gen table entry is pointing at a G_NO
                is not going to generate a comparision operation, and is
                depending on a previous instruction to set the flags.
            */
            return( TRUE );
        }
    }
    return( FALSE );
}

static void BuildLink( data_dag *i, data_dag *j )
/************************************************
    Added a data dependancy link between instructions i and j.
*/
{
    dep_list_entry  *dep;

    dep = AllocDep();
    dep->next = i->deps;
    i->deps = dep;
    dep->dep = j;
}

static void BuildDag( void )
/***************************
    Build the data dependancy DAG. Note that this sucker is an N**2 algorithm
    on the size of the basic block, so blocks had better not be too big.
*/
{
    data_dag    *dag_i;
    data_dag    *dag_j;
    bool        link_multi;
    bool        link_all;
    bool        used_cc;

    for( dag_i = DataDag; dag_i != NULL; dag_i = dag_i->prev ) {
        used_cc = (dag_i->ins->ins_flags & INS_CC_USED) != 0;
        link_all = FALSE;
        link_multi = FALSE;
        switch( dag_i->ins->head.opcode ) {
        case OP_EXT_ADD:
        case OP_EXT_SUB:
        case OP_EXT_MUL:
            /*
                ADC's can't get scheduled before the instruction that's setting
                the carry bit for them.
            */
            link_multi = TRUE;
            break;
        case OP_STORE_UNALIGNED:
        case OP_LOAD_UNALIGNED:
            /*
                Fixme: don't really want these to be WALLS, but they cause
                many problems because of their grenade like behaviour
            */
        case OP_NOP:
            /*
                Do not want any instructions moving across a NOP boundry.
            */
            link_all = TRUE;
        }
        for( dag_j = dag_i->prev; dag_j != NULL; dag_j = dag_j->prev ) {
            if( link_all ) {
                BuildLink( dag_i, dag_j );
            } else if( InsOrderDependant( dag_i->ins, dag_j->ins ) ) {
                BuildLink( dag_i, dag_j );
            } else if( link_multi && MultiIns( dag_j->ins ) ) {
                link_multi = FALSE;
                BuildLink( dag_i, dag_j );
            } else if( used_cc &&
                    (dag_j->ins->u.gen_table->op_type&MASK_CC)!=PRESERVE ) {
                /*
                    Consider this:

                    XOR         AX,AX   <-- j points here
                    SUB         BX,CX   <-- i points here
                    SBB         DX,SI

                    If 'i' is an instruction where the flags are used later
                    we can not allow any instruction that modifies the flags
                    to be scheduled after it.
                */
                BuildLink( dag_i, dag_j );
            } else if( (dag_i->ins->u.gen_table->op_type&MASK_CC)!=PRESERVE
                    && InsUsesCC( dag_j->ins ) ) {
                /*
                    consider:

                        ADD AX,SI => AX
                        ADC BX,DX => BX     <---- j points here
                        ??? CX,?? => CX     <---- i points here

                    we have to create a dependency between instruction 3 and
                    2 so that 3 never gets scheduled between 1 and 2 as it
                    will affect the flags
                */
                BuildLink( dag_i, dag_j );
            }
        }
    }
}

static pointer AnnointADag( data_dag *dag )
/******************************************
    Find out how many ancestors an instruction has, and what the longest
    amount of time it will take to execute all the instructions this one
    depends on (height).
*/
{
    unsigned        max_cycle_count;
    data_dag        *pred;
    dep_list_entry  *dep;

    max_cycle_count = 0;
    dag->visited = TRUE;
    for( dep = dag->deps; dep != NULL; dep = dep->next ) {
        pred = dep->dep;
        pred->anc_count++;
        if( !pred->visited ) SafeRecurse( AnnointADag, pred );
        if( pred->height > max_cycle_count ) max_cycle_count = pred->height;
    }
    dag->height = max_cycle_count + FUEntry( dag->ins )->opnd_stall;
    /* return a pointer to satisfy SafeRecurse */
    return( NULL );
}

static void AnnointDag( void )
/*****************************
    Add additional information to data dependancy DAG.
*/
{
    data_dag    *dag;

    for( dag = DataDag; dag != NULL; dag = dag->prev ) {
        if( !dag->visited ) AnnointADag( dag );
    }
}

extern int StallCost( instruction *ins, instruction *top )
/*********************************************************
    If instruction 'ins' were placed before instruction 'top', how long
    would instructions following 'ins' have to wait before they could
    complete?
*/
{
    unsigned    avail_fu;
    unsigned    fu_overlap;
    int         unit_stall;
    int         opnd_stall;
    FU_entry    *entry;
    instruction *curr;
    instruction *last;
    bool        ins_stack;

    /*
        NOP's always get a stall cost of -1 so that they are scheduled
        as soon as they become ready.
    */
    if( ins->head.opcode == OP_NOP ) return( -1 );
    /* or instructions whose condition code results are used */
    if( ins->ins_flags & INS_CC_USED ) return( -1 );
    if( top == NULL ) return( 0 );
    if( FPFreeIns( ins ) ) return( -1 );
    last = top;
    while( FPFreeIns( top ) ) top = top->head.next;
    entry = FUEntry( ins );
    avail_fu = entry->good_fu;
    unit_stall = entry->unit_stall;
    opnd_stall = entry->opnd_stall;
    ins_stack = StackOp( ins );
    curr = top;
    for( ;; ) {
        if( unit_stall == 0 && opnd_stall == 0 ) return( 0 );
        if( opnd_stall > 0 ) {
            --opnd_stall;
            switch( DataDependant( ins, curr, FALSE ) ) {
            case DEP_OP:
                return( opnd_stall * 2 );
            case DEP_INDEX:
                /* make stalls due to index usage slightly worse */
                return( opnd_stall * 2 + 1 );
            }
            /* two stack ops will stall on the implicit use of SP */
            if( ins_stack && StackOp( curr ) ) return( opnd_stall*2 + 1 );
        }
        if( unit_stall > 0 ) {
            --unit_stall;
            entry = FUEntry( curr );
            fu_overlap = avail_fu & entry->good_fu;
            if( fu_overlap != 0 ) {
                /* turn off one of the fu bits */
                fu_overlap &= fu_overlap - 1;
                avail_fu &= fu_overlap;
                if( avail_fu == 0 ) return( unit_stall * 2 );
            }
        }
        curr = curr->head.next;
        if( curr == last ) return( 0 );
    }
}


static int ScaleAdjust( name *op, hw_reg_set reg )
/*************************************************
    Figure out if we have to adjust this operand, and what the
    scale factor is.
*/
{
    hw_reg_set  idx_reg;

    if( op->n.class != N_INDEXED ) return( -1 );
    idx_reg = op->i.index->r.reg;
    if( !HW_Ovlap( idx_reg, reg ) ) return( -1 );
    if( op->i.index_flags & X_HIGH_BASE ) {
        if( HW_Ovlap( HighReg( idx_reg ), reg ) ) return( 0 );
    }
    if( op->i.index_flags & X_LOW_BASE ) {
        if( HW_Ovlap( LowReg( idx_reg ), reg ) ) return( 0 );
    }
    return( op->i.scale );
}


static void FixIndexAdjust( instruction *adj, bool forward )
/***********************************************************

    'adj' is an index adjust instruction. We must run forward/backward
    through the block looking for uses of the register as an index and
    adjust the offset by the correct amount. We can stop when we run
    into the 'adj' instruction, or an OP_BLOCK instruction, or an instruction
    that redefined the register used by 'adj' that isn't also an
    INS_INDEX_ADJUST. We know that we have to modify an index offset
    if the instruction id of the 'adj' is greater than the id of the
    instruction that we're checking when going forward, or the reverse
    when going backward.
*/
{
    instruction *chk;
    name        *op;
    type_length bias;
    hw_reg_set  reg;
    int         i;
    int         scale;

    bias = adj->operands[1]->c.int_value;
    if( adj->head.opcode == OP_SUB ) bias = -bias;
    if( forward ) bias = -bias;
    _INS_NOT_BLOCK( adj );
    reg = adj->result->r.reg;
    chk = adj;
    for( ;; ) {
        if( forward ) {
            chk = chk->head.next;
        } else {
            chk = chk->head.prev;
        }
        if( chk == adj ) break;
        if( chk->head.opcode == OP_BLOCK ) break;
        if( !(chk->ins_flags & INS_INDEX_ADJUST) && ReDefinedBy( chk, adj->result ) ) break;
        if( forward ) {
            if( chk->id > adj->id ) continue;
        } else {
            if( chk->id < adj->id ) continue;
        }
        for( i = chk->num_operands-1; i >= 0; --i ) {
            op = chk->operands[i];
            scale = ScaleAdjust( op, reg );
            if( scale >= 0 ) {
                chk->operands[i] = ScaleIndex( op->i.index, op->i.base,
                              op->i.constant + (bias << scale),
                              op->n.name_class, op->n.size,
                              op->i.scale, op->i.index_flags );
            }
        }
        op = chk->result;
        if( op != NULL ) {
            scale = ScaleAdjust( op, reg );
            if( scale >= 0 ) {
                chk->result = ScaleIndex( op->i.index, op->i.base,
                              op->i.constant + (bias << scale),
                              op->n.name_class, op->n.size,
                              op->i.scale, op->i.index_flags );
            }
        }
    }
}


static void ScheduleIns( void )
/******************************
    Rearrange the instructions in a block according to the data dependancy
    DAG for maximum overlap of the instruction pipeline. The block is built
    from bottom to the top. At each point we have an list of instructions
    that are ready to execute. We run down this list and pick the instruction
    with the minimum stall cost. If two instructions on the ready list have
    the same stall cost, pick the one that has the longest execution time
    of instructions that depend on the one we're trying to schedule (height).
    If two instructions are equal in stall cost and height pick the one
    with the most likelyhood of being stalled by an instruction preceeding
    it (more indexes, register usage, etc). Otherwise, choose the one
    that came last in the source order.

    NOTE: This routine does not maintain the live information when it
          rebuilds the block. Don't expect anything that depends on live
          analysis to work after this routine is done.
*/
{
    data_dag        *ready;
    data_dag        *curr;
    data_dag        *best;
    data_dag        *fp_stack_just_squeeks_by;
    dep_list_entry  *dep;
    instruction     *top;
    int             best_cost;
    int             curr_cost;
    int             stk_depth;
    int             stk_over;
    unsigned_16     last_seq;


    /*
        What the hell is he doing with the visited bit here!?...
        Here's what's I'm doing. Since the ready list is only singly linked,
        it's tough to delete things efficiently. The visited bit is
        used to indicate that the instruction has been scheduled, and the
        dag has been 'virtually' deleted from the list. All the virtually
        deleted dags at the front of the ready list are really deleted just
        before any newly ready dags are added.
    */
    /* get initial 'ready' list */
    ready = NULL;
    for( curr = DataDag; curr != NULL; curr = curr->prev ) {
        curr->visited = FALSE;
        if( curr->anc_count == 0 ) {
            curr->ready = ready;
            ready = curr;
        }
    }
    last_seq = 0;
    top = NULL;
    stk_depth = FPStackExit( SBlock );
    SBlock->ins.hd.next = (instruction *)&SBlock->ins; // so we can dump!
    SBlock->ins.hd.prev = (instruction *)&SBlock->ins;
    while( ready != NULL ) {
        /* find best instruction to schedule */
        best_cost = INT_MAX;
        best = NULL;
        fp_stack_just_squeeks_by = NULL;
        for( curr = ready; curr != NULL; curr = curr->ready ) {
            if( curr->visited ) continue;
            stk_over = FPStkOver( curr->ins, stk_depth );
            if( stk_over == 0 ) {
                fp_stack_just_squeeks_by = curr;
                continue;
            } else if( stk_over > 0 ) {
                continue;
            }
            curr_cost = StallCost( curr->ins, top );
#define MARK_BEST {best = curr; best_cost = curr_cost; }
            if( best == NULL || curr_cost < best_cost ) {
                MARK_BEST;
            } else if( curr_cost == best_cost ) {
                if( curr->height > best->height ) {
                    MARK_BEST;
                } else if( curr->height == best->height ) {
                    if( (curr->ins->ins_flags & INS_INDEX_ADJUST)
                        && DataDependant( curr->ins, best->ins, FALSE ) ) {
                        /*
                           drop in a INDEX_ADJUST as soon as it doesn't
                           hurt, otherwise we end up putting it too close
                           to its use.
                        */
                        MARK_BEST;
                    } else if( curr->stallable > best->stallable ) {
                        MARK_BEST;
                    } else if( curr->stallable == best->stallable ) {
                        if( curr->ins->sequence == last_seq && best->ins->sequence != last_seq ) {
                            // try to avoid fxch instructions
                            MARK_BEST;
                        } else {
                            _INS_NOT_BLOCK( curr->ins );
                            _INS_NOT_BLOCK( best->ins );
                            if( curr->ins->id > best->ins->id ) {
                                MARK_BEST;
                            }
                        }
                    }
                }
            }
        }
        if( best == NULL ) {
            /*
               for pow( x, y ) we need 4 entries. This may be all we have.
               so our usual rules won't let us schedule anything. This is
               an escape hatch.
            */
            best = fp_stack_just_squeeks_by;
        }
        if( best == NULL ) {
            _Zoiks( ZOIKS_081 );
            best = ready;
        }
        /* insert instruction in ring before the 'top' instruction */
        if( top == NULL ) {
            top = best->ins;
            top->head.prev = top;
            top->head.next = top;
        } else {
            best->ins->head.next = top;
            best->ins->head.prev = top->head.prev;
            top->head.prev->head.next = best->ins;
            top->head.prev = best->ins;
            top = best->ins;
        }
        if( FPStackIns( top ) ) {
            last_seq = top->sequence;
        }
        if( top->ins_flags & INS_INDEX_ADJUST ) {
            FixIndexAdjust( top, TRUE );
        }
        FPCalcStk( best->ins, &stk_depth );
        best->scheduled = TRUE;
        /* really delete any virtually deleted dags */
        best->visited = TRUE;
        for( ;; ) {
            if( ready == NULL ) break;
            if( !ready->visited ) break;
            ready = ready->ready;
        }
        /* find out who becomes ready now that this instruction is scheduled */
        for( dep = best->deps; dep != NULL; dep = dep->next ) {
            curr = dep->dep;
            if( --(curr->anc_count) == 0 ) {
                curr->ready = ready;
                ready = curr;
            }
        }
    }
    SBlock->ins.hd.next = top;
    SBlock->ins.hd.prev = top->head.prev;
    top->head.prev->head.next = (instruction *)&SBlock->ins;
    top->head.prev = (instruction *)&SBlock->ins;
    /*
       We have to run backwards through the block, adjusting all
       the index offsets that were scheduled before their corresponding
       index adjustment instructions. The opposite case of an index
       adjustment being moved before the use of the index register was
       handled when we scheduled the INS_INDEX_ADJUST instruction.
    */
    for( top = SBlock->ins.hd.prev; top->head.opcode != OP_BLOCK;
                top = top->head.prev ) {
        if( top->ins_flags & INS_INDEX_ADJUST ) {
            FixIndexAdjust( top, FALSE );
        }
    }
}

static  void    FreeDataDag( void )
/**********************************
    Free all the memory allocated for the data dependancy DAG.
*/
{
    dep_list_block      *dep;
    dep_list_block      *next;
    data_dag            *dag;
    data_dag            *prev;

    for( dag = DataDag; dag != NULL; dag = prev ) {
        prev = dag->prev;
        CGFree( dag );
    }
    DataDag = NULL;
    for( dep = CurrDepBlock; dep != NULL; dep = next ) {
        next = dep->next;
        FrlFreeSize( &DepFrl, (pointer *)dep, sizeof( dep_list_block ) );
    }
    CurrDepBlock = NULL;
}

static  void    SchedBlock( void )
/*********************************
    Reorder one block for maximum parallelism.
*/
{
    unsigned    num_instrs;

    num_instrs = CountIns( SBlock );
    if( num_instrs > 2 ) {
        InitDag();
        FPPreSched( SBlock );
        BuildDag();
        AnnointDag();
        /*
           We're well and truly screwed if we run out of memory during
           ScheduleIns or FPPostSched
        */
        SetMemOut( MO_FATAL );
        ScheduleIns();
        /*
           We don't need the DAG anymore, so free it here to make it less
           likely that we'll run out of memory when doing the FPPostSched
        */
        FreeDataDag();
        FPPostSched( SBlock );
        SetMemOut( MO_SUICIDE );
    }
}

void    Schedule( void )
/***********************
    Reorder the instructions in a routine for maximum overlap of the
    instruction pipeline.
*/
{
    mem_out_action      old_memout;
    bool                first_time;

    first_time = TRUE;
    InitFrl( &DepFrl );
    old_memout = SetMemOut( MO_SUICIDE );
    for( SBlock = HeadBlock; SBlock != NULL; SBlock = SBlock->next_block ) {
        if( Spawn( &SchedBlock ) != 0 ) {
            if( first_time ) {
                ProcMessage( MSG_SCHEDULER_DIED );
                first_time = FALSE;
            }
            FreeDataDag();
        }
    }
    SchedFrlFree();
    SetMemOut( old_memout );
    Renumber();
    ClearInsBits( INS_INDEX_ADJUST );
}
