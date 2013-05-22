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
* Description:  MIPS instruction encoding.
*
****************************************************************************/


#include "cgstd.h"
#include "cgdefs.h"
#include "coderep.h"
#include "pattern.h"
#include "vergen.h"
#include "ocentry.h"
#include "mipsenc.h"
#include "reloc.h"
#include "zoiks.h"
#include "coff.h"
#include "encode.h"
#include "cgaux.h"
#include "data.h"
#include "feprotos.h"
#include <assert.h>
#include <stdio.h>
#include "rtrtn.h"

extern void DumpInsOnly( instruction * );
extern void DumpString( char * );
extern void DumpPtr( pointer );
extern void DumpInt( int );
extern void DumpNL( void );
extern void DumpGen( struct opcode_entry * );
extern void DumpPtr( void *ptr );

extern void             ObjBytes( char *buffer, int size );
extern uint_8           RegTrans( hw_reg_set );
extern name             *DeAlias( name * );
extern void             TryScrapLabel( code_lbl * );
extern void             EmitDbgInfo( instruction * );
extern void             ObjEmitSeq( byte_seq * );
extern void             InputOC( any_oc * );
extern opcode_defs      FlipOpcode( opcode_defs );
extern void             FactorInt32( signed_32 val, signed_16 *, signed_16 *, signed_16 * );
extern  void            GenIType( uint_8 opcode, uint_8 rt, uint_8 rs, signed_16 immed );

extern type_class_def   Unsigned[];

#define _NameReg( op )                  ( (op)->r.arch_index )

#define _IsSigned( type )               ( Unsigned[type] != type )

#define _BinaryOpcode( a, b )           { { a, b }, { a, b } }
#define _SignedOpcode( a, b, c, d )     { { a, b }, { c, d } }
#define _BinaryImmOpcode( a )           { a, a }
#define _SignedImmOpcode( a, b )        { a, b }

// Our table for opcode values is really a list of pairs of
// primary opcode / function code pairs. There are two entries
// for each opcode in case the sign of the instruction matters;
// for example, for OP_RSHIFT we need to generate either srav or
// srlv. If the sign of the type of the instruction doesn't
// matter, we can just use the _BinaryOpcode macro to create
// identical cases, otherwise we give each pair explicitly.

static  uint_8  BinaryOpcodes4[][2][2] = {
        _BinaryOpcode( 0x00, 0x21 ),                    /* OP_ADD */
        _BinaryOpcode( 0x00, 0x21 ),                    /* OP_EXT_ADD */
        _BinaryOpcode( 0x00, 0x23 ),                    /* OP_SUB */
        _BinaryOpcode( 0x00, 0x23 ),                    /* OP_EXT_SUB */
        _SignedOpcode( 0x00, 0x19, 0x00, 0x18 ),        /* OP_MUL */
        _SignedOpcode( 0x00, 0x19, 0x00, 0x18 ),        /* OP_EXT_MUL */
        _SignedOpcode( 0x00, 0x1b, 0x00, 0x1a ),        /* OP_DIV */
        _SignedOpcode( 0x00, 0x1b, 0x00, 0x1a ),        /* OP_MOD */
        _BinaryOpcode( 0x00, 0x24 ),                    /* OP_AND */
        _BinaryOpcode( 0x00, 0x25 ),                    /* OP_OR */
        _BinaryOpcode( 0x00, 0x26 ),                    /* OP_XOR */
        _SignedOpcode( 0x00, 0x06, 0x00, 0x07 ),        /* OP_RSHIFT */
        _BinaryOpcode( 0x00, 0x04 ),                    /* OP_LSHIFT */
        _BinaryOpcode( 0x00, 0x00 ),                    /* OP_POW */
        _BinaryOpcode( 0x00, 0x00 ),                    /* OP_ATAN2 */
        _BinaryOpcode( 0x00, 0x00 ),                    /* OP_FMOD */
};

static  uint_8  BinaryOpcodes8[][2][2] = {
        _BinaryOpcode( 0x00, 0x21 ),                    /* OP_ADD */
        _BinaryOpcode( 0x00, 0x21 ),                    /* OP_EXT_ADD */
        _BinaryOpcode( 0x00, 0x23 ),                    /* OP_SUB */
        _BinaryOpcode( 0x00, 0x23 ),                    /* OP_EXT_SUB */
        _SignedOpcode( 0x00, 0x19, 0x00, 0x18 ),        /* OP_MUL */
        _SignedOpcode( 0x00, 0x19, 0x00, 0x18 ),        /* OP_EXT_MUL */
        _BinaryOpcode( 0x00, 0x00 ),                    /* OP_DIV */
        _BinaryOpcode( 0x00, 0x00 ),                    /* OP_MOD */
        _BinaryOpcode( 0x00, 0x24 ),                    /* OP_AND */
        _BinaryOpcode( 0x00, 0x25 ),                    /* OP_OR */
        _BinaryOpcode( 0x00, 0x26 ),                    /* OP_XOR */
        _SignedOpcode( 0x00, 0x06, 0x00, 0x07 ),        /* OP_RSHIFT */
        _BinaryOpcode( 0x00, 0x04 ),                    /* OP_LSHIFT */
        _BinaryOpcode( 0x00, 0x00 ),                    /* OP_POW */
        _BinaryOpcode( 0x00, 0x00 ),                    /* OP_ATAN2 */
        _BinaryOpcode( 0x00, 0x00 ),                    /* OP_FMOD */
};

/* MIPS only has slt/sltu - every other operation needs to be
 * reduced to something else.
 */
static  uint_8  SetOpcodes[][2][2] = {
        _BinaryOpcode( 0x00, 0x00 ),                    /* OP_SET_EQUAL */
        _BinaryOpcode( 0x00, 0x00 ),                    /* OP_SET_NOT_EQUAL */
        _BinaryOpcode( 0x00, 0x00 ),                    /* OP_SET_GREATER */
        _BinaryOpcode( 0x00, 0x00 ),                    /* OP_SET_LESS_EQUAL */
        _SignedOpcode( 0x00, 0x2b, 0x00, 0x2a ),        /* OP_SET_LESS */
        _BinaryOpcode( 0x00, 0x00 ),                    /* OP_SET_GREATER_EQUAL */
};

static  uint_8  BinaryImmedOpcodes[] = {
        0x09,                                           /* OP_ADD */
        0x09,                                           /* OP_EXT_ADD */
        0x09,                                           /* OP_SUB */
        0x09,                                           /* OP_EXT_SUB */
        0,                                              /* OP_MUL */
        0,                                              /* OP_EXT_MUL */
        0,                                              /* OP_DIV */
        0,                                              /* OP_MOD */
        0x0c,                                           /* OP_AND */
        0x0d,                                           /* OP_OR */
        0x0e,                                           /* OP_XOR */
        0,                                              /* OP_RSHIFT */
        0,                                              /* OP_LSHIFT */
        0,                                              /* OP_POW */
        0,                                              /* OP_ATAN2 */
        0,                                              /* OP_FMOD */
};

/* Note - 'reg SET_LESS_EQUAL imm' will be converted to 'reg SET_LESS (imm + 1)'
 * inside FindImmedOpcode. That's why the opcodes are the same as for SET_LESS.
 */
static  uint_8  SetImmedOpcodes[][2] = {
        _BinaryImmOpcode( 0x00 ),                       /* OP_SET_EQUAL */
        _BinaryImmOpcode( 0x00 ),                       /* OP_SET_NOT_EQUAL */
        _BinaryImmOpcode( 0x00 ),                       /* OP_SET_GREATER */
        _SignedImmOpcode( 0x0b, 0x0a ),                 /* OP_SET_LESS_EQUAL */
        _SignedImmOpcode( 0x0b, 0x0a ),                 /* OP_SET_LESS */
        _BinaryImmOpcode( 0x00 ),                       /* OP_SET_GREATER_EQUAL */
};

// For floating point operate format instructions. We don't need to store
// the primary opcode here since it is always 0x11 (ie. COP1) and we don't
// need the format type since it can be derived from the operand size easily
// enough. Function code is all we need here.
// Note that we are in for some minor discomfort if we need to start
// enabling different trap bits on instructions.

static  uint_8 FloatingBinaryOpcodes[] = {
        0x00,                                       /* OP_ADD */
        0x00,                                       /* OP_EXT_ADD */
        0x01,                                       /* OP_SUB */
        0x01,                                       /* OP_EXT_SUB */
        0x02,                                       /* OP_MUL */
        0x02,                                       /* OP_EXT_MUL */
        0x03,                                       /* OP_DIV */
};

static  uint_8 FloatingSetOpcodes[] = {
        0x32,                                       /* OP_SET_EQUAL */
        0x32,                                       /* OP_SET_NOT_EQUAL */
        0x36,                                       /* OP_SET_GREATER */
        0x36,                                       /* OP_SET_LESS_EQUAL */
        0x34,                                       /* OP_SET_LESS */
        0x34,                                       /* OP_SET_GREATER_EQUAL */
};


extern  void EmitInsReloc( mips_ins ins, pointer sym, owl_reloc_type type )
/*************************************************************************/
{
    oc_riscins          oc;

    oc.op.objlen = 4;
    oc.op.class = OC_RCODE;
    oc.op.reclen = sizeof( oc_riscins );
    oc.opcode = ins;
    oc.sym = sym;
    oc.reloc = type;
    InputOC( (any_oc *)&oc );
}


static  void EmitIns( mips_ins ins )
/**********************************/
{
    EmitInsReloc( ins, NULL, 0 );
}


extern  void GenLOADS32( signed_32 value, uint_8 reg )
/*****************************************************
 * Load a signed 32-bit constant 'value' into register 'reg'
 */
{
    if( (value < 32768) && (value > -32769) ) {
        // Only need sign extended low 16 bits - 'addiu rt,$zero,value'
        GenIType( 0x09, reg, MIPS_ZERO_SINK, (unsigned_16)value );
    } else if( (value & 0xffff) == 0 ) {
        // Only need high 16 bits - 'lui rt,$zero,(value >> 16)'
        GenIType( 0x0f, reg, MIPS_ZERO_SINK, (unsigned_16)(value >> 16) );
    } else {
        // Need two instructions - 'lui rt,$zero,(value >> 16)'
        GenIType( 0x0f, reg, MIPS_ZERO_SINK, (unsigned_16)(value >> 16) );
        // followed by 'ori rt,$zero,(value & 0xffff)'
        GenIType( 0x0d, reg, MIPS_ZERO_SINK, (unsigned_16)value );
    }
}


static  uint_8  *FindOpcodes( instruction *ins )
/***********************************************
 * Look up the opcodes for a binary operator
 */
{
    uint_8      *opcodes;

    if( _OpIsBinary( ins->head.opcode ) ) {
        if( ins->type_class == U8 || ins->type_class == I8 ) {
            opcodes = &BinaryOpcodes8[ins->head.opcode - FIRST_BINARY_OP][_IsSigned( ins->type_class )][0];
        } else {
            opcodes = &BinaryOpcodes4[ins->head.opcode - FIRST_BINARY_OP][_IsSigned( ins->type_class )][0];
        }
    } else if( _OpIsSet( ins->head.opcode ) ) {
        opcodes = &SetOpcodes[ins->head.opcode - FIRST_SET_OP][_IsSigned( ins->type_class )][0];
    } else {
        assert( 0 );
    }
    assert( opcodes[0] || opcodes[1] );
    return( opcodes );
}


static  uint_8 FindImmedOpcode( instruction *ins )
/*************************************************
 * Look up the opcode for a binary operator
 * where right hand operand is an immediate
 */
{
    uint_8      opcode;

    if( _OpIsBinary( ins->head.opcode ) ) {
        opcode = BinaryImmedOpcodes[ins->head.opcode - FIRST_BINARY_OP];
    } else if( _OpIsSet( ins->head.opcode ) ) {
        opcode = SetImmedOpcodes[ins->head.opcode - FIRST_SET_OP][_IsSigned( ins->type_class )];
        if( ins->head.opcode == OP_SET_LESS_EQUAL ) {
            // need to increment the immediate by one (since CPU can do
            // 'less than' but not 'less than or equal')
            ins->operands[1]->c.int_value++;
            assert( ins->operands[1]->c.int_value <= MIPS_MAX_OFFSET );
        }
    } else {
        assert( 0 );
    }
    assert( opcode );
    return( opcode );
}


static  uint_8 FindFloatingOpcodes( instruction *ins )
/*****************************************************
 * Look up the opcodes for a binary operator with
 * floating point operands
 */
{
    uint_8      opcode;

    assert( _IsFloating( ins->type_class ) );
    if( _OpIsBinary( ins->head.opcode ) ) {
        opcode = FloatingBinaryOpcodes[ins->head.opcode - FIRST_BINARY_OP];
        /* NB: this opcode may legitimately be zero - that's 'add' */
    } else if( _OpIsSet( ins->head.opcode ) ) {
        opcode = FloatingSetOpcodes[ins->head.opcode - FIRST_SET_OP];
        assert( opcode );
    } else {
        assert( 0 );
    }
    return( opcode );
}


extern  void GenMEMINSRELOC( uint_8 opcode, uint_8 rt, uint_8 rs, signed_16 displacement, pointer lbl, owl_reloc_type type )
/**************************************************************************************************************************/
{
    mips_ins            encoding;

    encoding = _Opcode( opcode ) | _Rt( rt ) | _Rs( rs ) | _SignedImmed( displacement );
    EmitInsReloc( encoding, lbl, type );
}


extern  void GenMEMINS( uint_8 opcode, uint_8 a, uint_8 b, signed_16 displacement )
/*********************************************************************************/
{
    mips_ins            encoding;

    encoding = _Opcode( opcode ) | _Rt( a ) | _Rs( b ) | _SignedImmed( displacement );
    EmitIns( encoding );
}


extern  void GenIType( uint_8 opcode, uint_8 rt, uint_8 rs, signed_16 immed )
/***************************************************************************/
{
    mips_ins            encoding;

    encoding = _Opcode( opcode ) | _Rs( rs ) | _Rt( rt ) | _Immed( immed );
    EmitIns( encoding );
}


extern  void GenRType( uint_8 opcode, uint_8 fc, uint_8 rd, uint_8 rs, uint_8 rt )
/********************************************************************************/
{
    mips_ins            encoding;

    encoding = _Opcode( opcode ) | _Rs( rs ) | _Rt( rt ) | _Rd( rd ) | _Function( fc );
    EmitIns( encoding );
}

extern  void GenIShift( uint_8 fc, uint_8 rd, uint_8 rt, uint_8 sa )
/******************************************************************/
{
    mips_ins            encoding;

    encoding = _Opcode( 0 ) | _Rs( 0 ) | _Rt( rt ) | _Rd( rd ) | _Shift( sa ) | _Function( fc );
    EmitIns( encoding );
}

extern  void GenJType( uint_8 opcode, pointer label )
/***************************************************/
{
    mips_ins            encoding;

    encoding = _Opcode( opcode );
    EmitInsReloc( encoding, label, OWL_RELOC_JUMP_ABS );
    // TODO: Handle delay slot better
    EmitIns( MIPS_NOP );
}


static  void GenFloatRType( type_class_def type, uint_8 fnc, uint_8 fd, uint_8 fs, uint_8 ft )
/********************************************************************************************/
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


extern  void GenRET( void )
/*************************/
{
    oc_ret      oc;

    oc.op.class = OC_RET;
    oc.op.reclen = sizeof( oc_ret );
    oc.op.objlen = 4;
    oc.pops = FALSE;            /* not used */
    InputOC( (any_oc *)&oc );
}


static  pointer symLabel( name *mem )
/***********************************/
{
    return( AskForSymLabel( mem->v.symbol, mem->m.memory_type ) );
}

static  uint_8  loadOpcodes[] = {
    0x24,                       /* U1 */
    0x20,                       /* I1 */
    0x25,                       /* U2 */
    0x21,                       /* I2 */
    0x23,                       /* U4 */    // 0x27 for MIPS64
    0x23,                       /* I4 */
    0x37,                       /* U8 */
    0x37,                       /* I8 */
    0x23,                       /* CP */
    0x23,                       /* PT */
    0x31,                       /* FS */
    0x35,                       /* FD */
    0x35,                       /* FL */
};

static  uint_8  storeOpcodes[] = {
    0x28,                       /* U1 */
    0x28,                       /* I1 */
    0x29,                       /* U2 */
    0x29,                       /* I2 */
    0x2b,                       /* U4 */
    0x2b,                       /* I4 */
    0x3f,                       /* U8 */
    0x3f,                       /* I8 */
    0x2b,                       /* CP */
    0x2b,                       /* PT */
    0x39,                       /* FS */
    0x3d,                       /* FD */
    0x3d,                       /* FL */
};

extern  type_length TempLocation( name *temp )
/********************************************/
{
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


extern  void GenCallLabelReg( pointer label, uint reg )
/*****************************************************/
{
    // This is used for calling into certain cg support routines. We'd
    // kinda like to use 'jal', except we must use something other
    // than ra for the return address. So 'jalr' it is...

    // Load address into $at (lui/addiu)
    // TODO: This should be different for PIC
    GenMEMINSRELOC( 0x0f, MIPS_GPR_SCRATCH, MIPS_ZERO_SINK, 0,
                label, OWL_RELOC_HALF_HI );
    GenMEMINSRELOC( 0x09, MIPS_GPR_SCRATCH, MIPS_GPR_SCRATCH, 0,
                label, OWL_RELOC_HALF_LO );

    // 'jalr reg,$at'
    GenRType( 0x00, 0x09, reg, MIPS_GPR_SCRATCH, 0 );
    // WARNING! WARNING! WARNING!
    // There's no delay slot here. Caller must handle that.
}


extern  void GenCallLabel( pointer label )
/****************************************/
{
    CodeHandle( OC_CALL, 4, label );
}


static  void doCall( instruction *ins )
/*************************************/
{
    pointer             sym;
    byte_seq            *code;
    code_lbl            *lbl;

    code = NULL;
    sym = ins->operands[CALL_OP_ADDR]->v.symbol;
    lbl = symLabel( ins->operands[CALL_OP_ADDR] );
    if( !AskIfRTLabel( lbl ) ) {
        code = FEAuxInfo( sym, CALL_BYTES );
    }
    if( code != NULL ) {
        ObjEmitSeq( code );
    } else {
        GenCallLabel( symLabel( ins->operands[CALL_OP_ADDR] ) );
    }
}


static  void addressTemp( name *temp, uint_8 *reg, int_16 *offset )
/*****************************************************************/
{
    type_length         temp_offset;

    temp_offset = TempLocation( temp );
    if( temp_offset > MIPS_MAX_OFFSET ) {
        // gen some code to load temp address into SCRATCH_REG
        GenLOADS32( temp_offset, MIPS_GPR_SCRATCH );
        // 'or $sp,$at,$zero', aka 'move $sp,$at'
        GenRType( 0x00, 0x25, MIPS_STACK_REG, MIPS_GPR_SCRATCH, MIPS_ZERO_SINK );
        *offset = 0;
        *reg = MIPS_GPR_SCRATCH;
    } else {
        *offset = temp_offset;
        *reg = MIPS_STACK_REG;
        if( CurrProc->targ.base_is_fp ) {
            *reg = MIPS_FRAME_REG;
        }
    }
}


static  void getMemEncoding( name *mem, uint_8 *reg_index, int_16 *offset )
/*************************************************************************/
{
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
        *reg_index = MIPS_ZERO_SINK;
        *offset = 0;
        _Zoiks( ZOIKS_078 );
        break;
    }
}


static  void doLoadStore( instruction *ins, bool load )
/*****************************************************/
{
    name        *mem;
    name        *reg;
    uint_8      opcode;
    uint_8      index;
    int_16      offset;

    if( load ) {
        mem = ins->operands[0];
        reg = ins->result;
        opcode = loadOpcodes[_OpClass( ins )];
    } else {
        reg = ins->operands[0];
        mem = ins->result;
        opcode = storeOpcodes[ins->type_class];
    }
    assert( reg->n.class == N_REGISTER );
    getMemEncoding( mem, &index, &offset );
    GenMEMINS( opcode, _NameReg( reg ), index, offset );
}


static  void doLoadStoreUnaligned( instruction *ins, bool load )
/**************************************************************/
{
#if 0
    name        *mem;
    name        *reg;
    uint_8      opcode1;
    uint_8      opcode2;
    uint_8      index;
    int_16      offset;

    if( load ) {
        mem = ins->operands[0];
        reg = ins->result;
        // 'lwl', 'lwr'
        opcode1 = 0x22; opcode2 = 0x26;
    } else {
        reg = ins->operands[0];
        mem = ins->result;
        // 'swl', 'swr'
        opcode1 = 0x2a; opcode2 = 0x2e;
    }
    assert( reg->n.class == N_REGISTER );
    getMemEncoding( mem, &index, &offset );
    // TODO: make sure offset can't overflow
    GenMEMINS( opcode1, _NameReg( reg ), index, offset + 3 );
    GenMEMINS( opcode2, _NameReg( reg ), index, offset );
#else
    doLoadStore( ins, load );
#endif
}


static  void GenCallIndirect( instruction *call )
/***********************************************/
{
    uint_8      reg_index;
    uint_8      mem_index;
    int_16      mem_offset;
    name        *addr;

    reg_index = MIPS_GPR_SCRATCH;   /* use the volatile scratch reg if possible */
    addr = call->operands[CALL_OP_ADDR];
    switch( addr->n.class ) {
    case N_REGISTER:
        reg_index = _NameReg( addr );
        break;
    case N_TEMP:
    case N_INDEXED:
        getMemEncoding( addr, &mem_index, &mem_offset );
        GenMEMINS( 0x23, reg_index, mem_index, mem_offset );
        break;
    }
    // 'jalr ra,reg_index'
    GenRType( 0x00, 0x09, MIPS_RETURN_ADDR, reg_index, 0 );
    // TODO: Handle delay slot better
    EmitIns( MIPS_NOP );
}


static  void doZero( instruction *ins, type_class_def class )
/***********************************************************/
{
    unsigned    size;

    size = TypeClassSize[class];
    switch( size ) {
    case 1:
        // 'andi res,op1,0x00ff'
        GenIType( 0x0c, _NameReg( ins->result ), _NameReg( ins->operands[0] ), 0x00ff );
        break;
    case 2:
        // 'andi res,op1,0xffff'
        GenIType( 0x0c, _NameReg( ins->result ), _NameReg( ins->operands[0] ), 0x0ffff );
        break;
    default:
        _Zoiks( ZOIKS_091 );
    }
}


static  void doSignExtend( instruction *ins, type_class_def from )
/****************************************************************/
{
    unsigned    from_size;
    int         res_index;
    int         src_index;
    int         shift_amt;

    res_index = _NameReg( ins->result );
    src_index = _NameReg( ins->operands[0] );
    from_size = TypeClassSize[from];
    if( from_size == 4 ) {
        // 'addu rd,$zero,rs' - MIPS64 only?
        GenRType( 0x00, 0x21, res_index, MIPS_ZERO_SINK, src_index );
    } else {
        // MIPS32 ISA Release 2 has 'seb'/'seh' instructions for this
        shift_amt = (REG_SIZE - from_size) * 8;
        // 'sll rd,rs,n'
        GenIShift( 0x00, res_index, src_index, shift_amt );
        // 'sra rd,rs,n'
        GenIShift( 0x03, res_index, res_index, shift_amt );
    }
}

// This is NT stuff - probably irreleveant unless someone wanted to
// support the MIPS version of NT!
#define RDTEB_ENCODING          0x000000ab
#define RDTEB_MAGIC_CONST       0x2c
#define V0                      0

static  bool    encodeThreadDataRef( instruction *ins )
/*****************************************************/
{
    name                *op;
    label_handle        tls_index;

    op = ins->operands[0];
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
#if 0
    GenMEMINSRELOC( 0x09, MIPS_GPR_SCRATCH, MIPS_ZERO_SINK, 0,
                tls_index, OWL_RELOC_HALF_HI );
    GenMEMINSRELOC( 0x08, MIPS_GPR_SCRATCH, MIPS_GPR_SCRATCH, 0,
                tls_index, OWL_RELOC_HALF_LO );
    EmitIns( RDTEB_ENCODING );
    GenMEMINS( loadOpcodes[I4], V0, V0, RDTEB_MAGIC_CONST );
    GenOPINS( 0x0010, 0x0002, MIPS_GPR_SCRATCH, V0, V0 );
    GenMEMINS( loadOpcodes[I4], V0, V0, 0 );
    GenMEMINSRELOC( 0x08, _NameReg( ins->result ),
                V0, 0, symLabel( op ), OWL_RELOC_HALF_LO );
#else
    assert( 0 );
#endif
    return( TRUE );
}


static  void Encode( instruction *ins )
/*************************************/
{
    uint_8              *opcodes;
    uint_8              opcode;
    uint_16             function;
    uint_8              reg_index;
    uint_8              mem_index;
    int_16              mem_offset;
    signed_16           high;
    signed_16           extra;
    signed_16           low;
    signed_16           imm_value;

    switch( ins->u.gen_table->generate ) {
    case G_CALL:
        assert( ins->operands[CALL_OP_ADDR]->n.class == N_MEMORY );
        doCall( ins );
        break;
    case G_CALLI:
        GenCallIndirect( ins );
        break;
    case G_UNARY:
        assert( ins->operands[0]->n.class == N_REGISTER );
        assert( ins->result->n.class == N_REGISTER );
        switch( ins->head.opcode ) {
        case OP_NEGATE:
            // 'neg.fmt fd,fs'
            switch( ins->type_class ) {
            case FS:
            case FD:
            case FL:
                GenFloatRType( ins->type_class, 0x07, _NameReg( ins->result ),
                        _NameReg( ins->operands[0] ), 0 );
                break;
            default:
                // 'subu rd,$zero,rs'
                GenRType( 0x00, 0x23, _NameReg( ins->result ),
                          MIPS_ZERO_SINK, _NameReg( ins->operands[0] ) );
            }
            break;
        case OP_COMPLEMENT:
            // 'nor rd,$zero,rs'
            GenRType( 0x00, 0x27, _NameReg( ins->result ),
                      MIPS_ZERO_SINK, _NameReg( ins->operands[0] ) );
            break;
        default:
            _Zoiks( ZOIKS_028 );
        }
        break;
    case G_MOVE_FP:
        assert( ins->operands[0]->n.class == N_REGISTER );
        assert( ins->result->n.class == N_REGISTER );
        // 'mov.s fd,fs'
        GenFloatRType( FS, 0x06, _NameReg( ins->result ),
                        _NameReg( ins->operands[0] ), 0 );
        break;
    case G_ZERO:
        assert( ins->operands[0]->n.class == N_REGISTER );
        assert( ins->result->n.class == N_REGISTER );
        assert( ins->head.opcode == OP_CONVERT );
        doZero( ins, ins->base_type_class );
        break;
    case G_SIGN:
        assert( ins->operands[0]->n.class == N_REGISTER );
        assert( ins->result->n.class == N_REGISTER );
        assert( ins->head.opcode == OP_CONVERT );
        doSignExtend( ins, ins->base_type_class );
        break;
    case G_CVTTS:
        assert( ins->operands[0]->n.class == N_REGISTER );
        assert( ins->result->n.class == N_REGISTER );
        // 'cvt.s.d fd,fs'
        GenFloatRType( FD, 0x20, _NameReg( ins->result ),
                        _NameReg( ins->operands[0] ), 0 );
        break;
    case G_FREGTOMI8:
        assert( ins->operands[0]->n.class == N_REGISTER );
        assert( ins->result->n.class != N_REGISTER );
        reg_index = _NameReg( ins->operands[0] );
        getMemEncoding( ins->result, &mem_index, &mem_offset );
        // 'sdc1 rt,offset(base)' - MIPS32-R2/MIPS64 only?
        GenIType( 0x3d, reg_index, mem_index, mem_offset );
        break;
    case G_MI8TOFREG:
        assert( ins->operands[0]->n.class != N_REGISTER );
        assert( ins->result->n.class == N_REGISTER );
        reg_index = _NameReg( ins->result );
        getMemEncoding( ins->operands[0], &mem_index, &mem_offset );
        // 'ldc1 rt,offset(base)' - MIPS32-R2/MIPS64 only?
        GenIType( 0x35, reg_index, mem_index, mem_offset );
        break;
    case G_BINARY_FP:
        assert( ins->operands[0]->n.class == N_REGISTER );
        assert( ins->operands[1]->n.class == N_REGISTER );
        assert( ins->result->n.class == N_REGISTER );
        function = FindFloatingOpcodes( ins );
        reg_index = _OpIsSet( ins->head.opcode ) ? 0 : _NameReg( ins->result );
        GenFloatRType( ins->type_class, function, reg_index,
                _NameReg( ins->operands[0] ), _NameReg( ins->operands[1] ) );
        break;
    case G_BINARY:
        assert( ins->operands[0]->n.class == N_REGISTER );
        assert( ins->operands[1]->n.class == N_REGISTER );
        assert( ins->result->n.class == N_REGISTER );
        opcodes = FindOpcodes( ins );
        switch( ins->head.opcode ) {
        case OP_LSHIFT:
        case OP_RSHIFT:
            // 'sllv', 'srlv' and 'srav' have the operands backwards
            GenRType( opcodes[0], opcodes[1], _NameReg( ins->result ),
                _NameReg( ins->operands[1] ), _NameReg( ins->operands[0] ) );
            break;
        case OP_MUL:
            GenRType( opcodes[0], opcodes[1], 0, _NameReg( ins->operands[0] ),
                _NameReg( ins->operands[1] ) );
            // 'mflo rd'
            GenRType( 0, 0x12, _NameReg( ins->result ), 0, 0 );
            break;
        case OP_DIV:
            // TODO: do something if divisor is zero
            GenRType( opcodes[0], opcodes[1], 0, _NameReg( ins->operands[0] ),
                _NameReg( ins->operands[1] ) );
            // 'mflo rd'
            GenRType( 0, 0x12, _NameReg( ins->result ), 0, 0 );
            break;
        case OP_MOD:
            // TODO: do something if divisor is zero
            GenRType( opcodes[0], opcodes[1], 0, _NameReg( ins->operands[0] ),
                _NameReg( ins->operands[1] ) );
            // 'mfhi rd'
            GenRType( 0, 0x10, _NameReg( ins->result ), 0, 0 );
            break;
        default:
            GenRType( opcodes[0], opcodes[1], _NameReg( ins->result ),
                _NameReg( ins->operands[0] ), _NameReg( ins->operands[1] ) );
            break;
        }
        break;
    case G_BINARY_IMM:
        assert( ins->operands[0]->n.class == N_REGISTER );
        assert( ins->operands[1]->n.class == N_CONSTANT );
        assert( ins->result->n.class == N_REGISTER );
        imm_value = ins->operands[1]->c.int_value;
        switch( ins->head.opcode ) {
        case OP_LSHIFT:
            // 'sll rd,rs,n'
            GenIShift( 0x00, _NameReg( ins->result ),
                _NameReg( ins->operands[0] ), imm_value );
            break;
        case OP_RSHIFT:
            if( _IsSigned( ins->type_class ) ) {
                // 'sra rd,rs,n'
                GenIShift( 0x03, _NameReg( ins->result ),
                    _NameReg( ins->operands[0] ), imm_value );
            } else {
                // 'srl rd,rs,n'
                GenIShift( 0x02, _NameReg( ins->result ),
                    _NameReg( ins->operands[0] ), imm_value );
            }
            break;
        case OP_SUB:
        case OP_EXT_SUB:
            // Have to flip sign since there's no 'subiu'
            imm_value = -imm_value;
            // Fall through
        default:
            opcode = FindImmedOpcode( ins );
            GenIType( opcode, _NameReg( ins->result ),
                    _NameReg( ins->operands[0] ), imm_value );
            break;
        }
        break;
    case G_BYTE_CONST:
        // TODO: this may not be needed on MIPS (since we can easily load 16-bit const)?
        assert( ins->operands[0]->n.class == N_CONSTANT );
        assert( ins->result->n.class == N_REGISTER );
        // 'addiu rt,$zero,immed'
        GenIType( 0x09, _NameReg( ins->result ), MIPS_ZERO_SINK,
                  (uint_8)ins->operands[0]->c.int_value );
        break;
    case G_MOVE:
        assert( ins->operands[0]->n.class == N_REGISTER );
        assert( ins->result->n.class == N_REGISTER );
        // 'or rd,rs,$zero'
        GenRType( 0x00, 0x25, _NameReg( ins->result ),
            _NameReg( ins->operands[0] ), MIPS_ZERO_SINK );
        if( TypeClassSize[ins->type_class] == 8 ) {
            // Move the odd register, too
            // TODO: there should probably be a separate G_MOVE8?
            GenRType( 0x00, 0x25, _NameReg( ins->result ) + 1,
                _NameReg( ins->operands[0] ) + 1, MIPS_ZERO_SINK );
        }
        break;
    case G_LEA_HIGH:
        assert( ins->operands[0]->n.class == N_CONSTANT );
        assert( ins->operands[0]->c.const_type == CONS_HIGH_ADDR );
        assert( ins->result->n.class == N_REGISTER );
        // 'lui rt,immed'
        GenIType( 0x0f, _NameReg( ins->result ), MIPS_ZERO_SINK, ins->operands[0]->c.int_value & 0xffff );
        break;
    case G_LEA:
        assert( ins->operands[0]->n.class == N_CONSTANT );
        assert( ins->result->n.class == N_REGISTER );
        switch( ins->operands[0]->c.const_type ) {
        case CONS_ABSOLUTE:
            // 'addiu rt,$zero,immed'
            GenIType( 0x09, _NameReg( ins->result ), MIPS_ZERO_SINK, ins->operands[0]->c.int_value );
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
        switch( ins->operands[0]->n.class ) {
        case N_INDEXED:
        case N_TEMP:
            assert( ins->result->n.class == N_REGISTER );
            getMemEncoding( ins->operands[0], &mem_index, &mem_offset );
            // 'addiu rt,rs,immed'
            GenIType( 0x09, _NameReg( ins->result ), mem_index, mem_offset );
            break;
        case N_MEMORY:
            assert( ins->result->n.class == N_REGISTER );
            FactorInt32( ins->operands[0]->v.offset, &high, &extra, &low );
            if( extra != 0 ) {
                _Zoiks( ZOIKS_132 );
            }
            if( !encodeThreadDataRef( ins ) ) {
                // 'lui rt,immed'
                GenMEMINSRELOC( 0x0f, _NameReg( ins->result ), MIPS_ZERO_SINK, high,
                            symLabel( ins->operands[0] ), OWL_RELOC_HALF_HI );
                // 'addiu rt,rs,immed'
                GenMEMINSRELOC( 0x09, _NameReg( ins->result ), _NameReg( ins->result ), low,
                            symLabel( ins->operands[0] ), OWL_RELOC_HALF_LO );
            }
            break;
        default:
            _Zoiks( ZOIKS_078 );
        }
        break;
    case G_MOVE_UI:
        // a load of an unsigned 16-bit immediate
        // 'ori rt,rs,immed'
        GenIType( 0x0d, _NameReg( ins->result ), MIPS_ZERO_SINK, ins->operands[0]->c.int_value );
        break;
    case G_LOAD_UA:
        doLoadStoreUnaligned( ins, TRUE );
        break;
    case G_LOAD:
        doLoadStore( ins, TRUE );
        break;
    case G_STORE_UA:
        doLoadStoreUnaligned( ins, FALSE );
        break;
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


extern  void GenObjCode( instruction *ins )
/*****************************************/
{
    _AlignmentCheck( ins, 4 );
    Encode( ins );
}


extern  void GenLabelReturn( void )
/*******************************/
{
    GenRET();
}


extern  byte ReverseCondition( byte cond )
/****************************************/
{
    return( FlipOpcode( cond ) );
}


extern  byte CondCode( instruction *ins )
/***************************************/
{
    return( ins->head.opcode );
}
