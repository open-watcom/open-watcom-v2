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
* Description:  MIPS instruction formats and encodings.
*
****************************************************************************/


#include "as.h"
#ifndef _STANDALONE_
#include "asinline.h"
#endif

#include "mipsenc.h"

typedef struct reloc_entry      *reloc_list;

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

#define MAX_VARIETIES   3
typedef op_type ot_array[MAX_VARIETIES][3];

// TODO: kill off all these once the axp residue is gone
#define _EightBits( x )         ( (x) & 0x00ff )
#define _ElevenBits( x )        ( (x) & 0x07ff )
#define _FourteenBits( x )      ( (x) & 0x3fff )
#define _TwentyBits( x    )     ( (x) & 0x000fffff )

#define _LIT_value( x )         ( _EightBits( x )     << 1  )
#define _LIT_bit                1
#define _LIT_unshifted( x )     ( _LIT_value( x ) | _LIT_bit )

#define _Memory_disp( x )       ( _SixteenBits( x )   << 0  )
#define _Mem_Func( x )          ( _SixteenBits( x )   << 0  )
#define _Op_Func( x )           ( _SixBits( x )       << 0  )
#define _FP_Op_Func( x )        ( _ElevenBits( x )    << 5  )
#define _LIT( x )               ( _LIT_unshifted( x ) << 12 )

// This is real MIPS stuff
#define _TenBits( x )           ( (x) & 0x03ff )
#define _Code( x )              ( _TwentyBits( x )    << 6  )
#define _TrapCode( x )          ( _TenBits( x )       << 6  )

#define _Longword_offset( x )   ( (x) >> 2 )

#define MAX_EXTENDED_INS        20  // Including the default buffer, we have
                                    // 21 dwords for an ins that emits multiple
                                    // instructions. (eg. ldb)

// Check if ".set noat" is in effect before using this
#define AT_REG_IDX              RegIndex( AT_REG )

#define ZERO_REG                MakeReg( RC_GPR, MIPS_ZERO_SINK )

#define OPCODE_NOP      0x00
#define OPCODE_ADDIU    0x09
#define OPCODE_ORI      0x0d
#define OPCODE_LUI      0x0f
#define FNCCODE_OR      0x25
#define FNCCODE_JR      0x08
#define FNCCODE_JALR    0x09

// TODO: kill off these macros
#define OPCODE_BIS      0x11
#define FUNCCODE_BIS    0x0020
#define OPCODE_LDA      0x8
#define OPCODE_LDAH     0x9

#define OP_HAS_RELOC( op )      ((op)->flags & (RELOC | UNNAMED_RELOC))
#define OP_RELOC_NAMED( op )    ((op)->flags & RELOC)

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

static uint_32  cop_codes[4] = {
    0x10,   // COP0
    0x11,   // COP1
    0x12,   // COP2
    0x13    // COP3
};


static bool ensureOpAbsolute( ins_operand *op, uint_8 opIdx )
//***********************************************************
{
    if( OP_HAS_RELOC( op ) ) {
        Error( RELOC_NOT_ALLOWED, opIdx );
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
        // we accept j^ to be specified for jump hint for now.
        if( (ret = reloc_translate[type]) != OWL_RELOC_JUMP_REL &&
            ret != OWL_RELOC_BRANCH_REL ) {
            Error( INVALID_RELOC_MODIFIER );
        }
        ret = OWL_RELOC_JUMP_REL;
        break;
    default:
        //Error( "internal - unexpected default type" );
        assert( false );
        ret = OWL_RELOC_ABSOLUTE;
    }
    return( ret );
}


static void doReloc( asm_reloc *reloc, ins_operand *op, owl_reloc_type rtype, uint_32 *offset )
//*********************************************************************************************
{
    if( op == NULL ) return;
    if( !( OP_HAS_RELOC( op ) ) ) return;
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


static void doOpcodeJType( uint_32 *buffer, uint_8 opcode )
//*********************************************************
{
    *buffer = _Opcode( opcode );
}


static void doOpcodeIType( uint_32 *buffer, uint_8 opcode, uint_8 rt, uint_8 rs, signed_16 immed )
//************************************************************************************************
{
    *buffer = _Opcode( opcode ) | _Rs( rs ) | _Rt( rt ) | _Immed( immed );
}


static void doOpcodeRType( uint_32 *buffer, uint_8 opcode, uint_8 fc, uint_8 rd, uint_8 rs, uint_8 rt )
//*****************************************************************************************************
{
    *buffer = _Opcode( opcode ) | _Rs( rs ) | _Rt( rt ) | _Rd( rd ) | _Function( fc );
}


static void doOpcodeCopOp( uint_32 *buffer, uint_8 opcode, uint_8 fc, uint_32 extra )
//***********************************************************************************
// This procedure doesn't fill in all the bits (missing bits 6-24).
// But we can fill it in using extra.
{

    *buffer = _Opcode( opcode ) | (1 << 25) | extra | _Function( fc );
}

#if 0
static void doOpcodeIShift( uint_32 *buffer, uint_8 fc, uint_8 rd, uint_8 rt, uint_8 sa )
//***************************************************************************************
{
    *buffer = _Opcode( 0 ) | _Rs( 0 ) | _Rt( rt ) | _Rd( rd ) | _Shift( sa ) | _Function( fc );
}


static void doOpcodeFloatRType( type_class_def type, uint_8 fnc, uint_8 fd, uint_8 fs, uint_8 ft )
//************************************************************************************************
{
    mips_ins            ins;
    int                 fmt;

    // Select operand format
    if( type == FS ) {
        fmt = 0x10;
    } else if( type == FD || type == FL ) {
        fmt = 0x11;
    } else {
        assert( 0 );
    }

    // Opcode is always COP1
    ins = _Opcode( 0x11 ) | _FPFormat( fmt ) | _Ft( ft ) | _Fs( fs ) | _Fd( fd ) | _Function( fnc );
    EmitIns( ins );
}
#endif

static void doOpcodeRsRt( uint_32 *buffer, ins_opcode opcode, uint_8 rs, uint_8 rt, uint_32 remain )
//**************************************************************************************************
{
    *buffer = _Opcode( opcode ) | _Rs( rs ) | _Rt( rt ) | remain;
}


static void doOpcodeFcRsRt( uint_32 *buffer, ins_opcode opcode, ins_funccode fc, uint_8 ra, uint_8 rc, uint_32 extra )
//********************************************************************************************************************
// This procedure doesn't fill in all the bits (missing bits 20-12).
// But we can fill it in using extra.
{
    *buffer = _Opcode( opcode ) | _Op_Func( fc ) | _Rs( ra ) | _Rt( rc ) |
              extra;
}


static void doOpcodeFcRsRtImm( uint_32 *buffer, ins_opcode opcode, ins_funccode fc, uint_8 rs, uint_8 rt, uint_32 imm )
//*********************************************************************************************************************
{
    *buffer = _Opcode( opcode ) | _Op_Func( fc ) | _Rs( rs ) | _Rt( rt ) |
              _Immed( imm );
}


static void doOpcodeFcRdRtSa( uint_32 *buffer, ins_opcode opcode, ins_funccode fc, uint_8 rd, uint_8 rt, uint_8 sa )
//******************************************************************************************************************
{
    *buffer = _Opcode( opcode ) | _Op_Func( fc ) | _Rd( rd ) | _Rt( rt ) |
              _Shift( sa );
}


static void doFPInst( uint_32 *buffer, ins_opcode opcode, uint_8 ra, uint_8 rb, uint_8 rc, uint_32 remain )
//*********************************************************************************************************
{
    *buffer = _Opcode( opcode ) | _Rs( ra ) | _Rt( rb ) |
              _FP_Op_Func( remain ) | _Rd( rc );
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
    doOpcodeRsRt( buffer, table->opcode, MIPS_FRAME_REG, RegIndex( ins->operands[0]->reg ), 0 );
}
#endif


static unsigned loadConst32( uint_32 *buffer, uint_8 d_reg, uint_8 s_reg, ins_operand *op, op_const c, asm_reloc *reloc, bool force_pair )
//****************************************************************************************************************************************
// load sequence for 32-bit constants
// Given: la $d_reg, foobar+c($s_reg)
//        info for foobar is stored in op
//        c should be passed in also
// Returns # of ins generated
{
    unsigned            ret = 1;
    int_16              low, high;

    s_reg = s_reg;
    if( force_pair ) {
        assert( reloc != NULL );
        if( op->reloc.type != ASM_RELOC_UNSPECIFIED ) {
            Error( INVALID_RELOC_MODIFIER );
            return( ret );
        }
    }
    low  = c & 0xffff;
    high = (c & 0xffff0000) >> 16;

    if( !force_pair && (c < 32768) && ((int_32)c > -32769) ) {
        // Only need sign extended low 16 bits - 'addiu rt,$zero,value'
        doOpcodeIType( buffer, OPCODE_ADDIU, d_reg, MIPS_ZERO_SINK, low );
        doReloc( reloc, op, OWL_RELOC_HALF_LO, buffer );
    } else if( !force_pair && !high ) {
        // Only need high 16 bits - 'lui rt,$zero,(value >> 16)'
        doOpcodeIType( buffer, OPCODE_LUI, d_reg, MIPS_ZERO_SINK, high );
        doReloc( reloc, op, OWL_RELOC_HALF_HI, buffer );
    } else {
        // Need two instructions: 'lui rt,$zero,(value >> 16)'
        doOpcodeIType( buffer, OPCODE_LUI, d_reg, MIPS_ZERO_SINK, high );
        doReloc( reloc, op, OWL_RELOC_HALF_HI, buffer );
        ++buffer;
        // followed by 'ori rt,rt,(value & 0xffff)'
        // or 'addiu' for the 'la' pseudo-ins
        if( force_pair )
            doOpcodeIType( buffer, OPCODE_ADDIU, d_reg, d_reg, low );
        else
            doOpcodeIType( buffer, OPCODE_ORI, d_reg, d_reg, low );
        doReloc( reloc, op, OWL_RELOC_HALF_LO, buffer );
        ++ret;
    }
    return( ret );
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
    return( loadConst32( buffer, RegIndex( op1->reg ), MIPS_ZERO_SINK,
                         op0, val, NULL, false ) );
}


static void doMov( uint_32 *buffer, ins_operand *operands[], domov_option m_opt )
//*******************************************************************************
{
    ins_operand     *op0, *op1;
    uint_32         extra;
    uint_32         abs_val;
    bool            ready = true;

    op0 = operands[0];
    op1 = operands[1];
    if( op0->type == OP_GPR ) {
        extra = _Rt( RegIndex( op0->reg ) );
    } else if( ( op0->constant & 0xff ) == op0->constant ) { // OP_IMMED implied
        extra = _LIT( op0->constant ); // this lit is between 0..255
        (void)ensureOpAbsolute( op0, 0 );
    } else if( m_opt == DOMOV_ABS ) {
        abs_val = abs( op0->constant );
        if( ( abs_val & 0xff ) == abs_val ) {
            extra = _LIT( abs_val ); // this lit is between 0..255
            // ensureOpAbsolute( op0, 0 );  should be done before calling doMov
        } else {
            ready = false;
        }
    } else {
        ready = false;
    }
    if( ready ) {
        doOpcodeFcRsRt( buffer, OPCODE_BIS, FUNCCODE_BIS,
                        MIPS_ZERO_SINK, RegIndex( op1->reg ), extra );
        return;
    }
    // Otherwise it's OP_IMMED with a greater than 8-bit literal.
    // We'll then use multiple LDA, LDAH instructions to load the literal.
    if( !ensureOpAbsolute( op0, 0 ) ) return;
    numExtendedIns += load32BitLiteral( buffer, op0, op1, m_opt ) - 1;
}


static void doLoadImm( uint_32 *buffer, ins_operand *operands[] )
//***************************************************************
// 'li' pseudo-ins
{
    ins_operand     *op0, *op1;
    int_32          value;
    uint_32         reg;

    op0 = operands[0];
    op1 = operands[1];
    ensureOpAbsolute( op1, 1 );
    reg = RegIndex( op0->reg );
    value = op1->constant;

    if( (value < 32768) && (value > -32769) ) {
        // Only need sign extended low 16 bits - 'addiu rt,$zero,value'
        doOpcodeIType( buffer, OPCODE_ADDIU, reg, MIPS_ZERO_SINK,
            (unsigned_16)value );
    } else if( (value & 0xffff) == 0 ) {
        // Only need high 16 bits - 'lui rt,$zero,(value >> 16)'
        doOpcodeIType( buffer, OPCODE_LUI, reg, MIPS_ZERO_SINK,
            (unsigned_16)(value >> 16) );
    } else {
        // Need two instructions: 'lui rt,$zero,(value >> 16)'
        doOpcodeIType( buffer, OPCODE_LUI, reg, MIPS_ZERO_SINK,
            (unsigned_16)(value >> 16) );
        // followed by 'ori rt,$zero,(value & 0xffff)'
        ++buffer;
        doOpcodeIType( buffer, OPCODE_ORI, reg, MIPS_ZERO_SINK,
            (unsigned_16)value );
        ++numExtendedIns;
    }
}


static void ITSysCode( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//********************************************************************************************
{
    ins_operand     *op;
    op_const        constant;

    assert( ins->num_operands < 2 );
    reloc = reloc;
    if( ins->num_operands > 0 ) {
        op = ins->operands[0];
        ensureOpAbsolute( op, 0 );
        constant = op->constant;
    } else {
        constant = 0;
    }
    *buffer = _Opcode( table->opcode ) | _Code( constant ) | _Op_Func( table->funccode );
}


static void ITTrap( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//*****************************************************************************************
{
    ins_operand     *op;
    op_const        code;

    reloc = reloc;
    if( ins->num_operands > 2 ) {
        op = ins->operands[2];
        ensureOpAbsolute( op, 2 );
        code = op->constant;
    } else {
        code = 0;
    }
    *buffer = _Opcode( table->opcode ) |
              _Rs( RegIndex( ins->operands[0]->reg ) ) |
              _Rt( RegIndex( ins->operands[1]->reg ) ) |
              _TrapCode( code ) | _Op_Func( table->funccode );
}


static void ITLoadUImm( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//*********************************************************************************************
{
    ins_operand     *op;

    reloc = reloc;
    assert( ins->num_operands == 2 );
    op = ins->operands[1];
    ensureOpAbsolute( op, 1 );
    doOpcodeIType( buffer, table->opcode, RegIndex( ins->operands[0]->reg ),
                   0, op->constant );
}


static void ITTrapImm( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//********************************************************************************************
{
    ins_operand     *op;

    reloc = reloc;
    assert( ins->num_operands == 2 );
    op = ins->operands[1];
    ensureOpAbsolute( op, 1 );
    doOpcodeIType( buffer, table->opcode, table->funccode,
                   RegIndex( ins->operands[0]->reg ), op->constant );
}


static void ITMemAll( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//*******************************************************************************************
{
    ins_operand     *op;
    owl_reloc_type  type;
    ins_opcode      opcode;

    assert( ins->num_operands == 2 );
    op = ins->operands[1];
    // If op is IMMED foo, it's actually REG_INDIRECT that we want: foo($zero)
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
    doOpcodeRsRt( buffer, opcode, RegIndex( op->reg ),
            RegIndex( ins->operands[0]->reg ), _Memory_disp( op->constant ) );
    type = ( opcode == OPCODE_LDAH ) ? OWL_RELOC_HALF_HI : OWL_RELOC_HALF_LO;
    doReloc( reloc, op, type, buffer );
}


static void ITMemA( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//*****************************************************************************************
{
    assert( ins->num_operands == 1 );
    reloc = reloc;
    doOpcodeRsRt( buffer, table->opcode, RegIndex( ins->operands[0]->reg ),
                  31, _Mem_Func( table->funccode ) );
}


static void ITMemB( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//*****************************************************************************************
{
    assert( ins->num_operands == 1 );
    reloc = reloc;
    doOpcodeRsRt( buffer, table->opcode, 31, RegIndex( ins->operands[0]->reg ),
                  _Mem_Func( table->funccode ) );
}


static void doMemJump( uint_32 *buffer, ins_table *table, uint_8 ra, uint_8 rb, ins_operand *addr_op, uint_32 hint, asm_reloc *reloc )
//************************************************************************************************************************************
{
    // Note that addr_op maybe NULL. If not, addr_op->constant == hint.
    assert( addr_op == NULL || addr_op->constant == hint );
    assert( addr_op == NULL || addr_op->type == OP_IMMED );
    doOpcodeRsRt( buffer, table->opcode, ra, rb,
                  (table->funccode << 14) | _FourteenBits(hint) );
    doReloc( reloc, addr_op, OWL_RELOC_JUMP_REL, buffer );
}


static int doDelaySlotNOP( uint_32 *buffer )
//******************************************
{
    int     inc = 0;

    if( _DirIsSet( REORDER ) ) {
        buffer += sizeof( uint_32 );
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
//****************************************************************************
// Stuff out an error message.
{
    ins = ins;
    actual = actual;
    wanted = wanted;    // it's a set of flags
    if( ( wanted & OP_NOTHING ) != OP_NOTHING ) {
        Error( OPERAND_INCORRECT, i );
    } else {
        Error( OPERAND_UNEXPECTED, i );
    }
}


static bool opValidate( ot_array *verify, instruction *ins, ins_opcount num_op, unsigned num_var )
//************************************************************************************************
{
    int             ctr, var, lasterr;
    op_type         actual = 0;
    op_type         wanted = 0;

    lasterr = -1;
    for( var = 0; var < num_var; var++ ) {
        lasterr = -1;
        for( ctr = 0; ctr < num_op; ctr++ ) {
            if( ins->operands[ctr]->type != (*verify)[var][ctr] ) {
                lasterr = ctr;
                actual = ins->operands[ctr]->type;
                wanted = (*verify)[var][ctr];
                break;
            }
        }
        if( lasterr == -1 ) {   // passed
            break;
        }
    }
    if( lasterr != -1 ) {
        opError( ins, actual, wanted, lasterr );
        return( false );
    }
    return( true );
}


static bool jmpOperandsValidate( instruction *ins, ins_opcount num_op, bool link )
//********************************************************************************
// Used by j, jal
{
    static op_type  verify1[][3] = { { OP_REG_INDIRECT, OP_NOTHING, OP_NOTHING },
                                     { OP_GPR, OP_NOTHING, OP_NOTHING },
                                     { OP_IMMED, OP_NOTHING, OP_NOTHING } };
    static op_type  verify2[][3] = { { OP_GPR, OP_REG_INDIRECT, OP_NOTHING },
                                     { OP_GPR, OP_GPR, OP_NOTHING },
                                     { OP_GPR, OP_IMMED, OP_NOTHING }};
    ot_array        *verify;
    ot_array        *verify_table[2] = { &verify1, &verify2 };
    unsigned        num_var;

    if( num_op == 0 )
        return( false );
    assert( num_op <= 2 );
    verify = verify_table[num_op - 1];
    if( num_op == 1 ) {
        num_var = sizeof( verify1 ) / sizeof( **verify1 ) / 3;
    } else {
        if( !link ) // two-operand from only valid for 'jal', not 'j'
            return( false );
        num_var = sizeof( verify2 ) / sizeof( **verify2 ) / 3;
    }
    assert( num_var <= MAX_VARIETIES );
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
    int     targ_idx;
    int     retn_idx;

    reloc = reloc;
    assert( ins->num_operands < 3 );
    if( ins->num_operands == 2 ) {
        targ_idx = RegIndex( ins->operands[1]->reg );
        retn_idx = RegIndex( ins->operands[0]->reg );
    } else {
        targ_idx = RegIndex( ins->operands[0]->reg );
        if( table->funccode & 1 )
            retn_idx = MIPS_RETURN_ADDR;    // jalr
        else
            retn_idx = 0;                   // jr
    }
    doOpcodeRType( buffer, table->opcode, table->funccode,
                   retn_idx, targ_idx, 0 );

    numExtendedIns += doDelaySlotNOP( buffer );
    return;
}


static void ITJump( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//*****************************************************************************************
{
    ins_operand *op0, *op1;
    ins_opcount num_op;
    bool        link;

    // 'j' and 'jal' may both be used with the jump target given as either
    // an expression or a GPR; this means the instructions will end up being
    // encoded as j/jal or jr/jalr depending on operands.
    num_op = ins->num_operands;
    link   = table->opcode & 1;
    // First check if the operands are of the right types
    if( !jmpOperandsValidate( ins, num_op, link ) )
        return;

    op0 = ins->operands[0];
    if( op0->type == OP_IMMED ) {   // real j/jal (to absolute address)
        doAbsJump( buffer, table, op0, reloc );
    } else if( num_op == 1 ) {
        if( link ) {    // jalr $ra,rs
            if( op0->reg == MIPS_RETURN_ADDR ) {
                // TODO: warn - non-restartable instruction
            }
            doOpcodeRType( buffer, 0, FNCCODE_JALR, RegIndex( MIPS_RETURN_ADDR ),
                RegIndex( op0->reg ), 0 );
        } else {        // jr rs
            doOpcodeRType( buffer, 0, FNCCODE_JR, 0, RegIndex( op0->reg ), 0 );
        }
    } else {    // jalr rd,rs
        op1 = ins->operands[1];
        if( op0->reg == op0->reg ) {
            // TODO: warn - non-restartable instruction
        }
        doOpcodeRType( buffer, 0, FNCCODE_JALR, RegIndex( op1->reg ),
            RegIndex( op0->reg ), 0 );
    }
    numExtendedIns += doDelaySlotNOP( buffer );
    return;
}


static bool retOperandsValidate( instruction *ins, ins_opcount num_op )
//*********************************************************************
// Can be used by ret, jsr_coroutine
{
    static op_type  verify1[][3] = { { OP_GPR, OP_NOTHING, OP_NOTHING },
                                     { OP_IMMED, OP_NOTHING, OP_NOTHING } };
    static op_type  verify2[][3] = { { OP_GPR, OP_REG_INDIRECT, OP_NOTHING },
                                     { OP_GPR, OP_IMMED, OP_NOTHING },
                                     { OP_REG_INDIRECT, OP_IMMED, OP_NOTHING }};
    static op_type  verify3[][3] = { { OP_GPR, OP_REG_INDIRECT, OP_IMMED } };
    ot_array        *verify;
    ot_array        *verify_table[3] = { (ot_array *)&verify1, &verify2, (ot_array *)&verify3 };
    unsigned        num_var;

    if( num_op == 0 )
        return( true );
    assert( num_op <= 3 );
    verify = verify_table[num_op - 1];
    if( num_op == 1 ) {
        num_var = sizeof( verify1 ) / sizeof( **verify1 ) / 3;
    } else if( num_op == 2 ) {
        num_var = sizeof( verify2 ) / sizeof( **verify2 ) / 3;
    } else {
        num_var = sizeof( verify3 ) / sizeof( **verify3 ) / 3;
    }
    assert( num_var <= MAX_VARIETIES );
    return( opValidate( verify, ins, num_op, num_var ) );
}


static void ITRet( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//****************************************************************************************
// Both ret and jsr coroutine use this
{
    ins_operand     *op0, *op1;
    ins_opcount     num_op;
    uint_8          d_reg_idx;  // default d_reg if not specified

    num_op = ins->num_operands;
    // First check if the operands are of the right types
    if( !retOperandsValidate( ins, num_op ) ) return;
    if( num_op == 3 ) {
        stdMemJump( table, ins, buffer, reloc );
        return;
    }
    if( table->funccode == 0x0003 ) { // jsr_coroutine
        // This is according to the MS asaxp documentation.
        d_reg_idx = MIPS_RETURN_ADDR;
    } else {
        assert( table->funccode == 0x0002 ); // ret
        d_reg_idx = 31; // $zero
    }
    if( num_op == 2 ) {
        if( (op0 = ins->operands[0])->type == OP_GPR ) {
            if( (op1 = ins->operands[1])->type == OP_REG_INDIRECT ) {
                doMemJump( buffer, table, RegIndex( op0->reg ),
                           RegIndex( op1->reg ), NULL, 1, reloc );
                return;
            }
            assert( op1->type == OP_IMMED );
            doMemJump( buffer, table, RegIndex( op0->reg ), MIPS_RETURN_ADDR,
                       op1, op1->constant, reloc );
            return;
        }
        assert( op0->type == OP_REG_INDIRECT );
        op1 = ins->operands[1];
        assert( op1->type == OP_IMMED );
        doMemJump( buffer, table, d_reg_idx, RegIndex( op0->reg ),
                   op1, op1->constant, reloc );
        return;
    }
    if( num_op == 1 ) {
        if( (op0 = ins->operands[0])->type == OP_GPR ) {
            doMemJump( buffer, table, RegIndex( op0->reg ), MIPS_RETURN_ADDR,
                       NULL, 1, reloc );
            return;
        }
        assert( op0->type == OP_IMMED );
        doMemJump( buffer, table, d_reg_idx, MIPS_RETURN_ADDR,
                   op0, op0->constant, reloc );
        return;
    }
    assert( num_op == 0 );
    doMemJump( buffer, table, d_reg_idx, MIPS_RETURN_ADDR, NULL, 1, reloc );
}


static void ITMemNone( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//********************************************************************************************
{
    ins = ins;
    assert( ins->num_operands == 0 );
    reloc = reloc;
    doOpcodeRsRt( buffer, table->opcode, 31, 31, _Mem_Func( table->funccode ) );
}


static void ITBranch( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//*******************************************************************************************
{
    ins_operand *op;

    assert( ins->num_operands == 2 );
    op = ins->operands[1];
    doOpcodeRsRt( buffer, table->opcode, RegIndex( ins->operands[0]->reg ), 0,
                  _Immed( _Longword_offset( op->constant ) ) );
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
                  _Immed( _Longword_offset( op->constant ) ) );
    doReloc( reloc, op, OWL_RELOC_BRANCH_REL, buffer );
    numExtendedIns += doDelaySlotNOP( buffer );
}


static void ITBranchZero( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//***********************************************************************************************
{
    ins_operand     *op;

    assert( ins->num_operands == 2 );
    op = ins->operands[1];
    doOpcodeIType( buffer, table->opcode, table->funccode,
                   RegIndex( ins->operands[0]->reg ),
                  _Immed( _Longword_offset( op->constant ) ) );
    doReloc( reloc, op, OWL_RELOC_BRANCH_REL, buffer );
    numExtendedIns += doDelaySlotNOP( buffer );
}


static void ITBranchCop( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//**********************************************************************************************
{
    ins_operand     *op;
    uint_32         opcode;

    assert( ins->num_operands == 1 );
    opcode = cop_codes[table->opcode >> 8];
    op = ins->operands[0];
    doOpcodeIType( buffer, opcode, table->funccode, table->opcode & 0xff,
                  _Immed( _Longword_offset( op->constant ) ) );
    doReloc( reloc, op, OWL_RELOC_BRANCH_REL, buffer );
    numExtendedIns += doDelaySlotNOP( buffer );
}


static void ITCop0Spc( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//********************************************************************************************
{
    uint_32         opcode;

    ins = ins; reloc = reloc;
    assert( ins->num_operands == 0 );
    opcode = cop_codes[0];
    doOpcodeCopOp( buffer, opcode, table->opcode, 0 );
}


static void ITOperate( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//********************************************************************************************
{
    assert( ins->num_operands == 3 );
    reloc = reloc;
    doOpcodeRType( buffer, table->opcode, table->funccode,
                   RegIndex( ins->operands[0]->reg ),
                   RegIndex( ins->operands[1]->reg ),
                   RegIndex( ins->operands[2]->reg ) );
}


static void ITMulDiv( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//*******************************************************************************************
{
    assert( ins->num_operands == 2 );
    reloc = reloc;
    doOpcodeRType( buffer, table->opcode, table->funccode, 0,
                   RegIndex( ins->operands[0]->reg ),
                   RegIndex( ins->operands[1]->reg ) );
}


static void ITMovFromSpc( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//***********************************************************************************************
{
    assert( ins->num_operands == 1 );
    reloc = reloc;
    doOpcodeRType( buffer, table->opcode, table->funccode,
                   RegIndex( ins->operands[0]->reg ), 0, 0 );
}


static void ITMovToSpc( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//*********************************************************************************************
{
    assert( ins->num_operands == 1 );
    reloc = reloc;
    doOpcodeRType( buffer, table->opcode, table->funccode, 0,
                   RegIndex( ins->operands[0]->reg ), 0 );
}


static void ITMovCop( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//*******************************************************************************************
{
    assert( ins->num_operands == 2 );
    reloc = reloc;
    doOpcodeRType( buffer, table->opcode, 0, RegIndex( ins->operands[1]->reg ),
        table->funccode, RegIndex( ins->operands[0]->reg ) );
}


static void ITOperateImm( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//***********************************************************************************************
{
    ins_operand     *op;

    assert( ins->num_operands == 3 );
    reloc = reloc;
    op = ins->operands[2];
    ensureOpAbsolute( op, 2 );
    doOpcodeFcRsRtImm( buffer, table->opcode, table->funccode,
                       RegIndex( ins->operands[1]->reg ),
                       RegIndex( ins->operands[0]->reg ), op->constant );
}


static void ITShiftImm( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//*********************************************************************************************
{
    ins_operand     *op;

    assert( ins->num_operands == 3 );
    reloc = reloc;
    op = ins->operands[2];
    doOpcodeFcRdRtSa( buffer, table->opcode, table->funccode,
                      RegIndex( ins->operands[0]->reg ),
                      RegIndex( ins->operands[1]->reg ), op->constant );
}


static void ITFPOperate( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//**********************************************************************************************
{
    ins_funccode    fc;

    assert( ins->num_operands == 3 );
    reloc = reloc;
    fc = getFuncCode( table, ins );
    doFPInst( buffer, table->opcode, RegIndex( ins->operands[0]->reg ),
              RegIndex( ins->operands[1]->reg ),
              RegIndex( ins->operands[2]->reg ), fc );
}


static void ITFPConvert( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//**********************************************************************************************
{
    ins_funccode    fc;

    assert( ins->num_operands == 2 );
    reloc = reloc;
    fc = getFuncCode( table, ins );
    doFPInst( buffer, table->opcode, 31, RegIndex( ins->operands[0]->reg ),
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
        doOpcodeRsRt( buffer, table->opcode, 31, 0,
                      _Immed( _Longword_offset( op0->constant ) ) );
        doReloc( reloc, op0, OWL_RELOC_BRANCH_REL, buffer );
        return;
    }
    // num_operands == 2
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
        uint_8          reg_num;

        reg_num = RegIndex( ins->operands[0]->reg );
        doFPInst( buffer, table->opcode, reg_num, reg_num, reg_num,
                  table->funccode );
        return;
    }
    ITFPOperate( table, ins, buffer, reloc );
}


static void ITPseudoClr( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//**********************************************************************************************
{
    assert( ins->num_operands == 1 );
    reloc = reloc;
    doOpcodeFcRsRt( buffer, table->opcode, table->funccode, 31,
                    RegIndex( ins->operands[0]->reg ), _Rd( 31 ) );
}


static void ITPseudoFclr( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//***********************************************************************************************
{
    assert( ins->num_operands == 1 );
    reloc = reloc;
    doFPInst( buffer, table->opcode, 31, 31,
              RegIndex( ins->operands[0]->reg ), table->funccode );
}


static void ITPseudoLImm( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//***********************************************************************************************
{
    assert( ins->num_operands == 2 );
    table = table;
    reloc = reloc;
    doLoadImm( buffer, ins->operands );
}


static void ITPseudoLAddr( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//************************************************************************************************
{
    ins_operand         *op;
    ins_operand         *ops[2];
    unsigned            inc;
//    op_const            val;
    uint_8              s_reg;

    table = table;
    assert( ins->num_operands == 2 );
    op = ins->operands[1];
    // If op is IMMED foo, it's actually REG_INDIRECT that we want: foo($zero)
    if( op->type == OP_IMMED ) {
        op->type = OP_REG_INDIRECT;
        op->reg = ZERO_REG;
    }
    assert( op->type == OP_REG_INDIRECT );
//    val = op->constant;
    s_reg = RegIndex( op->reg );
    if( !OP_HAS_RELOC( op ) && s_reg == MIPS_ZERO_SINK ) {
        // doMov() can only be called when op->reg is ZERO_REG and no reloc
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
            // We should emit lui/addiu pair.
            inc = loadConst32( buffer, RegIndex( ins->operands[0]->reg ),
                               s_reg, op, op->constant, reloc, true );
            numExtendedIns += inc - 1;
            return;
        }
    }
    inc = loadConst32( buffer, RegIndex( ins->operands[0]->reg ), s_reg, op,
                       op->constant, reloc, false );
    numExtendedIns += inc - 1;
}


static void ITPseudoMov( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//**********************************************************************************************
{
    table = table;
    assert( ins->num_operands == 2 );
    reloc = reloc;

    doOpcodeRType( buffer, 0, FNCCODE_OR, RegIndex( ins->operands[0]->reg ),
        RegIndex( ins->operands[1]->reg ), MIPS_ZERO_SINK );
}


static void ITPseudoFmov( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//***********************************************************************************************
{
    uint_8          reg_idx0;

    assert( ins->num_operands == 2 );
    reloc = reloc;
    reg_idx0 = RegIndex( ins->operands[0]->reg );
    doFPInst( buffer, table->opcode, reg_idx0, reg_idx0,
              RegIndex( ins->operands[1]->reg ), table->funccode );
}


static void ITPseudoNot( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//**********************************************************************************************
{
    ins_funccode    fc;
    ins_operand     *op;
    uint_32         extra;

    assert( ins->num_operands == 2 );
    reloc = reloc;

    op = ins->operands[0];
    if( op->type == OP_GPR ) {
        extra = _Rt( RegIndex( op->reg ) );
    } else {    // OP_IMMED
        assert( op->type == OP_IMMED );
        extra = _LIT( op->constant );
        (void)ensureOpAbsolute( op, 0 );
    }
    fc = getFuncCode( table, ins );
    doOpcodeFcRsRt( buffer, table->opcode, fc, 31,
                    RegIndex( ins->operands[1]->reg ), extra );
}


static void ITPseudoNegf( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//***********************************************************************************************
{
    ins_funccode    fc;

    assert( ins->num_operands == 2 );
    reloc = reloc;
    fc = getFuncCode( table, ins );
    doFPInst( buffer, table->opcode, 31, RegIndex( ins->operands[0]->reg ),
              RegIndex( ins->operands[1]->reg ), fc );
}


static void ITPseudoFneg( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//***********************************************************************************************
{
    uint_8          reg_idx0;

    assert( ins->num_operands == 2 );
    reloc = reloc;
    reg_idx0 = RegIndex( ins->operands[0]->reg );
    doFPInst( buffer, table->opcode, reg_idx0, reg_idx0,
              RegIndex( ins->operands[1]->reg ), table->funccode );
}


static void ITPseudoAbs( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc )
//**********************************************************************************************
{
    uint_8          s_reg_idx, d_reg_idx, rc_reg_idx;
    ins_operand     *src_op;
    bool            same_reg;

    assert( ins->num_operands == 2 );
    reloc = reloc;
    src_op = ins->operands[0];
    if( src_op->type == OP_IMMED ) {
        // Then just evaluate it and do a mov instead.
        // mov  abs(src_op->constant), d_reg
        if( ensureOpAbsolute( src_op, 0 ) ) {
            doMov( buffer, ins->operands, DOMOV_ABS );
        }
        return;
    }
    assert( src_op->type == OP_GPR );
    /* Emit an instruction sequence like:
       1. subq/v  $zero, $s_reg, $at    // if( $s_reg == $d_reg )
       2. cmovlt  $s_reg, $at, $s_reg
       ---or---
       1. subq/v  $zero, $s_reg, $d_reg // if( $s_reg != $d_reg )
       2. cmovgt  $s_reg, $s_reg, $d_reg
    */
    s_reg_idx = RegIndex( src_op->reg );
    d_reg_idx = RegIndex( ins->operands[1]->reg );
    same_reg = ( s_reg_idx == d_reg_idx );
    if( same_reg ) {
        // Then $at reg will be needed.
        if( !_DirIsSet( AT ) ) {
            Warning( INS_USES_AT_REG );
        }
        rc_reg_idx = AT_REG_IDX;
    } else {
        rc_reg_idx = d_reg_idx;
    }
    doOpcodeFcRsRt( buffer, table->opcode, table->funccode, 31, rc_reg_idx, _Rt( s_reg_idx ) );
    // So buffer gets ins #1. Now do ins #2.
    ++buffer;
#define OPCODE_CMOVGT           0x11
#define OPCODE_CMOVLT           0x11
#define FUNCCODE_CMOVGT         0x0066
#define FUNCCODE_CMOVLT         0x0044
    if( same_reg ) {
        doOpcodeFcRsRt( buffer, OPCODE_CMOVLT,
                        FUNCCODE_CMOVLT, s_reg_idx, s_reg_idx,
                        _Rt( AT_REG_IDX ) );
    } else {
        doOpcodeFcRsRt( buffer, OPCODE_CMOVGT,
                        FUNCCODE_CMOVGT, s_reg_idx, d_reg_idx,
                        _Rt( s_reg_idx ) );
    }
    ++numExtendedIns;
}


mips_format MIPSFormatTable[] = {
    #define PICK( a, b, c, d, e )       { b, { c, d, e } },
    #include "mipsfmt.inc"
    #undef PICK
};


bool MIPSValidate( instruction *ins )
//***********************************
// Make sure that all operands of the given instruction
// are of the type we are expecting. If not, we print
// out an error message.
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
    }   // NOTE: It might not catch all improper operand combinations
        // because we're using flags here
    return( true );
}


#ifdef _STANDALONE_
static void emitIns( owl_section_handle hdl, char *inscode, int size )
//********************************************************************
{
    ObjEmitData( hdl, inscode, size, true );
}
#else
static void emitIns( char *inscode, int size )
//********************************************
{
    ObjEmitData( inscode, size, true );
}
#endif


#ifdef _STANDALONE_
void MIPSEmit( owl_section_handle hdl, instruction *ins )
//*******************************************************
#else
void MIPSEmit( instruction *ins )
//*******************************
#endif
// Encode the given instruction (including emitting any
// relocs to the appropriate places), and emit the code
// to the given section.
{
    int             ctr;
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
#ifdef AS_DEBUG_DUMP
    #ifdef _STANDALONE_
        if( _IsOption( DUMP_INSTRUCTIONS ) ) {
            printf( " [%#010x]\n", result[ctr] );
        }
    #endif
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
