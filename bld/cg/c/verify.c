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
* Description:  Generic instruction operand verification routines.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "cfloat.h"
#include "zoiks.h"
#include "data.h"
#include "split.h"
#include "namelist.h"


extern  int             GetLog2(unsigned_32);

extern  opcode_entry    DoNop[];

static  bool    SameLocation( name *n1, name *n2 ) {
/**************************************************/
    type_length loc1, loc2;
    name        *base;

    if( n1->t.location == NO_LOCATION ) return( false );
    if( n2->t.location == NO_LOCATION ) return( false );
    base = DeAlias( n1 );
    loc1 = base->t.location + n1->v.offset - base->v.offset;
    base = DeAlias( n2 );
    loc2 = base->t.location + n2->v.offset - base->v.offset;
    if( _IsFloating( n1->n.name_class ) || _IsFloating( n2->n.name_class ) ) {
        if( n1->n.name_class != n2->n.name_class ) return( false );
    }
    return( loc1 == loc2 );
}


static  bool    NextCmp( instruction *ins ) {
/*********************************************/

    instruction *next;

    next = ins->head.next;
    if( !_OpIsCondition( next->head.opcode ) ) return( false );
    if( next->table == DoNop ) return( true );
    if( next->u.gen_table == NULL ) return( false );
    if( next->u.gen_table->generate != G_NO ) return( false );
    return( true );
}


static  bool    IsMin( name *op, type_class_def class ) {
/********************************************************
    Verify if "op" is a constant which is the smallest of possible
    values for type "class".
*/

    if( op->c.const_type == CONS_ABSOLUTE ) {
        switch( class ) {
        case U1:
        case U2:
        case U4:
            if( CFIsI16( op->c.value ) && op->c.lo.int_value == 0 ) return( true );
            break;
        case I1:
            if( CFIsI8( op->c.value ) && op->c.lo.int_value == 0x80 )
                return( true );
            break;
        case I2:
            if( CFIsI16( op->c.value ) && op->c.lo.int_value == 0x8000 )
                return( true );
            break;
        case I4:
            if( CFIsI32( op->c.value ) && op->c.lo.int_value == 0x80000000 )
                return( true );
            break;
        }
    }
    return( false );
}

static  bool    IsMax( name *op, type_class_def class ) {
/********************************************************
    Verify if "op" is a constant which is the largest of possible
    values for type "class".
*/

    if( op->c.const_type == CONS_ABSOLUTE ) {
        switch( class ) {
        case U1:
            if( CFIsU8( op->c.value ) && op->c.lo.int_value == 0xff )
                return( true );
            break;
        case U2:
            if( CFIsU16( op->c.value ) && op->c.lo.int_value == 0xffff )
                return( true );
            break;
        case U4:
            if( CFIsU32( op->c.value ) && op->c.lo.int_value == 0xffffffff )
                return( true );
            break;
        case I1:
            if( CFIsU8( op->c.value ) && op->c.lo.int_value == 0x7f )
                return( true );
            break;
        case I2:
            if( CFIsU16( op->c.value ) && op->c.lo.int_value == 0x7fff )
                return( true );
            break;
        case I4:
            if( CFIsU32( op->c.value ) && op->c.lo.int_value == 0x7fffffff )
                return( true );
            break;
        }
    }
    return( false );
}

static  bool    Op2Pow2( instruction *ins ) {
/*******************************************/

    int         log;

    log = GetLog2( ins->operands[1]->c.lo.int_value );
    if( log == -1 ) return( false );
    if( log == ( ( TypeClassSize[ins->type_class] * 8 ) - 1 ) ) {
        if( Unsigned[ins->type_class] != ins->type_class ) {
            return( false );
        }
    }
    return( true );
}

extern  bool    OtherVerify( vertype kind, instruction *ins,
                             name *op1, name *op2, name *result ) {
/******************************************************************
    verify a if "kind" is true about instruction "ins" whose operands
    are "op1", "op2", "result".  This the target independant code.  We
    want to know these things for the 8086, 386 and 370 compilers.
*/

    switch( kind ) {
    case V_NO:
        /* This can get called if it's an optimization reduction that's
            no good for volatile instructions */
        return( true );
    case V_CMPEQ:
        if( ins->head.opcode == OP_CMP_EQUAL ) return( true );
        if( ins->head.opcode == OP_CMP_NOT_EQUAL ) return( true );
        break;
    case V_CMPFALSE:
        if( ins->result != NULL ) return ( false );
        if( NextCmp( ins ) ) return( false );
        if( op1 == op2 ) {
            if( ins->head.opcode == OP_CMP_NOT_EQUAL
             || ins->head.opcode == OP_CMP_GREATER
             || ins->head.opcode == OP_CMP_LESS ) {
                return( true );
            }
        }
        if( op2->n.class == N_CONSTANT ) {
            if( ins->head.opcode == OP_CMP_LESS ) {
                return( IsMin( op2, ins->type_class ) );
            } else if( ins->head.opcode == OP_CMP_GREATER ) {
                return( IsMax( op2, ins->type_class ) );
            } else if( ins->head.opcode == OP_BIT_TEST_TRUE ) {
                return( OtherVerify( V_OP2ZERO, ins, op1, op2, result ) );
            }
        }
        break;
    case V_CMPTRUE:
        if( ins->result != NULL ) return ( false );
        if( NextCmp( ins ) ) return( false );
        if( op1 == op2 ) {
            if( ins->head.opcode == OP_CMP_EQUAL
             || ins->head.opcode == OP_CMP_GREATER_EQUAL
             || ins->head.opcode == OP_CMP_LESS_EQUAL ) {
                return( true );
            }
        }
        if( op2->n.class == N_CONSTANT ) {
            if( ins->head.opcode == OP_CMP_GREATER_EQUAL ) {
                return( IsMin( op2, ins->type_class ) );
            } else if( ins->head.opcode == OP_CMP_LESS_EQUAL ) {
                return( IsMax( op2, ins->type_class ) );
            } else if( ins->head.opcode == OP_BIT_TEST_FALSE ) {
                return( OtherVerify( V_OP2ZERO, ins, op1, op2, result ) );
            }
        }
        break;
    case V_OFFSETZERO:
        if( op1->n.class == N_INDEXED && op1->i.constant == 0
         && !HasTrueBase( op1 ) ) return( true );
        break;
    case V_OP1ONE:
        if( op1->c.const_type == CONS_ABSOLUTE && op1->c.lo.int_value == 1 )
            return( true );
        break;
    case V_OP1RELOC:
        if( op1->n.class == N_CONSTANT
         && op1->c.const_type != CONS_ABSOLUTE ) return( true );
        break;
    case V_OP1ZERO:
        if( op1->c.const_type == CONS_ABSOLUTE
         && CFIsI16( op1->c.value )
         && op1->c.lo.int_value == 0 ) return( true );
        break;
    case V_OP2NEG:
        if( ( op2->c.const_type == CONS_ABSOLUTE )
         && ( op2->c.lo.int_value < 0 )
         && ( op2->c.lo.int_value & 0xffff ) != 0x8000
         && ( op2->c.lo.int_value != 0x80000000 ) ) return( true );
        break;
    case V_OP2ONE:
        if( op2->c.const_type == CONS_ABSOLUTE
         && op2->c.lo.int_value == 1 ) return( true );
        break;
    case V_OP2POW2:
        if( !CFIsI32( op2->c.value ) ) return( false );
        return( Op2Pow2( ins ) );
        break;
    case V_OP2TWO:
        if( op2->c.const_type == CONS_ABSOLUTE
         && op2->c.lo.int_value == 2 ) return( true );
        break;
    case V_OP2ZERO:
        if( op2->c.const_type == CONS_ABSOLUTE
         &&  CFIsI16( op2->c.value )
         &&  op2->c.lo.int_value == 0 ) return( true );
        break;
    case V_OP2_FFFFFFFF:
        if( op2->c.lo.int_value == 0xFFFFFFFF ) return( true );
        break;
    case V_SAME_LOCN:
         if( op1->n.class != N_TEMP ) return( false );
         if( result->n.class != N_TEMP ) return( false );
         return( SameLocation( result, op1 ) );
    case V_SAME_TYPE:
        /* if we have a constant, the name_class of op1 is bogus */
        if( op1->n.class == N_CONSTANT ) {
            if( ins->type_class == result->n.name_class ) return( true );
        } else {
            if( op1->n.name_class == result->n.name_class ) return( true );
        }
        break;
    case V_SPEED:
        if( OptForSize <= 50 ) return( true );
        break;
    case V_SWAP_GOOD:
        switch( op2->n.class ) {
        case N_REGISTER:
        case N_CONSTANT:
            return( false );
        case N_TEMP:
            if( op2->t.temp_flags & CONST_TEMP ) return( false );
            /* fall through */
        default:
            if( op1 != result ) return( true );
            break;
        }
        break;
    case V_U_TEST:
        if( op2->c.const_type != CONS_ABSOLUTE ) return( false );
        if( !CFIsI16( op2->c.value ) ) return( false );
        if( op2->c.lo.int_value != 0 ) return( false );
        if( ins->head.opcode == OP_CMP_EQUAL
         || ins->head.opcode == OP_CMP_NOT_EQUAL
         || ins->type_class == U4 ) return( true );
        break;
    case V_SHIFT2BIG:
        /* check if shift amount is equal to or greater than register width */
        if( op2->c.const_type == CONS_ABSOLUTE
         && op2->c.lo.int_value >= REG_SIZE * 8 ) return( true );
        break;
    default:
        _Zoiks( ZOIKS_053 );
        return( false );
    }
    return( false );
}
