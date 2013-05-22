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
* Description:  Instruction verification routines for RISC architectures.
*
****************************************************************************/


#include "cgstd.h"
#include <assert.h>
#include <stdlib.h>
#include "cgdefs.h"
#include "coderep.h"
#include "vergen.h"
#include "data.h"
#include "feprotos.h"

extern  type_length     FlagsToAlignment( i_flags );

extern  bool    OtherVerify( vertype, instruction *, name *, name *, name * );

extern  type_class_def  Unsigned[];

static  bool    ByteConst( name *operand )
/****************************************/
{
    if( operand->n.class == N_CONSTANT ) {
        if( operand->c.const_type == CONS_ABSOLUTE ) {
            if( operand->c.int_value_2 == 0 ) {
                return( operand->c.int_value >= 0 &&
                        operand->c.int_value <= 255 );
            }
        }
    }
    return( FALSE );
}

static  bool    HalfWordConst( name *operand )
/********************************************/
{
    if( operand->n.class == N_CONSTANT ) {
        if( operand->c.const_type == CONS_ABSOLUTE ) {
            if( operand->c.int_value_2 == 0 ) {
                return( operand->c.int_value >= -32768 &&
                            operand->c.int_value <= 32767 );
            } else {
                return( operand->c.int_value_2 == -1 &&
                    operand->c.int_value <= 0 &&
                    operand->c.int_value >= -32768 );
            }
        }
    }
    return( FALSE );
}

static  bool    UHalfWordConst( name *operand )
/*********************************************/
{
    if( operand->n.class == N_CONSTANT ) {
        if( operand->c.const_type == CONS_ABSOLUTE ) {
            if( operand->c.int_value_2 == 0 ) {
                return( operand->c.int_value >= 0 &&
                            operand->c.int_value <= 0xffff );
            }
        }
    }
    return( FALSE );
}

static  bool    Is64BitConst( name *operand )
/*******************************************/
{
    // Return TRUE if constant is not a 32-bit (canonical) const
    // A canonical 64-bit constant is one whose bits 63:32 == bit 31
    if( operand->c.const_type == CONS_ABSOLUTE ) {
        if( operand->c.int_value_2 != (operand->c.int_value >> 31) ) {
            return( TRUE );
        }
    }
    return( FALSE );
}

static  bool    Symmetric( opcode_defs opcode )
/*********************************************/
{
    switch( opcode ) {
    case OP_ADD:
    case OP_EXT_ADD:
    case OP_MUL:
    case OP_EXT_MUL:
    case OP_AND:
    case OP_OR:
    case OP_XOR:
        return( TRUE );
    }
    return( FALSE );
}

static  type_class_def  InsTypeClass( instruction *ins )
/******************************************************/
{
    if( ins->head.opcode == OP_CONVERT ) {
        return( ins->base_type_class );
    }
    return( ins->type_class );
}

static  bool    Aligned( name *op, type_length align, type_class_def tipe )
/*************************************************************************/
{
    type_length         natural;
    type_length         actual;

    assert( align <= 8 );
    assert( op->n.class == N_TEMP || op->n.class == N_MEMORY || op->n.class == N_INDEXED );
    if( tipe == XX ) {
        natural = 1;    // FIXME - should be largest element of this structure
    } else {
        natural = TypeClassSize[ tipe ];
    }
    if( natural == 2 ) {
        if( _IsntTargetModel( ALIGNED_SHORT ) ) {
            return( FALSE );
        }
    }
    switch( op->n.class ) {
    case N_MEMORY:
        if( op->m.alignment != 0 ) {
            return( align <= op->m.alignment );
        }
        // Note: this assumes we always put global elements on 8-byte
        // boundaries
        // correction - front end only puts globals on 4-byte boundaries
        if( align == 8 ) return( FALSE );
        /* fall through */
    case N_TEMP:
        // Note: this assumes we are not packing elements of size < 8 into
        // 8-byte chunks of the stack - we may do this in the future
        return( ( op->v.offset % align ) == 0 );
    case N_INDEXED:
        actual = natural;
        if( HasAlignment( op ) ) {
            actual = FlagsToAlignment( op->i.index_flags );
        }
        if( ( op->i.constant % 8 ) != 0 ) {
            actual = min( actual, op->i.constant & 0x07 );
        }
        return( align <= actual );
    }
    return( FALSE );
}

extern  bool    DoVerify( vertype kind, instruction *ins )
/********************************************************/
{
    name                *op;

    switch( kind ) {
    case V_BYTECONST2:
        return( ByteConst( ins->operands[ 1 ] ) );
    case V_BYTECONST1:
        return( ByteConst( ins->operands[ 0 ] ) );
    case V_SYMMETRIC:
        return( Symmetric( ins->head.opcode ) );
    case V_OP1HIGHADDR:
        return( ( ins->operands[ 0 ]->n.class == N_CONSTANT ) &&
                ( ins->operands[ 0 ]->c.const_type == CONS_HIGH_ADDR ) );
    case V_UHALFWORDCONST2:
        return( UHalfWordConst( ins->operands[ 1 ] ) );
    case V_UHALFWORDCONST1:
        return( UHalfWordConst( ins->operands[ 0 ] ) );
    case V_HALFWORDCONST2:
#if _TARGET & _TARG_AXP
        if( ins->type_class == Unsigned[ ins->type_class ] &&
            ( ins->operands[ 1 ]->c.int_value & 0x8000 ) &&
            TypeClassSize[ ins->type_class ] >= 4 ) return( FALSE );
#endif
        return( HalfWordConst( ins->operands[ 1 ] ) );
    case V_HALFWORDCONST1:
#if _TARGET & _TARG_AXP
        if( ins->type_class == Unsigned[ ins->type_class ] &&
            ( ins->operands[ 0 ]->c.int_value & 0x8000 ) &&
            TypeClassSize[ ins->type_class ] >= 4 ) return( FALSE );
#endif
        return( HalfWordConst( ins->operands[ 0 ] ) );
    case V_AXPBRANCH:   // FIXME: appears to be unused!
        op = ins->operands[ 1 ];
        return( ins->result == NULL && op->n.class == N_CONSTANT &&
                op->c.const_type == CONS_ABSOLUTE && op->c.int_value == 0 );
    case V_MIPSBRANCH:
        return( ins->result == NULL && (ins->head.opcode == OP_CMP_EQUAL
                || ins->head.opcode == OP_CMP_NOT_EQUAL) );
    case V_RESNOTNULL:
        return( ins->result != NULL );
    case V_RESNULL:
        return( ins->result == NULL );
    case V_WORD_OR_QUAD:
        return( TypeClassSize[ ins->type_class ] == 4 ||
                    TypeClassSize[ ins->type_class ] == 8 );
    case V_FLOAT:
        return( _IsFloating( ins->type_class ) );
    case V_REG_SIZE:
        assert( ins->type_class == XX );
        // FIXME: Alignment of structs not guaranteed to be equal to size
        switch( ins->operands[ 0 ]->n.size ) {
        case 1:
        case 2:
        case 4:
        case 8:
            return( TRUE );
        }
        return( FALSE );
    case V_BASE_8:
        switch( ins->base_type_class ) {
        case I8:
        case U8:
            return( TRUE );
        }
        return( FALSE );
    case V_BASE_4:
        switch( ins->base_type_class ) {
        case I4:
        case U4:
        case CP:
        case PT:
            return( TRUE );
        }
        return( FALSE );
    case V_UNSIGNED:
        return( Unsigned[ ins->type_class ] == ins->type_class );
    case V_OP1_AL2:
        return( Aligned( ins->operands[ 0 ], 2, InsTypeClass( ins ) ) );
    case V_OP1_AL4:
        return( Aligned( ins->operands[ 0 ], 4, InsTypeClass( ins ) ) );
    case V_OP1_AL8:
        return( Aligned( ins->operands[ 0 ], 8, InsTypeClass( ins ) ) );
    case V_RES_AL2:
        return( Aligned( ins->result, 2, ins->type_class ) );
    case V_RES_AL4:
        return( Aligned( ins->result, 4, ins->type_class ) );
    case V_RES_AL8:
        return( Aligned( ins->result, 8, ins->type_class ) );
    case V_OP1_RES_AL8:
        return( Aligned( ins->operands[ 0 ], 8, InsTypeClass( ins ) ) &&
                Aligned( ins->result, 8, InsTypeClass( ins ) ) );
    case V_OP1_RES_AL4:
        return( Aligned( ins->operands[ 0 ], 4, InsTypeClass( ins ) ) &&
                Aligned( ins->result, 4, InsTypeClass( ins ) ) );
    case V_RES_TEMP:
        return( ins->result->n.class == N_TEMP && ins->result->t.alias == NULL );
    case V_RESCONSTTEMP:
        return( _ConstTemp( ins->result ) );
    case V_OP164BITCONST:
        assert( ins->operands[ 0 ]->n.class == N_CONSTANT );
        return( Is64BitConst( ins->operands[ 0 ] ) );
    default:
        return( OtherVerify( kind, ins, ins->operands[ 0 ], ins->operands[ 1 ], ins->result ) );
    }
}
