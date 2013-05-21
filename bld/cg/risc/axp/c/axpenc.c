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
* Description:  Alpha AXP instruction encoding.
*
****************************************************************************/


#include "cgstd.h"
#include "cgdefs.h"
#include "coderep.h"
#include "opcodes.h"
#include "pattern.h"
#include "vergen.h"
#include "procdef.h"
#include "ocentry.h"
#include "axpencod.h"
#include "reloc.h"
#include "zoiks.h"
#include "model.h"
#include "coff.h"
#include "encode.h"
#include "cgaux.h"
#include "rtclass.h"
#include "feprotos.h"
#include <assert.h>
#include <stdio.h>
#include "rtrtn.h"

extern void DumpInsOnly( instruction * );
extern void DumpString( char * );
extern void DumpPtr( pointer );
extern void DumpInt( int );
extern void DumpNL(void);
extern void DumpGen(struct opcode_entry*);
extern void DumpPtr( void *ptr );
extern void GenMEMINS( uint_8, uint_8, uint_8, signed_16 );

extern void             ObjBytes( char *buffer, int size );
extern uint_8           RegTrans( hw_reg_set );
extern void             OutLabel( label_handle );
extern void             OutReloc( label_handle, axp_reloc, unsigned );
extern hw_reg_set       StackReg( void );
extern hw_reg_set       FrameReg( void );
extern name             *DeAlias( name * );
extern void             TryScrapLabel( code_lbl * );
extern  offset          AskLocation();
extern  void            OutLineNum( unsigned_16 line, bool label_line );
extern void             EmitDbgInfo(instruction*);
extern  void            ObjEmitSeq( byte_seq * );
extern  void            InputOC( any_oc * );
extern  opcode_defs     FlipOpcode( opcode_defs );
extern  void            FactorInt32( signed_32 val, signed_16 *, signed_16 *, signed_16 * );

extern void GenMEMINS( uint_8 opcode, uint_8 a, uint_8 b, signed_16 displacement );

extern type_class_def   Unsigned[];
extern type_length      TypeClassSize[];
extern proc_def         *CurrProc;

#define _NameReg( op )                  ( (op)->r.arch_index )

#define _IsSigned( type )               ( Unsigned[ type ] != type )

#define _BinaryOpcode( a, b )           { { a, b }, { a, b } }
#define _SignedOpcode( a, b, c, d )     { { a, b }, { c, d } }

// Our table for opcode values is really a list of pairs of
// primary opcode / function code pairs. Their is two entries
// for each opcode in case the sign of the instruction matters;
// for example, for OP_RSHIFT we need to generate either sra or
// srl. If the sign of the type of the instruction doesn't
// matter, we can just use the _BinaryOpcode macro to create
// identical cases, otherwise we give each pair explicitly.


static  uint_8  BinaryOpcodes4[][2][2] = {
        _BinaryOpcode( 0x10, 0x00 ),                    /* OP_ADD */
        _BinaryOpcode( 0x10, 0x00 ),                    /* OP_EXT_ADD */
        _BinaryOpcode( 0x10, 0x09 ),                    /* OP_SUB */
        _BinaryOpcode( 0x10, 0x09 ),                    /* OP_EXT_SUB */
        _BinaryOpcode( 0x13, 0x00 ),                    /* OP_MUL */
        _BinaryOpcode( 0x13, 0x00 ),                    /* OP_EXT_MUL */
        _BinaryOpcode( 0x00, 0x00 ),                    /* OP_DIV */
        _BinaryOpcode( 0x00, 0x00 ),                    /* OP_MOD */
        _BinaryOpcode( 0x11, 0x00 ),                    /* OP_AND */
        _BinaryOpcode( 0x11, 0x20 ),                    /* OP_OR */
        _BinaryOpcode( 0x11, 0x40 ),                    /* OP_XOR */
        _SignedOpcode( 0x12, 0x34, 0x12, 0x3c ),        /* OP_RSHIFT */
        _BinaryOpcode( 0x12, 0x39 ),                    /* OP_LSHIFT */
        _BinaryOpcode( 0x00, 0x00 ),                    /* OP_POW */
        _BinaryOpcode( 0x00, 0x00 ),                    /* OP_ATAN2 */
        _BinaryOpcode( 0x00, 0x00 ),                    /* OP_FMOD */
};

static  uint_8  BinaryOpcodes8[][2][2] = {
        _BinaryOpcode( 0x10, 0x20 ),                    /* OP_ADD */
        _BinaryOpcode( 0x10, 0x20 ),                    /* OP_EXT_ADD */
        _BinaryOpcode( 0x10, 0x29 ),                    /* OP_SUB */
        _BinaryOpcode( 0x10, 0x29 ),                    /* OP_EXT_SUB */
        _BinaryOpcode( 0x13, 0x20 ),                    /* OP_MUL */
        _BinaryOpcode( 0x13, 0x20 ),                    /* OP_EXT_MUL */
        _BinaryOpcode( 0x00, 0x00 ),                    /* OP_DIV */
        _BinaryOpcode( 0x00, 0x00 ),                    /* OP_MOD */
        _BinaryOpcode( 0x11, 0x00 ),                    /* OP_AND */
        _BinaryOpcode( 0x11, 0x20 ),                    /* OP_OR */
        _BinaryOpcode( 0x11, 0x40 ),                    /* OP_XOR */
        _SignedOpcode( 0x12, 0x34, 0x12, 0x3c ),        /* OP_RSHIFT */
        _BinaryOpcode( 0x12, 0x39 ),                    /* OP_LSHIFT */
        _BinaryOpcode( 0x00, 0x00 ),                    /* OP_POW */
        _BinaryOpcode( 0x00, 0x00 ),                    /* OP_ATAN2 */
        _BinaryOpcode( 0x00, 0x00 ),                    /* OP_FMOD */
};

static  uint_8  SetOpcodes[][2][2] = {
        _BinaryOpcode( 0x10, 0x2d ),                    /* OP_SET_EQUAL */
        _BinaryOpcode( 0x00, 0x00 ),                    /* OP_SET_NOT_EQUAL */
        _BinaryOpcode( 0x00, 0x00 ),                    /* OP_SET_GREATER */
        _SignedOpcode( 0x10, 0x3d, 0x10, 0x6d ),        /* OP_SET_LESS_EQUAL */
        _SignedOpcode( 0x10, 0x1d, 0x10, 0x4d ),        /* OP_SET_LESS */
        _BinaryOpcode( 0x00, 0x00 ),                    /* OP_SET_GREATER_EQUAL */
};

// For floating point operate format instructions, the size of the
// instruction type (not the sign) is what determines the opcode. So
// we have a pair of function code values for each codegen logical
// opcode. We don't need the primary opcode since it is always 0x16.
// Note that we are in for some minor discomfort if we need to start
// enabling different trap bits on instructions.

static  uint_16 FloatingBinaryOpcodes[][2] = {
        { 0x080, 0x0a0 },                               /* OP_ADD */
        { 0x080, 0x0a0 },                               /* OP_EXT_ADD */
        { 0x581, 0x5a1 },                               /* OP_SUB */
        { 0x581, 0x5a1 },                               /* OP_EXT_SUB */
        { 0x582, 0x5a2 },                               /* OP_MUL */
        { 0x582, 0x5a2 },                               /* OP_EXT_MUL */
        { 0x583, 0x5a3 },                               /* OP_DIV */
};

static  uint_16 FloatingSetOpcodes[][1] = {
        { 0xa5 },                                       /* OP_SET_EQUAL */
        { 0x00 },                                       /* OP_SET_NOT_EQUAL */
        { 0x00 },                                       /* OP_SET_GREATER */
        { 0xa7 },                                       /* OP_SET_LESS_EQUAL */
        { 0xa6 },                                       /* OP_SET_LESS */
        { 0x00 },                                       /* OP_SET_GREATER_EQUAL */
};

static  uint_8  AlphaByteOpcodes[][2] = {
        { 0x12, 0x06 },                                 /* OP_EXTRACT_LOW */
        { 0x12, 0x4a },                                 /* OP_EXTRACT_HIGH */
        { 0x12, 0x0b },                                 /* OP_INSERT_LOW */
        { 0x12, 0x47 },                                 /* OP_INSERT_HIGH */
        { 0x12, 0x02 },                                 /* OP_MASK_LOW */
        { 0x12, 0x42 },                                 /* OP_MASK_HIGH */
        { 0x12, 0x30 },                                 /* OP_ZAP */
        { 0x12, 0x31 },                                 /* OP_ZAP_NOT */
};

// This is the bit pattern to OR into the function opcode when encoding
// an Alpha byte manipulation instruction. Note that it depends only upon
// the size of the register subset to be acted upon. Some of these
// combination can yield invalid instructions, but hopefully these will
// never be combined together.
static  uint_8  AlphaByteInsSizeBits[] = {
        0x00,                                           /* U1 */
        0x00,                                           /* I1 */
        0x10,                                           /* U2 */
        0x10,                                           /* I2 */
        0x20,                                           /* U4 */
        0x20,                                           /* I4 */
        0x30,                                           /* U8 */
        0x30,                                           /* I8 */
        0x20,                                           /* CP */
        0x20,                                           /* PT */
        0x20,                                           /* FS */
        0x30,                                           /* FD */
        0x30,                                           /* FL */
        0x00,                                           /* XX */
};

// Used for faking up a set of opcodes for an instruction when
// including an exhaustive table would have been too painful.
static  uint_8  ScratchOpcodes[2];

extern  void EmitInsReloc( axp_ins ins, pointer sym, owl_reloc_type type ) {
/**************************************************************************/

    oc_riscins          oc;

    oc.op.objlen = 4;
    oc.op.class = OC_RCODE;
    oc.op.reclen = sizeof( oc_riscins );
    oc.opcode = ins;
    oc.sym = sym;
    oc.reloc = type;
    InputOC( (any_oc *)&oc );
}

static  void EmitIns( axp_ins ins ) {
/***********************************/

    EmitInsReloc( ins, NULL, 0 );
}

static  void GenFPOPINS( uint_8 opcode, uint_16 function, uint_8 reg_a, uint_8 reg_b, uint_8 reg_c ) {
/****************************************************************************************************/

    axp_ins             ins;

    ins = _Opcode( opcode ) | _A( reg_a ) | _B( reg_b ) | _C( reg_c ) | _FPFunction( function );
    EmitIns( ins );
}

extern  void GenOPINS( uint_8 opcode, uint_8 function, uint_8 reg_a, uint_8 reg_b, uint_8 reg_c ) {
/*************************************************************************************************/

    axp_ins             ins;

    ins = _Opcode( opcode ) | _A( reg_a ) | _B( reg_b ) | _C( reg_c ) | _Function( function );
    EmitIns( ins );
}

static  void    GenOPIMM( uint_8 opcode, uint_8 function, uint_8 reg_a, uint_8 imm, uint_8 reg_c ) {
/**************************************************************************************************/

    axp_ins             ins;

    ins = _Opcode( opcode ) | _A( reg_a ) | _LIT( imm ) | ( 1 << 12 ) | _C( reg_c ) | _Function( function );
    EmitIns( ins );
}

extern  void    GenLOADS32( signed_32 value, uint_8 reg ) {
/*********************************************************/

    signed_16           high;
    signed_16           extra;
    signed_16           low;
    uint_8              curr;

    curr = AXP_ZERO_SINK;
    FactorInt32( value, &high, &extra, &low );
    if( low != 0 ) {
        // la low(curr) -> reg
        GenMEMINS( 0x08, reg, curr, low );
        curr = reg;
    }
    if( extra != 0 ) {
        // lah extra(curr) -> reg
        GenMEMINS( 0x09, reg, curr, extra );
        curr = reg;
    }
    if( high != 0 ) {
        // lah high(curr) -> reg
        GenMEMINS( 0x09, reg, curr, high );
        curr = reg;
    }
}

static  uint_8  *FindOpcodes( instruction *ins ) {
/************************************************/

    uint_8      *opcodes;

    if( _OpIsBinary( ins->head.opcode ) ) {
        if( ins->type_class == U8 || ins->type_class == I8 ) {
            opcodes = &BinaryOpcodes8[ ins->head.opcode - FIRST_BINARY_OP ][ _IsSigned( ins->type_class ) ][ 0 ];
        } else {
            opcodes = &BinaryOpcodes4[ ins->head.opcode - FIRST_BINARY_OP ][ _IsSigned( ins->type_class ) ][ 0 ];
        }
    } else if( _OpIsSet( ins->head.opcode ) ) {
        opcodes = &SetOpcodes[ ins->head.opcode - FIRST_SET_OP ][ _IsSigned( ins->type_class ) ][ 0 ];
    } else if( _OpIsAlphaByteIns( ins->head.opcode ) ) {
        // WARNING: must use these values before calling FindOpcodes again
        opcodes = &ScratchOpcodes[0];
        opcodes[ 0 ] = AlphaByteOpcodes[ ins->head.opcode - FIRST_ALPHA_BYTE_INS ][ 0 ];
        opcodes[ 1 ] = AlphaByteOpcodes[ ins->head.opcode - FIRST_ALPHA_BYTE_INS ][ 1 ];
        if( ins->head.opcode != OP_ZAP && ins->head.opcode != OP_ZAP_NOT ) {
            opcodes[ 1 ] |= AlphaByteInsSizeBits[ ins->base_type_class ];
        }
    } else {
        assert( 0 );
    }
    return( opcodes );
}

static  uint_16 FindFloatingOpcodes( instruction *ins ) {
/*******************************************************/

    uint_16     opcode;

    assert( _IsFloating( ins->type_class ) );
    if( _OpIsBinary( ins->head.opcode ) ) {
        opcode = FloatingBinaryOpcodes[ ins->head.opcode - FIRST_BINARY_OP ][ ins->type_class != FS ];
    } else if( _OpIsSet( ins->head.opcode ) ) {
        opcode = FloatingSetOpcodes[ ins->head.opcode - FIRST_SET_OP ][ 0 ];
    } else {
        assert( 0 );
    }
    return( opcode );
}

extern  void    GenMEMINSRELOC( uint_8 opcode, uint_8 a, uint_8 b, signed_16 displacement, pointer lbl, owl_reloc_type type ) {
/*****************************************************************************************************************************/

    axp_ins             encoding;

    encoding = _Opcode( opcode ) | _A( a ) | _B( b ) | _SignedImmed( displacement );
    EmitInsReloc( encoding, lbl, type );
}

extern  void    GenMEMINS( uint_8 opcode, uint_8 a, uint_8 b, signed_16 displacement ) {
/**************************************************************************************/
    axp_ins             encoding;

    encoding = _Opcode( opcode ) | _A( a ) | _B( b ) | _SignedImmed( displacement );
    EmitIns( encoding );
}

#if 0
static  uint_8  BranchOpcodes[][2] = {
    { 0x39, 0x31 },                     /* OP_CMP_EQUAL */
    { 0x3d, 0x35 },                     /* OP_CMP_NOT_EQUAL */
    { 0x3f, 0x37 },                     /* OP_CMP_GREATER */
    { 0x3b, 0x33 },                     /* OP_CMP_LESS_EQUAL */
    { 0x3a, 0x32 },                     /* OP_CMP_LESS */
    { 0x3e, 0x36 },                     /* OP_CMP_GREATER_EQUAL */
};
#endif

static  void    GenBRANCH( uint_8 opcode, uint_8 reg, pointer label ) {
/*********************************************************************/

    axp_ins             encoding;

    encoding = _Opcode( opcode ) | _A( reg );
    EmitInsReloc( encoding, label, OWL_RELOC_BRANCH_REL );
}

// move disp(Rs) -> Rd

extern  void    GenLOAD( hw_reg_set dst, hw_reg_set src, signed_16 displacement ) {
/*********************************************************************************/

    GenMEMINS( 0x29, RegTrans( dst ), RegTrans( src ), displacement );
}

extern  void    GenFLOAD( hw_reg_set dst, hw_reg_set src, signed_16 displacement ) {
/*********************************************************************************/

    GenMEMINS( 0x23, RegTrans( dst ), RegTrans( src ), displacement );
}

// move Rs -> disp(Rd)

extern  void    GenSTORE( hw_reg_set dst, signed_16 displacement, hw_reg_set src ) {
/**********************************************************************************/

    GenMEMINS( 0x2d, RegTrans( src ), RegTrans( dst ), displacement );
}

// move Fs -> disp(Rd)

extern  void    GenFSTORE( hw_reg_set dst, signed_16 displacement, hw_reg_set src ) {
/**********************************************************************************/

    GenMEMINS( 0x27, RegTrans( src ), RegTrans( dst ), displacement );
}

extern  void    GenRET( void )
/****************************/
{
    oc_ret      oc;

    oc.op.class = OC_RET;
    oc.op.reclen = sizeof( oc_ret );
    oc.op.objlen = 4;
    oc.pops = FALSE;            /* not used */
    InputOC( (any_oc *)&oc );
}

static  pointer symLabel( name *mem ) {
/*************************************/

    return( AskForSymLabel( mem->v.symbol, mem->m.memory_type ) );
}

static  uint_8  loadOpcodes[] = {
    0x28,                       /* U1 */
    0x28,                       /* I1 */
    0x28,                       /* U2 */
    0x28,                       /* I2 */
    0x28,                       /* U4 */
    0x28,                       /* I4 */
    0x29,                       /* U8 */
    0x29,                       /* I8 */
    0x28,                       /* CP */
    0x28,                       /* PT */
    0x22,                       /* FS */
    0x23,                       /* FD */
    0x23,                       /* FL */
};

static  uint_8  storeOpcodes[] = {
    0x00,                       /* U1 */
    0x00,                       /* I1 */
    0x00,                       /* U2 */
    0x00,                       /* I2 */
    0x2c,                       /* U4 */
    0x2c,                       /* I4 */
    0x2d,                       /* U8 */
    0x2d,                       /* I8 */
    0x2c,                       /* CP */
    0x2c,                       /* PT */
    0x26,                       /* FS */
    0x27,                       /* FD */
    0x27,                       /* FL */
};

extern  type_length     TempLocation( name *temp ) {
/**************************************************/

    name                *base;
    type_length         offset;

    assert( temp->n.class == N_TEMP );
    base = DeAlias( temp );
    if( base->t.location == NO_LOCATION ) {
        _Zoiks( ZOIKS_030 );
    }
    offset = CurrProc->targ.stack_map.locals.start;
    if( temp->t.temp_flags & STACK_PARM ) {
        offset = CurrProc->targ.frame_size;
    }
    return( offset + base->t.location + temp->v.offset - base->v.offset );
}

extern  void    GenCallLabelReg( pointer label, uint reg ) {
/**********************************************************/

    GenBRANCH( 0x34, reg, label );
}

extern  void    GenCallLabel( pointer label ) {
/*********************************************/

    CodeHandle( OC_CALL, 4, label );
}

static  void    doCall( instruction *ins ) {
/******************************************/

    pointer             sym;
    byte_seq            *code;
    code_lbl            *lbl;

    code = NULL;
    sym = ins->operands[ CALL_OP_ADDR ]->v.symbol;
    lbl = symLabel( ins->operands[ CALL_OP_ADDR ] );
    if( !AskIfRTLabel( lbl ) ) {
        code = FEAuxInfo( sym, CALL_BYTES );
    }
    if( code != NULL ) {
        ObjEmitSeq( code );
    } else {
        GenCallLabel( symLabel( ins->operands[ CALL_OP_ADDR ] ) );
    }
}

static  void    addressTemp( name *temp, uint_8 *reg, int_16 *offset ) {
/**********************************************************************/

    type_length         temp_offset;

    temp_offset = TempLocation( temp );
    if( temp_offset > AXP_MAX_OFFSET ) {
        // gen some code to load temp address into SCRATCH_REG
        GenLOADS32( temp_offset, AXP_GPR_SCRATCH );
        GenOPINS( 0x10, 0x00, AXP_STACK_REG, AXP_GPR_SCRATCH, AXP_GPR_SCRATCH );
        *offset = 0;
        *reg = AXP_GPR_SCRATCH;
    } else {
        *offset = temp_offset;
        *reg = AXP_STACK_REG;
        if( CurrProc->targ.base_is_fp ) {
            *reg = AXP_FRAME_REG;
        }
    }
}

static  void    getMemEncoding( name *mem, uint_8 *reg_index, int_16 *offset ) {
/******************************************************************************/

    switch( mem->n.class ) {
    case N_INDEXED:
        assert( mem->i.index->n.class == N_REGISTER );
        assert( mem->i.scale == 0 );
        assert( mem->i.constant == (type_length)((signed_16)mem->i.constant) );
        assert( ( mem->i.index_flags & X_LOW_ADDR_BASE ) == 0 );
        *reg_index = _NameReg( mem->i.index );
        *offset = (int_16)mem->i.constant;
        break;
    case N_TEMP:
        addressTemp( mem, reg_index, offset );
        break;
    case N_MEMORY:
    default:
        *reg_index = AXP_ZERO_SINK;
        *offset = 0;
        _Zoiks( ZOIKS_078 );
        break;
    }
}

static  void    doLoadStore( instruction *ins, bool load ) {
/**********************************************************/

    name        *mem;
    name        *reg;
    uint_8      opcode;
    uint_8      index;
    int_16      offset;

    if( load ) {
        mem = ins->operands[ 0 ];
        reg = ins->result;
        opcode = loadOpcodes[ _OpClass( ins ) ];
        if( ins->head.opcode == OP_LOAD_UNALIGNED ) {
            opcode = 0x0b;
        }
    } else {
        reg = ins->operands[ 0 ];
        mem = ins->result;
        opcode = storeOpcodes[ ins->type_class ];
        if( ins->head.opcode == OP_STORE_UNALIGNED ) {
            opcode = 0x0f;
        }
    }
    assert( reg->n.class == N_REGISTER );
    getMemEncoding( mem, &index, &offset );
    GenMEMINS( opcode, _NameReg( reg ), index, offset );
}

static uint_8 zapMask[] = {
    0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff
};

static  void    GenCallIndirect( instruction *call ) {
/****************************************************/

    uint_8      reg_index;
    uint_8      mem_index;
    int_16      mem_offset;
    name        *addr;

    reg_index = AXP_GPR_SCRATCH;        /* use the volatile scratch reg if possible */
    addr = call->operands[ CALL_OP_ADDR ];
    switch( addr->n.class ) {
    case N_REGISTER:
        reg_index = _NameReg( addr );
        break;
    case N_TEMP:
    case N_INDEXED:
        getMemEncoding( addr, &mem_index, &mem_offset );
        GenMEMINS( 0x28, reg_index, mem_index, mem_offset );
        break;
    }
    GenMEMINS( 0x1a, AXP_RETURN_ADDR, reg_index, 0x4000 );
}

static  void    doChop( instruction *ins, type_class_def class ) {
/****************************************************************/

    unsigned    size;
    unsigned    zap_mask;

    zap_mask = 0x00;
    size = TypeClassSize[ class ];
    switch( size ) {
    case 1:
        zap_mask = 0x01;
        break;
    case 2:
        zap_mask = 0x03;
        break;
    case 4:
        zap_mask = 0x0f;
        break;
    default:
        _Zoiks( ZOIKS_078 );
    }
    /* zapnot */
    GenOPIMM( 0x12, 0x31, _NameReg( ins->operands[ 0 ] ), zap_mask, _NameReg( ins->result ) );
}

static  void    doSignExtend( instruction *ins, type_class_def from ) {
/*********************************************************************/

    unsigned    from_size;
    int         res_index;
    int         src_index;
    int         shift_amt;

    res_index = _NameReg( ins->result );
    src_index = _NameReg( ins->operands[ 0 ] );
    from_size = TypeClassSize[ from ];
    if( from_size == 4 ) {
        /* addl r31, src -> dst */
        GenOPINS( 0x10, 0x00, AXP_ZERO_SINK, src_index, res_index );
    } else {
        shift_amt = ( REG_SIZE - from_size ) * 8;
        /* shl */
        GenOPIMM( 0x12, 0x39, src_index, shift_amt, res_index );
        /* sra */
        GenOPIMM( 0x12, 0x3c, res_index, shift_amt, res_index );
    }
}

#define RDTEB_ENCODING          0x000000ab
#define RDTEB_MAGIC_CONST       0x2c
#define V0                      0

static  bool    encodeThreadDataRef( instruction *ins ) {
/*******************************************************/

    name                *op;
    label_handle        tls_index;

    op = ins->operands[ 0 ];
    if( op->n.class != N_MEMORY ) return( FALSE );
    if( op->m.memory_type != CG_FE ) return( FALSE );
    if( ( FEAttr( op->v.symbol ) & FE_THREAD_DATA ) == 0 ) return( FALSE );

    /*
     * Put out a sequence that looks like:
                ldah    at, h^__tls_index(zero)
                lda     at, l^__tls_index(at)
                rdteb
                ldl     v0, 0x2c(v0)
                s4addl  at, v0, v0
                ldl     v0, (v0)
                lda     rn, l^variable(v0)

        In order to be able to zap v0 (which rdteb does),
        we always set the zap set on a LEA foo -> rn instruction
        to be v0 when foo is a piece of thread-local storage.
        This is done in FixMemRefs.
    */
    tls_index = RTLabel( RT_TLS_INDEX );
    GenMEMINSRELOC( 0x09, AXP_GPR_SCRATCH, AXP_ZERO_SINK, 0,
                tls_index, OWL_RELOC_HALF_HI );
    GenMEMINSRELOC( 0x08, AXP_GPR_SCRATCH, AXP_GPR_SCRATCH, 0,
                tls_index, OWL_RELOC_HALF_LO );
    EmitIns( RDTEB_ENCODING );
    GenMEMINS( loadOpcodes[ I4 ], V0, V0, RDTEB_MAGIC_CONST );
    GenOPINS( 0x0010, 0x0002, AXP_GPR_SCRATCH, V0, V0 );
    GenMEMINS( loadOpcodes[ I4 ], V0, V0, 0 );
    GenMEMINSRELOC( 0x08, _NameReg( ins->result ),
                V0, 0, symLabel( op ), OWL_RELOC_HALF_LO );
    return( TRUE );
}

static  void    Encode( instruction *ins ) {
/******************************************/

    uint_8              *opcodes;
    uint_16             function;
    uint_8              reg_index;
    uint_8              mem_index;
    int_16              mem_offset;
    signed_16           high;
    signed_16           extra;
    signed_16           low;

    switch( ins->u.gen_table->generate ) {
    case G_CALL:
        assert( ins->operands[ CALL_OP_ADDR ]->n.class == N_MEMORY );
        doCall( ins );
        break;
    case G_CALLI:
        GenCallIndirect( ins );
        break;
    case G_UNARY:
        assert( ins->operands[ 0 ]->n.class == N_REGISTER );
        assert( ins->result->n.class == N_REGISTER );
        switch( ins->head.opcode ) {
        case OP_NEGATE:
            /* NEG Ra => Rb  ...becomes... SUB R31,Ra => Rb */
            switch( ins->type_class ) {
            case FS:
                GenFPOPINS( 0x16, 0x81, AXP_ZERO_SINK,
                    _NameReg( ins->operands[ 0 ] ),
                    _NameReg( ins->result ) );
                break;
            case FD:
            case FL:
                GenFPOPINS( 0x16, 0xa1, AXP_ZERO_SINK,
                    _NameReg( ins->operands[ 0 ] ),
                    _NameReg( ins->result ) );
                break;
            default:
                GenOPINS( 0x10, 0x09, AXP_ZERO_SINK,
                    _NameReg( ins->operands[ 0 ] ),
                    _NameReg( ins->result ) );
            }
            break;
        case OP_COMPLEMENT:
            /* NOT Ra => Rb  ...becomes... ORNOT R31,Ra => Rb */
            GenOPINS( 0x11, 0x28, AXP_ZERO_SINK,
                _NameReg( ins->operands[ 0 ] ),
                _NameReg( ins->result ) );
            break;
        default:
            _Zoiks( ZOIKS_028 );
        }
        break;
    case G_MOVE_FP:
        /* CPYS r1,r1,r2 */
        GenFPOPINS( 0x17, 0x20,
                _NameReg( ins->operands[ 0 ] ), _NameReg( ins->operands[ 0 ] ),
                _NameReg( ins->result ) );
        break;
    case G_ZERO:
        assert( ins->operands[ 0 ]->n.class == N_REGISTER );
        assert( ins->result->n.class == N_REGISTER );
        assert( ins->head.opcode == OP_CONVERT );
        doChop( ins, ins->base_type_class );
        break;
    case G_SIGN:
        assert( ins->operands[ 0 ]->n.class == N_REGISTER );
        assert( ins->result->n.class == N_REGISTER );
        assert( ins->head.opcode == OP_CONVERT );
        doSignExtend( ins, ins->base_type_class );
        break;
    case G_CVTTS:
        assert( ins->operands[ 0 ]->n.class == N_REGISTER );
        assert( ins->result->n.class == N_REGISTER );
        GenFPOPINS( 0x16, 0xac,
                31, _NameReg( ins->operands[ 0 ] ),
                _NameReg( ins->result ) );
        break;
    case G_FREGTOMI8:
        assert( ins->operands[ 0 ]->n.class == N_REGISTER );
        assert( ins->result->n.class != N_REGISTER );
        reg_index = _NameReg( ins->operands[ 0 ] );
        GenFPOPINS( 0x16, 0x2f, AXP_ZERO_SINK, reg_index, AXP_FLOAT_SCRATCH );
        getMemEncoding( ins->result, &mem_index, &mem_offset );
        GenMEMINS( 0x27, AXP_FLOAT_SCRATCH, mem_index, mem_offset );
        break;
    case G_MI8TOFREG:
        assert( ins->operands[ 0 ]->n.class != N_REGISTER );
        assert( ins->result->n.class == N_REGISTER );
        reg_index = _NameReg( ins->result );
        getMemEncoding( ins->operands[ 0 ], &mem_index, &mem_offset );
        GenMEMINS( 0x23, reg_index, mem_index, mem_offset );
        GenFPOPINS( 0x16, 0xbe, AXP_ZERO_SINK, reg_index, reg_index );
        break;
    case G_BINARY_FP:
        assert( ins->operands[ 0 ]->n.class == N_REGISTER );
        assert( ins->operands[ 1 ]->n.class == N_REGISTER );
        assert( ins->result->n.class == N_REGISTER );
        function = FindFloatingOpcodes( ins );
        GenFPOPINS( 0x16, function,
                _NameReg( ins->operands[ 0 ] ), _NameReg( ins->operands[ 1 ] ),
                _NameReg( ins->result ) );
        break;
    case G_BINARY:
        assert( ins->operands[ 0 ]->n.class == N_REGISTER );
        assert( ins->operands[ 1 ]->n.class == N_REGISTER );
        assert( ins->result->n.class == N_REGISTER );
        opcodes = FindOpcodes( ins );
        GenOPINS( opcodes[ 0 ], opcodes[ 1 ],
                        _NameReg( ins->operands[ 0 ] ), _NameReg( ins->operands[ 1 ] ),
                        _NameReg( ins->result ) );
        break;
    case G_BINARY_IMM:
        assert( ins->operands[ 0 ]->n.class == N_REGISTER );
        assert( ins->operands[ 1 ]->n.class == N_CONSTANT );
        assert( ins->result->n.class == N_REGISTER );
        opcodes = FindOpcodes( ins );
        GenOPIMM( opcodes[ 0 ], opcodes[ 1 ],
                        _NameReg( ins->operands[ 0 ] ), ins->operands[ 1 ]->c.int_value,
                        _NameReg( ins->result ) );
        break;
    case G_BYTE_CONST:
        // generate an "add R31,byte -> Rn to do an unsigned 8-bit constant
        assert( ins->operands[ 0 ]->n.class == N_CONSTANT );
        assert( ins->result->n.class == N_REGISTER );
        GenOPIMM( 0x10, 0x00,
                        AXP_ZERO_SINK, (uint_8)ins->operands[ 0 ]->c.int_value,
                        _NameReg( ins->result ) );
        break;
    case G_MOVE:
        assert( ins->operands[ 0 ]->n.class == N_REGISTER );
        assert( ins->result->n.class == N_REGISTER );
        // generate a "BIS R31,Rn,Rm" instruction
        GenOPINS( 0x11, 0x20,
                        AXP_ZERO_SINK, _NameReg( ins->operands[ 0 ] ),
                        _NameReg( ins->result ) );
        break;
    case G_ZAP:
        assert( ins->operands[ 0 ]->n.class == N_REGISTER );
        assert( ins->result->n.class == N_REGISTER );
        assert( ins->flags.zap_value <= 8 );
        // generate a "ZAPNOT Ra,#i,Rb" instruction
        GenOPIMM( 0x12, 0x31,
                        _NameReg( ins->operands[ 0 ] ),
                        zapMask[ ins->flags.zap_value ],
                        _NameReg( ins->result ) );
        break;
    case G_LEA_HIGH:
        assert( ins->operands[ 0 ]->n.class == N_CONSTANT );
        assert( ins->operands[ 0 ]->c.const_type == CONS_HIGH_ADDR );
        assert( ins->result->n.class == N_REGISTER );
        GenMEMINS( 0x09, _NameReg( ins->result ), AXP_ZERO_SINK, ins->operands[ 0 ]->c.int_value & 0xffff );
        break;
    case G_LEA:
        assert( ins->operands[ 0 ]->n.class == N_CONSTANT );
        assert( ins->result->n.class == N_REGISTER );
        switch( ins->operands[ 0 ]->c.const_type ) {
        case CONS_ABSOLUTE:
            GenMEMINS( 0x08, _NameReg( ins->result ), AXP_ZERO_SINK, ins->operands[ 0 ]->c.int_value );
            break;
        case CONS_LOW_ADDR:
        case CONS_HIGH_ADDR:
        case CONS_OFFSET:
        case CONS_ADDRESS:
            _Zoiks( ZOIKS_028 );
            break;
        }
        break;
    case G_LOAD_ADDR:
        switch( ins->operands[ 0 ]->n.class ) {
        case N_INDEXED:
        case N_TEMP:
            assert( ins->result->n.class == N_REGISTER );
            getMemEncoding( ins->operands[ 0 ], &mem_index, &mem_offset );
            GenMEMINS( 0x08, _NameReg( ins->result ), mem_index, mem_offset );
            break;
        case N_MEMORY:
            assert( ins->result->n.class == N_REGISTER );
            FactorInt32( ins->operands[ 0 ]->v.offset, &high, &extra, &low );
            if( extra != 0 ) {
                _Zoiks( ZOIKS_132 );
            }
            if( !encodeThreadDataRef( ins ) ) {
                GenMEMINSRELOC( 0x09, _NameReg( ins->result ), AXP_ZERO_SINK, high,
                            symLabel( ins->operands[ 0 ] ), OWL_RELOC_HALF_HI );
                GenMEMINSRELOC( 0x08, _NameReg( ins->result ), _NameReg( ins->result ), low,
                            symLabel( ins->operands[ 0 ] ), OWL_RELOC_HALF_LO );
            }
            break;
        default:
            _Zoiks( ZOIKS_078 );
        }
        break;
    case G_LDQ_U:
    case G_LOAD:
        doLoadStore( ins, TRUE );
        break;
    case G_STQ_U:
    case G_STORE:
        doLoadStore( ins, FALSE );
        break;
    case G_CVTTQ:
        _Zoiks( ZOIKS_028 );
        break;
    case G_DEBUG:
        EmitDbgInfo( ins );
        break;
    case G_CONDBR:
        GenCondJump( ins );
        break;
    case G_NO:
        return;
    case G_UNKNOWN:
        _Zoiks( ZOIKS_097 );
        break;
    default:
        _Zoiks( ZOIKS_028 );
    }
#ifndef NDEBUG
    if( _IsTargetModel( ASM_OUTPUT ) ) {
        DumpString( "        " );
        DumpGen( ins->u.gen_table );
        DumpString( " - " );
        DumpInsOnly( ins );
        DumpNL();
    }
#endif
}

extern  void    GenObjCode( instruction *ins )  {
/********************************************/

    _AlignmentCheck( ins, 8 );
    Encode( ins );
}

#if 0
extern  void    GenJumpIf( instruction *ins, pointer label ) {
/************************************************************/

    GenBRANCH( BranchOpcodes[ ins->head.opcode - FIRST_COMPARISON ][ _IsFloating( ins->type_class ) ],
                _NameReg( ins->operands[ 0 ] ), label );
#ifndef NDEBUG
    if( _IsTargetModel( ASM_OUTPUT ) ) {
        DumpString( "Jcc L" );
        DumpPtr( label );
        DumpNL();
    }
#endif
}
#endif

extern  void    GenLabelReturn() {
/********************************/

    GenRET();
}


extern  byte    ReverseCondition( byte cond ) {
/*********************************************/

    return( FlipOpcode( cond ) );
}

extern  byte    CondCode( instruction *ins ) {
/********************************************/

    return( ins->head.opcode );
}
