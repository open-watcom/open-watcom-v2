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
* Description:  Definition of warning levels for each message.
*
****************************************************************************/


// warn(code,level)
warn( WARN_PARM_INCONSISTENT_INDIRECTION_LEVEL, 1 )
warn( WARN_NONPORTABLE_PTR_CONV,                1 )
warn( WARN_PCTYPE_MISMATCH,                     1 )
warn( WARN_PARM_COUNT_MISMATCH,                 1 )
warn( WARN_INCONSISTENT_INDIRECTION_LEVEL,      1 )
warn( WARN_ASSIGNMENT_IN_BOOL_EXPR,             1 )
warn( WARN_CONSTANT_TOO_BIG,                    1 )
warn( WARN_MISSING_RETURN_VALUE,                1 )
warn( WARN_DUPLICATE_TYPEDEF_ALREADY_DEFINED,   1 )
warn( WARN_PASCAL_PRAGMA_NOT_DEFINED,           1 )
warn( WARN_FORTRAN_PRAGMA_NOT_DEFINED,          1 )
warn( WARN_MEANINGLESS,                         1 )
warn( WARN_POINTER_TRUNCATION,                  1 )
warn( WARN_POINTER_TYPE_MISMATCH,               1 )
warn( WARN_MISSING_LAST_SEMICOLON,              1 )
warn( WARN_ADDR_OF_ARRAY,                       1 )
warn( WARN_SYM_NOT_ASSIGNED,                    2 )
warn( WARN_DEAD_CODE,                           2 )
warn( WARN_SYM_NOT_REFERENCED,                  2 )
warn( WARN_UNDECLARED_PP_SYM,                   2 )
warn( WARN_ASSUMED_IMPORT,                      1 )
warn( WARN_NESTED_COMMENT,                      3 )
warn( WARN_USEFUL_SIDE_EFFECT,                  3 )
warn( WARN_RET_ADDR_OF_AUTO,                    1 )
warn( WARN_POINTER_PROMOTION,                   3 )
warn( WARN_PASTE_TOKEN_DISCARDED,               1 )
warn( WARN_UNREFERENCED_LABEL,                  1 )
warn( WARN_ADDR_OF_STATIC_FUNC_TAKEN,           1 )
warn( WARN_LVALUE_CAST,                         1 )
warn( WARN_JUNK_FOLLOWS_DIRECTIVE,              1 )
warn( WARN_LIT_TOO_LONG,                        1 )
warn( WARN_SPLICE_IN_CPP_COMMENT,               1 )
warn( WARN_COMPARE_ALWAYS,                      1 )
warn( WARN_REPEATED_MODIFIER,                   3 )
warn( WARN_QUALIFIER_MISMATCH,                  1 )
warn( WARN_SIGN_MISMATCH,                       1 )
warn( WARN_PARM_NOT_REFERENCED,                 3 )
warn( WARN_LOSE_PRECISION,                      1 )
warn( WARN_LEVEL_1,                             1 )
warn( WARN_ENUM_CONSTANT_TOO_LARGE,             1 )
warn( WARN_NO_STG_OR_TYPE,                      1 )
warn( WARN_NO_RET_TYPE_GIVEN,                   3 )
warn( WARN_NO_DATA_TYPE_GIVEN,                  3 )
warn( WARN_ASSEMBLER_WARNING,                   3 )
warn( WARN_SYMBOL_NAME_TOO_LONG,                1 )
warn( WARN_SHIFT_AMOUNT_NEGATIVE,               1 )
warn( WARN_SHIFT_AMOUNT_TOO_BIG,                1 )
warn( WARN_COMPARE_UNSIGNED_VS_ZERO,            1 )
warn( WARN_FUNCTION_STG_CLASS_REDECLARED,       1 )
warn( WARN_NO_EOL_BEFORE_EOF,                   1 )
warn( WARN_OBSOLETE_FUNC_DECL,                  3 )
warn( WARN_NONPROTO_FUNC_CALLED,                3 )
warn( WARN_NONPROTO_FUNC_CALLED_INDIRECT,       3 )
warn( WARN_MISSING_QUOTE,                       1 )
warn( WARN_PLAIN_CHAR_SUBSCRIPT,                4 )
warn( WARN_DIV_BY_ZERO,                         1 )
warn( WARN_CAST_POINTER_TRUNCATION,             3 )
