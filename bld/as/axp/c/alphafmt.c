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
* Description:  Alpha AXP instruction formats and encodings.
*
****************************************************************************/


#include "as.h"
#ifndef _STANDALONE_
#include "asinline.h"
#endif

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
    op_type     ops[ MAX_OPERANDS ];
} alpha_format;

typedef enum {
    DOMOV_ORIGINAL,
    DOMOV_ABS,
} domov_option;

#define MAX_VARIETIES   3
typedef op_type ot_array[MAX_VARIETIES][3];

#define _FiveBits( x )          ( (x) & 0x001f )
#define _SixBits( x )           ( (x) & 0x003f )
#define _SevenBits( x )         ( (x) & 0x007f )
#define _EightBits( x )         ( (x) & 0x00ff )
#define _ElevenBits( x )        ( (x) & 0x07ff )
#define _FourteenBits( x )      ( (x) & 0x3fff )
#define _SixteenBits( x )       ( (x) & 0xffff )
#define _TwentyOneBits( x )     ( (x) & 0x001fffff )

#define _LIT_value( x )         ( _EightBits( x )     << 1  )
#define _LIT_bit                1
#define _LIT_unshifted( x )     ( _LIT_value( x ) | _LIT_bit )

#define _Opcode( x )            ( _SixBits( x )       << 26 )
#define _Ra( x )                ( _FiveBits( x )      << 21 )
#define _Rb( x )                ( _FiveBits( x )      << 16 )
#define _Rc( x )                ( _FiveBits( x )      << 0  )
#define _Memory_disp( x )       ( _SixteenBits( x )   << 0  )
#define _Mem_Func( x )          ( _SixteenBits( x )   << 0  )
#define _Branch_disp( x )       ( _TwentyOneBits( x ) << 0  )
#define _Op_Func( x )           ( _SevenBits( x )     << 5  )
#define _FP_Op_Func( x )        ( _ElevenBits( x )    << 5  )
#define _LIT( x )               ( _LIT_unshifted( x ) << 12 )

#define _Longword_offset( x )   ( (x) >> 2 )

#define MAX_EXTENDED_INS        20  // Including the default buffer, we have
                                    // 21 dwords for an ins that emits multiple
                                    // instructions. (eg. ldb)

#define RA_REG_IDX              26
#define SP_REG_IDX              30
#define FP_REG_IDX              15
#define ZERO_REG_IDX            31
// Check if ".set noat" is in effect before using this
#define AT_REG_IDX              RegIndex( AT_REG )

#define ZERO_REG                MakeReg( RC_GPR, ZERO_REG_IDX )

#define OPCODE_BIS      0x11
#define FUNCCODE_BIS    0x0020
#define OPCODE_LDA      0x8
#define OPCODE_LDAH     0x9

#define OP_HAS_RELOC( op )      ((op)->flags & (RELOC | UNNAMED_RELOC))
#define OP_RELOC_NAMED( op )    ((op)->flags & RELOC)

static unsigned numExtendedIns = 0;  // >= 1 when we use extendedIns
static uint_32 result[ MAX_EXTENDED_INS + 1 ];

static owl_reloc_type reloc_translate[] = {
    OWL_RELOC_ABSOLUTE,     // Corresponds to ASM_RELOC_UNSPECIFIED
    OWL_RELOC_WORD,
    OWL_RELOC_HALF_HI,
    OWL_RELOC_HALF_HA,
    OWL_RELOC_HALF_LO,
    OWL_RELOC_BRANCH_REL,   // j^ reloc
    OWL_RELOC_JUMP_REL,     // jump hint
};

static bool ensureOpAbsolute( ins_operand *op, uint_8 opIdx ) {
//*************************************************************

    if( OP_HAS_RELOC( op ) ) {
        Error( RELOC_NOT_ALLOWED, opIdx );
        return( FALSE );
    }
    return( TRUE );
}

static void addReloc( asm_reloc *reloc, op_reloc_target target, owl_reloc_type type, unsigned loc, bool is_named ) {
//******************************************************************************************************************

    reloc_list  new_entry;

    new_entry = MemAlloc( sizeof( struct reloc_entry ) );
    new_entry->next = NULL;
    new_entry->target = target;
    new_entry->type = type;
    new_entry->loc = loc;
    new_entry->target = target;
    new_entry->is_named = is_named;
    if( reloc->first == NULL ) {
        reloc->first = new_entry;
    } else {
        reloc->last->next = new_entry;
    }
    reloc->last = new_entry;
}

static owl_reloc_type relocType( asm_reloc_type type, owl_reloc_type default_type ) {
//***********************************************************************************

    owl_reloc_type  ret;

    if( type == ASM_RELOC_UNSPECIFIED ) {
        return( default_type );
    }
    switch( default_type ) {
    case OWL_RELOC_HALF_HI:
    case OWL_RELOC_HALF_LO:
        if( ( ret = reloc_translate[type] ) != OWL_RELOC_HALF_HI &&
            ret != OWL_RELOC_HALF_LO ) {
            Error( INVALID_RELOC_MODIFIER );
        }
        break;
    case OWL_RELOC_BRANCH_REL:  // j^ reloc
        if( ( ret = reloc_translate[type] ) != default_type ) {
            Error( INVALID_RELOC_MODIFIER );
        }
        break;
    case OWL_RELOC_JUMP_REL:    // jump hint
        // we accept j^ to be specified for jump hint for now.
        if( ( ret = reloc_translate[type] ) != OWL_RELOC_JUMP_REL &&
            ret != OWL_RELOC_BRANCH_REL ) {
            Error( INVALID_RELOC_MODIFIER );
        }
        ret = OWL_RELOC_JUMP_REL;
        break;
    default:
        //Error( "internal - unexpected default type" );
        assert( FALSE );
        ret = OWL_RELOC_ABSOLUTE;
    }
    return( ret );
}

static void doReloc( asm_reloc *reloc, ins_operand *op, owl_reloc_type rtype, uint_32 *offset ) {
//***********************************************************************************************

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
static ins_funccode getFuncCode( ins_table *table, instruction *ins ) {
//*********************************************************************

    ins_funccode fc;

    fc = table->funccode;
    switch( table->template ) {
    case IT_OPERATE:
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
        assert( FALSE ); // Others should have no need to call this.
    }
    return( fc );
}

static void doOpcodeRaRb( uint_32 *buffer, ins_opcode opcode, uint_8 ra, uint_8 rb, uint_32 remain ) {
//****************************************************************************************************

    *buffer = _Opcode( opcode ) | _Ra( ra ) | _Rb( rb ) | remain;
}

static void doOpcodeFcRaRc( uint_32 *buffer, ins_opcode opcode, ins_funccode fc, uint_8 ra, uint_8 rc, uint_32 extra ) {
//**********************************************************************************************************************
// This procedure doesn't fill in all the bits (missing bits 20-12).
// But we can fill it in using extra.

    *buffer = _Opcode( opcode ) | _Op_Func( fc ) | _Ra( ra ) | _Rc( rc ) |
              extra;
}

static void doFPInst( uint_32 *buffer, ins_opcode opcode, uint_8 ra, uint_8 rb, uint_8 rc, uint_32 remain ) {
//***********************************************************************************************************

    *buffer = _Opcode( opcode ) | _Ra( ra ) | _Rb( rb ) |
              _FP_Op_Func( remain ) | _Rc( rc );
}

#ifndef _STANDALONE_
static void doAutoVar( asm_reloc *reloc, op_reloc_target targ, uint_32 *buffer, ins_table *table, instruction *ins ) {
//********************************************************************************************************************

    ins_operand *op;

    op = ins->operands[1];
    if( op->reg != ZERO_REG ) {
        Error( BAD_BASE_REG_FOR_STACKVAR );
        return;
    }
    addReloc( reloc, targ, OWL_RELOC_FP_OFFSET, (unsigned)( (char *)buffer - (char *)result ), TRUE );
    doOpcodeRaRb( buffer, table->opcode, RegIndex( ins->operands[0]->reg ),
                  FP_REG_IDX, 0 );
}
#endif

static unsigned ldaConst32( uint_32 *buffer, uint_8 d_reg, uint_8 s_reg, ins_operand *op, op_const c, asm_reloc *reloc, bool force_pair ) {
//*****************************************************************************************************************************************
// LDA-LDAH sequence for 32-bit constants
// Given: lda $d_reg, foobar+c($s_reg)
//        info for foobar is stored in op
//        c should be passed in also
// Returns # of ins generated

    int_32              tmp;
    int                 ctr;
    unsigned            ret = 1;
    int_16              low, high[2];
    uint_8              base_reg;
    owl_reloc_type      type;
    bool                hi_reloc_emitted = FALSE;

    if( force_pair ) {
        assert( reloc != NULL );
        if( op->reloc.type != ASM_RELOC_UNSPECIFIED ) {
            Error( INVALID_RELOC_MODIFIER );
            return( ret );
        }
    }
    low = ( c & 0xffff );
    tmp = c - (int_32)low;
    high[0] = ( ( tmp & 0xffff0000 ) >> 16 );
    if( c >= 0x7fff8000 ) {
        // if c is in range 0x7FFF8000..0x7FFFFFFF, tmp = 0x80000000
        // => high[0] = 0x8000 => negative! So we need to split high[0]
        // up to high[0] + high[1] and load them up in different LDAH's.
        high[1] = 0x4000;
        tmp -= 0x40000000;
        high[0] = ( ( tmp & 0xffff0000 ) >> 16 );
    } else {
        high[1] = 0;
    }
    for( ctr = 0; ctr < 2; ctr++ ) {
        if( high[ctr] != 0 ) {
            base_reg = ( ctr == 0 ? s_reg : d_reg );
            doOpcodeRaRb( buffer, OPCODE_LDAH,
                          d_reg, base_reg, _Memory_disp( high[ctr] ) );
            if( reloc && !hi_reloc_emitted ) {
                type = relocType( op->reloc.type, OWL_RELOC_HALF_HI );
                if( type == OWL_RELOC_HALF_HI ) {
                    doReloc( reloc, op, OWL_RELOC_HALF_HI, buffer );
                    hi_reloc_emitted = TRUE; // only need to generate h^ once
                } else {
                    assert( type == OWL_RELOC_HALF_LO );
                    // a l^ modifier was specified explicitly
                    // Do the reloc at the end (lda)
                    // Doing it in ldah will be wrong.
                }
            }
            ++ret;
            ++buffer;
        }
    }
    if( !hi_reloc_emitted && force_pair ) { // no LDAH has been generated yet
        // force_pair => no modifier should've been specified
        // We are asked to force one, so here
        doOpcodeRaRb( buffer, OPCODE_LDAH, d_reg, s_reg,
                      _Memory_disp( 0 ) );
        doReloc( reloc, op, OWL_RELOC_HALF_HI, buffer );
        ++buffer;
        ++ret;
    }
    base_reg = ( ret == 1 ? s_reg : d_reg );
    doOpcodeRaRb( buffer, OPCODE_LDA, d_reg, base_reg,
                  _Memory_disp( low ) );
    if( reloc ) {
        doReloc( reloc, op, OWL_RELOC_HALF_LO, buffer );
    }
    return( ret );
}

#if 0
static unsigned forceLoadAddressComplete( uint_8 d_reg, uint_8 s_reg, ins_operand *op, uint_32 *buffer, asm_reloc *reloc ) {
//***********************************************************************************************************************

    /* Generate:
        LDAH    $d_reg, h^foo($s_reg)
        LDA     $d_reg, l^foo+c($d_reg)
       store them in buffer[0], buffer[1]
    */
    doOpcodeRaRb( buffer, OPCODE_LDAH, d_reg, s_reg,
                  _Memory_disp( 0 ) );
    doReloc( reloc, op, OWL_RELOC_HALF_HI, buffer );
    buffer++;
    doOpcodeRaRb( buffer, OPCODE_LDA, d_reg, d_reg,
                  _Memory_disp( op->constant ) );
    doReloc( reloc, op, OWL_RELOC_HALF_LO, buffer );
    return( 2 );
}
#endif


static unsigned load32BitLiteral( uint_32 *buffer, ins_operand *op0, ins_operand *op1, domov_option m_opt ) {
//***********************************************************************************************************

    op_const        val;

    if( m_opt == DOMOV_ABS ) {
        val = abs( op0->constant );
    } else {
        val = op0->constant;
    }
    return( ldaConst32( buffer, RegIndex( op1->reg ), ZERO_REG_IDX,
                        op0, val, NULL, FALSE ) );
}

static void doMov( uint_32 *buffer, ins_operand *operands[], domov_option m_opt ) {
//*********************************************************************************

    ins_operand     *op0, *op1;
    uint_32         extra;
    uint_32         abs_val;
    bool            ready = TRUE;

    op0 = operands[0];
    op1 = operands[1];
    if( op0->type == OP_GPR ) {
        extra = _Rb( RegIndex( op0->reg ) );
    } else if( ( op0->constant & 0xff ) == op0->constant ) { // OP_IMMED implied
        extra = _LIT( op0->constant ); // this lit is between 0..255
        (void)ensureOpAbsolute( op0, 0 );
    } else if( m_opt == DOMOV_ABS &&
               ( ( ( abs_val = abs( op0->constant ) ) & 0xff ) == abs_val ) ) {
        extra = _LIT( abs_val ); // this lit is between 0..255
        // ensureOpAbsolute( op0, 0 );  should be done before calling doMov
    } else {
        ready = FALSE;
    }
    if( ready ) {
        doOpcodeFcRaRc( buffer, OPCODE_BIS, FUNCCODE_BIS,
                        ZERO_REG_IDX, RegIndex( op1->reg ), extra );
        return;
    }
    // Otherwise it's OP_IMMED with a greater than 8-bit literal.
    // We'll then use multiple LDA, LDAH instructions to load the literal.
    if( !ensureOpAbsolute( op0, 0 ) ) return;
    numExtendedIns += load32BitLiteral( buffer, op0, op1, m_opt ) - 1;
}

static void ITLoadAddress( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//**************************************************************************************************

    ins_operand         *op;
    ins_operand         *ops[2];
    unsigned            inc;
    op_const            val;
    uint_8              s_reg;

    table = table;
    assert( ins->num_operands == 2 );
    assert( table->opcode == OPCODE_LDA );
    op = ins->operands[1];
    // If op is IMMED foo, it's actually REG_INDIRECT that we want: foo($zero)
    if( op->type == OP_IMMED ) {
        op->type = OP_REG_INDIRECT;
        op->reg = ZERO_REG;
    }
    assert( op->type == OP_REG_INDIRECT );
    val = op->constant;
    s_reg = RegIndex( op->reg );
    if( !OP_HAS_RELOC( op ) && s_reg == ZERO_REG_IDX ) {
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
            // Then we should emit LDA-LDAH pair.
            inc = ldaConst32( buffer, RegIndex( ins->operands[0]->reg ),
                              s_reg, op, op->constant, reloc, TRUE );
            numExtendedIns += inc - 1;
            return;
        }
    }
    inc = ldaConst32( buffer, RegIndex( ins->operands[0]->reg ), s_reg, op,
                      op->constant, reloc, FALSE );
    numExtendedIns += inc - 1;
/*
    doOpcodeRaRb( buffer, table->opcode, RegIndex( ins->operands[0]->reg ),
                  s_reg, _Memory_disp( val ) );
    doReloc( reloc, op, OWL_RELOC_HALF_LO, buffer );*/
}

#if 0
static void ITMemLDAH( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//**********************************************************************************************

    ins_operand *op;

    assert( table->opcode == OPCODE_LDAH );
    op = ins->operands[1];
    // If op is IMMED foo, it's actually REG_INDIRECT that we want: foo($zero)
    if( op->type == OP_IMMED ) {
        op->type = OP_REG_INDIRECT;
        op->reg = ZERO_REG;
    }
    assert( op->type == OP_REG_INDIRECT );
    doOpcodeRaRb( buffer, table->opcode, RegIndex( ins->operands[0]->reg ),
                  RegIndex( op->reg ), _Memory_disp( op->constant ) );
    doReloc( reloc, op, OWL_RELOC_HALF_HI, buffer );
}
#endif

static void ITMemAll( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*********************************************************************************************

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
    doOpcodeRaRb( buffer, opcode, RegIndex( ins->operands[0]->reg ),
                  RegIndex( op->reg ), _Memory_disp( op->constant ) );
    type = ( opcode == OPCODE_LDAH ) ? OWL_RELOC_HALF_HI : OWL_RELOC_HALF_LO;
    doReloc( reloc, op, type, buffer );
}

static void ITMemA( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*******************************************************************************************

    assert( ins->num_operands == 1 );
    reloc = reloc;
    doOpcodeRaRb( buffer, table->opcode, RegIndex( ins->operands[0]->reg ),
                  31, _Mem_Func( table->funccode ) );
}

static void ITMemB( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*******************************************************************************************

    assert( ins->num_operands == 1 );
    reloc = reloc;
    doOpcodeRaRb( buffer, table->opcode, 31, RegIndex( ins->operands[0]->reg ),
                  _Mem_Func( table->funccode ) );
}

static void doMemJump( uint_32 *buffer, ins_table *table, uint_8 ra, uint_8 rb, ins_operand *addr_op, uint_32 hint, asm_reloc *reloc ) {
//**************************************************************************************************************************************

    // Note that addr_op maybe NULL. If not, addr_op->constant == hint.
    assert( addr_op == NULL || addr_op->constant == hint );
    assert( addr_op == NULL || addr_op->type == OP_IMMED );
    doOpcodeRaRb( buffer, table->opcode, ra, rb,
                  (table->funccode << 14) | _FourteenBits(hint) );
    doReloc( reloc, addr_op, OWL_RELOC_JUMP_REL, buffer );
}

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

static bool opValidate( ot_array *verify, instruction *ins, ins_opcount num_op, unsigned num_var ) {
//**************************************************************************************************

    int             ctr, var, lasterr;
    op_type         actual, wanted;

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
        return( FALSE );
    }
    return( TRUE );
}

static bool jmpOperandsValidate( instruction *ins, ins_opcount num_op ) {
//***********************************************************************
// Used by jmp, jsr

    static op_type  verify1[][3] = { { OP_REG_INDIRECT, OP_NOTHING, OP_NOTHING },
                                     { OP_IMMED, OP_NOTHING, OP_NOTHING } };
    static op_type  verify2[][3] = { { OP_GPR, OP_REG_INDIRECT, OP_NOTHING },
                                     { OP_GPR, OP_IMMED, OP_NOTHING },
                                     { OP_REG_INDIRECT, OP_IMMED, OP_NOTHING }};
    static op_type  verify3[][3] = { { OP_GPR, OP_REG_INDIRECT, OP_IMMED } };
    ot_array        *verify;
    ot_array        *verify_table[3] = { (ot_array *)&verify1, &verify2, (ot_array *)&verify3 };
    unsigned        num_var;

    if( num_op == 0 ) return( TRUE );
    assert( num_op <= 3 );
    verify = verify_table[ num_op - 1 ];
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

static void stdMemJump( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//***********************************************************************************************

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

static void ITMemJump( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//**********************************************************************************************

    ins_operand *op0, *op1;
    ins_opcount num_op;
    int         inc;
    uint_8      d_reg_idx;      // default d_reg if not specified

    num_op = ins->num_operands;
    // First check if the operands are of the right types
    if( !jmpOperandsValidate( ins, num_op ) ) return;
    if( num_op == 3 ) {
        stdMemJump( table, ins, buffer, reloc );
        return;
    }
    if( table->funccode == 0x0001 ) { // jsr
        // This is according to the MS asaxp documentation.
        d_reg_idx = RA_REG_IDX;
    } else {
        assert( table->funccode == 0x0000 ); // jmp
        d_reg_idx = 31; // $zero
    }
    if( num_op == 2 ) {
        if( (op0 = ins->operands[0])->type == OP_GPR ) {
            if( (op1 = ins->operands[1])->type == OP_REG_INDIRECT ) {
                doMemJump( buffer, table, RegIndex( op0->reg ),
                           RegIndex( op1->reg ), NULL, 0, reloc );
                return;
            }
            assert( op1->type == OP_IMMED );
            if( !_DirIsSet( AT ) ) {
                Warning( INS_USES_AT_REG );
            }
            /* load addr to $at (as s_reg) from op1 */
            inc = ldaConst32( buffer, AT_REG_IDX, ZERO_REG_IDX, op1,
                              op1->constant, reloc, TRUE );
            doMemJump( buffer + inc, table, RegIndex( op0->reg ), AT_REG_IDX,
                       NULL, 0, reloc );
            numExtendedIns += inc;  // total # of instructions = inc + 1
            return;
        }
        assert( op0->type == OP_REG_INDIRECT );
        op1 = ins->operands[1];
        assert( op1->type == OP_IMMED );
        doMemJump( buffer, table, d_reg_idx, RegIndex( op0->reg ),
                   op1, op1->constant, reloc );
        return;
    }
    assert( num_op == 1 );
    if( (op0 = ins->operands[0])->type == OP_REG_INDIRECT ) {
        doMemJump( buffer, table, d_reg_idx, RegIndex( op0->reg ),
                   NULL, 0, reloc );
        return;
    }
    assert( op0->type == OP_IMMED );
    if( !_DirIsSet( AT ) ) {
        Warning( INS_USES_AT_REG );
    }
    /* Gen code to load addr to $at (as s_reg) from op1 */
    inc = ldaConst32( buffer, AT_REG_IDX, ZERO_REG_IDX, op0, op0->constant, reloc, TRUE );
    doMemJump( buffer + inc, table, d_reg_idx, AT_REG_IDX,
               NULL, 0, reloc );
    numExtendedIns += inc;  // total # of instructions = inc + 1
    return;

}

static bool retOperandsValidate( instruction *ins, ins_opcount num_op ) {
//***********************************************************************
// Can be used by ret, jsr_coroutine

    static op_type  verify1[][3] = { { OP_GPR, OP_NOTHING, OP_NOTHING },
                                     { OP_IMMED, OP_NOTHING, OP_NOTHING } };
    static op_type  verify2[][3] = { { OP_GPR, OP_REG_INDIRECT, OP_NOTHING },
                                     { OP_GPR, OP_IMMED, OP_NOTHING },
                                     { OP_REG_INDIRECT, OP_IMMED, OP_NOTHING }};
    static op_type  verify3[][3] = { { OP_GPR, OP_REG_INDIRECT, OP_IMMED } };
    ot_array        *verify;
    ot_array        *verify_table[3] = { (ot_array *)&verify1, (ot_array *)&verify2, (ot_array *)&verify3 };
    unsigned        num_var;

    if( num_op == 0 ) return( TRUE );
    assert( num_op <= 3 );
    verify = verify_table[ num_op - 1 ];
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

static void ITRet( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//******************************************************************************************
// Both ret and jsr coroutine use this

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
        d_reg_idx = RA_REG_IDX;
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
            doMemJump( buffer, table, RegIndex( op0->reg ), RA_REG_IDX,
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
            doMemJump( buffer, table, RegIndex( op0->reg ), RA_REG_IDX,
                       NULL, 1, reloc );
            return;
        }
        assert( op0->type == OP_IMMED );
        doMemJump( buffer, table, d_reg_idx, RA_REG_IDX,
                   op0, op0->constant, reloc );
        return;
    }
    assert( num_op == 0 );
    doMemJump( buffer, table, d_reg_idx, RA_REG_IDX, NULL, 1, reloc );
}

static void ITMemNone( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//**********************************************************************************************

    ins = ins;
    assert( ins->num_operands == 0 );
    reloc = reloc;
    doOpcodeRaRb( buffer, table->opcode, 31, 31, _Mem_Func( table->funccode ) );
}

static void ITBranch( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*********************************************************************************************

    ins_operand *op;

    assert( ins->num_operands == 2 );
    op = ins->operands[1];
    doOpcodeRaRb( buffer, table->opcode, RegIndex( ins->operands[0]->reg ), 0,
                  _Branch_disp( _Longword_offset( op->constant ) ) );
    doReloc( reloc, op, OWL_RELOC_BRANCH_REL, buffer );
}

static void ITOperate( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//**********************************************************************************************

    ins_funccode    fc;
    ins_operand     *op;
    uint_32         extra;

    assert( ins->num_operands == 3 );
    reloc = reloc;
    op = ins->operands[1];
    if( op->type == OP_GPR ) {
        extra = _Rb( RegIndex( op->reg ) );
    } else {    // OP_IMMED
        assert( op->type == OP_IMMED );
        extra = _LIT( op->constant );
        (void)ensureOpAbsolute( op, 1 );
    }
    fc = getFuncCode( table, ins );
    doOpcodeFcRaRc( buffer, table->opcode, fc,
                    RegIndex( ins->operands[0]->reg ),
                    RegIndex( ins->operands[2]->reg ), extra );
}

static void ITFPOperate( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//************************************************************************************************

    ins_funccode    fc;

    assert( ins->num_operands == 3 );
    reloc = reloc;
    fc = getFuncCode( table, ins );
    doFPInst( buffer, table->opcode, RegIndex( ins->operands[0]->reg ),
              RegIndex( ins->operands[1]->reg ),
              RegIndex( ins->operands[2]->reg ), fc );
}

static void ITFPConvert( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//************************************************************************************************

    ins_funccode    fc;

    assert( ins->num_operands == 2 );
    reloc = reloc;
    fc = getFuncCode( table, ins );
    doFPInst( buffer, table->opcode, 31, RegIndex( ins->operands[0]->reg ),
              RegIndex( ins->operands[1]->reg ), fc );
}

static void ITBr( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*****************************************************************************************

    ins_operand *op0, *op1;
    assert( ins->num_operands == 1 || ins->num_operands == 2 );
    op0 = ins->operands[0];
    if( ins->num_operands == 1 ) {
        if( op0->type != OP_IMMED ) {
            Error( IMPROPER_OPERAND, 0 );
            return;
        }
        doOpcodeRaRb( buffer, table->opcode, 31, 0,
                      _Branch_disp( _Longword_offset( op0->constant ) ) );
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

static void ITMTMFFpcr( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//***********************************************************************************************

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

static void ITPseudoClr( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//************************************************************************************************

    assert( ins->num_operands == 1 );
    reloc = reloc;
    doOpcodeFcRaRc( buffer, table->opcode, table->funccode, 31,
                    RegIndex( ins->operands[0]->reg ), _Rb( 31 ) );
}

static void ITPseudoFclr( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*************************************************************************************************

    assert( ins->num_operands == 1 );
    reloc = reloc;
    doFPInst( buffer, table->opcode, 31, 31,
              RegIndex( ins->operands[0]->reg ), table->funccode );
}

static void ITPseudoMov( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//************************************************************************************************

    assert( ins->num_operands == 2 );
    table = table;
    reloc = reloc;
    doMov( buffer, ins->operands, DOMOV_ORIGINAL );
}

static void ITPseudoFmov( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*************************************************************************************************

    uint_8          reg_idx0;

    assert( ins->num_operands == 2 );
    reloc = reloc;
    reg_idx0 = RegIndex( ins->operands[0]->reg );
    doFPInst( buffer, table->opcode, reg_idx0, reg_idx0,
              RegIndex( ins->operands[1]->reg ), table->funccode );
}

static void ITPseudoNot( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//************************************************************************************************

    ins_funccode    fc;
    ins_operand     *op;
    uint_32         extra;

    assert( ins->num_operands == 2 );
    reloc = reloc;

    op = ins->operands[0];
    if( op->type == OP_GPR ) {
        extra = _Rb( RegIndex( op->reg ) );
    } else {    // OP_IMMED
        assert( op->type == OP_IMMED );
        extra = _LIT( op->constant );
        (void)ensureOpAbsolute( op, 0 );
    }
    fc = getFuncCode( table, ins );
    doOpcodeFcRaRc( buffer, table->opcode, fc, 31,
                    RegIndex( ins->operands[1]->reg ), extra );
}

static void ITPseudoNegf( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*************************************************************************************************

    ins_funccode    fc;

    assert( ins->num_operands == 2 );
    reloc = reloc;
    fc = getFuncCode( table, ins );
    doFPInst( buffer, table->opcode, 31, RegIndex( ins->operands[0]->reg ),
              RegIndex( ins->operands[1]->reg ), fc );
}

static void ITPseudoFneg( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//*************************************************************************************************

    uint_8          reg_idx0;

    assert( ins->num_operands == 2 );
    reloc = reloc;
    reg_idx0 = RegIndex( ins->operands[0]->reg );
    doFPInst( buffer, table->opcode, reg_idx0, reg_idx0,
              RegIndex( ins->operands[1]->reg ), table->funccode );
}

static void ITPseudoAbs( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//************************************************************************************************

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
    doOpcodeFcRaRc( buffer, table->opcode, table->funccode, 31, rc_reg_idx, _Rb( s_reg_idx ) );
    // So buffer gets ins #1. Now do ins #2.
    ++buffer;
#define OPCODE_CMOVGT           0x11
#define OPCODE_CMOVLT           0x11
#define FUNCCODE_CMOVGT         0x0066
#define FUNCCODE_CMOVLT         0x0044
    if( same_reg ) {
        doOpcodeFcRaRc( buffer, OPCODE_CMOVLT,
                        FUNCCODE_CMOVLT, s_reg_idx, s_reg_idx,
                        _Rb( AT_REG_IDX ) );
    } else {
        doOpcodeFcRaRc( buffer, OPCODE_CMOVGT,
                        FUNCCODE_CMOVGT, s_reg_idx, d_reg_idx,
                        _Rb( s_reg_idx ) );
    }
    ++numExtendedIns;
}

static void ITCallPAL( ins_table *table, instruction *ins, uint_32 *buffer, asm_reloc *reloc ) {
//**********************************************************************************************

    ins_operand     *op;
    op_const        constant;
    uint_32         opcode;

    assert( ins->num_operands == 1 );
    table = table;
    reloc = reloc;
    op = ins->operands[0];
    (void)ensureOpAbsolute( op, 0 );
    constant = op->constant;
    opcode = ( constant & 0xfC000000 ) >> 26;
    if( opcode != 0x00 &&
        opcode != 0x19 &&
        opcode != 0x1b &&
        opcode != 0x1d &&
        opcode != 0x1e &&
        opcode != 0x1f ) {
        Error( INVALID_PALCODE );
    }
    *buffer = constant;
}

alpha_format AlphaFormatTable[] = {
    #define PICK( a, b, c, d, e )       { b, { c, d, e } },
    #include "alphafmt.inc"
    #undef PICK
};

bool AlphaValidate( instruction *ins ) {
//**************************************
// Make sure that all operands of the given instruction
// are of the type we are expecting. If not, we print
// out an error message.

    int                 i;
    alpha_format        *fmt;
    ins_operand         *op;

    fmt = &AlphaFormatTable[ ins->format->table_entry->template ];
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
    }   // NOTE: It might not catch all improper operand combinations
        // because we're using flags here
    return( TRUE );
}

#ifdef _STANDALONE_
static void emitIns( owl_section_handle hdl, char *inscode, int size ) {
//**********************************************************************

    ObjEmitData( hdl, inscode, size, TRUE );
}
#else
static void emitIns( char *inscode, int size ) {
//**********************************************

    ObjEmitData( inscode, size, TRUE );
}
#endif

#ifdef _STANDALONE_
void AlphaEmit( owl_section_handle hdl, instruction *ins ) {
//**********************************************************
#else
void AlphaEmit( instruction *ins ) {
//**********************************
#endif
// Encode the given instruction (including emitting any
// relocs to the appropriate places), and emit the code
// to the given section.

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
    AlphaFormatTable[ table->template ].func( table, ins, result, &reloc );
    for( ctr = 0; ctr <= numExtendedIns; ctr++ ) {
        if( ( curr_reloc = reloc.first ) != NULL ) {
            assert( curr_reloc->loc >= ctr * sizeof( *result ) );
            if( curr_reloc->loc == ctr * sizeof( *result ) ) {
                reloc.first = curr_reloc->next;
                if( curr_reloc->is_named ) {
                    #ifdef _STANDALONE_
                    ObjEmitReloc( hdl, SymName( curr_reloc->target.ptr ),
                                  curr_reloc->type, TRUE, TRUE );
                    #else
                    ObjEmitReloc( SymName( curr_reloc->target.ptr ),
                                  curr_reloc->type, TRUE, TRUE );
                    #endif
                } else {
                    #ifdef _STANDALONE_
                    ObjEmitReloc( hdl, &curr_reloc->target.label,
                                  curr_reloc->type, TRUE, FALSE );
                    #else
                    ObjEmitReloc( &curr_reloc->target.label,
                                  curr_reloc->type, TRUE, FALSE );
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
