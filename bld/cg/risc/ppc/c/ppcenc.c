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
* Description:  PowerPC instruction encoding.
*
****************************************************************************/


#include "_cgstd.h"
#include <stdio.h>
#include "coderep.h"
#include "reloc.h"
#include "zoiks.h"
#include "coff.h"
#include "cgauxcc.h"
#include "cgauxinf.h"
#include "optmain.h"
#include "ppcenc.h"
#include "data.h"
#include "objout.h"
#include "dbsyms.h"
#include "object.h"
#include "encode.h"
#include "intrface.h"
#include "targetin.h"
#include "rgtbl.h"
#include "rscobj.h"
#include "split.h"
#include "namelist.h"
#include "ppclbl.h"
#include "insutil.h"
#include "dumpio.h"
#include "dumpins.h"
#include "dumptab.h"
#include "feprotos.h"


#define _NameRegTrans( op )         ((reg_idx)(op)->r.arch_index)
#define _EmitIns( ins )             ObjBytes( &(ins), sizeof( ppc_ins ) )
#define _ObjEmitSeq( code )         ObjBytes( code->data, code->length )

#define _BinaryOpcode( a, b )       { { a, b }, { a, b } }
#define _SignedOpcode( a, b, c, d ) { { a, b }, { c, d } }

#define MAX_ALIGNMENT   16

#define LT              0x00
#define GT              0x01
#define EQ              0x02

#define NORMAL          0x0C
#define INVERT          0x04

#define ZERO_SINK       0

/*
 * Our table for gen_opcode values is really a list of pairs of
 * primary opcode / function code pairs. Their is two entries
 * for each opcode in case the sign of the instruction matters;
 * for example, for OP_RSHIFT we need to generate either srw or
 * sraw. If the sign of the type of the instruction doesn't
 * matter, we can just use the _BinaryOpcode macro to create
 * identical cases, otherwise we give each pair explicitly.
 */

static  gen_opcode  BinaryOpcodes[][2][2] = {
    _BinaryOpcode( 31, 266 ),           /* OP_ADD */
    _BinaryOpcode( 31, 266 ),           /* OP_EXT_ADD */
    _BinaryOpcode( 31, 8 ),             /* OP_SUB */
    _BinaryOpcode( 31, 8 ),             /* OP_EXT_SUB */
    _BinaryOpcode( 31, 235 ),           /* OP_MUL */
    _BinaryOpcode( 31, 235 ),           /* OP_EXT_MUL */
    _SignedOpcode( 31, 459, 31, 491 ),  /* OP_DIV */
    _BinaryOpcode( 0, 0 ),              /* OP_MOD */
    _BinaryOpcode( 31, 28 ),            /* OP_AND */
    _BinaryOpcode( 31, 444 ),           /* OP_OR */
    _BinaryOpcode( 31, 316 ),           /* OP_XOR */
    _SignedOpcode( 31, 536, 31, 792 ),  /* OP_RSHIFT */
    _BinaryOpcode( 31, 24 ),            /* OP_LSHIFT */
    _BinaryOpcode( 0, 0 ),              /* OP_POW */
    _BinaryOpcode( 0, 0 ),              /* OP_ATAN2 */
    _BinaryOpcode( 0, 0 ),              /* OP_FMOD */
};

static  gen_opcode  FPOpcodes[][2] = {
    { 63, 21 },         /* OP_ADD */
    { 63, 21 },         /* OP_EXT_ADD */
    { 63, 20 },         /* OP_SUB */
    { 63, 20 },         /* OP_EXT_SUB */
    { 63, 25 },         /* OP_MUL */
    { 63, 25 },         /* OP_EXT_MUL */
    { 63, 18 },         /* OP_DIV */
    { 0, 0 },           /* OP_MOD */
    { 0, 0 },           /* OP_AND */
    { 0, 0 },           /* OP_OR */
    { 0, 0 },           /* OP_XOR */
    { 0, 0 },           /* OP_RSHIFT */
    { 0, 0 },           /* OP_LSHIFT */
    { 0, 0 },           /* OP_POW */
    { 0, 0 },           /* OP_ATAN2 */
    { 0, 0 },           /* OP_FMOD */
};

static  gen_opcode  BinaryImmedOpcodes[] = {
    14,                 /* OP_ADD */
    14,                 /* OP_EXT_ADD */
    8,                  /* OP_SUB */
    8,                  /* OP_EXT_SUB */
    7,                  /* OP_MUL */
    7,                  /* OP_EXT_MUL */
    0,                  /* OP_DIV */
    0,                  /* OP_MOD */
    28,                 /* OP_AND */
    24,                 /* OP_OR */
    26,                 /* OP_XOR */
    0,                  /* OP_RSHIFT */
    0,                  /* OP_LSHIFT */
    0,                  /* OP_POW */
    0,                  /* OP_ATAN2 */
    0,                  /* OP_FMOD */
};

static  gen_opcode  loadOpcodes[] = {
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

static  gen_opcode  storeOpcodes[] = {
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

static  gen_opcode  BranchOpcodes[][2] = {
    /*
     * page 3-68 for a real description
     * BO     BI
     */
    { NORMAL, EQ },     /* OP_CMP_EQUAL */
    { INVERT, EQ },     /* OP_CMP_NOT_EQUAL */
    { NORMAL, GT },     /* OP_CMP_GREATER */
    { INVERT, GT },     /* OP_CMP_LESS_EQUAL */
    { NORMAL, LT },     /* OP_CMP_LESS */
    { INVERT, LT },     /* OP_CMP_GREATER_EQUAL */
};

static byte     Zeros[MAX_ALIGNMENT];

static ppc_ins  ins_encoding = 0;

void *InsRelocInit( void *ins )
/*****************************/
{
    ins_encoding = *(ppc_ins *)ins;
    return( &ins_encoding );
}

void InsRelocAddSignedImmed( int disp )
/*************************************/
{
    ins_encoding |= _SignedImmed( disp );
}

void *InsRelocNext( void *ins )
/*****************************/
{
    return( (ppc_ins *)ins + 1 );
}

void EmitInsReloc( void *ins, pointer sym, owl_reloc_type type )
/**************************************************************/
{
#if 0
    /*
     * copy & paste from AXP cg
     */
    any_oc      oc;

    oc.oc_rins.op.class = OC_RCODE;
    oc.oc_rins.op.reclen = sizeof( oc_riscins );
    oc.oc_rins.op.objlen = 4;
    oc.oc_rins.opcode = *(ppc_ins *)ins;
    oc.oc_rins.sym = sym;
    oc.oc_rins.reloc = type;
    InputOC( &oc );
#else
    /* unused parameters */ (void)ins; (void)sym; (void)type;

    _Zoiks( ZOIKS_091 );
#endif
}

static  gen_opcode  *FindOpcodes( instruction *ins )
/**************************************************/
{
    gen_opcode      *opcodes;

    if( _IsFloating( ins->type_class ) ) {
        opcodes = &FPOpcodes[ins->head.opcode - FIRST_BINARY_OP][0];
    } else {
        opcodes = &BinaryOpcodes[ins->head.opcode - FIRST_BINARY_OP][_IsSigned( ins->type_class ) ? 1 : 0][0];
    }
    return( opcodes );
}


static  gen_opcode  *FindImmedOpcodes( instruction *ins )
/*******************************************************/
{
    gen_opcode      *opcodes;

    opcodes = &BinaryImmedOpcodes[ins->head.opcode - FIRST_BINARY_OP];
    if( *opcodes == 0 ) {
        _Zoiks( ZOIKS_091 );
    }
    return( opcodes );
}


static void    GenFPOPINS( gen_opcode op1, gen_opcode op2, reg_idx a, reg_idx c, reg_idx d )
//******************************************************************************************
{
    ins_encoding = _Opcode( op1 ) | _A( a ) | _C( c ) | _D( d ) | _Opcode2( op2 );
    _EmitIns( ins_encoding );
}


void    GenOPINS( gen_opcode op1, gen_opcode op2, reg_idx a, reg_idx b, reg_idx s )
//*********************************************************************************
{
    ins_encoding = _Opcode( op1 ) | _A( a ) | _B( b ) | _S( s ) | _Opcode2( op2 );
    _EmitIns( ins_encoding );
}


void    GenOPIMM( gen_opcode op1, reg_idx d, reg_idx a, int_16 immed )
//********************************************************************
{
    ins_encoding = _Opcode( op1 ) | _D( d ) | _A( a ) | _SignedImmed( immed );
    _EmitIns( ins_encoding );
}


void    GenMTSPR( reg_idx d, uint_32 spr, bool from )
//***************************************************
{
    ins_encoding = _Opcode( 31 ) | _Opcode2( 467 );
    if( from ) {
        ins_encoding = _Opcode( 31 ) | _Opcode2( 339 );
    }
    ins_encoding |= _D( d ) | _SPR( spr );
    _EmitIns( ins_encoding );
}


void    GenMEMINS( gen_opcode op, reg_idx d, reg_idx i, int_16 displacement )
/***************************************************************************/
{
    ins_encoding = _Opcode( op ) | _D( d ) | _A( i ) | _SignedImmed( displacement );
    _EmitIns( ins_encoding );
}


static void    GenBRANCH( gen_opcode op, pointer label, bool link, bool absolute )
/********************************************************************************/
{
    int_32              loc;

    loc = AskLocation();
    ins_encoding = _Opcode( op ) | __AA( absolute ) | __LK( link ) | _BranchImmed( -loc );
    OutReloc( label, PPC_RELOC_BRANCH, 0 );
    _EmitIns( ins_encoding );
}


static void    GenCONDBR( gen_opcode op, gen_opcode bo, gen_opcode bi, pointer label )
/************************************************************************************/
{
    ins_encoding = _Opcode( op ) | _S( bo ) | _A( bi );
    OutReloc( label, PPC_RELOC_BRANCH_COND, 0 );
    _EmitIns( ins_encoding );
}


static void    GenCMP( gen_opcode op, gen_opcode op2, reg_idx a, reg_idx b )
/**************************************************************************/
{
    ins_encoding = _Opcode( op ) | _A( a ) | _B( b ) | _Opcode2( op2 );
    _EmitIns( ins_encoding );
}


static void    GenCMPIMM( gen_opcode op, reg_idx a, int_16 imm )
/**************************************************************/
{
    ins_encoding = _Opcode( op ) | _A( a ) | _SignedImmed( imm );
    _EmitIns( ins_encoding );
}


static void    GenRAWINS( ppc_ins encoding )
/******************************************/
{
    _EmitIns( encoding );
}


static  pointer symLabel( name *mem )
/***********************************/
{
    return( AskForSymLabel( mem->v.symbol, mem->m.memory_type ) );
}


type_length     TempLocation( name *temp )
/****************************************/
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


static  void    GenNoReturn( void ) {
/************************************
    Generate a noreturn instruction (pseudo instruction)
*/

    any_oc      oc;

    oc.oc_entry.hdr.class = OC_NORET;
    oc.oc_entry.hdr.reclen = offsetof( oc_entry, data );
    oc.oc_entry.hdr.objlen = 0;
    InputOC( &oc );
}

static  void    doCall( instruction *ins )
/****************************************/
{
    call_class      cclass;
    byte_seq        *code;
    label_handle    lbl;
    name            *op;

    op = ins->operands[CALL_OP_ADDR];
    cclass = (call_class)(pointer_uint)FindAuxInfo( op, FEINF_CALL_CLASS );
    code = FindAuxInfo( op, FEINF_CALL_BYTES );
    if( code != NULL ) {
        _ObjEmitSeq( code );
        if( cclass & FECALL_GEN_ABORTS ) {
            GenNoReturn();
        }
    } else {
        lbl = symLabel( op );
        lbl = GetWeirdPPCDotDotLabel( lbl );
        GenBRANCH( 18, lbl, true, false );
        OutReloc( lbl, PPC_RELOC_GLUE, 0 );
        ins_encoding = 0x60000000;  // ..znop for linker thunk
        _EmitIns( ins_encoding );
    }
    if( cclass & FECALL_GEN_NORETURN ) {
        GenNoReturn();
    }
}


static void getMemEncoding( name *mem, reg_idx *reg_mem, int_16 *offset )
/***********************************************************************/
{
    switch( mem->n.class ) {
    case N_INDEXED:
        assert( mem->i.index->n.class == N_REGISTER );
        assert( mem->i.scale == 0 );
        assert( mem->i.constant == (type_length)((int_16)mem->i.constant) );
        assert( ( mem->i.index_flags & X_LOW_ADDR_BASE ) == 0 );
        *reg_mem = RegTrans( mem->i.index->r.reg );
        *offset = (int_16)mem->i.constant;
        break;
    case N_TEMP:
        *reg_mem = RegTrans( FrameReg() );
        *offset = TempLocation( mem );
        break;
    case N_MEMORY:
    default:
        *reg_mem = ZERO_SINK;
        *offset = 0;
        _Zoiks( ZOIKS_119 );
        break;
    }
}


static  void    doLoadStore( instruction *ins, bool load )
/********************************************************/
{
    name        *mem;
    name        *reg;
    gen_opcode  op;
    reg_idx     reg_mem;
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
    getMemEncoding( mem, &reg_mem, &offset );
    GenMEMINS( op, RegTrans( reg->r.reg ), reg_mem, offset );
}


static  void    doSign( instruction *ins )
/****************************************/
{
    switch( ins->base_type_class ) {
    case U1:
    case I1:
        /*
         * extsb
         */
        GenOPINS( 31, 954, _NameRegTrans( ins->result ), 0,
            _NameRegTrans( ins->operands[0] ) );
        break;
    case U2:
    case I2:
        /*
         * extsh
         */
        GenOPINS( 31, 922, _NameRegTrans( ins->result ), 0,
            _NameRegTrans( ins->operands[0] ) );
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
        /*
         * andi op1,0x00ff -> res
         */
        GenOPIMM( 28, _NameRegTrans( ins->operands[0] ),
            _NameRegTrans( ins->result ), 0x00ff );
        break;
    case U2:
    case I2:
        /*
         * andi op1,0xffff -> res
         */
        GenOPIMM( 28, _NameRegTrans( ins->operands[0] ),
            _NameRegTrans( ins->result ), -1 );
        break;
    default:
        _Zoiks( ZOIKS_091 );
    }
}


static  void    GenCallIndirect( instruction *call )
/**************************************************/
{
    reg_idx     src;
    reg_idx     reg;
    reg_idx     reg_mem;
    int_16      mem_offset;
    name        *addr;
    gen_opcode  ldw;
    gen_opcode  stw;

    reg = AT_REG_IDX; /* use the volatile scratch reg if possible */
    src = AT_REG_IDX;
    addr = call->operands[CALL_OP_ADDR];
    ldw = loadOpcodes[U4];
    stw = storeOpcodes[U4];
    GenMEMINS( stw, RTOC_REG_IDX, SP_REG_IDX, 4 );
    switch( addr->n.class ) {
    case N_REGISTER:
        src = _NameRegTrans( addr );
        break;
    case N_TEMP:
    case N_INDEXED:
        getMemEncoding( addr, &reg_mem, &mem_offset );
        GenMEMINS( ldw, src, reg_mem, mem_offset );
        break;
    }
    GenMEMINS( ldw, RTOC_REG_IDX, src, 4 ); // careful - src, reg could be same reg
    GenMEMINS( ldw, reg, src, 0 );
    GenMTSPR( reg, SPR_CTR, false );
    GenRAWINS( 0x4e9e0421 );                // bcctrl 20,30
    GenMEMINS( ldw, RTOC_REG_IDX, SP_REG_IDX, 4 );
}


static  void    GenVaStart( instruction *ins )
/********************************************/
{
    gen_opcode  stb;
    gen_opcode  stw;
    gen_opcode  li;
    reg_idx     reg;
    reg_idx     tmp;
    reg_idx     stack;

    assert( ins->operands[0]->n.class == N_REGISTER );
    reg = _NameRegTrans( ins->operands[0] );
    tmp = AT_REG_IDX;
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


void    GenReturn( void )
/***********************/
{
   ins_encoding = 0x4e800020;   // "blr" FIXME - need linkage docs
   _EmitIns( ins_encoding );
}


static  void    Encode( instruction *ins )
/****************************************/
{
    reg_idx             a;      // usually used for the result
    reg_idx             b;
    reg_idx             s;
    reg_idx             temp;
    gen_opcode          op1;
    gen_opcode          op2;
    gen_opcode          *ops;
    int_16              mem_offset;
    reg_idx             reg_mem;


    switch( G( ins ) ) {
    case G_MOVE_FP:
        assert( ins->result->n.class == N_REGISTER );
        assert( ins->operands[0]->n.class == N_REGISTER );
        GenOPINS( 63, 72, 0, _NameRegTrans( ins->operands[0] ),
            _NameRegTrans( ins->result ) );
        break;
    case G_MOVE:
        // or op, op -> dst
        assert( ins->result->n.class == N_REGISTER );
        assert( ins->operands[0]->n.class == N_REGISTER );
        b = _NameRegTrans( ins->operands[0] );
        GenOPINS( 31, 444, _NameRegTrans( ins->result ), b, b );
        break;
    case G_LOAD:
    case G_STORE:
        doLoadStore( ins, G( ins ) == G_LOAD );
        break;
    case G_CALL:
        doCall( ins );
        break;
    case G_LEA_HIGH:
        assert( ins->operands[0]->n.class == N_CONSTANT );
        assert( ins->operands[0]->c.const_type == CONS_HIGH_ADDR );
        assert( ins->result->n.class == N_REGISTER );
        /* addis rd, imm(0) */
        GenOPIMM( 15, _NameRegTrans( ins->result ), ZERO_SINK,
            ins->operands[0]->c.lo.int_value & 0xffff );
        break;
    case G_MOVE_UI:
        /* a load of an unsigned 16-bit immediate */
        /* use addi rd, imm(0) */
        GenOPIMM( 14, _NameRegTrans( ins->result ), ZERO_SINK,
            ins->operands[0]->c.lo.int_value );
        break;
    case G_LEA:
        assert( ins->operands[0]->n.class == N_CONSTANT );
        assert( ins->result->n.class == N_REGISTER );
        switch( ins->operands[0]->c.const_type ) {
        case CONS_ABSOLUTE:
            // addi rd, imm(0)
            GenOPIMM( 14, _NameRegTrans( ins->result ), ZERO_SINK,
                ins->operands[0]->c.lo.int_value );
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
            getMemEncoding( ins->operands[0], &reg_mem, &mem_offset );
            // addi rd, imm(rs|sp)
            GenOPIMM( 14, _NameRegTrans( ins->result ), reg_mem, mem_offset );
            break;
        case N_MEMORY:
            assert( ins->result->n.class == N_REGISTER );
            OutReloc( symLabel( ins->operands[0] ), PPC_RELOC_TOC_OFFSET, 2 );
            // lwz rd, imm(rtoc)
            GenMEMINS( 32, _NameRegTrans( ins->result ), RTOC_REG_IDX, 0 );
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
                        _NameRegTrans( ins->operands[0] ),
                        _NameRegTrans( ins->operands[1] ),
                        _NameRegTrans( ins->result ) );
            break;
        }
        /* fall through */
    case G_BINARY:
        assert( ins->operands[0]->n.class == N_REGISTER );
        assert( ins->operands[1]->n.class == N_REGISTER );
        assert( ins->result->n.class == N_REGISTER );
        ops = FindOpcodes( ins );
        a = _NameRegTrans( ins->operands[0] );
        b = _NameRegTrans( ins->operands[1] );
        s = _NameRegTrans( ins->result );
        if( ins->head.opcode == OP_SUB
          && G( ins ) == G_BINARY ) {
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
        s = _NameRegTrans( ins->operands[0] );
        b = _NameRegTrans( ins->operands[1] );
        a = _NameRegTrans( ins->result );
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
            op2 = 31 - _FiveBits( ins->operands[1]->c.lo.int_value );
            b = _FiveBits( ins->operands[1]->c.lo.int_value );
            GenOPINS( 21, op2, _NameRegTrans( ins->result ), b,
                _NameRegTrans( ins->operands[0] ) );
            break;
        case OP_RSHIFT:
            if( _IsSigned( ins->type_class ) ) {
                GenOPINS( 31, 824, _NameRegTrans( ins->result ),
                    (reg_idx)ins->operands[1]->c.lo.int_value,
                    _NameRegTrans( ins->operands[0] ) );
            } else {
                // rlwinm dst,src,32-n,n,31
                b = _FiveBits( ins->operands[1]->c.lo.int_value );
                op2 = ( b << 5 ) | 31;
                b = 32 - b;
                GenOPINS( 21, op2, _NameRegTrans( ins->result ), b,
                    _NameRegTrans( ins->operands[0] ) );
            }
            break;
        default:
            ops = FindImmedOpcodes( ins );
            if( G( ins ) == G_BINARYS_IMM ) {
                s = _NameRegTrans( ins->operands[0] );
                a = _NameRegTrans( ins->result );
            } else {
                s = _NameRegTrans( ins->result );
                a = _NameRegTrans( ins->operands[0] );
            }
            GenOPIMM( ops[0], s, a, ins->operands[1]->c.lo.int_value );
            break;
        }
        break;
    case G_CMP_FP:
        assert( ins->operands[0]->n.class == N_REGISTER );
        assert( ins->operands[1]->n.class == N_REGISTER );
        GenCMP( 63, 32, _NameRegTrans( ins->operands[0] ),
            _NameRegTrans( ins->operands[1] ) );
        break;
    case G_CMP:
        assert( ins->operands[0]->n.class == N_REGISTER );
        assert( ins->operands[1]->n.class == N_REGISTER );
        op2 = 32;
        if( _IsSigned( ins->type_class ) ) {
            op2 = 0;
        }
        GenCMP( 31, op2, _NameRegTrans( ins->operands[0] ),
            _NameRegTrans( ins->operands[1] ) );
        break;
    case G_CMP_I:
        assert( ins->operands[0]->n.class == N_REGISTER );
        assert( ins->operands[1]->n.class == N_CONSTANT );
        op1 = 10;
        if( _IsSigned( ins->type_class ) ) {
            op1 = 11;
        }
        GenCMPIMM( op1, _NameRegTrans( ins->operands[0] ),
            ins->operands[1]->c.lo.int_value );
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
        a = _NameRegTrans( ins->operands[0] );
        s = _NameRegTrans( ins->result );
        switch( ins->head.opcode ) {
        case OP_NEGATE:
            // neg src -> dst
            GenOPINS( 31, 104, a, 0, s );
            break;
        case OP_COMPLEMENT:
            // not src -> dst
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
    case G_DEBUG:
        DbgBlkInfo( ins );
        break;
    case G_UNKNOWN:
        _Zoiks( ZOIKS_097 );
        break;
    default:
        _Zoiks( ZOIKS_028 );
        break;
    }
#ifdef DEVBUILD
    if( _IsTargetModel( CGSW_RISC_ASM_OUTPUT ) ) {
        DumpLiteral( "        " );
        DumpGen( ins->u.gen_table );
        DumpLiteral( " - " );
        DumpInsOnly( ins );
        DumpNL();
    }
#endif
}


void    GenObjCode( instruction *ins )
/************************************/
{
    Encode( ins );
    if( G( ins ) == G_CMP
      || G( ins ) == G_CMP_I
      || G( ins ) == G_CMP_FP ) {
        GenCondJump( ins );
    }
}

void    CodeLabel( label_handle label, unsigned alignment )
/*********************************************************/
{
    offset      loc;
    offset      modulus;

    assert( alignment <= MAX_ALIGNMENT );
    loc = AskLocation();
    modulus = loc % alignment;
    if( modulus != 0 ) {
        ObjBytes( Zeros, alignment - modulus );
    }
    OutLabel( label );
#ifdef DEVBUILD
    if( _IsTargetModel( CGSW_RISC_ASM_OUTPUT ) ) {
        DumpChar( 'L' );
        DumpPtr( label );
        DumpChar( ':' );
        DumpNL();
    }
#endif
}


void    CodeLineNumber( cg_linenum line, bool label )
/***************************************************/
{
#ifdef DEVBUILD
    if( _IsTargetModel( CGSW_RISC_ASM_OUTPUT ) ) {
        DumpLiteral( "Source Line: " );
        DumpInt( line );
        DumpNL();
    }
#endif
    OutLineNum( line, label );
}


void    GenJumpLabel( label_handle label )
/****************************************/
{
    GenBRANCH( 18, label, false, false );
#ifdef DEVBUILD
    if( _IsTargetModel( CGSW_RISC_ASM_OUTPUT ) ) {
        DumpLiteral( "JMP L" );
        DumpPtr( label );
        DumpNL();
    }
#endif
}

static void    GenJumpIf( instruction *ins, pointer label )
/*********************************************************/
{
    gen_opcode  *ops;

    ops = &BranchOpcodes[ins->head.opcode - FIRST_COMPARISON][0]; // fixme - floating point
    GenCONDBR( 16, ops[0], ops[1], label );
#ifdef DEVBUILD
    if( _IsTargetModel( CGSW_RISC_ASM_OUTPUT ) ) {
        DumpLiteral( "Jcc L" );
        DumpPtr( label );
        DumpNL();
    }
#endif
}


void    GenKillLabel( label_handle lbl )
/**************************************/
{
    _ValidLbl( lbl );
    if( !_TstStatus( lbl, CODELABEL ) )
        return;
    // if( _TstStatus( lbl, OWL_OWNED ) )
    //     return;
    TryScrapLabel( lbl );
}


void    GenCallLabel( pointer label )
/***********************************/
{
    /* unused parameters */ (void)label;
}


void    GenLabelReturn( void )
/****************************/
{
}


byte    ReverseCondition( byte cond )
/***********************************/
{
    return( cond );
}


static label_handle LocateLabel( instruction *ins, byte dst_idx )
/***************************************************************/
{
    if( dst_idx == NO_JUMP )
        return( NULL );
    return( InsBlock( ins->head.next )->edge[dst_idx].destination.u.lbl );
}


void    GenCondJump( instruction *cond )
/**************************************/
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
