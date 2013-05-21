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
* Description:  PowerPC instruction encoding.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "opcodes.h"
#include "pattern.h"
#include "vergen.h"
#include "reloc.h"
#include "zoiks.h"
#include "model.h"
#include "coff.h"
#include "procdef.h"
#include "cgaux.h"
#include "ppcenc.h"
#include "ppcgen.h"
#include <assert.h>
#include <stdio.h>

extern void DumpInsOnly( instruction * );
extern void DumpString( char * );
extern void DumpPtr( pointer );
extern void DumpInt( int );
extern void DumpNL( void );
extern void DumpGen( struct opcode_entry * );
extern void DumpPtr( void *ptr );
extern void GenCondJump( instruction * );

extern void             ObjBytes( char *buffer, int size );
extern uint_8           RegTrans( hw_reg_set );
extern void             OutLabel( label_handle );
extern void             OutReloc( label_handle, ppc_reloc, unsigned );
extern hw_reg_set       StackReg( void );
extern hw_reg_set       FrameReg( void );
extern name             *DeAlias( name * );
extern void             TryScrapLabel( code_lbl * );
extern void             DbgBlkBeg( dbg_block *blk, offset lc );
extern void             DbgBlkEnd( dbg_block *blk, offset lc );
extern offset           AskLocation( void );
extern void             OutLineNum( unsigned_16 line, bool label_line );
extern void             EmitDbgInfo( instruction * );
extern pointer          FEAuxInfo( pointer *, aux_class );
extern code_lbl         *GetWeirdPPCDotDotLabel( code_lbl * );
extern  void            GenCondJump( instruction *cond );

extern type_class_def   Unsigned[];
extern type_length      TypeClassSize[];
extern proc_def         *CurrProc;

#define _NameReg( op )                  ( (op)->r.arch_index )
#define _IsSigned( type )               ( Unsigned[type] != type )
#define _Emit( ins )                    ObjBytes( (char *)&(ins), sizeof( ppc_ins ) )

#define ZERO_SINK       0
#define STACK_REG       1
#define TOC_REG         2
#define VOLATILE_REG    12

#pragma off (unreferenced);

#define _BinaryOpcode( a, b )           { { a, b }, { a, b } }
#define _SignedOpcode( a, b, c, d )     { { a, b }, { c, d } }

// Our table for opcode values is really a list of pairs of
// primary opcode / function code pairs. Their is two entries
// for each opcode in case the sign of the instruction matters;
// for example, for OP_RSHIFT we need to generate either srw or
// sraw. If the sign of the type of the instruction doesn't
// matter, we can just use the _BinaryOpcode macro to create
// identical cases, otherwise we give each pair explicitly.

static  opcode  BinaryOpcodes[][2][2] = {
        _BinaryOpcode( 31, 266 ),                       /* OP_ADD */
        _BinaryOpcode( 31, 266 ),                       /* OP_EXT_ADD */
        _BinaryOpcode( 31, 8 ),                         /* OP_SUB */
        _BinaryOpcode( 31, 8 ),                         /* OP_EXT_SUB */
        _BinaryOpcode( 31, 235 ),                       /* OP_MUL */
        _BinaryOpcode( 31, 235 ),                       /* OP_EXT_MUL */
        _SignedOpcode( 31, 459, 31, 491 ),              /* OP_DIV */
        _BinaryOpcode( 0, 0 ),                          /* OP_MOD */
        _BinaryOpcode( 31, 28 ),                        /* OP_AND */
        _BinaryOpcode( 31, 444 ),                       /* OP_OR */
        _BinaryOpcode( 31, 316 ),                       /* OP_XOR */
        _SignedOpcode( 31, 536, 31, 792 ),              /* OP_RSHIFT */
        _BinaryOpcode( 31, 24 ),                        /* OP_LSHIFT */
        _BinaryOpcode( 0, 0 ),                          /* OP_POW */
        _BinaryOpcode( 0, 0 ),                          /* OP_ATAN2 */
        _BinaryOpcode( 0, 0 ),                          /* OP_FMOD */
};

static  opcode  FPOpcodes[][2] = {
        { 63, 21 },                                     /* OP_ADD */
        { 63, 21 },                                     /* OP_EXT_ADD */
        { 63, 20 },                                     /* OP_SUB */
        { 63, 20 },                                     /* OP_EXT_SUB */
        { 63, 25 },                                     /* OP_MUL */
        { 63, 25 },                                     /* OP_EXT_MUL */
        { 63, 18 },                                     /* OP_DIV */
        { 0, 0 },                                       /* OP_MOD */
        { 0, 0 },                                       /* OP_AND */
        { 0, 0 },                                       /* OP_OR */
        { 0, 0 },                                       /* OP_XOR */
        { 0, 0 },                                       /* OP_RSHIFT */
        { 0, 0 },                                       /* OP_LSHIFT */
        { 0, 0 },                                       /* OP_POW */
        { 0, 0 },                                       /* OP_ATAN2 */
        { 0, 0 },                                       /* OP_FMOD */
};

static  opcode  BinaryImmedOpcodes[] = {
        14,                     /* OP_ADD */
        14,                     /* OP_EXT_ADD */
        8,                      /* OP_SUB */
        8,                      /* OP_EXT_SUB */
        7,                      /* OP_MUL */
        7,                      /* OP_EXT_MUL */
        0,                      /* OP_DIV */
        0,                      /* OP_MOD */
        28,                     /* OP_AND */
        24,                     /* OP_OR */
        26,                     /* OP_XOR */
        0,                      /* OP_RSHIFT */
        0,                      /* OP_LSHIFT */
        0,                      /* OP_POW */
        0,                      /* OP_ATAN2 */
        0,                      /* OP_FMOD */
};


static  opcode  *FindOpcodes( instruction *ins )
/**********************************************/
{
    opcode      *opcodes;

    if( _IsFloating( ins->type_class ) ) {
        opcodes = &FPOpcodes[ins->head.opcode - FIRST_BINARY_OP][0];
    } else {
        opcodes = &BinaryOpcodes[ins->head.opcode - FIRST_BINARY_OP][_IsSigned( ins->type_class )][0];
    }
    return( opcodes );
}


static  opcode  *FindImmedOpcodes( instruction *ins )
/***************************************************/
{
    opcode      *opcodes;

    opcodes = &BinaryImmedOpcodes[ins->head.opcode - FIRST_BINARY_OP];
    if( *opcodes == 0 ) {
        _Zoiks( ZOIKS_091 );
    }
    return( opcodes );
}


extern  void    GenFPOPINS( opcode op1, opcode op2, reg_index a, reg_index c, reg_index d )
//*****************************************************************************************
{
    ppc_ins             encoding;

    encoding = _Op1( op1 ) | _A( a ) | _C( c ) | _D( d ) | _Op2( op2 );
    _Emit( encoding );
}


extern  void    GenOPINS( opcode op1, opcode op2, reg_index a, reg_index b, reg_index s )
//***************************************************************************************
{
    ppc_ins             encoding;

    encoding = _Op1( op1 ) | _A( a ) | _B( b ) | _S( s ) | _Op2( op2 );
    _Emit( encoding );
}


extern  void    GenOPIMM( opcode op1, reg_index d, reg_index a, signed_16 immed )
//*******************************************************************************
{
    ppc_ins             encoding;

    encoding = _Op1( op1 ) | _D( d ) | _A( a ) | _SignedImmed( immed );
    _Emit( encoding );
}


extern  void    GenMTSPR( reg_index d, uint_32 spr, bool from )
//*************************************************************
{
    ppc_ins             encoding;

    encoding = _Op1( 31 ) | _Op2( 467 );
    if( from ) {
        encoding = _Op1( 31 ) | _Op2( 339 );
    }
    encoding |= _D( d ) | _SPR( spr );
    _Emit( encoding );
}


extern  void    GenMEMINS( opcode op, reg_index d, reg_index i, signed_16 displacement )
/**************************************************************************************/
{
    ppc_ins             encoding;

    encoding = _Op1( op ) | _D( d ) | _A( i ) | _SignedImmed( displacement );
    _Emit( encoding );
}


extern  void    GenBRANCH( opcode op, pointer label, bool link, bool absolute )
/*****************************************************************************/
{
    ppc_ins             encoding;
    int_32              loc;

    loc = AskLocation();
    encoding = _Op1( op ) | _AA( absolute ) | _LK( link ) | _BranchImmed( -loc );
    OutReloc( label, PPC_RELOC_BRANCH, 0 );
    _Emit( encoding );
}


extern  void    GenCONDBR( opcode op, opcode bo, opcode bi, pointer label )
/*************************************************************************/
{
    ppc_ins             encoding;

    encoding = _Op1( op ) | _S( bo ) | _A( bi );
    OutReloc( label, PPC_RELOC_BRANCH_COND, 0 );
    _Emit( encoding );
}


extern  void    GenCMP( opcode op, opcode op2, reg_index a, reg_index b )
/***********************************************************************/
{
    ppc_ins             encoding;

    encoding = _Op1( op ) | _A( a ) | _B( b ) | _Op2( op2 );
    _Emit( encoding );
}


extern  void    GenCMPIMM( opcode op, reg_index a, signed_16 imm )
/****************************************************************/
{
    ppc_ins             encoding;

    encoding = _Op1( op ) | _A( a ) | _SignedImmed( imm );
    _Emit( encoding );
}


extern  void    GenRAWINS( uint_32 encoding )
/*******************************************/
{
    _Emit( encoding );
}


static  pointer symLabel( name *mem )
/***********************************/
{
    return( AskForSymLabel( mem->v.symbol, mem->m.memory_type ) );
}


extern  type_length     TempLocation( name *temp )
/************************************************/
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
        offset = CurrProc->targ.frame_size + STACK_HEADER_SIZE;
    }
    return( offset + temp->t.location + temp->v.offset - base->v.offset );
}


static  void    doCall( instruction *ins )
/****************************************/
{
    pointer         sym;
    byte_seq        *code;
    ppc_ins         encoding;
    code_lbl        *lbl;

    code = code;
    sym = ins->operands[CALL_OP_ADDR]->v.symbol;
    code = FEAuxInfo( sym, CALL_BYTES );
    if( code != NULL ) {
        ObjBytes( code->data, code->length );
    } else {
        lbl = symLabel( ins->operands[CALL_OP_ADDR] );
        lbl = GetWeirdPPCDotDotLabel( lbl );
        GenBRANCH( 18, lbl, TRUE, FALSE );
        encoding = 0x60000000;  // ..znop for linker thunk
        OutReloc( lbl, PPC_RELOC_GLUE, 0 );
        ObjBytes( (char *)&encoding, sizeof( ppc_ins ) );
    }
}


static  void    getMemEncoding( name *mem, reg_index *index, int_16 *offset )
/***************************************************************************/
{
    switch( mem->n.class ) {
    case N_INDEXED:
        assert( mem->i.index->n.class == N_REGISTER );
        assert( mem->i.scale == 0 );
        assert( mem->i.constant == (type_length)((signed_16)mem->i.constant) );
        assert( ( mem->i.index_flags & X_LOW_ADDR_BASE ) == 0 );
        *index = RegTrans( mem->i.index->r.reg );
        *offset = (int_16)mem->i.constant;
        break;
    case N_TEMP:
        *index = RegTrans( FrameReg() );
        *offset = TempLocation( mem );
        break;
    case N_MEMORY:
    default:
        *index = ZERO_SINK;
        *offset = 0;
        _Zoiks( ZOIKS_119 );
        break;
    }
}


static  opcode  loadOpcodes[] = {
    34,                 /* U1 */
    34,                 /* I1 */
    40,                 /* U2 */
    42,                 /* I2 */
    32,                 /* U4 */
    32,                 /* I4 */
    00,                 /* U8 */
    00,                 /* I8 */
    32,                 /* CP */
    32,                 /* PT */
    48,                 /* FS */
    50,                 /* FD */
    50,                 /* FL */
};

static  opcode  storeOpcodes[] = {
    38,                 /* U1 */
    38,                 /* I1 */
    44,                 /* U2 */
    44,                 /* I2 */
    36,                 /* U4 */
    36,                 /* I4 */
    00,                 /* U8 */
    00,                 /* I8 */
    36,                 /* CP */
    36,                 /* PT */
    52,                 /* FS */
    54,                 /* FD */
    54,                 /* FL */
};


static  void    doLoadStore( instruction *ins, bool load )
/********************************************************/
{
    name        *mem;
    name        *reg;
    opcode      op;
    reg_index   index;
    int_16      offset;

    if( load ) {
        mem = ins->operands[0];
        reg = ins->result;
        op = loadOpcodes[ins->type_class];
    } else {
        reg = ins->operands[0];
        mem = ins->result;
        op = storeOpcodes[ins->type_class];
    }
    assert( op != 0 );
    assert( reg->n.class == N_REGISTER );
    getMemEncoding( mem, &index, &offset );
    GenMEMINS( op, RegTrans( reg->r.reg ), index, offset );
}


static  void    doSign( instruction *ins )
/****************************************/
{
    switch( ins->base_type_class ) {
    case U1:
    case I1:
        // extsb
        GenOPINS( 31, 954, _NameReg( ins->result ), 0, _NameReg( ins->operands[0] ) );
        break;
    case U2:
    case I2:
        // extsh
        GenOPINS( 31, 922, _NameReg( ins->result ), 0, _NameReg( ins->operands[0] ) );
        break;
    default:
        _Zoiks( ZOIKS_091 );
    }
}


static  void    doZero( instruction *ins )
/****************************************/
{
    switch( ins->base_type_class ) {
    case U1:
    case I1:
        // andi op1,0x00ff -> res
        GenOPIMM( 28, _NameReg( ins->operands[0] ), _NameReg( ins->result ), 0x00ff );
        break;
    case U2:
    case I2:
        // andi op1,0xffff -> res
        GenOPIMM( 28, _NameReg( ins->operands[0] ), _NameReg( ins->result ), 0xffff );
        break;
    default:
        _Zoiks( ZOIKS_091 );
    }
}


static  void    GenCallIndirect( instruction *call )
/**************************************************/
{
    reg_index   src;
    reg_index   reg;
    reg_index   mem_index;
    int_16      mem_offset;
    name        *addr;
    opcode      ldw;
    opcode      stw;

    reg = VOLATILE_REG; /* use the volatile scratch reg if possible */
    src = VOLATILE_REG;
    addr = call->operands[CALL_OP_ADDR];
    ldw = loadOpcodes[U4];
    stw = storeOpcodes[U4];
    GenMEMINS( stw, TOC_REG, STACK_REG, 4 );
    switch( addr->n.class ) {
    case N_REGISTER:
        src = _NameReg( addr );
        break;
    case N_TEMP:
    case N_INDEXED:
        getMemEncoding( addr, &mem_index, &mem_offset );
        GenMEMINS( ldw, src, mem_index, mem_offset );
        break;
    }
    GenMEMINS( ldw, TOC_REG, src, 4 );  // careful - src, reg could be same reg
    GenMEMINS( ldw, reg, src, 0 );
    GenMTSPR( reg, SPR_CTR, FALSE );
    GenRAWINS( 0x4e9e0421 );
    GenMEMINS( ldw, TOC_REG, STACK_REG, 4 );
}


static  void    GenVaStart( instruction *ins )
/********************************************/
{
    opcode      stb;
    opcode      stw;
    opcode      li;
    reg_index   reg;
    reg_index   tmp;
    reg_index   stack;

    assert( ins->operands[0]->n.class == N_REGISTER );
    reg = _NameReg( ins->operands[0] );
    tmp = VOLATILE_REG;
    stack = RegTrans( FrameReg() );
    stb = storeOpcodes[U1];
    stw = storeOpcodes[U4];
    li = 14;    // addi
    GenOPIMM( li, tmp, ZERO_SINK, CurrProc->state.parm.gr );
    GenMEMINS( stb, tmp, reg, 0 );
    GenOPIMM( li, tmp, ZERO_SINK, CurrProc->state.parm.fr );
    GenMEMINS( stb, tmp, reg, 1 );
    GenOPIMM( li, tmp, stack, CurrProc->state.parm.offset + CurrProc->targ.frame_size + STACK_HEADER_SIZE );
    GenMEMINS( stw, tmp, reg, 4 );
    GenOPIMM( li, tmp, stack, CurrProc->targ.stack_map.saved_regs.start );
    GenMEMINS( stw, tmp, reg, 8 );
}


static  void    DbgBlkInfo( instruction *ins )
/********************************************/
{
    offset lc;

    lc = AskLocation();
    if( ins->flags.nop_flags & NOP_DBGINFO_START ) {
        DbgBlkBeg( (dbg_block *)ins->operands[0], lc );
    } else {
        DbgBlkEnd( (dbg_block *)ins->operands[0], lc );
    }
}


extern  void    GenRET( void )
/****************************/
{
   ppc_ins      encoding;

   encoding = 0x4e800020;       // FIXME - need linkage docs
   _Emit( encoding );
}


static  void    Encode( instruction *ins )
/****************************************/
{
    reg_index           a;      // usually used for the result
    reg_index           b;
    reg_index           s;
    reg_index           temp;
    opcode              op1;
    opcode              op2;
    opcode              *ops;
    signed_16           mem_offset;
    reg_index           mem_index;


    switch( ins->u.gen_table->generate ) {
    case G_MOVE_FP:
        assert( ins->result->n.class == N_REGISTER );
        assert( ins->operands[0]->n.class == N_REGISTER );
        GenOPINS( 63, 72, 0, _NameReg( ins->operands[0] ), _NameReg( ins->result ) );
        break;
    case G_MOVE:
        // or op, op -> dst
        assert( ins->result->n.class == N_REGISTER );
        assert( ins->operands[0]->n.class == N_REGISTER );
        b = _NameReg( ins->operands[0] );
        GenOPINS( 31, 444, _NameReg( ins->result ), b, b );
        break;
    case G_LOAD:
    case G_STORE:
        doLoadStore( ins, ins->u.gen_table->generate == G_LOAD );
        break;
    case G_CALL:
        doCall( ins );
        break;
    case G_LEA_HIGH:
        assert( ins->operands[0]->n.class == N_CONSTANT );
        assert( ins->operands[0]->c.const_type == CONS_HIGH_ADDR );
        assert( ins->result->n.class == N_REGISTER );
        /* addis k(r0) -> rn */
        GenOPIMM( 15, _NameReg( ins->result ), ZERO_SINK, ins->operands[0]->c.int_value & 0xffff );
        break;
    case G_MOVE_UI:
        /* a load of an unsigned 16-bit immediate */
        /* use or rd, imm(r0) */
        GenOPIMM( 24, _NameReg( ins->result ), ZERO_SINK, ins->operands[0]->c.int_value );
        break;
    case G_LEA:
        assert( ins->operands[0]->n.class == N_CONSTANT );
        assert( ins->result->n.class == N_REGISTER );
        switch( ins->operands[0]->c.const_type ) {
        case CONS_ABSOLUTE:
            // addi rd, imm(r0)
            GenOPIMM( 14, _NameReg( ins->result ), ZERO_SINK, ins->operands[0]->c.int_value );
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
            // addi rd,off(ri|sp)
            GenOPIMM( 14, _NameReg( ins->result ), mem_index, mem_offset );
            break;
        case N_MEMORY:
            assert( ins->result->n.class == N_REGISTER );
            OutReloc( symLabel( ins->operands[0] ), PPC_RELOC_TOC_OFFSET, 2 );
            GenMEMINS( 32, _NameReg( ins->result ), TOC_REG, 0 );
            break;
        default:
            _Zoiks( ZOIKS_119 );
        }
        break;
    case G_BINARY_FP:
        assert( ins->operands[0]->n.class == N_REGISTER );
        assert( ins->operands[1]->n.class == N_REGISTER );
        assert( ins->result->n.class == N_REGISTER );
        if( ins->head.opcode == OP_MUL ) {
            ops = FindOpcodes( ins );
            GenFPOPINS( ops[0], ops[1],
                        _NameReg( ins->operands[0] ),
                        _NameReg( ins->operands[1] ),
                        _NameReg( ins->result ) );
            break;
        }
        /* fall through */
    case G_BINARY:
        assert( ins->operands[0]->n.class == N_REGISTER );
        assert( ins->operands[1]->n.class == N_REGISTER );
        assert( ins->result->n.class == N_REGISTER );
        ops = FindOpcodes( ins );
        a = _NameReg( ins->operands[0] );
        b = _NameReg( ins->operands[1] );
        s = _NameReg( ins->result );
        if( ins->head.opcode == OP_SUB && ins->u.gen_table->generate == G_BINARY ) {
            /* someone sucks - it's not me */
            temp = a;
            a = b;
            b = temp;
        }
        GenOPINS( ops[0], ops[1], a, b, s );
        break;
    case G_BINARYS:
        assert( ins->operands[0]->n.class == N_REGISTER );
        assert( ins->operands[1]->n.class == N_REGISTER );
        assert( ins->result->n.class == N_REGISTER );
        ops = FindOpcodes( ins );
        s = _NameReg( ins->operands[0] );
        b = _NameReg( ins->operands[1] );
        a = _NameReg( ins->result );
        GenOPINS( ops[0], ops[1], a, b, s );
        break;
    case G_BINARYS_IMM:
    case G_BINARY_IMM:
        assert( ins->operands[0]->n.class == N_REGISTER );
        assert( ins->operands[1]->n.class == N_CONSTANT );
        assert( ins->result->n.class == N_REGISTER );
        switch( ins->head.opcode ) {
        case OP_LSHIFT:
            // rlwinm dst,src,n,0,31-n
            op2 = 31 - _FiveBits( ins->operands[1]->c.int_value );
            b = _FiveBits( ins->operands[1]->c.int_value );
            GenOPINS( 21, op2, _NameReg( ins->result ), b, _NameReg( ins->operands[0] ) );
            break;
        case OP_RSHIFT:
            if( _IsSigned( ins->type_class ) ) {
                GenOPINS( 31, 824, _NameReg( ins->result ),
                    ins->operands[1]->c.int_value, _NameReg( ins->operands[0] ) );
            } else {
                // rlwinm dst,src,32-n,n,31
                b = _FiveBits( ins->operands[1]->c.int_value );
                op2 = ( b << 5 ) | 31;
                b = 32 - b;
                GenOPINS( 21, op2, _NameReg( ins->result ), b, _NameReg( ins->operands[0] ) );
            }
            break;
        default:
            ops = FindImmedOpcodes( ins );
            if( ins->u.gen_table->generate == G_BINARYS_IMM ) {
                s = _NameReg( ins->operands[0] );
                a = _NameReg( ins->result );
            } else {
                s = _NameReg( ins->result );
                a = _NameReg( ins->operands[0] );
            }
            GenOPIMM( ops[0], s, a, ins->operands[1]->c.int_value );
            break;
        }
        break;
    case G_CMP_FP:
        assert( ins->operands[0]->n.class == N_REGISTER );
        assert( ins->operands[1]->n.class == N_REGISTER );
        GenCMP( 63, 32, _NameReg( ins->operands[0] ), _NameReg( ins->operands[1] ) );
        break;
    case G_CMP:
        assert( ins->operands[0]->n.class == N_REGISTER );
        assert( ins->operands[1]->n.class == N_REGISTER );
        op2 = 32;
        if( _IsSigned( ins->type_class ) ) {
            op2 = 0;
        }
        GenCMP( 31, op2, _NameReg( ins->operands[0] ), _NameReg( ins->operands[1] ) );
        break;
    case G_CMP_I:
        assert( ins->operands[0]->n.class == N_REGISTER );
        assert( ins->operands[1]->n.class == N_CONSTANT );
        op1 = 10;
        if( _IsSigned( ins->type_class ) ) {
            op1 = 11;
        }
        GenCMPIMM( op1, _NameReg( ins->operands[0] ), ins->operands[1]->c.int_value );
        break;
    case G_SIGN:
        assert( ins->operands[0]->n.class == N_REGISTER );
        assert( ins->result->n.class == N_REGISTER );
        doSign( ins );
        break;
    case G_ZERO:
        assert( ins->operands[0]->n.class == N_REGISTER );
        assert( ins->result->n.class == N_REGISTER );
        doZero( ins );
        break;
    case G_FREGTOMI4:
        assert( ins->operands[0]->n.class == N_REGISTER );
        assert( ins->result->n.class == N_TEMP || ins->result->n.class == N_INDEXED );
        break;
    case G_UNARY:
        assert( ins->operands[0]->n.class == N_REGISTER );
        assert( ins->result->n.class == N_REGISTER );
        a = _NameReg( ins->operands[0] );
        s = _NameReg( ins->result );
        switch( ins->head.opcode ) {
        case OP_NEGATE:
            // neg src -> dst
            GenOPINS( 31, 104, a, 0, s );
            break;
        case OP_COMPLEMENT:
            GenOPINS( 31, 124, a, a, s );
            break;
        default:
            _Zoiks( ZOIKS_119 );
        }
        break;
    case G_CALLI:
        GenCallIndirect( ins );
        break;
    case G_VASTART:
        GenVaStart( ins );
        break;
    case G_NO:
        return;
    case G_UNKNOWN:
        _Zoiks( ZOIKS_097 );
        break;
    case G_DEBUG:
        DbgBlkInfo( ins );
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


extern  void    GenObjCode( instruction *ins )
/********************************************/
{
    Encode( ins );
    if( ins->u.gen_table->generate == G_CMP ||
        ins->u.gen_table->generate == G_CMP_I ||
        ins->u.gen_table->generate == G_CMP_FP ) {
        GenCondJump( ins );
    }
}

#define MAX_ALIGNMENT   16

static char Zeros[MAX_ALIGNMENT];

extern  void    CodeLabel( label_handle label, unsigned alignment )
/*****************************************************************/
{
    offset      loc;
    offset      modulus;

    assert( alignment <= MAX_ALIGNMENT );
    loc = AskLocation();
    modulus = loc % alignment;
    if( modulus != 0 ) {
        ObjBytes( &Zeros[0], alignment - modulus );
    }
    OutLabel( label );
#ifndef NDEBUG
    if( _IsTargetModel( ASM_OUTPUT ) ) {
        DumpString( "L" );
        DumpPtr( label );
        DumpString( ":" );
        DumpNL();
    }
#endif
}


extern  void    CodeLineNum( cg_linenum line, bool label )
/********************************************************/
{
#ifndef NDEBUG
    if( _IsTargetModel( ASM_OUTPUT ) ) {
        DumpString( "Source Line: " );
        DumpInt( line );
        DumpNL();
    }
#endif
    OutLineNum( line, label );
}


extern  void    GenJumpLabel( pointer label )
/*******************************************/
{
    GenBRANCH( 18, label, FALSE, FALSE );
#ifndef NDEBUG
    if( _IsTargetModel( ASM_OUTPUT ) ) {
        DumpString( "JMP L" );
        DumpPtr( label );
        DumpNL();
    }
#endif
}

#define LT      0x00
#define GT      0x01
#define EQ      0x02

#define NORMAL  0x0C
#define INVERT  0x04

static  opcode  BranchOpcodes[][2] = {
    // page 3-68 for a real description
    // BO     BI
    { NORMAL, EQ },                     /* OP_CMP_EQUAL */
    { INVERT, EQ },                     /* OP_CMP_NOT_EQUAL */
    { NORMAL, GT },                     /* OP_CMP_GREATER */
    { INVERT, GT },                     /* OP_CMP_LESS_EQUAL */
    { NORMAL, LT },                     /* OP_CMP_LESS */
    { INVERT, LT },                     /* OP_CMP_GREATER_EQUAL */
};

extern  void    GenJumpIf( instruction *ins, pointer label )
/**********************************************************/
{
    opcode      *ops;

    ops = &BranchOpcodes[ins->head.opcode - FIRST_COMPARISON][0]; // fixme - floating point
    GenCONDBR( 16, ops[0], ops[1], label );
#ifndef NDEBUG
    if( _IsTargetModel( ASM_OUTPUT ) ) {
        DumpString( "Jcc L" );
        DumpPtr( label );
        DumpNL();
    }
#endif
}


extern  void    GenKillLabel( code_lbl *lbl )
/*******************************************/
{
    _ValidLbl( lbl );
    if( _TstStatus( lbl, CODELABEL ) == FALSE ) return;
    // if( _TstStatus( lbl, OWL_OWNED ) != FALSE ) return;
    TryScrapLabel( lbl );
}


extern  void    GenCallLabel( pointer label )
/*******************************************/
{
}


extern  void    GenLabelReturn( void )
/************************************/
{
}


extern  byte    ReverseCondition( byte cond )
/*******************************************/
{
    return( cond );
}


extern  label_handle    LocateLabel( instruction *ins, int index )
/****************************************************************/
{
    if( index == NO_JUMP ) return( NULL );
    for( ;; ) {
        ins = ins->head.next;
        if( ins->head.opcode == OP_BLOCK ) break;
    }
    return( _BLOCK( ins )->edge[index].destination );
}


static  block   *InsBlock( instruction *ins )
/*******************************************/
{
    while( ins->head.opcode != OP_BLOCK ) {
        ins = ins->head.next;
    }
    return( _BLOCK( ins ) );
}


extern  void    GenCondJump( instruction *cond )
/**********************************************/
{
    label_handle        dest_false;
    label_handle        dest_true;
    block               *blk;

    dest_false = LocateLabel( cond, _FalseIndex( cond ) );
    dest_true = LocateLabel( cond, _TrueIndex( cond ) );
    if( dest_false != dest_true ) {
        GenJumpIf( cond, dest_true );
    }
    blk = InsBlock( cond );
    if( dest_false != blk->next_block->label ) {
        GenJumpLabel( dest_false );
    }
}


extern  void EmitInsReloc( ppc_ins ins, pointer sym, owl_reloc_type type )
/************************************************************************/
{
#if 0

    // copy & paste from AXP cg
    oc_riscins          oc;

    oc.op.objlen = 4;
    oc.op.class = OC_RCODE;
    oc.op.reclen = sizeof( oc_riscins );
    oc.opcode = ins;
    oc.sym = sym;
    oc.reloc = type;
    InputOC( (any_oc *)&oc );
#else
    _Zoiks( ZOIKS_091 );
#endif
}
