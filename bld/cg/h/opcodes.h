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
    #define PICK(e,i,d1,d2,ot,pnum,attr)  OP_##i,
    #define ONLY_INTERNAL_CGOPS
    #include "cgops.h"
    #undef ONLY_INTERNAL_CGOPS
    #undef PICK
} opcode_defs;

#define FIRST_OP                OP_NOP
#define  FIRST_ARITH_OP         OP_ADD
#define FIRST_CSE_OP            OP_ADD
#define   FIRST_BINARY_OP       OP_ADD
#define     FIRST_BIT_OP        OP_AND
#define     LAST_BIT_OP         OP_XOR
#define   FIRST_IFUNC           OP_POW
#define     FIRST_BINARY_IFUNC  OP_POW
#define     LAST_BINARY_IFUNC   OP_FMOD
#define   LAST_BINARY_OP        OP_FMOD
#define     FIRST_UNARY_OP      OP_NEGATE
#define      FIRST_UNARY_IFUNC  OP_LOG
#define      LAST_UNARY_IFUNC   OP_TANH
#define   LAST_IFUNC            OP_TANH
#define     LAST_UNARY_OP       OP_PTR_TO_FOREIGN
#define  LAST_ARITH_OP          OP_PTR_TO_FOREIGN
#define  FIRST_MOVE_OP          OP_CONVERT
#if ( _TARGET & _TARG_RISC ) == 0
#define LAST_CSE_OP             OP_CONVERT
#else
#define LAST_CSE_OP             OP_LA
#endif
#define  LAST_MOVE_OP           OP_MOV
#define  FIRST_OP_WITH_LABEL    OP_BIT_TEST_TRUE
#define   FIRST_CONDITION       OP_BIT_TEST_TRUE
#define    FIRST_COMPARISON     OP_CMP_EQUAL
#define    LAST_COMPARISON      OP_CMP_GREATER_EQUAL
#define   LAST_CONDITION        OP_CMP_GREATER_EQUAL
#define  LAST_OP_WITH_LABEL     OP_CALL
#define  FIRST_SET_OP           OP_SET_EQUAL
#define  LAST_SET_OP            OP_SET_GREATER_EQUAL
#define  FIRST_ALPHA_BYTE_INS   OP_EXTRACT_LOW
#define  LAST_ALPHA_BYTE_INS    OP_ZAP_NOT
#define LAST_OP                 OP_BLOCK


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
