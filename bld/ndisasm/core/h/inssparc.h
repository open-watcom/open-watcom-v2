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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#define op1(a)          ((a##ul)<<30)
#define op2(a,b)        (op1(a)+((b##ul)<<22))
#define op3(a,b)        (op1(a)+((b##ul)<<19))
#define opb(a,b,c)      (op1(a)+((b##ul)<<25)+((c##ul)<<22))
#define OP0             (op1(0x3))
#define OP2             (op2(0x3,0x7))
#define OP3             (op3(0x3,0x3f))
#define OPB             (opb(0x3,0x0f,0x07))

/*
      Idx,              Name,           Opcode,         Mask,           Handler
*/
inspick( ldsb,          "ldsb",         op3(3,0x09),    OP3,            SPARCMem )
inspick( ldsh,          "ldsh",         op3(3,0x0a),    OP3,            SPARCMem )
inspick( ldub,          "ldub",         op3(3,0x01),    OP3,            SPARCMem )
inspick( lduh,          "lduh",         op3(3,0x02),    OP3,            SPARCMem )
inspick( ld,            "ld",           op3(3,0x00),    OP3,            SPARCMem )
inspick( ldd,           "ldd",          op3(3,0x03),    OP3,            SPARCMem )
inspick( ldsba,         "ldsba",        op3(3,0x19),    OP3,            SPARCMem )
inspick( ldsha,         "ldsha",        op3(3,0x1a),    OP3,            SPARCMem )
inspick( lduba,         "lduba",        op3(3,0x11),    OP3,            SPARCMem )
inspick( lduha,         "lduha",        op3(3,0x12),    OP3,            SPARCMem )
inspick( lda,           "lda",          op3(3,0x10),    OP3,            SPARCMem )
inspick( ldda,          "ldda",         op3(3,0x13),    OP3,            SPARCMem )

inspick( ldf,           "ld",           op3(3,0x20),    OP3,            SPARCMemF )
inspick( lddf,          "ldd",          op3(3,0x23),    OP3,            SPARCMemF )
inspick( ldfsr,         "ld",           op3(3,0x21),    OP3,            SPARCMemF )

inspick( ldc,           "ld",           op3(3,0x30),    OP3,            SPARCMemC )
inspick( lddc,          "ldd",          op3(3,0x33),    OP3,            SPARCMemC )
inspick( ldcsr,         "ld",           op3(3,0x31),    OP3,            SPARCMemC )

inspick( stb,           "stb",          op3(3,0x05),    OP3,            SPARCMem )
inspick( sth,           "sth",          op3(3,0x06),    OP3,            SPARCMem )
inspick( st,            "st",           op3(3,0x04),    OP3,            SPARCMem )
inspick( std,           "std",          op3(3,0x07),    OP3,            SPARCMem )
inspick( stba,          "stba",         op3(3,0x15),    OP3,            SPARCMem )
inspick( stha,          "stha",         op3(3,0x16),    OP3,            SPARCMem )
inspick( sta,           "sta",          op3(3,0x14),    OP3,            SPARCMem )
inspick( stda,          "stda",         op3(3,0x17),    OP3,            SPARCMem )

inspick( stf,           "st",           op3(3,0x24),    OP3,            SPARCMemF )
inspick( stdf,          "std",          op3(3,0x27),    OP3,            SPARCMemF )
inspick( stfsr,         "st",           op3(3,0x25),    OP3,            SPARCMemF )
inspick( stdfq,         "std",          op3(3,0x26),    OP3,            SPARCMemF )

inspick( stc,           "st",           op3(3,0x34),    OP3,            SPARCMemC )
inspick( stdc,          "std",          op3(3,0x37),    OP3,            SPARCMemC )
inspick( stcsr,         "st",           op3(3,0x35),    OP3,            SPARCMemC )
inspick( stdcq,         "std",          op3(3,0x36),    OP3,            SPARCMemC )

inspick( ldstub,        "ldstub",       op3(3,0x0d),    OP3,            SPARCMem )
inspick( ldstuba,       "ldstuba",      op3(3,0x1d),    OP3,            SPARCMem )

inspick( swap,          "swap",         op3(3,0x0f),    OP3,            SPARCMem )
inspick( swapa,         "swapa",        op3(3,0x1f),    OP3,            SPARCMem )

inspick( sethi,         "sethi",        op2(0,0x04),    OP2,            SPARCSetHi )
// nop handled by inshook to provide pseudonym

inspick( and,           "and",          op3(2,0x01),    OP3,            SPARCOp3 )
inspick( andcc,         "andcc",        op3(2,0x11),    OP3,            SPARCOp3 )
inspick( andn,          "andn",         op3(2,0x05),    OP3,            SPARCOp3 )
inspick( andncc,        "andncc",       op3(2,0x15),    OP3,            SPARCOp3 )
inspick( or,            "or",           op3(2,0x02),    OP3,            SPARCOp3 )
inspick( orcc,          "orcc",         op3(2,0x12),    OP3,            SPARCOp3 )
inspick( orn,           "orn",          op3(2,0x06),    OP3,            SPARCOp3 )
inspick( orncc,         "orncc",        op3(2,0x16),    OP3,            SPARCOp3 )
inspick( xor,           "xor",          op3(2,0x03),    OP3,            SPARCOp3 )
inspick( xorcc,         "xorcc",        op3(2,0x13),    OP3,            SPARCOp3 )
inspick( xnor,          "xnor",         op3(2,0x07),    OP3,            SPARCOp3 )
inspick( xnorcc,        "xnorcc",       op3(2,0x17),    OP3,            SPARCOp3 )

inspick( sll,           "sll",          op3(2,0x25),    OP3,            SPARCOp3 )
inspick( srl,           "srl",          op3(2,0x26),    OP3,            SPARCOp3 )
inspick( sra,           "sra",          op3(2,0x27),    OP3,            SPARCOp3 )

inspick( add,           "add",          op3(2,0x00),    OP3,            SPARCOp3 )
inspick( addcc,         "addcc",        op3(2,0x10),    OP3,            SPARCOp3 )
inspick( addx,          "addx",         op3(2,0x08),    OP3,            SPARCOp3 )
inspick( addxcc,        "addxcc",       op3(2,0x18),    OP3,            SPARCOp3 )

inspick( taddcc,        "taddcc",       op3(2,0x20),    OP3,            SPARCOp3 )
inspick( taddcctv,      "taddcctv",     op3(2,0x22),    OP3,            SPARCOp3 )

inspick( sub,           "sub",          op3(2,0x04),    OP3,            SPARCOp3 )
inspick( subcc,         "subcc",        op3(2,0x14),    OP3,            SPARCOp3 )
inspick( subx,          "subx",         op3(2,0x0c),    OP3,            SPARCOp3 )
inspick( subxcc,        "subxcc",       op3(2,0x1c),    OP3,            SPARCOp3 )

inspick( tsubcc,        "tsubcc",       op3(2,0x21),    OP3,            SPARCOp3 )
inspick( tsubcctv,      "tsubcctv",     op3(2,0x23),    OP3,            SPARCOp3 )

inspick( mulscc,        "mulscc",       op3(2,0x24),    OP3,            SPARCOp3 )

inspick( umul,          "umul",         op3(2,0x0a),    OP3,            SPARCOp3 )
inspick( smul,          "smul",         op3(2,0x0b),    OP3,            SPARCOp3 )
inspick( umulcc,        "umulcc",       op3(2,0x1a),    OP3,            SPARCOp3 )
inspick( smulcc,        "smulcc",       op3(2,0x1b),    OP3,            SPARCOp3 )

inspick( udiv,          "udiv",         op3(2,0x0e),    OP3,            SPARCOp3 )
inspick( sdiv,          "sdiv",         op3(2,0x0f),    OP3,            SPARCOp3 )
inspick( udivcc,        "udivcc",       op3(2,0x1e),    OP3,            SPARCOp3 )
inspick( sdivcc,        "sdivcc",       op3(2,0x1f),    OP3,            SPARCOp3 )

inspick( save,          "save",         op3(2,0x3c),    OP3,            SPARCOp3 )
inspick( restore,       "restore",      op3(2,0x3d),    OP3,            SPARCOp3 )

inspick( bn,            "bn",           opb(0,0x0,0x2), OPB,            SPARCBranch )
inspick( be,            "be",           opb(0,0x1,0x2), OPB,            SPARCBranch )
inspick( ble,           "ble",          opb(0,0x2,0x2), OPB,            SPARCBranch )
inspick( bl,            "bl",           opb(0,0x3,0x2), OPB,            SPARCBranch )
inspick( bleu,          "bleu",         opb(0,0x4,0x2), OPB,            SPARCBranch )
inspick( bcs,           "bcs",          opb(0,0x5,0x2), OPB,            SPARCBranch )
inspick( bneg,          "bneg",         opb(0,0x6,0x2), OPB,            SPARCBranch )
inspick( bvs,           "bvs",          opb(0,0x7,0x2), OPB,            SPARCBranch )
inspick( ba,            "ba",           opb(0,0x8,0x2), OPB,            SPARCBranch )
inspick( bne,           "bne",          opb(0,0x9,0x2), OPB,            SPARCBranch )
inspick( bg,            "be",           opb(0,0xa,0x2), OPB,            SPARCBranch )
inspick( bge,           "bge",          opb(0,0xb,0x2), OPB,            SPARCBranch )
inspick( bgu,           "bgu",          opb(0,0xc,0x2), OPB,            SPARCBranch )
inspick( bcc,           "bcc",          opb(0,0xd,0x2), OPB,            SPARCBranch )
inspick( bpos,          "bpos",         opb(0,0xe,0x2), OPB,            SPARCBranch )
inspick( bvc,           "bvc",          opb(0,0xf,0x2), OPB,            SPARCBranch )

inspick( fbn,           "fbn",          opb(0,0x0,0x6), OPB,            SPARCBranch )
inspick( fbne,          "fbne",         opb(0,0x1,0x6), OPB,            SPARCBranch )
inspick( fblg,          "fblg",         opb(0,0x2,0x6), OPB,            SPARCBranch )
inspick( fbul,          "fbul",         opb(0,0x3,0x6), OPB,            SPARCBranch )
inspick( fbl,           "fbl",          opb(0,0x4,0x6), OPB,            SPARCBranch )
inspick( fbug,          "fbug",         opb(0,0x5,0x6), OPB,            SPARCBranch )
inspick( fbg,           "fbg",          opb(0,0x6,0x6), OPB,            SPARCBranch )
inspick( fbu,           "fbu",          opb(0,0x7,0x6), OPB,            SPARCBranch )
inspick( fba,           "fba",          opb(0,0x8,0x6), OPB,            SPARCBranch )
inspick( fbe,           "fbe",          opb(0,0x9,0x6), OPB,            SPARCBranch )
inspick( fbue,          "fbue",         opb(0,0xa,0x6), OPB,            SPARCBranch )
inspick( fbge,          "fbge",         opb(0,0xb,0x6), OPB,            SPARCBranch )
inspick( fbuge,         "fbuge",        opb(0,0xc,0x6), OPB,            SPARCBranch )
inspick( fble,          "fble",         opb(0,0xd,0x6), OPB,            SPARCBranch )
inspick( fbule,         "fbule",        opb(0,0xe,0x6), OPB,            SPARCBranch )
inspick( fbo,           "fbo",          opb(0,0xf,0x6), OPB,            SPARCBranch )

inspick( cbn,           "cbn",          opb(0,0x0,0x7), OPB,            SPARCBranch )
inspick( cb123,         "cb123",        opb(0,0x1,0x7), OPB,            SPARCBranch )
inspick( cb12,          "cb12",         opb(0,0x2,0x7), OPB,            SPARCBranch )
inspick( cb13,          "cb13",         opb(0,0x3,0x7), OPB,            SPARCBranch )
inspick( cb1,           "cb1",          opb(0,0x4,0x7), OPB,            SPARCBranch )
inspick( cb23,          "cb23",         opb(0,0x5,0x7), OPB,            SPARCBranch )
inspick( cb2,           "cb2",          opb(0,0x6,0x7), OPB,            SPARCBranch )
inspick( cb3,           "cb3",          opb(0,0x7,0x7), OPB,            SPARCBranch )
inspick( cba,           "cba",          opb(0,0x8,0x7), OPB,            SPARCBranch )
inspick( cb0,           "cb0",          opb(0,0x9,0x7), OPB,            SPARCBranch )
inspick( cb03,          "cb03",         opb(0,0xa,0x7), OPB,            SPARCBranch )
inspick( cb02,          "cb02",         opb(0,0xb,0x7), OPB,            SPARCBranch )
inspick( cb023,         "cb023",        opb(0,0xc,0x7), OPB,            SPARCBranch )
inspick( cb01,          "cb01",         opb(0,0xd,0x7), OPB,            SPARCBranch )
inspick( cb013,         "cb013",        opb(0,0xe,0x7), OPB,            SPARCBranch )
inspick( cb012,         "cb012",        opb(0,0xf,0x7), OPB,            SPARCBranch )

inspick( call,          "call",         op1(1),         OP0,            SPARCCall )

inspick( jmpl,          "jmpl",         op3(2,0x38),    OP3,            SPARCMem )

// note: have to special-case this in InsHook so that we don't print %g0 operand
inspick( rett,          "rett",         op3(2,0x39),    OP3,            SPARCMem )
