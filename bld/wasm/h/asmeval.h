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


#ifndef ASMEVAL_H
#define ASMEVAL_H

typedef enum exprtype {
    EXPR_EMPTY,
    EXPR_ADDR,          // e.g. "foo", "seg foo" and "offset foo"
    EXPR_CONST,         // A constant; note that "label1 - label2" -> constant
    EXPR_REG,           // A register
    EXPR_UNDEF,         // undefined type when error occures or result is undefined
} exprtype;

typedef struct expr_list {
    exprtype        type;           // Type of expression
    int_32          value;          // For constant, which may also be the offset
                                    //   to a label
    char            *string;        // for strings only -- NULL otherwise
    token_idx       base_reg;       // position of token for base register
                                    // if type is EXPR_REG, it holds register
    token_idx       idx_reg;        // position of token for index register
    token_idx       label;          // Position of token holding the label
    token_idx       override;       // Position of token holding the override label
                                    //   or register
    asm_token       instr;          // instruction token for label
                                    //
    bool            indirect;       // Whether inside [] or not
    bool            explicit;       // Whether expression type explicitly given
    bool            empty;
    bool            abs;
    memtype         mem_type;       // Whether expr is BYTE, WORD, DWORD, etc.
    uint_8          scale;          // scaling factor 1, 2, 4, or 8 - 386 code only
    asm_sym_handle  sym;
    asm_sym_handle  mbr;
} expr_list;

extern token_idx    EvalExpr( token_buffer *tokbuf, token_idx start, token_idx end, bool );
extern bool         EvalOperand( token_buffer *tokbuf, token_idx *, token_idx, expr_list *, bool );
#if defined( _STANDALONE_ )
extern token_idx    EvalConstant( token_buffer *tokbuf, token_idx start, token_idx end, bool );
#endif

#endif
