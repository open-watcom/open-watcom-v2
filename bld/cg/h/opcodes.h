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
* Description:  Code generator opcodes and their classification.
*
****************************************************************************/


#ifndef OPCODES_H_INCLUDED
#define OPCODES_H_INCLUDED
#include "targsys.h"

typedef enum {

    FIRST_OP,
    OP_NOP = FIRST_OP,

/*    binary operators */

    FIRST_ARITH_OP,
FIRST_CSE_OP = FIRST_ARITH_OP,
        FIRST_BINARY_OP = FIRST_ARITH_OP,
            OP_ADD = FIRST_BINARY_OP,
            OP_EXT_ADD,
            OP_SUB,
            OP_EXT_SUB,
            OP_MUL,
            OP_EXT_MUL,
            OP_DIV,
            OP_MOD,
            FIRST_BIT_OP,
                OP_AND = FIRST_BIT_OP,
                OP_OR,
                OP_XOR,
            LAST_BIT_OP = OP_XOR,
            OP_RSHIFT,
            OP_LSHIFT,
            FIRST_IFUNC,
            FIRST_BINARY_IFUNC = FIRST_IFUNC,
                OP_POW = FIRST_BINARY_IFUNC,
                OP_P5DIV,
                OP_ATAN2,
                OP_FMOD,
            LAST_BINARY_IFUNC = OP_FMOD,
        LAST_BINARY_OP = LAST_BINARY_IFUNC,

/*    unary operators */

        FIRST_UNARY_OP,
            OP_NEGATE = FIRST_UNARY_OP,
            OP_COMPLEMENT,
            FIRST_UNARY_IFUNC,
                OP_LOG = FIRST_UNARY_IFUNC,
                OP_COS,
                OP_SIN,
                OP_TAN,
                OP_SQRT,
                OP_FABS,
                OP_ACOS,
                OP_ASIN,
                OP_ATAN,
                OP_COSH,
                OP_EXP,
                OP_LOG10,
                OP_SINH,
                OP_TANH,
            LAST_UNARY_IFUNC = OP_TANH,
            LAST_IFUNC = LAST_UNARY_IFUNC,
            OP_PTR_TO_NATIVE,
            OP_PTR_TO_FOREIGN,
        LAST_UNARY_OP = OP_PTR_TO_FOREIGN,
    LAST_ARITH_OP = LAST_UNARY_OP,

    OP_SLACK_19,

/*      Move operators */

    FIRST_MOVE_OP,
        OP_CONVERT = FIRST_MOVE_OP,
#if ( _TARGET & _TARG_RISC ) == 0
LAST_CSE_OP = OP_CONVERT,
#endif
        OP_LA,
#if _TARGET & _TARG_RISC
LAST_CSE_OP = OP_LA,
#endif
        OP_CAREFUL_LA,
        OP_ROUND,
        OP_MOV,
    LAST_MOVE_OP = OP_MOV,

/*    Calling operators */

    OP_CALL_INDIRECT,
    OP_PUSH,
    OP_POP,
    OP_PARM_DEF,

/*    Select statement operator */

    OP_SELECT,  /*  generalized jump */

/*    Condition code setting instructions (no result, third operand is a label) */

    FIRST_OP_WITH_LABEL,
        FIRST_CONDITION = FIRST_OP_WITH_LABEL,
            OP_BIT_TEST_TRUE = FIRST_CONDITION,
            OP_BIT_TEST_FALSE,

            FIRST_COMPARISON,
                OP_CMP_EQUAL = FIRST_COMPARISON,
                OP_CMP_NOT_EQUAL,
                OP_CMP_GREATER,
                OP_CMP_LESS_EQUAL,
                OP_CMP_LESS,
                OP_CMP_GREATER_EQUAL,
            LAST_COMPARISON = OP_CMP_GREATER_EQUAL,
        LAST_CONDITION = LAST_COMPARISON,

/*    Operator whose argument is a label */

    OP_CALL,
    LAST_OP_WITH_LABEL = OP_CALL,

    FIRST_SET_OP,
        OP_SET_EQUAL = FIRST_SET_OP,
        OP_SET_NOT_EQUAL,
        OP_SET_GREATER,
        OP_SET_LESS_EQUAL,
        OP_SET_LESS,
        OP_SET_GREATER_EQUAL,
    LAST_SET_OP = OP_SET_GREATER_EQUAL,

    OP_DEBUG_INFO,
    OP_CHEAP_NOP,

/* Alpha specific opcodes */

    OP_LOAD_UNALIGNED,
    OP_STORE_UNALIGNED,
    FIRST_ALPHA_BYTE_INS,
        // do not change the order of these (see axpenc.c for details)
        OP_EXTRACT_LOW = FIRST_ALPHA_BYTE_INS,
        OP_EXTRACT_HIGH,
        OP_INSERT_LOW,
        OP_INSERT_HIGH,
        OP_MASK_LOW,
        OP_MASK_HIGH,
        OP_ZAP,
        OP_ZAP_NOT,
    LAST_ALPHA_BYTE_INS = OP_ZAP_NOT,

    OP_STK_ALLOC,
    OP_VA_START,

/*  some extras to add opcodes later */

    OP_SLACK_31,
    OP_SLACK_32,
    OP_SLACK_33,
    OP_SLACK_34,
    OP_SLACK_35,
    OP_SLACK_36,
    OP_SLACK_37,
    OP_SLACK_38,
    OP_SLACK_39,

/*  instruction in block has this opcode */

    OP_BLOCK,
    LAST_OP = OP_BLOCK

} opcode_defs;


#define _OpIsBinary( op )       ( (op) >= FIRST_BINARY_OP && \
                                  (op) <= LAST_BINARY_OP )

#define _OpIsUnary( op )        ( (op) >= FIRST_UNARY_OP && \
                                  (op) <= LAST_UNARY_OP )

#define _OpIsArith( op )        ( (op) >= FIRST_ARITH_OP && \
                                  (op) <= LAST_ARITH_OP )

#define _OpIsCSE( op )          ( (op) >= FIRST_CSE_OP && \
                                  (op) <= LAST_CSE_OP )

#define _OpIsBit( op )          ( (op) >= FIRST_BIT_OP && \
                                  (op) <= LAST_BIT_OP )

#define _OpIsIFunc( op )        ( (op) >= FIRST_IFUNC && \
                                  (op) <= LAST_IFUNC )

#define _OpIsBinIFunc( op )     ( (op) >= FIRST_BINARY_IFUNC && \
                                  (op) <= LAST_BINARY_IFUNC )

#define _OpIsUnIFunc( op )      ( (op) >= FIRST_UNARY_IFUNC && \
                                  (op) <= LAST_UNARY_IFUNC )

#define _OpIsCompare( op )      ( (op) >= FIRST_COMPARISON && \
                                  (op) <= LAST_COMPARISON )

#define _OpIsCondition( op )    ( (op) >= FIRST_CONDITION && \
                                  (op) <= LAST_CONDITION )

#define _OpIsJump( op )         ( _OpIsCondition( op ) || (op) == OP_SELECT )

#define _OpIsCall( op )         ( (op) == OP_CALL || (op) == OP_CALL_INDIRECT )

#define _OpCommutes( op )       ( ( (op) == OP_ADD ) \
                               || ( (op) == OP_MUL ) \
                               || ( (op) == OP_AND ) \
                               || ( (op) == OP_OR ) \
                               || ( (op) == OP_XOR ) )

#define _OpIsShift( op )        ( (op) == OP_RSHIFT || (op) == OP_LSHIFT )

#define _OpIsSet( op )          ( (op) >= FIRST_SET_OP && (op) <= LAST_SET_OP )

#define _OpIsAlphaByteIns( op ) ( (op) >= FIRST_ALPHA_BYTE_INS && (op) <= LAST_ALPHA_BYTE_INS )
#endif
