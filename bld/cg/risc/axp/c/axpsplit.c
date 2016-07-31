/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Alpha AXP specific instruction splitting.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "tables.h"
#include "zoiks.h"
#include "data.h"
#include "makeins.h"
#include "rtrtn.h"
#include "namelist.h"
#include "rscconst.h"
#include "rgtbl.h"
#include "split.h"
#include "insutil.h"


extern  void            UpdateLive( instruction *, instruction * );
extern  name            *OffsetMem( name *, type_length, type_class_def );

extern  instruction     *rLOAD_1( instruction *ins ) {
/****************************************************/

    name                *temp_1;
    name                *temp_2;
    instruction         *new_ins;
    instruction         *first_ins;
    name                *mem;

    // assert( ins->operands[0]->n.class == N_INDEXED );
    assert( ins->result->n.class == N_REGISTER );
    assert( ins->type_class == I1 || ins->type_class == U1 );

    mem = OffsetMem( ins->operands[0], 0, U8 );
    temp_2 = AllocTemp( U8 );
    temp_1 = AllocTemp( I8 );
    // ldq_u x(rn) -> temp_1
    // lda x(rn) -> temp_2
    // extbl temp_1, temp_2 -> r1
    first_ins = MakeUnary( OP_LOAD_UNALIGNED, mem, temp_1, I8 );
    PrefixIns( ins, first_ins );
    new_ins = MakeUnary( OP_LA, mem, temp_2, U8 );
    PrefixIns( ins, new_ins );
    new_ins = MakeNary( OP_EXTRACT_LOW, temp_1, temp_2, temp_1, I8, U1, 2 );
    PrefixIns( ins, new_ins );
    new_ins = MakeConvert( temp_1, ins->result, ins->type_class, I8 );
    ReplIns( ins, new_ins );
    UpdateLive( first_ins, new_ins );
    return( first_ins );
}

extern  instruction     *rSTORE_1( instruction *ins ) {
/*****************************************************/

    name                *temp_1;
    name                *temp_2;
    name                *temp_3;
    name                *index;
    name                *byte_loc;
    instruction         *first_ins;
    instruction         *new_ins;

    // assert( ins->result->n.class == N_INDEXED );
    assert( ins->operands[0]->n.class == N_REGISTER );
    assert( ins->type_class == I1 || ins->type_class == U1 );

    temp_1 = AllocTemp( U8 );
    temp_2 = AllocTemp( U8 );
    temp_3 = AllocTemp( U8 );
    index = OffsetMem( ins->result, 0, U8 );
    byte_loc = ins->operands[0];
    // expansion for mov r1 -> x(r2) is:
    // lda x(r2) -> temp_1
    // ldq_u x(r2) -> temp_2
    // insbl r1, temp_1 -> temp_3
    // mskbl temp_2, temp_1 -> temp_2
    // or temp_2, temp_3 -> temp_2
    // stq_u temp_2 -> x(r2)
    first_ins = MakeUnary( OP_LA, index, temp_1, U8 );
    PrefixIns( ins, first_ins );
    new_ins = MakeConvert( byte_loc, temp_3, U8, ins->type_class );
    PrefixIns( ins, new_ins );
    new_ins = MakeUnary( OP_LOAD_UNALIGNED, index, temp_2, U8 );
    PrefixIns( ins, new_ins );
    new_ins = MakeNary( OP_INSERT_LOW, temp_3, temp_1, temp_3, U8, U1, 2 );
    PrefixIns( ins, new_ins );
    new_ins = MakeNary( OP_MASK_LOW, temp_2, temp_1, temp_2, U8, U1, 2 );
    PrefixIns( ins, new_ins );
    new_ins = MakeBinary( OP_OR, temp_2, temp_3, temp_2, U8 );
    PrefixIns( ins, new_ins );
    new_ins = MakeUnary( OP_STORE_UNALIGNED, temp_2, index, U8 );
    ReplIns( ins, new_ins );
    UpdateLive( first_ins, new_ins );
    return( first_ins );
}

extern  instruction     *rLOAD_2( instruction *ins ) {
/****************************************************/

    instruction         *first_ins;
    instruction         *new_ins;
    name                *temp;
    name                *mem;
    // name             *new_mem;
    name                *result;

    assert( ins->result->n.class == N_REGISTER );
    assert( ins->operands[0]->n.class == N_INDEXED || ins->operands[0]->n.class == N_TEMP );
    // FIXME: This assumes shorts are two-byte aligned
    // Microsoft appears to be using some evil alternate scheme.
    mem = OffsetMem( ins->operands[0], 0, U8 );
    result = AllocTemp( U8 );
    temp = AllocTemp( U8 );
    // mov U2 k(rn) -> rm expands to:
    // ldq_u    k(rn) -> rm
    // lea              k(rn) -> temp
    // extwl    rm, temp, rm
    first_ins = MakeUnary( OP_LOAD_UNALIGNED, mem, result, U8 );
    PrefixIns( ins, first_ins );
    new_ins = MakeUnary( OP_LA, mem, temp, U8 );
    PrefixIns( ins, new_ins );
    new_ins = MakeNary( OP_EXTRACT_LOW, result, temp, result, U8, U2, 2 );
    PrefixIns( ins, new_ins );
    new_ins = MakeConvert( result, ins->result, ins->type_class, U8 );
    ReplIns( ins, new_ins );
    UpdateLive( first_ins, new_ins );
    return( first_ins );
}

extern  instruction     *rLOAD_2U( instruction *ins ) {
/*****************************************************/

    name                *mem;
    name                *new_mem;
    name                *result;
    name                *temp;
    name                *addr;
    instruction         *first_ins;
    instruction         *new_ins;

    mem = OffsetMem( ins->operands[0], 0, U8 );
    new_mem = OffsetMem( ins->operands[0], 1, U8 );
    result = AllocTemp( U8 );
    temp = AllocTemp( U8 );
    addr = AllocTemp( U8 );
    first_ins = MakeUnary( OP_LOAD_UNALIGNED, mem, result, U8 );
    PrefixIns( ins, first_ins );
    new_ins = MakeUnary( OP_LOAD_UNALIGNED, new_mem, temp, U8 );
    PrefixIns( ins, new_ins );
    new_ins = MakeUnary( OP_LA, mem, addr, U8 );
    PrefixIns( ins, new_ins );
    new_ins = MakeNary( OP_EXTRACT_LOW, result, addr, result, U8, U2, 2 );
    PrefixIns( ins, new_ins );
    new_ins = MakeNary( OP_EXTRACT_HIGH, temp, addr, temp, U8, U2, 2 );
    PrefixIns( ins, new_ins );
    new_ins = MakeBinary( OP_OR, temp, result, result, U8 );
    PrefixIns( ins, new_ins );
    new_ins = MakeConvert( result, ins->result, ins->type_class, U8 );
    ReplIns( ins, new_ins );
    UpdateLive( first_ins, new_ins );
    return( first_ins );
}

extern  instruction     *rSTORE_2( instruction *ins ) {
/*****************************************************/

    name                *temp_1;
    name                *temp_2;
    name                *addr;
    name                *index;
    name                *src;
    instruction         *first_ins;
    instruction         *new_ins;

    assert( ins->operands[0]->n.class == N_REGISTER );
    // assert( ins->result->n.class == N_INDEXED );
    assert( ins->type_class == U2 || ins->type_class == I2 );

    // mov rm -> x(rn) goes to:
    //          lea     x(rn) -> addr
    //          ldq_u   x(rn) -> temp_1
    //          inswl   rm, addr, temp_2
    //          mskwl   temp_1, addr, temp_1
    //          or      temp_1, temp_2, temp_1
    //          stq_u   temp_1, x(rn)
    temp_1 = AllocTemp( U8 );
    temp_2 = AllocTemp( U8 );
    addr = AllocTemp( U8 );
    index = OffsetMem( ins->result, 0, U8 );
    src = ins->operands[0];
    first_ins = MakeUnary( OP_LA, index, addr, U8 );
    PrefixIns( ins, first_ins );
    new_ins = MakeUnary( OP_LOAD_UNALIGNED, index, temp_1, U8 );
    PrefixIns( ins, new_ins );
    new_ins = MakeConvert( src, temp_2, U8, ins->type_class );
    PrefixIns( ins, new_ins );
    new_ins = MakeNary( OP_INSERT_LOW, temp_2, addr, temp_2, U8, U2, 2 );
    PrefixIns( ins, new_ins );
    new_ins = MakeNary( OP_MASK_LOW, temp_1, addr, temp_1, U8, U2, 2 );
    PrefixIns( ins, new_ins );
    new_ins = MakeBinary( OP_OR, temp_1, temp_2, temp_1, U8 );
    PrefixIns( ins, new_ins );
    new_ins = MakeUnary( OP_STORE_UNALIGNED, temp_1, index, U8 );
    ReplIns( ins, new_ins );
    UpdateLive( first_ins, new_ins );
    return( first_ins );
}

extern  instruction     *rSTORE_2U( instruction *ins ) {
/******************************************************/

    name                *mem;
    name                *new_mem;
    name                *result;
    name                *temp;
    name                *value;
    name                *low;
    name                *high;
    name                *addr;
    instruction         *first_ins;
    instruction         *new_ins;


    mem = OffsetMem( ins->result, 0, U8 );
    new_mem = OffsetMem( ins->result, 1, U8 );
    addr = AllocTemp( U8 );
    result = AllocTemp( U8 );
    temp = AllocTemp( U8 );
    high = AllocTemp( U8 );
    low = AllocTemp( U8 );
    value = AllocTemp( U8 );
    first_ins = MakeConvert( ins->operands[0], value, U8, ins->type_class );
    PrefixIns( ins, first_ins );
    new_ins = MakeUnary( OP_LA, mem, addr, U8 );
    PrefixIns( ins, new_ins );
    new_ins = MakeUnary( OP_LOAD_UNALIGNED, new_mem, high, U8 );
    PrefixIns( ins, new_ins );
    new_ins = MakeUnary( OP_LOAD_UNALIGNED, mem, low, U8 );
    PrefixIns( ins, new_ins );
    new_ins = MakeNary( OP_INSERT_HIGH, value, addr, temp, U8, U2, 2 );
    PrefixIns( ins, new_ins );
    new_ins = MakeNary( OP_INSERT_LOW, value, addr, result, U8, U2, 2 );
    PrefixIns( ins, new_ins );
    new_ins = MakeNary( OP_MASK_HIGH, high, addr, high, U8, U2, 2 );
    PrefixIns( ins, new_ins );
    new_ins = MakeNary( OP_MASK_LOW, low, addr, low, U8, U2, 2 );
    PrefixIns( ins, new_ins );
    new_ins = MakeBinary( OP_OR, high, temp, high, U8 );
    PrefixIns( ins, new_ins );
    new_ins = MakeBinary( OP_OR, low, result, low, U8 );
    PrefixIns( ins, new_ins );
    new_ins = MakeUnary( OP_STORE_UNALIGNED, high, new_mem, U8 );
    PrefixIns( ins, new_ins );
    new_ins = MakeUnary( OP_STORE_UNALIGNED, low, mem, U8 );
    ReplIns( ins, new_ins );
    UpdateLive( first_ins, new_ins );
    return( first_ins );
}

extern  instruction     *rLOAD_4U( instruction *ins ) {
/*****************************************************/

    instruction         *first_ins;
    instruction         *new_ins;
    name                *mem_1;
    name                *mem_2;
    name                *temp_1;
    name                *temp_2;
    name                *temp_3;

    mem_1 = OffsetMem( ins->operands[0], 0, U8 );
    mem_2 = OffsetMem( ins->operands[0], 3, U8 );
    temp_1 = AllocTemp( U8 );
    temp_2 = AllocTemp( U8 );
    temp_3 = AllocTemp( U8 );
    first_ins = MakeUnary( OP_LOAD_UNALIGNED, mem_1, temp_1, U8 );
    PrefixIns( ins, first_ins );
    new_ins = MakeUnary( OP_LOAD_UNALIGNED, mem_2, temp_2, U8 );
    PrefixIns( ins, new_ins );
    new_ins = MakeUnary( OP_LA, mem_1, temp_3, U8 );
    PrefixIns( ins, new_ins );
    new_ins = MakeNary( OP_EXTRACT_LOW, temp_1, temp_3, temp_1, U8, U4, 2 );
    PrefixIns( ins, new_ins );
    new_ins = MakeNary( OP_EXTRACT_HIGH, temp_2, temp_3, temp_2, U8, U4, 2 );
    PrefixIns( ins, new_ins );
    new_ins = MakeBinary( OP_OR, temp_1, temp_2, temp_1, U8 );
    PrefixIns( ins, new_ins );
    new_ins = MakeConvert( temp_1, ins->result, ins->type_class, U8 );
    ReplIns( ins, new_ins );
    UpdateLive( first_ins, new_ins );
    return( first_ins );
}

extern  instruction     *rSTORE_4U( instruction *ins ) {
/******************************************************/

    instruction         *first_ins;
    instruction         *new_ins;
    name                *high;
    name                *low;
    name                *temp_high;
    name                *temp_low;
    name                *addr;
    name                *mem_high;
    name                *mem_low;
    name                *value;

    mem_low =  OffsetMem( ins->result, 0, U8 );
    mem_high = OffsetMem( ins->result, 3, U8 );
    value = AllocTemp( U8 );
    addr = AllocTemp( U8 );
    high = AllocTemp( U8 );
    low = AllocTemp( U8 );
    temp_high = AllocTemp( U8 );
    temp_low = AllocTemp( U8 );
    first_ins = MakeConvert( ins->operands[0], value, U8, ins->type_class );
    PrefixIns( ins, first_ins );
    new_ins = MakeUnary( OP_LA, mem_low, addr, U8 );
    PrefixIns( ins, new_ins );
    new_ins = MakeUnary( OP_LOAD_UNALIGNED, mem_high, high, U8 );
    PrefixIns( ins, new_ins );
    new_ins = MakeUnary( OP_LOAD_UNALIGNED, mem_low, low, U8 );
    PrefixIns( ins, new_ins );
    new_ins = MakeNary( OP_INSERT_HIGH, value, addr, temp_high, U8, U4, 2 );
    PrefixIns( ins, new_ins );
    new_ins = MakeNary( OP_INSERT_LOW, value, addr, temp_low, U8, U4, 2 );
    PrefixIns( ins, new_ins );
    new_ins = MakeNary( OP_MASK_HIGH, high, addr, high, U8, U4, 2 );
    PrefixIns( ins, new_ins );
    new_ins = MakeNary( OP_MASK_LOW, low, addr, low, U8, U4, 2 );
    PrefixIns( ins, new_ins );
    new_ins = MakeBinary( OP_OR, high, temp_high, high, U8 );
    PrefixIns( ins, new_ins );
    new_ins = MakeBinary( OP_OR, low, temp_low, low, U8 );
    PrefixIns( ins, new_ins );
    new_ins = MakeUnary( OP_STORE_UNALIGNED, high, mem_high, U8 );
    PrefixIns( ins, new_ins );
    new_ins = MakeUnary( OP_STORE_UNALIGNED, low, mem_low, U8 );
    ReplIns( ins, new_ins );
    UpdateLive( first_ins, new_ins );
    return( first_ins );
}

extern  instruction     *rLOAD_8U( instruction *ins ) {
/*****************************************************/

    _Zoiks( ZOIKS_091 );
    return( ins );
}

extern  instruction     *rSTORE_8U( instruction *ins ) {
/******************************************************/

    _Zoiks( ZOIKS_091 );
    return( ins );
}

extern  instruction     *rMOVEXX_8( instruction *ins ) {
/******************************************************/

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

#if 0
extern instruction      *rCONSTLOAD( instruction *ins ) {
/*******************************************************/
    signed_16           high;
    signed_16           low;
    signed_16           extra;

    assert( ins->operands[0]->n.class == N_CONSTANT );
    assert( ins->operands[0]->c.const_type == CONS_ABSOLUTE );

    first = NULL;
    FactorInt32( ins->operands[0]->c.lo.int_value, &high, &extra, &low );
    // work to be done here - need some way of accurately representing
    // the ldah rn,extra(rn) instruction
}
#else
extern instruction      *rCONSTLOAD( instruction *ins ) {
/*******************************************************/

    instruction         *new_ins;
    instruction         *first_ins;
    name                *high_part;
    name                *low_part;
    name                *temp;
    name                *cons;
    unsigned_32         high;
    unsigned_32         low;
    unsigned_32         k;
    unsigned_32         c;
    type_class_def      index_class;
    bool                cruft_in_high_dword;

    assert( ins->operands[0]->n.class == N_CONSTANT );
    assert( ins->operands[0]->c.const_type == CONS_ABSOLUTE );

    cons = ins->operands[0];
    c = cons->c.lo.uint_value;
    k = 65536U;
    high = c / k;
    low = c % k;
    if( low >= ( k / 2 ) ) {
        high += 1;
        low -= k;
    }
    assert( ( (signed_16)high * k + (signed_16)low ) == c );
    high_part = AllocAddrConst( NULL, high, CONS_HIGH_ADDR, ins->type_class );
    if( low == 0 ) {
        first_ins = MakeMove( high_part, ins->result, ins->type_class );
        ReplIns( ins, first_ins );
        if( ( c & 0x80000000 ) &&
            ( first_ins->type_class == Unsigned[first_ins->type_class] ) ) {
            new_ins = MakeBinary( OP_ZAP_NOT, first_ins->result, AllocS32Const( 0x0f ), first_ins->result, first_ins->type_class );
            SuffixIns( first_ins, new_ins );
            UpdateLive( first_ins, new_ins );
        }
    } else {
        index_class = I4;
        temp = AllocTemp( index_class );
        first_ins = MakeMove( high_part, temp, index_class );
        PrefixIns( ins, first_ins );
        low_part = AllocIndex( temp, NULL, low, ins->type_class );
        new_ins = MakeUnary( OP_LA, low_part, ins->result, ins->type_class );
        PrefixIns( ins, new_ins );
        cruft_in_high_dword = false;
        if( c >= 0x7fff8000 && c <= 0x7fffffff ) {
            cruft_in_high_dword = true;
        }
        if( ( c & 0x80000000 ) &&
            ( ins->type_class == Unsigned[ins->type_class] ) ) {
            cruft_in_high_dword = true;
        }
        if( cruft_in_high_dword ) {
            new_ins = MakeBinary( OP_ZAP_NOT, ins->result, AllocS32Const( 0x0f ), ins->result, ins->type_class );
            PrefixIns( ins, new_ins );
        }
        FreeIns( ins );
        UpdateLive( first_ins, new_ins );
    }
    return( first_ins );
}
#endif

static instruction *CheapCall( instruction *ins, rt_class rtindex, name *p1, name *p2 ) {
/***************************************************************************************/

    instruction         *call;
    hw_reg_set          reg;
    name                *reg_name;
    label_handle        lbl;

    reg = HW_EMPTY;
    lbl = RTLabel( rtindex );
    call = NewIns( 3 );
    call->head.opcode = OP_CALL;
    call->type_class = WD;
    call->operands[CALL_OP_USED] = p1;
    call->operands[CALL_OP_USED2] = p2;
    call->operands[CALL_OP_ADDR] = AllocMemory( lbl, 0, CG_LBL, WD );
    call->result = NULL;
    call->num_operands = 2;         /* special case for OP_CALL*/
    HW_TurnOn( reg, ReturnAddrReg() );
    HW_TurnOn( reg, ScratchReg() );
    HW_CTurnOn( reg, HW_R1 );   // know this is only other reg modified!
    HW_CTurnOn( reg, HW_R2 );   // and this one two!
    HW_CTurnOn( reg, HW_R3 );   // and this one three!
    reg_name = AllocRegName( reg );
    call->zap = &reg_name->r;
    PrefixIns( ins, call );
    return( call );
}

static void CopyStack( instruction *ins, name *alloc_size, type_length arg_size ) {
/*********************************************************************************/

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

extern instruction *rALLOCA( instruction *ins ) {
/***********************************************/

    name                *sreg;
    name                *amount;
    name                *real_amount;
    name                *temp;
    unsigned_32         value;
    instruction         *first;
    instruction         *last;
    type_class_def      class;
    bool                check;

    sreg = AllocRegName( StackReg() );
    amount = ins->operands[0];
    temp = AllocTemp( ins->type_class );
    class = WD;
    check = true;
    CurrProc->targ.base_is_fp = true;
    if( amount->n.class == N_CONSTANT && amount->c.const_type == CONS_ABSOLUTE ) {
        value = amount->c.lo.uint_value;
        value = _RoundUp( value, STACK_ALIGNMENT );
        real_amount = AllocS32Const( value );
        first = MakeBinary( OP_SUB, sreg, AllocS32Const( value ), temp, class );
        PrefixIns( ins, first );
        if( value <= ( _TARGET_PAGE_SIZE - 7 ) ) {
            check = false;
        }
    } else {
        real_amount = AllocTemp( ins->type_class );
        first = MakeBinary( OP_ADD, amount, AllocS32Const( STACK_ALIGNMENT - 1 ), temp, class );
        PrefixIns( ins, first );
        last = MakeBinary( OP_AND, temp, AllocU32Const( ~( STACK_ALIGNMENT - 1 ) ), real_amount, class );
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
