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


#include "as.h"

static bool insErrFlag = FALSE;    // to tell whether we had problems or not

#define INS( a, b, c, d, e ) { a, b, c, d, e, NULL }

ins_table AlphaTable[] = {
 /* INS( name,      opcode, funccode,   template,               method ), */
 // Memory Format Instructions
    INS( "lda",     0x08,   0x0000,     IT_MEMORY_LDA,          ENUM_NONE ),
    INS( "ldah",    0x09,   0x0000,     IT_MEMORY_ALL,          ENUM_NONE ),
    INS( "ldf",     0x20,   0x0000,     IT_FP_MEMORY_ALL,       ENUM_NONE ),
    INS( "ldg",     0x21,   0x0000,     IT_FP_MEMORY_ALL,       ENUM_NONE ),
    INS( "ldl",     0x28,   0x0000,     IT_MEMORY_ALL,          ENUM_NONE ),
    INS( "ldl_l",   0x2A,   0x0000,     IT_MEMORY_ALL,          ENUM_NONE ),
    INS( "ldq",     0x29,   0x0000,     IT_MEMORY_ALL,          ENUM_NONE ),
    INS( "ldq_l",   0x2B,   0x0000,     IT_MEMORY_ALL,          ENUM_NONE ),
    INS( "ldq_u",   0x0B,   0x0000,     IT_MEMORY_ALL,          ENUM_NONE ),
    INS( "lds",     0x22,   0x0000,     IT_FP_MEMORY_ALL,       ENUM_NONE ),
    INS( "ldt",     0x23,   0x0000,     IT_FP_MEMORY_ALL,       ENUM_NONE ),
    INS( "stf",     0x24,   0x0000,     IT_FP_MEMORY_ALL,       ENUM_NONE ),
    INS( "stg",     0x25,   0x0000,     IT_FP_MEMORY_ALL,       ENUM_NONE ),
    INS( "stl",     0x2C,   0x0000,     IT_MEMORY_ALL,          ENUM_NONE ),
    INS( "stl_c",   0x2E,   0x0000,     IT_MEMORY_ALL,          ENUM_NONE ),
    INS( "stq",     0x2D,   0x0000,     IT_MEMORY_ALL,          ENUM_NONE ),
    INS( "stq_c",   0x2F,   0x0000,     IT_MEMORY_ALL,          ENUM_NONE ),
    INS( "stq_u",   0x0F,   0x0000,     IT_MEMORY_ALL,          ENUM_NONE ),
    INS( "sts",     0x26,   0x0000,     IT_FP_MEMORY_ALL,       ENUM_NONE ),
    INS( "stt",     0x27,   0x0000,     IT_FP_MEMORY_ALL,       ENUM_NONE ),
 // Memory Format Instructions with a function code
    INS( "fetch",   0x18,   0x8000,     IT_MEMORY_B,            ENUM_NONE ),
    INS( "fetch_m", 0x18,   0xA000,     IT_MEMORY_B,            ENUM_NONE ),
    INS( "mb",      0x18,   0x4000,     IT_MEMORY_NONE,         ENUM_NONE ),
    INS( "wmb",     0x18,   0x4400,     IT_MEMORY_NONE,         ENUM_NONE ),
    INS( "rc",      0x18,   0xE000,     IT_MEMORY_A,            ENUM_NONE ),
    INS( "rpcc",    0x18,   0xC000,     IT_MEMORY_A,            ENUM_NONE ),
    INS( "rs",      0x18,   0xF000,     IT_MEMORY_A,            ENUM_NONE ),
    INS( "trapb",   0x18,   0x0000,     IT_MEMORY_NONE,         ENUM_NONE ),
    INS( "excb",    0x18,   0x0400,     IT_MEMORY_NONE,         ENUM_NONE ),
 // Memory Branch Instructions
    INS( "jmp",     0x1A,   0x0000,     IT_MEMORY_JUMP,         ENUM_NONE ),
    INS( "jsr",     0x1A,   0x0001,     IT_MEMORY_JUMP,         ENUM_NONE ),
    INS( "jsr_coroutine", 0x1A, 0x0003, IT_RET,                 ENUM_NONE ),
    INS( "ret",     0x1A,   0x0002,     IT_RET,                 ENUM_NONE ),
 // Branch Format Instructions
    INS( "br",      0x30,   0x0000,     IT_BR,                  ENUM_NONE ),
    INS( "fbeq",    0x31,   0x0000,     IT_FP_BRANCH,           ENUM_NONE ),
    INS( "fblt",    0x32,   0x0000,     IT_FP_BRANCH,           ENUM_NONE ),
    INS( "fble",    0x33,   0x0000,     IT_FP_BRANCH,           ENUM_NONE ),
    INS( "bsr",     0x34,   0x0000,     IT_BRANCH,              ENUM_NONE ),
    INS( "fbne",    0x35,   0x0000,     IT_FP_BRANCH,           ENUM_NONE ),
    INS( "fbge",    0x36,   0x0000,     IT_FP_BRANCH,           ENUM_NONE ),
    INS( "fbgt",    0x37,   0x0000,     IT_FP_BRANCH,           ENUM_NONE ),
    INS( "blbc",    0x38,   0x0000,     IT_BRANCH,              ENUM_NONE ),
    INS( "beq",     0x39,   0x0000,     IT_BRANCH,              ENUM_NONE ),
    INS( "blt",     0x3A,   0x0000,     IT_BRANCH,              ENUM_NONE ),
    INS( "ble",     0x3B,   0x0000,     IT_BRANCH,              ENUM_NONE ),
    INS( "blbs",    0x3C,   0x0000,     IT_BRANCH,              ENUM_NONE ),
    INS( "bne",     0x3D,   0x0000,     IT_BRANCH,              ENUM_NONE ),
    INS( "bge",     0x3E,   0x0000,     IT_BRANCH,              ENUM_NONE ),
    INS( "bgt",     0x3F,   0x0000,     IT_BRANCH,              ENUM_NONE ),
 // Operate Format Instructions
    INS( "addl",    0x10,   0x0000,     IT_OPERATE,             ENUM_OF_ADDL ),
    INS( "addq",    0x10,   0x0020,     IT_OPERATE,             ENUM_OF_ADDL ),
    INS( "cmpbge",  0x10,   0x000F,     IT_OPERATE,             ENUM_NONE ),
    INS( "cmpeq",   0x10,   0x002D,     IT_OPERATE,             ENUM_NONE ),
    INS( "cmple",   0x10,   0x006D,     IT_OPERATE,             ENUM_NONE ),
    INS( "cmplt",   0x10,   0x004D,     IT_OPERATE,             ENUM_NONE ),
    INS( "cmpule",  0x10,   0x003D,     IT_OPERATE,             ENUM_NONE ),
    INS( "cmpult",  0x10,   0x001D,     IT_OPERATE,             ENUM_NONE ),
    INS( "subl",    0x10,   0x0009,     IT_OPERATE,             ENUM_OF_ADDL ),
    INS( "subq",    0x10,   0x0029,     IT_OPERATE,             ENUM_OF_ADDL ),
    INS( "s4addl",  0x10,   0x0002,     IT_OPERATE,             ENUM_NONE ),
    INS( "s4addq",  0x10,   0x0022,     IT_OPERATE,             ENUM_NONE ),
    INS( "s4subl",  0x10,   0x000B,     IT_OPERATE,             ENUM_NONE ),
    INS( "s4subq",  0x10,   0x002B,     IT_OPERATE,             ENUM_NONE ),
    INS( "s8addl",  0x10,   0x0012,     IT_OPERATE,             ENUM_NONE ),
    INS( "s8addq",  0x10,   0x0032,     IT_OPERATE,             ENUM_NONE ),
    INS( "s8subl",  0x10,   0x001B,     IT_OPERATE,             ENUM_NONE ),
    INS( "s8subq",  0x10,   0x003B,     IT_OPERATE,             ENUM_NONE ),
    INS( "and",     0x11,   0x0000,     IT_OPERATE,             ENUM_NONE ),
    INS( "bic",     0x11,   0x0008,     IT_OPERATE,             ENUM_NONE ),
    INS( "bis",     0x11,   0x0020,     IT_OPERATE,             ENUM_NONE ),
    INS( "cmoveq",  0x11,   0x0024,     IT_OPERATE,             ENUM_NONE ),
    INS( "cmovlbc", 0x11,   0x0016,     IT_OPERATE,             ENUM_NONE ),
    INS( "cmovlbs", 0x11,   0x0014,     IT_OPERATE,             ENUM_NONE ),
    INS( "cmovge",  0x11,   0x0046,     IT_OPERATE,             ENUM_NONE ),
    INS( "cmovgt",  0x11,   0x0066,     IT_OPERATE,             ENUM_NONE ),
    INS( "cmovle",  0x11,   0x0064,     IT_OPERATE,             ENUM_NONE ),
    INS( "cmovlt",  0x11,   0x0044,     IT_OPERATE,             ENUM_NONE ),
    INS( "cmovne",  0x11,   0x0026,     IT_OPERATE,             ENUM_NONE ),
    INS( "eqv",     0x11,   0x0048,     IT_OPERATE,             ENUM_NONE ),
    INS( "ornot",   0x11,   0x0028,     IT_OPERATE,             ENUM_NONE ),
    INS( "xor",     0x11,   0x0040,     IT_OPERATE,             ENUM_NONE ),
    INS( "extbl",   0x12,   0x0006,     IT_OPERATE,             ENUM_NONE ),
    INS( "extlh",   0x12,   0x006A,     IT_OPERATE,             ENUM_NONE ),
    INS( "extll",   0x12,   0x0026,     IT_OPERATE,             ENUM_NONE ),
    INS( "extqh",   0x12,   0x007A,     IT_OPERATE,             ENUM_NONE ),
    INS( "extql",   0x12,   0x0036,     IT_OPERATE,             ENUM_NONE ),
    INS( "extwh",   0x12,   0x005A,     IT_OPERATE,             ENUM_NONE ),
    INS( "extwl",   0x12,   0x0016,     IT_OPERATE,             ENUM_NONE ),
    INS( "insbl",   0x12,   0x000B,     IT_OPERATE,             ENUM_NONE ),
    INS( "inslh",   0x12,   0x0067,     IT_OPERATE,             ENUM_NONE ),
    INS( "insll",   0x12,   0x002B,     IT_OPERATE,             ENUM_NONE ),
    INS( "insqh",   0x12,   0x0077,     IT_OPERATE,             ENUM_NONE ),
    INS( "insql",   0x12,   0x003B,     IT_OPERATE,             ENUM_NONE ),
    INS( "inswh",   0x12,   0x0057,     IT_OPERATE,             ENUM_NONE ),
    INS( "inswl",   0x12,   0x001B,     IT_OPERATE,             ENUM_NONE ),
    INS( "mskbl",   0x12,   0x0002,     IT_OPERATE,             ENUM_NONE ),
    INS( "msklh",   0x12,   0x0062,     IT_OPERATE,             ENUM_NONE ),
    INS( "mskll",   0x12,   0x0022,     IT_OPERATE,             ENUM_NONE ),
    INS( "mskqh",   0x12,   0x0072,     IT_OPERATE,             ENUM_NONE ),
    INS( "mskql",   0x12,   0x0032,     IT_OPERATE,             ENUM_NONE ),
    INS( "mskwh",   0x12,   0x0052,     IT_OPERATE,             ENUM_NONE ),
    INS( "mskwl",   0x12,   0x0012,     IT_OPERATE,             ENUM_NONE ),
    INS( "sll",     0x12,   0x0039,     IT_OPERATE,             ENUM_NONE ),
    INS( "sra",     0x12,   0x003C,     IT_OPERATE,             ENUM_NONE ),
    INS( "srl",     0x12,   0x0034,     IT_OPERATE,             ENUM_NONE ),
    INS( "zap",     0x12,   0x0030,     IT_OPERATE,             ENUM_NONE ),
    INS( "zapnot",  0x12,   0x0031,     IT_OPERATE,             ENUM_NONE ),
    INS( "mull",    0x13,   0x0000,     IT_OPERATE,             ENUM_OF_ADDL ),
    INS( "mulq",    0x13,   0x0020,     IT_OPERATE,             ENUM_OF_ADDL ),
    INS( "umulh",   0x13,   0x0030,     IT_OPERATE,             ENUM_NONE ),
 // Floating-Point Operate Format Instructions - Data Type Independent
    INS( "cpys",    0x17,   0x0020,     IT_FP_OPERATE,          ENUM_NONE ),
    INS( "cpyse",   0x17,   0x0022,     IT_FP_OPERATE,          ENUM_NONE ),
    INS( "cpysn",   0x17,   0x0021,     IT_FP_OPERATE,          ENUM_NONE ),
    INS( "cvtlq",   0x17,   0x0010,     IT_FP_CONVERT,          ENUM_NONE ),
    INS( "cvtql",   0x17,   0x0030,     IT_FP_CONVERT,          ENUM_DTI_CVTQL ),
    INS( "fcmoveq", 0x17,   0x002A,     IT_FP_OPERATE,          ENUM_NONE ),
    INS( "fcmovge", 0x17,   0x002D,     IT_FP_OPERATE,          ENUM_NONE ),
    INS( "fcmovgt", 0x17,   0x002F,     IT_FP_OPERATE,          ENUM_NONE ),
    INS( "fcmovle", 0x17,   0x002E,     IT_FP_OPERATE,          ENUM_NONE ),
    INS( "fcmovlt", 0x17,   0x002C,     IT_FP_OPERATE,          ENUM_NONE ),
    INS( "fcmovne", 0x17,   0x002B,     IT_FP_OPERATE,          ENUM_NONE ),
    INS( "mf_fpcr", 0x17,   0x0025,     IT_MT_MF_FPCR,          ENUM_NONE ),
    INS( "mt_fpcr", 0x17,   0x0024,     IT_MT_MF_FPCR,          ENUM_NONE ),
 // Floating-Point Operate Format Instructions - IEEE
    INS( "adds",    0x16,   0x0080,     IT_FP_OPERATE,          ENUM_IEEE_ADDS ),
    INS( "addt",    0x16,   0x00A0,     IT_FP_OPERATE,          ENUM_IEEE_ADDS ),
    INS( "cmpteq",  0x16,   0x00A5,     IT_FP_OPERATE,          ENUM_IEEE_CMPTEQ ),
    INS( "cmptlt",  0x16,   0x00A6,     IT_FP_OPERATE,          ENUM_IEEE_CMPTEQ ),
    INS( "cmptle",  0x16,   0x00A7,     IT_FP_OPERATE,          ENUM_IEEE_CMPTEQ ),
    INS( "cmptun",  0x16,   0x00A4,     IT_FP_OPERATE,          ENUM_IEEE_CMPTEQ ),
    INS( "cvtqs",   0x16,   0x00BC,     IT_FP_CONVERT,          ENUM_IEEE_CVTQS ),
    INS( "cvtqt",   0x16,   0x00BE,     IT_FP_CONVERT,          ENUM_IEEE_CVTQS ),
    INS( "cvtts",   0x16,   0x00AC,     IT_FP_CONVERT,          ENUM_IEEE_ADDS ),
    INS( "divs",    0x16,   0x0083,     IT_FP_OPERATE,          ENUM_IEEE_ADDS ),
    INS( "divt",    0x16,   0x00A3,     IT_FP_OPERATE,          ENUM_IEEE_ADDS ),
    INS( "muls",    0x16,   0x0082,     IT_FP_OPERATE,          ENUM_IEEE_ADDS ),
    INS( "mult",    0x16,   0x00A2,     IT_FP_OPERATE,          ENUM_IEEE_ADDS ),
    INS( "subs",    0x16,   0x0081,     IT_FP_OPERATE,          ENUM_IEEE_ADDS ),
    INS( "subt",    0x16,   0x00A1,     IT_FP_OPERATE,          ENUM_IEEE_ADDS ),
    INS( "cvttq",   0x16,   0x00AF,     IT_FP_CONVERT,          ENUM_IEEE_CVTTQ ),
 // Floating-Point Operate Format Instructions - VAX
    INS( "addf",    0x15,   0x0080,     IT_FP_OPERATE,          ENUM_VAX_ADDF ),
    INS( "cvtdg",   0x15,   0x009E,     IT_FP_CONVERT,          ENUM_VAX_ADDF ),
    INS( "addg",    0x15,   0x00A0,     IT_FP_OPERATE,          ENUM_VAX_ADDF ),
    INS( "cmpgeq",  0x15,   0x00A5,     IT_FP_OPERATE,          ENUM_VAX_CMPGEQ ),
    INS( "cmpglt",  0x15,   0x00A6,     IT_FP_OPERATE,          ENUM_VAX_CMPGEQ ),
    INS( "cmpgle",  0x15,   0x00A7,     IT_FP_OPERATE,          ENUM_VAX_CMPGEQ ),
    INS( "cvtgf",   0x15,   0x00AC,     IT_FP_CONVERT,          ENUM_VAX_ADDF ),
    INS( "cvtgd",   0x15,   0x00AD,     IT_FP_CONVERT,          ENUM_VAX_ADDF ),
    INS( "cvtqf",   0x15,   0x00BC,     IT_FP_CONVERT,          ENUM_VAX_CVTQF ),
    INS( "cvtqg",   0x15,   0x00BE,     IT_FP_CONVERT,          ENUM_VAX_CVTQF ),
    INS( "divf",    0x15,   0x0083,     IT_FP_OPERATE,          ENUM_VAX_ADDF ),
    INS( "divg",    0x15,   0x00A3,     IT_FP_OPERATE,          ENUM_VAX_ADDF ),
    INS( "mulf",    0x15,   0x0082,     IT_FP_OPERATE,          ENUM_VAX_ADDF ),
    INS( "mulg",    0x15,   0x00A2,     IT_FP_OPERATE,          ENUM_VAX_ADDF ),
    INS( "subf",    0x15,   0x0081,     IT_FP_OPERATE,          ENUM_VAX_ADDF ),
    INS( "subg",    0x15,   0x00A1,     IT_FP_OPERATE,          ENUM_VAX_ADDF ),
    INS( "cvtgq",   0x15,   0x00AF,     IT_FP_CONVERT,          ENUM_VAX_CVTGQ ),
 // PALcode
    INS( "call_pal",0x00,   0x0000,     IT_CALL_PAL,            ENUM_NONE ),
 // Stylized Code Forms
    // nop and fnop are in directiv.c since they don't require much parsing
    INS( "clr",     0x11,   0x0020,     IT_PSEUDO_CLR,          ENUM_NONE ),
    INS( "fclr",    0x17,   0x0020,     IT_PSEUDO_FCLR,         ENUM_NONE ),
    INS( "fmov",    0x17,   0x0020,     IT_PSEUDO_FMOV,         ENUM_NONE ),
    INS( "negl",    0x10,   0x0009,     IT_PSEUDO_NOT,          ENUM_OF_ADDL ),
    INS( "negq",    0x10,   0x0029,     IT_PSEUDO_NOT,          ENUM_OF_ADDL ),
    INS( "negf",    0x15,   0x0081,     IT_PSEUDO_NEGF,         ENUM_VAX_ADDF ),
    INS( "negg",    0x15,   0x00A1,     IT_PSEUDO_NEGF,         ENUM_VAX_ADDF ),
    INS( "negs",    0x16,   0x0081,     IT_PSEUDO_NEGF,         ENUM_IEEE_ADDS ),
    INS( "negt",    0x16,   0x00A1,     IT_PSEUDO_NEGF,         ENUM_IEEE_ADDS ),
    INS( "fneg",    0x17,   0x0021,     IT_PSEUDO_FNEG,         ENUM_NONE ),
    INS( "fnegf",   0x17,   0x0021,     IT_PSEUDO_FNEG,         ENUM_NONE ),
    INS( "fnegg",   0x17,   0x0021,     IT_PSEUDO_FNEG,         ENUM_NONE ),
    INS( "fnegs",   0x17,   0x0021,     IT_PSEUDO_FNEG,         ENUM_NONE ),
    INS( "fnegt",   0x17,   0x0021,     IT_PSEUDO_FNEG,         ENUM_NONE ),
    INS( "not",     0x11,   0x0028,     IT_PSEUDO_NOT,          ENUM_NONE ),
    INS( "or",      0x11,   0x0020,     IT_OPERATE,             ENUM_NONE ),
    INS( "andnot",  0x11,   0x0008,     IT_OPERATE,             ENUM_NONE ),
    INS( "xornot",  0x11,   0x0048,     IT_OPERATE,             ENUM_NONE ),
    INS( "fabs",    0x17,   0x0020,     IT_PSEUDO_NEGF,         ENUM_NONE ),
    INS( "sextl",   0x10,   0x0000,     IT_PSEUDO_NOT,          ENUM_NONE ),
 // The following pseudo-instructions might emit multiple real instructions
    INS( "mov",     0x11,   0x0020,     IT_PSEUDO_MOV,          ENUM_NONE ),
    // abs pseudo ins (opcode & funccode are from subl/v, subq/v)
    INS( "absl",    0x10,   0x0049,     IT_PSEUDO_ABS,          ENUM_NONE ),
    INS( "absq",    0x10,   0x0069,     IT_PSEUDO_ABS,          ENUM_NONE ),

#if 0
 // NYI: (more pseudo-ins that are in MS asaxp)
 //--------------------------------------------
    // Load instructions that emits more than one instructions
    INS( "ldb",     0x??,   0x????,     IT_PSEUDO_????,         ENUM_NONE ),
    INS( "ldbu",    0x??,   0x????,     IT_PSEUDO_????,         ENUM_NONE ),
    INS( "ldw",     0x??,   0x????,     IT_PSEUDO_????,         ENUM_NONE ),
    INS( "ldwu",    0x??,   0x????,     IT_PSEUDO_????,         ENUM_NONE ),
    INS( "uldw",    0x??,   0x????,     IT_PSEUDO_????,         ENUM_NONE ),
    INS( "uldwu",   0x??,   0x????,     IT_PSEUDO_????,         ENUM_NONE ),
    INS( "uldl",    0x??,   0x????,     IT_PSEUDO_????,         ENUM_NONE ),
    INS( "uldq",    0x??,   0x????,     IT_PSEUDO_????,         ENUM_NONE ),
    // Store instructions that emits more than one instructions
    INS( "stb",     0x??,   0x????,     IT_PSEUDO_????,         ENUM_NONE ),
    INS( "stw",     0x??,   0x????,     IT_PSEUDO_????,         ENUM_NONE ),
    INS( "ustw",    0x??,   0x????,     IT_PSEUDO_????,         ENUM_NONE ),
    INS( "ustl",    0x??,   0x????,     IT_PSEUDO_????,         ENUM_NONE ),
    INS( "ustq",    0x??,   0x????,     IT_PSEUDO_????,         ENUM_NONE ),
    // Integer divide and remainder instructions that are unsupported in
    // hardward level.
    // (MS asaxp does this by generating bsr to some _div function elsewhere)
    INS( "divl",    0x??,   0x????,     IT_PSEUDO_????,         ENUM_NONE ),
    INS( "divlu",   0x??,   0x????,     IT_PSEUDO_????,         ENUM_NONE ),
    INS( "divq",    0x??,   0x????,     IT_PSEUDO_????,         ENUM_NONE ),
    INS( "divqu",   0x??,   0x????,     IT_PSEUDO_????,         ENUM_NONE ),
    INS( "reml",    0x??,   0x????,     IT_PSEUDO_????,         ENUM_NONE ),
    INS( "remlu",   0x??,   0x????,     IT_PSEUDO_????,         ENUM_NONE ),
    INS( "remq",    0x??,   0x????,     IT_PSEUDO_????,         ENUM_NONE ),
    INS( "remqu",   0x??,   0x????,     IT_PSEUDO_????,         ENUM_NONE ),
#endif
};

#define MAX_NAME_LEN    20  // maximum length of an Alpha instruction mnemonic

static void addInstructionSymbol( qualifier_flags flags, ins_table *table_entry ) {
//*********************************************************************************
// Given an instruction name for which the optional bits in flags
// are turned on, add a symbol for it to the symbol table.

    sym_handle  sym;
    ins_symbol  *entry;
    char        buffer[ MAX_NAME_LEN ];

    strcpy( buffer, table_entry->name );
    if( flags ) {
        strcat( buffer, "/" );
        if( flags & QF_S ) strcat( buffer, "s" );

        if( flags & QF_U ) strcat( buffer, "u" );   // u & v are mutually
        if( flags & QF_V ) strcat( buffer, "v" );   // exclusive

        if( flags & QF_I ) strcat( buffer, "i" );

        if( flags & QF_C ) strcat( buffer, "c" );   // c, m, & d are
        if( flags & QF_M ) strcat( buffer, "m" );   // mutually exclusive
        if( flags & QF_D ) strcat( buffer, "d" );
    }

    entry = MemAlloc( sizeof( ins_symbol ) );
    entry->table_entry = table_entry;
    entry->flags = flags;

    // link it into our list of symbols for this table entry
    entry->next = table_entry->symbols;
    table_entry->symbols = entry;

    sym = SymAdd( buffer, SYM_INSTRUCTION );
    SymSetLink( sym, (void *)entry );
}

static void enum_NONE( ins_enum_method method, uint_32 mask, uint_8 level, void (*func)( qualifier_flags, ins_table * ), void *parm ) {
//********************************************************************************************************************************

    method = method;
    level = level;
    mask = mask;
    func( QF_NONE, parm );
}

static void enum_OF_ADDL( ins_enum_method method, uint_32 mask, uint_8 level, void (*func)( qualifier_flags, ins_table * ), void *parm ) {
//***********************************************************************************************************************************

    method = method;
    level = level;
    mask = mask;
    func( QF_NONE, parm );
    func( QF_V, parm );
}

static void enum_DTI_CVTQL( ins_enum_method method, uint_32 mask, uint_8 level, void (*func)( qualifier_flags, ins_table * ), void *parm ) {
//*************************************************************************************************************************************

    method = method;
    level = level;
    mask = mask;
    func( QF_NONE, parm );
    func( QF_V, parm );
    func( QF_S | QF_V, parm );
}

static void enum_IEEE_CMPTEQ( ins_enum_method method, uint_32 mask, uint_8 level, void (*func)( qualifier_flags, ins_table * ), void *parm ) {
//***************************************************************************************************************************************

    method = method;
    level = level;
    mask = mask;
    func( QF_NONE, parm );
    func( QF_S | QF_U, parm );
}

static void enum_IEEE_CVTQS( ins_enum_method method, uint_32 mask, uint_8 level, void (*func)( qualifier_flags, ins_table * ), void *parm ) {
//**************************************************************************************************************************************

    assert( level < 2 );
    switch( level ) {
    case 0:
        enum_IEEE_CVTQS( method, mask, level + 1, func, parm );
        mask |= ( QF_S | QF_U | QF_I );
        enum_IEEE_CVTQS( method, mask, level + 1, func, parm );
        break;
    case 1:
        func( mask, parm );
        mask |= QF_C;
        func( mask, parm );
        mask &= ~QF_C;
        mask |= QF_M;
        func( mask, parm );
        mask &= ~QF_M;
        mask |= QF_D;
        func( mask, parm );
        break;
    }
}

static void enum_VAX_ADDF_or_CVTGQ( ins_enum_method method, uint_32 mask, uint_8 level, void (*func)( qualifier_flags, ins_table * ), void *parm ) {
//*********************************************************************************************************************************************

    assert( level < 3 );
    switch( level ) {
    case 0:
        enum_VAX_ADDF_or_CVTGQ( method, mask, level + 1, func, parm );
        mask |= QF_S;
        enum_VAX_ADDF_or_CVTGQ( method, mask, level + 1, func, parm );
        break;
    case 1:
        enum_VAX_ADDF_or_CVTGQ( method, mask, level + 1, func, parm );
        mask |= ( ( method == ENUM_VAX_ADDF ) ? QF_U : QF_V );
        enum_VAX_ADDF_or_CVTGQ( method, mask, level + 1, func, parm );
        break;
    case 2:
        func( mask, parm );
        mask |= QF_C;
        func( mask, parm );
        break;
    }
}

static void enum_VAX_CMPGEQ( ins_enum_method method, uint_32 mask, uint_8 level, void (*func)( qualifier_flags, ins_table * ), void *parm ) {
//**************************************************************************************************************************************

    method = method;
    level = level;
    mask = mask;
    func( QF_NONE, parm );
    func( QF_S, parm );
}

static void enum_VAX_CVTQF( ins_enum_method method, uint_32 mask, uint_8 level, void (*func)( qualifier_flags, ins_table * ), void *parm ) {
//*************************************************************************************************************************************

    method = method;
    level = level;
    mask = mask;
    func( QF_NONE, parm );
    func( QF_C, parm );
}

static void enum_IEEE_ADDS_or_CVTTQ( ins_enum_method method, uint_32 mask, uint_8 level, void (*func)( qualifier_flags, ins_table * ), void *parm ) {
//**********************************************************************************************************************************************

    assert( level < 4 );
    switch( level ) {
    case 0:
        enum_IEEE_ADDS_or_CVTTQ( method, mask, level + 1, func, parm );
        mask |= QF_S;
        enum_IEEE_ADDS_or_CVTTQ( method, mask, level + 1, func, parm );
        break;
    case 1:
        if( !mask ) {
            enum_IEEE_ADDS_or_CVTTQ( method, mask, level + 1, func, parm );
        }
        mask |= ( ( method == ENUM_IEEE_ADDS ) ? QF_U : QF_V );
        enum_IEEE_ADDS_or_CVTTQ( method, mask, level + 1, func, parm );
        break;
    case 2:
        enum_IEEE_ADDS_or_CVTTQ( method, mask, level + 1, func, parm );
        if( mask & QF_S ) {
            mask |= QF_I;
            enum_IEEE_ADDS_or_CVTTQ( method, mask, level + 1, func, parm );
        }
        break;
    case 3:
        func( mask, parm );
        mask |= QF_C;
        func( mask, parm );
        mask &= ~QF_C;
        mask |= QF_M;
        func( mask, parm );
        mask &= ~QF_M;
        mask |= QF_D;
        func( mask, parm );
        break;
    }
}

typedef void (*enumFunc_t)( ins_enum_method, uint_32, uint_8, void (*func)( qualifier_flags, ins_table * ), void * );

#define PICK( a, b )    b,
static enumFunc_t enumFunc[] = {
#include "insenum.inc"
};
#undef PICK

static void enumInstructions( ins_enum_method method, void (*func)( qualifier_flags set, ins_table *parm ), void *parm ) {
//************************************************************************************************
// Depending on which enum_method it belongs to, different instruction-
// enumeration functions will be called to generate all the possible
// instructions with the different qualifiers attached.

    enumFunc[method]( method, QF_NONE, 0, func, parm );
}

#ifdef _STANDALONE_
#ifndef NDEBUG
static char *itStrings[] = {
#define PICK( a, b, c, d, e ) #a,
#include "alphafmt.inc"
#undef PICK
};

extern void DumpITString( ins_template template ) {
    printf( itStrings[ template ] );
}

static char *insEnumStrings[] = {
#define PICK( a, b ) #a,
#include "insenum.inc"
#undef PICK
};

extern void DumpInsEnumMethod( ins_enum_method method ) {
//*******************************************************

    printf( insEnumStrings[ method ] );
}

extern void DumpInsTableEntry( ins_table *table_entry ) {
//*******************************************************

    ins_symbol  *symbol;

    printf( "%s: 0x%x(0x%x) ", table_entry->name, table_entry->opcode, table_entry->funccode );
    DumpITString( table_entry->template );
    printf( ", " );
    DumpInsEnumMethod( table_entry->method );
    printf( "\n\tSymbol entries: " );
    symbol = table_entry->symbols;
    while( symbol != NULL ) {
        printf( " %x", symbol );
        symbol = symbol->next;
    }
    printf( "\n" );
}

extern void DumpInsTables() {
//***************************

    ins_table   *curr;
    int         i, n;

    n = sizeof( AlphaTable ) / sizeof( AlphaTable[0] );
    for( i = 0; i < n; i++ ) {
        curr = &AlphaTable[ i ];
        DumpInsTableEntry( curr );
    }
}
#endif
#endif

extern void InsInit() {
//*********************

    ins_table   *curr;
    int         i, n;

    n = sizeof( AlphaTable ) / sizeof( AlphaTable[0] );
    for( i = 0; i < n; i++ ) {
        curr = &AlphaTable[ i ];
        /* for each possible symbol generated by this instruction, add a symbol table entry */
        enumInstructions( curr->method, addInstructionSymbol, curr );
    }
#ifndef NDEBUG
    #ifdef _STANDALONE_
    if( _IsOption( DUMP_INS_TABLE ) ) DumpInsTables();
    #endif
#endif
}

extern instruction *InsCreate( sym_handle op_sym ) {
//**************************************************
// Allocate an instruction and initialize it.

    instruction *ins;

    ins = MemAlloc( sizeof( instruction ) );
    ins->opcode_sym = op_sym;
    ins->format = SymGetLink( op_sym );
    ins->num_operands = 0;
    return( ins );
}

extern void InsAddOperand( instruction *ins, ins_operand *op ) {
//**********************************************************
// Add an operand to the given instruction.

    if( ins->num_operands == MAX_OPERANDS ) {
        if( !insErrFlag ) {
            Error( MAX_NUMOP_EXCEEDED );
            insErrFlag = TRUE;
        }
        MemFree( op );
        return;
    }
    if( insErrFlag) insErrFlag = FALSE;
    ins->operands[ ins->num_operands++ ] = op;
}

extern void InsEmit( instruction *ins ) {
//***************************************
// Check an instruction to make sure operands match
// and encode it. The encoded instruction is emitted
// to the current OWL section.

#ifndef NDEBUG
    #ifdef _STANDALONE_
    if( _IsOption( DUMP_INSTRUCTIONS ) ) {
        DumpIns( ins );
    }
    #endif
#endif
    if( insErrFlag == FALSE && AlphaValidate( ins ) ) {
        #ifdef _STANDALONE_
        AlphaEmit( CurrentSection, ins );
        #else
        AlphaEmit( ins );
        #endif
    }
}

extern void InsDestroy( instruction *ins ) {
//******************************************
// Free up an instruction and all operands which
// are hanging off of it.

    int         i;

    for( i = 0; i < ins->num_operands; i++ ) {
        MemFree( ins->operands[ i ] );
    }
    MemFree( ins );
}

extern void InsFini() {
//*********************

    ins_table   *curr;
    ins_symbol  *next;
    ins_symbol  *entry;
    int         i, n;
    extern instruction *AsCurrIns; // from as.y

    if( AsCurrIns != NULL ) {
        InsDestroy( AsCurrIns );
        AsCurrIns = NULL;
    }
    n = sizeof( AlphaTable ) / sizeof( AlphaTable[0] );
    for( i = 0; i < n; i++ ) {
        curr = &AlphaTable[ i ];
        for( entry = curr->symbols; entry != NULL; entry = next ) {
            next = entry->next;
            MemFree( entry );
        }
        curr->symbols = NULL;   // need to reset this pointer
    }
}

#ifdef _STANDALONE_
#ifndef NDEBUG
extern void DumpIns( instruction *ins ) {
//***************************************

    int         i;

    printf( "%-11s", SymName( ins->opcode_sym ) );
    for( i = 0; i < ins->num_operands; i++ ) {
        if( i != 0 ) printf( ", " );
        DumpOperand( ins->operands[ i ] );
    }
    printf( "\n" );
}
#endif
#endif
