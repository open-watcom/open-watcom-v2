/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  MIPS instruction formats and encodings.
*
****************************************************************************/


#include "as.h"
#ifndef _STANDALONE_
#include "asinline.h"
#endif


/*
 * TODO: kill off all these once the axp residue is gone
 */

#define _FP_Op_Func( x )        ( _ElevenBits( x )    << 5  )

/*
 * This is real MIPS stuff
 */
#define _Code( x )              ( _TwentyBits( x )    << 6  )
#define _TrapCode( x )          ( _TenBits( x )       << 6  )

#define _Longword_offset( x )   ( (x) >> 2 )

#define MAX_EXTENDED_INS        20  // Including the default buffer, we have
                                    // 21 dwords for an ins that emits multiple
                                    // instructions. (eg. ldb)

#define OPCODE_NOP              0x00
#define OPCODE_ADDIU            0x09
#define OPCODE_ADDU             0x00
#define FNCCODE_ADDU            0x21
#define OPCODE_ORI              0x0d
#define OPCODE_LUI              0x0f
#define OPCODE_SLT              0x00
#define FNCCODE_SLT             0x2a
#define FNCCODE_OR              0x25
#define FNCCODE_JR              0x08
#define FNCCODE_JALR            0x09

/*
 * TODO: kill off these macros
 */
#define OPCODE_LDA              0x8
#define OPCODE_LDAH             0x9

#define OP_HAS_RELOC( op )      (((op)->flags & (RELOC | UNNAMED_RELOC)) != 0)
#define OP_RELOC_NAMED( op )    ((op)->flags & RELOC)

typedef struct reloc_entry      *reloc_list;

typedef op_type                 ot_array[MAX_OPERANDS];

struct reloc_entry {
    reloc_list          next;
    op_reloc_target     target;
    owl_reloc_type      type;
    unsigned            loc;            // offset of the reloc from &result[0].
    bool                is_named;       // does it have a corr. named symbol
};

typedef struct asm_reloc {
    reloc_list          first;
    reloc_list          last;
} asm_reloc;

typedef void (*fmt_func)( ins_table *, instruction *, uint_32 *, asm_reloc * );

typedef struct {
    fmt_func    func;
    op_type     ops[MAX_OPERANDS];
} mips_format;

typedef enum {
    DOMOV_ORIGINAL,
    DOMOV_ABS,
} domov_option;

static unsigned numExtendedIns = 0;  // >= 1 when we use extendedIns
static uint_32 result[MAX_EXTENDED_INS + 1];

static owl_reloc_type reloc_translate[] = {
    OWL_RELOC_ABSOLUTE,     // Corresponds to ASM_RELOC_UNSPECIFIED
    OWL_RELOC_WORD,
    OWL_RELOC_HALF_HI,
    OWL_RELOC_HALF_HA,
    OWL_RELOC_HALF_LO,
    OWL_RELOC_BRANCH_REL,   // j^ reloc
    OWL_RELOC_JUMP_REL,     // jump hint
};

static ins_opcode   cop_codes[4] = {
    0x10,   // COP0
    0x11,   // COP1
    0x12,   // COP2
    0x13    // COP3
};


static bool checkOpAbsolute( ins_operand *op, int op_idx )
//********************************************************
{
    if( OP_HAS_RELOC( op ) ) {
        Error( RELOC_NOT_ALLOWED, op_idx );
        return( false );
    }
    return( true );
}


static void addReloc( asm_reloc *reloc, op_reloc_target target, owl_reloc_type type, unsigned loc, bool is_named )
//****************************************************************************************************************
{
    reloc_list  new_entry;

    new_entry = MemAlloc( sizeof( struct reloc_entry ) );
    new_entry->next = NULL;
    new_entry->target = target;
    new_entry->type = type;
    new_entry->loc = loc;
    new_entry->is_named = is_named;
    if( reloc->first == NULL ) {
        reloc->first = new_entry;
    } else {
        reloc->last->next = new_entry;
    }
    reloc->last = new_entry;
}


static owl_reloc_type relocType( asm_reloc_type type, owl_reloc_type default_type )
//*********************************************************************************
{
    owl_reloc_type  ret;

    if( type == ASM_RELOC_UNSPECIFIED ) {
        return( default_type );
    }
    switch( default_type ) {
    case OWL_RELOC_HALF_HI:
    case OWL_RELOC_HALF_LO:
        if( (ret = reloc_translate[type]) != OWL_RELOC_HALF_HI &&
            ret != OWL_RELOC_HALF_LO ) {
            Error( INVALID_RELOC_MODIFIER );
        }
        break;
    case OWL_RELOC_BRANCH_REL:  // j^ reloc
        if( (ret = reloc_translate[type]) != default_type ) {
            Error( INVALID_RELOC_MODIFIER );
        }
        break;
    case OWL_RELOC_JUMP_REL:    // jump hint
        /*
         * we accept j^ to be specified for jump hint for now.
         */
        if( (ret = reloc_translate[type]) != OWL_RELOC_JUMP_REL &&
            ret != OWL_RELOC_BRANCH_REL ) {
            Error( INVALID_RELOC_MODIFIER );
        }
        ret = OWL_RELOC_JUMP_REL;
        break;
    default:
//        Error( "internal - unexpected default type" );
        assert( false );
        ret = OWL_RELOC_ABSOLUTE;
    }
    return( ret );
}


static void doReloc( asm_reloc *reloc, ins_operand *op, owl_reloc_type rtype, uint_32 *offset )
//*********************************************************************************************
{
    if( op == NULL )
        return;
    if( !OP_HAS_RELOC( op ) )
        return;
    addReloc( reloc, op->reloc.target, relocType( op->reloc.type, rtype ),
              (unsigned)( (char *)offset - (char *)result ), ( op->flags & RELOC ) );
}

/*
    IT_OPERATE:
        QF_V..........+0x40
    IT_FP_xxxx:
        QF_S..........+0x400
        QF_U..........+0x100
        QF_V..........+0x100
        QF_I..........+0x200
        QF_C..........-0x080
        QF_M..........-0x040
        QF_D..........+0x040
*/
static ins_funccode getFuncCode( ins_table *table, instruction *ins )
//*******************************************************************
{
    ins_funccode fc;

    fc = table->funccode;
    switch( table->template ) {
    case IT_OPERATE:
    case IT_OPERATE_IMM:
    case IT_PSEUDO_NOT:
        if( ins->format->flags & QF_V ) {
            fc += 0x40;
        } else {
            assert( ins->format->flags == 0 ); // No other valid flags
        }
        break;
    case IT_FP_OPERATE:
    case IT_FP_CONVERT:
    case IT_PSEUDO_NEGF:
        if( ins->format->flags & QF_S ) fc += 0x400;
        if( ins->format->flags & QF_U ) fc += 0x100;
        if( ins->format->flags & QF_V ) fc += 0x100;
        if( ins->format->flags & QF_I ) fc += 0x200;
        if( ins->format->flags & QF_C ) fc -= 0x080;
        if( ins->format->flags & QF_M ) fc -= 0x040;
        if( ins->format->flags & QF_D ) fc += 0x040;
        break;
    default:
        assert( false ); // Others should have no need to call this.
    }
    return( fc );
}


static void doOpcodeJType( uint_32 *buffer, ins_opcode opcode )
//*************************************************************
{
    *buffer = _Opcode( opcode );
}


static void doOpcodeIType( uint_32 *buffer, ins_opcode opcode, reg_idx rt, reg_idx rs, op_const imm )
//***************************************************************************************************
{
    *buffer = _Opcode( opcode ) | _Rs( rs ) | _Rt( rt ) | _SignedImmed( imm );
}


static void doOpcodeITypeSa( uint_32 *buffer, ins_opcode opcode, reg_idx rd, reg_idx rt, op_const sa )
//****************************************************************************************************
{
    *buffer = _Opcode( opcode ) | _Rd( rd ) | _Rt( rt ) | _Shift( sa );
}


static void doOpcodeRType( uint_32 *buffer, ins_opcode opcode, ins_funccode fc, reg_idx rd, reg_idx rs, reg_idx rt )
//******************************************************************************************************************
{
    *buffer = _Opcode( opcode ) | _Rs( rs ) | _Rt( rt ) | _Rd( rd ) | _Function( fc );
}


static void doOpcodeCopOp( uint_32 *buffer, ins_opcode opcode, ins_funccode fc, uint_32 extra )
/**********************************************************************************************
 * This procedure doesn't fill in all the bits (missing bits 6-24).
 * But we can fill it in using extra.
 */
{

    *buffer = _Opcode( opcode ) | (1 << 25) | extra | _Function( fc );
}

#if 0
static void doOpcodeIShift( uint_32 *buffer, uint_8 fc, reg_idx rd, reg_idx rt, op_const sa )
//*******************************************************************************************
{
    *buffer = _Opcode( 0 ) | _Rs( 0 ) | _Rt( rt ) | _Rd( rd ) | _Shift( sa ) | _Function( fc );
}


static void doOpcodeFloatRType( type_class_def type, uint_8 fnc, reg_idx fd, reg_idx fs, reg_idx ft )
//***************************************************************************************************
{
    mips_ins            ins;
    int                 fmt;

    /*
     * Select operand format
     */
    if( type == FS ) {
        fmt = 0x10;
    } else if( type == FD || type == FL ) {
        fmt = 0x11;
    } else {
        assert( 0 );
    }
    /*
     * Opcode is always COP1
     */
    ins = _Opcode( 0x11 ) | _FPFormat( fmt ) | _Ft( ft ) | _Fs( fs ) | _Fd( fd ) | _Function( fnc );
    EmitIns( ins );
}
#endif


static void doFPInst( uint_32 *buffer, ins_opcode opcode, reg_idx rs, reg_idx rt, reg_idx rd, uint_32 remain )
//************************************************************************************************************
{
    *buffer = _Opcode( opcode ) | _Rs( rs ) | _Rt( rt ) |
              _FP_Op_Func( remain ) | _Rd( rd );
}


#ifndef _STANDALONE_
static void doAutoVar( asm_reloc *reloc, op_reloc_target targ, uint_32 *buffer, ins_table *table, instruction *ins )
//******************************************************************************************************************
{
    ins_operand *op;

    op = ins->operands[1];
    if( op->reg != ZERO_REG ) {
        Error( BAD_BASE_REG_FOR_STACKVAR );
        return;
    }
    addReloc( reloc, targ, OWL_RELOC_FP_OFFSET, (unsigned)( (char *)buffer - (char *)result ), true );
    doOpcodeIType( buffer, table->opcode, RegIndex( ins->operands[0]->reg ), FP_REG_IDX, 0 );
}
#endif


static unsigned loadConst32( uint_32 *buffer, reg_idx reg_dst, reg_idx reg_src, ins_operand *op, op_const val, asm_reloc *reloc, bool force_pair )
/*************************************************************************************************************************************************
 * load sequence for 32-bit constants
 * Given: la $reg_dst, foobar+c($reg_src)
 *        info for foobar is stored in op
 *        c should be passed in also
 * Returns # of ins generated
 */
{
    ins_opcode  opcode;
    int         reloc_type;

    /* unused parameters */ (void)reg_src;

    if( force_pair ) {
        assert( reloc != NULL );
        if( op->reloc.type != ASM_RELOC_UNSPECIFIED ) {
            Error( INVALID_RELOC_MODIFIER );
            return( 0 );
        }
    } else {
        reloc_type = 0;
        opcode = 0;
        if( ( val < 32768 )
          && ( val > -32769 ) ) {
            /*
             * Only need sign extended low 16 bits - 'addiu rt,$zero,value'
             */
            opcode = OPCODE_ADDIU;
            reloc_type = OWL_RELOC_HALF_LO;
        } else if( (val & 0xffff) == 0 ) {
            /*
             * Only need high 16 bits - 'lui rt,$zero,(value >> 16)'
             */
            val >>= 16;
            opcode = OPCODE_LUI;
            reloc_type = OWL_RELOC_HALF_HI;
        } else if( (val & 0xffff0000) == 0 ) {
            /*
             * Only need low 16 bits - 'ori rt,$zero,value'
             */
            opcode = OPCODE_ORI;
            reloc_type = OWL_RELOC_HALF_LO;
        }
        if( reloc_type != 0 ) {
            doOpcodeIType( buffer, opcode, reg_dst, ZERO_REG_IDX, val );
            if( reloc != NULL )
                doReloc( reloc, op, reloc_type, buffer );
            return( 1 );
        }
    }
    /*
     * Need two instructions: 'lui rt,$zero,(value >> 16)'  (high word)
     */
    doOpcodeIType( buffer, OPCODE_LUI, reg_dst, ZERO_REG_IDX, val >> 16 );
    if( reloc != NULL )
        doReloc( reloc, op, OWL_RELOC_HALF_HI, buffer );
    ++buffer;
    /*
     * followed by 'ori rt,rt,(value & 0xffff)'             (low word)
     * or 'addiu' for the 'la' pseudo-ins
     */
    opcode = ( force_pair ) ? OPCODE_ADDIU : OPCODE_ORI;
    doOpcodeIType( buffer, opcode, reg_dst, reg_dst, val );
    if( reloc != NULL )
        doReloc( reloc, op, OWL_RELOC_HALF_LO, buffer );
    return( 2 );
}


static unsigned load32BitLiteral( uint_32 *buffer, ins_operand *op0, ins_operand *op1, domov_option m_opt )
//*********************************************************************************************************
{
    op_const        val;

    if( m_opt == DOMOV_ABS ) {
        val = abs( op0->constant );
    } else {
        val = op0->constant;
    }
    return( loadConst32( buffer, RegIndex( op1->reg ), ZERO_REG_IDX, NULL, val, NULL, false ) );
}


static void doMov( uint_32 *buffer, ins_operand *operands[], domov_option m_opt )
//*******************************************************************************
{
    ins_operand     *op0;
    ins_operand     *op1;

    op0 = operands[0];
    op1 = operands[1];
    if( op0->type == OP_GPR ) {
        doOpcodeRType( buffer, OPCODE_ADDU, FNCCODE_ADDU, RegIndex( op1->reg ), ZERO_REG_IDX, RegIndex( op0->reg ) );
    } else {
        if( checkOpAbsolute( op0, 0 ) ) {
            numExtendedIns += load32BitLiteral( buffer, op0, op1, m_opt ) - 1;
        }
    }
}


static void doLoadImm( uint_32 *buffer, ins_operand *operands[] )
/****************************************************************
 * 'li' pseudo-ins
 */
{
    ins_operand     *op0, *op1;
    int_32          value;
    reg_idx         reg;

    op0 = operands[0];
    op1 = operands[1];
    checkOpAbsolute( op1, 1 );
    reg = RegIndex( op0->reg );
    value = op1->constant;
    numExtendedIns += loadConst32( buffer, reg, ZERO_REG_IDX, NULL, value, NULL, false ) - 1;
}


static void ITSysCode( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//********************************************************************************************
{
    ins_operand     *op;
    op_const        constant;

    /* unused parameters */ (void)reloc;

    assert( ins->num_operands < 2 );
    if( ins->num_operands > 0 ) {
        op = ins->operands[0];
        checkOpAbsolute( op, 0 );
        constant = op->constant;
    } else {
        constant = 0;
    }
    *buffer = _Opcode( table->opcode ) | _Code( constant ) | _Function( table->funccode );
}


static void ITTrap( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//*****************************************************************************************
{
    ins_operand     *op;
    op_const        code;

    /* unused parameters */ (void)reloc;

    if( ins->num_operands > 2 ) {
        op = ins->operands[2];
        checkOpAbsolute( op, 2 );
        code = op->constant;
    } else {
        code = 0;
    }
    *buffer = _Opcode( table->opcode ) |
                _Rs( RegIndex( ins->operands[0]->reg ) ) |
                _Rt( RegIndex( ins->operands[1]->reg ) ) |
                _TrapCode( code ) | _Function( table->funccode );
}


static void ITLoadUImm( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//*********************************************************************************************
{
    ins_operand     *op;

    /* unused parameters */ (void)reloc;

    assert( ins->num_operands == 2 );
    op = ins->operands[1];
    checkOpAbsolute( op, 1 );
    doOpcodeIType( buffer, table->opcode, RegIndex( ins->operands[0]->reg ), 0, op->constant );
}


static void ITTrapImm( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//********************************************************************************************
{
    ins_operand     *op;

    /* unused parameters */ (void)reloc;

    assert( ins->num_operands == 2 );
    op = ins->operands[1];
    checkOpAbsolute( op, 1 );
    doOpcodeIType( buffer, table->opcode, (reg_idx)table->funccode, RegIndex( ins->operands[0]->reg ), op->constant );
}


static void ITMemAll( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//*******************************************************************************************
{
    ins_operand     *op;
    owl_reloc_type  type;
    ins_opcode      opcode;

    assert( ins->num_operands == 2 );
    op = ins->operands[1];
    /*
     * If op is IMMED foo, it's actually REG_INDIRECT that we want: foo($zero)
     */
    if( op->type == OP_IMMED ) {
        op->type = OP_REG_INDIRECT;
        op->reg = ZERO_REG;
    }
    assert( op->type == OP_REG_INDIRECT );
#ifndef _STANDALONE_
    if( OP_RELOC_NAMED( op ) ) {
        char    *name;

        name = SymName( op->reloc.target.ptr );
        if( AsmQueryState( AsmQuerySymbol( name ) ) == SYM_STACK ) {
            doAutoVar( reloc, op->reloc.target, buffer, table, ins );
            return;
        }
    }
#endif
    opcode = table->opcode;
    doOpcodeIType( buffer, opcode, RegIndex( ins->operands[0]->reg ), RegIndex( op->reg ), op->constant );
    type = ( opcode == OPCODE_LDAH ) ? OWL_RELOC_HALF_HI : OWL_RELOC_HALF_LO;
    doReloc( reloc, op, type, buffer );
}


static void ITMemA( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//*****************************************************************************************
{
    /* unused parameters */ (void)reloc;

    assert( ins->num_operands == 1 );
    doOpcodeIType( buffer, table->opcode, ZERO_REG_IDX, RegIndex( ins->operands[0]->reg ), table->funccode );
}


static void ITMemB( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//*****************************************************************************************
{
    /* unused parameters */ (void)reloc;

    assert( ins->num_operands == 1 );
    doOpcodeIType( buffer, table->opcode, RegIndex( ins->operands[0]->reg ), ZERO_REG_IDX, table->funccode );
}


static void doMemJump( uint_32 *buffer, ins_table *table, reg_idx rs, reg_idx rt, ins_operand *addr_op, int_32 hint, asm_reloc *reloc )
//*************************************************************************************************************************************
{
    /*
     * Note that addr_op maybe NULL. If not, addr_op->constant == hint.
     */
    assert( addr_op == NULL || addr_op->constant == hint );
    assert( addr_op == NULL || addr_op->type == OP_IMMED );
    doOpcodeIType( buffer, table->opcode, rt, rs, (table->funccode << 14) | _FourteenBits( hint ) );
    doReloc( reloc, addr_op, OWL_RELOC_JUMP_REL, buffer );
}


static int doDelaySlotNOP( uint_32 *buffer )
//******************************************
{
    int     inc = 0;

    if( _DirIsSet( REORDER ) ) {
        buffer++;
        *buffer = OPCODE_NOP;
        inc = 1;
    }
    return( inc );
}


static void doAbsJump( uint_32 *buffer, ins_table *table, ins_operand *addr_op, asm_reloc *reloc )
//************************************************************************************************
{
    assert( addr_op == NULL || addr_op->type == OP_IMMED );
    doOpcodeJType( buffer, table->opcode );
    doReloc( reloc, addr_op, OWL_RELOC_JUMP_ABS, buffer );
}


static void opError( instruction *ins, op_type actual, op_type wanted, int i )
/*****************************************************************************
 * Stuff out an error message.
 */
{
    /* unused parameters */ (void)ins; (void)actual;

    if( (wanted & OP_NOTHING) != OP_NOTHING ) {
        Error( OPERAND_INCORRECT, i );
    } else {
        Error( OPERAND_UNEXPECTED, i );
    }
}


static bool opValidate( ot_array *verify, instruction *ins, int num_op, int num_var )
//***********************************************************************************
{
    int             ctr;
    int             var;
    int             lasterr = 0;
    op_type         wanted = 0;

    for( var = 0; var < num_var; var++ ) {
        for( ctr = 0; ctr < num_op; ctr++ ) {
            if( ins->operands[ctr]->type != (*verify)[ctr] ) {
                lasterr = ctr;
                wanted = (*verify)[ctr];
                break;
            }
        }
        if( ctr == num_op ) {   // passed
            return( true );
        }
        verify++;
    }
    /*
     * not passed, error
     */
    opError( ins, ins->operands[lasterr]->type, wanted, lasterr );
    return( false );
}


static bool jmpOperandsValidate( instruction *ins, int num_op, bool link )
/*************************************************************************
 * Used by j, jal
 */
{
    static ot_array verify1[] = { { OP_REG_INDIRECT, OP_NOTHING, OP_NOTHING },
                                  { OP_GPR, OP_NOTHING, OP_NOTHING },
                                  { OP_IMMED, OP_NOTHING, OP_NOTHING } };
    static ot_array verify2[] = { { OP_GPR, OP_REG_INDIRECT, OP_NOTHING },
                                  { OP_GPR, OP_GPR, OP_NOTHING },
                                  { OP_GPR, OP_IMMED, OP_NOTHING }};
    ot_array        *verify;
    int             num_var;

    if( num_op == 0 )
        return( false );
    assert( num_op <= 2 );
    if( num_op == 1 ) {
        verify = verify1;
        num_var = sizeof( verify1 ) / sizeof( verify1[0] );
    } else {
        if( !link ) // two-operand from only valid for 'jal', not 'j'
            return( false );
        verify = verify2;
        num_var = sizeof( verify2 ) / sizeof( verify2[0] );
    }
    return( opValidate( verify, ins, num_op, num_var ) );
}


static void stdMemJump( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//*********************************************************************************************
{
    ins_operand *op1, *op2;

    op1 = ins->operands[1];
    if( op1->constant != 0 ) {
        Error( IMPROPER_OPERAND, 1 );
        return;
    }
    op2 = ins->operands[2];
    doMemJump( buffer, table, RegIndex( ins->operands[0]->reg ),
               RegIndex( op1->reg ), op2, op2->constant, reloc );
}


static void ITRegJump( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//********************************************************************************************
{
    reg_idx     reg_targ;
    reg_idx     reg_retn;

    /* unused parameters */ (void)reloc;

    assert( ins->num_operands < 3 );
    if( ins->num_operands == 2 ) {
        reg_targ = RegIndex( ins->operands[1]->reg );
        reg_retn = RegIndex( ins->operands[0]->reg );
    } else {
        reg_targ = RegIndex( ins->operands[0]->reg );
        if( table->funccode & 1 ) {
            reg_retn = RA_REG_IDX;      // jalr
        } else {
            reg_retn = 0;               // jr
        }
    }
    doOpcodeRType( buffer, table->opcode, table->funccode,
                    reg_retn,
                    reg_targ,
                    0 );

    numExtendedIns += doDelaySlotNOP( buffer );
}


static void ITJump( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//*****************************************************************************************
{
    ins_operand *op0, *op1;
    int         num_op;
    bool        link;

    /*
     * 'j' and 'jal' may both be used with the jump target given as either
     * an expression or a GPR; this means the instructions will end up being
     * encoded as j/jal or jr/jalr depending on operands.
     */
    num_op = ins->num_operands;
    link   = table->opcode & 1;
    /*
     * First check if the operands are of the right types
     */
    if( !jmpOperandsValidate( ins, num_op, link ) )
        return;

    op0 = ins->operands[0];
    if( op0->type == OP_IMMED ) {   // real j/jal (to absolute address)
        doAbsJump( buffer, table, op0, reloc );
    } else if( num_op == 1 ) {
        if( link ) {    // jalr $ra,rs
            if( op0->reg == RA_REG_IDX ) {
                /*
                 * TODO: warn - non-restartable instruction
                 */
            }
            doOpcodeRType( buffer, 0, FNCCODE_JALR,
                            RA_REG_IDX,
                            RegIndex( op0->reg ),
                            0 );
        } else {        // jr rs
            doOpcodeRType( buffer, 0, FNCCODE_JR,
                            0,
                            RegIndex( op0->reg ),
                            0 );
        }
    } else {    // jalr rd,rs
        op1 = ins->operands[1];
        if( op0->reg == op1->reg ) {
            /*
             * TODO: warn - non-restartable instruction
             */
        }
        doOpcodeRType( buffer, 0, FNCCODE_JALR,
                        RegIndex( op1->reg ),
                        RegIndex( op0->reg ),
                        0 );
    }
    numExtendedIns += doDelaySlotNOP( buffer );
}


static bool retOperandsValidate( instruction *ins, int num_op )
/**************************************************************
 * Can be used by ret, jsr_coroutine
 */
{
    static ot_array verify1[] = { { OP_GPR, OP_NOTHING, OP_NOTHING },
                                  { OP_IMMED, OP_NOTHING, OP_NOTHING } };
    static ot_array verify2[] = { { OP_GPR, OP_REG_INDIRECT, OP_NOTHING },
                                  { OP_GPR, OP_IMMED, OP_NOTHING },
                                  { OP_REG_INDIRECT, OP_IMMED, OP_NOTHING }};
    static ot_array verify3[] = { { OP_GPR, OP_REG_INDIRECT, OP_IMMED } };
    ot_array        *verify;
    int             num_var;

    if( num_op == 0 )
        return( true );
    assert( num_op <= 3 );
    if( num_op == 1 ) {
        verify = verify1;
        num_var = sizeof( verify1 ) / sizeof( verify1[0] );
    } else if( num_op == 2 ) {
        verify = verify2;
        num_var = sizeof( verify2 ) / sizeof( verify2[0] );
    } else {
        verify = verify3;
        num_var = sizeof( verify3 ) / sizeof( verify3[0] );
    }
    return( opValidate( verify, ins, num_op, num_var ) );
}


static void ITRet( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
/*****************************************************************************************
 * Both ret and jsr coroutine use this
 */
{
    ins_operand     *op0, *op1;
    int             num_op;
    reg_idx         reg_def;  // default reg_def if not specified

    num_op = ins->num_operands;
    /*
     * First check if the operands are of the right types
     */
    if( !retOperandsValidate( ins, num_op ) )
        return;
    if( num_op == 3 ) {
        stdMemJump( table, ins, buffer, reloc );
        return;
    }
    if( table->funccode == 0x0003 ) { // jsr_coroutine
        /*
         * This is according to the MS asaxp documentation.
         */
        reg_def = RA_REG_IDX;
    } else {
        assert( table->funccode == 0x0002 ); // ret
        reg_def = ZERO_REG_IDX; // $zero
    }
    if( num_op == 2 ) {
        if( (op0 = ins->operands[0])->type == OP_GPR ) {
            if( (op1 = ins->operands[1])->type == OP_REG_INDIRECT ) {
                doMemJump( buffer, table, RegIndex( op0->reg ),
                           RegIndex( op1->reg ), NULL, 1, reloc );
                return;
            }
            assert( op1->type == OP_IMMED );
            doMemJump( buffer, table, RegIndex( op0->reg ), RA_REG_IDX,
                       op1, op1->constant, reloc );
            return;
        }
        assert( op0->type == OP_REG_INDIRECT );
        op1 = ins->operands[1];
        assert( op1->type == OP_IMMED );
        doMemJump( buffer, table, reg_def, RegIndex( op0->reg ),
                   op1, op1->constant, reloc );
        return;
    }
    if( num_op == 1 ) {
        if( (op0 = ins->operands[0])->type == OP_GPR ) {
            doMemJump( buffer, table, RegIndex( op0->reg ), RA_REG_IDX,
                       NULL, 1, reloc );
            return;
        }
        assert( op0->type == OP_IMMED );
        doMemJump( buffer, table, reg_def, RA_REG_IDX,
                   op0, op0->constant, reloc );
        return;
    }
    assert( num_op == 0 );
    doMemJump( buffer, table, reg_def, RA_REG_IDX, NULL, 1, reloc );
}


static void ITMemNone( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//********************************************************************************************
{
    /* unused parameters */ (void)reloc; (void)ins;

    assert( ins->num_operands == 0 );
    doOpcodeIType( buffer, table->opcode, ZERO_REG_IDX, ZERO_REG_IDX, table->funccode );
}


static void ITBranch( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//*******************************************************************************************
{
    ins_operand *op;

    assert( ins->num_operands == 2 );
    op = ins->operands[1];
    doOpcodeIType( buffer, table->opcode, 0, RegIndex( ins->operands[0]->reg ),
                  _Longword_offset( op->constant ) );
    doReloc( reloc, op, OWL_RELOC_BRANCH_REL, buffer );
    numExtendedIns += doDelaySlotNOP( buffer );
}


static void ITBranchTwo( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//**********************************************************************************************
{
    ins_operand     *op;

    assert( ins->num_operands == 3 );
    op = ins->operands[2];
    doOpcodeIType( buffer, table->opcode, RegIndex( ins->operands[1]->reg ),
                   RegIndex( ins->operands[0]->reg ),
                   _Longword_offset( op->constant ) );
    doReloc( reloc, op, OWL_RELOC_BRANCH_REL, buffer );
    numExtendedIns += doDelaySlotNOP( buffer );
}


static void ITBranchZero( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//***********************************************************************************************
{
    ins_operand     *op;

    assert( ins->num_operands == 2 );
    op = ins->operands[1];
    doOpcodeIType( buffer, table->opcode, (reg_idx)table->funccode,
                   RegIndex( ins->operands[0]->reg ),
                   _Longword_offset( op->constant ) );
    doReloc( reloc, op, OWL_RELOC_BRANCH_REL, buffer );
    numExtendedIns += doDelaySlotNOP( buffer );
}


static void ITBranchCop( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//**********************************************************************************************
{
    ins_operand     *op;
    ins_opcode      opcode;

    assert( ins->num_operands == 1 );
    opcode = cop_codes[table->opcode >> 8];
    op = ins->operands[0];
    doOpcodeIType( buffer, opcode, (reg_idx)table->funccode, table->opcode & 0xff,
                   _Longword_offset( op->constant ) );
    doReloc( reloc, op, OWL_RELOC_BRANCH_REL, buffer );
    numExtendedIns += doDelaySlotNOP( buffer );
}


static void ITCop0Spc( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//********************************************************************************************
{
    ins_opcode      opcode;

    /* unused parameters */ (void)reloc; (void)ins;

    assert( ins->num_operands == 0 );
    opcode = cop_codes[0];
    doOpcodeCopOp( buffer, opcode, table->opcode, 0 );
}


static void ITOperate( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//********************************************************************************************
{
    /* unused parameters */ (void)reloc;

    assert( ins->num_operands == 3 );
    doOpcodeRType( buffer, table->opcode, table->funccode,
                    RegIndex( ins->operands[0]->reg ),
                    RegIndex( ins->operands[1]->reg ),
                    RegIndex( ins->operands[2]->reg ) );
}


static void ITMulDiv( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//*******************************************************************************************
{
    /* unused parameters */ (void)reloc;

    assert( ins->num_operands == 2 );
    doOpcodeRType( buffer, table->opcode, table->funccode,
                    0,
                    RegIndex( ins->operands[0]->reg ),
                    RegIndex( ins->operands[1]->reg ) );
}


static void ITMovFromSpc( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//***********************************************************************************************
{
    /* unused parameters */ (void)reloc;

    assert( ins->num_operands == 1 );
    doOpcodeRType( buffer, table->opcode, table->funccode,
                    RegIndex( ins->operands[0]->reg ),
                    0,
                    0 );
}


static void ITMovToSpc( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//*********************************************************************************************
{
    /* unused parameters */ (void)reloc;

    assert( ins->num_operands == 1 );
    doOpcodeRType( buffer, table->opcode, table->funccode,
                    0,
                    RegIndex( ins->operands[0]->reg ),
                    0 );
}


static void ITMovCop( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//*******************************************************************************************
{
    /* unused parameters */ (void)reloc;

    assert( ins->num_operands == 2 );
    doOpcodeRType( buffer, table->opcode, 0,
                    RegIndex( ins->operands[1]->reg ),
                    (reg_idx)table->funccode,
                    RegIndex( ins->operands[0]->reg ) );
}


static void ITOperateImm( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//***********************************************************************************************
{
    ins_operand     *op;

    /* unused parameters */ (void)reloc;

    assert( ins->num_operands == 3 );
    op = ins->operands[2];
    checkOpAbsolute( op, 2 );
    doOpcodeIType( buffer, table->opcode,
                    RegIndex( ins->operands[0]->reg ), RegIndex( ins->operands[1]->reg ),
                    op->constant );
}


static void ITShiftImm( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//*********************************************************************************************
{
    ins_operand     *op;

    /* unused parameters */ (void)reloc;

    assert( ins->num_operands == 3 );
    op = ins->operands[2];
    doOpcodeITypeSa( buffer, table->opcode,
                        RegIndex( ins->operands[0]->reg ),
                        RegIndex( ins->operands[1]->reg ), op->constant );
}


static void ITFPOperate( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//**********************************************************************************************
{
    ins_funccode    fc;

    /* unused parameters */ (void)reloc;

    assert( ins->num_operands == 3 );
    fc = getFuncCode( table, ins );
    doFPInst( buffer, table->opcode, RegIndex( ins->operands[0]->reg ),
                RegIndex( ins->operands[1]->reg ),
                RegIndex( ins->operands[2]->reg ), fc );
}


static void ITFPConvert( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//**********************************************************************************************
{
    ins_funccode    fc;

    /* unused parameters */ (void)reloc;

    assert( ins->num_operands == 2 );
    fc = getFuncCode( table, ins );
    doFPInst( buffer, table->opcode, ZERO_REG_IDX, RegIndex( ins->operands[0]->reg ),
                RegIndex( ins->operands[1]->reg ), fc );
}


static void ITBr( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//***************************************************************************************
{
    ins_operand *op0, *op1;
    assert( ins->num_operands == 1 || ins->num_operands == 2 );
    op0 = ins->operands[0];
    if( ins->num_operands == 1 ) {
        if( op0->type != OP_IMMED ) {
            Error( IMPROPER_OPERAND, 0 );
            return;
        }
        doOpcodeIType( buffer, table->opcode, 0, ZERO_REG_IDX,
                      _Longword_offset( op0->constant ) );
        doReloc( reloc, op0, OWL_RELOC_BRANCH_REL, buffer );
        return;
    }
    /*
     * num_operands == 2
     */
    op1 = ins->operands[1];
    if( op0->type != OP_GPR ) {
        Error( IMPROPER_OPERAND, 0 );
        return;
    }
    if( op1->type != OP_IMMED ) {
        Error( IMPROPER_OPERAND, 1 );
        return;
    }
    ITBranch( table, ins, buffer, reloc );
}


static void ITMTMFFpcr( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//*********************************************************************************************
{
    assert( ins->num_operands == 1 || ins->num_operands == 3 );
    if( ins->num_operands == 1 ) {
        reg_idx     reg_num;

        reg_num = RegIndex( ins->operands[0]->reg );
        doFPInst( buffer, table->opcode, reg_num, reg_num, reg_num, table->funccode );
        return;
    }
    ITFPOperate( table, ins, buffer, reloc );
}


static void ITPseudoClr( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//**********************************************************************************************
{
    /* unused parameters */ (void)reloc;

    assert( ins->num_operands == 1 );
    doOpcodeRType( buffer, table->opcode, table->funccode, RegIndex( ins->operands[0]->reg ), ZERO_REG_IDX, ZERO_REG_IDX );
}


static void ITPseudoFclr( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//***********************************************************************************************
{
    /* unused parameters */ (void)reloc;

    assert( ins->num_operands == 1 );
    doFPInst( buffer, table->opcode, ZERO_REG_IDX, ZERO_REG_IDX,
                RegIndex( ins->operands[0]->reg ), table->funccode );
}


static void ITPseudoLImm( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//***********************************************************************************************
{
    /* unused parameters */ (void)reloc; (void)table;

    assert( ins->num_operands == 2 );
    doLoadImm( buffer, ins->operands );
}


static void ITPseudoLAddr( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//************************************************************************************************
{
    ins_operand         *op;
    ins_operand         *ops[2];
    unsigned            inc;
//    op_const            val;
    reg_idx             reg_src;

    /* unused parameters */ (void)table;

    assert( ins->num_operands == 2 );
    op = ins->operands[1];
    /*
     * If op is IMMED foo, it's actually REG_INDIRECT that we want: foo($zero)
     */
    if( op->type == OP_IMMED ) {
        op->type = OP_REG_INDIRECT;
        op->reg = ZERO_REG;
    }
    assert( op->type == OP_REG_INDIRECT );
//    val = op->constant;
    reg_src = RegIndex( op->reg );
    if( !OP_HAS_RELOC( op ) && reg_src == ZERO_REG_IDX ) {
        /*
         * doMov() can only be called when op->reg is ZERO_REG and no reloc
         */
        ops[0] = op;
        ops[1] = ins->operands[0];
        doMov( buffer, ops, DOMOV_ORIGINAL );
        return;
    }
    if( OP_HAS_RELOC( op ) ) {
#ifndef _STANDALONE_
        if( OP_RELOC_NAMED( op ) ) {
            char        *name;

            name = SymName( op->reloc.target.ptr );
            if( AsmQueryState( AsmQuerySymbol( name ) ) == SYM_STACK ) {
                doAutoVar( reloc, op->reloc.target, buffer, table, ins );
                return;
            }
        }
#endif
        if( op->reloc.type == ASM_RELOC_UNSPECIFIED ) {
            /*
             * We should emit lui/addiu pair.
             */
            inc = loadConst32( buffer, RegIndex( ins->operands[0]->reg ),
                               reg_src, op, op->constant, reloc, true );
            numExtendedIns += inc - 1;
            return;
        }
    }
    inc = loadConst32( buffer, RegIndex( ins->operands[0]->reg ), reg_src, op,
                       op->constant, reloc, false );
    numExtendedIns += inc - 1;
}


static void ITPseudoMov( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//**********************************************************************************************
{
    /* unused parameters */ (void)reloc; (void)table;

    assert( ins->num_operands == 2 );
    doOpcodeRType( buffer, 0, FNCCODE_OR,
                    RegIndex( ins->operands[0]->reg ),
                    RegIndex( ins->operands[1]->reg ),
                    ZERO_REG_IDX );
}


static void ITPseudoFmov( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//***********************************************************************************************
{
    reg_idx         reg_idx0;

    /* unused parameters */ (void)reloc;

    assert( ins->num_operands == 2 );
    reg_idx0 = RegIndex( ins->operands[0]->reg );
    doFPInst( buffer, table->opcode, reg_idx0, reg_idx0,
                RegIndex( ins->operands[1]->reg ), table->funccode );
}


static void ITPseudoNot( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//**********************************************************************************************
{
    ins_funccode    fc;
    ins_operand     *op;

    /* unused parameters */ (void)reloc;

    assert( ins->num_operands == 2 );
    fc = getFuncCode( table, ins );
    op = ins->operands[0];
    doOpcodeRType( buffer, table->opcode, fc, RegIndex( ins->operands[1]->reg ), ZERO_REG_IDX, RegIndex( op->reg ) );
}


static void ITPseudoNegf( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//***********************************************************************************************
{
    ins_funccode    fc;

    /* unused parameters */ (void)reloc;

    assert( ins->num_operands == 2 );
    fc = getFuncCode( table, ins );
    doFPInst( buffer, table->opcode, ZERO_REG_IDX, RegIndex( ins->operands[0]->reg ),
                RegIndex( ins->operands[1]->reg ), fc );
}


static void ITPseudoFneg( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//***********************************************************************************************
{
    reg_idx         reg_idx0;

    /* unused parameters */ (void)reloc;

    assert( ins->num_operands == 2 );
    reg_idx0 = RegIndex( ins->operands[0]->reg );
    doFPInst( buffer, table->opcode, reg_idx0, reg_idx0,
                RegIndex( ins->operands[1]->reg ), table->funccode );
}


static void ITPseudoAbs( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//**********************************************************************************************
{
    reg_idx         reg_src;
    reg_idx         reg_dst;
    reg_idx         reg_rc;
    ins_operand     *src_op;
    bool            same_reg;

    /* unused parameters */ (void)reloc;

    assert( ins->num_operands == 2 );
    src_op = ins->operands[0];
    if( src_op->type == OP_IMMED ) {
        /*
         * Then just evaluate it and do a mov instead.
         * mov  abs(src_op->constant), reg_dst
         */
        if( checkOpAbsolute( src_op, 0 ) ) {
            doMov( buffer, ins->operands, DOMOV_ABS );
        }
        return;
    }
    assert( src_op->type == OP_GPR );
    /* Emit an instruction sequence like:
     * 1. subq/v  $zero, $reg_src, $at    // if( $reg_src == $reg_dst )
     * 2. cmovlt  $reg_src, $at, $reg_src
     * ---or---
     * 1. subq/v  $zero, $reg_src, $reg_dst // if( $reg_src != $reg_dst )
     * 2. cmovgt  $reg_src, $reg_src, $reg_dst
     */
    reg_src = RegIndex( src_op->reg );
    reg_dst = RegIndex( ins->operands[1]->reg );
    same_reg = ( reg_src == reg_dst );
    if( same_reg ) {
        /*
         * Then $at reg will be needed.
         */
        if( !_DirIsSet( AT ) ) {
            Warning( INS_USES_AT_REG );
        }
        reg_rc = AT_REG_IDX;
    } else {
        reg_rc = reg_dst;
    }
    doOpcodeRType( buffer, table->opcode, table->funccode, reg_rc, ZERO_REG_IDX, reg_src );
    /*
     * So buffer gets ins #1. Now do ins #2.
     */
    ++buffer;
#define OPCODE_CMOVGT           0x11
#define OPCODE_CMOVLT           0x11
#define FUNCCODE_CMOVGT         0x0066
#define FUNCCODE_CMOVLT         0x0044
    if( same_reg ) {
        doOpcodeRType( buffer, OPCODE_CMOVLT, FUNCCODE_CMOVLT, reg_src, reg_src, AT_REG_IDX );
    } else {
        doOpcodeRType( buffer, OPCODE_CMOVGT, FUNCCODE_CMOVGT, reg_dst, reg_src, reg_src );
    }
    ++numExtendedIns;
}


static void ITPseudoBranch2( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//**************************************************************************************************
{
    ins_operand     *op;
    int             i1;
    int             i2;

    assert( ins->num_operands == 3 );
    op = ins->operands[2];
    i1 = 0;
    i2 = 1;
    if( table->funccode ) {
        i1 = 1;
        i2 = 0;
    }
    doOpcodeRType( buffer, OPCODE_SLT, FNCCODE_SLT, AT_REG_IDX, RegIndex( ins->operands[i1]->reg ), RegIndex( ins->operands[i2]->reg ) );
    ++buffer;
    numExtendedIns++;
    doOpcodeIType( buffer, table->opcode, ZERO_REG_IDX, AT_REG_IDX,
                   _Longword_offset( op->constant ) );
    doReloc( reloc, op, OWL_RELOC_BRANCH_REL, buffer );
    numExtendedIns += doDelaySlotNOP( buffer );
}


mips_format MIPSFormatTable[] = {
    #define PICK( a, b, c, d, e )       { b, { c, d, e } },
    #include "_mipsfmt.h"
    #undef PICK
};


bool MIPSValidate( instruction *ins )
/************************************
 * Make sure that all operands of the given instruction
 * are of the type we are expecting. If not, we print
 * out an error message.
 */
{
    int                 i;
    mips_format         *fmt;
    ins_operand         *op;

    fmt = &MIPSFormatTable[ins->format->table_entry->template];
    for( i = 0; i < ins->num_operands; i++ ) {
        op = ins->operands[i];
        if( ( op->type & fmt->ops[i] ) != op->type ) {
            opError( ins, op->type, fmt->ops[i], i );
            return( false );
        }
    }
    if( i < MAX_OPERANDS ) {
        if( ( fmt->ops[i] & OP_NOTHING ) != OP_NOTHING ) {
            Error( NOT_ENOUGH_INSOP );
            return( false );
        }
    }   /*
         * NOTE: It might not catch all improper operand combinations
         * because we're using flags here
         */
    return( true );
}


#ifdef _STANDALONE_
static void emitIns( owl_section_handle hdl, char *inscode, size_t size )
//***********************************************************************
{
    ObjEmitData( hdl, inscode, size, true );
}
#else
static void emitIns( char *inscode, size_t size )
//***********************************************
{
    ObjEmitData( inscode, size, true );
}
#endif


#ifdef _STANDALONE_
void MIPSEmit( owl_section_handle hdl, instruction *ins )
#else
void MIPSEmit( instruction *ins )
#endif
/********************************************************
 * Encode the given instruction (including emitting any
 * relocs to the appropriate places), and emit the code
 * to the given section.
 */
{
    unsigned        ctr;
    ins_table       *table;
    asm_reloc       reloc = { NULL, NULL };
    reloc_list      curr_reloc;
#ifdef _STANDALONE_
    uint_8          old_alignment;

    if( OWLTellSectionType( hdl ) & OWL_SEC_ATTR_BSS ) {
        Error( INVALID_BSS_STATEMENT );
        return;
    }
    old_alignment = CurrAlignment;
    if( CurrAlignment < 2 ) { // Instructions should at least be dword aligned
        CurrAlignment = 2;
    }
#endif
    table = ins->format->table_entry;
    MIPSFormatTable[table->template].func( table, ins, result, &reloc );
    for( ctr = 0; ctr <= numExtendedIns; ctr++ ) {
        if( ( curr_reloc = reloc.first ) != NULL ) {
            assert( curr_reloc->loc >= ctr * sizeof( *result ) );
            if( curr_reloc->loc == ctr * sizeof( *result ) ) {
                reloc.first = curr_reloc->next;
                if( curr_reloc->is_named ) {
#ifdef _STANDALONE_
                    ObjEmitReloc( hdl, SymName( curr_reloc->target.ptr ),
                                  curr_reloc->type, true, true );
#else
                    ObjEmitReloc( SymName( curr_reloc->target.ptr ),
                                  curr_reloc->type, true, true );
#endif
                } else {
#ifdef _STANDALONE_
                    ObjEmitReloc( hdl, &curr_reloc->target.label,
                                  curr_reloc->type, true, false );
#else
                    ObjEmitReloc( &curr_reloc->target.label,
                                  curr_reloc->type, true, false );
#endif
                }
#ifdef AS_DEBUG_DUMP
                switch( curr_reloc->type ) {
                case OWL_RELOC_WORD:
                    _DBGMSG1( "word" ); break;
                case OWL_RELOC_HALF_LO:
                    _DBGMSG1( "l^" ); break;
                case OWL_RELOC_HALF_HI:
                    _DBGMSG1( "h^" ); break;
                case OWL_RELOC_BRANCH_REL:
                    _DBGMSG1( "j^" ); break;
                case OWL_RELOC_JUMP_REL:
                    _DBGMSG1( "jump hint" ); break;
                default:
                    _DBGMSG1( "absolute (shouldn't use)" ); break;
                }
                _DBGMSG1( " reloc emitted for the instruction.\n" );
#endif
                MemFree( curr_reloc );
            }
        }
#ifdef _STANDALONE_
        emitIns( hdl, (char *)&result[ctr], sizeof( uint_32 ) );
#else
        emitIns( (char *)&result[ctr], sizeof( uint_32 ) );
#endif
#if defined( _STANDALONE_ ) && defined( AS_DEBUG_DUMP )
        if( _IsOption( DUMP_INSTRUCTIONS ) ) {
            printf( " [%#010x]\n", result[ctr] );
        }
#endif
    }
    assert( reloc.first == NULL ); // Should all be emitted already!
    reloc.last = NULL;
    if( numExtendedIns != 0 ) {
        if( !_DirIsSet( MACRO ) ) {
            Warning( MACRO_INSTRUCTION );
        }
        numExtendedIns = 0;
    }
#ifdef _STANDALONE_
    CurrAlignment = old_alignment;
#endif
}
