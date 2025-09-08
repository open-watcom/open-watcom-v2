/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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

#ifndef ASMINS_H
#define ASMINS_H

#include "asmopnds.h"
#include "asmsym.h"
#include "asmops.h"
#include "asmtok.h"

enum regsize {
    A_BYTE = 0,
    A_WORD,
    A_DWORD,
};

typedef enum operand_idx {
    OPND1 = 0,
    OPND2,
    OPND3,
    OPND_MAX
} operand_idx;

typedef unsigned short  asmins_idx;

typedef struct AsmCodeName {
    asmins_idx      position;       // starting position in AsmOpTable
    unsigned short  len :4;         // length of command, e.g. "AX" = 2
    unsigned short  index :12;      // index into AsmChars[] in asmops2.h
    unsigned short  next;           // index to next item in hash item list
} AsmCodeName;

typedef struct asm_ins {
    asm_token           token;                  /* T_ADD, etc */
    unsigned            allowed_prefix  : 4;    /* allowed prefix */
    unsigned            byte1_info      : 4;    /* flags for 1st byte */
    unsigned            rm_info         : 2;    /* info on r/m byte */
    unsigned            wds             : 1;    /* info on wds bits */
    unsigned            opnd_type_3rd   : 4;    /* info on 3rd operand */
    unsigned            opnd_dir        : 1;    /* operand direction */
    asm_cpu             cpu;                    /* CPU type */
    OPNDTYPE            opnd_type1;             /* info on 1st operand type */
    OPNDTYPE            opnd_type2;             /* info on 2nd operand type */
    unsigned char       opcode;                 /* opcode byte */
    unsigned char       rm_byte;                /* mod_rm_byte */
} asm_ins;

typedef struct asm_code {
    struct {
        asm_token       ins;            // prefix before instruction, e.g. lock
        prefix_reg      seg;            // segment register override
        boolbit         adrsiz  : 1;    // address size prefix
        boolbit         opsiz   : 1;    // operand size prefix
    } prefix;
    memtype             mem_type;       // byte / word / etc. NOT near/far
    long                data[OPND_MAX];
    struct {
        asm_token       token;
        asm_cpu         cpu;
        OPNDTYPE        opnd_type[OPND_MAX];
        unsigned char   opcode;
        unsigned char   rm_byte;
    } info;
    signed char     extended_ins;
    unsigned char   sib;
    boolbit         use32           : 1;
    boolbit         indirect        : 1;    // CALL/JMP indirect jump
    boolbit         mem_type_fixed  : 1;
} asm_code;

#define NO_PREFIX   0x00
#define LOCK        0x01
#define REP         0x02
#define REPxx       0x03
#define FWAIT       0x04
#define NO_FWAIT    0x05
#define IREG        0x06    // indicates INDEX register for an OP_REGISTER entry

#define F_16        0x1
#define F_32        0x2
#define F_0F        0x3
#define F_F3        0x4
#define F_0F0F      0x5     // AMD 3DNow prefix
#define F_660F      0x6     // SSEx prefix 1
#define F_F20F      0x7     // SSEx prefix 2
#define F_F30F      0x8     // SSEx prefix 3

#define no_RM       0x1
#define R_in_OP     0x2

/* Note on the byte_1_info
   10 ( + F_0F ) -> the first byte is 0x0F, follow by opcode and rm_byte
   01 ( + F_16 ) -> the first byte is OPSIZ prefix if in use32 segment
   11 ( + F_32 ) -> the first byte is OPSIZ prefix if in use16 segment
   00            -> the first byte is opcode, follow by rm_byte      */

/* Note on the rm_info:
   00               -> has rm_byte
   01 ( + no_RM   ) -> no rm_byte
   10 ( + R_in_OP ) -> no rm_byte, reg field is included in opcode */


/* NOTE: The order of table is IMPORTANT !! */
/* OP_A should put before OP_R16 & OP_R
   OP_R16   "     "    "    OP_R
   OP_I8    "     "    "    OP_I
   OP_M ( without extension ) should follow OP_M_x
   OP_R ( without extension ) should follow OP_Rx
   OP_I ( without extension ) should follow OP_Ix  */

extern const asm_ins        ASMI86FAR AsmOpTable[];
extern const AsmCodeName    AsmOpcode[];
extern const char           AsmChars[];

extern const char           *const regs[3][4];

#if defined( _STANDALONE_ )

extern void     find_frame( asm_sym_handle sym );
extern int      GetInsString( asm_token, char * );

#endif

extern int      OperandSize( OPNDTYPE opnd );
extern bool     InRange( unsigned long val, unsigned bytes );
extern bool     cpu_directive( asm_token );
extern bool     AsmParse( token_buffer *tokbuf, const char *curline );
extern bool     NextArrayElement( token_buffer *tokbuf, bool *next );
extern bool     data_init( token_buffer *tokbuf, token_idx, token_idx );
extern bool     match_phase_1( void );

#endif
