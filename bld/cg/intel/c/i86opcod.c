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


#include "standard.h"
#include "coderep.h"
#include "opcodes.h"
#include "vergen.h"
#include "pccode.h"
#include "ocentry.h"
#include "pcencode.h"

/* 8086 Code Generation Tables & Stuff*/

/* instruction tables*/

static opcode_defs G1Index[] = {
        OP_ADD,
        OP_EXT_ADD,
        OP_SUB,
        OP_EXT_SUB,
        OP_AND,
        OP_OR,
        OP_XOR,
        OP_BIT_TEST_TRUE,
        OP_BIT_TEST_FALSE,
        OP_NOP };               /* matches compares*/

static opcode G1Opcodes[] = {
/*       RC      AC      MC      RR2     RM2     MR2*/
       0xc080, 0x4,    0x0080, 0xc000, 0x02,    0,      /* ADD*/
       0xd080, 0x14,   0x1080, 0xc010, 0x12,   0x10,    /* ADD_EXT*/
       0xe880, 0x2c,   0x2880, 0xc028, 0x2a,   0x28,    /* SUB*/
       0xd880, 0x1c,    M_SBB, 0xc018, 0x1a,   0x18,    /* SUB_EXT*/
       0xe080, 0x24,   0x2080, 0xc020, 0x22,   0x20,    /* AND*/
       0xc880, 0x0c,   0x0880, 0xc008, 0x0a,   0x08,    /* OR*/
       0xf080, 0x34,   0x3080, 0xc030, 0x32,   0x30,    /* XOR*/
       0xc0f6, 0xa8,   0x00f6,  M_TESTRR,0x84, 0x84,    /* BIT_TEST_TRUE*/
       0xc0f6, 0xa8,   0x00f6,  M_TESTRR,0x84, 0x84,    /* BIT_TEST_FALSE*/
       0xf880, 0x3c,   0x3880, 0xc038, 0x3a,   0x38 };  /* CMP*/

static opcode_defs G2Index[] = {
        OP_PUSH, OP_POP, OP_ADD, OP_SUB };

static opcode G2Opcodes[] = {
        M_PUSH, M_POP, 0x40,   0x48 };

static opcode_defs G3Index[] = {
        OP_ADD,
        OP_SUB,
        OP_MUL,
        OP_EXT_MUL,
        OP_DIV,
        OP_MOD,
        OP_NEGATE,
        OP_COMPLEMENT,
        OP_PUSH,
        OP_POP };

static opcode G3Opcodes[] = {
/*       R1      M1      R2      M2*/
       0xc0fe, 0x00fe, 0xc0fe, 0x00fe,  /* INC*/
       0xc8fe, 0x08fe, 0xc8fe, 0x00fe,  /* DEC*/
       0xe0f6, 0x20f6, 0xe0f6, 0x20f6,  /* MUL*/
       0xe0f6, 0x20f6, 0xe0f6, 0x20f6,  /* EXT_MUL*/
       0xf0f6, 0x30f6, 0xf0f6, 0x30f6,  /* DIV*/
       0xf0f6, 0x30f6, 0xf0f6, 0x30f6,  /* MOD*/
       0xd8f6,  M_NEG, 0xd8f6,  M_NEG,  /* NEGATE*/
       0xd0f6,  M_NOT, 0xd0f6,  M_NOT,  /* COMPLEMENT*/
       0xf0ff, 0x30ff, 0xf0ff, 0x30ff,  /* PUSH*/
       0xc08f, 0x008f, 0xc08f, 0x008f };/* POP*/

static opcode_defs G4Index[] = {
        OP_RSHIFT,
        OP_LSHIFT };

static opcode G4Opcodes[] = {
/*      1SHIFT  R1SHIFT NSHIFT  RNSHIFT CLSHIFT RCLSHIFT*/
        M_SHR1, M_SHR1,0x28c0, 0x28c0, 0x28d2, 0x28d2,   /* OP_RSHIFT*/
        M_SHL1, M_SHL1,0x20c0, 0x20c0, 0x20d2, 0x20d2 }; /* OP_LSHIFT*/

static opcode G5Opcodes[] = {
/*       RR1     RM1     MR1*/
       0xc088, 0x008a, 0x0088 };

static opcode G6Opcodes[] = {
/*       LEA   LDSES   MS1     SM1     RS      SR      MOVMC   MADDR   TEST*/
        M_LEA, 0xc4,   0x8c,   0x8e,   0xc08c, 0xc08e, 0xc6,   0xc7,    M_TESTRR
};

static opcode G7Opcodes[] = {
/*       SEGR1         MOVAM   MOVMA   MOVRC   RADDR   C1*/
        M_PUSHSEG,     0xa0,   0xa2,   0xb0,   0xb8,   0x68  };

static opcode_defs G8Index[] = {
        OP_ADD,
        OP_MUL,
        OP_SUB,
        OP_DIV };

static opcode G8Opcodes[] = {
/* RRFBIN  RNFBIN  RRFBINP RNFBINP RRFBIND RNFBIND MRFBIN  MNFBIN*/
 0xc0d8, 0xc0d8, 0xc0de, 0xc0de, 0xc0dc, 0xc0dc, 0x00d8, 0x00d8, /* OP_ADD*/
 0xc8d8, 0xc8d8, 0xc8de, 0xc8de, 0xc8dc, 0xc8dc, 0x08d8, 0x08d8, /* OP_MUL*/
 0xe8d8, 0xe0d8, 0xe0de, 0xe8de, 0xe0dc, 0xe8dc, 0x28d8, 0x20d8, /* OP_SUB*/
 0xf8d8, 0xf0d8, 0xf0de, 0xf8de, 0xf0dc, 0xf8dc, 0x38d8, 0x30d8};/* OP_DIV*/

static opcode G9Opcodes[] = {
/*  MFLD   RFLD   IFLD   MFST   RFST   IFST   FCHS   FRNDINT FLDZ  FLD1*/
  0x00d9,0xc0d9,0x28df,0x18d9,0xd8dd,0x38df,0xe0d9,0xfcd9,0xeed9,0xe8d9,
/*  FINIT  FCOMPP MCOMP  RCOMP  MCOM   RCOM   MFSTNP RFSTNP FTST*/
  0xe3db,0xd9de,0x18d8,0xd8d8,0x10d8,0xd0d8,0x10d9,0xd0dd,0xe4d9 };

static opcode_defs NoIndex[] = {
        OP_NOP };               /* i.e. will match anything*/

extern pccode_def PCCodeTable[] = {

/* low    width                  opcode_table    opcode_list     flags*/
G_GROUP1,G_GROUP2-G_GROUP1,     &G1Opcodes,     &G1Index,   BYTE_WORD,
G_GROUP2,G_GROUP3-G_GROUP2,     &G2Opcodes,     &G2Index,   BYTE_OPCODE,
G_GROUP3,G_GROUP4-G_GROUP3,     &G3Opcodes,     &G3Index,BYTE_WORD|SIGN_UNSIGN,
G_GROUP4,G_GROUP5-G_GROUP4,     &G4Opcodes,     &G4Index,SIGN_UNSIGN|BYTE_WORD,
G_GROUP5,G_GROUP6-G_GROUP5,     &G5Opcodes,     &NoIndex,   BYTE_WORD,
G_GROUP6,G_GROUP7-G_GROUP6,     &G6Opcodes,     &NoIndex,   0,
G_GROUP7,G_GROUP8-G_GROUP7,     &G7Opcodes,     &NoIndex,   BYTE_OPCODE,
G_GROUP8,G_GROUP9-G_GROUP8,     &G8Opcodes,     &G8Index,   NEED_WAIT,
G_GROUP9,G_OTHER-G_GROUP9,      &G9Opcodes,     &NoIndex,   NEED_WAIT,
G_OTHER, 0,                     NULL,           NULL,       0 };
