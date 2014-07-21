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


#ifndef _ASMOPS_H_
#define _ASMOPS_H_

#include "asmops.gh"

typedef enum asm_cpu {
        /* bit count from left: ( need at least 7 bits )
           bit 0-2:   Math coprocessor
           bit 3:     Protected mode
           bit 4-6:   cpu type
           bit 7-11;  extension set */

        P_NO87  = 0x0000,         /* no FPU */
        P_87    = 0x0001,         /* 8087 */
        P_287   = 0x0002,         /* 80287 */
        P_387   = 0x0004,         /* 80387 */

        P_PM    = 0x0008,         /* protect-mode */

        P_86    = 0x0000,         /* 8086, default */
        P_186   = 0x0010,         /* 80186 */
        P_286   = 0x0020,         /* 80286 */
        P_286p  = P_286 | P_PM,   /* 80286, protected mode */
        P_386   = 0x0030,         /* 80386 */
        P_386p  = P_386 | P_PM,   /* 80386, protected mode */
        P_486   = 0x0040,         /* 80486 */
        P_486p  = P_486 | P_PM,   /* 80486, protected mode */
        P_586   = 0x0050,         /* pentium */
        P_586p  = P_586 | P_PM,   /* pentium, protected mode */
        P_686   = 0x0060,         /* pentium */
        P_686p  = P_686 | P_PM,   /* pentium, protected mode */

        P_MMX   = 0x0080,         /* MMX extension instructions */
        P_K3D   = 0x0100,         /* 3DNow extension instructions */
        P_SSE   = 0x0200,         /* SSE extension instructions */
        P_SSE2  = 0x0400,         /* SSE extension instructions */
        P_SSE3  = 0x0800,         /* SSE extension instructions */

        NO_OPPRFX  = P_MMX | P_SSE | P_SSE2 | P_SSE3,

        P_FPU_MASK = 0x0007,
        P_CPU_MASK = 0x0070,
        P_EXT_MASK = 0x0F80
} asm_cpu;


typedef enum tok_class {
        TC_FINAL,
        TC_INSTR,
        TC_RES_ID,
        TC_ID,
        TC_REG,
        TC_STRING,
        TC_DIRECTIVE,
        TC_DIRECT_EXPR,
        TC_NUM,
        TC_FLOAT,
        TC_NOOP,                 /* No operation */

        TC_POSITIVE,
        TC_NEGATIVE,
        TC_ID_IN_BACKQUOTES,
        TC_PATH,
        TC_UNARY_OPERATOR,
        TC_RELATION_OPERATOR,
        TC_ARITH_OPERATOR,
        TC_BAD_NUM,

        TC_OP_BRACKET,       // '(',
        TC_OP_SQ_BRACKET,    // '[',
        TC_OP_BRACE,         // '{',
        TC_CL_BRACKET,       // ')',
        TC_CL_SQ_BRACKET,    // ']',
        TC_CL_BRACE,         // '}'
        TC_COMMA,            // ',',
        TC_COLON,            // ':',
        TC_SEMI_COLON,       // ';',
        TC_PLUS,             // '+',
        TC_MINUS,            // '-',
        TC_DOT,              // '.',
        TC_QUESTION_MARK,    // '?',
        TC_PERCENT,          // '%',
} tok_class;

typedef struct asm_tok {
        tok_class       class;
        char            *string_ptr;
        union {
            long          value;
            float         float_value;
            unsigned char bytes[10];
            asm_token     token;
        } u;
} asm_tok;

#endif
