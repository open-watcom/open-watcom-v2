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
* Description:  PowerPC instruction formats and encodings.
*
****************************************************************************/


#include "as.h"

typedef enum {
    UNNAMED,            // unnamed reloc
    SYMBOLIC            // symbolic reloc
} asm_reloc_flag;

typedef struct asm_reloc {
    owl_reloc_type      type;
    union {
        int_32          label_num;
        sym_handle      sym;
    }                   target;
    uint_32             valid : 1;
    uint_32             is_unnamed : 1;
} asm_reloc;

typedef void (*fmt_func)( ins_table *, instruction *, uint_32 *, asm_reloc * );

typedef struct {
    fmt_func    func;
    op_type     ops[ MAX_OPERANDS ];
} ppc_format;

#define _SixBits( x )           ( (x) & 0x003f )
#define _TenBits( x )           ( (x) & 0x03ff )
#define _FiveBits( x )          ( (x) & 0x001f )
#define _EightBits( x )         ( (x) & 0x00ff )
#define _FourteenBits( x )      ( (x) & 0x3fff )
#define _TwentyFourBits( x )    ( (x) & 0x00ffffff )

// these correspond to letters on Appendix A table of Motorola refrence
#define _D( x )         ( _FiveBits(x) << 21 )
#define _S( x )         ( _FiveBits(x) << 21 )
#define _A( x )         ( _FiveBits(x) << 16 )
#define _B( x )         ( _FiveBits(x) << 11 )
#define _C( x )         ( _FiveBits(x) << 6 )
#define _LI( x )        ( _TwentyFourBits(x) << 2 )
#define _BO( x )        ( _FiveBits(x) << 21 )
#define _BI( x )        ( _FiveBits(x) << 16 )
#define _BD( x )        ( _FourteenBits(x) << 2 )
#define _SH( x )        ( _FiveBits(x) << 11 )
#define _MB( x )        ( _FiveBits(x) << 6 )
#define _ME( x )        ( _FiveBits(x) << 1 )
#define _CRM( x )       ( _EightBits(x) << 12 )
#define _TBR( x )       ( _TenBits(x) << 11 )
#define _SPR( x )       ( _TenBits(x) << 11 )
#define _FM( x )        ( _EightBits(x) << 17 )

#define _Opcode( x )    ( _SixBits(x) << 26 )
#define _Opcode2( x )   ( _TenBits(x) << 1 ) // some opcode2 includes OE

// sixteen bit signed immediate
#define _SignedImmed( x )       ( (x) & 0xffff )

#define _OE( x )        ( ( ( (x) & IF_SETS_OVERFLOW ) ? 1 : 0 ) << 10 )
#define _RC( x )        ( ( ( (x) & IF_SETS_CC ) ? 1 : 0 ) << 0 )
#define _AA( x )        ( ( ( (x) & IF_SETS_ABSOLUTE ) ? 1 : 0 ) << 1 )
#define _LK( x )        ( ( ( (x) & IF_SETS_LINK ) ? 1 : 0 ) << 0 )

#define _Longword_offset( x )   ( (x) >> 2 )

#define _IsAbsolute( x )        ( ( (x) & IF_SETS_ABSOLUTE ) ? 1 : 0 )

static owl_reloc_type reloc_translate[] = {
    OWL_RELOC_ABSOLUTE,     // Corresponds to ASM_RELOC_UNSPECIFIED
    OWL_RELOC_WORD,
    OWL_RELOC_HALF_HI,
    OWL_RELOC_HALF_HA,
    OWL_RELOC_HALF_LO,
    OWL_RELOC_JUMP_REL,
    OWL_RELOC_BRANCH_REL,
};

static int_32 SPRChkList[] = {
    0, 1, 4, 5, 6, 8, 9, 18, 19, 22, 25, 26, 27, 272, 273, 274, 275, 282, 287,
    528, 529, 530, 531, 532, 533, 534, 535, 1008, 1009, 1010, 1013, 1023
};

static bool ensureTypeCorrect( ins_operand *op, op_type type, uint_8 opIdx ) {
//****************************************************************************

    if( op->type != type ) {
        Error( IMPROPER_OPERAND, opIdx );
        return( FALSE );
    }
    return( TRUE );
}

static bool ensureOpAbsolute( ins_operand *op, uint_8 opIdx ) {
//*************************************************************

    if( ( op->flags & ( RELOC | UNNAMED_RELOC ) ) ) {
        Error( RELOC_NOT_ALLOWED, opIdx );
        return( FALSE );
    }
    return( TRUE );
}

static void doEncode2( uint_32 *buffer, ins_opcode p, uint r1, uint r2, uint imm, ins_flags flags ) {
//****************************************************************************************************************
// Use when there are 2 5-bit blocks (other than opcode blocks) and
// an immediate block (16-bit)

    *buffer = _Opcode( p ) | _D( r1 ) | _A( r2 ) | _SignedImmed( imm ) | _OE( flags ) | _RC( flags );
}

static void doEncode3( uint_32 *buffer, ins_opcode p, ins_opcode s, uint r1, uint r2, uint r3, ins_flags flags ) {
//****************************************************************************************************************
// Use when there are 3 5-bit blocks (other than opcode blocks)

    *buffer = _Opcode( p ) | _D( r1 ) | _A( r2 ) | _B( r3 ) | _OE( flags ) | _Opcode2( s ) | _RC( flags );
}

static void doEncode4( uint_32 *buffer, ins_opcode p, ins_opcode s, uint r1, uint r2, uint r3, uint r4, ins_flags flags ) {
//*************************************************************************************************************************
// Use when there are 4 5-bit blocks (other than opcode blocks)

    *buffer = _Opcode( p ) | _D( r1 ) | _A( r2 ) | _C( r3 ) | _B( r4 ) | _Opcode2( s ) | _RC( flags );
}

static void doEncode5( uint_32 *buffer, ins_opcode p, uint r1, uint r2, uint r3, uint r4, uint r5, ins_flags flags ) {
//********************************************************************************************************************
// Use when there are 5 5-bit blocks (other than opcode blocks) e.g. rlwimi

    *buffer = _Opcode( p ) | _S( r1 ) | _A( r2 ) | _SH( r3 ) | _MB( r4 ) | _ME( r5 ) | _RC( flags );
}

static void doEncodeBoBiBd( uint_32 *buffer, ins_opcode p, uint bo, uint bi, uint bd, ins_flags flags ) {
//*******************************************************************************************************

    *buffer = _Opcode( p ) | _BO( bo ) | _BI( bi ) | _BD( bd ) | _AA( flags ) | _LK( flags );
}

static void doEncodeBoBiOp2( uint_32 *buffer, ins_opcode p, ins_opcode q, uint bo, uint bi, ins_flags flags ) {
//*************************************************************************************************************

    *buffer = _Opcode( p ) | _BO( bo ) | _BI( bi ) | _Opcode2( q ) | _AA( flags ) | _LK( flags );
}

static void doEncodeSPR( uint_32 *buffer, ins_opcode p, ins_opcode q, uint regidx, uint spr, ins_flags flags ) {
//**************************************************************************************************************

    int_32      swapped_spr;

    swapped_spr = ( ( spr & 0x1f ) << 5 ) | ( ( spr & 0x3e0 ) >> 5 );
    *buffer = _Opcode( p ) | _Opcode2( q ) | _D( regidx ) |
              _SPR( swapped_spr ) | _RC( flags ) | _OE( flags );
}

static owl_reloc_type relocType( asm_reloc_type type, owl_reloc_type default_type, bool absolute ) {
//**************************************************************************************************
// The absolute flag is used for jump/branch only

    owl_reloc_type  ret;

    if( type == ASM_RELOC_UNSPECIFIED ) {
        if( default_type == OWL_RELOC_JUMP_REL && absolute ) {
            return( OWL_RELOC_JUMP_ABS );
        } else if( default_type == OWL_RELOC_BRANCH_REL && absolute ) {
            return( OWL_RELOC_BRANCH_ABS );
        }
        return( default_type );
    }
    switch( default_type ) {
    case OWL_RELOC_HALF_HI:
    case OWL_RELOC_HALF_HA:
    case OWL_RELOC_HALF_LO:
        if( ( ret = reloc_translate[type] ) != OWL_RELOC_HALF_HI &&
            ret != OWL_RELOC_HALF_HA && ret != OWL_RELOC_HALF_LO ) {
            Error( INVALID_RELOC_MODIFIER );
        }
        break;
    case OWL_RELOC_BRANCH_REL:
    case OWL_RELOC_JUMP_REL:
        if( ( ret = reloc_translate[type] ) != default_type ) {
            Error( INVALID_RELOC_MODIFIER );
            break;
        }
        if( ret == OWL_RELOC_JUMP_REL && absolute ) {
            ret = OWL_RELOC_JUMP_ABS;
        } else if( ret == OWL_RELOC_BRANCH_REL && absolute ) {
            ret = OWL_RELOC_BRANCH_ABS;
        }
        break;
    default:
        //Error( "internal - unexpected default type" );
        assert( FALSE );
        ret = OWL_RELOC_ABSOLUTE;
    }
    return( ret );
}

static void doReloc( asm_reloc *reloc, ins_operand *op, owl_reloc_type rtype, ins_flags flags ) {
//***********************************************************************************************
// If it is a jump reloc, we should pass it in as a relative no matter what
// and let relocType change it to absolute if AA bit is set.

    assert( rtype != OWL_RELOC_JUMP_ABS && rtype != OWL_RELOC_BRANCH_ABS );
    if( ( op->flags & RELOC ) == RELOC ) {
        reloc->target.sym = op->reloc.target.ptr;
        reloc->type = relocType( op->reloc.type, rtype, _IsAbsolute( flags ) );
        reloc->is_unnamed = 0;
        reloc->valid = 1;
    } else if( ( op->flags & UNNAMED_RELOC ) == UNNAMED_RELOC ) {
        reloc->target.label_num = op->reloc.target.label;
        reloc->type = relocType( op->reloc.type, rtype, _IsAbsolute( flags ) );
        reloc->is_unnamed = 1;
        reloc->valid = 1;
    }
}

static void ITBinary( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*********************************************************************************************
// Three operands line up in the order they will be in the encoding

    assert( ins->num_operands == 3 );
    doEncode3( buffer, table->primary, table->secondary,
        RegIndex( ins->operands[0]->reg ), RegIndex( ins->operands[1]->reg ),
        RegIndex( ins->operands[2]->reg ),
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITBinary2( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//**********************************************************************************************
// The first two operands are reversed in terms of bit positioning

    assert( ins->num_operands == 3 );
    doEncode3( buffer, table->primary, table->secondary,
        RegIndex( ins->operands[1]->reg ), RegIndex( ins->operands[0]->reg ),
        RegIndex( ins->operands[2]->reg ),
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITBinaryImmed( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//**************************************************************************************************

    ins_operand *op;

    assert( ins->num_operands == 3 );
    op = ins->operands[2];
    doEncode2( buffer, table->primary, RegIndex( ins->operands[0]->reg ),
        RegIndex( ins->operands[1]->reg ), op->constant,
        ( ins->format->flags & table->optional ) | table->required );
    doReloc( reloc, op, OWL_RELOC_HALF_LO, 0 );
}

static void ITBinaryImmed2( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//***************************************************************************************************

    ins_operand *op;

    assert( ins->num_operands == 3 );
    op = ins->operands[2];
    doEncode2( buffer, table->primary, RegIndex( ins->operands[1]->reg ),
        RegIndex( ins->operands[0]->reg ), op->constant,
        ( ins->format->flags & table->optional ) | table->required );
    doReloc( reloc, op, OWL_RELOC_HALF_LO, 0 );
}

static void ITUnary( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//********************************************************************************************

    assert( ins->num_operands == 2 );
    doEncode3( buffer, table->primary, table->secondary,
        RegIndex( ins->operands[0]->reg ), RegIndex( ins->operands[1]->reg ), 0,
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITUnary2( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*********************************************************************************************

    assert( ins->num_operands == 2 );
    doEncode3( buffer, table->primary, table->secondary,
        RegIndex( ins->operands[1]->reg ), RegIndex( ins->operands[0]->reg ), 0,
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITFPBin( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//********************************************************************************************

    ITBinary( table, ins, buffer, reloc );
}


static void ITFPUnary( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//**********************************************************************************************

    assert( ins->num_operands == 2 );
    doEncode3( buffer, table->primary, table->secondary,
        RegIndex( ins->operands[0]->reg ), 0, RegIndex( ins->operands[1]->reg ),
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITFPCmp( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//********************************************************************************************

    assert( ins->num_operands == 3 );
    doEncode3( buffer, table->primary, table->secondary,
        RegIndex( ins->operands[0]->reg ) << 2,
        RegIndex( ins->operands[1]->reg ), RegIndex( ins->operands[2]->reg ),
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITFPMulAdd( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//***********************************************************************************************

    assert( ins->num_operands == 4 );
    doEncode4( buffer, table->primary, table->secondary,
        RegIndex( ins->operands[0]->reg ), RegIndex( ins->operands[1]->reg ),
        RegIndex( ins->operands[2]->reg ), RegIndex( ins->operands[3]->reg ),
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITFPMul( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//********************************************************************************************

    assert( ins->num_operands == 3 );
    doEncode4( buffer, table->primary, table->secondary,
        RegIndex( ins->operands[0]->reg ), RegIndex( ins->operands[1]->reg ),
        RegIndex( ins->operands[2]->reg ), 0,
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITBranch( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*********************************************************************************************

    ins_operand *op;
    ins_flags   flags;

    assert( ins->num_operands == 1 );
    flags = ( ins->format->flags & table->optional ) | table->required;
    op = ins->operands[0];
    *buffer = _Opcode( table->primary ) |
              _LI( _Longword_offset( op->constant ) ) |
              _AA( flags ) | _LK( flags );
    doReloc( reloc, op, OWL_RELOC_JUMP_REL, flags );
}

static void ITBranchCond( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*************************************************************************************************

    ins_operand *op[3];
    ins_flags   flags;
    int         ctr;

    assert( ins->num_operands == 3 );
    for( ctr = 0; ctr < 3; ++ctr ) {
        op[ctr] = ins->operands[ctr];
        if( ( ctr < 2 ) && !ensureOpAbsolute( op[ctr], ctr ) ) return;
    }
    flags = ( ins->format->flags & table->optional ) | table->required;
    doEncodeBoBiBd( buffer, table->primary, op[0]->constant, op[1]->constant,
        _Longword_offset( op[2]->constant ), flags );
    doReloc( reloc, op[2], OWL_RELOC_BRANCH_REL, flags );
}

static void ITBranchSpec( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*************************************************************************************************

    ins_operand *op[2];
//    ins_flags   flags;
    int         ctr;

    assert( ins->num_operands == 2 );
//    flags = ( ins->format->flags & table->optional ) | table->required;
    for( ctr = 0; ctr < 2; ++ctr ) {
        op[ctr] = ins->operands[ctr];
        if( !ensureOpAbsolute( op[ctr], ctr ) ) return;
    }
    doEncodeBoBiOp2( buffer, table->primary, table->secondary,
        op[0]->constant, op[1]->constant,
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITCmp( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//******************************************************************************************

    ins_operand *op[4], *opRa, *opRb;
    uint        crfIdx, L_bit, ctr;
    ins_opcount opcount;
    op_type     verify4[4] = { OP_CRF, OP_IMMED, OP_GPR, OP_GPR };
    op_type     verify3[3] = { OP_IMMED, OP_GPR, OP_GPR };
    op_type     *verify;

    assert( ins->num_operands == 3 || ins->num_operands == 4 );
    opcount = ins->num_operands;
    verify = ( opcount == 3 ) ?  verify3 : verify4;
    for( ctr = 0; ctr < opcount; ctr++ ) {
        if( !ensureTypeCorrect( op[ctr] = ins->operands[ctr], verify[ctr], ctr ) ) return;
    }
    if( opcount == 4 ) {
        crfIdx = RegIndex( op[0]->reg );
        ctr = 1;
    } else {    // 3 operands
        crfIdx = 0;
        ctr = 0;
    }
    L_bit = op[ctr]->constant;
    if( ( L_bit & 1 ) != L_bit ) {
        Error( OP_OUT_OF_RANGE, ctr );
    }
    if( !ensureOpAbsolute( op[ctr], ctr ) ) return;
    opRa = op[++ctr];
    opRb = op[++ctr];
    doEncode3( buffer, table->primary, table->secondary,
        ( crfIdx << 2 ) | L_bit,
        RegIndex( opRa->reg ), RegIndex( opRb->reg ),
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITCmpImmed( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//***********************************************************************************************

    ins_operand *op[4], *opRa, *opSimm;
    uint        crfIdx, L_bit, ctr;
    ins_opcount opcount;
    op_type     verify4[4] = { OP_CRF, OP_IMMED, OP_GPR, OP_IMMED };
    op_type     verify3[3] = { OP_IMMED, OP_GPR, OP_IMMED };
    op_type     *verify;

    assert( ins->num_operands == 3 || ins->num_operands == 4 );
    opcount = ins->num_operands;
    verify = ( opcount == 3 ) ?  verify3 : verify4;
    for( ctr = 0; ctr < opcount; ctr++ ) {
        if( !ensureTypeCorrect( op[ctr] = ins->operands[ctr], verify[ctr], ctr ) ) return;
    }
    if( opcount == 4 ) {
        crfIdx = RegIndex( op[0]->reg );
        ctr = 1;
    } else {    // 3 operands
        crfIdx = 0;
        ctr = 0;
    }
    L_bit = op[ctr]->constant;
    if( ( L_bit & 1 ) != L_bit ) {
        Error( OP_OUT_OF_RANGE, ctr );
    }
    if( !ensureOpAbsolute( op[ctr], ctr ) ) return;
    opRa = op[++ctr];
    opSimm = op[++ctr];
    doEncode2( buffer, table->primary, ( crfIdx << 2 ) | L_bit,
        RegIndex( opRa->reg ), opSimm->constant,
        ( ins->format->flags & table->optional ) | table->required );
    doReloc( reloc, opSimm, OWL_RELOC_HALF_LO, 0 );
}

static void ITCondBit( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//**********************************************************************************************

    assert( ins->num_operands == 3 );
    doEncode3( buffer, table->primary, table->secondary,
        RegIndex( ins->operands[0]->reg ), RegIndex( ins->operands[1]->reg ),
        RegIndex( ins->operands[2]->reg ),
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITCondUn( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*********************************************************************************************

    assert( ins->num_operands == 2 );
    doEncode3( buffer, table->primary, table->secondary,
        RegIndex( ins->operands[0]->reg ) << 2,
        RegIndex( ins->operands[1]->reg ) << 2, 0,
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITMcrxr( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//********************************************************************************************

    assert( ins->num_operands == 1 );
    doEncode3( buffer, table->primary, table->secondary,
        RegIndex( ins->operands[0]->reg ) << 2, 0, 0,
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITMfcr( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*******************************************************************************************

    assert( ins->num_operands == 1 );
    doEncode3( buffer, table->primary, table->secondary,
        RegIndex( ins->operands[0]->reg ), 0, 0,
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITMffs( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*******************************************************************************************

    ITMfcr( table, ins, buffer, reloc );
}

static bool SPRValidate( int_32 spr ) {
//*************************************

    int n, ctr;
    bool status = FALSE;

    if( ( spr & 0x3ff ) != spr ) return( FALSE );
    n = sizeof( SPRChkList ) / sizeof( SPRChkList[0] );
    for( ctr = 0; !status && ctr < n; ++ctr ) {
        status = ( spr == SPRChkList[ ctr ] ) ? TRUE : FALSE;
    }
    return( status );
}

static void ITMfspr( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//********************************************************************************************

    ins_operand *const_op;

    assert( ins->num_operands == 2 );
    const_op = ins->operands[1];
    if( !SPRValidate( const_op->constant ) ) {
        Error( BAD_REG_ENCODING, 1, "SPR" );
    }
    if( !ensureOpAbsolute( const_op, 1 ) ) return;
    doEncodeSPR( buffer, table->primary, table->secondary,
        RegIndex( ins->operands[0]->reg ), const_op->constant,
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITMfsr( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*******************************************************************************************

    ins_operand *const_op;

    assert( ins->num_operands == 2 );
    const_op = ins->operands[1];
    if( const_op->constant > 0xf ) {
        Error( OP_OUT_OF_RANGE, 1 );
    }
    if( !ensureOpAbsolute( const_op, 1 ) ) return;
    doEncode3( buffer, table->primary, table->secondary,
        RegIndex( ins->operands[0]->reg ), const_op->constant, 0,
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITMfsrin( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*********************************************************************************************

    assert( ins->num_operands == 2 );
    doEncode3( buffer, table->primary, table->secondary,
        RegIndex( ins->operands[0]->reg ), 0, RegIndex( ins->operands[1]->reg ),
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITMftb( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*******************************************************************************************

    ins_operand *const_op;
    ins_flags   flags;

    assert( ins->num_operands == 2 );
    const_op = ins->operands[1];
    if( const_op->constant != 0x188 && const_op->constant != 0x1a8 ) {
        Error( BAD_REG_ENCODING, 1, "TBR" );
    }
    if( !ensureOpAbsolute( const_op, 1 ) ) return;
    flags = ( ins->format->flags & table->optional ) | table->required;
    *buffer = _Opcode( table->primary ) | _Opcode2( table->secondary ) |
              _D( RegIndex( ins->operands[0]->reg ) ) |
              _TBR( const_op->constant ) | _OE( flags ) | _RC( flags );
}

static void ITMtcrf( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//********************************************************************************************

    ins_operand *const_op;
    ins_flags   flags;

    assert( ins->num_operands == 2 );
    const_op = ins->operands[0];
    if( const_op->constant > 0xff ) {
        Error( OP_OUT_OF_RANGE, 0 );
    }
    if( !ensureOpAbsolute( const_op, 0 ) ) return;
    flags = ( ins->format->flags & table->optional ) | table->required;
    *buffer = _Opcode( table->primary ) | _Opcode2( table->secondary ) |
              _S( RegIndex( ins->operands[1]->reg ) ) |
              _CRM( const_op->constant ) | _OE( flags ) | _RC( flags );
}

static void ITMtfsb( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//********************************************************************************************

    ITMfcr( table, ins, buffer, reloc );
}

static void ITMtfsf( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//********************************************************************************************

    ins_operand *opFM;
    ins_flags   flags;

    assert( ins->num_operands == 2 );
    opFM = ins->operands[0];
    if( opFM->constant > 0xff ) {
        Error( OP_OUT_OF_RANGE, 0 );
    }
    if( !ensureOpAbsolute( opFM, 0 ) ) return;
    flags = ( ins->format->flags & table->optional ) | table->required;
    *buffer = _Opcode( table->primary ) | _Opcode2( table->secondary ) |
              _FM( opFM->constant ) | _B( RegIndex( ins->operands[1]->reg ) ) |
              _OE( flags ) | _RC( flags );
}

static void ITMtfsfImmed( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*************************************************************************************************

    ins_operand *const_op;

    assert( ins->num_operands == 2 );
    const_op = ins->operands[1];
    if( const_op->constant > 0xff ) {
        Error( OP_OUT_OF_RANGE, 1 );
    }
    if( !ensureOpAbsolute( const_op, 1 ) ) return;
    doEncode3( buffer, table->primary, table->secondary,
        RegIndex( ins->operands[0]->reg ) << 2, 0, const_op->constant << 1,
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITMtspr( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//********************************************************************************************

    ins_operand *const_op;

    assert( ins->num_operands == 2 );
    const_op = ins->operands[0];
    if( !SPRValidate( const_op->constant ) ) {
        Error( BAD_REG_ENCODING, 0, "SPR" );
        return;
    }
    if( !ensureOpAbsolute( const_op, 0 ) ) return;
    doEncodeSPR( buffer, table->primary, table->secondary,
        RegIndex( ins->operands[1]->reg ), const_op->constant,
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITMtsr( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*******************************************************************************************

    ins_operand *const_op;

    assert( ins->num_operands == 2 );
    const_op = ins->operands[0];
    if( const_op->constant > 0xf ) {
        Error( OP_OUT_OF_RANGE, 0 );
    }
    if( !ensureOpAbsolute( const_op, 1 ) ) return;
    doEncode3( buffer, table->primary, table->secondary,
        RegIndex( ins->operands[1]->reg ), const_op->constant, 0,
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITDc( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*****************************************************************************************

    assert( ins->num_operands == 2 );
    doEncode3( buffer, table->primary, table->secondary,
        0, RegIndex( ins->operands[0]->reg ), RegIndex( ins->operands[1]->reg ),
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITEieio( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//********************************************************************************************

    assert( ins->num_operands == 0 );
    doEncode3( buffer, table->primary, table->secondary, 0, 0, 0,
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITFctid( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//********************************************************************************************

    assert( ins->num_operands == 2 );
    doEncode3( buffer, table->primary, table->secondary,
        RegIndex( ins->operands[0]->reg ), 0, RegIndex( ins->operands[1]->reg ),
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITLdStr( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//********************************************************************************************

    ins_operand *op;

    assert( ins->num_operands == 2 );
    op = ins->operands[1];
    doEncode2( buffer, table->primary, RegIndex( ins->operands[0]->reg ),
        RegIndex( op->reg ), op->constant,
        ( ins->format->flags & table->optional ) | table->required );
    doReloc( reloc, op, OWL_RELOC_HALF_LO, 0 );
}

static void ITLdStrIndex( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*************************************************************************************************

    assert( ins->num_operands == 3 );
    doEncode3( buffer, table->primary, table->secondary,
        RegIndex( ins->operands[0]->reg ), RegIndex( ins->operands[1]->reg ),
        RegIndex( ins->operands[2]->reg ),
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITLdStrFP( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//**********************************************************************************************

    ins_operand *op;

    assert( ins->num_operands == 2 );
    op = ins->operands[1];
    doEncode2( buffer, table->primary, RegIndex( ins->operands[0]->reg ),
        RegIndex( op->reg ), op->constant,
        ( ins->format->flags & table->optional ) | table->required );
    doReloc( reloc, op, OWL_RELOC_HALF_LO, 0 );
}

static void ITLdStrIndexFP( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//***************************************************************************************************

    ITBinary( table, ins, buffer, reloc );
}

static void ITLswi( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*******************************************************************************************

    ins_operand *op;

    assert( ins->num_operands == 3 );
    op = ins->operands[2];
    if( !ensureOpAbsolute( op, 2 ) ) return;
    doEncode3( buffer, table->primary, table->secondary,
        RegIndex( ins->operands[0]->reg ), RegIndex( ins->operands[1]->reg ),
        op->constant,
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITShift( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//********************************************************************************************

    ins_operand *op;

    assert( ins->num_operands == 3 );
    op = ins->operands[2];
    if( !ensureOpAbsolute( op, 2 ) ) return;
    doEncode3( buffer, table->primary, table->secondary,
        RegIndex( ins->operands[1]->reg ), RegIndex( ins->operands[0]->reg ),
        op->constant,
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITShiftImmed( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*************************************************************************************************

    ins_operand *const_op[3];
    int         ctr;

    assert( ins->num_operands == 5 );
    for( ctr = 0; ctr < 3; ++ctr ) {
        const_op[ctr] = ins->operands[ctr + 2];
        if( !ensureOpAbsolute( const_op[ctr], ctr + 2 ) ) return;
    }
    doEncode5( buffer, table->primary, RegIndex( ins->operands[1]->reg ),
        RegIndex( ins->operands[0]->reg ), const_op[0]->constant,
        const_op[1]->constant, const_op[2]->constant,
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITShiftIndex( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*************************************************************************************************

    ins_operand *const_op[2];
    int         ctr;

    assert( ins->num_operands == 5 );
    for( ctr = 0; ctr < 2; ++ctr ) {
        const_op[ctr] = ins->operands[ctr + 3];
        if( !ensureOpAbsolute( const_op[ctr], ctr + 3 ) ) return;
    }
    doEncode5( buffer, table->primary, RegIndex( ins->operands[1]->reg ),
        RegIndex( ins->operands[0]->reg ), RegIndex( ins->operands[2]->reg ),
        const_op[0]->constant, const_op[1]->constant,
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITTlbie( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//********************************************************************************************

    assert( ins->num_operands == 1 );
    doEncode3( buffer, table->primary, table->secondary, 0, 0,
        RegIndex( ins->operands[0]->reg ),
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITTrap( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*******************************************************************************************

    ins_operand *op;

    assert( ins->num_operands == 3 );
    op = ins->operands[0];
    if( !ensureOpAbsolute( op, 0 ) ) return;
    doEncode3( buffer, table->primary, table->secondary,
        op->constant, RegIndex( ins->operands[1]->reg ), RegIndex( ins->operands[2]->reg ),
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITTrapImmed( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//************************************************************************************************

    ins_operand *op0, *op2;

    assert( ins->num_operands == 3 );
    op0 = ins->operands[0];
    if( !ensureOpAbsolute( op0, 0 ) ) return;
    op2 = ins->operands[2];
    doEncode2( buffer, table->primary, op0->constant,
        RegIndex( ins->operands[1]->reg ), op2->constant,
        ( ins->format->flags & table->optional ) | table->required );
    doReloc( reloc, op2, OWL_RELOC_HALF_LO, 0 );
}

static void ITSMLoadImmed( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//**************************************************************************************************

    ins_operand *op;

    assert( ins->num_operands == 2 );
    op = ins->operands[1];
    doEncode2( buffer, table->primary, RegIndex( ins->operands[0]->reg ),
        0, op->constant,
        ( ins->format->flags & table->optional ) | table->required );
    doReloc( reloc, op, OWL_RELOC_HALF_LO, 0 );
}

static void ITSMLoadAddr( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*************************************************************************************************

    ins_operand *op;

    assert( ins->num_operands == 2 );
    op = ins->operands[1];
    doEncode2( buffer, table->primary, RegIndex( ins->operands[0]->reg ),
        RegIndex( op->reg ), op->constant,
        ( ins->format->flags & table->optional ) | table->required );
    doReloc( reloc, op, OWL_RELOC_HALF_LO, 0 );
}

static void ITSMMovReg( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//***********************************************************************************************

    assert( ins->num_operands == 2 );
    doEncode3( buffer, table->primary, table->secondary,
        RegIndex( ins->operands[1]->reg ), RegIndex( ins->operands[0]->reg ),
        RegIndex( ins->operands[1]->reg ),
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITSMSubImmed( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*************************************************************************************************

    ins_operand *op;

    assert( ins->num_operands == 3 );
    op = ins->operands[2];
    if( !ensureOpAbsolute( op, 0 ) ) return;
    doEncode2( buffer, table->primary, RegIndex( ins->operands[0]->reg ),
        RegIndex( ins->operands[1]->reg ), -op->constant,
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITSMCmpwi( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//**********************************************************************************************

    ins_operand *op0, *opRa, *opSimm;
    uint        crfIdx;

    assert( ins->num_operands == 2 || ins->num_operands == 3 );
    op0 = ins->operands[0];
    if( ins->num_operands == 3 ) {
        if( !ensureTypeCorrect( op0, OP_CRF, 0 ) ) return;
        crfIdx = RegIndex( op0->reg );
        opRa = ins->operands[1];
        if( !ensureTypeCorrect( opRa, OP_GPR, 1 ) ) return;
        opSimm = ins->operands[2];
        assert( opSimm->type == OP_IMMED );
    } else { // 2 operands
        crfIdx = 0;
        opRa = op0;
        opSimm = ins->operands[1];
        if( !ensureTypeCorrect( opRa, OP_GPR, 0 ) ) return;
        if( !ensureTypeCorrect( opSimm, OP_IMMED, 1 ) ) return;
    }
    doEncode2( buffer, table->primary, crfIdx << 2,
        RegIndex( opRa->reg ), opSimm->constant,
        ( ins->format->flags & table->optional ) | table->required );
    doReloc( reloc, opSimm, OWL_RELOC_HALF_LO, 0 );
}

static void ITSMCmpw( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*********************************************************************************************

    ins_operand *op0, *opRa, *opRb;
    uint        crfIdx;

    assert( ins->num_operands == 2 || ins->num_operands == 3 );
    op0 = ins->operands[0];
    if( ins->num_operands == 3 ) {
        if( !ensureTypeCorrect( op0, OP_CRF, 0 ) ) return;
        crfIdx = RegIndex( op0->reg );
        opRa = ins->operands[1];
        opRb = ins->operands[2];
        assert( opRa->type == OP_GPR );
        assert( opRb->type == OP_GPR );
    } else { // 2 operands
        crfIdx = 0;
        opRa = op0;
        opRb = ins->operands[1];
        if( !ensureTypeCorrect( opRa, OP_GPR, 0 ) ) return;
        assert( opRb->type == OP_GPR );
    }
    doEncode3( buffer, table->primary, table->secondary, crfIdx << 2,
        RegIndex( opRa->reg ), RegIndex( opRb->reg ),
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITSMExtlwi( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//***********************************************************************************************

    ins_operand *const_op[2];
    int         ctr;

    assert( ins->num_operands == 4 );
    for( ctr = 0; ctr < 2; ++ctr ) {
        const_op[ctr] = ins->operands[ctr + 2];
        if( !ensureOpAbsolute( const_op[ctr], ctr + 2 ) ) return;
    }
    doEncode5( buffer, table->primary, RegIndex( ins->operands[1]->reg ),
        RegIndex( ins->operands[0]->reg ), const_op[1]->constant, 0,
        const_op[0]->constant - 1,
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITSMSrwi( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*********************************************************************************************

    ins_operand *const_op;
    uint        n;
    uint        sh = 0;
    uint        mb = 0;
    uint        me = 0;

    assert( ins->num_operands == 3 );
    const_op = ins->operands[2];
    if( !ensureOpAbsolute( const_op, 2 ) ) return;
    n = const_op->constant;
    switch( table->special ) {
    case SRWI:
        sh = 32 - n; mb = n; me = 31;
        break;
    case CLRRWI:
        sh = 0; mb = 0; me = 31 - n;
        break;
    default:
        assert( FALSE );
    }
    doEncode5( buffer, table->primary, RegIndex( ins->operands[1]->reg ),
        RegIndex( ins->operands[0]->reg ), sh, mb, me,
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITSMRotlw( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//**********************************************************************************************

    assert( ins->num_operands == 3 );
    doEncode5( buffer, table->primary, RegIndex( ins->operands[1]->reg ),
        RegIndex( ins->operands[0]->reg ), RegIndex( ins->operands[2]->reg ),
        0, 31,
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITSMInslw( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//**********************************************************************************************

    ins_operand *const_op[2];
    uint        n, b;
    int         ctr;

    assert( ins->num_operands == 4 );
    for( ctr = 0; ctr < 2; ++ctr ) {
        const_op[ctr] = ins->operands[ctr + 2];
        if( !ensureOpAbsolute( const_op[ctr], ctr + 2 ) ) return;
    }
    n = const_op[0]->constant;
    b = const_op[1]->constant;
    doEncode5( buffer, table->primary, RegIndex( ins->operands[1]->reg ),
        RegIndex( ins->operands[0]->reg ), 32 - b, b, b + n - 1,
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITSMTrapUncond( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//***************************************************************************************************

    assert( ins->num_operands == 0 );
    doEncode3( buffer, table->primary, table->secondary, TO_ANY, 0, 0,
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITSMTrap( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*********************************************************************************************

    assert( ins->num_operands == 2 );
    doEncode3( buffer, table->primary, table->secondary, table->special,
        RegIndex( ins->operands[0]->reg ), RegIndex( ins->operands[1]->reg ),
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITSMTrapImmed( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//**************************************************************************************************

    ins_operand *op1;

    assert( ins->num_operands == 2 );
    op1 = ins->operands[1];
    doEncode2( buffer, table->primary, table->special,
        RegIndex( ins->operands[0]->reg ), op1->constant,
        ( ins->format->flags & table->optional ) | table->required );
    doReloc( reloc, op1, OWL_RELOC_HALF_LO, 0 );
}

static void ITSMMovSpr( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//***********************************************************************************************

    assert( ins->num_operands == 1 );
    assert( SPRValidate( table->special ) );
    doEncodeSPR( buffer, table->primary, table->secondary,
        RegIndex( ins->operands[0]->reg ), table->special,
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITSMMovSprNCover( bool isMoveTo, ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//********************************************************************************************************************

    ins_operand *op;
    uint        spr;
    int         idx0, idx1;

    assert( ins->num_operands == 2 );
    assert( SPRValidate( table->special ) );
    if( isMoveTo ) {
        idx0 = 0;
        idx1 = 1;
    } else {
        idx0 = 1;
        idx1 = 0;
    }
    op = ins->operands[idx0];
    if( !ensureOpAbsolute( op, idx0 ) ) return;
    if( ( op->constant & 3 ) != op->constant ) {
        Error( OP_OUT_OF_RANGE, idx0 );
        return;
    }
    spr = table->special;
    switch( table->special ) {
    case SPRG:
        spr += op->constant;
        break;
    case BATU:
    case BATL:
        spr += (op->constant << 1);
        break;
    default:
        assert( FALSE );
    }
    doEncodeSPR( buffer, table->primary, table->secondary,
        RegIndex( ins->operands[idx1]->reg ), spr,
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITSMMtsprN( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//***********************************************************************************************

    ITSMMovSprNCover( TRUE, table, ins, buffer, reloc );
}

static void ITSMMfsprN( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//***********************************************************************************************

    ITSMMovSprNCover( FALSE, table, ins, buffer, reloc );
}

static void ITSMBCIcc( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//**********************************************************************************************

    ins_operand *op0, *opBd;
    ins_special special;
    ins_flags   flags;
    uint        startBI;

    assert( ins->num_operands == 1 || ins->num_operands == 2 );
    op0 = ins->operands[0];
    if( ins->num_operands == 1 ) {
        if( !ensureTypeCorrect( op0, OP_IMMED, 0 ) ) return;
        opBd = op0;
        startBI = 0;
    } else { // ins->num_operands == 2
        opBd = ins->operands[1];
        assert( opBd->type == OP_IMMED );
        if( !ensureTypeCorrect( op0, OP_CRF, 0 ) ) return;
        startBI = RegCrfToBI( op0->reg );
    }
    special = table->special;
    flags = ( ins->format->flags & table->optional ) | table->required;
    doEncodeBoBiBd( buffer, table->primary, _BICC_BO( special ),
        startBI + _BICC_BI( special ), _Longword_offset( opBd->constant ), flags );
    doReloc( reloc, opBd, OWL_RELOC_BRANCH_REL, flags );
}

static void ITSMBC( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*******************************************************************************************
// Four possibilities; the remainder are all improper:
//   i) bdnz OP_IMMED(reloc okay)
//  ii) bdnz OP_CRF, OP_IMMED(reloc okay)
// iii) bdnz OP_BI, OP_IMMED(reloc okay)
//  iv) bdnz OP_IMMED(no reloc), OP_IMMED(reloc okay)

    ins_operand *op0;
    ins_operand *opBd = NULL;
    ins_flags   flags;
    uint        valueBI = 0;

    assert( ins->num_operands >= 1 && ins->num_operands <= 2 );
    op0 = ins->operands[0];
    switch( ins->num_operands ) {
    case 1: // case i
        if( !ensureTypeCorrect( op0, OP_IMMED, 0 ) ) return;
        valueBI = 0;
        opBd = op0;
        break;
    case 2: // cases ii, iii, iv
        opBd = ins->operands[1];
        if( !ensureTypeCorrect( opBd, OP_IMMED, 1 ) ) return;
        switch( op0->type ) {
        case OP_CRF:
            valueBI = RegCrfToBI( op0->reg );
            break;
        case OP_IMMED:
            if( !ensureOpAbsolute( op0, 0 ) ) return;
            // fall through
        case OP_BI:
            valueBI = op0->constant;
            break;
        default:
            assert( FALSE ); // validate should've taken care of the rest
        }
    }
    flags = ( ins->format->flags & table->optional ) | table->required;
    doEncodeBoBiBd( buffer, table->primary, table->special,
        valueBI, _Longword_offset( opBd->constant ), flags );
    doReloc( reloc, opBd, OWL_RELOC_BRANCH_REL, flags );
}

static void ITSMBSpecIcc( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*************************************************************************************************

#ifndef NDEBUG
    ins_operand *op0;
#endif
    ins_special special;
    uint        startBI;

    assert( ins->num_operands == 0 || ins->num_operands == 1 );
    if( ins->num_operands == 0 ) {
        startBI = 0;
    } else { // ins->num_operands == 1
#ifndef NDEBUG
        op0 = ins->operands[0];
        assert( op0->type == OP_CRF );
#endif
        startBI = RegCrfToBI( ins->operands[0]->reg );
    }
    special = table->special;
    doEncodeBoBiOp2( buffer, table->primary, table->secondary,
        _BICC_BO( special ), startBI + _BICC_BI( special ),
        ( ins->format->flags & table->optional ) | table->required );
}

static void ITSMBSpec( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//**********************************************************************************************
// Four possibilities; the remainder are all improper:
//   i) bdnzlr (no operand)
//  ii) bdnzlr OP_CRF
// iii) bdnzlr OP_BI
//  iv) bdnzlr OP_IMMED(no reloc)

    ins_operand *op0;
    uint        valueBI = 0;

    assert( ins->num_operands == 0 || ins->num_operands == 1 );
    switch( ins->num_operands ) {
    case 0: // case i
        valueBI = 0;
        break;
    case 1: // cases ii, iii, iv
        op0 = ins->operands[0];
        switch( op0->type ) {
        case OP_CRF:
            valueBI = RegCrfToBI( op0->reg );
            break;
        case OP_IMMED:
            if( !ensureOpAbsolute( op0, 0 ) ) return;
            // fall through
        case OP_BI:
            valueBI = op0->constant;
            break;
        default:
            assert( FALSE ); // validate should've taken care of the rest
        }
    }
    doEncodeBoBiOp2( buffer, table->primary, table->secondary,
        table->special, valueBI,
        ( ins->format->flags & table->optional ) | table->required );
}

ppc_format PPCFormatTable[] = {
    #define PICK( a, b, c, d, e, f, g ) { b, { c, d, e, f, g } },
    #include "ppcfmt.inc"
    #undef PICK
};

static void opError( instruction *ins, op_type actual, op_type wanted, int i ) {
//******************************************************************************
// Stuff out an error message.

    ins = ins;
    actual = actual;
    wanted = wanted;    // it's a set of flags
    if( ( wanted & OP_NOTHING ) != OP_NOTHING ) {
        Error( OPERAND_INCORRECT, i );
    } else {
        Error( OPERAND_UNEXPECTED, i );
    }
}

bool PPCValidate( instruction *ins ) {
//************************************
// Make sure that all operands of the given instruction
// are of the type we are expecting. If not, we print
// out an error message.

    int         i;
    ppc_format  *fmt;
    ins_operand *op;

    fmt = &PPCFormatTable[ ins->format->table_entry->template ];
    for( i = 0; i < ins->num_operands; i++ ) {
        op = ins->operands[ i ];
        if( ( op->type & fmt->ops[ i ] ) != op->type ) {
            opError( ins, op->type, fmt->ops[ i ], i );
            return( FALSE );
        }
    }
    if( i < MAX_OPERANDS ) {
        if( ( fmt->ops[ i ] & OP_NOTHING ) != OP_NOTHING ) {
            Error( NOT_ENOUGH_INSOP );
            return( FALSE );
        }
    }   // NOTE: It might not catch all improper operand conbinations
        // because we're using flags here
    return( TRUE );
}

#ifdef _STANDALONE_
void PPCEmit( owl_section_handle hdl, instruction *ins ) {
//********************************************************
#else
void PPCEmit( instruction *ins ) {
//********************************
#endif
// Encode the given instruction (including emitting any
// relocs to the appropriate places), and emit the code
// to the given section.

    ins_table       *table;
    uint_32         result;
    asm_reloc       reloc = { OWL_RELOC_ABSOLUTE, { 0 }, 0, 0 };
    #ifdef _STANDALONE_
    uint_8          oldAlignment;

    if( OWLTellSectionType( hdl ) & OWL_SEC_ATTR_BSS ) {
        Error( INVALID_BSS_STATEMENT );
        return;
    }
    oldAlignment = CurrAlignment;
    if( oldAlignment < 2 ) { // Instructions should at least be dword aligned
        CurrAlignment = 2;
    }
    #endif
    table = ins->format->table_entry;
    PPCFormatTable[ table->template ].func( table, ins, &result, &reloc );
    if( reloc.valid ) {
        #ifdef _STANDALONE_
        if( reloc.is_unnamed ) {
            ObjEmitReloc( hdl, &reloc.target.label_num, reloc.type, TRUE, FALSE );
        } else {
            ObjEmitReloc( hdl, SymName( reloc.target.sym ), reloc.type, TRUE, TRUE );
        }
        #else
        if( reloc.is_unnamed ) {
            ObjEmitReloc( &reloc.target.label_num, reloc.type, TRUE, FALSE );
        } else {
            ObjEmitReloc( SymName( reloc.target.sym ), reloc.type, TRUE, TRUE );
        }
        #endif
#ifdef MYDEBUG
        switch( reloc.type ) {
        case OWL_RELOC_WORD:
            printf( "word" ); break;
        case OWL_RELOC_HALF_LO:
            printf( "l^" ); break;
        case OWL_RELOC_HALF_HI:
            printf( "h^" ); break;
        case OWL_RELOC_HALF_HA:
            printf( "ha^" ); break;
        case OWL_RELOC_BRANCH_REL:
            printf( "br^" ); break;
        case OWL_RELOC_BRANCH_ABS:
            printf( "ba^" ); break;
        case OWL_RELOC_JUMP_REL:
            printf( "jr^" ); break;
        case OWL_RELOC_JUMP_ABS:
            printf( "ja^" ); break;
        default:
            printf( "absolute (shouldn't use)" ); break;
        }
        printf( " reloc emitted for the next instruction.\n" );
#endif
    }
    #ifdef _STANDALONE_
    ObjEmitData( hdl, (char *)&result, sizeof( result ), TRUE );
    CurrAlignment = oldAlignment;
    #else
    ObjEmitData( (char *)&result, sizeof( result ), TRUE );
    #endif

#ifdef AS_DEBUG_DUMP
    #ifdef _STANDALONE_
    if( _IsOption( DUMP_INSTRUCTIONS ) ) {
        printf( " [%#010x]\n", result );
    }
    #endif
#endif
}
