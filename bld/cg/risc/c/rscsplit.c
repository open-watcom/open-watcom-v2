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
* Description:  RISC instruction spliting reductions.
*
****************************************************************************/


#include "_cgstd.h"
#include "coderep.h"
#include "system.h"
#include "zoiks.h"
#include "cfloat.h"
#include "makeins.h"
#include "convins.h"
#include "data.h"
#include "rtrtn.h"
#include "namelist.h"
#include "rgtbl.h"
#include "split.h"
#include "insutil.h"
#include "rtcall.h"
#include "optab.h"
#include "optimize.h"
#include "objout.h"
#include "liveinfo.h"
#include "opctable.h"
#include "rscsplit.h"
#include "_split.h"
#include "_rscsplit.h"

#if 0
extern  name            *AddrConst( name *, int, constant_class );
#endif

instruction *(* ReduceTab[])() = {
    #define _R_( x, f )     f
    #include "r.h"
    #undef _R_
};

static instruction      *PromoteOperand( instruction *ins ) {
/***********************************************************/

    name                *temp;
    instruction         *new_ins;

    if( _IsFloating( ins->type_class ) ) {
        if( ins->type_class == FS ) {
            temp = AllocTemp( FD );
            new_ins = MakeConvert( ins->operands[0], temp, ins->type_class, FD );
            ins->operands[0] = temp;
            PrefixIns( ins, new_ins );
            UpdateLive( new_ins, ins );
            return( new_ins );
        }
    } else {
        if( ins->type_class < U8 ) {
            temp = AllocTemp( U8 );
            new_ins = MakeConvert( ins->operands[0], temp, ins->type_class, U8 );
            ins->operands[0] = temp;
            PrefixIns( ins, new_ins );
            UpdateLive( new_ins, ins );
            return( new_ins );
        }
    }
    return( NULL );
}

instruction     *rDOSET( instruction *ins )
/*****************************************/
{
    instruction         *new;

    switch( ins->head.opcode ) {
    case OP_SET_NOT_EQUAL:
        // FIXME: This is correct but stupid
        ins->head.opcode = OP_SET_EQUAL;
        new = MakeBinary( OP_SET_EQUAL, ins->result, AllocS32Const( 0 ), ins->result, ins->type_class );
        SuffixIns( ins, new );
        break;
    case OP_SET_GREATER:
        ins->head.opcode = OP_SET_LESS_EQUAL;
        rSWAPOPS( ins );
        break;
    case OP_SET_GREATER_EQUAL:
        ins->head.opcode = OP_SET_LESS;
        rSWAPOPS( ins );
        break;
    case OP_SET_LESS:
    case OP_SET_LESS_EQUAL:
    case OP_SET_EQUAL:
        break;
    default:
        _Zoiks( ZOIKS_120 );
    }
    switch( TypeClassSize[ins->type_class] ) {
    case 4:
        ins->table = OpcodeTable( BIN4 );
        break;
    case 8:
        ins->table = OpcodeTable( BIN8 );
        break;
    default:
        ins->table = OpcodeTable( BIN4 );
        _Zoiks( ZOIKS_120 );
    }
    return( ins );
}

instruction      *rSIMPCMP( instruction *ins )
/****************************************************
* Simplify a comparison instruction for a platform that can't perform
* arbitrary reg/reg comparisons.
*/
{
    instruction         *new;
    opcode_defs         opcode;
    bool                reverse;

    reverse = false;
    opcode = 0;
    assert( ins->result == NULL );
    switch( ins->head.opcode ) {
    case OP_CMP_NOT_EQUAL:
        reverse = true;
        /* fall through */
    case OP_CMP_EQUAL:
        opcode = OP_SET_EQUAL;
        break;
    case OP_CMP_GREATER:
        reverse = true;
        /* fall through */
    case OP_CMP_LESS_EQUAL:
        opcode = OP_SET_LESS_EQUAL;
        break;
    case OP_CMP_GREATER_EQUAL:
        reverse = true;
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

instruction      *rDOTEST( instruction *ins )
/*******************************************/
{
    instruction         *first;
    instruction         *last;
    name                *temp;
    opcode_defs         opcode;

    temp = AllocTemp( ins->type_class );
    switch( ins->head.opcode ) {
    case OP_BIT_TEST_TRUE:
        opcode = OP_CMP_NOT_EQUAL;
        break;
    case OP_BIT_TEST_FALSE:
        opcode = OP_CMP_EQUAL;
        break;
    default:
        opcode = OP_CMP_EQUAL;
        _Zoiks( ZOIKS_120 );
    }
    first = MakeBinary( OP_AND, ins->operands[0],
                                ins->operands[1], temp, ins->type_class );
    PrefixIns( ins, first );
    last = MakeCondition( opcode, temp, AllocS32Const( 0 ),
                                _TrueIndex( ins ), _FalseIndex( ins ), ins->type_class );
    ReplIns( ins, last );
    UpdateLive( first, last );
    return( first );
}

instruction      *rPUSHTOMOV( instruction *ins )
/**********************************************/
{
    instruction         *new_ins;
    instruction         *first_ins;
    instruction         *prom_ins;
    name                *stack_reg;
    type_class_def      push_class;

    push_class = U8;
    if( _IsFloating( ins->type_class ) ) {
        push_class = FD;
    }
    prom_ins = PromoteOperand( ins );
    stack_reg = AllocRegName( StackReg() );
    first_ins = MakeBinary( OP_SUB, stack_reg, AllocS32Const( TypeClassSize[push_class] ), stack_reg, WD );
    PrefixIns( ins, first_ins );
    new_ins = MakeMove( ins->operands[0], AllocIndex( stack_reg, NULL, 0, push_class ), push_class );
    ReplIns( ins, new_ins );
    first_ins = ( ( prom_ins == NULL ) ? first_ins : prom_ins );
    UpdateLive( first_ins, new_ins );
    return( first_ins );
}

instruction      *rPOPTOMOV( instruction *ins )
/*********************************************/
{
    instruction         *new_ins;
    instruction         *first_ins;
    instruction         *prom_ins;
    name                *stack_reg;
    type_class_def      push_class;

    push_class = U8;
    if( _IsFloating( ins->type_class ) ) {
        push_class = FD;
    }
    prom_ins = PromoteOperand( ins );
    stack_reg = AllocRegName( StackReg() );
    first_ins = MakeMove( AllocIndex( stack_reg, NULL, 0, push_class ), ins->result, WD );
    PrefixIns( ins, first_ins );
    new_ins = MakeBinary( OP_ADD, stack_reg, AllocS32Const( TypeClassSize[push_class] ), stack_reg, push_class );
    ReplIns( ins, new_ins );
    first_ins = ( ( prom_ins == NULL ) ? first_ins : prom_ins );
    UpdateLive( first_ins, new_ins );
    return( first_ins );
}

static  instruction     *LoadFPConst( instruction *ins, name **constant ) {
/*************************************************************************/

    name                *temp_1;
    name                *temp_2;
    name                *mem;
    instruction         *first_ins;
    instruction         *last_ins;

    temp_1 = AllocTemp( CP );
    temp_2 = AllocTemp( ins->type_class );
    mem = GenFloat( *constant, ins->type_class );
    first_ins = MakeUnary( OP_LA, mem, temp_1, WD );
    PrefixIns( ins, first_ins );
    last_ins = MakeMove( AllocIndex( temp_1, NULL, 0, ins->type_class ), temp_2, ins->type_class );
    PrefixIns( ins, last_ins );
    *constant = temp_2;
    UpdateLive( first_ins, ins );
    return( first_ins );
}

instruction     *rOP1CMEM( instruction *ins )
/*******************************************/
{
    ins = LoadFPConst( ins, &ins->operands[0] );
    return( ins );
}

instruction     *rOP2CMEM( instruction *ins )
/*******************************************/
{
    ins = LoadFPConst( ins, &ins->operands[1] );
    return( ins );
}

instruction     *rCHANGETYPE( instruction *ins )
/**********************************************/
{
    type_class_def              new;

    assert( ins->type_class == XX );
    switch( ins->operands[0]->n.size ) {
    case 1:
        new = U1;
        break;
    case 2:
        new = U2;
        break;
    case 4:
        new = U4;
        break;
    case 8:
        new = U8;
        break;
    default:
        new = U1;
        _Zoiks( ZOIKS_120 );
    }
    ChangeType( ins, new );
    return( ins );
}

static void     UseAddress( name *op ) {
/**************************************/

    name        *next;

    switch( op->n.class ) {
    case N_TEMP:
        next = op->t.alias;
        for(;;) {
            next->v.usage |= USE_ADDRESS;
            if( next == op )
                break;
            next = next->t.alias;
        }
        break;
    case N_MEMORY:
        op->v.usage |= USE_ADDRESS;
        break;
    }
}

instruction     *rMOVEXX( instruction *ins )
/******************************************/
{
    instruction         *first_ins;
    instruction         *new_ins;
    name                *len;
    name                *reg_name;
    label_handle        lbl;
    hw_reg_set          all_regs;
    hw_reg_set          reg;

    reg = FirstReg( RL_PARM_4 );
    all_regs = reg;
    reg_name = AllocRegName( reg );
    first_ins = MakeUnary( OP_LA, ins->result, reg_name, CP );
    PrefixIns( ins, first_ins );
    UseAddress( ins->result );
    reg = FirstReg( RL_PARM2_4 );
    reg_name = AllocRegName( reg );
    HW_TurnOn( all_regs, reg );
    new_ins = MakeUnary( OP_LA, ins->operands[0], reg_name, CP );
    PrefixIns( ins, new_ins );
    UseAddress( ins->operands[0] );
    len = AllocS32Const( ins->operands[0]->n.size );
    reg = FirstReg( RL_PARM3_4 );
    reg_name = AllocRegName( reg );
    HW_TurnOn( all_regs, reg );
    new_ins = MakeMove( len, reg_name, CP );
    PrefixIns( ins, new_ins );
    HW_TurnOn( all_regs, ReturnAddrReg() );
    reg_name = AllocRegName( all_regs );
    lbl = RTLabel( RT_MEMCPY );
    new_ins = NewIns( 3 );
    new_ins->head.opcode = OP_CALL;
    new_ins->type_class = ins->type_class;
    new_ins->operands[CALL_OP_USED] = reg_name;
    new_ins->operands[CALL_OP_USED2] = reg_name;
    new_ins->operands[CALL_OP_ADDR] = AllocMemory( lbl, 0, CG_LBL, ins->type_class );
    HW_CTurnOn( all_regs, HW_FULL );
    HW_TurnOff( all_regs, SavedRegs() );
    HW_CTurnOff( all_regs, HW_UNUSED );
    HW_TurnOn( all_regs, ReturnAddrReg() );
    reg_name = AllocRegName( all_regs );
    new_ins->result = NULL;
    new_ins->num_operands = 2;          /* special case for OP_CALL*/
    HW_CTurnOn( all_regs, HW_FULL );
    HW_TurnOff( all_regs, SavedRegs() );
    HW_CTurnOff( all_regs, HW_UNUSED );
    HW_TurnOn( all_regs, ReturnAddrReg() );
    new_ins->zap = &reg_name->r;        /* all parm regs could be zapped*/
    ReplIns( ins, new_ins );
    UpdateLive( first_ins, new_ins );
    return( first_ins );
}

name    *TrimConst( name *c, type_class_def tipe )
/************************************************/
{
    signed_32           value;

    switch( tipe ) {
    case U1:
        value = (unsigned_8)c->c.lo.int_value;
        break;
    case I1:
        value = (signed_8)c->c.lo.int_value;
        break;
    case U2:
        value = (unsigned_16)c->c.lo.int_value;
        break;
    case I2:
        value = (signed_16)c->c.lo.int_value;
        break;
    case U4:
    case I4:
        /* FIXME: int_value should be a 64-bit const for the Alpha
         * and we should be doing the same thing here.
         */
    default:
        return( c );
    }
    if( value != c->c.lo.int_value ) {
        c = AllocS32Const( value );
    }
    return( c );
}

static  instruction     *doPromote( instruction *ins, type_class_def tipe ) {
/***************************************************************************/

    instruction *op0_ins;
    instruction *op1_ins;
    instruction *last_ins;
    name        *t0;
    name        *t1;

    t0 = AllocTemp( tipe );
    if( ins->operands[0]->n.class == N_CONSTANT ) {
        op0_ins = NULL;
        ins->operands[0] = TrimConst( ins->operands[0], ins->type_class );
    } else {
        op0_ins = MakeConvert( ins->operands[0], t0, tipe, ins->type_class );
        ins->operands[0] = t0;
        PrefixIns( ins, op0_ins );
    }

    if( ins->operands[1]->n.class == N_CONSTANT ) {
        op1_ins = NULL;
        ins->operands[1] = TrimConst( ins->operands[1], ins->type_class );
    } else {
        t1 = AllocTemp( tipe );
        op1_ins = MakeConvert( ins->operands[1], t1, tipe, ins->type_class );
        ins->operands[1] = t1;
        PrefixIns( ins, op1_ins );
    }

    last_ins = ins;
    if( ins->result != NULL ) {
        last_ins = MakeConvert( t0, ins->result, ins->type_class, tipe );
        ins->result = t0;
        SuffixIns( ins, last_ins );
    }

    ChangeType( ins, tipe );

    if( op0_ins != NULL ) {
        UpdateLive( op0_ins, last_ins );
        return( op0_ins );
    }
    if( op1_ins != NULL ) {
        UpdateLive( op1_ins, last_ins );
        return( op1_ins );
    }
    return( ins );
}

instruction     *rBIN2INT( instruction *ins )
/*******************************************/
{
    type_class_def      new;

    new = I4;
    if( Unsigned[ins->type_class] == ins->type_class ) {
        new = U4;
    }
    return( doPromote( ins, new ) );
}

instruction     *rBIN2QUAD( instruction *ins )
/********************************************/
{
    type_class_def      new;

    new = I8;
    if( Unsigned[ins->type_class] == ins->type_class ) {
        new = U8;
    }

    return( doPromote( ins, new ) );
}

bool    UnChangeable( instruction *ins )
/**************************************/
{
    /* unused parameters */ (void)ins;

    return( false );
}

name    *Int64Equivalent( name *name )
/*********************************************
* Return a U64 equivalent of a double value
*/
{
    constant_defn       *defn;

    defn = GetFloat( name, FD );
    return( AllocU64Const( *(unsigned_32 *)( defn->value + 0 ), *(unsigned_32 *)( defn->value + 2 ) ) );
}

name    *LowPart( name *tosplit, type_class_def class )
/**************************************************************
* Return the low (of type 'class') part of name 'tosplit'
* Note: There may not be any need to support splitting to
* classes other than U4/I4 on RISC (assuming that registers are
* at least 32-bit).
*/
{
    name                *new;
    signed_8            s8;
    unsigned_8          u8;
    signed_16           s16;
    unsigned_16         u16;
    constant_defn       *floatval;

    new = NULL;
    switch( tosplit->n.class ) {
    case N_CONSTANT:
        if( tosplit->c.const_type == CONS_ABSOLUTE ) {
            if( class == U1 ) {
                u8 = tosplit->c.lo.int_value & 0xff;
                new = AllocUIntConst( u8 );
            } else if( class == I1 ) {
                s8 = tosplit->c.lo.int_value & 0xff;
                new = AllocIntConst( s8 );
            } else if( class == U2 ) {
                u16 = tosplit->c.lo.int_value & 0xffff;
                new = AllocUIntConst( u16 );
            } else if( class == I2 ) {
                s16 = tosplit->c.lo.int_value & 0xffff;
                new = AllocIntConst( s16 );
            } else if( class == I4 ) {
                new = AllocS32Const( tosplit->c.lo.int_value );
            } else if( class == U4 ) {
                new = AllocU32Const( tosplit->c.lo.uint_value );
            } else if( class == FL ) {
                _Zoiks( ZOIKS_129 );
            } else { /* FD */
                floatval = GetFloat( tosplit, FD );
                new = AllocConst( CFCnvU32F( _TargetLongInt( *(unsigned_32 *)( floatval->value + 0 ) ) ) );
            }
#if 0
        } else if( tosplit->c.const_type == CONS_ADDRESS ) {
            new = AddrConst( tosplit->c.value, tosplit->c.lo.int_value, CONS_OFFSET );
#endif
        } else {
            _Zoiks( ZOIKS_044 );
        }
        break;
    case N_REGISTER:
        if( class == U1 || class == I1 ) {
            new = AllocRegName( Low16Reg( tosplit->r.reg ) );
        } else if( class == U2 || class == I2 ) {
            new = AllocRegName( Low32Reg( tosplit->r.reg ) );
        } else {
            new = AllocRegName( Low64Reg( tosplit->r.reg ) );
        }
        break;
    case N_TEMP:
        new = TempOffset( tosplit, 0, class );
        if( new->t.temp_flags & CONST_TEMP ) {
            name *cons = tosplit->v.symbol;
            if( tosplit->n.type_class == FD ) {
                cons = Int64Equivalent( cons );
            }
            new->v.symbol = LowPart( cons, class );
        }
        break;
    case N_MEMORY:
        new = AllocMemory( tosplit->v.symbol, tosplit->v.offset,
                            tosplit->m.memory_type, class );
        new->v.usage = tosplit->v.usage;
        break;
    case N_INDEXED:
        new = ScaleIndex( tosplit->i.index, tosplit->i.base,
                            tosplit->i.constant, class,
                            0, tosplit->i.scale, tosplit->i.index_flags );
        break;
    }
    return( new );
}

name    *HighPart( name *tosplit, type_class_def class )
/***************************************************************
* Return the high (of type 'class') part of name 'tosplit'
* Note: There may not be any need to support splitting to
* classes other than U4/I4 on RISC (assuming that registers are
* at least 32-bit).
*/
{
    name                *new;
    signed_8            s8;
    unsigned_8          u8;
    signed_16           s16;
    unsigned_16         u16;
    constant_defn       *floatval;

    new = NULL;
    switch( tosplit->n.class ) {
    case N_CONSTANT:
        if( tosplit->c.const_type == CONS_ABSOLUTE ) {
            if( class == U1 ) {
                u8 = ( tosplit->c.lo.int_value >> 8 ) & 0xff;
                new = AllocUIntConst( u8 );
            } else if( class == I1 ) {
                s8 = ( tosplit->c.lo.int_value >> 8 ) & 0xff;
                new = AllocIntConst( s8 );
            } else if( class == U2 ) {
                u16 = ( tosplit->c.lo.int_value >> 16 ) & 0xffff;
                new = AllocUIntConst( u16 );
            } else if( class == I2 ) {
                s16 = ( tosplit->c.lo.int_value >> 16 ) & 0xffff;
                new = AllocIntConst( s16 );
            } else if( class == I4 ) {
                new = AllocS32Const( tosplit->c.hi.int_value );
            } else if( class == U4 ) {
                new = AllocU32Const( tosplit->c.hi.uint_value );
            } else if( class == FL ) {
                _Zoiks( ZOIKS_129 );
            } else { /* FD */
                floatval = GetFloat( tosplit, FD );
                new = AllocConst( CFCnvU32F( _TargetLongInt( *(unsigned_32 *)( floatval->value + 2 ) ) ) );
            }
#if 0
        } else if( tosplit->c.const_type == CONS_ADDRESS ) {
            new = AddrConst( tosplit->c.value, tosplit->c.lo.int_value, CONS_SEGMENT );
#endif
        } else {
            _Zoiks( ZOIKS_044 );
        }
        break;
    case N_REGISTER:
        if( class == U1 || class == I1 ) {
            new = AllocRegName( High16Reg( tosplit->r.reg ) );
        } else if( class == U2 || class == I2 ) {
            new = AllocRegName( High32Reg( tosplit->r.reg ) );
        } else {
            new = AllocRegName( High64Reg( tosplit->r.reg ) );
        }
        break;
    case N_TEMP:
        new = TempOffset( tosplit, tosplit->n.size/2, class );
        if( new->t.temp_flags & CONST_TEMP ) {
            name *cons = tosplit->v.symbol;
            if( tosplit->n.type_class == FD ) {
                cons = Int64Equivalent( cons );
            }
            new->v.symbol = HighPart( cons, class );
        }
        break;
    case N_MEMORY:
        new = AllocMemory( tosplit->v.symbol,
                                tosplit->v.offset + tosplit->n.size/2,
                                tosplit->m.memory_type, class );
        new->v.usage = tosplit->v.usage;
        break;
    case N_INDEXED:
        new = ScaleIndex( tosplit->i.index, tosplit->i.base,
                tosplit->i.constant+ tosplit->n.size/2, class,
                0, tosplit->i.scale, tosplit->i.index_flags );
        break;
    }
    return( new );
}

name    *OffsetMem( name *mem, type_length offset, type_class_def tipe )
/**********************************************************************/
{
    name                *new_mem;

    if( mem->n.class == N_INDEXED ) {
        new_mem = ScaleIndex( mem->i.index, mem->i.base,
                        mem->i.constant + offset, mem->n.type_class,
                        TypeClassSize[tipe], mem->i.scale, mem->i.index_flags );
    } else {
        assert( mem->n.class == N_TEMP );
        new_mem = STempOffset( mem, offset, tipe, TypeClassSize[tipe] );
    }
    return( new_mem );
}

instruction     *rSHR( instruction *ins )
/***************************************/
{
    type_class_def      target_type;
    name                *temp_1;
    name                *temp_2;
    instruction         *first_ins;
    instruction         *new_ins;

    // Since sizeof( reg ) > sizeof( int ), we could have junk
    // hanging about in the high 32 bits (or whatever) of the
    // operand. In order to conform to ANSI specs, we have to zero
    // this out, or sign extend the operand if it's a signed
    // shift right. To do this - we make a convert up to 8-byte
    // thing and then convert back down afterwords.

    target_type = U8;
    if( Unsigned[ins->type_class] != ins->type_class ) {
        target_type = I8;
    }
    temp_1 = AllocTemp( target_type );
    temp_2 = AllocTemp( target_type );
    first_ins = MakeConvert( ins->operands[0], temp_1, target_type, ins->type_class );
    PrefixIns( ins, first_ins );
    if( ins->operands[1]->n.class != N_CONSTANT ) {
        new_ins = MakeConvert( ins->operands[1], temp_2, target_type, ins->type_class );
        PrefixIns( ins, new_ins );
    } else {
        temp_2 = ins->operands[1];
    }
    new_ins = MakeBinary( OP_RSHIFT, temp_1, temp_2, temp_1, target_type );
    PrefixIns( ins, new_ins );
    new_ins = MakeConvert( temp_1, ins->result, ins->type_class, target_type );
    ReplIns( ins, new_ins );
    UpdateLive( first_ins, new_ins );
    return( first_ins );
}

instruction     *rMOVEXX_4( instruction *ins )
/********************************************/
{
    name        *temp;
    name        *src;
    name        *dst;
    type_length curr;           // which quad word we are currently on
    type_length size;           // size of the structure
    type_length words;          // number of full quad-word writes to use
    type_length rem;            // number of bytes to write after last quad
    instruction *new_ins;
    instruction *first_ins;
    instruction *last_ins;

    /*
     * Bust up a MOVXX into a series of 4-byte moves - we are guaranteed
     * that both the source and dest are 4-byte aligned.
     */
    assert( ins->operands[0]->n.class == N_TEMP || ins->operands[0]->n.class == N_INDEXED );
    temp = AllocTemp( U4 );
    size = ins->operands[0]->n.size;
    first_ins = NULL;
    last_ins = NULL;
    curr = 0;
    words = size / 4;
    rem = size % 4;
    if( rem ) {
        if( ins->result->n.class == N_TEMP ) {
            if( ( ins->result->t.temp_flags & ALIAS ) == 0 ) {
                // we have a write to a struct on the stack which is a master
                // since we don't 'pack' anything into the empty space after
                // this struct, we can safely overwrite it and not bother
                // doing the non-destructive last dword write
                words += 1;
                rem = 0;
            }
        }
    }
    while( words ) {
        src = OffsetMem( ins->operands[0], curr, U4 );
        dst = OffsetMem( ins->result, curr, U4 );
        curr += 4;
        words -= 1;
        new_ins = MakeMove( src, temp, U4 );
        PrefixIns( ins, new_ins );
        if( first_ins == NULL ) {
            first_ins = new_ins;
        }
        new_ins = MakeMove( temp, dst, U4 );
        if( words == 0 && rem == 0 ) {
            ReplIns( ins, new_ins );
            last_ins = new_ins;
        } else {
            PrefixIns( ins, new_ins );
        }
    }
    switch( rem ) {
    case 0:
        break;
    case 1:
        src = OffsetMem( ins->operands[0], curr, U1 );
        dst = OffsetMem( ins->result, curr, U1 );
        last_ins = MakeMove( src, temp, U1 );
        if( first_ins == NULL ) {
            first_ins = last_ins;
        }
        ReplIns( ins, last_ins );
        break;
    case 2:
        src = OffsetMem( ins->operands[0], curr, U2 );
        dst = OffsetMem( ins->result, curr, U2 );
        last_ins = MakeMove( src, temp, U2 );
        if( first_ins == NULL ) {
            first_ins = last_ins;
        }
        ReplIns( ins, last_ins );
        break;
    case 3:
        src = OffsetMem( ins->operands[0], curr, U1 );
        dst = OffsetMem( ins->result, curr, U1 );
        new_ins = MakeMove( src, temp, U1 );
        PrefixIns( ins, new_ins );
        if( first_ins == NULL ) {
            first_ins = new_ins;
        }
        src = OffsetMem( ins->operands[0], curr, U2 );
        dst = OffsetMem( ins->result, curr, U2 );
        last_ins = MakeMove( src, temp, U2 );
        ReplIns( ins, last_ins );
        break;
    default:
        assert( 0 );
    }
    UpdateLive( first_ins, last_ins );
    return( first_ins );
}


instruction     *rCLRHI_4( instruction *ins )
/********************************************
* Clear the high 32 bits of a 64-bit name
*/
{
    name                *high;
    name                *low;
    instruction         *new_ins;
    type_class_def      tipe;

    tipe = HalfClass[ins->type_class];
    low = LowPart( ins->result, tipe );
    high = HighPart( ins->result, tipe );
    ChangeType( ins, tipe );
    ins->head.opcode = OP_MOV;
    ins->result = low;
    new_ins = MakeMove( AllocS32Const( 0 ), high, tipe );
    PrefixIns( ins, new_ins );
    return( new_ins );
}


instruction     *rSEX_4TO8( instruction *ins )
/*********************************************
* Sign-extend a 32-bit name to 64-bit
*/
{
    instruction         *ins1;
    instruction         *ins2;
    name                *high;

    high = HighPart( ins->result, WD );
    ins->result = LowPart( ins->result, WD );
    ins->head.opcode = OP_MOV;
    ins->type_class = WD;
    ins->base_type_class = WD;
    ins->table = NULL;
    ins1 = MakeMove( ins->operands[0], high, WD );
    SuffixIns( ins, ins1 );
    ins2 = MakeBinary( OP_RSHIFT, high, AllocIntConst( 31 ), high, SW );
    SuffixIns( ins1, ins2 );
    return( ins );
}


instruction      *rMOVELOW( instruction *ins )
/****************************************************
* Move low part of a name, in other words chop off the
* high part (e.g. convert U8==>U4)
*/
{
    ins->head.opcode = OP_MOV;
    ins->operands[0] = LowPart( ins->operands[0], ins->type_class );
    ins->table = NULL;
    return( ins );
}
