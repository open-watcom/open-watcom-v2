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


#ifndef ASMOPS1_H
#include "watcom.h"

struct AsmCodeName {
        unsigned short  position;       // starting position in AsmOpTable
        unsigned short  len :4,         // length of command, e.g. "AX" = 2
                        index :12;      // index into AsmChars[] in asmops2.h
        struct AsmCodeName *next;
};

enum asm_cpu {
        /* bit count from left: ( need at least 7 bits )
           bit 0-2:   Math coprocessor
           bit 3:   Protected mode
           bit 4-6: cpu type */

        P_NO87  = 0x00,         /* no FPU */
        P_87    = 0x01,         /* 8087 */
        P_287   = 0x02,         /* 80287 */
        P_387   = 0x04,         /* 80387 */

        P_PM    = 0x08,         /* protect-mode */

        P_86    = 0x00,         /* 8086, default */
        P_186   = 0x10,         /* 80186 */
        P_286   = 0x20,         /* 80286 */
        P_286p  = P_286 | P_PM, /* 80286, protected mode */
        P_386   = 0x30,         /* 80386 */
        P_386p  = P_386 | P_PM, /* 80386, protected mode */
        P_486   = 0x40,         /* 80486 */
        P_486p  = P_486 | P_PM, /* 80486, protected mode */
        P_586   = 0x50,         /* pentium */
        P_586p  = P_586 | P_PM, /* pentium, protected mode */
        P_686   = 0x60,         /* pentium */
        P_686p  = P_686 | P_PM, /* pentium, protected mode */

        P_END  = 0x70,      /* END */


        P_FPU_MASK = 0x07,
        P_CPU_MASK = 0x70
};


enum state {
        T_FINAL,
        T_INS,
        T_RES_ID,
        T_ID,
        T_REG,
        T_STRING,
        T_DIRECTIVE,
        T_DIRECT_EXPR,
        T_DEC_NUM,
        T_OCT_NUM,
        T_HEX_NUM_0,
        T_HEX_NUM,
        T_NUM,
        T_FLOAT,
        T_NOOP,                 /* No operation */

        T_POSITIVE,
        T_NEGATIVE,
        T_ID_IN_BACKQUOTES,
        T_BIN_NUM,
        T_PATH,
        T_UNARY_OPERATOR,
        T_BAD_NUM,

        T_OP_BRACKET    = '(',
        T_OP_SQ_BRACKET = '[',
        T_CL_BRACKET    = ')',
        T_CL_SQ_BRACKET = ']',
        T_COMMA         = ',',
        T_COLON         = ':',
        T_SEMI_COLON    = ';',
        T_TIMES         = '*',
        T_PLUS          = '+',
        T_MINUS         = '-',
        T_DOT           = '.',
        T_QUESTION_MARK = '?',
        T_PERCENT       = '%'
};

struct asm_tok {
        long            value;
        enum state      token;
        char            *string_ptr;
        unsigned long   extra_value;
};

#define ASMOPS1_H
#endif
