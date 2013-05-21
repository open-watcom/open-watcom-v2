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
#include "conflict.h"
#include "pattern.h"
#include "opcodes.h"
#include "cgmem.h"
#include "regset.h"
#include "model.h"
#include "cgaux.h"
#include "offset.h"
#include "makeins.h"
#include "s37index.def"

extern  block               *HeadBlock;
extern  conflict_node       *ConfList;
extern  opcode_defs         String[];

extern  bool            IsIndexReg(hw_reg_set,type_class_def,bool);
extern  conflict_node   *NameConflict(instruction*,name*);
extern  name            *AllocAddrConst(name*,int,constant_class,type_class_def);
extern  name            *AllocMemory(pointer,type_length,cg_class,type_class_def);
extern  name            *AllocTemp(type_class_def);
extern  name            *ScaleIndex(name*,name*,type_length,type_class_def,type_length,int,i_flags);
extern  reg_set_index   MarkIndex(instruction*,name*,bool);
extern  void            PrefixIns(instruction*,instruction*);
extern  name            *IndexToTemp(instruction*,name*);
extern  void            NoMemIndex(instruction *);
extern  void            ReplaceOperand(instruction*,name*,name*);
extern  name            *FindIndex(instruction*);
extern  void            TempStrategy(void);
extern  int             FEAttr(sym_handle);
extern  pointer         FEAuxInfo(pointer,int);
extern  name            *AllocRegName(hw_reg_set);
extern  hw_reg_set      GblReg();
extern  name            *AllocIntConst(int);
extern  hw_reg_set      WordReg(int);
extern  void            GetRALN(name*,char*,char*);
extern  offset          AskAddress( label_handle );
extern  label_handle    AskForSymLabel( pointer, cg_class );
extern  void            TellLblBase( label_handle, label_handle, offset );
extern  bool            AskNameROM(pointer,cg_class);
extern  void            SetDataOffsets( offset );




conflict_node *IndexSplit( instruction *ins, name *index ) {
/***********************************************************
*/

    name                *temp;
    conflict_node       *conf;

    temp = IndexToTemp( ins, index );
    conf = NameConflict( ins, temp );
    _SetTrue( conf, INDEX_SPLIT );
    MarkIndex( ins, temp, 0 );
    return( conf );
}


extern  instruction     *NeedIndex( instruction *ins ) {
/*******************************************************
    If any index conflicts are marked as NEEDS_INDEX_SPLIT, split them
    out into a temp as well.
*/

    name                *index;

    index = FindIndex( ins );
    if( index != NULL ) {
        IndexSplit( ins, index );
        ins = ins->head.prev;
    }
    return( ins );
}


static bool LoadingAddress( instruction *ins, name *mem_loc ) {
/**************************************************************
*/
    return( ( ins->head.opcode == OP_LA || ins->head.opcode == OP_CAREFUL_LA )
            && mem_loc != ins->result );
}



extern bool MemNeedsReloc( pointer symbol, cg_class memory_type ) {
/*******************************************
*/
    return( _IsModel( CODE_RENT ) && !AskNameROM( symbol, memory_type ) );
}

struct idx_list  {
    struct idx_list *next;
    name            *idx;
    name            *mem_loc;
    label_handle    label;
};
struct base_temp {
    name             *temp1;   /*initial temp to use */
    struct idx_list  *list;  /* list of index vars to updated */
};

static  struct base_temp   BaseTemp;

static void BaseTempInit(){
    BaseTemp.temp1 = AllocTemp( WD );
    BaseTemp.list = NULL;
}

static void BaseTempAdd( name *idx, label_handle label ){
/******************************************
 Add index var to list that needs base
 Keep in increasing offset
*/
    struct idx_list *cur, **lnk;

    for( lnk = &BaseTemp.list; (cur=*lnk)!=NULL; lnk = &cur->next ){
        if( idx->i.constant <= cur->idx->i.constant )break;
    }
    /* don't add same index twice */
    if( cur == NULL || idx != cur->idx ){
        cur = CGAlloc( sizeof( struct idx_list ) );
        cur->idx = idx;
        cur->label = label;
        cur->next = *lnk;
        *lnk = cur;
    }
}

static void BaseTempFini( instruction *start ){
/********************
   Set up a new base temp each time refs go out of range of the current
   temp, replace the index with the new temp.
   prune the list to only each new base temp for fixup after regalloc
*/
    struct idx_list *list, **lnk;
    type_length      cur_addr, disp;
    instruction     *new_ins;
    name            *temp;
    label_handle    base;
    offset          base_offset;

    list = BaseTemp.list;
    if( list != NULL ){
        cur_addr = -4096;
        base = NULL;
        while( list != NULL ){ /* adjust offsets */
            list->mem_loc = list->idx->i.base;
            disp = list->idx->i.constant - cur_addr;
            if( disp > 4095 ){
                temp = AllocTemp( WD );  /* get a base temp */
                cur_addr = list->idx->i.constant;
                base = list->label;
                base_offset = list->mem_loc->v.offset;
            }
            TellLblBase( list->label, base, base_offset );
            list->idx->i.constant -= cur_addr;
            list->idx->i.index  = temp;
            list = list->next;
        }
        lnk = &BaseTemp.list;
        while( (list = *lnk ) != NULL ){
            if( list->idx->i.constant != 0 ){
                *lnk = list->next;
                CGFree( list );
            }else{
                list->idx = list->idx->i.index;
                new_ins = MakeUnary( OP_LA, list->mem_loc,
                                    list->idx, WD );
                PrefixIns( start, new_ins );
                lnk = &list->next;
            }
        }
    }
}

static void BacktoConst( struct idx_list *what ){
/*************************************
  Find all occurances of temp and replace with a =A(base)
*/
    block       *blk;
    instruction *ins;

    blk = HeadBlock;
    while( blk != NULL ) {
        ins = blk->ins.hd.next;
        while( ins->head.opcode != OP_BLOCK ) {
            SettoACon( ins, what  );
            ins = ins->head.next;
        }
        blk = blk->next_block;
    }
}

static  void    SettoACon( instruction *ins, struct idx_list  *what ) {
/**********************************************
    find any refs to temp and replace with a constant
*/

    int         i;
    name        *temp;
    name        *con;

    temp = what->idx;
    con = AllocAddrConst( what->mem_loc, 0, CONS_ADDRESS, WD );
    i = ins->num_operands;
    while( --i >= 0 ) {
        if( ins->operands[ i ] == temp ) {
            ins->operands[ i ] = con;
        }
    }
    if( ins->result == temp ){
    /* a zoiks ? */
    }
}

extern void MemtoBaseTemp( void ){
/********************************
    change memrefs to an index off a temp loaded with
    an address in range of the 4k barrier ieee!
*/
    instruction *ins;
    block       *blk;

    blk = HeadBlock;
    SetDataOffsets( 0 ); /* set offsets for fixed data */
    BaseTempInit();
    while( blk != NULL ) {
        ins = blk->ins.hd.next;
        while( ins->head.opcode != OP_BLOCK ) {
            ins = NoMemRefs( ins )->head.next;
        }
        blk = blk->next_block;
    }
    blk = HeadBlock;
    BaseTempFini( blk->ins.hd.next );
}


void FixMemBases( void ){
/****************************
    replace any base temp that didn't make it to a reg with an acon
*/
    struct idx_list *list, *old;
    name   *temp;

    list = BaseTemp.list;
    while( list != NULL ){
        temp = list->idx;
        if( temp->v.usage & USE_MEMORY ){
            BacktoConst( list );
        }
        old = list;
        list = list->next;
        CGFree( old );
    }
}

static name *RepMemWithOffset(  name * mem_loc ) {
/*************************************************
    If "mem_loc" can be based off a global temp do so
    by using BaseTemp sorry about the static I'll fix it later
*/
    name               *new_idx;
    i_flags             flags;
    label_handle        label;
    unsigned  long int  addr;

    label = AskForSymLabel( mem_loc->v.symbol, mem_loc->m.memory_type );
    addr = AskAddress( label );
    if( addr != -1 ){
        flags = X_FAKE_BASE | X_BASE_IS_INDEX;
        if( mem_loc->v.usage & VAR_VOLATILE ) {
            flags |= X_VOLATILE;
        }
        addr += mem_loc->v.offset;
        new_idx = ScaleIndex( BaseTemp.temp1, mem_loc, addr,
                           mem_loc->n.name_class,
                          mem_loc->n.size, 0, flags );
        BaseTempAdd( new_idx, label );
    }else{
        new_idx = NULL;
    }
    return( new_idx );
}

static  name   *RepAux( name * mem_loc ) {
/********************************************************************
    If "mem_loc" is a global reg based var turn into offset( reg )
*/
    name                *new_idx;
    fe_attr             attr;
    int                 offset;
    i_flags             flags;

    attr = 0;
    offset = -1;
    if( mem_loc->m.memory_type == CG_FE && mem_loc->v.symbol != NULL ) {
        attr = FEAttr( mem_loc->v.symbol );
    }
    if( !( attr & FE_PROC ) && ( attr & (FE_IMPORT|FE_GLOBAL) ) ) {
        offset = (int)FEAuxInfo( FEAuxInfo( mem_loc->v.symbol, AUX_LOOKUP ),
                                 AUX_OFFSET );
    }
    if( offset != -1 ) {
        flags = X_FAKE_BASE;
        if( mem_loc->v.usage & VAR_VOLATILE ) {
            flags |= X_VOLATILE;
        }
        new_idx = ScaleIndex( AllocRegName( GblReg() ), mem_loc, offset,
                              mem_loc->n.name_class, mem_loc->n.size,
                              0, flags );
    }else{
        new_idx = NULL;
    }
    return( new_idx );
}

static  name   *RepRelocMem( instruction * ins, name * mem_loc ) {
/********************************************************************
     "mem_loc" is a relative mem location add in global base reg.
*/
    name                *new_idx;
    name                *temp;
    instruction         *new_ins;
    i_flags             flags;

    flags = X_FAKE_BASE;
    if( mem_loc->v.usage & VAR_VOLATILE ) {
        flags |= X_VOLATILE;
    }
    temp = AllocTemp( WD );
    new_ins = MakeUnary( OP_CAREFUL_LA, mem_loc, temp, WD );
    PrefixIns( ins, new_ins );
    new_ins = MakeBinary( OP_ADD, temp, AllocRegName( GblReg() ),
                          temp, WD );
    PrefixIns( ins, new_ins );
    if( LoadingAddress( ins, mem_loc ) ) {
        ins->head.opcode = OP_CONVERT;
        ins->base_type_class = WD;
        new_idx = temp;
    } else {
        new_idx = ScaleIndex( temp, mem_loc, 0, mem_loc->n.name_class,
                              mem_loc->n.size, 0, flags );
    }
    return( new_idx );
}

static  instruction    *MemToIndex( instruction * ins, name * mem_loc ) {
/************************************************************************
    Turn memory reference "mem_loc" in "ins" into a LA  MEM_LOC => temp
    and use [temp] instead.  This matches the 370 addressing modes more
    closely, and allows the register allocator to the these address
    constants in registers.  This also takes care of variables named in
    aux files, by changing them into references like 100(,10).  Finally,
    when the RENT option is turned on, non-constant references are changed
    into LA MEM_LOC => temp, ADD temp, 10 => temp
*/

    name                *temp;
    name                *new_idx;
    instruction         *new_ins;
    i_flags             flags;

    flags = X_FAKE_BASE;
    if( mem_loc->v.usage & VAR_VOLATILE ) {
        flags |= X_VOLATILE;
    }
    if( (new_idx = RepAux(  mem_loc )) != NULL ){
        ReplaceOperand( ins, mem_loc, new_idx );
    }else if( (new_idx = RepMemWithOffset(  mem_loc )) != NULL ){
        ReplaceOperand( ins, mem_loc, new_idx );
    }else if( MemNeedsReloc( mem_loc->v.symbol, mem_loc->m.memory_type ) ) {
        new_idx = RepRelocMem( ins, mem_loc );
        ReplaceOperand( ins, mem_loc, new_idx );
    } else {
        if( LoadingAddress( ins, mem_loc ) ){
            ins = NULL;
        }else{
            temp = AllocTemp( WD );
            new_ins = MakeUnary( OP_CAREFUL_LA, mem_loc, temp, WD );
            new_idx = ScaleIndex( temp, mem_loc, 0, mem_loc->n.name_class,
                              mem_loc->n.size, 0, flags );
            PrefixIns( ins, new_ins );
            ReplaceOperand( ins, mem_loc, new_idx );
        }
    }
    return( ins );
}


static  void    AddBaseIndex( instruction * ins, name * index ) {
/****************************************************************
    When we have an N_INDEXED name "index" in "ins" of the form X[y],
    where X is N_MEMORY or N_TEMP, we need to change it into an
    LEA X => temp     if you just  use addr const load then need
    code for AUX & rent
    ADD temp+y =>temp
    and use [temp] instead, since the 370 cannot handle
    complex addressing modes.
*/

    name                *temp;
    instruction         *new_ins;
    name                *new_idx;
    name                *iname;
    type_class_def      class;
    i_flags             flags;


    temp = AllocTemp( WD );
    new_ins = MakeUnary( OP_LA, index->i.base, temp, WD );
    PrefixIns( ins, new_ins );
    iname = index->i.index;
    class = iname->n.name_class;
    new_idx = AllocTemp( class );
    new_ins = MakeBinary( OP_ADD, temp, iname, new_idx, PT );
    PrefixIns( ins, new_ins );
    flags = index->i.index_flags;
    flags |= X_FAKE_BASE;
    new_idx = ScaleIndex(  new_idx, index->i.base,
                           index->i.constant, index->n.name_class,
                           index->n.size, index->i.scale,
                           flags );
    ReplaceOperand( ins, index, new_idx );
}


extern  bool    IndexOkay( instruction *ins, name *index ) {
/***********************************************************
    return TRUE if "index" needs to be split out of instruction and a
    short lived temporary used instead.
*/

    name                *name;
    conflict_node       *conf;

    name = index->i.index;
    if( name->n.class == N_REGISTER ) {
        return( IsIndexReg( name->r.reg, name->n.name_class, 0 ) );
    }
    if( name->v.conflict == NULL ) return( FALSE );
    if( name->v.usage & USE_MEMORY ) return( FALSE );
    if( name->n.class != N_TEMP ) return( FALSE );
    conf = NameConflict( ins, name );
    if( conf == NULL ) return( FALSE );
    if( _Is( conf, NEEDS_INDEX_SPLIT ) ) {
        _SetFalse( conf, NEEDS_INDEX );
        return( FALSE );
    } else {
        _SetTrue( conf, NEEDS_INDEX );
        ins->head.state = OPERANDS_NEED_WORK;
        ins->t.index_needs = MarkIndex( ins, name, 0 );
        return( TRUE );
    }
}


static  instruction     *NoMemRefs( instruction *ins ) {
/*******************************************************
    Get rid of all direct memory references in "ins" since the 370 can't
    handle them.  We need to load =A(x) or =V(x) or use the global
    register (10) instead.
*/

    int         i;
    name        *mem_loc;
    instruction *last_ins;

    last_ins = ins;
    for( ;; ) {
        i = ins->num_operands;
        mem_loc = NULL;
        while( --i >= 0 ) {
            if( ins->operands[ i ]->n.class == N_MEMORY ) {
                mem_loc = ins->operands[ i ];
            }
        }
        if( ins->result != NULL ) {
            if( ins->result->n.class == N_MEMORY ) {
                mem_loc = ins->result;
            }
        }
        if( mem_loc == NULL ) break;
        ins = MemToIndex( ins, mem_loc );
        if( ins == NULL ) break;
        last_ins = ins;
    }
    return( last_ins );
}


extern void FixCallIns( instruction *ins ) {
/*******************************************
    Make the L  LN,=A(routine) explicit for all call instructions so that
    the constant can get cached in registers.
*/
    char        ra,ln;
    name        *routine;
    name        *reg;
    name        *used;
    hw_reg_set  tmp;
    instruction *new_ins;

    if( ins->head.opcode != OP_CALL ) return;
    routine = ins->operands[CALL_OP_ADDR];
    GetRALN( routine, &ra, &ln );
    reg = AllocRegName( WordReg( ln ) );
    new_ins = MakeUnary( OP_LA, routine, reg, WD );
    HW_Asgn( tmp, ins->operands[CALL_OP_USED]->r.reg );
    HW_TurnOn( tmp, reg->r.reg );
    used = AllocRegName( tmp );
    ins->operands[CALL_OP_USED] = used;
    ins->operands[CALL_OP_USED2] = used;
    PrefixIns( ins, new_ins );
}


static  void    NoMemBase( instruction *ins ) {
/**********************************************
    Make sure that no N_INDEXED names has a base field, since cannot be
    generated using 370 addressing modes.
*/

    int         i;
    name        *bad_index;
    name        *op;

    if( ins->head.opcode == OP_SELECT ) return;
    for( ;; ) {
        i = ins->num_operands;
        bad_index = NULL;
        while( --i >= 0 ) {
            op = ins->operands[ i ];
            if( op->n.class == N_INDEXED ) {
                if( HasTrueBase( op ) ) {
                    bad_index = op;
                }
            }
        }
        op = ins->result;
        if( op != NULL ) {
            if( op->n.class == N_INDEXED ) {
                if( HasTrueBase( op ) ) {
                    bad_index = op;
                }
            }
        }
        if( bad_index == NULL ) break;
        (void)AddBaseIndex( ins, bad_index );
    }
}


extern  void    FixSegments() {
/******************************
    stub for 8086 version
*/

}


extern  void    FixMemRefs() {
/*****************************
    Make sure that all memory references are broken up into things that
    we can handle using 370 addressing modes.
*/
    block       *blk;
    instruction *ins;

    blk = HeadBlock;
    while( blk != NULL ) {
        ins = blk->ins.hd.next;
        while( ins->head.opcode != OP_BLOCK ) {
            NoMemIndex( ins );
            ins = ins->head.next;
        }
        blk = blk->next_block;
    }
    blk = HeadBlock;
    while( blk != NULL ) {
        ins = blk->ins.hd.next;
        while( ins->head.opcode != OP_BLOCK ) {
            NoMemBase( ins );
            ins = ins->head.next;
        }
        blk = blk->next_block;
    }
    blk = HeadBlock;
    while( blk != NULL ) {
        ins = blk->ins.hd.next;
        while( ins->head.opcode != OP_BLOCK ) {
            FixCallIns( ins );
            ins = ins->head.next;
        }
        blk = blk->next_block;
    }
    TempStrategy();
}


extern  void    FixChoices() {
/*****************************
    stub for 8086 version.
*/
}


extern  bool    LoadAToMove( instruction *ins ) {
/************************************************
    Turn a load address instruction into MOV    =A(X) => foo
*/

    name        *op;

    if( ins->head.opcode != OP_LA && ins->head.opcode != OP_CAREFUL_LA ) {
        return( FALSE );
    }
    op = ins->operands[ 0 ];
    if( op->n.class != N_MEMORY ) return( FALSE );
    ins->head.opcode = OP_MOV;
    ins->operands[ 0 ] = AllocAddrConst( op, 0, CONS_ADDRESS, WD );
    return( TRUE );
}


extern  void    FixFPConsts( instruction *ins ) {
/************************************************
    stub for 8087
*/

    ins = ins;
}

