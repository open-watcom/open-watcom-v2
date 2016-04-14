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


#include "cgstd.h"
#include "coderep.h"
#include "procdef.h"
#include "cfloat.h"
#include "opcodes.h"
#include "cgmem.h"
#include "makeins.h"
#include "s37temps.def"

extern  void            *SortList(void*,unsigned int,bool(*)(void*,void*));
extern  void            CountTempRefs(void);
extern  void            PropLocal(name*);
extern  name            *AllocAddrConst(name*,int,constant_class,type_class_def);
extern  cfloat          *CFCnvU32F(unsigned_32);
extern  name            *SAllocIndex(name*,name*,type_length,type_class_def,type_length);
extern  name            *DeAlias(name*);
extern  name            *AllocTemp(type_class_def);
extern  void            PrefixIns(instruction*,instruction*);
extern  name            *ScaleIndex(name*,name*,type_length,type_class_def,type_length,int,i_flags);
extern  name            *AllocRegName(hw_reg_set);
extern  hw_reg_set      DisplayReg(void);
extern  void            FindReferences();
extern  void            DoNothing( instruction* );

extern  proc_def        *CurrProc;
extern  block           *HeadBlock;
extern  name            *Names[];

#define SLACK           (1*1024)
#define _4K             (4*1024)

#define SAFE            (_4K-SLACK)

static  name            *NoseIn;
static  bool            NoseInAdded;


static  bool    TempBigger( name *t1, name *t2 ) {
/************************************************/

    return( t1->n.size > t2->n.size );
}


static  void    SortTemps() {
/****************************
    Sort the temporaries by size
*/

    Names[ N_TEMP ] = SortList( Names[ N_TEMP ], offsetof( name, n.next_name ),
                                TempBigger );
}


extern  void    PushLocals() {
/*****************************
    Intentionally a stub for 370 version
*/

}


extern  void    SetTempLocation( name *temp, type_length size ) {
/****************************************************************
    Grab a temp off the stack and set its location
*/

    temp->t.location = CurrProc->locals.size + CurrProc->locals.base;
    CurrProc->locals.size += size;
}


extern  void    RelocParms() {
/*****************************
    Relocate parameter locations based on what type of prolog we generated,
    how many registers were pushed on the stack, and all that other stuff
    figured out by GenProlog and TempStrategy
*/

    name        *temp;

    if( CurrProc->state.attr & (ROUTINE_ALTERNATE_AR) ) return;
    for( temp = Names[ N_TEMP ]; temp != NULL; temp = temp->n.next_name ) {
        if( ( temp->v.usage & HAS_MEMORY ) == 0 ) continue;
        if( temp->t.temp_flags & (STACK_PARM|FAR_LOCAL) ) continue;
        if( temp == NoseIn ) continue;
        temp->t.location += CurrProc->parms.size;
    }
}


extern  void    AdjustConsts( type_length size ) {
/*************************************************
    There were some instructions left in HeadBlock of the form
    MOV const => temp, to be used to get access to parms/autos that aren't
    addressable with a register +- 4K. Once we have figured out exactly
    where all our temporaries are going to be, we know what to set
    these constants to.
*/

    name        *cons;

    for( cons = Names[ N_CONSTANT ]; cons != NULL; cons = cons->n.next_name ) {
        if( cons->c.const_type == CONS_OFFSET ) {
            cons->c.const_type = CONS_ABSOLUTE;
            cons->c.int_value *= _4K;
            cons->c.int_value += size;
            cons->c.value = CFCnvU32F( cons->c.int_value );
        }
    }
}


extern type_length FarLocalSize()
/*******************************/
{
    if( NoseIn == NULL || NoseInAdded ) return( CurrProc->targ.far_local_size );
    return( CurrProc->targ.far_local_size + NoseIn->n.size );
}


extern  bool    AdjustFarLocals( type_length size ) {
/****************************************************
    Set the location of NoseIn now that temp locations are known(see OnTheEdge).
*/

    NoseInAdded = true;
    if( NoseIn == NULL ) return( false );
    NoseIn->t.location = size;
    size += NoseIn->n.size;
    CurrProc->targ.far_local_size += NoseIn->n.size;
    AdjustConsts( size );
    return( true );
}



extern  void    AdjustNearLocals( type_length size ) {
/****************************************************
    Bump up the offset of each near local to adjust for the save area
*/
    name        *temp;

    for( temp = Names[ N_TEMP ]; temp != NULL; temp = temp->n.next_name ) {
        if( ( temp->v.usage & HAS_MEMORY ) == 0 ) continue;
        if( temp->t.temp_flags & (STACK_PARM|FAR_LOCAL) ) continue;
        temp->t.location += size;
    }
}



static  void    AddAliasRefs() {
/*******************************

    Add the reference count of any alias to its master temporaries
    reference count, to simplify the subsequent computations.
*/

    name        *temp;
    name        *alias;

    for( temp = Names[ N_TEMP ]; temp != NULL; temp = temp->n.next_name ) {
        if( temp->t.temp_flags & ALIAS ) continue;
        alias = temp->t.alias;
        while( alias != temp ) {
            temp->t.u.ref_count += alias->t.u.ref_count;
            alias = alias->t.alias;
        }
    }
}


static  void    ThrowOutBigTemps( type_length temp_size ) {
/**********************************************************
    Mark temporaries as needing an expensive reference, biggest to smallest,
    until we can address the rest cheaply (within 4K area).
*/

    name        *temp;
    name        *alias;

    NoseIn = NULL;
    for( temp = Names[ N_TEMP ]; temp != NULL; temp = temp->n.next_name ) {
        if( temp_size <= SAFE ) break;
        if( temp->t.temp_flags & (STACK_PARM|ALIAS) ) continue;
        temp_size -= temp->n.size;
        NoseIn = temp;
        alias = temp;
        do {
            alias->t.temp_flags |= VISITED;
            alias = alias->t.alias;
        } while( alias != temp );
    }
}


static  void    ThrowOutParms() {
/********************************
    We're going to be bumping AR past the parameters, so all parameters
    need an expensive reference.
*/

    name        *temp;
    name        *base;
    name        *alias;
    type_length place;

    NoseIn = NULL;
    for( temp = Names[ N_TEMP ]; temp != NULL; temp = temp->n.next_name ) {
        if( temp->t.temp_flags & ALIAS ) continue;
        if( !( temp->t.temp_flags & STACK_PARM ) ) continue;
        base = DeAlias( temp );
        place = base->t.location + ( temp->v.offset - base->v.offset );
        alias = temp;
        do {
            alias->t.temp_flags |= FAR_LOCAL;
            alias = alias->t.alias;
        } while( alias != temp );
    }
}


static  void    OnTheEdge() {
/****************************
    Well, we have marked a bunch of temporaries as VISITED (meaning they
    need an expensive reference). We can change our mind now and let
    one of these stick its nose into the 4K area so it can be cheaply
    addressed. (NoseIn). Figure out which one is most worthy (saves most
    references) and remember it
*/

    name        *temp;

    for( temp = Names[ N_TEMP ]; temp != NULL; temp = temp->n.next_name ) {
        if( temp->t.temp_flags & VISITED ) {
            if( temp->t.u.ref_count > NoseIn->t.u.ref_count ) {
                NoseIn = DeAlias( temp );
            }
        }
    }
    temp = NoseIn;
    do {
        temp->t.temp_flags &= ~VISITED;
        temp = temp->t.alias;
    } while( temp != NoseIn );
}


static  void    AllocFarLocals() {
/*********************************
    Allocate stack space to all far locals, relatave to 0 being the
    first "far" local. These locations will be adjusted later by
    AdjustFarLocals.
*/

    name        *temp;

    for( temp = Names[ N_TEMP ]; temp != NULL; temp = temp->n.next_name ) {
        if( !( temp->t.temp_flags & VISITED ) ) continue;
        temp->t.temp_flags &= ~VISITED;
        temp->t.temp_flags |= FAR_LOCAL;
        if( temp->t.temp_flags & ALIAS ) continue;
        temp->t.location = CurrProc->targ.far_local_size;
        CurrProc->targ.far_local_size += temp->n.size;
        temp->v.usage |= HAS_MEMORY+USE_MEMORY;
        PropLocal( temp );
    }
    NoseIn->v.usage |= HAS_MEMORY+USE_MEMORY;
    NoseIn->t.location = 0;
    PropLocal( NoseIn );
}


static  void    CheckOp( name **offsets, instruction *ins, name **pop ) {
/************************************************************************
    used by FixFarLocalRefs to change one far local reference to
    an index, using the appropriate multiple of 4K constant to get
    at the temporary. The constant values are adjusted after the
    prolog is generated.
*/

    name        *op;
    name        *base;
    name        *temp;
    unsigned_32 place;
    int         i;
    instruction *new_ins;

    op = *pop;
    if( op->n.class == N_INDEXED ) {
        temp = op->i.index;
        if( temp->n.class != N_TEMP ) return;
        if( !( temp->t.temp_flags & FAR_LOCAL ) ) return;
        new_ins = MakeMove( temp, AllocTemp( temp->n.name_class ), temp->n.name_class );
        *pop = ScaleIndex( new_ins->result, op->i.base,
                          op->i.constant, op->n.class, op->n.size,
                          op->i.scale, op->i.index_flags );
        PrefixIns( ins, new_ins );
        CheckOp( offsets, new_ins, &new_ins->operands[ 0 ] );
    }
    if( op->n.class != N_TEMP ) return;
    if( !( op->t.temp_flags & FAR_LOCAL ) ) return;
    base = DeAlias( op );
    place = base->t.location + ( op->v.offset - base->v.offset );
    i = place/_4K;
    if( offsets[ i ] == NULL ) {
        /*set the symbol field in the AddrConst to non-NULL for score-boarder*/
        new_ins = MakeMove( AllocAddrConst( (name *)&CurrProc, i,
                                            CONS_OFFSET, WD ),
                        AllocTemp( WD ), WD );
        offsets[ i ] = new_ins->result;
        PrefixIns( HeadBlock->ins.hd.next, new_ins );
    }
    temp = AllocTemp( WD ),
    new_ins = MakeMove( offsets[ i ], temp, WD );
    PrefixIns( ins, new_ins );
    new_ins = MakeBinary( OP_ADD, temp, AllocRegName( DisplayReg() ), temp, WD);
    PrefixIns( ins, new_ins );
    *pop = ScaleIndex( temp, op, place%_4K,
                        op->n.name_class, op->n.size, 0, X_FAKE_BASE );
}


static  void    FixFarLocalRefs( type_length size ) {
/****************************************************
    Turn far local references into indexed references so that we
    can address all of our auto variables and parms.
*/

    block       *blk;
    instruction *ins;
    int         i;
    int         offset_size;
    name        **offsets;

    i = size / _4K;
    offset_size = (i+1) * sizeof( name** );
    offsets = CGAlloc( offset_size );
    while( i >= 0 ) {
        offsets[ i ] = NULL;
        --i;
    }
    blk = HeadBlock;
    while( blk != NULL ) {
        ins = blk->ins.hd.next;
        while( ins->head.opcode != OP_BLOCK ) {
            i = ins->num_operands;
            while( --i >= 0 ) {
                CheckOp( offsets, ins, &ins->operands[ i ] );
            }
            if( ins->result != NULL ) {
                CheckOp( offsets, ins, &ins->result );
            }
            ins = ins->head.next;
        }
        blk = blk->next_block;
    }
    CGFree( offsets );
}


extern  void    TempStrategy() {
/*******************************
    Figure out whether we will have any auto variables, parms
    which aren't within 4K of the AR register, and turn references
    to these temporaries into indexed references.
    Look only for autos that have complicated lives.
*/

    name        *temp;
    type_length temp_size;

    temp_size = 0;
    NoseInAdded = false;
    NoseIn = NULL;
    if( CurrProc->state.attr & ROUTINE_OS ) {
        CurrProc->parms.size = 0;
    } else {
        CurrProc->parms.size = CurrProc->state.parm.offset;
    }
    FindReferences();
    for( temp = Names[ N_TEMP ]; temp != NULL; temp = temp->n.next_name ) {
        if( temp->t.temp_flags & ALIAS ) continue;
        if( temp->t.temp_flags & STACK_PARM ) continue;
        if( temp->v.usage & USE_IN_ANOTHER_BLOCK ) {
            temp_size += temp->n.size;
        }
    }
    if( temp_size + CurrProc->parms.size <= SAFE ) {
        /* don't worry, be happy */
    } else {
        if( temp_size <= SAFE || CurrProc->parms.size > 100 ) {
            CurrProc->state.attr |= ROUTINE_ALTERNATE_AR;
            ThrowOutParms();
            FixFarLocalRefs( CurrProc->parms.size );
            AdjustConsts( -CurrProc->parms.size );
        }
        if( temp_size > SAFE ) {
            SortTemps();
            ThrowOutBigTemps( temp_size );
            CountTempRefs();
            AddAliasRefs();
            OnTheEdge();
            AllocFarLocals();
            FixFarLocalRefs( CurrProc->targ.far_local_size );
        }
    }
}

extern  void    EatParmConv( void ) {
/****************************************************
    Eat any converts of parms that stayed in stack.
*/

    block       *blk;
    instruction *ins;
    name        *result;
    name        *parmin;

    blk = HeadBlock;
    if( blk != NULL ) {
        ins = blk->ins.hd.next;
        while( ins->head.opcode != OP_BLOCK ) {
            if( ins->head.opcode == OP_CONVERT
             && ins->ins_flags|INS_PARAMETER ){
                result = ins->result;
                parmin = ins->operands[0];
                if( parmin->n.class == N_TEMP && result->n.class == N_TEMP ) {
                    DoNothing( ins );
                }
            }
            ins = ins->head.next;
        }
    }
}
