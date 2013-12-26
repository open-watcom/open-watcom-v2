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
* Description:  i86 instruction splitting (simplifications/optimizations).
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "regset.h"
#include "model.h"
#include "system.h"
#include "zoiks.h"
#include "cfloat.h"
#include "makeins.h"
#include "namelist.h"


typedef struct eight_byte_name {
        union name     *low;
        union name     *mid_low;
        union name     *mid_high;
        union name     *high;
} eight_byte_name;

extern  void            ChangeType( instruction *, type_class_def );
extern  name            *IntEquivalent( name * );
extern  void            DupSegOp( instruction *, instruction *, int );
extern  opcode_entry    *CodeTable( instruction * );
extern  bool            SameThing( name *, name * );
extern  instruction     *MoveConst( unsigned_32, name *, type_class_def );
extern  void            UpdateLive( instruction *, instruction * );
extern  void            DupSegRes( instruction *, instruction * );
extern  void            MoveSegOp( instruction *, instruction *, int );
extern  void            SuffixIns( instruction *, instruction * );
extern  void            HalfType( instruction * );
extern  hw_reg_set      High32Reg( hw_reg_set );
extern  hw_reg_set      High16Reg( hw_reg_set );
extern  hw_reg_set      Low32Reg( hw_reg_set );
extern  name            *AllocRegName( hw_reg_set );
extern  hw_reg_set      Low16Reg( hw_reg_set );
extern  name            *AddrConst( name *, int, constant_class );
extern  void            ReplIns( instruction *, instruction * );
extern  void            PrefixIns( instruction *, instruction * );
extern  void            DupSeg( instruction *, instruction * );
extern  void            DoNothing( instruction * );
extern  name            *SegName( name * );
extern  void            DelSeg( instruction * );
extern  name            *ScaleIndex( name *, name *, type_length, type_class_def, type_length, int, i_flags );
extern  bool            Overlaps( name *, name * );
extern  bool            IndexOverlaps( instruction *ins, int i );

/*forward declaration*/
static  void            Split8Name( instruction *ins, name *tosplit, eight_byte_name *out );

extern    type_class_def        HalfClass[];
extern    type_class_def        Unsigned[];


extern  name    *LowPart( name *tosplit, type_class_def class )
/*************************************************************/
{
    name                *new = NULL;
    name                *new_cons;
    signed_8            s8;
    unsigned_8          u8;
    signed_16           s16;
    unsigned_16         u16;
    unsigned_32         u32;
    constant_defn       *floatval;

    switch( tosplit->n.class ) {
    case N_CONSTANT:
        if( tosplit->c.const_type == CONS_ABSOLUTE ) {
            if( class == U1 ) {
                u8 = tosplit->c.int_value & 0xff;
                new = AllocUIntConst( u8 );
            } else if( class == I1 ) {
                s8 = tosplit->c.int_value & 0xff;
                new = AllocIntConst( s8 );
            } else if( class == U2 ) {
                u16 = tosplit->c.int_value & 0xffff;
                new = AllocUIntConst( u16 );
            } else if( class == I2 ) {
                s16 = tosplit->c.int_value & 0xffff;
                new = AllocIntConst( s16 );
            } else if( class == FL ) {
                _Zoiks( ZOIKS_125 );
            } else { /* FD */
                floatval = GetFloat( tosplit, FD );
                u32 = (unsigned_32)floatval->value[ 1 ] << 16;
                u32 += floatval->value[ 0 ];
                new = AllocConst( CFCnvU32F( _TargetLongInt( u32 ) ) );
            }
        } else if( tosplit->c.const_type == CONS_ADDRESS ) {
            new = AddrConst( tosplit->c.value,
                                   tosplit->c.int_value, CONS_OFFSET );
        } else {
            _Zoiks( ZOIKS_044 );
        }
        break;
    case N_REGISTER:
        if( class == U1 || class == I1 ) {
            new = AllocRegName( Low16Reg( tosplit->r.reg ) );
        } else {
            new = AllocRegName( Low32Reg( tosplit->r.reg ) );
        }
        break;
    case N_TEMP:
        new = TempOffset( tosplit, 0, class );
        if( new->t.temp_flags & CONST_TEMP ) {
            if( tosplit->n.name_class == FS ) {
                new_cons = IntEquivalent( tosplit->v.symbol );
            } else {
                new_cons = tosplit->v.symbol;
            }
            new->v.symbol = LowPart( new_cons, class );
        }
        break;
    case N_MEMORY:
        new = AllocMemory( tosplit->v.symbol, tosplit->v.offset,
                                tosplit->m.memory_type, class );
        new->v.usage = tosplit->v.usage;
        break;
    case N_INDEXED:
        new = ScaleIndex( tosplit->i.index, tosplit->i.base,
                                tosplit->i.constant, class, 0, tosplit->i.scale,
                                tosplit->i.index_flags );
        break;
    default:
        break;
    }
    return( new );
}


extern  name    *HighPart( name *tosplit, type_class_def class )
/**************************************************************/
{
    name                *new = NULL;
    name                *new_cons;
    name                *op;
    signed_8            s8;
    unsigned_8          u8;
    signed_16           s16;
    unsigned_16         u16;
    unsigned_32         u32;
    constant_defn       *floatval;

    switch( tosplit->n.class ) {
    case N_CONSTANT:
        if( tosplit->c.const_type == CONS_ABSOLUTE ) {
            if( class == U1 ) {
                u8 = ( tosplit->c.int_value >> 8 ) & 0xff;
                new = AllocUIntConst( u8 );
            } else if( class == I1 ) {
                s8 = ( tosplit->c.int_value >> 8 ) & 0xff;
                new = AllocIntConst( s8 );
            } else if( class == U2 ) {
                u16 = ( tosplit->c.int_value >> 16 ) & 0xffff;
                new = AllocUIntConst( u16 );
            } else if( class == I2 ) {
                s16 = ( tosplit->c.int_value >> 16 ) & 0xffff;
                new = AllocIntConst( s16 );
            } else if( class == FL ) {
                _Zoiks( ZOIKS_125 );
            } else { /* FD */
                floatval = GetFloat( tosplit, FD );
                u32 = (unsigned_32)floatval->value[ 3 ] << 16;
                u32 += floatval->value[ 2 ];
                new = AllocConst( CFCnvU32F( _TargetLongInt( u32 ) ) );
            }
        } else if( tosplit->c.const_type == CONS_ADDRESS ) {
            new = AddrConst( tosplit->c.value,
                                   tosplit->c.int_value, CONS_SEGMENT );
        } else {
            _Zoiks( ZOIKS_044 );
        }
        break;
    case N_REGISTER:
        if( class == U1 || class == I1 ) {
            new = AllocRegName( High16Reg( tosplit->r.reg ) );
        } else {
            new = AllocRegName( High32Reg( tosplit->r.reg ) );
        }
        break;
    case N_TEMP:
        new = TempOffset( tosplit, tosplit->n.size/2, class );
        if( new->t.temp_flags & CONST_TEMP ) {
            if( tosplit->n.name_class == FS ) {
                new_cons = IntEquivalent( tosplit->v.symbol );
            } else {
                new_cons = tosplit->v.symbol;
            }
            op = HighPart( new_cons, class );
            if( op->n.class == N_REGISTER ) return( op );
            new->v.symbol = op;
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
                tosplit->i.constant+ tosplit->n.size/2, class, 0,
                tosplit->i.scale, tosplit->i.index_flags );
        break;
    default:
        break;
    }
    return( new );
}


extern  name    *OffsetPart( name *tosplit )
/******************************************/
{
    return( LowPart( tosplit, U2 ) );
}


extern  name    *SegmentPart( name *tosplit )
/*******************************************/
{
    return( HighPart( tosplit, U2 ) );
}


extern  instruction     *SplitUnary( instruction *ins )
/*****************************************************/
{
    instruction *new_ins;
    name        *high_res;
    name        *low_res;

    HalfType( ins );
    if( ins->result == NULL ) {
        high_res = NULL;
        low_res = NULL;
    } else {
        high_res = HighPart( ins->result, ins->type_class );
        low_res  = LowPart( ins->result, ins->type_class );
    }
    new_ins = MakeUnary( ins->head.opcode,
                         LowPart( ins->operands[ 0 ], ins->type_class ),
                         low_res, ins->type_class );
    ins->operands[ 0 ] = HighPart( ins->operands[ 0 ],ins->type_class );
    ins->result = high_res;
    if( ins->head.opcode == OP_PUSH ) {
        DupSeg( ins, new_ins );
        SuffixIns( ins, new_ins );
        new_ins = ins;
    } else {
        DupSeg( ins, new_ins );
        PrefixIns( ins, new_ins );
    }
    return( new_ins );
}

static  instruction     *SplitOverlapped( instruction *ins, int op )
/******************************************************************/
/* Used to split 8 byte operation because of overlap problems */
{
    instruction         *move;
    name                *tmp;

    tmp = AllocTemp( ins->type_class );
    move = MakeMove( ins->operands[op], tmp, ins->type_class );
    MoveSegOp( ins, move, op );
    ins->operands[op] = tmp;
    PrefixIns( ins, move );
    UpdateLive( move, ins );
    return( move );
}

extern  instruction     *rMOVE8LOW( instruction *ins )
/****************************************************/
/*   Move low 4 bytes of 8 byte thingy */
{
    instruction         *lo_ins;
    instruction         *mid_lo_ins;
    eight_byte_name     left;

    if( IndexOverlaps( ins, 0 ) ) {
        return( SplitOverlapped( ins, 0 ) );
    }
    Split8Name( ins, ins->operands[ 0 ], &left );
    lo_ins = MakeMove( left.low, LowPart( ins->result, U2 ), U2 );
    mid_lo_ins = MakeMove( left.mid_low, HighPart( ins->result, U2 ), U2 );
    DupSeg( ins, lo_ins );
    DupSeg( ins, mid_lo_ins );
    PrefixIns( ins, lo_ins );
    ReplIns( ins, mid_lo_ins );
    UpdateLive( lo_ins, mid_lo_ins );
    return( lo_ins );
}

extern  instruction     *rSPLIT8( instruction *ins )
/**************************************************/
/*   Used to split 8 byte push, pop and move*/
{
    opcode_defs         op;
    instruction         *lo_ins;
    instruction         *mid_lo_ins;
    instruction         *mid_hi_ins;
    instruction         *hi_ins;
    instruction         *first_ins;
    instruction         *last_ins;
    eight_byte_name     result;
    eight_byte_name     left;

    if( IndexOverlaps( ins, 0 ) ) {
        return( SplitOverlapped( ins, 0 ) );
    }
    Split8Name( ins, ins->operands[ 0 ], &left );
    Split8Name( ins, ins->result, &result );
    op = ins->head.opcode;
    lo_ins = MakeUnary( op, left.low, result.low, U2 );
    mid_lo_ins = MakeUnary( op, left.mid_low, result.mid_low, U2 );
    mid_hi_ins = MakeUnary( op, left.mid_high, result.mid_high, U2 );
    hi_ins = MakeUnary( op, left.high, result.high, U2 );
    DupSeg( ins, lo_ins );
    DupSeg( ins, mid_lo_ins );
    DupSeg( ins, mid_hi_ins );
    DupSeg( ins, hi_ins );
    if( op == OP_POP ) {       /* do low first*/
        first_ins = lo_ins;
        PrefixIns( ins, lo_ins );
        PrefixIns( ins, mid_lo_ins );
        PrefixIns( ins, mid_hi_ins );
        ReplIns( ins, hi_ins );
        last_ins = hi_ins;
    } else {
        first_ins = hi_ins;
        PrefixIns( ins, hi_ins );
        PrefixIns( ins, mid_hi_ins );
        PrefixIns( ins, mid_lo_ins );
        ReplIns( ins, lo_ins );
        last_ins = lo_ins;
    }
    UpdateLive( first_ins, last_ins );
    return( first_ins );
}

extern  instruction     *rSPLIT8BIN( instruction *ins )
/*****************************************************/
/*   Used to split 8 byte binary operations (ADD,SUB,Logicals) */
{
    opcode_defs         op;
    instruction         *lo_ins;
    instruction         *mid_lo_ins;
    instruction         *mid_hi_ins;
    instruction         *hi_ins;
    eight_byte_name     result;
    eight_byte_name     left;
    eight_byte_name     rite;

    if( IndexOverlaps( ins, 0 ) ) {
        return( SplitOverlapped( ins, 0 ) );
    }
    if( IndexOverlaps( ins, 1 ) ) {
        return( SplitOverlapped( ins, 1 ) );
    }
    Split8Name( ins, ins->operands[ 0 ], &left );
    Split8Name( ins, ins->operands[ 1 ], &rite );
    Split8Name( ins, ins->result, &result );
    op = ins->head.opcode;
    lo_ins = MakeBinary( op, left.low, rite.low, result.low, U2 );
    switch( op ) {
    case OP_ADD:
        op = OP_EXT_ADD;
        break;
    case OP_SUB:
        op = OP_EXT_SUB;
        break;
    default:
        break;
    }
    mid_lo_ins = MakeBinary( op, left.mid_low, rite.mid_low, result.mid_low, U2 );
    mid_hi_ins = MakeBinary( op, left.mid_high, rite.mid_high, result.mid_high, U2 );
    hi_ins = MakeBinary( op, left.high, rite.high, result.high, U2 );
    switch( op ) {
    case OP_EXT_ADD:
    case OP_EXT_SUB:
        hi_ins->table = CodeTable( hi_ins );
        mid_hi_ins->table = hi_ins->table;
        mid_hi_ins->ins_flags |= INS_CC_USED;
        mid_lo_ins->table = hi_ins->table;
        mid_lo_ins->ins_flags |= INS_CC_USED;
        lo_ins->table = hi_ins->table;
        lo_ins->ins_flags |= INS_CC_USED;
        break;
    default:
        break;
    }
    DupSeg( ins, lo_ins );
    DupSeg( ins, mid_lo_ins );
    DupSeg( ins, mid_hi_ins );
    DupSeg( ins, hi_ins );
    PrefixIns( ins, lo_ins );
    PrefixIns( ins, mid_lo_ins );
    PrefixIns( ins, mid_hi_ins );
    ReplIns( ins, hi_ins );
    UpdateLive( lo_ins, hi_ins );
    return( lo_ins );
}

extern  instruction     *rSPLIT8NEG( instruction *ins )
/*****************************************************/
/*   Used to split 8 byte negate */
{
    instruction         *new;

    /*  OK, so we're not really splitting the thing. But it works just
     *  as well.
     */
    new = MakeBinary( OP_SUB, AllocIntConst( 0 ), ins->operands[0],
                        ins->result, ins->type_class );
    DupSeg( ins, new );
    ReplIns( ins, new );
    return( new );
}

extern  instruction     *rSPLIT8TST( instruction *ins )
/*****************************************************/
/*   Used to split 8 byte test */
{
    eight_byte_name     left;
    eight_byte_name     rite;
    byte                true_idx;
    byte                false_idx;
    opcode_defs         op;
    instruction         *lo_ins;
    instruction         *mid_lo_ins;
    instruction         *mid_hi_ins;
    instruction         *hi_ins;

    Split8Name( ins, ins->operands[ 0 ], &left );
    Split8Name( ins, ins->operands[ 1 ], &rite );
    true_idx = _TrueIndex( ins );
    false_idx = _FalseIndex( ins );
    op = ins->head.opcode;
    lo_ins = MakeCondition( op, left.low, rite.low, true_idx, false_idx, U2 );
    switch( op ) {
    case OP_BIT_TEST_FALSE:
        op = OP_BIT_TEST_TRUE;
        true_idx = false_idx;
        break;
    default:
        break;
    }
    false_idx = NO_JUMP;
    mid_lo_ins = MakeCondition( op, left.mid_low, rite.mid_low, true_idx, false_idx, U2 );
    mid_hi_ins = MakeCondition( op, left.mid_high, rite.mid_high, true_idx, false_idx, U2 );
    hi_ins = MakeCondition( op, left.high, rite.high, true_idx, false_idx, U2 );
    DupSeg( ins, lo_ins );
    DupSeg( ins, mid_lo_ins );
    DupSeg( ins, mid_hi_ins );
    DupSeg( ins, hi_ins );
    PrefixIns( ins, hi_ins );
    PrefixIns( ins, mid_hi_ins );
    PrefixIns( ins, mid_lo_ins );
    ReplIns( ins, lo_ins );
    UpdateLive( hi_ins, lo_ins );
    return( hi_ins );
}

extern  instruction     *rSPLIT8CMP( instruction *ins )
/*****************************************************/
/*   Used to split 8 byte compare */
{
    eight_byte_name     left;
    eight_byte_name     rite;
    byte                true_idx;
    byte                false_idx;
    instruction         *new[8];
    type_class_def      high_class;
    unsigned            i;
    unsigned            j;

/*
 * 2005-04-06 RomanT (bug #407)
 * I removed calls do DoNothing(), it seems ok, extra jumps are perfectly
 * optimized out in other places of compiler. Calling DoNothing() on chain
 * of conditions to reuse existing CC flags is ugly and causes unpredictable
 * logical faults in other places.
 */
    Split8Name( ins, ins->operands[ 0 ], &left );
    Split8Name( ins, ins->operands[ 1 ], &rite );
    true_idx = _TrueIndex( ins );
    false_idx = _FalseIndex( ins );
    high_class = HalfClass[ HalfClass[ ins->type_class ] ];
    i = -1;
    switch( ins->head.opcode ) {
    case OP_CMP_EQUAL:
        new[++i] = MakeCondition( OP_CMP_NOT_EQUAL, left.high, rite.high,
                                        false_idx, NO_JUMP, U2 );
        new[++i] = MakeCondition( OP_CMP_NOT_EQUAL, left.mid_high, rite.mid_high,
                                        false_idx, NO_JUMP, U2 );
        new[++i] = MakeCondition( OP_CMP_NOT_EQUAL, left.mid_low, rite.mid_low,
                                        false_idx, NO_JUMP, U2 );
        new[++i] = MakeCondition( OP_CMP_EQUAL, left.low, rite.low,
                                        true_idx, false_idx, U2 );
        break;
    case OP_CMP_NOT_EQUAL:
        new[++i] = MakeCondition( OP_CMP_NOT_EQUAL, left.high, rite.high,
                                        true_idx, NO_JUMP, U2 );
        new[++i] = MakeCondition( OP_CMP_NOT_EQUAL, left.mid_high, rite.mid_high,
                                        true_idx, NO_JUMP, U2 );
        new[++i] = MakeCondition( OP_CMP_NOT_EQUAL, left.mid_low, rite.mid_low,
                                        true_idx, NO_JUMP, U2 );
        new[++i] = MakeCondition( OP_CMP_NOT_EQUAL, left.low, rite.low,
                                        true_idx, false_idx, U2 );
        break;
    case OP_CMP_GREATER:
    case OP_CMP_GREATER_EQUAL:
        new[++i] = MakeCondition( OP_CMP_GREATER, left.high, rite.high,
                                        true_idx, NO_JUMP, high_class );
        new[++i] = MakeCondition( OP_CMP_NOT_EQUAL, left.high, rite.high,
                                        false_idx, NO_JUMP, U2 );
        new[++i] = MakeCondition( OP_CMP_GREATER, left.mid_high, rite.mid_high,
                                        true_idx, NO_JUMP, U2 );
        new[++i] = MakeCondition( OP_CMP_NOT_EQUAL, left.mid_high, rite.mid_high,
                                        false_idx, NO_JUMP, U2 );
        new[++i] = MakeCondition( OP_CMP_GREATER, left.mid_low, rite.mid_low,
                                        true_idx, NO_JUMP, U2 );
        new[++i] = MakeCondition( OP_CMP_NOT_EQUAL, left.mid_low, rite.mid_low,
                                        false_idx, NO_JUMP, U2 );
        new[++i] = MakeCondition( ins->head.opcode, left.low, rite.low,
                                        true_idx, false_idx, U2 );
        break;
    case OP_CMP_LESS:
    case OP_CMP_LESS_EQUAL:
        new[++i] = MakeCondition( OP_CMP_LESS, left.high, rite.high,
                                        true_idx, NO_JUMP, high_class );
        new[++i] = MakeCondition( OP_CMP_NOT_EQUAL, left.high, rite.high,
                                        false_idx, NO_JUMP, U2 );
        new[++i] = MakeCondition( OP_CMP_LESS, left.mid_high, rite.mid_high,
                                        true_idx, NO_JUMP, U2 );
        new[++i] = MakeCondition( OP_CMP_NOT_EQUAL, left.mid_high, rite.mid_high,
                                        false_idx, NO_JUMP, U2 );
        new[++i] = MakeCondition( OP_CMP_LESS, left.mid_low, rite.mid_low,
                                        true_idx, NO_JUMP, U2 );
        new[++i] = MakeCondition( OP_CMP_NOT_EQUAL, left.mid_low, rite.mid_low,
                                        false_idx, NO_JUMP, U2 );
        new[++i] = MakeCondition( ins->head.opcode, left.low, rite.low,
                                        true_idx, false_idx, U2 );
        break;
    default:
        break;
    }
    for( j = 0; j < i; ++j ) {
        DupSeg( ins, new[j] );
        PrefixIns( ins, new[j] );
    }
    DupSeg( ins, new[i] );
    ReplIns( ins, new[i] );
    UpdateLive( new[0], new[i] );
    return( new[0] );
}


extern  instruction     *rCLRHI_D( instruction *ins )
/***************************************************/
/*   Clear the high dword of an 8 byte thingy (zero extend) */
{
    eight_byte_name     result;
    instruction         *move1;
    instruction         *move2;
    instruction         *move3;
    instruction         *move4;

    if( IndexOverlaps( ins,  0 ) ) {
        return( SplitOverlapped( ins, 0 ) );
    }
    Split8Name( ins, ins->result, &result );
    move1 = MakeMove( LowPart( ins->operands[0], U2 ), result.low, U2 );
    move2 = MakeMove( HighPart( ins->operands[0], U2 ), result.mid_low, U2 );
    move3 = MakeMove( AllocIntConst( 0 ), result.mid_high, U2 );
    move4 = MakeMove( AllocIntConst( 0 ), result.high, U2 );
    DupSeg( ins, move1 );
    DupSeg( ins, move2 );
    DupSegRes( ins, move3 );
    DupSegRes( ins, move4 );
    PrefixIns( ins, move1 );
    PrefixIns( ins, move2 );
    PrefixIns( ins, move3 );
    ReplIns( ins, move4 );
    UpdateLive( move1, move4 );
    return( move1 );
}


extern  instruction     *rCDQ( instruction *ins )
/***********************************************/
/*   Sign extend a 4 byte thingy to an 8 byte thingy */
{
    eight_byte_name     result;
    instruction         *shift;
    instruction         *move1, *move2;
    instruction         *lo_ins;
    instruction         *mid_lo_ins;
    name                *temp;
    name                *high;

    if( IndexOverlaps( ins, 0 ) ) {
        return( SplitOverlapped( ins, 0 ) );
    }
    Split8Name( ins, ins->result, &result );
    /*
     * 2005-09-26 RomanT
     * Use temp name to avoid creation of instruction where both
     * operand and result have segment prefix (cg cannot handle this)
     */
    temp = AllocTemp( I2 );
    high = HighPart( ins->operands[0], U2 );
    lo_ins = MakeMove( LowPart( ins->operands[0], U2 ), result.low, U2 );
    mid_lo_ins = MakeMove( high, result.mid_low, U2 );
    shift = MakeBinary( OP_RSHIFT, high, AllocIntConst( 15 ), temp, I2 );
    move1 = MakeMove( temp, result.mid_high, U2 );
    move2 = MakeMove( temp, result.high, U2 );
    DupSeg( ins, lo_ins );
    DupSeg( ins, mid_lo_ins );
    DupSeg( ins, shift );
    DupSegRes( ins, move1 );
    DupSegRes( ins, move2 );
    PrefixIns( ins, lo_ins );
    PrefixIns( ins, mid_lo_ins );
    PrefixIns( ins, shift );
    PrefixIns( ins, move1 );
    ReplIns( ins, move2 );
    UpdateLive( lo_ins, move2 );
    return( lo_ins );
}

static  name    *High8Part( instruction *ins, name *temp )
/********************************************************/
{
    eight_byte_name     expand;

    Split8Name( ins, temp, & expand );
    return( expand.high );
}


static  void    Split8Name( instruction *ins, name *tosplit, eight_byte_name *out )
/*********************************************************************************/
{
    type_length         offset;
    cg_sym_handle       symbol;
    constant_defn       *floatval;
    cg_class            cg;
    eight_byte_name     konst;

    out->low      = NULL;
    out->mid_low  = NULL;
    out->mid_high = NULL;
    out->high     = NULL;

    if( tosplit == NULL ) return;
    switch( tosplit->n.class ) {
    case N_REGISTER:
        out->low      = AllocRegName( HW_DX );
        out->mid_low  = AllocRegName( HW_CX );
        out->mid_high = AllocRegName( HW_BX );
        out->high     = AllocRegName( HW_AX );
        break;
    case N_INDEXED:
        offset = tosplit->i.constant;
        out->low      = ScaleIndex( tosplit->i.index,
                        tosplit->i.base, offset, I2, 0, tosplit->i.scale,
                        tosplit->i.index_flags );
        out->mid_low  = ScaleIndex( tosplit->i.index,
                        tosplit->i.base, offset + 2, I2, 0, tosplit->i.scale,
                        tosplit->i.index_flags );
        out->mid_high = ScaleIndex( tosplit->i.index,
                        tosplit->i.base, offset + 4, I2, 0, tosplit->i.scale,
                        tosplit->i.index_flags );
        out->high     = ScaleIndex( tosplit->i.index,
                        tosplit->i.base, offset + 6, I2, 0, tosplit->i.scale,
                        tosplit->i.index_flags );
        break;
    case N_TEMP:
        out->low      = TempOffset( tosplit, 0, I2 );
        out->mid_low  = TempOffset( tosplit, 2, I2 );
        out->mid_high = TempOffset( tosplit, 4, I2 );
        out->high     = TempOffset( tosplit, 6, I2 );
        if( tosplit->t.temp_flags & CONST_TEMP ) {
            Split8Name( ins, tosplit->v.symbol, &konst );
            out->low->v.symbol          = konst.low;
            out->mid_low->v.symbol      = konst.mid_low;
            out->mid_high->v.symbol     = konst.mid_high;
            out->high->v.symbol         = konst.high;
        }
        break;
    case N_MEMORY:
        symbol = tosplit->v.symbol;
        cg = tosplit->m.memory_type;
        offset = tosplit->v.offset;
        out->low      = AllocMemory( symbol, offset, cg, I2 );
        out->mid_low  = AllocMemory( symbol, offset + 2, cg, I2 );
        out->mid_high = AllocMemory( symbol, offset + 4, cg, I2 );
        out->high     = AllocMemory( symbol, offset + 6, cg, I2 );
        out->low->v.usage      = tosplit->v.usage;
        out->mid_low->v.usage  = tosplit->v.usage;
        out->mid_high->v.usage = tosplit->v.usage;
        out->high->v.usage     = tosplit->v.usage;
        break;
    case N_CONSTANT:
        switch( ins->type_class ) {
        case I8:
        case U8:
            out->low      = AllocIntConst( _TargetInt( tosplit->c.int_value & 0xffff ) );
            out->mid_low  = AllocIntConst( _TargetInt( tosplit->c.int_value >> 16 ) );
            out->mid_high = AllocIntConst( _TargetInt( tosplit->c.int_value_2 & 0xffff ) );
            out->high     = AllocIntConst( _TargetInt( tosplit->c.int_value_2 >> 16 ) );
            break;
        case FD:
            floatval = GetFloat( tosplit, FD );
            out->low      = AllocIntConst( _TargetInt( floatval->value[ 0 ] ) );
            out->mid_low  = AllocIntConst( _TargetInt( floatval->value[ 1 ] ) );
            out->mid_high = AllocIntConst( _TargetInt( floatval->value[ 2 ] ) );
            out->high     = AllocIntConst( _TargetInt( floatval->value[ 3 ] ) );
            break;
        default:
            Zoiks( ZOIKS_136 );
            break;
        }
        break;
    default:
        Zoiks( ZOIKS_135 );
        break;
    }
}


extern  instruction     *rCYPSHIFT( instruction *ins )
/****************************************************/
{
    instruction *ins1;
    instruction *ins2;
    instruction *ins3;
    name        *temp;
    unsigned    half_count;

    HalfType( ins );
    temp = AllocTemp( ins->type_class );
    half_count = ins->operands[1]->c.int_value - temp->n.size * 8;
    if( ins->head.opcode == OP_LSHIFT ) {
        ins1 = MakeBinary( OP_LSHIFT,
                        LowPart( ins->operands[ 0 ], ins->type_class ),
                        AllocIntConst( half_count ),
                        temp, ins->type_class );
        ins2 = MakeMove( temp, HighPart( ins->result, ins->type_class ),
                          ins->type_class );
        ins3 = MoveConst( 0, LowPart( ins->result, ins->type_class ),
                           ins->type_class );
        DupSegRes( ins, ins2 );
    } else {
        ins1 = MakeBinary( OP_RSHIFT,
                        HighPart( ins->operands[ 0 ], ins->type_class ),
                        AllocIntConst( half_count ),
                        temp, ins->type_class );
        ins2 = NULL;
        ins3 = MakeConvert( temp, ins->result, ins->result->n.name_class,
                            ins->type_class );
    }
    DupSegOp( ins, ins1, 0 );
    DupSegRes( ins, ins3 );
    ins->operands[ 0 ] = AllocIntConst( 0 ); // so false live info not gen'd
    PrefixIns( ins, ins1 );
    if( ins2 != NULL ) PrefixIns( ins, ins2 );
    ReplIns( ins, ins3 );
    UpdateLive( ins1, ins3 );
    return( ins1 );
}


extern  instruction     *rBYTESHIFT( instruction *ins )
/*****************************************************/
{
    instruction *ins1;
    instruction *ins2;
    instruction *ins3;
    instruction *ins4;
    name        *op1;
    name        *res;

    op1 = ins->operands[ 0 ];
    res = ins->result;
    if( ins->head.opcode == OP_LSHIFT ) {
        ins1 = MakeMove(  LowPart( HighPart( op1, U2 ), U1 ),
                          HighPart( HighPart( res, U2 ), U1 ), U1 );
        ins2 = MakeMove( HighPart(  LowPart( op1, U2 ), U1 ),
                           LowPart( HighPart( res, U2 ), U1 ), U1 );
        ins3 = MakeMove(  LowPart(  LowPart( op1, U2 ), U1 ),
                          HighPart(  LowPart( res, U2 ), U1 ), U1 );
        ins4 = MoveConst( 0, LowPart( LowPart( res, U2 ), U1 ), U1 );
    } else {
        ins1 = MakeMove( HighPart(  LowPart( op1, U2 ), U1 ),
                           LowPart(  LowPart( res, U2 ), U1 ), U1 );
        ins2 = MakeMove(  LowPart( HighPart( op1, U2 ), U1 ),
                          HighPart(  LowPart( res, U2 ), U1 ), U1 );
        ins3 = MakeMove( HighPart( HighPart( op1, U2 ), U1 ),
                           LowPart( HighPart( res, U2 ), U1 ), U1 );
        ins4 = MoveConst( 0, HighPart( HighPart( res, U2 ), U1 ), U1 );
    }
    DupSeg( ins, ins1 );
    DupSeg( ins, ins2 );
    DupSeg( ins, ins3 );
    DupSegRes( ins, ins4 );
    PrefixIns( ins, ins1 );
    PrefixIns( ins, ins2 );
    ins->operands[ 0 ] = AllocIntConst( 0 );
    PrefixIns( ins, ins3 );
    ReplIns( ins, ins4 );
    UpdateLive( ins1, ins4 );
    return( ins1 );
}

extern instruction      *rLOADLONGADDR( instruction *ins )
/********************************************************/
{
    instruction         *new_ins;
    name                *name1;

    new_ins = MakeMove( SegName( ins->operands[ 0 ] ),
                            HighPart( ins->result, U2 ), U2 );
    ChangeType( ins, U2 );
    ins->result = LowPart( ins->result, U2 );
    name1 = ins->operands[ 0 ];
    if( name1->n.class == N_INDEXED ) {
        if( name1->i.index->n.size == 4 ) {
            ins->operands[ 0 ] = ScaleIndex(
                LowPart(name1->i.index,U2),
                name1->i.base,
                name1->i.constant,
                name1->n.name_class,
                name1->n.size,
                name1->i.scale,
                name1->i.index_flags );
        }
    }
    DupSegRes( ins, new_ins );
    PrefixIns( ins, new_ins );
    return( new_ins );
}


extern  instruction     *rHIGHCMP( instruction *ins )
/***************************************************/
{
    name                *name1;

    name1 = ins->operands[ 0 ];
    switch( ins->type_class ) {
    /* floating point comparison with 0*/
    case FD:
    case I8:
    case U8:
        name1 = High8Part( ins, name1 );
        break;
    default:
        name1 = HighPart( name1, I2 );
        break;
    }
    ins->operands[ 0 ] = name1;
    ChangeType( ins, I2 );
    return( ins );
}


extern instruction      *rMAKEU4( instruction *ins )
/**************************************************/
{
    instruction         *new_ins;
    instruction         *ins2;

/* change pointer '==' or '!=' to U4 compare*/
    new_ins = MakeMove( ins->operands[ 0 ], AllocTemp( U4 ), U4 );
    DupSegOp( ins, new_ins, 0 );
    ins2 = MakeMove( ins->operands[ 1 ], AllocTemp( U4 ), U4 );
    DupSegOp( ins, ins2, 0 );
    ins->operands[ 0 ] = new_ins->result;
    ins->operands[ 1 ] = ins2->result;
    DelSeg( ins );
    PrefixIns( ins, new_ins );
    PrefixIns( ins, ins2 );
    ChangeType( ins, U4 );
    return( new_ins );
}

extern  instruction     *rSPLITPUSH( instruction *ins )
/*****************************************************/
/* NOT NEEDED */
{
    return( ins );
}


extern  instruction     *rEXT_PUSH1( instruction *ins )
/*****************************************************/
{
    name        *temp;
    instruction *new_ins;

    temp = AllocTemp( U2 );
    new_ins = MakeMove( ins->operands[ 0 ], LowPart( temp, U1 ), U1 );
    ins->operands[ 0 ] = temp;
    ChangeType( ins, U2 );
    MoveSegOp( ins, new_ins, 0 );
    PrefixIns( ins, new_ins );
    return( new_ins );
}


extern  instruction     *rEXT_PUSH2( instruction *ins )
/*****************************************************/
/* NOT NEEDED */
{
    return( ins );
}


extern  instruction     *rINTCOMP( instruction *ins )
/***************************************************/
{
    name                *left;
    name                *rite;
    instruction         *low;
    instruction         *high;
    type_class_def      half_class;
    type_class_def      quarter_class;
    byte                true_idx;
    byte                false_idx;
    byte                first_idx;
    bool                rite_is_zero;

    rite_is_zero = CFTest( ins->operands[1]->c.value ) == 0;
    half_class = HalfClass[  ins->type_class  ];
    left = ins->operands[ 0 ];
    rite = ins->operands[ 1 ];
    true_idx = _TrueIndex( ins );
    false_idx = _FalseIndex( ins );
    if( ins->head.opcode == OP_CMP_EQUAL ) {
         first_idx = false_idx;
    } else {
         first_idx = true_idx;
    }
    if( ins->type_class == FD ) {
        quarter_class = HalfClass[ half_class ];

        if( rite_is_zero ) {
            high = MakeCondition( OP_BIT_TEST_TRUE,
                        HighPart( HighPart( left, half_class ), quarter_class ),
                        AllocIntConst( 0x7fff ),
                        first_idx, NO_JUMP, quarter_class );
        } else {
            high = MakeCondition( OP_CMP_NOT_EQUAL,
                        HighPart( HighPart( left, half_class ), quarter_class ),
                        HighPart( HighPart( rite, half_class ), quarter_class ),
                        first_idx, NO_JUMP, quarter_class );
        }
        DupSeg( ins, high );
        PrefixIns( ins, high );
        low = MakeCondition( OP_CMP_NOT_EQUAL,
                        LowPart( HighPart( left, half_class ), quarter_class ),
                        LowPart( HighPart( rite, half_class ), quarter_class ),
                        first_idx, NO_JUMP, quarter_class );
        DupSeg( ins, low );
        PrefixIns( ins, low );
        low = MakeCondition( OP_CMP_NOT_EQUAL,
                        HighPart( LowPart( left, half_class ), quarter_class ),
                        HighPart( LowPart( rite, half_class ), quarter_class ),
                        first_idx, NO_JUMP, quarter_class );
        DupSeg( ins, low );
        PrefixIns( ins, low );
        low = MakeCondition( ins->head.opcode,
                        LowPart( LowPart( left, half_class ), quarter_class ),
                        LowPart( LowPart( rite, half_class ), quarter_class ),
                        true_idx, false_idx, quarter_class );
    } else {
        rite = IntEquivalent( rite );
        if( rite_is_zero ) {
            high = MakeCondition( OP_BIT_TEST_TRUE,
                        HighPart( left, half_class ),
                        AllocIntConst( 0x7fff ),
                        first_idx, NO_JUMP, half_class );
        } else {
            high = MakeCondition( OP_CMP_NOT_EQUAL,
                        HighPart( left, half_class ),
                        HighPart( rite, half_class ),
                        first_idx, NO_JUMP, half_class );
        }
        DupSeg( ins, high );
        PrefixIns( ins, high );
        low = MakeCondition( ins->head.opcode,
                        LowPart( left, half_class ),
                        LowPart( rite, half_class ),
                        true_idx, false_idx,
                        half_class );
    }
    DupSeg( ins, low );
    ReplIns( ins, low );
    return( high );
}

extern  instruction     *rCONVERT_UP( instruction *ins ) { return( ins ); }
extern  instruction     *rCYP_SEX( instruction *ins ) { return( ins ); }
