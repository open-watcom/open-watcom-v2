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
* Description:  MIPS specific instruction reductions.
*
****************************************************************************/


#include "standard.h"
#include "coderep.h"
#include "opcodes.h"
#include "vergen.h"
#include "tables.h"
#include "pattern.h"
#include "rtclass.h"
#include "zoiks.h"
#include "model.h"
#include "procdef.h"
#include "makeins.h"
#include <assert.h>
#include "rtrtn.h"

extern  name            *AllocMemory( pointer, type_length, cg_class, type_class_def );
extern  name            *AllocIndex( name *, name *, type_length, type_class_def );
extern  name            *AllocS32Const( signed_32 );
extern  name            *AllocRegName( hw_reg_set );
extern  name            *AllocTemp( type_class_def );
extern  name            *AllocAddrConst( name *, int, constant_class, type_class_def );
extern  name            *ScaleIndex( name *, name *, type_length, type_class_def, type_length, int, i_flags );
extern  name            *STempOffset( name *, type_length, type_class_def, type_length );

extern  hw_reg_set      StackReg( void );
extern  hw_reg_set      ScratchReg( void );
extern  hw_reg_set      ReturnAddrReg( void );

extern  void            SuffixIns( instruction *, instruction * );
extern  void            PrefixIns( instruction *, instruction * );
extern  void            ReplIns( instruction *, instruction * );
extern  void            ChangeType( instruction *, type_class_def );

extern  instruction     *rSWAPCMP( instruction * );

extern  void            UpdateLive( instruction *, instruction * );
extern  name            *OffsetMem( name *, type_length, type_class_def );

extern  opcode_entry    *OpcodeTable( table_def );

extern  type_length     TypeClassSize[];
extern  type_class_def  Unsigned[];
extern  type_length     MaxStack;
extern  proc_def        *CurrProc;


extern  instruction *rMOVEXX_8( instruction *ins )
/************************************************/
{
    name        *bit_mask;
    name        *temp;
    name        *temp_2;
    name        *src;
    name        *dst;
    type_length curr;           // which quad word we are currently on
    type_length size;           // size of the structure
    type_length quads;          // number of full quad-word writes to use
    type_length rem;            // number of bytes to write after last quad
    instruction *new_ins;
    instruction *first_ins;
    instruction *last_ins;

    /*
     * Bust up a MOVXX into a series of 8-byte moves - we are guaranteed
     * that both the source and dest are 8-byte aligned.
     */
    assert( ins->operands[0]->n.class == N_TEMP || ins->operands[0]->n.class == N_INDEXED );
    temp = AllocTemp( U8 );
    size = ins->operands[0]->n.size;
    first_ins = NULL;
    last_ins = NULL;
    curr = 0;
    quads = size / 8;
    rem = size % 8;
    if( rem ) {
        if( ins->result->n.class == N_TEMP ) {
            if( ( ins->result->t.temp_flags & ALIAS ) == EMPTY ) {
                // we have a write to a struct on the stack which is a master
                // since we don't 'pack' anything into the empty space after
                // this struct, we can safely overwrite it and not bother
                // doing the non-destructive last quad-word write
                quads += 1;
                rem = 0;
            }
        }
    }
    while( quads ) {
        src = OffsetMem( ins->operands[0], curr, U8 );
        dst = OffsetMem( ins->result, curr, U8 );
        curr += 8;
        quads -= 1;
        new_ins = MakeMove( src, temp, U8 );
        PrefixIns( ins, new_ins );
        if( first_ins == NULL ) {
            first_ins = new_ins;
        }
        new_ins = MakeMove( temp, dst, U8 );
        if( quads == 0 && rem == 0 ) {
            ReplIns( ins, new_ins );
            last_ins = new_ins;
        } else {
            PrefixIns( ins, new_ins );
        }
    }
    if( rem != 0 ) {
        if( rem == 4 ) {
            src = OffsetMem( ins->operands[0], curr, U4 );
            dst = OffsetMem( ins->result, curr, U4 );
            temp_2 = AllocTemp( U4 );
            new_ins = MakeMove( src, temp_2, U4 );
            PrefixIns( ins, new_ins );
            if( first_ins == NULL ) {
                first_ins = new_ins;
            }
            new_ins = MakeMove( temp_2, dst, U4 );
            ReplIns( ins, new_ins );
            last_ins = new_ins;
        } else {
            src = OffsetMem( ins->operands[0], curr, U8 );
            dst = OffsetMem( ins->result, curr, U8 );
            temp_2 = AllocTemp( U8 );
            bit_mask = AllocS32Const( ( 1 << rem ) - 1 );
            new_ins = MakeMove( src, temp, U8 );
            PrefixIns( ins, new_ins );
            if( first_ins == NULL ) {
                first_ins = new_ins;
            }
            new_ins = MakeMove( dst, temp_2, U8 );
            PrefixIns( ins, new_ins );
            new_ins = MakeBinary( OP_ZAP_NOT, temp, bit_mask, temp, U8 );
            PrefixIns( ins, new_ins );
            new_ins = MakeBinary( OP_ZAP, temp_2, bit_mask, temp_2, U8 );
            PrefixIns( ins, new_ins );
            new_ins = MakeBinary( OP_OR, temp_2, temp, temp, U8 );
            PrefixIns( ins, new_ins );
            new_ins = MakeMove( temp, dst, U8 );
            ReplIns( ins, new_ins );
            last_ins = new_ins;
        }

    }
    UpdateLive( first_ins, last_ins );
    return( first_ins );
}


extern instruction *rCONSTLOAD( instruction *ins )
/************************************************/
{
    unsigned_32         low;
    unsigned_32         high;
    unsigned_32         c;
    name                *high_part;
    name                *temp;
    instruction         *first_ins;
    instruction         *new_ins;
    type_class_def      class;

    assert( ins->operands[0]->n.class == N_CONSTANT );
    assert( ins->operands[0]->c.const_type == CONS_ABSOLUTE );

    class = ins->type_class;
    c = ins->operands[0]->c.int_value;
    high = ( c >> 16 ) & 0xffff;
    low  = c & 0xffff;
    high_part = AllocAddrConst( NULL, high, CONS_HIGH_ADDR, class );
    temp = AllocTemp( class );
    first_ins = MakeMove( high_part, temp, class );
    PrefixIns( ins, first_ins );
    new_ins = MakeBinary( OP_OR, temp, AllocS32Const( low ), ins->result, class );
    ReplIns( ins, new_ins );
    UpdateLive( first_ins, new_ins );
    return( first_ins );
}


extern instruction *rLOAD_4U( instruction *ins )
/**********************************************/
{
    instruction         *first_ins;
    instruction         *new_ins;
    name                *mem_1;
    name                *mem_2;
    name                *temp;

    mem_1 = OffsetMem( ins->operands[0], 3, U4 );
    mem_2 = OffsetMem( ins->operands[0], 0, U4 );
    temp = AllocTemp( U4 );
    first_ins = MakeUnary( OP_LOAD_UNALIGNED, mem_1, temp, U4 );
    PrefixIns( ins, first_ins );
    new_ins = MakeUnary( OP_LOAD_UNALIGNED, mem_2, temp, U4 );
    PrefixIns( ins, new_ins );
    new_ins = MakeMove( temp, ins->result, U4 );
    ReplIns( ins, new_ins );
    UpdateLive( first_ins, new_ins );
    return( first_ins );
}


static instruction *CheapCall( instruction *ins, rt_class rt_call, name *p1, name *p2 )
/*************************************************************************************/
{
    instruction         *call;
    hw_reg_set          reg;
    name                *reg_name;
    label_handle        lbl;

    reg = HW_EMPTY;
    lbl = RTLabel( rt_call );
    call = NewIns( 3 );
    call->head.opcode = OP_CALL;
    call->type_class = WD;
    call->operands[CALL_OP_USED] = p1;
    call->operands[CALL_OP_USED2] = p2;
    call->operands[CALL_OP_ADDR]= AllocMemory( lbl, 0, CG_LBL, WD );
    call->result = NULL;
    call->num_operands = 2;     /* special case for OP_CALL*/
    HW_TurnOn( reg, ReturnAddrReg() );
    HW_TurnOn( reg, ScratchReg() );
    // TODO: these regs are most likely wrong for MIPS
    HW_CTurnOn( reg, HW_R1 );   // know this is only other reg modified!
    HW_CTurnOn( reg, HW_R2 );   // and this one two!
    HW_CTurnOn( reg, HW_R3 );   // and this one three!
    reg_name = AllocRegName( reg );
    call->zap = &reg_name->r;
    PrefixIns( ins, call );
    return( call );
}


static void CopyStack( instruction *ins, name *alloc_size, type_length arg_size )
/*******************************************************************************/
{
    instruction         *new_ins;
    name                *p1;
    name                *p2;

    p1 = AllocRegName( HW_D1 );
    new_ins = MakeMove( alloc_size, p1, WD );
    PrefixIns( ins, new_ins );
    p2 = AllocRegName( HW_D2 );
    new_ins = MakeMove( AllocS32Const( arg_size ), p2, WD );
    PrefixIns( ins, new_ins );
    CheapCall( ins, RT_STK_COPY, p1, p2 );
}


extern instruction *rALLOCA( instruction *ins )
/*********************************************/
{
    name                *sreg;
    name                *amount;
    name                *real_amount;
    name                *temp;
    unsigned_32         value;
    instruction         *first;
    instruction         *last;
    type_class_def      class;
    unsigned_32         stack_align;
    bool                check;

    sreg = AllocRegName( StackReg() );
    amount = ins->operands[0];
    temp = AllocTemp( ins->type_class );
    class = WD;
    stack_align = STACK_ALIGNMENT;
    check = TRUE;
    CurrProc->targ.base_is_fp = TRUE;
    if( amount->n.class == N_CONSTANT && amount->c.const_type == CONS_ABSOLUTE ) {
        value = amount->c.int_value;
        value = _RoundUp( value, stack_align );
        real_amount = AllocS32Const( value );
        first = MakeBinary( OP_SUB, sreg, AllocS32Const( value ), temp, class );
        PrefixIns( ins, first );
        if( value <= (PAGE_SIZE - 7) ) {
            check = FALSE;
        }
    } else {
        real_amount = AllocTemp( ins->type_class );
        first = MakeBinary( OP_ADD, amount, AllocS32Const( stack_align - 1 ), temp, class );
        PrefixIns( ins, first );
        last = MakeBinary( OP_AND, temp, AllocS32Const( ~(stack_align - 1) ), real_amount, class );
        PrefixIns( ins, last );
        last = MakeBinary( OP_SUB, sreg, real_amount, temp, class );
        PrefixIns( ins, last );
    }
    last = MakeMove( temp, sreg, WD );
    PrefixIns( ins, last );
    if( check ) {
        CheapCall( ins, RT_STK_CRAWL, AllocRegName( HW_EMPTY ), AllocRegName( HW_EMPTY ) );
    }
    if( MaxStack != 0 ) {
        if( _IsModel( MICROSOFT_COMPATIBLE ) ) {
            // in order to support doing alloca's in parm lists, we copy
            // the parm cache area down now
            CopyStack( ins, real_amount, MaxStack );

        }
        last = MakeBinary( OP_ADD, temp, AllocS32Const( MaxStack ), ins->result, class );
    } else {
        last = MakeMove( temp, ins->result, class );
    }
    ReplIns( ins, last );
    UpdateLive( first, last );
    return( first );
}


/* MIPS is a little weird - it only has 'set' instruction for
 * 'less than' (reg/reg and reg/imm variants). Conditional branch
 * instructions can't take an immediate operand (except $zero of course)
 * and there isn't a full set of reg/reg conditional branches. So we
 * have to carefully reduce the instructions here...
 * NB: This is a clone of rSIMPCMP from rscsplit.c
 */
extern instruction      *rM_SIMPCMP( instruction *ins )
/******************************************************/
{
    instruction         *new;
    opcode_defs         opcode;
    bool                reverse;

    reverse = FALSE;
    assert( ins->result == NULL );
    switch( ins->head.opcode ) {
    case OP_CMP_NOT_EQUAL:
        reverse = TRUE;
        /* fall through */
    case OP_CMP_EQUAL:
        opcode = OP_SET_EQUAL;
        break;
    case OP_CMP_GREATER:
        reverse = TRUE;
        /* fall through */
    case OP_CMP_LESS_EQUAL:
        opcode = OP_SET_LESS_EQUAL;
        /* Special reduction: use OP_SET_LESS but increment constant */
        if( (ins->operands[1]->n.class == N_CONSTANT)
            && (ins->operands[1]->c.const_type == CONS_ABSOLUTE) ) {
            signed_32           value;

            opcode = OP_SET_LESS;
            // TODO: we may be leaking memory here by losing track of the
            // original constant operand
            value = ins->operands[1]->c.int_value;
            ins->operands[1] = AllocS32Const( value + 1 );
        }
        if( ins->operands[1]->n.class == N_REGISTER ) {
            // Swap operands and reverse condition - we can do slt/sltu but
            // nothing else
            return( rSWAPCMP( ins ) );
        }
        break;
    case OP_CMP_GREATER_EQUAL:
        reverse = TRUE;
        /* fall through */
    case OP_CMP_LESS:
        opcode = OP_SET_LESS;
        break;
    }
    ins->result = AllocTemp( ins->type_class );
    switch( ins->type_class ) {
    case I4:
    case U4:
        ins->table = OpcodeTable( BIN4 );
        break;
    case I8:
    case U8:
        ins->table = OpcodeTable( BIN8 );
        break;
    case FS:
        ins->table = OpcodeTable( FBINS );
        break;
    case FD:
    case FL:
        ins->table = OpcodeTable( FBIND );
        break;
    default:
        _Zoiks( ZOIKS_096 );
    }
    ins->head.opcode = opcode;
    opcode = OP_CMP_NOT_EQUAL;
    if( reverse ) {
        opcode = OP_CMP_EQUAL;
    }
    new = MakeCondition( opcode, ins->result, AllocS32Const( 0 ), _TrueIndex( ins ), _FalseIndex( ins ), ins->type_class );
    SuffixIns( ins, new );
    return( new );
}
