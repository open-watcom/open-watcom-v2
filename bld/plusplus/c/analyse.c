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


/*
ANALYSE.C -- analyse parsed tree of tokens
          -- issue any semantic errors
          -- convert parse tree into form that can be directly emitted
             using IcEmitExpr
*/

#include "plusplus.h"

#include <stddef.h>

#include "memmgr.h"
#include "stringl.h"
#include "cgfront.h"
#include "cast.h"
#include "fnovload.h"
#include "calldiag.h"
#include "fold.h"
#include "dbg.h"
#include "toggle.h"
#include "codegen.h"
#include "defarg.h"
#include "fnbody.h"
#include "context.h"
#include "rtfuns.h"
#include "ctexcept.h"
#include "objmodel.h"
#include "analtyid.h"
#   include "stats.h"
#ifdef XTRA_RPT
#   include "initdefs.h"
#endif
#ifndef NDEBUG
#include "pragdefn.h"
#endif
#include "mngless.h"

// define action-codes for (operation,operand,operand)-combination actions

typedef enum            // OPAC -- operation actions
{   REQD_INT_LEFT       // - required: integer on left
,   REQD_INT_RIGHT      // - required: integer on right
,   REQD_LVALUE_LEFT    // - required: lvalue on left
,   REQD_NOT_ENUM_LEFT  // - required: left must not be enum variable
#if 0
,   REQD_NOT_BOOL_LEFT  // - required: left must not be enum variable
#endif
,   REQD_ENUM_SAME      // - required: if enum, must be same enum
,   REQD_ZERO_LEFT      // - required: left must be 0 integer constant
,   REQD_ZERO_RIGHT     // - required: right must be 0 integer constant
,   REQD_PTR_SCALES_LEFT// - required: left not ptr to fun or void
,   REQD_PTR_SCALES_RIGHT//- required: right not ptr to fun or void
,   REQD_NOT_CONST_LEFT // - required: not constant on left
,   REQD_NOT_CONST_INIT // - required: constant on right ==> constant on left
,   REQD_NOT_ARRAY_LEFT // - required: not array on left
,   REQD_NOT_FUNC_LEFT  // - required: not function on left
,   REQD_NOT_FUNC_RIGHT // - required: not function on right
,   REQD_NOT_MFUN_LEFT  // - required: not this-member on left
,   REQD_NOT_MFUN_RIGHT // - required: not this-member on right
,   REQD_DEFD_CLPTR_LEFT// - required: defined class, when left ptr to class
,   REQD_DEFD_CLPTR_RIGHT//- required: defined class, when right ptr to class
,   REQD_CLASS_LEFT     // - required: defined class on left
,   REQD_CLASS_PTR_LEFT // - required: defined class pointer on left
,   REQD_BOOL_LEFT      // - required: boolean operator on left
,   REQD_BOOL_RIGHT     // - required: boolean operator on right
,   REQD_BOOL_LHS_ASSIGN// - required: lhs of assignment must be bool type
,   REQD_NOT_VOID_INDIRECT // - required: not void on left
,   WARN_ADJ_COMPARES   // - warn if adjacent compares
,   WARN_OPEQ_INT_TRUNC // - warn if @= int value will truncate
,   WARN_INT_TRUNC      // - warn if integer value is truncated
,   WARN_BOOL_ASSIGN_RIGHT // - warn if assignment in right boolean expression
,   WARN_BOOL_ASSIGN_LEFT  // - warn if assignment in left boolean expression
,   WARN_CONST_COND_LEFT   // - warn if constant conditional expr. on left
,   WARN_CONST_COND_RIGHT  // - warn if constant conditional expr. on right
,   WARN_POINTER_ZERO   // - warn if (p >= 0),(p < 0),(0 <= p),(0 > p)
,   WARN_USELESS_CMP    // - warn if compare is always true or false
,   WARN_MINUS_UNSIGNED // - warn if - ( <unsigned operand> )
,   CONV_INIT_REF       // - undo fetch of fetch of an initialization ref.
,   CONV_STAR_ADDR_OF   // - remove *& operations
,   CONV_SWITCH         // - switch operands
,   CONV_CMP_ZERO_LEFT  // - compare-to-zero generated on left
,   CONV_CMP_ZERO_RIGHT // - compare-to-zero generated on right
,   CONV_TO_PTR_DIFF    // - convert expression to ptr difference
,   CONV_STRIP_TYPE     // - strip one level of type informatiom
,   CONV_RVALUE_LEFT    // - get rvalue of left side
,   CONV_RVALUE_RIGHT   // - get rvalue of right side
,   CONV_MEMBER         // - convert right side from (class-qual,id) to member
,   CONV_INDEX          // - do index conversion
,   CONV_TYPE_RIGHT     // - convert type to right type
,   CONV_TYPE_LEFT      // - convert type to left type
,   CONV_AA_COMMON      // - convert to common arithmetic type
,   CONV_PP_COMMON      // - convert to common ptr
,   CONV_PP_ASSIGN      // - convert for ptr assignment
,   CONV_CTOR           // - convert a CTOR specification (5.2.3)
,   CONV_REFERENCE      // - convert a reference
,   CONV_MEANINGLESS    // - expression now meaningless
,   CONV_LOCN_RIGHT     // - set source-info. from right node
,   CONV_BOOL_ASSIGN    // - convert rhs r-value to bool (if necessary)
,   CONV_DYN_CAST       // - dynamic_cast<type>(expr)
,   CONV_CONST_CAST     // - const_cast<type>(expr)
,   CONV_REINT_CAST     // - reinterpret_cast<type>( expr )
,   CONV_STATIC_CAST    // - static_cast<type>( expr )
,   CONV_EXPLICIT_CAST  // - explicit cast
,   STATIC_TEMP_SET     // - set for static init, if required
,   STATIC_TEMP_RESET   // - reset for static init, if required
,   ASSIGN_OTHER        // - do a class, member-ptr assignment
,   INIT_CL_MP          // - do a class/memb-ptr initialization
,   CONV_FUN_MP_LEFT    // - convert left to func memb. ptr, if req'd
,   CONV_FUN_MP_RIGHT   // - convert right to func memb. ptr, if req'd
,   CONV_INIT_BARE      // - convert type(val) to val
,   CONV_FUN_MP_COLON   // - set up for colon memb-ptr extension
,   CONV_FUN_MP_CMP     // - set up for comparison memb-ptr extension
,   CONV_FUN_MP_CHECK   // - check operands for memb-ptr extension
,   RESULT_UN_ARITH     // - set type for unary arithmetic result
,   RESULT_BIN_ARITH    // - set type for binary arithmetic result
,   RESULT_BIN_SHIFT    // - set type for binary shift result
,   RESULT_CMP_ARITH    // - set type for binary arithmetic comparison
,   RESULT_ADDR_OF      // - set result for unary & (address of)
,   RESULT_INCDEC_ARITH // - increment/decrement for arithmetic operand
,   RESULT_PTR_SIZE     // - adjust right by '* sizeof(left)'
,   RESULT_INCDEC_PTR   // - increment/decrement for ptr operand
,   RESULT_QUESTMRK     // - ?: operation
,   RESULT_LVALUE       // - result is lvalue
,   RESULT_INDIRECT     // - result of an indirection
,   RESULT_BOOLEAN      // - result is boolean
,   RESULT_SIZEOF       // - set sizeof result
,   RESULT_OFFSETOF     // - set offsetof result
,   RESULT_CALL         // - call operation
,   RESULT_BARE         // - bare result is meaningful with a side effect
,   RESULT_NEW          // - "NEW" operator
,   RESULT_DLT          // - "DELETE" operator
,   RESULT_COLON_RVALUE // - common rvalue check for arith:arith, ptr:ptr
,   RESULT_COLON_AA     // - arith : arith
,   RESULT_COLON_PP     // - ptr : ptr
,   RESULT_COLON_OTHER  // - other : other ( void : void ), ( class, class )
,   RESULT_COLON        // - general result of ":" operator
,   RESULT_ASSIGN       // - result is assignment
,   RESULT_RETURN       // - result is return value
,   RESULT_RETURN_PA    // - result is return value (arith -> ptr )
,   RESULT_RETURN_AA    // - result is return value (arith -> arith)
,   RESULT_COMMA        // - result of ',' operator
,   RESULT_INIT         // - result of initialization (scalar)
,   RESULT_MINUS_PP     // - result of ptr - ptr
,   RESULT_BITQUEST     // - handle (expr-1?bit-fld-1:bit-fld-2) = expr-2
,   RESULT_TID_EXPR     // - result of typeid( <expr> )
,   RESULT_TID_TYPE     // - result of typeid( <type> )
,   RELOAD_EXPR_BINARY  // - reload type, left, right
,   RELOAD_EXPR_UNARY   // - reload type, left
,   RELOAD_EXPR_TYPE    // - reload type
,   CONV_BASIC_TYPE     // - do a TypedefModifierRemove on the current type
,   RESULT_MEMPTR_QUAL  // - do a .* or ->*
,   RESULT_OTHER_CMP    // - do comparison from member ptr., class
,   RESULT_THROW        // - do a throw
,   RESULT_SEGOP        // - do a :> operation
,   RESULT_SEGNAME      // - handle __segname( "..." )
,   RESULT_COND_EXPR    // - result is binary logical expression
,   RESULT_COND_OPRS    // - result has binary logical expressions
,   RESULT_COND_OPR     // - result has unary logical expression
,   DREF_PTR_LEFT       // - dereference left pointer
,   DREF_PTR_RIGHT      // - dereference right pointer
,   DIAG_FUNC_RIGHT_ONE // - diagnose misuse of function, function ptr (right)
,   DIAG_FUNC_MANY1     // - diagnose misuse of function, function ptr (right)
,   DIAG_FUNC_MANY2     // - diagnose misuse of function, function ptr (right)
,   DIAG_FUNC_LEFT      // - diagnose misuse of function, function ptr (left)
,   DIAG_VOID_LEFT      // - diagnose 'void' left operand
,   DIAG_VOID_RIGHT     // - diagnose 'void' left operand
#if 0
,   DIAG_AUTO_RETURN    // - diagnose return of addr/ref of auto
,   DIAG_AUTO_RETURN_REF// - diagnose return of ref of auto
,   RESULT_RETURN_VAL   // - set return value
,   RESULT_RETURN_VAL_COND // - set return value, if not class
#endif
,   CHECK_RETN_OPT      // - check for return optimization
,   ERR__IMPOSSIBLE     // - error: impossible
,   ERR__NOT_IMPL       // - error: not implemented
,   ERR__BOTH_PTR       // - error: both operands cannot be pointer
,   ERR__SUB_AP         // - error: arithmetic operand minus pointer
,   ERR__ONLY_AR        // - error: only arithmetic operands allowed
,   ERR__ONLY_AP        // - error: only arithmetic or ptr operands
,   ERR__ONLY_IN        // - error: only integral operands allowed
,   ERR__AP_ASSN        // - error: arith = ptr.
,   ERR__NO_PTR         // - error: expression must be pointer
,   ERR__INDEX          // - error: attempt to subscript non-array
,   ERR__PP_INDEX       // - error: attempt to subscript with a ptr index
,   ERR__NOT_FUN        // - error: left expression not a function
,   ERR__SEGOP          // - error: bad operands for ":>"
,   OPAC_END            // - termination action
} OPAC;


// Define action-combinations for (operation,operand,operand) actions.
// These correspond to the non-error codes for OPR_RTN_CODE.

static OPAC opac_BIN_ARITH_I[]  =   {   REQD_INT_LEFT
                                    ,   REQD_INT_RIGHT
                                    ,   CONV_RVALUE_LEFT
                                    ,   CONV_RVALUE_RIGHT
                                    ,   RESULT_BIN_ARITH
                                    ,   CONV_MEANINGLESS
                                    ,   OPAC_END
                                    };

static OPAC opac_BIN_ARITH[]    =   {   CONV_RVALUE_LEFT
                                    ,   CONV_RVALUE_RIGHT
                                    ,   RESULT_BIN_ARITH
                                    ,   CONV_MEANINGLESS
                                    ,   OPAC_END
                                    };

static OPAC opac_MINUS_PP[]     =   {   REQD_PTR_SCALES_LEFT
                                    ,   REQD_PTR_SCALES_RIGHT
                                    ,   DREF_PTR_LEFT
                                    ,   DREF_PTR_RIGHT
                                    ,   RESULT_MINUS_PP
                                    ,   REQD_DEFD_CLPTR_LEFT
                                    ,   CONV_TO_PTR_DIFF
                                    ,   CONV_MEANINGLESS
                                    ,   OPAC_END
                                    };

static OPAC opac_QUESTMRK[]     =   {   DIAG_FUNC_LEFT
                                    ,   CONV_CMP_ZERO_LEFT
                                    ,   WARN_CONST_COND_LEFT
                                    ,   RESULT_QUESTMRK
                                    ,   RESULT_COND_EXPR
                                    ,   OPAC_END
                                    };

static OPAC opac_COLON_AA[]     =   {   RESULT_COLON_RVALUE
                                    ,   RESULT_COLON_AA
                                    ,   RESULT_COLON
                                    ,   RESULT_COND_OPRS
                                    ,   OPAC_END
                                    };

static OPAC opac_COLON_PP[]     =   {   CONV_FUN_MP_COLON
                                    ,   CONV_FUN_MP_CHECK
                                    ,   DIAG_FUNC_LEFT
                                    ,   DIAG_FUNC_RIGHT_ONE
                                    ,   REQD_NOT_MFUN_LEFT
                                    ,   REQD_NOT_MFUN_RIGHT
                                    ,   RESULT_COLON_RVALUE
                                    ,   RESULT_COLON_PP
                                    ,   RELOAD_EXPR_BINARY
                                    ,   RESULT_COLON
                                    ,   RESULT_COND_OPRS
                                    ,   OPAC_END
                                    };

static OPAC opac_COLON_AP[]     =   {   CONV_FUN_MP_COLON
                                    ,   CONV_FUN_MP_CHECK
                                    ,   DIAG_FUNC_RIGHT_ONE
                                    ,   CONV_RVALUE_RIGHT
                                    ,   REQD_ZERO_LEFT
                                    ,   RESULT_COLON_RVALUE
                                    ,   CONV_TYPE_RIGHT
                                    ,   RESULT_COLON
                                    ,   RESULT_COND_OPRS
                                    ,   OPAC_END
                                    };

static OPAC opac_COLON_PA[]     =   {   CONV_FUN_MP_COLON
                                    ,   CONV_FUN_MP_CHECK
                                    ,   DIAG_FUNC_LEFT
                                    ,   CONV_RVALUE_LEFT
                                    ,   REQD_ZERO_RIGHT
                                    ,   RESULT_COLON_RVALUE
                                    ,   RESULT_COLON
                                    ,   RESULT_COND_OPRS
                                    ,   OPAC_END
                                    };

static OPAC opac_COLON_OTHER[]  =   {   CONV_FUN_MP_LEFT
                                    ,   CONV_FUN_MP_RIGHT
                                    ,   DIAG_FUNC_LEFT
                                    ,   DIAG_FUNC_RIGHT_ONE
                                    ,   RESULT_COLON_RVALUE
                                    ,   RESULT_COLON_OTHER
                                    ,   RESULT_COLON
                                    ,   RESULT_COND_OPRS
                                    ,   OPAC_END
                                    };

static OPAC opac_UN_ARITH_I[]   =   {   REQD_INT_LEFT
                                    ,   CONV_RVALUE_LEFT
                                    ,   RESULT_UN_ARITH
                                    ,   CONV_MEANINGLESS
                                    ,   OPAC_END
                                    };

static OPAC opac_UN_ARITH[]     =   {   CONV_RVALUE_LEFT
                                    ,   WARN_MINUS_UNSIGNED
                                    ,   RESULT_UN_ARITH
                                    ,   CONV_MEANINGLESS
                                    ,   OPAC_END
                                    };

static OPAC opac_UN_PLUS_PTR[]  =   {   DIAG_FUNC_LEFT
                                    ,   CONV_RVALUE_LEFT
                                    ,   CONV_MEANINGLESS
                                    ,   OPAC_END
                                    };

static OPAC opac_ADDR_OF[]      =   {   REQD_LVALUE_LEFT
                                    ,   RESULT_ADDR_OF
                                    ,   CONV_MEANINGLESS
                                    ,   OPAC_END
                                    };

static OPAC opac_INDIRECT[]     =   {   DIAG_FUNC_LEFT
                                    ,   DREF_PTR_LEFT
                                    ,   CONV_STRIP_TYPE
                                    ,   REQD_NOT_VOID_INDIRECT
                                    ,   CONV_STAR_ADDR_OF
                                    ,   RESULT_INDIRECT
                                    ,   OPAC_END
                                    };

static OPAC opac_LINCDEC_PTR[]  =   {   DIAG_FUNC_LEFT
                                    ,   REQD_PTR_SCALES_LEFT
                                    ,   REQD_NOT_ARRAY_LEFT
                                    ,   REQD_DEFD_CLPTR_LEFT
                                    ,   REQD_LVALUE_LEFT
                                    ,   REQD_NOT_CONST_LEFT
                                    ,   RESULT_INCDEC_PTR
                                    ,   CONV_TYPE_LEFT
                                    ,   RESULT_ASSIGN
                                    ,   OPAC_END
                                    };

static OPAC opac_LINCDEC_ARITH[] =  {   REQD_LVALUE_LEFT
                                    ,   REQD_NOT_ENUM_LEFT
                                    ,   REQD_NOT_CONST_LEFT
                                    ,   RESULT_INCDEC_ARITH
                                    ,   RESULT_ASSIGN
                                    ,   OPAC_END
                                    };

static OPAC opac_INCDEC_PTR[]   =   {   DIAG_FUNC_LEFT
                                    ,   REQD_PTR_SCALES_LEFT
                                    ,   REQD_NOT_ARRAY_LEFT
                                    ,   REQD_DEFD_CLPTR_LEFT
                                    ,   REQD_LVALUE_LEFT
                                    ,   REQD_NOT_CONST_LEFT
                                    ,   RESULT_INCDEC_PTR
                                    ,   RESULT_BARE
                                    ,   OPAC_END
                                    };

static OPAC opac_INCDEC_ARITH[] =   {   REQD_LVALUE_LEFT
                                    ,   REQD_NOT_ENUM_LEFT
                                    ,   REQD_NOT_CONST_LEFT
                                    ,   RESULT_INCDEC_ARITH
                                    ,   RESULT_BARE
                                    ,   OPAC_END
                                    };

static OPAC opac_SW_LV_EXPR[]   =   {   DIAG_FUNC_RIGHT_ONE
                                    ,   REQD_INT_LEFT
                                    ,   REQD_PTR_SCALES_RIGHT
                                    ,   REQD_DEFD_CLPTR_RIGHT
                                    ,   CONV_SWITCH
                                    ,   RELOAD_EXPR_BINARY
                                    ,   CONV_TYPE_LEFT
                                    ,   CONV_BASIC_TYPE
                                    ,   CONV_RVALUE_LEFT
                                    ,   DREF_PTR_RIGHT
                                    ,   RESULT_PTR_SIZE
                                    ,   CONV_MEANINGLESS
                                    ,   OPAC_END
                                    };

static OPAC opac_LVALUE_BEXPR[] =   {   DIAG_FUNC_LEFT
                                    ,   REQD_INT_RIGHT
                                    ,   REQD_PTR_SCALES_LEFT
                                    ,   REQD_DEFD_CLPTR_LEFT
                                    ,   DREF_PTR_LEFT
                                    ,   CONV_RVALUE_RIGHT
                                    ,   RESULT_PTR_SIZE
                                    ,   CONV_MEANINGLESS
                                    ,   OPAC_END
                                    };

static OPAC opac_SHIFT_OP[]     =   {   REQD_INT_LEFT
                                    ,   REQD_INT_RIGHT
                                    ,   CONV_RVALUE_LEFT
                                    ,   CONV_RVALUE_RIGHT
                                    ,   RESULT_BIN_SHIFT
                                    ,   CONV_MEANINGLESS
                                    ,   OPAC_END
                                    };

static OPAC opac_EQUAL_PP[]     =   {   DIAG_FUNC_LEFT
                                    ,   DIAG_FUNC_MANY1
                                    ,   REQD_LVALUE_LEFT
                                    ,   REQD_NOT_FUNC_LEFT
                                    ,   REQD_NOT_CONST_LEFT
                                    ,   REQD_NOT_ARRAY_LEFT
                                    ,   CONV_RVALUE_RIGHT
                                    ,   CONV_PP_ASSIGN
                                    ,   CONV_TYPE_LEFT
                                    ,   RESULT_ASSIGN
                                    ,   OPAC_END
                                    };

static OPAC opac_EQUAL_AP[]     =   {   DIAG_FUNC_MANY1
                                    ,   REQD_BOOL_LHS_ASSIGN
                                    ,   REQD_LVALUE_LEFT
                                    ,   REQD_NOT_CONST_LEFT
                                    ,   CONV_RVALUE_RIGHT
                                    ,   CONV_TYPE_LEFT
                                    ,   CONV_BOOL_ASSIGN
                                    ,   RESULT_ASSIGN
                                    ,   RESULT_BITQUEST
                                    ,   OPAC_END
                                    };

static OPAC opac_EQUAL_AA[]     =   {   REQD_LVALUE_LEFT
                                    ,   REQD_NOT_CONST_LEFT
                                    ,   REQD_ENUM_SAME
                                    ,   CONV_RVALUE_RIGHT
                                    ,   WARN_OPEQ_INT_TRUNC
                                    ,   CONV_TYPE_LEFT
                                    ,   CONV_BOOL_ASSIGN
                                    ,   RESULT_ASSIGN
                                    ,   RESULT_BITQUEST
                                    ,   OPAC_END
                                    };

static OPAC opac_EQUAL_PZ[]     =   {   DIAG_FUNC_LEFT
                                    ,   REQD_LVALUE_LEFT
                                    ,   REQD_NOT_FUNC_LEFT
                                    ,   REQD_NOT_ARRAY_LEFT
                                    ,   REQD_NOT_CONST_LEFT
                                    ,   REQD_ZERO_RIGHT
                                    ,   CONV_RVALUE_RIGHT
                                    ,   CONV_TYPE_LEFT
                                    ,   RESULT_ASSIGN
                                    ,   OPAC_END
                                    };

static OPAC opac_EQUAL_OTHER[]  =   {   CONV_FUN_MP_RIGHT
                                    ,   DIAG_FUNC_LEFT
                                    ,   DIAG_FUNC_MANY1
                                    ,   REQD_LVALUE_LEFT
                                    ,   REQD_NOT_FUNC_LEFT
                                    ,   ASSIGN_OTHER
                                    ,   RELOAD_EXPR_TYPE
                                    ,   RESULT_BARE
                                    ,   OPAC_END
                                    };

static OPAC opac_INIT_PP[]      =   {   CONV_INIT_REF
                                    ,   REQD_NOT_FUNC_LEFT
                                    ,   CONV_BASIC_TYPE
                                    ,   CONV_INIT_BARE
                                    ,   STATIC_TEMP_SET
                                    ,   RESULT_INIT
                                    ,   STATIC_TEMP_RESET
                                    ,   RELOAD_EXPR_TYPE
                                    ,   OPAC_END
                                    };

static OPAC opac_INIT_AP[]      =   {   REQD_BOOL_LHS_ASSIGN
                                    ,   CONV_INIT_REF
                                    ,   CONV_BASIC_TYPE
                                    ,   REQD_LVALUE_LEFT
                                    ,   CONV_INIT_BARE
                                    ,   STATIC_TEMP_SET
                                    ,   RESULT_INIT
                                    ,   STATIC_TEMP_RESET
                                    ,   RELOAD_EXPR_TYPE
                                    ,   OPAC_END
                                    };

static OPAC opac_INIT_AA[]      =   {   CONV_INIT_REF
                                    ,   CONV_BASIC_TYPE
                                    ,   REQD_LVALUE_LEFT
                                    ,   REQD_ENUM_SAME
                                    ,   CONV_INIT_BARE
                                    ,   STATIC_TEMP_SET
                                    ,   RESULT_INIT
                                    ,   STATIC_TEMP_RESET
                                    ,   RELOAD_EXPR_TYPE
                                    ,   OPAC_END
                                    };

static OPAC opac_INIT_PZ[]      =   {   CONV_INIT_REF
                                    ,   REQD_NOT_FUNC_LEFT
                                    ,   CONV_BASIC_TYPE
                                    ,   CONV_INIT_BARE
                                    ,   REQD_ZERO_RIGHT
                                    ,   CONV_RVALUE_RIGHT
                                    ,   STATIC_TEMP_SET
                                    ,   RESULT_INIT
                                    ,   STATIC_TEMP_RESET
                                    ,   RELOAD_EXPR_TYPE
                                    ,   OPAC_END
                                    };


static OPAC opac_INIT_OTHER[]  =    {   CONV_FUN_MP_RIGHT
                                    ,   DIAG_FUNC_MANY2
                                    ,   CONV_INIT_REF
                                    ,   REQD_NOT_FUNC_LEFT
                                    ,   CONV_BASIC_TYPE
                                    ,   STATIC_TEMP_SET
                                    ,   INIT_CL_MP
                                    ,   STATIC_TEMP_RESET
                                    ,   RELOAD_EXPR_TYPE
                                    ,   RESULT_BARE
                                    ,   OPAC_END
                                    };

static OPAC opac_RETURN__P[] =      {   CONV_FUN_MP_RIGHT
                                    ,   DIAG_FUNC_MANY1
                                    ,   CHECK_RETN_OPT
                                    ,   RESULT_RETURN
//                                  ,   DIAG_AUTO_RETURN
//                                  ,   RESULT_RETURN_VAL_COND
                                    ,   OPAC_END
                                    };

static OPAC opac_RETURN_PA[] =      {   CHECK_RETN_OPT
                                    ,   RESULT_RETURN_PA
//                                  ,   DIAG_AUTO_RETURN_REF
//                                  ,   RESULT_RETURN_VAL
                                    ,   OPAC_END
                                    };

static OPAC opac_RETURN_AA[] =      {   WARN_INT_TRUNC
                                    ,   CHECK_RETN_OPT
                                    ,   RESULT_RETURN_AA
//                                  ,   DIAG_AUTO_RETURN_REF
//                                  ,   RESULT_RETURN_VAL
                                    ,   OPAC_END
                                    };

static OPAC opac_CMP_PTR_ZERO[] =   {   CONV_FUN_MP_CMP
                                    ,   CONV_FUN_MP_CHECK
                                    ,   DIAG_FUNC_LEFT
                                    ,   CONV_RVALUE_LEFT
                                    ,   REQD_ZERO_RIGHT
                                    ,   CONV_RVALUE_RIGHT
                                    ,   WARN_ADJ_COMPARES
                                    ,   WARN_POINTER_ZERO
                                    ,   RESULT_BOOLEAN
                                    ,   CONV_MEANINGLESS
                                    ,   OPAC_END
                                    };

static OPAC opac_CMP_ZERO_PTR[] =   {   CONV_FUN_MP_CMP
                                    ,   CONV_FUN_MP_CHECK
                                    ,   DIAG_FUNC_RIGHT_ONE
                                    ,   REQD_ZERO_LEFT
                                    ,   CONV_RVALUE_LEFT
                                    ,   CONV_RVALUE_RIGHT
                                    ,   WARN_ADJ_COMPARES
                                    ,   WARN_POINTER_ZERO
                                    ,   RESULT_BOOLEAN
                                    ,   CONV_MEANINGLESS
                                    ,   OPAC_END
                                    };

static OPAC opac_CMP_PP[]       =   {   CONV_FUN_MP_CMP
                                    ,   CONV_FUN_MP_CHECK
                                    ,   DIAG_FUNC_LEFT
                                    ,   DIAG_FUNC_RIGHT_ONE
                                    ,   CONV_RVALUE_LEFT
                                    ,   CONV_RVALUE_RIGHT
                                    ,   WARN_ADJ_COMPARES
                                    ,   WARN_POINTER_ZERO
                                    ,   CONV_PP_COMMON
                                    ,   RESULT_BOOLEAN
                                    ,   CONV_MEANINGLESS
                                    ,   OPAC_END
                                    };

static OPAC opac_CMP_AA[]       =   {   CONV_RVALUE_LEFT
                                    ,   CONV_RVALUE_RIGHT
                                    ,   WARN_ADJ_COMPARES
                                    ,   WARN_USELESS_CMP
                                    ,   RESULT_CMP_ARITH
                                    ,   CONV_AA_COMMON
                                    ,   RESULT_BOOLEAN
                                    ,   CONV_MEANINGLESS
                                    ,   OPAC_END
                                    };

static OPAC opac_CMP_MP[]       =   {   CONV_FUN_MP_LEFT
                                    ,   CONV_FUN_MP_RIGHT
                                    ,   DIAG_FUNC_LEFT
                                    ,   DIAG_FUNC_RIGHT_ONE
                                    ,   DIAG_VOID_LEFT
                                    ,   DIAG_VOID_RIGHT
                                    ,   WARN_ADJ_COMPARES
                                    ,   RESULT_OTHER_CMP
                                    ,   RESULT_BOOLEAN
                                    ,   CONV_MEANINGLESS
                                    ,   OPAC_END
                                    };

static OPAC opac_BIN_LOG_PP[]   =   {   DIAG_FUNC_LEFT
                                    ,   DIAG_FUNC_RIGHT_ONE
                                    ,   WARN_BOOL_ASSIGN_LEFT
                                    ,   WARN_BOOL_ASSIGN_RIGHT
                                    ,   WARN_CONST_COND_LEFT
                                    ,   WARN_CONST_COND_RIGHT
                                    ,   CONV_CMP_ZERO_RIGHT
                                    ,   CONV_CMP_ZERO_LEFT
                                    ,   RESULT_BOOLEAN
                                    ,   CONV_MEANINGLESS
                                    ,   RESULT_COND_OPR
                                    ,   RESULT_COND_EXPR
                                    ,   OPAC_END
                                    };

static OPAC opac_BIN_LOG_PA[]   =   {   DIAG_FUNC_LEFT
                                    ,   WARN_BOOL_ASSIGN_LEFT
                                    ,   WARN_BOOL_ASSIGN_RIGHT
                                    ,   WARN_CONST_COND_LEFT
                                    ,   WARN_CONST_COND_RIGHT
                                    ,   CONV_CMP_ZERO_RIGHT
                                    ,   CONV_CMP_ZERO_LEFT
                                    ,   RESULT_BOOLEAN
                                    ,   CONV_MEANINGLESS
                                    ,   RESULT_COND_OPR
                                    ,   RESULT_COND_EXPR
                                    ,   OPAC_END
                                    };

static OPAC opac_BIN_LOG_AP[]   =   {   DIAG_FUNC_RIGHT_ONE
                                    ,   WARN_BOOL_ASSIGN_LEFT
                                    ,   WARN_BOOL_ASSIGN_RIGHT
                                    ,   WARN_CONST_COND_LEFT
                                    ,   WARN_CONST_COND_RIGHT
                                    ,   CONV_CMP_ZERO_RIGHT
                                    ,   CONV_CMP_ZERO_LEFT
                                    ,   RESULT_BOOLEAN
                                    ,   CONV_MEANINGLESS
                                    ,   RESULT_COND_OPR
                                    ,   RESULT_COND_EXPR
                                    ,   OPAC_END
                                    };

static OPAC opac_BIN_LOG_AA[]   =   {   WARN_BOOL_ASSIGN_LEFT
                                    ,   WARN_BOOL_ASSIGN_RIGHT
                                    ,   WARN_CONST_COND_LEFT
                                    ,   WARN_CONST_COND_RIGHT
                                    ,   CONV_CMP_ZERO_RIGHT
                                    ,   CONV_CMP_ZERO_LEFT
                                    ,   RESULT_BOOLEAN
                                    ,   CONV_MEANINGLESS
                                    ,   RESULT_COND_OPR
                                    ,   RESULT_COND_EXPR
                                    ,   OPAC_END
                                    };

static OPAC opac_BIN_LOG_MP[]   =   {   DIAG_FUNC_LEFT
                                    ,   DIAG_FUNC_RIGHT_ONE
                                    ,   WARN_BOOL_ASSIGN_LEFT
                                    ,   WARN_BOOL_ASSIGN_RIGHT
                                    ,   WARN_CONST_COND_LEFT
                                    ,   WARN_CONST_COND_RIGHT
                                    ,   REQD_BOOL_LEFT
                                    ,   REQD_BOOL_RIGHT
                                    ,   CONV_CMP_ZERO_RIGHT
                                    ,   CONV_CMP_ZERO_LEFT
                                    ,   RESULT_BOOLEAN
                                    ,   CONV_MEANINGLESS
                                    ,   RESULT_COND_OPR
                                    ,   RESULT_COND_EXPR
                                    ,   OPAC_END
                                    };

static OPAC opac_UN_LOG[]       =   {   WARN_BOOL_ASSIGN_LEFT
                                    ,   WARN_CONST_COND_LEFT
                                    ,   CONV_CMP_ZERO_LEFT
                                    ,   RESULT_BOOLEAN
                                    ,   CONV_MEANINGLESS
                                    ,   OPAC_END
                                    };

static OPAC opac_UN_LOG_P[]     =   {   DIAG_FUNC_LEFT
                                    ,   WARN_BOOL_ASSIGN_LEFT
                                    ,   WARN_CONST_COND_LEFT
                                    ,   CONV_CMP_ZERO_LEFT
                                    ,   RESULT_BOOLEAN
                                    ,   CONV_MEANINGLESS
                                    ,   OPAC_END
                                    };

static OPAC opac_UN_LOG_MP[]    =   {   WARN_BOOL_ASSIGN_LEFT
                                    ,   WARN_CONST_COND_LEFT
                                    ,   REQD_BOOL_LEFT
                                    ,   CONV_CMP_ZERO_LEFT
                                    ,   RESULT_BOOLEAN
                                    ,   CONV_MEANINGLESS
                                    ,   OPAC_END
                                    };

static OPAC opac_OP_SIZEOF[]    =   {   RESULT_SIZEOF
                                    ,   RELOAD_EXPR_TYPE
                                    ,   CONV_MEANINGLESS
                                    ,   OPAC_END
                                    };

static OPAC opac_CAST[]         =   {   CONV_FUN_MP_RIGHT
                                    ,   DIAG_FUNC_MANY1
                                    ,   CONV_MEANINGLESS
                                    ,   CONV_EXPLICIT_CAST
                                    ,   OPAC_END
                                    };

static OPAC opac_DYN_CAST[] =       {   CONV_FUN_MP_RIGHT
                                    ,   DIAG_FUNC_MANY1
                                    ,   CONV_MEANINGLESS
                                    ,   CONV_DYN_CAST
                                    ,   OPAC_END
                                    };

static OPAC opac_CONST_CAST[] =     {   CONV_FUN_MP_RIGHT
                                    ,   DIAG_FUNC_MANY1
                                    ,   CONV_MEANINGLESS
                                    ,   CONV_CONST_CAST
                                    ,   OPAC_END
                                    };

static OPAC opac_REINT_CAST[] =     {   CONV_FUN_MP_RIGHT
                                    ,   DIAG_FUNC_MANY1
                                    ,   CONV_MEANINGLESS
                                    ,   CONV_REINT_CAST
                                    ,   OPAC_END
                                    };

static OPAC opac_STAT_CAST[] =      {   CONV_FUN_MP_RIGHT
                                    ,   DIAG_FUNC_MANY1
                                    ,   CONV_MEANINGLESS
                                    ,   CONV_STATIC_CAST
                                    ,   OPAC_END
                                    };

static OPAC opac_TID_EXPR[] =       {   CONV_MEANINGLESS
                                    ,   RESULT_TID_EXPR
                                    ,   OPAC_END
                                    };

static OPAC opac_TID_TYPE[] =       {   CONV_MEANINGLESS
                                    ,   RESULT_TID_TYPE
                                    ,   OPAC_END
                                    };

static OPAC opac_OPEQ_PTR[]     =   {   DIAG_FUNC_LEFT
                                    ,   REQD_LVALUE_LEFT
                                    ,   REQD_INT_RIGHT
                                    ,   REQD_NOT_FUNC_LEFT
                                    ,   REQD_NOT_CONST_LEFT
                                    ,   REQD_NOT_ARRAY_LEFT
                                    ,   REQD_DEFD_CLPTR_LEFT
                                    ,   REQD_PTR_SCALES_LEFT
                                    ,   CONV_RVALUE_RIGHT
                                    ,   RESULT_PTR_SIZE
                                    ,   CONV_TYPE_LEFT
                                    ,   RESULT_ASSIGN
                                    ,   OPAC_END
                                    };

static OPAC opac_OPEQ_INT[]     =   {   REQD_LVALUE_LEFT
                                    ,   REQD_INT_LEFT
                                    ,   REQD_INT_RIGHT
                                    ,   REQD_NOT_ENUM_LEFT
                                    //,   REQD_NOT_BOOL_LEFT
                                    ,   REQD_NOT_CONST_LEFT
                                    ,   WARN_OPEQ_INT_TRUNC
                                    ,   CONV_RVALUE_RIGHT
                                    ,   CONV_TYPE_LEFT
                                    ,   RESULT_ASSIGN
                                    ,   OPAC_END
                                    };

static OPAC opac_OPEQ_ARITH[]   =   {   REQD_LVALUE_LEFT
                                    ,   REQD_NOT_ENUM_LEFT
                                    //,   REQD_NOT_BOOL_LEFT
                                    ,   REQD_NOT_CONST_LEFT
                                    ,   WARN_OPEQ_INT_TRUNC
                                    ,   CONV_RVALUE_RIGHT
                                    ,   CONV_TYPE_LEFT
                                    ,   RESULT_ASSIGN
                                    ,   OPAC_END
                                    };

static OPAC opac_OPEQ_SHIFT[]   =   {   REQD_LVALUE_LEFT
                                    ,   REQD_INT_LEFT
                                    ,   REQD_INT_RIGHT
                                    ,   REQD_NOT_ENUM_LEFT
                                    //,   REQD_NOT_BOOL_LEFT
                                    ,   REQD_NOT_CONST_LEFT
                                    ,   CONV_RVALUE_RIGHT
                                    ,   CONV_TYPE_LEFT
                                    ,   RESULT_ASSIGN
                                    ,   OPAC_END
                                    };

static OPAC opac_INDEX_PA[]     =   {   DIAG_FUNC_LEFT
                                    ,   REQD_INT_RIGHT
                                    ,   REQD_NOT_FUNC_LEFT
                                    ,   REQD_PTR_SCALES_LEFT
                                    ,   REQD_DEFD_CLPTR_LEFT
                                    ,   DREF_PTR_LEFT
                                    ,   CONV_RVALUE_RIGHT
                                    ,   RESULT_PTR_SIZE
                                    ,   CONV_INDEX
                                    ,   CONV_STRIP_TYPE
                                    ,   RESULT_LVALUE
                                    ,   CONV_MEANINGLESS
                                    ,   OPAC_END
                                    };

static OPAC opac_INDEX_AP[]     =   {   DIAG_FUNC_RIGHT_ONE
                                    ,   REQD_INT_LEFT
                                    ,   REQD_NOT_FUNC_RIGHT
                                    ,   REQD_PTR_SCALES_RIGHT
                                    ,   REQD_DEFD_CLPTR_RIGHT
                                    ,   CONV_SWITCH
                                    ,   RELOAD_EXPR_BINARY
                                    ,   CONV_TYPE_LEFT
                                    ,   CONV_BASIC_TYPE
                                    ,   CONV_RVALUE_LEFT
                                    ,   DREF_PTR_RIGHT
                                    ,   RESULT_PTR_SIZE
                                    ,   CONV_INDEX
                                    ,   CONV_STRIP_TYPE
                                    ,   RESULT_LVALUE
                                    ,   CONV_MEANINGLESS
                                    ,   OPAC_END
                                    };

static OPAC opac_CALL[]         =   {   RESULT_CALL
                                    ,   RELOAD_EXPR_TYPE
                                    ,   CONV_REFERENCE
                                    ,   RESULT_BARE
                                    ,   OPAC_END
                                    };

static OPAC opac_COMMA[]        =   {   CONV_FUN_MP_RIGHT
                                    ,   DIAG_FUNC_LEFT
                                    ,   DIAG_FUNC_MANY2
                                    ,   CONV_TYPE_RIGHT
                                    ,   RESULT_COMMA
                                    ,   OPAC_END
                                    };

static OPAC opac_PARAMETER[]    =   {   CONV_FUN_MP_RIGHT
                                    ,   CONV_TYPE_RIGHT
                                    ,   CONV_LOCN_RIGHT
                                    ,   OPAC_END
                                    };

static OPAC opac_PARAMETER_P[]  =   {   DIAG_FUNC_MANY2
                                    ,   CONV_TYPE_RIGHT
                                    ,   CONV_LOCN_RIGHT
                                    ,   OPAC_END
                                    };

static OPAC opac_NEW[]          =   {   RESULT_NEW
                                    ,   RELOAD_EXPR_TYPE
                                    ,   RESULT_BARE
                                    ,   OPAC_END
                                    };

static OPAC opac_DLT[]          =   {   RESULT_DLT
                                    ,   OPAC_END
                                    };

static OPAC opac_CTOR[]         =   {   REQD_NOT_ARRAY_LEFT
                                    ,   CONV_CTOR
                                    ,   OPAC_END
                                    };

static OPAC opac_CONVERT_INT[]  =   {   REQD_INT_LEFT
                                    ,   CONV_RVALUE_LEFT
                                    ,   CONV_MEANINGLESS
                                    ,   OPAC_END
                                    };

static OPAC opac_DOT_STAR[]  =      {   REQD_LVALUE_LEFT
                                    ,   REQD_CLASS_LEFT
                                    ,   RESULT_MEMPTR_QUAL
                                    ,   RELOAD_EXPR_TYPE
                                    ,   CONV_MEANINGLESS
                                    ,   OPAC_END
                                    };

static OPAC opac_ARROW_STAR[]  =    {   CONV_RVALUE_LEFT
                                    ,   REQD_CLASS_PTR_LEFT
                                    ,   REQD_DEFD_CLPTR_LEFT
                                    ,   RESULT_MEMPTR_QUAL
                                    ,   RELOAD_EXPR_TYPE
                                    ,   CONV_MEANINGLESS
                                    ,   OPAC_END
                                    };

static OPAC opac_THROW[] =          {   DIAG_FUNC_LEFT
                                    ,   RESULT_THROW
                                    ,   OPAC_END
                                    };

static OPAC opac_SEGOP[] =          {   CONV_RVALUE_LEFT
                                    ,   CONV_RVALUE_RIGHT
                                    ,   REQD_INT_LEFT
                                    ,   CONV_SWITCH
                                    ,   RELOAD_EXPR_BINARY
                                    ,   RESULT_SEGOP
                                    ,   CONV_MEANINGLESS
                                    ,   OPAC_END
                                    };

static OPAC opac_SEGNAME[] =        {   RESULT_SEGNAME
                                    ,   CONV_MEANINGLESS
                                    ,   OPAC_END
                                    };

static OPAC opac_DO_NOTHING[] =     {   OPAC_END
                                    };

static OPAC opac_IMPOSSIBLE[] =     {   ERR__IMPOSSIBLE
                                    ,   OPAC_END
                                    };

static OPAC opac_NOT_IMPL[] =       {   ERR__NOT_IMPL
                                    ,   OPAC_END
                                    };

static OPAC opac_ER__BOTH_PTR[] =   {   ERR__BOTH_PTR
                                    ,   OPAC_END
                                    };

static OPAC opac_ER__SUB_AP[] =     {   ERR__SUB_AP
                                    ,   OPAC_END
                                    };

static OPAC opac_ER__ONLY_AR[] =    {   ERR__ONLY_AR
                                    ,   OPAC_END
                                    };

static OPAC opac_ER__ONLY_AP[] =    {   ERR__ONLY_AP
                                    ,   OPAC_END
                                    };

static OPAC opac_ER__ONLY_IN[] =    {   ERR__ONLY_IN
                                    ,   OPAC_END
                                    };

static OPAC opac_ER__AP_ASSN[] =    {   ERR__AP_ASSN
                                    ,   OPAC_END
                                    };

static OPAC opac_ER__NO_PTR[] =     {   ERR__NO_PTR
                                    ,   OPAC_END
                                    };

static OPAC opac_ER__INDEX[] =      {   ERR__INDEX
                                    ,   OPAC_END
                                    };

static OPAC opac_ER__PP_INDEX[] =   {   ERR__PP_INDEX
                                    ,   OPAC_END
                                    };

static OPAC opac_ER__SEGOP[] =      {   ERR__SEGOP
                                    ,   OPAC_END
                                    };

static OPAC opac_ER__NOT_FUN[] =    {   ERR__NOT_FUN
                                    ,   OPAC_END
                                    };


// provide definitions for use with opcodes

#define OPR_RTNS                                                          \
 OPR_RTN(BIN_ARITH      )/* - binary arithmetic operation               */\
,OPR_RTN(BIN_ARITH_I    )/* - binary integral arithmetic operation      */\
,OPR_RTN(MINUS_PP       )/* - ptr - ptr                                 */\
,OPR_RTN(QUESTMRK       )/* - op ? op                                   */\
,OPR_RTN(COLON_PP       )/* - ptr : ptr                                 */\
,OPR_RTN(COLON_AA       )/* - arith : arith                             */\
,OPR_RTN(COLON_AP       )/* - arith : ptr                               */\
,OPR_RTN(COLON_PA       )/* - ptr : arith                               */\
,OPR_RTN(COLON_OTHER    )/* - other : other: void, class, class         */\
,OPR_RTN(UN_PLUS_PTR    )/* - + ptr.                                    */\
,OPR_RTN(UN_ARITH       )/* - unary arithmetic                          */\
,OPR_RTN(UN_ARITH_I     )/* - unary arithmetic, integral operand        */\
,OPR_RTN(ADDR_OF        )/* - unary &                                   */\
,OPR_RTN(INDIRECT       )/* - * operand                                 */\
,OPR_RTN(LINCDEC_PTR    )/* - ++ptr, --ptr,                             */\
,OPR_RTN(LINCDEC_ARITH  )/* - ++arith, --arith                          */\
,OPR_RTN(INCDEC_PTR     )/* - ptr++, ptr--                              */\
,OPR_RTN(INCDEC_ARITH   )/* - arith++, arith--                          */\
,OPR_RTN(SW_LV_EXPR     )/* - switch operands, do LVALUE_BEXPR          */\
,OPR_RTN(SHIFT_OP       )/* - shift operation                           */\
,OPR_RTN(LVALUE_BEXPR   )/* - try lvalue expr; then BIN_ARITH           */\
,OPR_RTN(EQUAL_PP       )/* - ptr = ptr;                                */\
,OPR_RTN(EQUAL_AP       )/* - arithmetic = ptr                          */\
,OPR_RTN(EQUAL_AA       )/* - arithmetic = arithmetic                   */\
,OPR_RTN(EQUAL_PZ       )/* - ptr = arithmetic; must be zero on left    */\
,OPR_RTN(EQUAL_OTHER    )/* - other = other                             */\
,OPR_RTN(INIT_PP        )/* - ptr = ptr;                                */\
,OPR_RTN(INIT_AP        )/* - arithmetic = ptr                          */\
,OPR_RTN(INIT_AA        )/* - arithmetic = arithmetic                   */\
,OPR_RTN(INIT_PZ        )/* - ptr = arithmetic; must be zero on left    */\
,OPR_RTN(INIT_OTHER     )/* - other = other                             */\
,OPR_RTN(CMP_PTR_ZERO   )/* - must be (ptr,0);                          */\
,OPR_RTN(CMP_ZERO_PTR   )/* - must be (0,ptr);                          */\
,OPR_RTN(CMP_PP         )/* - compare (ptr,ptr)                         */\
,OPR_RTN(CMP_AA         )/* - compare (arith,arith)                     */\
,OPR_RTN(CMP_MP         )/* - compare other (should be member-ptrs)     */\
,OPR_RTN(BIN_LOG_PP     )/* - binary logical expression ( ptr, ptr )    */\
,OPR_RTN(BIN_LOG_PA     )/* - binary logical expression ( ptr, arith )  */\
,OPR_RTN(BIN_LOG_AP     )/* - binary logical expression ( arith, ptr )  */\
,OPR_RTN(BIN_LOG_AA     )/* - binary logical expression ( arith, arith )*/\
,OPR_RTN(BIN_LOG_MP     )/* - binary logical expression (memb-ptr)      */\
,OPR_RTN(UN_LOG         )/* - unary logical expression                  */\
,OPR_RTN(UN_LOG_P       )/* - unary logical expression (ptr)            */\
,OPR_RTN(UN_LOG_MP      )/* - unary logical expression (memb-ptr)       */\
,OPR_RTN(OP_SIZEOF      )/* - sizeof( type ) or sizeof( expr )          */\
,OPR_RTN(CAST           )/* - cast                                      */\
,OPR_RTN(DYN_CAST       )/* - dynamic_cast<type>( expr )                */\
,OPR_RTN(CONST_CAST     )/* - const_cast<type>( expr )                  */\
,OPR_RTN(REINT_CAST     )/* - reinterpret_cast<type>(expr)              */\
,OPR_RTN(STAT_CAST      )/* - static_cast<type>(expr)                   */\
,OPR_RTN(TID_EXPR       )/* - typeid(<expr>)                            */\
,OPR_RTN(TID_TYPE       )/* - typeid(<type>)                            */\
,OPR_RTN(OPEQ_PTR       )/* - ptr [op]= integer                         */\
,OPR_RTN(OPEQ_ARITH     )/* - arith [op]= arith                         */\
,OPR_RTN(OPEQ_INT       )/* - integer [op]= integer                     */\
,OPR_RTN(OPEQ_SHIFT     )/* - integer [shift]= integer                  */\
,OPR_RTN(INDEX_PA       )/* - ptr[arith]                                */\
,OPR_RTN(INDEX_AP       )/* - arith[ptr]                                */\
,OPR_RTN(COMMA          )/* - o,o                                       */\
,OPR_RTN(CALL           )/* - function call                             */\
,OPR_RTN(PARAMETER      )/* - parameter on function call                */\
,OPR_RTN(PARAMETER_P    )/* - parameter on function call (could be ptr) */\
,OPR_RTN(NEW            )/* - new operations                            */\
,OPR_RTN(DLT            )/* - delete operations                         */\
,OPR_RTN(CTOR           )/* - CTOR a value                              */\
,OPR_RTN(RETURN__P      )/* - return value ( ? -> ? )                   */\
,OPR_RTN(RETURN_PA      )/* - return value ( arith -> ptr )             */\
,OPR_RTN(RETURN_AA      )/* - return value ( arith -> arith )           */\
,OPR_RTN(DOT_STAR       )/* - .* operator                               */\
,OPR_RTN(ARROW_STAR     )/* - ->* operator                              */\
,OPR_RTN(CONVERT_INT    )/* - convert to integer                        */\
,OPR_RTN(THROW          )/* - throw an expression                       */\
,OPR_RTN(SEGOP          )/* - :> operator                               */\
,OPR_RTN(SEGNAME        )/* - __segname( "..." )                        */\
                                                                          \
,OPR_RTN(IMPOSSIBLE     )/* - error: compiler error                     */\
,OPR_RTN(DO_NOTHING     )/* - do nothing                                */\
                                                                          \
,OPR_RTN(ER__BOTH_PTR   )/* - error: both operands cannot be pointer    */\
,OPR_RTN(ER__SUB_AP     )/* - error: arithmetic operand minus pointer   */\
,OPR_RTN(ER__ONLY_AR    )/* - error: only arithmetic operands allowed   */\
,OPR_RTN(ER__ONLY_AP    )/* - error: only arithmetic or ptr operands    */\
,OPR_RTN(ER__ONLY_IN    )/* - error: only integral operands allowed     */\
,OPR_RTN(ER__AP_ASSN    )/* - error: arith = ptr.                       */\
,OPR_RTN(ER__NO_PTR     )/* - error: expression must be pointer         */\
,OPR_RTN(ER__INDEX      )/* - error: attempt to subscript non-array     */\
,OPR_RTN(ER__PP_INDEX   )/* - error: ptr[ptr]                           */\
,OPR_RTN(ER__NOT_FUN    )/* - error: left expression not a function     */\
,OPR_RTN(ER__SEGOP      )/* - error: bad operands for ":>"              */\
,OPR_RTN(NOT_IMPL       )/* - for now, feature not implemented          */

// define routine codes for operand combinations
//
// This table is indexed using the CO_... codes from PPOPSDEF.H and so
// must be synchonized with it.

typedef enum {
    #define OPR_RTN(a) a
    OPR_RTNS
    #undef OPR_RTN
} OPR_RTN_CODE;


// The following table sets the actions for a non-error combination of
// OPCL_RTN_CODE.
// The table must be in the same order as OPCL_RTN_CODES.

static OPAC *opcomb_actions[] = {
    #define OPR_RTN(a) opac_ ## a
    OPR_RTNS
    #undef OPR_RTN
};

typedef enum            // OPCL -- operands classification
{   OPCL_PTR            // - ptr operand
,   OPCL_ARITH          // - arithmetic operand
,   OPCL_PTR_PTR        // - binary( ptr, ptr ) or unary( ptr )
                            = OPCL_PTR * 2 + OPCL_PTR
,   OPCL_PTR_ARITH      // - binary( ptr, arith )
                            = OPCL_PTR * 2 + OPCL_ARITH
,   OPCL_ARITH_PTR      // - binary( arith, ptr )
                            = OPCL_ARITH * 2 + OPCL_PTR
,   OPCL_ARITH_ARITH    // - binary( arith, arith ) or unary( arith )
                            = OPCL_ARITH * 2 + OPCL_ARITH
,   OPCL_OTHER          // - anything else
,   OPCL_MAX            // - number of classifications
} OPCL;

static OPR_RTN_CODE opr_rtn_table[][OPCL_MAX] = {
#include "ppopsan.h"
};


typedef struct                  // ERROR_CODES
{   unsigned reqd_kind;         // - required for left operand, when binary
    unsigned err_unary;         // - error when a unary operator
    unsigned err_left;          // - error when left operand is bad
    unsigned err_right;         // - error when right operand is bad
} ERROR_CODES;

static ERROR_CODES errs_not_arith = {   OPCL_ARITH
                                    ,   ERR_EXPR_MUST_BE_ARITHMETIC
                                    ,   ERR_LEFT_EXPR_MUST_BE_ARITHMETIC
                                    ,   ERR_RIGHT_EXPR_MUST_BE_ARITHMETIC
                                    };

static ERROR_CODES errs_not_int =   {   OPCL_ARITH
                                    ,   ERR_EXPR_MUST_BE_INTEGRAL
                                    ,   ERR_LEFT_EXPR_MUST_BE_INTEGRAL
                                    ,   ERR_RIGHT_EXPR_MUST_BE_INTEGRAL
                                    };

#if 0
static CNV_DIAG diagReturn =        // diagnosis for return conversion
    {   ERR_RETURN_IMPOSSIBLE
    ,   ERR_RETURN_AMBIGUOUS
    ,   ERR_CALL_WATCOM
    ,   ERR_RETURN_PRIVATE
    ,   ERR_RETURN_PROTECTED
    };

static CNV_DIAG diagDefarg =        // diagnosis for def. argument conversion
    {   ERR_DEFARG_IMPOSSIBLE
    ,   ERR_DEFARG_AMBIGUOUS
    ,   ERR_CALL_WATCOM
    ,   ERR_DEFARG_PRIVATE
    ,   ERR_DEFARG_PROTECTED
    };
#endif

static CNV_DIAG diagInit =          // diagnosis for init. conversion
    {   ERR_INIT_IMPOSSIBLE
    ,   ERR_INIT_AMBIGUOUS
    ,   ERR_CALL_WATCOM
    ,   ERR_INIT_PRIVATE
    ,   ERR_INIT_PROTECTED
    };

static CNV_DIAG diagAssignment =    // diagnosis for assignment conversion
    {   ERR_ASSIGN_IMPOSSIBLE
    ,   ERR_ASSIGN_AMBIGUOUS
    ,   ERR_CALL_WATCOM
    ,   ERR_ASSIGN_PRIVATE
    ,   ERR_ASSIGN_PROTECTED
    };
#if 0
static CNV_DIAG diagCast =          // diagnosis for cast conversion
    {   ERR_CAST_ILLEGAL
    ,   ERR_CAST_AMBIGUOUS
    ,   ERR_CONVERT_FROM_VIRTUAL_BASE
    ,   ERR_CAST_PRIVATE
    ,   ERR_CAST_PROTECTED
    };
#endif
static CNV_DIAG diagPtrConvAssign = // diagnosis for assignment ptr conversion
    {   ERR_PTR_CONVERSION
    ,   ERR_ASSIGN_AMBIGUOUS
    ,   ERR_CONVERT_FROM_VIRTUAL_BASE
    ,   ERR_ASSIGN_PRIVATE
    ,   ERR_ASSIGN_PROTECTED
    };

static CNV_DIAG diagPtrConvCommon = // diagnosis for assignment ptr conversion
    {   ERR_PTR_CONVERSION
    ,   ERR_PTR_COMMON_AMBIGUOUS
    ,   ERR_CONVERT_FROM_VIRTUAL_BASE
    ,   ERR_PTR_COMMON_PRIVATE
    ,   ERR_PTR_COMMON_PROTECTED
    };

static CNV_DIAG diagColonRef =      // diagnosis for colon reference conversion
    {   ERR_COLON_REF_CNV_IMPOSSIBLE
    ,   ERR_COLON_REF_CNV_AMBIGUOUS
    ,   0
    ,   ERR_COLON_REF_CNV_PRIVATE
    ,   ERR_COLON_REF_CNV_PROTECTED
    };

static CNV_DIAG diagColonMp =       // diagnosis for ":" MP conversion
    {   ERR_CALL_WATCOM
    ,   ERR_MP_PTR_AMBIGUOUS
    ,   ERR_CALL_WATCOM             // - virtual to derived ok
    ,   ERR_MP_PTR_PRIVATE
    ,   ERR_MP_PTR_PROTECTED
    };

static CNV_DIAG diagMembPtrCmp =    // diagnosis for ==, != MP conversion
    {   ERR_BAD_COLON_OPERANDS
    ,   ERR_MP_PTR_AMBIGUOUS
    ,   ERR_CALL_WATCOM             // - virtual to derived ok
    ,   ERR_MP_PTR_PRIVATE
    ,   ERR_MP_PTR_PROTECTED
    };

static CNV_DIAG diagClassCmp =      // diagnosis for class comparison
    {   ERR_COMMON_CL_IMPOSSIBLE
    ,   ERR_COMMON_CL_AMBIGUOUS
    ,   ERR_CALL_WATCOM
    ,   ERR_COMMON_CL_PRIVATE
    ,   ERR_COMMON_CL_PROTECTED
    };

static CNV_DIAG diagColonClass =    // diagnosis for class ":" conversion
    {   ERR_COMMON_CL_IMPOSSIBLE
    ,   ERR_COMMON_CL_AMBIGUOUS
    ,   ERR_CALL_WATCOM
    ,   ERR_COMMON_CL_PRIVATE
    ,   ERR_COMMON_CL_PROTECTED
    };

static CALL_DIAG diagCall =         // diagnosis for function call
    {   ERR_FUNCTION_AMBIGUOUS_OVERLOAD
    ,   ERR_FUNCTION_NO_MATCH
    ,   ERR_FUNCTION_NO_OVERLOAD
    };


static bool isRelationalOperator(   // TEST IF RELATIONAL OPERATOR
    PTREE node )                    // - node containing operator
{
    if( node->op == PT_BINARY ) {
        switch( node->cgop ) {
        case CO_GT:
        case CO_LT:
        case CO_GE:
        case CO_LE:
            return TRUE;
        }
    }
    return FALSE;
}

static void warnPointerZero(    // WARN IF POINTER COMPARISON TO 0 IS CONST
    PTREE expr,                 // - expression
    PTREE left,                 // - left subtree
    PTREE right )               // - right subtree
{
    bool zero_left;             // - TRUE ==> zero on left
    bool zero_right;            // - TRUE ==> zero on right

    zero_left = NodeIsZeroConstant( left );
    zero_right = NodeIsZeroConstant( right );
    if( zero_left != zero_right ) {
        switch( expr->cgop ) {
          case CO_LT :
            if( zero_right ) {
                PTreeWarnExpr( expr, WARN_POINTER_LT_0 );
            }
            break;
          case CO_GT :
            if( zero_left ) {
                PTreeWarnExpr( expr, WARN_POINTER_LT_0 );
            }
            break;
          case CO_GE :
            if( zero_right ) {
                PTreeWarnExpr( expr, WARN_POINTER_GE_0 );
            }
            break;
          case CO_LE :
            if( zero_left ) {
                PTreeWarnExpr( expr, WARN_POINTER_GE_0 );
            }
            break;
        }
    }
}

static int numSize( type_id id ) {     // NUMSIZE - number of bits
// return 0 not a num, else number of bits | SIGN_BIT if signed
    int     size;

    size = 0;
    switch( id ) {
    case TYP_SCHAR:
        size = SIGN_BIT;
    case TYP_UCHAR:
        size |= 8;
        break;
    case TYP_SSHORT:
        size = SIGN_BIT;
    case TYP_USHORT:
        size |= 16;
        break;
    case TYP_SLONG:
        size = SIGN_BIT;
    case TYP_ULONG:
    case TYP_POINTER:
    case TYP_FUNCTION:
    case TYP_MEMBER_POINTER:
        size |= 32;
        break;
    case TYP_SLONG64:
        size = SIGN_BIT;
    case TYP_ULONG64:
        size |= 64;
        break;
    case TYP_SINT:
        size = SIGN_BIT;
    case TYP_UINT:
#if TARGET_INT == 2
        size |= 16;
#else
        size |= 32;
#endif
        break;
    }
    return( size );
}

static int numSizeType( TYPE typ ) {     // NUMSIZE - number of bits
// return 0 not a num, else number of bits | SIGN_BIT if signed
    int     size;

    size = numSize( typ->id );
    if( typ->id == TYP_BITFIELD ) {
        size = typ->u.b.field_width;
        switch( typ->of->id ) {
        case TYP_SCHAR:
        case TYP_SSHORT:
        case TYP_SINT:
        case TYP_SLONG:
        case TYP_SLONG64:
            size |= SIGN_BIT;
            break;
        default:
            break;
        }
    }
    return( size );
}

static CGOP commRelOp( CGOP cgop ) {
// map cgop to commuted oprand equivelent
    switch( cgop ) {
    case CO_NE: // a != b => b != a
    case CO_EQ:
        break;
    case CO_GE: // a >= b => b <= a
        cgop = CO_LE;
        break;
    case CO_LT:
        cgop = CO_GT;
        break;
    case CO_GT:
        cgop = CO_LT;
        break;
    case CO_LE:
        cgop = CO_GE;
        break;
    DbgDefault("Default in commRelOp in analyse.c\n");
    }
    return cgop;
}

static void warnIfUseless( PTREE op1, PTREE op2, CGOP cgop, PTREE expr )
{
    TYPE            op1_type, op2_type, result_type;
    signed_64       val, low, high;
    int             op1_size, result_size;
    cmp_result      ret;
    bool            rev_ret;
    rel_op          rel = 0;
    char            num1[25], num2[25];
    INT_CONSTANT    icon;

    op1_type = TypedefModifierRemove( op1->type );
    op2_type = TypedefModifierRemove( op2->type );
    op1_size = numSizeType( op1_type );
    if( op1_size != 0 ) {
        result_type = TypeBinArithResult( op1_type, op2_type );
        if( result_type->id == TYP_ERROR ) {
            DbgAssert( op1_type->id == TYP_POINTER ||
                       op1_type->id == TYP_FUNCTION ||
                       op1_type->id == TYP_MEMBER_POINTER );
            result_size = numSize( TYP_ULONG );
        } else {
            result_size = numSize( result_type->id );
        }
        DbgAssert( result_size != 0 );
        rev_ret = FALSE;
        switch( cgop ) { // mapped rel ops to equivalent cases
        case CO_NE:
            rev_ret = TRUE;
        case CO_EQ:
            rel = REL_EQ;
            break;
        case CO_GE:
            rev_ret = TRUE;
        case CO_LT:
            rel = REL_LT;
            break;
        case CO_GT:
            rev_ret = TRUE;
        case CO_LE:
            rel = REL_LE;
            break;
        DbgDefault("Default in warnIfUseless in analyse.c.c\n");
        }
        NodeIsIntConstant( op2, &icon );
        if( icon.type->id == TYP_SLONG64 || icon.type->id == TYP_ULONG64 ) {
            val = icon.u.value;
        } else if( icon.type->id == TYP_ULONG || icon.type->id == TYP_UINT ) {
            U32ToU64( icon.u.uval, &val );
        } else {
            I32ToI64( icon.u.sval, &val );
        }
        ret = CheckMeaninglessCompare( rel
                                   , op1_size
                                   , result_size
                                   , ( op2_type->id == TYP_BITFIELD )
                                   , val
                                   , &low
                                   , &high );
        if( ret != CMP_VOID ) {
            if( rev_ret ) {
                if( ret == CMP_FALSE ) {
                    ret = CMP_TRUE;
                } else {
                    ret = CMP_FALSE;
                }
            }
            if( PTreeWarnExpr( expr, ret == CMP_TRUE ? WARN_ALWAYS_TRUE : WARN_ALWAYS_FALSE ) & MS_PRINTED ) {
                // msg was issued so give informational messages
                if( NumSign(op1_size) ) { // signed
                    sprintf( num1, "%lld", VAL64( low ) );
                    sprintf( num2, "%lld", VAL64( high ) );
                    CErr( INF_SIGNED_TYPE_RANGE, op1->type, num1, num2 );
                } else {
                    sprintf( num1, "%llu", VAL64( low ) );
                    sprintf( num2, "%llu", VAL64( high ) );
                    CErr( INF_UNSIGNED_TYPE_RANGE, op1->type, num1, num2 );
                }
                if( NumSign(result_size) ) {
                    sprintf( num1, "%lld", VAL64( val ) );
                    CErr( INF_SIGNED_CONST_EXPR_VALUE, num1 );
                } else {
                    sprintf( num1, "%llu", VAL64( val ) );
                    CErr( INF_UNSIGNED_CONST_EXPR_VALUE, num1 );
                }
            }
        }
    }
}

static void warnUselessCompare( // WARN IF COMPARISON IS USELESS (CONSTANT)
    PTREE expr,                 // - expression
    PTREE left,                 // - left operand
    PTREE right )               // - right operand
{
    PTREE       op1, op2;
    bool        constant_right;
    bool        constant_left;
    CGOP        cgop;

    constant_right = FALSE;
    constant_left = FALSE;
    cgop = expr->cgop;
    if( NodeIsConstantInt( right ) ) {
        constant_right = TRUE;
        op1 = left;
        op2 = right;
    } else if ( NodeIsConstantInt( left ) ) {
        constant_left = TRUE;
        op1 = right;
        op2 = left;
        cgop = commRelOp( cgop );
    }
    if( constant_left || constant_right ) {
        warnIfUseless( op1, op2, cgop, expr );
    }
}

static void warnMeaningfulSideEffect( // CHECK EXPRESSION FOR MEANING, SIDE EFF
    PTREE expr )                // - expression
{
    if( expr->op != PT_ERROR ) {
        if( 0 == ( expr->flags & PTF_MEANINGFUL ) ) {
            if( 0 == ( expr->flags & PTF_SIDE_EFF ) ) {
                PTreeWarnExpr( expr, WARN_EXPR_NOT_MEANINGFUL );
            } else {
                PTreeWarnExpr( expr, WARN_ONLY_SIDE_EFFECT );
            }
        } else if( 0 == ( expr->flags & PTF_SIDE_EFF ) ) {
            PTreeWarnExpr( expr, WARN_EXPR_NO_SIDE_EFFECT );
        }
    }
}


static void exprError(          // PRINT ERROR MESSAGE WITH EXPR TYPE
    PTREE expr,                 // - expr
    MSG_NUM msg )               // - error message
{
    TYPE result_type;

    if( PTreeErrorExpr( expr, msg ) & MS_PRINTED ) {
        result_type = NodeType( expr );
        if( result_type != NULL ) {
            InfMsgPtr( INF_EXPR_TYPE, result_type );
        }
    }
}

static TYPE operandError(       // PRINT ERROR MESSAGE WITH OPERAND TYPES
    PTREE expr,                 // - expression
    MSG_NUM msg )               // - error message
{
    PTREE left_expr;
    PTREE right_expr;
    TYPE left_type;
    TYPE right_type;

    left_type = NULL;
    right_type = NULL;
    switch( expr->op ) {
    case PT_BINARY:
        right_expr = expr->u.subtree[1];
        if( right_expr != NULL ) {
            right_type = NodeType( right_expr );
        }
        /* fall through */
    case PT_UNARY:
        left_expr = expr->u.subtree[0];
        if( left_expr != NULL ) {
            left_type = NodeType( left_expr );
        }
        break;
    }
    if( PTreeErrorExpr( expr, msg ) & MS_PRINTED ) {
        if( left_type != NULL ) {
            if( right_type != NULL ) {
                InfMsgPtr( INF_LEFT_OPERAND_TYPE, left_type );
                InfMsgPtr( INF_RIGHT_OPERAND_TYPE, right_type );
            } else {
                InfMsgPtr( INF_OPERAND_TYPE, left_type );
            }
        }
    }
    return NULL;
}


static TYPE analyse_err_left(   // ANALYSE KIND OF ERROR MESSAGE TO WRITE
    PTREE expr,                 // - expression in error
    MSG_NUM unary_msg,          // - message when unary operator
    MSG_NUM binary_msg )        // - message when binary operator
{
    MSG_NUM msg;                // - message code to be used

    if( expr->op == PT_UNARY ) {
        msg = unary_msg;
    } else {
        msg = binary_msg;
    }
    return operandError( expr, msg );
}


static TYPE analyse_err(        // ANALYSE KIND OF ERROR MESSAGE TO WRITE
    PTREE expr,                 // - expression in error
    unsigned left_kind,         // - kind of operand on left
    ERROR_CODES *msgs )         // - messages
{
    MSG_NUM msg;                // - message code to be used

    if( expr->op == PT_UNARY ) {
        msg = msgs->err_unary;
    } else if( left_kind == OPCL_OTHER ) {
        msg = msgs->err_left;
    } else if( left_kind == msgs->reqd_kind ) {
        msg = msgs->err_right;
    } else {
        msg = msgs->err_left;
    }
    return operandError( expr, msg );
}


static bool is_ptr_constant(    // CHECK IF NODE IS TYPED AS PTR TO A CONSTANT
    PTREE expr )                // - node
{
    TYPE type;                  // - type pointed at
    type_flag flags;            // - modifier flags
    bool retn;                  // - TRUE ==> has a constant type

    type = TypedefModifierRemove( expr->type );
    if( type->id == TYP_POINTER ) {
        TypeModFlags( type->of, &flags );
        if( flags & TF1_CONST ) {
            retn = TRUE;
        } else {
            retn = FALSE;
        }
    } else {
        retn = FALSE;
    }
    return retn;
}


static PTREE bld_type_size(     // BUILD CONSTANT NODE WITH SIZE OF TYPE
    TYPE type )                 // - type in question
{
    return NodeOffset( CgTypeSize( type ) );
}


static PTREE bld_sizeof_type(   // BUILD CONSTANT NODE WITH SIZEOF() OF TYPE
    TYPE type )                 // - type in question
{
    target_size_t type_size;
    type_id sizeof_type;

    /* very precise semantics: type of constant must be unsigned [long] */
    type_size = CgTypeSize( type );
    sizeof_type = TYP_UINT;
#if TARGET_INT < TARGET_LONG
    if( type_size > TARGET_UINT_MAX ) {
        sizeof_type = TYP_ULONG;
    }
#endif
    return PTreeIntConstant( type_size, sizeof_type );
}


static PTREE bld_pted_size(     // BUILD CONSTANT NODE WITH SIZE POINTED AT
    PTREE ptr )                 // - node which is a pointer
{
    return bld_type_size( TypePointedAtModified( ptr->type ) );
}


static PTREE bld_ptr_adj(       // BUILD A POINTER ADJUSTMENT
    PTREE factor,               // - factor
    PTREE ptr )                 // - pointer being adjusted
{
    PTREE new_node;             // - new node
    TYPE ptr_type;              // - pointer type
    target_size_t pted_size;    // - size of ptr object type

    ptr_type = ptr->type;
    factor = NodeConvert( TypePointerDiff( ptr_type ), factor );
    pted_size = CgTypeSize( TypePointedAtModified( ptr_type ) );
    if( pted_size == 1 ) {
        return factor;
    }
    new_node = NodeBinary( CO_TIMES, factor, NodeOffset( pted_size ) );
    new_node->type = factor->type;
    return FoldBinary( new_node );
}


static TYPE checkForCharPromotion // CHECK FOR PLAIN CHAR PROMOTION
    ( TYPE tgt_type             // - target type
    , PTREE src )               // - source node
{
    if( tgt_type->id == TYP_SINT
     && TypedefModifierRemoveOnly( src->type )->id == TYP_CHAR ) {
        PTreeWarnExpr( src, WARN_CHAR_PROMOTION );
    }
    return tgt_type;
}


static TYPE unary_arith_result( // COMPUTE RESULT OF UNARY ARITHMETIC
    PTREE left )                // - left operand
{
    TYPE retn;
    if( CompFlags.plain_char_promotion ) {
        retn = TypeUnArithResult( left->type );
        retn = checkForCharPromotion( retn, left );
    } else {
        return TypeUnArithResult( left->type );
    }
    return retn;
}

static TYPE binary_arith_result(// COMPUTE RESULT OF BINARY ARITHMETIC
    PTREE left,                 // - left operand
    PTREE right )               // - right operand
{
    TYPE retn;
    if( CompFlags.plain_char_promotion ) {
        retn = TypeBinArithResult( left->type, right->type );
        retn = checkForCharPromotion( retn, left );
        retn = checkForCharPromotion( retn, right );
    } else {
        retn = TypeBinArithResult( left->type, right->type );
    }
    return retn;
}


static bool ptr_scales(         // TEST IF EXPRESSION IF PTR. IS SCALABLE
    PTREE expr )                // - expression
{
    TYPE type;                  // - type
    type_flag not_used;         // - flags (not used)

    type = TypePointedAt( expr->type, &not_used );
    switch( type->id ) {
    case TYP_FUNCTION:
    case TYP_VOID:
        return FALSE;
    case TYP_ARRAY:
        if( type->u.a.array_size == 0 ) {
            return FALSE;
        }
        break;
    }
    return TRUE;
}


static unsigned classify_operand( // CLASSIFY OPERAND AS PTR, ARITH, OTHER
    PTREE operand )             // - the operand
{
    unsigned retn;              // - classification
    TYPE type;                  // - operand type

    if( operand == NULL ) {
        retn = OPCL_OTHER;
    } else {
        type = operand->type;
        if( type == NULL ) {
            retn = OPCL_OTHER;
        } else {
            type = TypeReferenced( type );
            switch( operand->op ) {
              case PT_INT_CONSTANT :
              case PT_FLOATING_CONSTANT :
              case PT_STRING_CONSTANT :
              case PT_SYMBOL :
              case PT_BINARY :
              case PT_UNARY :
              case PT_TYPE :
              case PT_DUP_EXPR :
              case PT_IC :
                type = TypedefModifierRemove( type );
                if( NULL != PointerTypeEquivalent( type ) ) {
                    retn = OPCL_PTR;
                } else if( NULL != ArithType( type ) ) {
                    retn = OPCL_ARITH;
                } else {
                    retn = OPCL_OTHER;
                }
                break;
              default :
                retn = OPCL_OTHER;
                break;
            }
        }
    }
    return retn;
}


static PTREE generateCtor(      // GENERATE A CTOR CALL
    PTREE this_node,            // - object being CTOR'd
    PTREE ctor_node )           // - CO_CTOR node
{
    PTREE initial;              // - parameters for initialization
    PTREE func_node;            // - node containing optional CTOR symbol
    TYPE object_type;           // - type of object being CTOR'd
    SYMBOL ctor;                // - SYMBOL for constructor
    PTREE retn;                 // - resultant expression

    ctor_node = PTreePromoteLocn( ctor_node );
    initial = ctor_node->u.subtree[1];
    func_node = ctor_node->u.subtree[0];
    object_type = TypedefModifierRemoveOnly( ctor_node->type );
    if( func_node == NULL ) {
        ctor = NULL;
    } else {
        ctor = func_node->u.symcg.symbol;
        PTreeFree( func_node );
    }
    retn = EffectCtor( initial
                     , ctor
                     , object_type
                     , this_node
                     , &ctor_node->locn
                     , EFFECT_EXACT | EFFECT_DECOR_TEMP );
    PTreeFree( ctor_node );
    return retn;
}


static PTREE convertCtor(       // CONVERT CTOR EXPRESSION
    PTREE expr )                // - expression
{
    SYMBOL ctor;                // - CTOR symbol
    PTREE *a_left;              // - reference( left node)
    TYPE type;                  // - type being CTOR'ed
//  unsigned retn;              // - conversion return
    FNOV_DIAG fnov_diag;        // - diagnosis information

    type = PTreeOpLeft(expr)->type;
    expr->type = type;
    switch( AnalyseCtorDiag( type
                           , &ctor
                           , PTreeRefRight( expr )
                           , &fnov_diag ) ) {
      case CNV_IMPOSSIBLE :
        CtorDiagNoMatch( expr, ERR_CTOR_IMPOSSIBLE, &fnov_diag );
        break;
      case CNV_ERR :
        PTreeErrorNode( expr );
        break;
      case CNV_OK :
        a_left = PTreeRefLeft( expr );
        NodeFreeDupedExpr( *a_left );
        if( ctor == NULL ) {
            *a_left = NULL;
        } else {
            *a_left = MakeNodeSymbol( ctor );
            PTreeExtractLocn( expr , &(*a_left)->locn );
        }
        if( NULL == StructType( type ) ) {
            PTREE old = expr;
            expr = old->u.subtree[1];
            if( expr == NULL ) {
                TYPE underlying;    // - underlying type
                underlying = TypedefModifierRemoveOnly( type );
                switch( underlying->id ) {
                case TYP_FLOAT :
                case TYP_DOUBLE :
                case TYP_LONG_DOUBLE :
                    expr = PTreeFloatingConstantStr( "0", underlying->id );
                    break;
                case TYP_MEMBER_POINTER:
                    expr = MembPtrZero( type );
                    break;
                default:
                    expr = NodeIntegralConstant( 0, type );
                }
            }
            PTreeExtractLocn( old, &expr->locn );
            PTreeFree( old->u.subtree[0] );
            PTreeFree( old );
#if 0
            retn = ConvertExprDiagnosed( &expr
                                       , type
                                       , CNV_INIT
                                       , expr
                                       , &diagInit );
#else
            expr = CastImplicit( expr, type, CNV_INIT, &diagInit );
#endif
        } else {
            PTREE temp = NodeTemporary( type );
            expr = generateCtor( temp, expr );
            if( expr->op != PT_ERROR ) {
                expr = NodeDtorExpr( expr, temp->u.symcg.symbol );
            }
        }
        break;
    }
    FnovFreeDiag( &fnov_diag );
    return expr;
}


static PTREE initClass(         // INIT. A CLASS ( INIT OR RETURN )
    PTREE left,                 // - what is inited
    PTREE right_comma,          // - initialization expression (comma'd ?)
    CNV_DIAG *diagnosis,        // - diagnosis for errors
    TOKEN_LOCN *init_locn )     // - initialization location
{
    PTREE expr;                 // - resultant expression
    PTREE right;                // - initialization expression (not comma'd)
    TYPE type;                  // - type being initialized
    SYMBOL ctor;                // - CTOR to be applied
    CNV_RETN retn;              // - conversion indication
    PTREE dtor;                 // - DTOR-mark node
    FNOV_DIAG fnov_diag;        // - diagnosis information

    right = PTreeOp( &right_comma );
    type = TypedefModifierRemoveOnly( left->type );
    if( NodeIsBinaryOp( right, CO_DTOR ) ) {
        dtor = right;
        right = dtor->u.subtree[1];
    } else {
        dtor = NULL;
    }
    if( NodeIsBinaryOp( right, CO_LIST ) ) {    // class c( parms )
        TYPE orig_right;        // - original type on right
#ifndef NDEBUG
        if( dtor != NULL ) {
            CFatal( "initClass -- dtor with CO_LIST" );
        }
#endif
        orig_right = NodeType( right_comma );
        retn = AnalyseCtorDiag( type, &ctor, &right_comma, &fnov_diag );
        switch( retn ) {
          case CNV_OK :
            expr = EffectCtor( right_comma
                             , ctor
                             , type
                             , left
                             , init_locn
                             , EFFECT_EXACT );
            break;
          case CNV_AMBIGUOUS :
            CallDiagAmbiguous( right_comma, diagnosis->msg_ambiguous, &fnov_diag );
            NodeFreeDupedExpr( left );
            expr = right_comma;
            break;
          case CNV_IMPOSSIBLE :
            ConversionTypesSet( orig_right, left->type );
            expr = NodeBinary( CO_CTOR, left, right_comma );
            expr->locn = *init_locn;
            CtorDiagNoMatch( expr, diagnosis->msg_impossible, &fnov_diag );
            ConversionDiagnoseInf();
            break;
          default :
            ConversionTypesSet( orig_right, left->type );
            ConversionDiagnose( retn, right_comma, diagnosis );
            NodeFreeDupedExpr( left );
            expr = right_comma;
            break;
        }
        FnovFreeDiag( &fnov_diag );
    } else {                                   // class c = expr
#if 0
        expr = ClassDefaultCopyDiag( left, right_comma, diagnosis );
#else
        expr = CopyInit( right_comma
                       , left
                       , TypeForLvalue( left )
                       , diagnosis );
#endif
    }
    return expr;
}


static PTREE initClassFromExpr( // INITIALIZE CLASS FROM EXPRESSION
    PTREE expr,                 // - expression
    CNV_DIAG *diagnosis )       // - diagnosis for errors
{
    PTREE left;                 // - left argument
    PTREE right;                // - right argument
    TOKEN_LOCN init_locn;       // - initialization location

    left = expr->u.subtree[0];
    right = expr->u.subtree[1];
    PTreeFree( PTreeExtractLocn( expr, &init_locn ) );
    return initClass( left, right, diagnosis, &init_locn );
}


static void warnBoolAssignment( // WARN IF ASSIGNMENT IN BOOLEAN EXPRESSION
    PTREE expr )                // - expression
{
    if( NodeIsBinaryOp( expr, CO_EQUAL ) ) {
        if( NodeIsConstantInt( PTreeOpRight( expr ) ) ) {
            PTreeWarnExpr( expr, WARN_ASSIGN_CONST_IN_BOOL_EXPR );
        } else {
            PTreeWarnExpr( expr, WARN_ASSIGN_VALUE_IN_BOOL_EXPR );
        }
    }
}


static void warnBoolConstVal(   // WARNING FOR BOOLEAN CONSTANT VALUES
    int value,                  // - value
    PTREE err_expr,             // - expression for error
    PTREE expr )                // - expression
{
    if( NodeIsUnaryOp( err_expr, CO_EXCLAMATION ) ) {
        value = ( 0 == value );
        expr = err_expr;
    }
    if( 0 == value ) {
        PTreeWarnExpr( expr, WARN_ALWAYS_FALSE );
    } else {
        PTreeWarnExpr( expr, WARN_ALWAYS_TRUE );
    }
}


static void warnBoolConst(      // WARN IF CONSTANT BOOLEAN EXPRESSION
    PTREE expr,                 // - expression
    PTREE err_expr )            // - expression for error
{
    if( expr->flags & PTF_PTR_NONZERO ) {
        warnBoolConstVal( 1, err_expr, expr );
    }
}


static PTREE convertInitBare(   // CONVERT type(val) TO val
    PTREE expr )                // - initialization expresion
{
    PTREE right;                // - right operand

    right = PTreeOpRight( expr );
    if( NodeIsBinaryOp( right, CO_LIST ) ) {
        *( PTreeRefRight( expr ) ) = right->u.subtree[1];
        right->u.subtree[1] = NULL;
        if( right->u.subtree[0] != NULL ) {
            PTreeErrorExpr( right, ERR_ONE_CTOR_ARG_REQD );
        }
        NodeFreeDupedExpr( right );
        right = PTreeOpRight( expr );
    }
    return right;
}


static PTREE* nextFuncNode(     // GET NEXT NODE IN FUNCTION TRAVERSAL
    PTREE expr )                // - current node
{
    PTREE* retn;                // - addr[ next node ]

    if( NodeIsBinaryOp( expr, CO_COMMA ) ) {
        retn = &expr->u.subtree[1];
    } else if( expr->op == PT_DUP_EXPR ) {
        retn = &expr->u.dup.subtree[0];
    } else {
        retn = NULL;
    }
    return retn;
}


enum                            // USED TO INDICATE TYPE OF '&' RESOLUTION
{   ADDRFN_RESOLVE_MANY     =1  // - overloading '&func' is ok
,   ADDRFN_RESOLVE_MANY_USE =2  // - overloading 'func' is ok
,   ADDRFN_RESOLVE_ONE      =4  // - resolve 'func' as used
,   ADDRFN_MEMBPTR_KLUGE    =8  // - kluge for MFC (allow membptr without ::)
};


static bool analyseStaticFunc(  // ANALYSE GOOD REFERENCE TO STATIC FUNC(S)
    TYPE result_type,           // - NULL or type to be zapped in
    PTREE* root,                // - root of expression
    PTREE func,                 // - function node found
    unsigned resolution )       // - kind of resolution
{
    PTREE expr;                 // - current node being analysed
    PTREE* prune;               // - point at which to prune
    bool retn;                  // - return: TRUE ==> static fun is ok

    if( NULL == func ) {
        retn = TRUE;
    } else {
        SYMBOL funsym = func->u.symcg.symbol;
//      funsym->flag |= SF_REFERENCED;
        if( SymIsThisFuncMember( funsym ) ) {
            if( (func->flags & PTF_COLON_QUALED) || (resolution & ADDRFN_MEMBPTR_KLUGE) ) {
                retn = TRUE;
            } else {
                PTreeErrorExprSymInf( *root
                                    , ERR_ADDR_NONSTAT_MEMBER_FUNC
                                    , funsym );
                retn = FALSE;
            }
        } else {
            funsym->flag |= SF_REFERENCED;
            prune = root;
            for( ; prune != NULL; ) {
                expr = *prune;
                if( expr == func ) break;
                if( NodeIsUnaryOp( expr, CO_ADDR_OF ) ) {
                    prune = &expr->u.subtree[0];
                } else if( NodeIsBinaryOp( expr, CO_DOT )
                        || NodeIsBinaryOp( expr, CO_ARROW ) ) {
//
// note: next version, use comma since we can't throw away the LHS, according
//       to revised standard.
//
                    *prune = NodePruneLeftTop( expr );
                } else {
                    prune = nextFuncNode( expr );
                }
            }
            for( expr = *root; ; ) {
                if( expr == func ) break;
                expr->flags |= PTF_PTR_NONZERO;
                if( NULL != result_type ) {
                    expr->type = result_type;
                }
                prune = nextFuncNode( expr );
                if( prune == NULL ) break;
                expr = *prune;
            }
            retn = TRUE;
        }
    }
    return retn;
}


// A function may appear to be overloaded, because of default arguments. This
// routine finds the symbol which is not a default symbol.
//
static bool resolveActualAddrOf(// RESOLVE &func FOR ACTUAL NON-OVERLOAD
    PTREE node )                // - node for lookup
{
    SYMBOL func;                // - function from lookup
    bool retn;                  // - return: TRUE ==> all ok

    func = ActualNonOverloadedFunc( node->u.symcg.symbol, node->u.symcg.result );
    if( CNV_OK == ConvertOvFunNode( MakePointerTo( func->sym_type )
                                  , node ) ) {
//      func->flag |= SF_ADDR_TAKEN | SF_REFERENCED;
        func->flag |= SF_ADDR_TAKEN;
        node->flags |= PTF_PTR_NONZERO;
        retn = TRUE;
    } else {
        PTreeErrorNode( node );
        retn = FALSE;
    }
    return retn;
}


static bool analyseAddrOfFunc(  // ANALYSE '&func'
    PTREE* a_expr,              // - addr[ expression ]
    unsigned resolution )       // - ADDRFN_RESOLVE_... bits
{
    PTREE expr;                 // - actual expression
    bool retn;                  // - return: TRUE ==> all ok
    PTREE addrof;               // - '&' node or function node
    PTREE fnode;                // - function node

    expr = PTreeOp( a_expr );
    addrof = expr;
    switch( NodeAddrOfFun( addrof, &fnode ) ) {
      default  :
        retn = TRUE;
        break;
      case ADDR_FN_MANY :
        if( resolution & ADDRFN_RESOLVE_MANY ) {
            retn = analyseStaticFunc( NULL, a_expr, fnode, resolution );
        } else {
            PTreeErrorExpr( fnode, ERR_ADDR_OF_OVERLOADED_FUN );
            PTreeErrorNode( expr );
            retn = FALSE;
        }
        break;
      case ADDR_FN_MANY_USED :
        if( resolution & ADDRFN_RESOLVE_MANY_USE ) {
            retn = analyseStaticFunc( NULL, a_expr, fnode, resolution );
        } else {
            PTreeErrorExpr( fnode, ERR_ADDR_OF_OVERLOADED_FUN );
            PTreeErrorNode( expr );
            retn = FALSE;
        }
        break;
      case ADDR_FN_ONE :
        if( resolution & ADDRFN_RESOLVE_ONE ) {
            if( resolveActualAddrOf( fnode ) ) {
                SYMBOL sym;     // - symbol for function
                sym =  fnode->u.symcg.symbol;
                if( (fnode->flags & PTF_COLON_QUALED) && ( SymIsThisFuncMember( sym ) ) ) {
                    expr->type = MakeMemberPointerTo( SymClass( sym ), fnode->type );
                    expr->flags |= PTF_PTR_NONZERO;
                    retn = TRUE;
                } else {
                    retn = analyseStaticFunc( MakePointerTo( fnode->type )
                                            , a_expr
                                            , fnode
                                            , resolution );
                }
            } else {
                PTreeErrorNode( expr );
                retn = FALSE;
            }
        } else {
            retn = analyseStaticFunc( NULL, a_expr, fnode, resolution );
        }
        break;
      case ADDR_FN_ONE_USED :
        if( resolveActualAddrOf( fnode ) ) {
            retn = analyseStaticFunc( fnode->type
                                    , a_expr
                                    , fnode
                                    , resolution );
        } else {
            PTreeErrorNode( expr );
            retn = FALSE;
        }
        break;
    }
    return retn;
}


static bool isInitRef(          // DETERMINE IF INITIALIZATION REFERENCE
    PTREE node )                // - LVALUE node
{
    return ( PTreeOpFlags( node ) & PTO_RVALUE )
        && ( NULL != TypeReference( node->u.subtree[0]->type ) );
}


static void convertInitRef(     // CONVERT SUBTREE TO INITIALIZATION REFER.
    PTREE *ref )                // - reference[ subtree to be converted ]
{
    PTREE old;                  // - old node
    PTREE init;                 // - init node

    ref = PTreeRef( ref );
    old = *ref;
    if( isInitRef( old ) ) {
        init = old->u.subtree[0];
        *ref = init;
        PTreeFree( old );
        init = NodeRemoveCasts( init );
        if( init->cgop == CO_NAME_PARM_REF ) {
            if( init->u.symcg.symbol == NULL ) {
                init->cgop = CO_NAME_THIS;
            } else {
                init->cgop = CO_NAME_NORMAL;
            }
        }
    }
}


static bool reqdBoolOperand(    // VERIFY A BOOLEAN OPERAND
    PTREE operand )
{
    TYPE type;                  // - operand type
    bool retn;                  // - return: TRUE ==> is a bool operand

    type = operand->type;
    if( ( NULL != ArithType( type ) )
      ||( NULL != PointerTypeEquivalent( type ) )
      ||( NULL != MemberPtrType( type ) ) ) {
        retn = TRUE;
    } else {
        exprError( operand, ERR_NOT_BOOLEAN );
        retn = FALSE;
    }
    return retn;
}


static unsigned getConstBits(   // GET SIGNIFICANT BITS IN CONSTANT NODE
    PTREE con )                 // - constant node
{
#if defined( WATCOM_BIG_ENDIAN )
#   error fix algorithm for Big Endian
#endif
    signed_64 value;            // - constant value
    unsigned sig;               // - used to test significant bits
    unsigned bits;              // - number of bits

    value = con->u.int64_constant;
    if( SignedIntType( con->type ) && value.u.sign.v ) {
        U64Neg( &value, &value );
    }
    if( 0 == value.u._32[I64HI32] ) {
        bits = 0;
        sig = value.u._32[I64LO32];
    } else {
        bits = 32;
        sig = value.u._32[I64HI32];
    }
    for( ; sig != 0; ++bits ) {
        sig >>= 1;
    }
    return bits;
}


static unsigned getConstBitsType( // GET NUMBER OF SIGNIFICANT BITS FOR A TYPE
    TYPE type )                 // - the type
{
    if( BoolType( type ) ) {
        return( 1 );
    }
    return CgMemorySize( type ) * TARGET_BITS_CHAR;
}

static bool truncDueToPromotion(// SEE IF TRUNCATION CAN BE DUE TO DEFAULT PROMOTIONS
    PTREE node,                 // - source node
    TYPE tgt_type )             // - target type
{
    TYPE src_type;
    TYPE promote_type;

    tgt_type = TypedefModifierRemoveOnly( tgt_type );
    if( tgt_type->id == TYP_BITFIELD ) {
        tgt_type = tgt_type->of;
    }
    promote_type = TypeBinArithResult( tgt_type, tgt_type );
    src_type = TypedefModifierRemoveOnly( node->type );
    return( TypesIdentical( promote_type, src_type ) );
}

static unsigned getNumBitFieldBits( PTREE node ) // GET NUM BITS FOR BITFIELD
{
    TYPE type;

    DbgAssert( NodeIsUnaryOp( node, CO_BITFLD_CONVERT ) );
    type = node->u.subtree[0]->type;
    type = TypedefModifierRemoveOnly( type );   // not needed?
    return( type->u.b.field_width );
}


static void warnIntTrunc(       // WARN WHEN INTEGER TRUNCATION
    PTREE node,                 // - node to be tested
    TYPE tgt_type,              // - target type (may be TYP_BITFIELD)
    unsigned tgt_bits )         // - # bits (maximum) in target
{
    unsigned bits;              // - bits in value
    MSG_NUM warning;            // - warning msg to use
    unsigned right_bits;        // - # bits in rhs expr
    PTREE constant;             // - constant node
    PTREE colon;                // - colon node
    TYPE node_type;             // - node type
    TYPE left_type;             // - node's left subtree type
    TYPE right_type;            // - node's right subtree type
    struct {
        unsigned exact_RHS_bits:1;// - we know RHS bits precisely
    } flag;

    flag.exact_RHS_bits = 0;
    constant = NodeGetConstantNode( node );
    if( constant != NULL ) {
        bits = getConstBits( constant );
        flag.exact_RHS_bits = 1;
        warning = WARN_TRUNC_INT_CONSTANT;
    } else {
        if( NodeIsBinaryOp( node, CO_QUESTION ) ) {
            colon = node->u.subtree[1];
            constant = NodeGetConstantNode( PTreeOpLeft( colon ) );
            if( NULL == constant ) {
                bits = getConstBitsType( node->type );
                warning = WARN_TRUNC_INT_VALUE;
            } else {
                bits = getConstBits( constant );
                flag.exact_RHS_bits = 1;
                constant = NodeGetConstantNode( PTreeOpRight( colon ) );
                if( constant == NULL ) {
                    bits = getConstBitsType( node->type );
                    warning = WARN_TRUNC_INT_VALUE;
                } else {
                    right_bits = getConstBits( constant );
                    if( right_bits > bits ) {
                        bits = right_bits;
                    }
                    warning = WARN_TRUNC_INT_CONSTANT;
                }
            }
        } else if( node->flags & PTF_BOOLEAN ) {
            bits = 1;
            warning = WARN_TRUNC_INT_VALUE;
        } else {
            node_type = node->type;
            if( node->op == PT_BINARY ) {
                switch( node->cgop ) {
                case CO_OR:
                case CO_AND:
                case CO_XOR:
                    // these binary ops don't create more bits
                    // so promotion to int confuses things
                    left_type = node->u.subtree[0]->type;
                    bits = getConstBitsType( left_type );
                    right_type = node->u.subtree[1]->type;
                    right_bits = getConstBitsType( right_type );
                    if( right_bits > bits ) {
                        bits = right_bits;
                    }
                    break;
                default:
                    bits = getConstBitsType( node_type );
                }
            } else if( node->op == PT_UNARY ) {
                switch( node->cgop ) {
                case CO_BITFLD_CONVERT :
                    bits = getNumBitFieldBits( node );
                    break;
                case CO_TILDE:
                case CO_UPLUS:
                    // these unary ops don't create more bits
                    // so promotion to int confuses things
                    node_type = node->u.subtree[0]->type;
                    // fall through
                default :
                    if( node->cgop == CO_FETCH && node->u.subtree[0] != NULL
                     && NodeIsUnaryOp( node->u.subtree[0], CO_BITFLD_CONVERT ) ) {
                        bits = getNumBitFieldBits( node->u.subtree[0] );
                        flag.exact_RHS_bits = 1;
                    } else {
                        bits = getConstBitsType( node_type );
                    }
                    break;
                }
            } else {
                bits = getConstBitsType( node_type );
            }
            warning = WARN_TRUNC_INT_VALUE;
        }
    }
    if( ! flag.exact_RHS_bits ) {
        // see if LHS is a bit-field; we can't do much checking because
        // we'll produce annoying warnings that can't be coded around
        tgt_type = TypedefModifierRemoveOnly( tgt_type );
        if( tgt_type->id == TYP_BITFIELD ) {
            tgt_bits = getConstBitsType( tgt_type->of );
        }
    }
    if( tgt_bits < bits ) {
        if( warning == WARN_TRUNC_INT_VALUE ) {
            if( truncDueToPromotion( node, tgt_type ) ) {
                PTreeWarnExpr( node, WARN_TRUNC_INT_VALUE_PROMOTED );
            } else {
                PTreeWarnExpr( node, warning );
            }
        } else {
            PTreeWarnExpr( node, warning );
        }
    }
}


void AnalyseIntTrunc(           // ANALYSE INTEGRAL TRUNCATION
    PTREE expr,                 // - expression for error
    TYPE type )                 // - target type
{
    if( IntegralType( expr->type ) && IntegralType( type ) ) {
        warnIntTrunc( expr, type, getConstBitsType( type ) );
    }
}


static bool diagThisMemberFun(  // DIAGNOSE NON-STATIC MEMBER FUNCTION
    PTREE expr,                 // - expression to be tested
    PTREE err_expr )            // - expression being analysed
{
    bool retn;                  // - return: TRUE ==> diagnosed as error
    SYMBOL fun;                 // - potential non-static member fun.

    if( expr->op == PT_SYMBOL ) {
        fun = expr->u.symcg.symbol;
        if( SymIsThisFuncMember( fun ) ) {
            PTreeErrorExprSym( expr, ERR_CANT_BE_MEMB_FUN, fun );
            PTreeErrorNode( err_expr );
            retn = TRUE;
        } else {
            retn = FALSE;
        }
    } else {
        retn = FALSE;
    }
    return retn;
}


static bool allowClassCastAsLValue( PTREE *p_expr )
{
    TYPE class_type = StructType( NodeType( *p_expr ) );
    bool retn;

    if( class_type == NULL ) {
        retn = FALSE;
    } else {
        *p_expr = NodeForceLvalue( *p_expr );
        retn = TRUE;
    }
    return retn;
}


static TYPE findCommonPtrNullType( PTREE expr )
{
    TYPE expr_type;
    TYPE type;
    type_flag mod;
    type_flag default_mod;

    /*
        handles b ? &i : 0 when the default memory model for a pointer to
        'i' is far but 'i' is near (the 0 must be NULL in the default memory
        model!)
    */
    expr_type = expr->type;
    type = TypedefModifierRemoveOnly( expr_type );
    if( PointerType( type ) == NULL ) {
        return expr_type;
    }
    type = TypeModFlagsEC( type->of, &mod );
    default_mod = DefaultMemoryFlag( type );
    if( default_mod & ( TF1_FAR | TF1_HUGE ) ) {
        if( mod & TF1_NEAR ) {
            // memory models are different
            mod &= ~TF1_MEM_MODEL;
            mod |= default_mod;
            type = MakeModifiedType( type, mod );
            expr_type = MakeFarPointerToNear( type );
        }
    }
    if( ( mod ^ default_mod ) & TF1_MEM_MODEL ) {
        // memory models are different
        mod &= ~TF1_MEM_MODEL;
        mod |= default_mod;
        type = MakeModifiedType( type, mod );
        expr_type = MakePointerTo( type );
    }
    return expr_type;
}


static void checkRetnOpt(       // CHECK IF RETURN OPTIMIZATION IS OK
    PTREE expr )                // - return expression
{
    DbgVerify( NodeIsBinaryOp( expr, CO_RETURN )
             , "checkRetnOpt: not return" );
    if( FnRetnOpt() ) {
        if( expr->u.subtree[1]->flags & PTF_SIDE_EFF ) {
            FnRetnOptOff();
        } else {
            PTREE right = PTreeOpRight( expr );
            if( right->op == PT_SYMBOL ) {
                SYMBOL sym = right->u.symcg.symbol;
                SYMBOL retn_opt_sym = FnRetnOptSym();
                if( retn_opt_sym == NULL ) {
                    if( FnRetnOptimizable( sym ) ) {
                        FnRetnOptSetSym( sym );
                    } else {
                        FnRetnOptOff();
                    }
                } else if( sym != retn_opt_sym ) {
                    FnRetnOptOff();
                }
            } else {
                FnRetnOptOff();
            }
        }
    }
}


static PTREE convertIncDec(     // CONVERT ++, -- operands
    CGOP new_op,                // - transformed opcode
    PTREE expr,                 // - expression
    PTREE left )                // - left operand
{
    TYPE type;                  // - result type

    type = NULL;
    switch( expr->cgop ) {
      case CO_PRE_MINUS_MINUS:
      case CO_PRE_PLUS_PLUS:
        type = NodeType( left );
        break;
      case CO_POST_MINUS_MINUS:
      case CO_POST_PLUS_PLUS:
        type = TypeReferenced( left->type );
        break;
    }
    expr = NodeSetType( expr, type, 0 );
    expr->cgop = new_op;
    expr->op = PT_BINARY;
    return expr;
}

ExtraRptTable( ctrOps, CO_LOG, 1 );

#ifdef XTRA_RPT

static void init( INITFINI* defn )  // initialization
{
    static const char * const opNames[] = {
#define PPOPCD( id ) #id ,
#include "ppopsdef.h"
    };
    defn = defn;
    ExtraRptRegisterTab( "Frequency of operators", opNames, &ctrOps[0][0], CO_LOG, 1 );
}

INITDEFN( analyse_reports, init, InitFiniStub )
#endif

PTREE AnalyseOperator(          // ANALYSE AN OPERATOR
    PTREE expr )                // - expression
{
    PTREE left;                 // - left operand
    PTREE right;                // - right operand
    PTREE temp;                 // - temporary operand
    TYPE type;                  // - type to be used
    unsigned index_left;        // - operand index (left)
    unsigned index_right;       // - operand index (right)
    unsigned index;             // - operands index
    OPR_RTN_CODE action_code;   // - code for actions
    TEMP_TYPE temp_class;       // - SC_... for next temporary
    OPAC *ap;                   // - actions pointer
    PTREE templ;
    OPAC *opac_memb_ptr_ext;    // - operator-specific analysis string
                                //   for member-ptr extension

    {   bool opsok;             // - indicates analysis ok
        PTREE on_left;          // - operand on left
        PTREE on_right;         // - operand on right
        PTREE orig;             // - original node (avoid use of "expr")
        PTO_FLAG flags;         // - flags for operator

        flags = PTreeOpFlags( expr );
        orig = expr;
        templ = NULL;
//      addr_left = &orig->u.subtree[0];
        opsok = TRUE;
        ExtraRptTabIncr( ctrOps, orig->cgop, 0 );
        on_left = orig->u.subtree[0];
        if( ( on_left != NULL ) && ( on_left->cgop == CO_TEMPLATE ) ) {
            // remove template parameters
            templ = on_left;
            on_left = orig->u.subtree[0] = on_left->u.subtree[0];
            templ->u.subtree[0] = NULL;
        }
        if( flags & PTO_UNARY ) {
            if( orig->cgop == CO_ADDR_OF || orig->cgop == CO_INDIRECT ) {
                if( (on_left->flags & PTF_LV_CHECKED) == 0 ) {
                    opsok = AnalyseLvalueAddrOf( &orig->u.subtree[0] );
                }
            } else if( on_left != NULL && (on_left->flags & PTF_LV_CHECKED) == 0 ) {
                opsok = AnalyseLvalue( &orig->u.subtree[0] );
            }
        } else {
            if( on_left != NULL && (on_left->flags & PTF_LV_CHECKED) == 0 ) {
                opsok = AnalyseLvalue( &orig->u.subtree[0] );
            }
            if( flags & PTO_BINARY ) {
                on_right = orig->u.subtree[1];
                if( on_right != NULL && (on_right->flags & PTF_LV_CHECKED) == 0 ) {
                    if( !AnalyseLvalue( &orig->u.subtree[1] ) ) {
                        opsok = FALSE;
                    }
                }
            }
        }
        if( !opsok ) {
            PTreeErrorNode( orig );
            return orig;
        }
        if( flags & PTO_CAN_OVERLOAD ) {
            orig = OverloadOperator( orig );
            if( orig->op == PT_ERROR ) {
                return orig;
            }
        }
        left = PTreeOpLeft( orig );
        index_left = classify_operand( left );
        if( PT_BINARY == orig->op ) {
            right = PTreeOpRight( orig );
            index_right = classify_operand( right );
            orig->flags |= (PTreeEffFlags( left ) | PTreeEffFlags( right )) & (PTF_SIDE_EFF | PTF_MEANINGFUL);
        } else {
            right = NULL;
            orig->u.subtree[1] = NULL;      // robustness addition
            index_right = index_left;
            orig->flags |= PTreeEffFlags( left ) & (PTF_SIDE_EFF | PTF_MEANINGFUL);
        }
        expr = orig;
    }
    index = index_left + index_left + index_right;
    if( index > OPCL_OTHER ) {
        index = OPCL_OTHER;
    }
    if( left == NULL ) {
        type = NULL;
    } else {
        left->type = BindTemplateClass( left->type, &left->locn, FALSE );
        type = TypedefModifierRemoveOnly( left->type );
    }
    if( right != NULL ) {
        right->type = BindTemplateClass( right->type, &right->locn, FALSE );
    }
    opac_memb_ptr_ext = NULL;
    temp_class = TEMP_TYPE_NONE;
    action_code = opr_rtn_table[ expr->cgop ][ index ];
    for( ap = opcomb_actions[ action_code ]; *ap < OPAC_END ; ++ap ) {
        // The following label is goto'ed when a new string is set for
        // analysis.
start_opac_string:
        // The following cases "break" when an error is detected, and
        // will "continue" when successful
        switch( *ap ) {
          case ERR__IMPOSSIBLE :
            PTreeErrorExpr( expr, ERR_FRONT_END );
            type = NULL;
            break;
          case ERR__NOT_IMPL :       // ------- remove later on ------
            PTreeErrorExpr( expr, ERR_NOT_IMPLEMENTED );
            type = NULL;
            break;
          case ERR__BOTH_PTR :
            type = operandError( expr, ERR_BOTH_PTRS );
            break;
          case ERR__SUB_AP :
            type = operandError( expr, ERR_SUB_PTR_FROM_ARITH );
            break;
          case ERR__ONLY_AR :
            type = analyse_err( expr, index_left, &errs_not_arith );
            break;
          case ERR__ONLY_AP :
            if( expr->op == PT_UNARY ) {
                type = operandError( expr, ERR_NOT_PTR_ARITH );
            } else if( ( index_left == OPCL_ARITH ) || ( index_left == OPCL_PTR ) ) {
                type = operandError( expr, ERR_RIGHT_NOT_PTR_ARITH );
            } else {
                type = operandError( expr, ERR_LEFT_NOT_PTR_ARITH );
            }
            break;
          case ERR__ONLY_IN :
            type = analyse_err( expr, index_left, &errs_not_int );
            break;
          case ERR__AP_ASSN :
            type = operandError( expr, ERR_PTR_TO_ARITH_ASSIGNMENT );
            break;
          case ERR__NO_PTR :
            type = operandError( expr, ERR_EXPR_MUST_BE_POINTER_TO );
            break;
          case ERR__INDEX :
            type = operandError( expr, ERR_EXPR_MUST_BE_ARRAY );
            break;
          case ERR__PP_INDEX :
            type = operandError( expr, ERR_INDEX_MUST_BE_INTEGRAL );
            break;
          case ERR__NOT_FUN :
            type = operandError( expr, ERR_NOT_A_FUNCTION );
            break;
          case ERR__SEGOP :
            type = operandError( expr, ERR_SEGOP_OPERANDS );
            break;
          case DIAG_FUNC_LEFT :
            if( index_left != OPCL_PTR )
                continue;
#if 0
            if( analyseAddrOfFunc( &expr->u.subtree[0]
                                 , ADDRFN_RESOLVE_MANY_USE ) ) {
#else
            if( analyseAddrOfFunc( &expr->u.subtree[0]
                                 , ADDRFN_RESOLVE_ONE ) ) {
#endif
                if( (left->flags & PTF_CALLED_ONLY) == 0 ) {
                    left = PTreeOpLeft( expr );
                    continue;
                }
                PTreeErrorExpr( left, ERR_MEMB_PTR_FUNC_NOT_CALLED );
            }
            break;
          case DIAG_FUNC_MANY1 :
          { unsigned resolution;    // - used for resolution of func. addr
            if( index_right != OPCL_PTR )
                continue;
            resolution = ADDRFN_RESOLVE_ONE
                       | ADDRFN_RESOLVE_MANY
                       | ADDRFN_RESOLVE_MANY_USE;
            // KLUGE FOR MFC
            if( CompFlags.extensions_enabled
             && NULL != MemberPtrType( expr->u.subtree[0]->type ) ) {
                 resolution |= ADDRFN_MEMBPTR_KLUGE;
            }
            // END KLUGE FOR MFC
            if( ! analyseAddrOfFunc( &expr->u.subtree[1], resolution ) )
                break;
            right = PTreeOpRight( expr );
            if( right->flags & PTF_CALLED_ONLY ) {
                PTreeErrorExpr( right, ERR_MEMB_PTR_FUNC_NOT_CALLED );
                break;
            }
          } continue;
          case DIAG_FUNC_MANY2 :
          { unsigned resolution;    // - used for resolution of func. addr
            if( index_right != OPCL_PTR )
                continue;
            resolution = ADDRFN_RESOLVE_ONE
                       | ADDRFN_RESOLVE_MANY
                       | ADDRFN_RESOLVE_MANY_USE;
            // KLUGE FOR MFC
            if( CompFlags.extensions_enabled ) {
                 resolution |= ADDRFN_MEMBPTR_KLUGE;
            }
            // END KLUGE FOR MFC
            if( ! analyseAddrOfFunc( &expr->u.subtree[1], resolution ) )
                break;
            right = PTreeOpRight( expr );
            if( right->flags & PTF_CALLED_ONLY ) {
                PTreeErrorExpr( right, ERR_MEMB_PTR_FUNC_NOT_CALLED );
                break;
            }
          } continue;
          case DIAG_FUNC_RIGHT_ONE :
            if( index_right != OPCL_PTR )
                continue;
            if( ! analyseAddrOfFunc( &expr->u.subtree[1], ADDRFN_RESOLVE_ONE ) ) break;
            if( right->flags & PTF_CALLED_ONLY ) {
                PTreeErrorExpr( right, ERR_MEMB_PTR_FUNC_NOT_CALLED );
                break;
            }
            continue;
          case DIAG_VOID_LEFT :
            if( VoidType( left->type ) ) {
                type = operandError( expr, ERR_EXPR_IS_VOID );
                break;
            }
            continue;
          case DIAG_VOID_RIGHT :
            if( VoidType( right->type ) ) {
                type = operandError( expr, ERR_EXPR_IS_VOID );
                break;
            }
            continue;
          case REQD_INT_LEFT :
            if( IntegralType( TypeReferenced( type ) ) != NULL )
                continue;
            analyse_err_left( expr
                            , ERR_EXPR_MUST_BE_INTEGRAL
                            , ERR_LEFT_EXPR_MUST_BE_INTEGRAL );
            break;
          case REQD_INT_RIGHT :
            if( IntegralType( TypeReferenced( right->type ) ) != NULL )
                continue;
            operandError( expr, ERR_RIGHT_EXPR_MUST_BE_INTEGRAL );
            break;
          case REQD_BOOL_LHS_ASSIGN :
            if( BoolType( left->type ) != NULL )
                continue;
            operandError( expr, ERR_PTR_TO_ARITH_ASSIGNMENT );
            break;
          case REQD_LVALUE_LEFT :
            if( left->flags & PTF_LVALUE )
                continue;
            if( allowClassCastAsLValue( &expr->u.subtree[0] ) ) {
                left = PTreeOpLeft( expr );
                left->type = BindTemplateClass( left->type, &left->locn,
                                                FALSE );
                type = TypedefModifierRemoveOnly( left->type );
                continue;
            }
            if( expr->op == PT_BINARY ) {
                LvalueErrLeft( left, expr );
            } else if( NodeIsUnaryOp( expr, CO_ADDR_OF ) ) {
                PTreeErrorExpr( expr, ERR_CANT_TAKE_ADDR_OF_RVALUE );
            } else {
                LvalueErr( left, expr );
            }
            break;
          case REQD_NOT_ENUM_LEFT :
            if( EnumType( left->type ) == NULL )
                continue;
            operandError( expr, ERR_BAD_ENUM_ASSIGNMENT );
            if( expr->op != PT_ERROR )
                continue;
            break;
#if 0
          case REQD_NOT_BOOL_LEFT :
            if( BoolType( left->type ) == NULL )
                continue;
            operandError( expr, ERR_BAD_BOOL_ASSIGNMENT );
            break;
#endif
          case REQD_ENUM_SAME :
          { TYPE type_l;    // - left type
            type_l = EnumType( left->type );
            if( type_l == NULL )
                continue;
            if( type_l == EnumType( right->type ) )
                continue;
            operandError( expr, ERR_BAD_ENUM_ASSIGNMENT );
            if( expr->op != PT_ERROR ) {
                continue;
            }
          } break;
          case REQD_ZERO_LEFT :
            if( ! NodeIsZeroIntConstant( left ) ) {
                exprError( left, ERR_NOT_PTR_OR_ZERO );
                break;
            }
            if( NodeIsBinaryOp( expr, CO_COLON ) ) {
                TYPE common_ptr_type;
                common_ptr_type = findCommonPtrNullType( right );
                if( common_ptr_type != right->type ) {
#if 0
                    ConvertExpr( &expr->u.subtree[0], common_ptr_type, CNV_EXPR );
                    ConvertExpr( &expr->u.subtree[1], common_ptr_type, CNV_EXPR );
#else
                    expr->u.subtree[0] = CastImplicit( expr->u.subtree[0]
                                                     , common_ptr_type
                                                     , CNV_EXPR
                                                     , NULL );
                    expr->u.subtree[1] = CastImplicit( expr->u.subtree[1]
                                                     , common_ptr_type
                                                     , CNV_EXPR
                                                     , NULL );
#endif
                    left = PTreeOpLeft( expr );
                    right = PTreeOpRight( expr );
                    type = TypedefModifierRemoveOnly( left->type );
                    continue;
                }
            }
#if 0
            ConvertExpr( &expr->u.subtree[0], right->type, CNV_EXPR );
#else
            expr->u.subtree[1] = NodeRvalue( expr->u.subtree[1] );
            if( expr->u.subtree[1]->op == PT_ERROR ) {
                PTreeErrorNode( expr );
                break;
            }
            right = PTreeOpRight( expr );
            expr->u.subtree[0] = CastImplicit( expr->u.subtree[0]
                                             , right->type
                                             , CNV_EXPR
                                             , NULL );
#endif
            left = PTreeOpLeft( expr );
            type = right->type;
            continue;
          case REQD_ZERO_RIGHT :
            if( ( NodeIsBinaryOp( expr, CO_EQUAL ) ||
                  NodeIsBinaryOp( expr, CO_INIT  ) )
              &&( 0 != TypeIsBasedPtr( type ) ) ) {
                continue;
            }
            if( ! NodeIsZeroIntConstant( right ) ) {
                exprError( right, ERR_NOT_PTR_OR_ZERO );
                break;
            }
            if( NodeIsBinaryOp( expr, CO_COLON ) ) {
                TYPE common_ptr_type;
                common_ptr_type = findCommonPtrNullType( left );
                if( common_ptr_type != left->type ) {
#if 0
                    ConvertExpr( &expr->u.subtree[0], common_ptr_type, CNV_EXPR );
                    ConvertExpr( &expr->u.subtree[1], common_ptr_type, CNV_EXPR );
#else
                    expr->u.subtree[0] = CastImplicit( expr->u.subtree[0]
                                                     , common_ptr_type
                                                     , CNV_EXPR
                                                     , NULL );
                    expr->u.subtree[1] = CastImplicit( expr->u.subtree[1]
                                                     , common_ptr_type
                                                     , CNV_EXPR
                                                     , NULL );
#endif
                    left = PTreeOpLeft( expr );
                    right = PTreeOpRight( expr );
                    type = TypedefModifierRemoveOnly( left->type );
                    continue;
                }
#if 0
                ConvertExpr( &expr->u.subtree[1], left->type, CNV_EXPR );
#else
                expr->u.subtree[1] = CastImplicit( expr->u.subtree[1]
                                                 , left->type
                                                 , CNV_EXPR
                                                 , NULL );
#endif
                right = PTreeOpRight( expr );
                continue;
            }
#if 0
            ConvertExpr( &expr->u.subtree[1], left->type, CNV_EXPR );
#else
            expr->u.subtree[1] = CastImplicit( expr->u.subtree[1]
                                             , left->type
                                             , CNV_EXPR
                                             , NULL );
#endif
            right = PTreeOpRight( expr );
            continue;
          case REQD_PTR_SCALES_LEFT :
            left->type = BindTemplateClass( left->type, &left->locn, TRUE );
            if( ptr_scales( left ) )
                continue;
            analyse_err_left( expr
                            , ERR_PTR_SCALES
                            , ERR_PTR_SCALES_LEFT );
            break;
          case REQD_PTR_SCALES_RIGHT :
            right->type = BindTemplateClass( right->type, &right->locn, TRUE );
            if( ptr_scales( right ) )
                continue;
            operandError( expr, ERR_PTR_SCALES_RIGHT );
            break;
          case REQD_NOT_CONST_LEFT :
            if( NodeIsConstant( expr->u.subtree[0] )
             || TypeIsConst( TypeForLvalue( expr->u.subtree[0] ) ) ) {
                PTreeErrorExpr( expr, ERR_MODIFY_CONSTANT );
                break;
            }
            continue;
          case REQD_NOT_CONST_INIT :
            if( !is_ptr_constant( right ) )
                continue;
            if( is_ptr_constant( left ) )
                continue;
            operandError( expr, ERR_CONST_PTR_INIT );
            break;
          case REQD_NOT_ARRAY_LEFT :
            if( ArrayType( type ) == NULL )
                continue;
            operandError( expr, ERR_ARRAY_LEFT );
            break;
          case REQD_NOT_VOID_INDIRECT :
            if( VoidType( type ) == NULL )
                continue;
            operandError( expr, ERR_VOID_INDIRECTION );
            break;
          case REQD_NOT_FUNC_LEFT :
            if( NULL == FunctionDeclarationType( type ) )
                continue;
            analyse_err_left( expr
                            , ERR_CANT_BE_FUNC
                            , ERR_CANT_BE_FUNC_LEFT );
            break;
          case REQD_NOT_FUNC_RIGHT :
            if( NULL == FunctionDeclarationType( right->type ) )
                continue;
            operandError( expr, ERR_CANT_BE_FUNC_RIGHT );
            break;
          case REQD_NOT_MFUN_LEFT :
            if( diagThisMemberFun( left, expr ) )
                break;
            continue;
          case REQD_NOT_MFUN_RIGHT :
            if( diagThisMemberFun( right, expr ) )
                break;
            continue;
          case REQD_DEFD_CLPTR_LEFT :
          { TYPE pointed;       // type pointed at
            pointed = TypedefModifierRemoveOnly( type->of );
            if( pointed == NULL )
                break;
            if( pointed->id != TYP_CLASS )
                continue;
            if( pointed->u.c.info->defined )
                continue;
            exprError( left, ERR_UNDEFED_CLASS_PTR );
          } break;
          case REQD_DEFD_CLPTR_RIGHT :
          { TYPE pointed;       // type pointed at
            pointed = TypedefModifierRemoveOnly( right->type );
            pointed = TypedefModifierRemoveOnly( pointed->of );
            if( pointed == NULL )
                break;
            if( pointed->id != TYP_CLASS )
                continue;
            if( pointed->u.c.info->defined )
                continue;
            exprError( right, ERR_UNDEFED_CLASS_PTR );
          } break;
          case REQD_CLASS_PTR_LEFT :
            if( ( type->id == TYP_POINTER )
              &&( 0 == ( type->flag & TF1_REFERENCE ) )
              &&( NULL != StructType( type->of ) ) )
                continue;
            exprError( left, ERR_NOT_CLASS_PTR );
            PTreeErrorNode( expr );
            break;
          case REQD_CLASS_LEFT :
          { TYPE cltype;        // class type
            if( left->flags & PTF_LVALUE ) {
                cltype = ClassTypeForType( type );
            } else {
                cltype = StructType( type );
            }
            if( NULL == cltype ) {
                exprError( expr, ERR_NOT_CLASS );
                break;
            } else if( ! cltype->u.c.info->defined ) {
                exprError( expr, ERR_UNDEFINED_CLASS_OBJECT );
                break;
            }
          } continue;
          case REQD_BOOL_LEFT :
            if( reqdBoolOperand( left ) )
                continue;
            break;
          case REQD_BOOL_RIGHT :
            if( reqdBoolOperand( right ) )
                continue;
            break;
          case CONV_PP_ASSIGN :
          { type_flag flags_l;      // type flags (left)
            type_flag flags_r;      // type flags (right)
            type_flag flags_bad;    // type flags (missing on left)
            left->type = BindTemplateClass( left->type, &left->locn, FALSE );
            right->type = BindTemplateClass( right->type, &right->locn, FALSE );
            TypePointedAt( right->type, &flags_r );
            TypePointedAt( left->type, &flags_l );
            flags_bad = flags_r & ~ flags_l;
            if( flags_bad & TF1_CONST ) {
                operandError( expr, ERR_LEFT_NOT_CONST_PTR );
                break;
            }
            if( flags_bad & TF1_VOLATILE ) {
                operandError( expr, ERR_LEFT_NOT_VOLATILE_PTR );
                break;
            }
            expr = CastImplicitRight( expr
                                    , NodeType( left )->of
                                    , CNV_ASSIGN
                                    , &diagPtrConvAssign );
            if( expr->op != PT_ERROR ) {
                continue;
            }
          } break;
          case CONV_PP_COMMON :
            expr = CastImplicitCommonPtrExpr( expr
                                            , &diagPtrConvCommon
                                            , FALSE );
            if( PT_ERROR == expr->op )
                break;
            continue;
          case CONV_AA_COMMON :
            expr->u.subtree[0] = NodeConvert( type
                                            , expr->u.subtree[0] );
            expr->u.subtree[1] = NodeConvert( type
                                            , expr->u.subtree[1] );
            left = PTreeOpLeft( expr );
            right = PTreeOpRight( expr );
            continue;
          case CONV_STAR_ADDR_OF :
            expr = NodeStripPoints( expr );
            continue;
          case CONV_SWITCH :
            *PTreeRefRight( expr ) = left;
            *PTreeRefLeft( expr ) = right;
            continue;
          case CONV_CMP_ZERO_LEFT :
            left = expr->u.subtree[0];
            left = NodeConvertToBool( left );
            expr->u.subtree[0] = left;
            if( left->op != PT_ERROR )
                continue;
            PTreeErrorNode( expr );
            break;
          case CONV_CMP_ZERO_RIGHT :
            right = expr->u.subtree[1];
            right = NodeConvertToBool( right );
            expr->u.subtree[1] = right;
            if( right->op != PT_ERROR )
                continue;
            PTreeErrorNode( expr );
            break;
          case CONV_TO_PTR_DIFF :
            type = TypePointerDiff( type );
            expr->type = type;
            expr = NodeBinary( CO_DIVIDE
                              , expr
                              , bld_pted_size( left ) );
            expr = PTreeCopySrcLocation( expr, left );
            continue;
          case CONV_STRIP_TYPE :
            if( type->id == TYP_FUNCTION ) {
                expr->flags |= left->flags & PTF_PTR_NONZERO;
            } else {
                type = CgStripType( type );
            }
            continue;
          case CONV_RVALUE_LEFT :
            left = NodeRvalueLeft( expr );
            left->type = BindTemplateClass( left->type, &left->locn, FALSE );
            type = TypedefModifierRemoveOnly( left->type );
            continue;
          case CONV_RVALUE_RIGHT :
            right = NodeRvalueRight( expr );
            right->type = BindTemplateClass( right->type, &right->locn, FALSE );
            continue;
          case CONV_INDEX :
            DbgAssert( expr->cgop == CO_INDEX );
            expr->cgop = CO_DOT;
            expr->flags |= PTF_WAS_INDEX;
            continue;
          case CONV_TYPE_LEFT :
            left->type = BindTemplateClass( left->type, &left->locn, FALSE );
            type = left->type;
            continue;
          case CONV_TYPE_RIGHT :
            if( right->flags & PTF_LVALUE ) {
                expr->flags |= PTF_LVALUE;
            }
            right->type = BindTemplateClass( right->type, &right->locn, FALSE );
            type = right->type;
            continue;
          case CONV_REFERENCE :
          { TYPE refed;     // referenced type
            refed = TypeReference( type );
            if( refed == NULL )
                continue;
            type = BindTemplateClass( refed, &expr->locn, FALSE );
            expr->flags |= PTF_LVALUE;
          } continue;
          case CONV_MEANINGLESS :
            expr->flags &= ~ PTF_MEANINGFUL;
            continue;
          case CONV_LOCN_RIGHT :
            PTreeExtractLocn( expr->u.subtree[1], &expr->locn );
            continue;
          case CONV_BOOL_ASSIGN :
            if( BoolType( left->type ) ) {
                expr->type = left->type;
                expr = CastImplicitRight( expr
                                        , left->type
                                        , CNV_EXPR
                                        , &diagAssignment );
                if( expr->op == PT_ERROR ) {
                    break;
                }
            }
            continue;
          case CONV_FUN_MP_RIGHT :  // right --> member ptr, if req'd
          { if( !MembPtrExtension( right ) )
                continue;
            expr->u.subtree[1] = MembPtrExtend( expr->u.subtree[1] );
            right = PTreeOpRight( expr );
          } continue;
          case CONV_FUN_MP_LEFT :   // left --> member ptr, if req'd
          { if( !MembPtrExtension( left ) )
                continue;
            expr->u.subtree[0] = MembPtrExtend( expr->u.subtree[0] );
            left = PTreeOpLeft( expr );
            type = TypedefModifierRemoveOnly( left->type );
          } continue;
          case CONV_FUN_MP_COLON :  // SET COLON STRING
            opac_memb_ptr_ext = opac_COLON_OTHER;
            continue;
          case CONV_FUN_MP_CMP :    // SET COMPARISON STRING
            opac_memb_ptr_ext = opac_CMP_MP;
            continue;
          case CONV_FUN_MP_CHECK :  // CHECK FOR MEMBPTR EXTENSION
            if( MembPtrExtension( right ) || MembPtrExtension( left ) ) {
                ap = opac_memb_ptr_ext;
                goto start_opac_string;
            }
            continue;
          case ASSIGN_OTHER :
          { CNV_RETN retn;  // - conversion return
            type = TypedefModifierRemoveOnly( TypeReferenced( type ) );
            type = BindTemplateClass( type, &expr->locn, FALSE );
            if( type->id == TYP_CLASS ) {
                expr = ClassAssign( expr );
            } else if( type->id == TYP_MEMBER_POINTER ) {
                retn = MembPtrAssign( &expr );
                ConversionDiagnose( retn
                                  , expr
                                  , &diagAssignment );
            } else {
                expr->type = left->type;
                expr = CastImplicitRight( expr
                                        , left->type
                                        , CNV_EXPR
                                        , &diagAssignment );
            }
            if( expr->op == PT_ERROR ) {
                break;
            }
          } continue;
          case RESULT_INIT :    // INITIALIZE A NON-CLASS ELEMENT
            expr = CastImplicitRight( expr
                                    , left->type
                                    , CNV_INIT_COPY
                                    , &diagInit );
            if( expr->op == PT_ERROR )
                break;
            type = left->type;
            expr->type = type;
            expr->flags |= PTF_SIDE_EFF | PTF_MEANINGFUL | PTF_LVALUE;
            continue;
          case CHECK_RETN_OPT:  // CHECK RETURN OPTIMIZATION OK
            checkRetnOpt( expr );
            continue;
          case RESULT_RETURN :
          { TYPE type_l;        // - left type
            type = BindTemplateClass( type, &expr->locn, FALSE );
            if( NULL != StructType( type ) ) {
                expr = AnalyseReturnClassVal( expr );
                if( expr->op == PT_ERROR ) {
                    break;
                }
            } else {
                type_l = EnumType( left->type );
                if( type_l != NULL ) {
                    if( type_l != EnumType( right->type ) ) {
                        exprError( expr, ERR_BAD_ENUM_RETURN );
                        break;
                    }
                }
                type_l = PointerTypeEquivalent( type );
                if( ( NULL != type_l )
                  &&( (type_l->flag & TF1_REFERENCE) == 0 )
                  &&( NULL != ArithType( right->type ) )
                  &&( ! NodeIsZeroIntConstant( right ) ) ) {
                    exprError( right, ERR_NOT_PTR_OR_ZERO );
                    break;
                }
                type = left->type;
                expr = AnalyseReturnSimpleVal( expr );
                if( expr->op == PT_ERROR ) {
                    break;
                }
            }
          } continue;
          case RESULT_RETURN_PA :
          { TYPE type_l;        // - left type
            type_l = PointerTypeEquivalent( type );
            if( (type_l->flag & TF1_REFERENCE) == 0 ) {
                if( ! NodeIsZeroIntConstant( right ) ) {
                    exprError( right, ERR_NOT_PTR_OR_ZERO );
                    break;
                }
            }
            type = left->type;
            expr = AnalyseReturnSimpleVal( expr );
            if( expr->op == PT_ERROR ) {
                break;
            }
          } continue;
          case RESULT_RETURN_AA :
          { TYPE type_l;        // - left type
            type_l = EnumType( left->type );
            if( type_l != NULL ) {
                if( type_l != EnumType( right->type ) ) {
                    exprError( expr, ERR_BAD_ENUM_RETURN );
                    break;
                }
            }
            type = left->type;
            expr = AnalyseReturnSimpleVal( expr );
            if( expr->op == PT_ERROR ) {
                break;
            }
          } continue;
          case CONV_INIT_BARE : // CONVERT TYPE(VAL) TO VAL
            right = convertInitBare( expr );
            continue;
          case STATIC_TEMP_SET :    // SET STATIC TEMP'ING
            if( expr->flags & PTF_STATIC_INIT ) {
                temp_class = TemporaryClass( TEMP_TYPE_STATIC );
            } else if( expr->flags & PTF_BLOCK_INIT ) {
                temp_class = TemporaryClass( TEMP_TYPE_BLOCK );
            }
            continue;
          case STATIC_TEMP_RESET :  // RESET STATIC TEMP'ING
            if( temp_class != TEMP_TYPE_NONE ) {
                TemporaryClass( temp_class );
                temp_class = TEMP_TYPE_NONE;
            }
            continue;
          case INIT_CL_MP :     // INIT. A CLASS/MEMB-PTR ELEMENT
          { unsigned retn;      // - conversion return
            if( type->id == TYP_CLASS ) {
                expr = initClassFromExpr( expr, &diagInit );
                if( expr->op == PT_ERROR )
                    break;
                if( TypeExactDtorable( type ) ) {
                    expr->flags |= PTF_DTORABLE_INIT;
                }
            } else if( NULL != ClassTypeForType( type ) ) {
                PTREE node;     // - temporary created
                TOKEN_LOCN locn;// - location
                PTreeExtractLocn( expr, &locn );
                if( NodeIsBinaryOp( right, CO_LIST ) ) {
                    node = right;
                    right = right->u.subtree[1];
                    PTreeFree( node );
                    *PTreeRefRight( expr ) = right;
                }
                expr->type = left->type;
                expr = CastImplicitRight( expr
                                        , left->type
                                        , CNV_INIT_COPY
                                        , &diagInit );
                if( expr->op == PT_ERROR ) {
                    break;
                }
            } else if( type->id == TYP_MEMBER_POINTER ) {
                retn = MembPtrAssign( &expr );
                if( CNV_OK != ConversionDiagnose( retn
                                                , expr
                                                , &diagInit ) ) {
                    break;
                }
            } else {
                right = convertInitBare( expr );
                expr->type = left->type;
                expr = CastImplicitRight( expr
                                        , left->type
                                        , CNV_INIT_COPY
                                        , &diagInit );
                if( expr->op == PT_ERROR ) {
                    break;
                }
            }
          } continue;
          case RESULT_UN_ARITH :
          case RESULT_BIN_SHIFT :
            type = unary_arith_result( left );
            continue;
          case RESULT_CMP_ARITH :
            if( TypesIdentical( left->type, right->type ) ) {
                type = left->type;
                continue;
            }
            // drops thru
          case RESULT_BIN_ARITH :
            type = binary_arith_result( left, right );
            continue;
          case RESULT_ADDR_OF :
          { SYMBOL sym; // - symbol on left or NULL
            SYMBOL drf; // - dereferenced symbol
            TYPE class_type; // - non-NULL if a class type
            PTREE fnode;     // - function node
            TYPE refd;  // - referenced type
            if( NULL != ArrayType( type ) ) {
                PTreeWarnExpr( expr, WARN_ADDR_OF_ARRAY );
            } else if( left->flags & PTF_CALLED_ONLY ) {
                PTreeErrorExpr( left, ERR_MEMB_PTR_FUNC_NOT_CALLED );
                break;
            }
            class_type = StructType( ArrayBaseType( type ) );
            class_type = BindTemplateClass( class_type, &expr->locn, FALSE );
            if( class_type != NULL && ! TypeDefined( class_type ) ) {
                PTreeSetErrLoc( expr );
                CErr2p( WARN_ASSUMING_NO_OVERLOADED_OP_ADDR, class_type );
            }
            if( NodeIsUnaryOp( left, CO_BITFLD_CONVERT ) ) {
                PTreeErrorExpr( expr, ERR_CANT_TAKE_ADDR_OF_BIT_FIELD );
                break;
            }
            if( left->op == PT_SYMBOL ) {
                sym = left->u.symcg.symbol;
                if( sym->id == SC_REGISTER ) {
                    PTreeWarnExpr( expr, WARN_CANT_TAKE_ADDR_OF_REGISTER );
                } else if( sym->id == SC_ENUM ) {
                    PTreeErrorExprSymInf( expr, ERR_CANT_TAKE_ADDR_OF_RVALUE, sym );
                    break;
                }
                sym->flag |= SF_ADDR_TAKEN;
                if( ( left->flags & PTF_COLON_QUALED ) && ( SymIsThisMember( sym ) ) ) {
                    type = MakeMemberPointerTo( SymClass(sym )
                                              , sym->sym_type );
                    expr->flags |= PTF_PTR_NONZERO;
                    continue;
                } else if( SymIsThisFuncMember( sym ) ) {
                    if( CompFlags.extensions_enabled ) {
                        // KLUGE for MFC
                        type = MakeMemberPointerTo( SymClass(sym ), sym->sym_type );
                        expr->flags |= PTF_PTR_NONZERO;
                        continue;
                    } else {
                        PTreeErrorExprSymInf( expr
                                            , ERR_ADDR_NONSTAT_MEMBER_FUNC
                                            , sym );
                        break;
                    }
                }
                fnode = NULL;
                drf = NULL;
            } else {
                sym = NULL;
                if( NodeIsBinaryOp( left, CO_ARROW ) ||
                    NodeIsBinaryOp( left, CO_DOT ) ) {
                    if( left->u.subtree[1]->op == PT_SYMBOL ) {
                        fnode = left->u.subtree[1];
                        drf = fnode->u.symcg.symbol;
                        if( SymIsFunction( drf ) ) {
                            if( SymIsThisFuncMember( drf ) ) {
                                PTreeErrorExprSymInf( expr
                                                    , ERR_ADDR_NONSTAT_MEMBER_FUNC
                                                    , drf );
                                break;
                            }
                        } else {
                            fnode = NULL;
                        }
                    } else {
                        drf = NULL;
                        fnode = NULL;
                    }
                } else {
                    drf = NULL;
                    fnode = NULL;
                }
            }
            refd = TypeReference( type );
            if( refd == NULL ) {
                PTREE dot_beg;
                TYPE ptr_type;
                if( sym != NULL ) {
                    expr->flags |= left->flags & PTF_MEMORY_EXACT;
                }
                for( dot_beg = left
                   ; NodeIsBinaryOp( dot_beg, CO_DOT )
                   ; dot_beg = PTreeOpLeft( dot_beg ) );
                type = TypeAutoDefault( left->type, dot_beg );
                ptr_type = MakePointerTo( type );
                if( fnode != NULL
                 && NULL != FunctionDeclarationType( type ) ) {
                    PTREE dup = expr;
                    if( ! analyseStaticFunc( ptr_type
                                           , &dup
                                           , fnode
                                           , 0 ) )
                        break;
                    expr = dup;
                }
                type = ptr_type;
            } else {
                type = MakePointerTo( refd );
                if( fnode != NULL
                 && NULL != FunctionDeclarationType( refd ) ) {
                    PTREE dup = expr;
                    if( ! analyseStaticFunc( type
                                           , &dup
                                           , fnode
                                           , 0 ) )
                        break;
                    expr = dup;
                }
            }
            expr->flags |= PTF_PTR_NONZERO;
          } continue;
          case RESULT_COMMA :
            warnMeaningfulSideEffect( expr->u.subtree[0] );
            if( NodeIsUnaryOp( expr->u.subtree[0], CO_BITFLD_CONVERT ) ) {
                left = NodeRvalueLeft( expr );
            }
            right = expr->u.subtree[1];
            if( NodeIsUnaryOp( right, CO_BITFLD_CONVERT ) ) {
                expr->u.subtree[1] = right->u.subtree[0];
                expr = AnalyseOperator( expr );
                right->flags |= expr->flags & (PTF_SIDE_EFF | PTF_MEANINGFUL);
                right->u.subtree[0] = expr;
                expr = right;
                type = expr->type;
            } else {
                if( right->flags & PTF_MEANINGFUL ) {
                    expr->flags |= PTF_MEANINGFUL;
                } else {
                    expr->flags &= ~PTF_MEANINGFUL;
                }
            }
            continue;
          case RESULT_TID_EXPR :
            expr = AnalyseTypeidExpr( expr );
            if( expr->op == PT_ERROR )
                break;
            type = expr->type;
            continue;
          case RESULT_TID_TYPE :
            expr = AnalyseTypeidType( expr );
            if( expr->op == PT_ERROR )
                break;
            type = expr->type;
            continue;
          case CONV_INIT_REF :
            convertInitRef( &expr->u.subtree[0] );
            left = PTreeOpLeft( expr );
            type = TypedefModifierRemoveOnly( left->type );
            continue;
          case CONV_CTOR :
            expr = convertCtor( expr );
            if( expr->op == PT_ERROR )
                break;
            type = expr->type;
            continue;
          case RESULT_INCDEC_ARITH :
          { TYPE refed = TypeReferenced( type );
            PTREE node;
            CGOP new_op;
            if( BoolType( refed ) != NULL ) {
                switch( expr->cgop ) {
                  case CO_PRE_MINUS_MINUS:
                  case CO_POST_MINUS_MINUS:
                    PTreeErrorExpr( expr, ERR_CANT_DEC_BOOL );
                    break;
                  case CO_PRE_PLUS_PLUS:
                    new_op = CO_BPRE_BOOL_PLUS_PLUS;
                    break;
                  case CO_POST_PLUS_PLUS:
                    new_op = CO_BPOST_BOOL_PLUS_PLUS;
                    break;
                }
                if( expr->op == PT_ERROR ) {
                    break;
                }
            } else {
                new_op = expr->cgop + CO_BPRE_PLUS_PLUS - CO_PRE_PLUS_PLUS;
            }
            expr = convertIncDec( new_op, expr, left );
            refed = TypedefModifierRemoveOnly( refed );
            node = NodeIntegralConstant( 1, refed );
            *PTreeRefRight( expr ) = node;
            type = expr->type;
          } continue;
          case RESULT_INCDEC_PTR :
          { CGOP new_op;
            new_op = expr->cgop + CO_BPRE_PLUS_PLUS - CO_PRE_PLUS_PLUS;
            expr = convertIncDec( new_op, expr, left );
            *PTreeRefRight( expr ) = bld_pted_size( left );
            type = expr->type;
          } continue;
          case RESULT_PTR_SIZE :
            *PTreeRefRight( expr ) = bld_ptr_adj( right, left );
            continue;
          case RESULT_QUESTMRK :
            left = expr->u.subtree[0];
            expr->flags &= ~ PTF_MEANINGFUL;
            expr->flags |= PTF_FETCH & right->flags;
            type = right->type;
            continue;
          case RESULT_LVALUE :
            expr->flags |= PTF_LVALUE;
            continue;
          case RESULT_INDIRECT :
            expr->flags |= PTF_LVALUE | PTF_MEANINGFUL | (PTF_PTR_NONZERO & left->flags);
            continue;
          case RESULT_BARE :
            expr->flags |= PTF_SIDE_EFF | PTF_MEANINGFUL;
            continue;
          case RESULT_ASSIGN :
            expr->flags |= PTF_SIDE_EFF | PTF_MEANINGFUL;
            if( NodeBitField( expr->u.subtree[0] ) )
                continue;
            expr->flags |= PTF_LVALUE;
            if( CompFlags.plain_char_promotion ) {
                CheckCharPromotion( expr );
            }
            continue;
          case RESULT_BITQUEST :    // (ex-1?bit-1:bit-2) = expr
            if( NodeIsBinaryOp( left, CO_QUESTION ) ) {
                PTREE colon;        // - colon operation
                PTREE opl;          // - left of colon
                PTREE opr;          // - right of colon
                colon = PTreeOpRight( left );
                opl = PTreeOpLeft( colon );
                opr = PTreeOpRight( colon );
                if( NodeBitField( opr ) && NodeBitField( opl ) ) {
                    expr->type = type;
                    expr = NodeBitQuestAssign( expr );
                    expr->flags |= PTF_SIDE_EFF | PTF_MEANINGFUL;
                }
            }
            continue;
          case RESULT_BOOLEAN :
            expr = NodeSetBooleanType( expr );
            type = expr->type;
            continue;
          case RESULT_SIZEOF :
            if( ( type->id == TYP_BITFIELD )
              ||( NodeIsUnaryOp( left, CO_BITFLD_CONVERT ) ) ) {
                PTreeErrorExpr( expr, ERR_CANT_TAKE_SIZEOF_FIELD );
                break;
            }
            if( type->id == TYP_VOID ) {
                PTreeErrorExpr( expr, ERR_CANT_TAKE_SIZEOF_VOID );
                break;
            }
            if( type->id == TYP_FUNCTION ) {
                PTreeErrorExpr( expr, ERR_CANT_TAKE_SIZEOF_FUNC );
                break;
            }
            if( ! TypeDefined( type ) ) {
                exprError( expr, ERR_CANT_TAKE_SIZEOF_UNDEFINED );
                break;
            }
            if( TypeHasSpecialFields( type ) ) {
                PTreeWarnExpr( expr, WARN_CLASS_HAS_SPECIAL_FIELDS );
            }
            if( expr->flags & PTF_SIDE_EFF ) {
                PTreeWarnExpr( expr, WARN_SIZEOF_SIDE_EFFECT );
            }
            temp = expr;
            expr = bld_sizeof_type( type );
            expr = PTreeCopySrcLocation( expr, temp );
            NodeFreeDupedExpr( temp );
            continue;
          case CONV_EXPLICIT_CAST :
            expr = CastExplicit( expr );
            if( expr->op == PT_ERROR )
                break;
            type = NodeType( expr );
            continue;
          case CONV_CONST_CAST :
            expr = CastConst( expr );
            if( expr->op == PT_ERROR )
                break;
            type = NodeType( expr );
            continue;
            break;
          case CONV_DYN_CAST :
            expr = CastDynamic( expr );
            if( expr->op == PT_ERROR )
                break;
            type = NodeType( expr );
            continue;
          case CONV_REINT_CAST :
            expr = CastReint( expr );
            if( expr->op == PT_ERROR )
                break;
            type = NodeType( expr );
            continue;
          case CONV_STATIC_CAST :
            expr = CastStatic( expr );
            if( expr->op == PT_ERROR )
                break;
            type = NodeType( expr );
            continue;
          case RESULT_MINUS_PP :
          { TYPE rtype;         // - type of object on right
            TYPE ltype;         // - type of object on left
            type_flag rflag;    // - flags on right
            type_flag lflag;    // - flags on left
            ltype = TypePointedAt( expr->u.subtree[0]->type, &lflag );
            lflag &= ~ ( TF1_CV_MASK | TF1_MEM_MODEL );
            rtype = TypePointedAt( expr->u.subtree[1]->type, &rflag );
            rflag &= ~ ( TF1_CV_MASK | TF1_MEM_MODEL );
            if( ( lflag == rflag ) && TypesIdentical( rtype, ltype ) ) {
                continue;
            }
            operandError( expr, ERR_BAD_PTR_MINUS_OPERANDS );
          } break;
          case RESULT_CALL :
            if( NULL == FunctionDeclarationType( type ) ) {
                left = NodeRvalueLeft( expr );
            }
            type = TypedefModifierRemoveOnly( left->type );
            if( type->id == TYP_POINTER ) {
                if( type->flag & TF1_REFERENCE ) {
                    type = TypedefModifierRemove( type->of );
                    if( type->id == TYP_POINTER ) {
                        type = TypedefModifierRemove( type->of );
                    }
                } else {
                    type = TypedefModifierRemoveOnly( type->of );
                }
            }
            if( type->id != TYP_FUNCTION ) {
                exprError( expr, ERR_NOT_A_FUNCTION );
                break;
            }
            if( templ != NULL ) {
                // re-insert template parameters
                templ->u.subtree[0] = expr->u.subtree[0];
                expr->u.subtree[0] = templ;
                templ = NULL;
            }
            expr = AnalyseCall( expr, &diagCall );
            if( expr->op == PT_ERROR )
                break;
            continue;
          case RESULT_NEW :
            expr = AnalyseNew( expr, left->type );
            if( expr->op == PT_ERROR )
                break;
            type = BindTemplateClass( expr->type, &expr->locn, TRUE );
            continue;
          case RESULT_DLT :
            expr = AnalyseDelete( expr, FALSE );
            if( expr->op == PT_ERROR )
                break;
            type = expr->type;
            continue;
          case RESULT_COLON_RVALUE :
          { TYPE type_right;    // type of right node
            TYPE type_left;     // type of left node
            TYPE ref_left;      // referenced type: left
            TYPE ref_right;     // referenced type: right
            type_flag flags_left;   // reference flags on left
            type_flag flags_right;  // reference flags on right
            if( type->id == TYP_VOID )
                continue;
            type_left = TypedefModifierRemoveOnly( NodeType( left ) );
            type_right = TypedefModifierRemoveOnly( NodeType( right ) );
            if( TypesIdentical( type_right, type_left ) ) {
                type = type_left;
            } else {
                bool force_rvalue;
                bool cast_to_left;
                if( type_left->id == TYP_POINTER
                 && type_right->id == TYP_POINTER
                 && ( type_left->flag & TF1_REFERENCE )
                 && ( type_right->flag & TF1_REFERENCE ) ) {
                    ref_right = TypeGetActualFlags( type_right->of, &flags_right );
                    ref_left = TypeGetActualFlags( type_left->of, &flags_left );
                    if( ref_right == ref_left ) {
                        type_flag common;
                        flags_right &= TF1_CV_MASK;
                        flags_left &= TF1_CV_MASK;
                        common = flags_right & flags_left;
                        if( flags_right == common ) {
                            cast_to_left = TRUE;
                            force_rvalue = FALSE;
                        } else if( flags_left == common ) {
                            cast_to_left = FALSE;
                            force_rvalue = FALSE;
                        } else {
                            force_rvalue = TRUE;
                        }
                    } else {
                        switch( TypeCommonDerivation( ref_left, ref_right ) ) {
                          case CTD_NO :
                            force_rvalue = TRUE;
                            break;
                          case CTD_LEFT :
                          case CTD_LEFT_PROTECTED :
                          case CTD_LEFT_PRIVATE :
                          case CTD_LEFT_AMBIGUOUS :
                          case CTD_LEFT_VIRTUAL :
                           cast_to_left = FALSE;
                           force_rvalue = FALSE;
                           break;
                          case CTD_RIGHT :
                          case CTD_RIGHT_PROTECTED :
                          case CTD_RIGHT_PRIVATE :
                          case CTD_RIGHT_AMBIGUOUS :
                          case CTD_RIGHT_VIRTUAL :
                           cast_to_left = TRUE;
                           force_rvalue = FALSE;
                           break;
                        }
                    }
                } else {
                    force_rvalue = TRUE;
                }
                if( force_rvalue ) {
                    left = NodeRvalueExactLeft( expr );
                    if( PT_ERROR == left->op )
                        break;
                    type = TypedefModifierRemoveOnly( left->type );
                    right = NodeRvalueExactRight( expr );
                    if( PT_ERROR == right->op ) {
                        break;
                    }
                } else {
                    PTREE bound;
                    if( cast_to_left ) {
                        bound = CastImplicit( expr->u.subtree[1]
                                            , type_left
                                            , CNV_EXPR
                                            , &diagColonRef );
                        expr->u.subtree[1] = bound;
                        type = type_left;
                    } else {
                        bound = CastImplicit( expr->u.subtree[0]
                                            , type_right
                                            , CNV_EXPR
                                            , &diagColonRef );
                        expr->u.subtree[0] = bound;
                        type = type_right;
                    }
                    if( PT_ERROR == bound->op ) {
                        break;
                    }
                }
            }
            NodeSetType( expr, type, PTF_LV_CHECKED );
          } continue;
          case RESULT_COLON_AA :
          { TYPE type_left;     // - unmodified type on left
            TYPE type_right;    // - unmodified type on right
            type_flag flag_left;// - flags on left
            type_flag flag_right;//- flags on right
            type_flag flags;    // - flags for result
            type = left->type;
            if( expr->flags & PTF_LVALUE )
                continue;
            type_left = TypeModFlagsEC( left->type, &flag_left );
            type_right = TypeModFlagsEC( right->type, &flag_right );
            if( TypesIdentical( type_right, type_left ) ) {
                flags = flag_left & flag_right & TF1_CV_MASK;
                if( flags == 0 ) {
                    type = type_left;
                } else {
                    type = MakeBasedModifierOf( type_left
                                              , flags
                                              , NULL );
                }
            } else {
                type = binary_arith_result( left, right );
            }
          } continue;
          case RESULT_COLON_PP :
          {// unsigned retn;  // - conversion return
            NodeSetType( expr, type, PTF_LV_CHECKED );
            if( expr->flags & PTF_LVALUE )
                continue;
            NodeRvalueRight( expr );
            NodeRvalueLeft( expr );
            expr = CastImplicitCommonPtrExpr( expr
                                            , &diagPtrConvCommon
                                            , TRUE );
            if( PT_ERROR == expr->op ) {
                break;
            }
          } continue;
          case RESULT_COLON_OTHER :
          { TYPE type_right;    // - unmodified type to left
            if( expr->flags & PTF_LVALUE )
                continue;
            type_right = TypedefModifierRemoveOnly( right->type );
            if( type->id == TYP_VOID ) {
                if( type_right->id == TYP_VOID )
                    continue;
                if( left->flags & PTF_THROW_EXPR ) {
                    type = right->type;
                } else {
                    exprError( right, ERR_NOT_VOID );
                    break;
                }
            } else if( type_right->id == TYP_VOID ) {
                if( right->flags & PTF_THROW_EXPR )
                    continue;
                exprError( left, ERR_NOT_VOID );
                break;
            } else if( ConvertCommonType( &expr
                                        , &diagColonClass
                                        , &diagColonMp ) ) {
                if( expr->op == PT_ERROR )
                    break;
                type = expr->type;
                left = expr->u.subtree[0];
                right = expr->u.subtree[1];
                if( NULL != MemberPtrType( type ) ) {
                    left = ConvertMembPtrConst( &expr->u.subtree[0] );
                    right = ConvertMembPtrConst( &expr->u.subtree[1] );
                }
            } else {
                break;
            }
          } continue;
          case RESULT_COLON :
          #define ExtractColonFlags(node) ( node->flags & PTF_FETCH )
          { PTF_FLAG rflag; // - flags on right
            PTF_FLAG lflag; // - flags on left
            lflag = ExtractColonFlags( expr->u.subtree[0] );
            rflag = ExtractColonFlags( expr->u.subtree[1] );
            expr->flags &= ~ ( PTF_FETCH );
            expr->flags |= lflag & rflag;
            expr->flags |= ( lflag | rflag ) & PTF_SIDE_EFF;
          } continue;
          #undef ExtractColonFlags
          case RESULT_COND_OPRS :
            expr->u.subtree[0] = NodeComma( NodeIc( IC_COND_TRUE )
                                          , expr->u.subtree[0] );
            expr->u.subtree[1] = NodeComma( NodeIc( IC_COND_FALSE )
                                          , expr->u.subtree[1] );
            left = expr->u.subtree[0];
            right = expr->u.subtree[1];
            continue;
          case RESULT_COND_OPR :
            expr->u.subtree[1] = NodeComma( NodeIc( IC_COND_TRUE )
                                          , expr->u.subtree[1] );
            continue;
          case RESULT_COND_EXPR :
            expr->flags |= PTF_COND_END;
            continue;
          case RESULT_MEMPTR_QUAL :     // .* and ->*
            if( NULL == MemberPtrType( right->type ) ) {
                operandError( expr, ERR_RIGHT_NOT_MEMBPTR );
                break;
            } else {
                expr = MembPtrDereference( expr );
            }
            continue;
          case RESULT_OTHER_CMP :       // ==, != for other operands
            if( ConvertCommonType( &expr
                                 , &diagClassCmp
                                 , &diagMembPtrCmp ) ) {
                if( expr->op == PT_ERROR )
                    break;
                type = expr->type;
                if( MemberPtrType( type ) ) {
                    expr = MembPtrCompare( expr );
                    if( expr->op != PT_ERROR ) {
                        continue;
                    }
                } else {
                    operandError( expr, ERR_LEFT_NOT_PTR_ARITH_MP );
                }
            }
            break;
          case RESULT_THROW :           // throw an expression
          { PTREE throw_exp;    // - throw expression or NULL
            {   TOKEN_LOCN err_locn; // - error location
                PTreeExtractLocn( expr, &err_locn );
                SetErrLoc( &err_locn );
                ExceptionsCheck();
                SetErrLoc( NULL );
            }
            throw_exp = expr->u.subtree[0];
            PTreeFree( expr );
            if( throw_exp == NULL ) {
                expr = RunTimeCall( NULL
                                  , GetBasicType( TYP_VOID )
                                  , RTF_RETHROW );
            } else {
                unsigned rtcode;// - code for R/T routine
                TYPE cl_type;   // - NULL or class type
                type = throw_exp->type;
                cl_type = ClassTypeForType( type );
                if( ! TypeDefedNonAbstract( type
                                          , throw_exp
                                          , ERR_THROW_ABSTRACT
                                          , ERR_THROW_UNDEFED ) ) {
                    expr = throw_exp;
                    break;
                } else if( NULL != ArrayType( type ) ) {
                    throw_exp = NodeRvalue( throw_exp );
                    type = throw_exp->type;
                } else if( NULL != MemberPtrType( type ) ) {
                    throw_exp = NodeAssignTemporary( type, throw_exp );
                    type = NodeType( throw_exp );
                } else if( NULL != FunctionDeclarationType( type ) ) {
                    type = TypeCanonicalThr( type );
                    throw_exp = NodeConvert( type, throw_exp );
                    throw_exp->flags &= ~ PTF_LVALUE;
                }
                if( BasedPtrType( type ) ) {
                    if( ! NodeDerefPtr( &throw_exp ) ) {
                        expr = throw_exp;
                        break;
                    }
                    type = throw_exp->type;
                }
                type = TypePointedAtModified( type );
                if( type != NULL ) {
                    if( TypeTruncByMemModel( type ) ){
                        expr = throw_exp;
                        PTreeErrorExpr( expr, ERR_USE_FAR );
                        break;
                    } else {
// expand ?
                    }
                }
                type = NodeType( throw_exp );
                switch( ThrowCategory( type ) ) {
//                  PTREE constant; // - constant node
                  case THROBJ_SCALAR :
                  case THROBJ_PTR_FUN :
                  case THROBJ_PTR_CLASS :
                  case THROBJ_PTR_SCALAR :
                  case THROBJ_VOID_STAR :
                    rtcode = RTF_THROW;
                    if( throw_exp->flags & PTF_LVALUE ) {
                        expr = PTreeOp( &throw_exp );
                    } else {
                        INT_CONSTANT int_con;
                        if( NodeIsIntConstant( throw_exp, &int_con )
                            && Zero64( &int_con.u.value ) ) {
                            rtcode = RTF_THROW_ZERO;
                        }
//                      constant = NodeGetConstantNode( throw_exp );
                        throw_exp = NodeAssignTemporary( throw_exp->type, throw_exp );
                        expr = PTreeOpLeft( throw_exp );
                        type = NodeType( expr );
                    }
                    break;
                  case THROBJ_CLASS :
                  case THROBJ_CLASS_VIRT :
                    throw_exp = NodeForceLvalue( throw_exp );
                    expr = throw_exp;
                    type = NodeType( expr );
                    DbgVerify( NULL != TypeReference( type )
                             , "throw expression not co-erced to lvalue" );
                    rtcode = RTF_THROW;
                    break;
                  case THROBJ_REFERENCE :
                    expr = throw_exp;
                    rtcode = RTF_THROW;
                    break;
                  DbgDefault( "ANALYSE -- invalid throw category" );
                }
                if( expr->op == PT_SYMBOL ) {
                    expr->u.symcg.symbol->flag |= SF_ADDR_TAKEN;
                }
                expr = ThrowTypeSig( type, throw_exp );
                if( expr == NULL ) {
                    expr = PTreeErrorNode( throw_exp );
                    break;
                }
                expr = RunTimeCall( NodeArgument( expr, throw_exp )
                                  , GetBasicType( TYP_VOID )
                                  , rtcode );
            }
            expr = FunctionCouldThrow( expr );
            type = expr->type;
            expr->flags |= PTF_MEANINGFUL
                         | PTF_SIDE_EFF
                         | PTF_THROW_EXPR;
          } continue;
          case RESULT_SEGOP :       // int :> ptr OPERATOR
          { PTREE left;             // - ptr operand (note: switched)
            TYPE segid_type;

            segid_type = TypeSegId();
            warnIntTrunc( right, segid_type, getConstBitsType( segid_type ) );
            left = expr->u.subtree[0];
            if( TypeIsBasedPtr( type ) ) {
                left = NodeConvert( TypeSegAddr(), left );
                expr->u.subtree[0] = left;
            }
            type = TypeSegOp( left->type );
          } continue;
          case RESULT_SEGNAME :     // __segname( "..." )
            expr = NodeReplace( expr
                              , NodeSegname( left->u.string->string ) );
            type = expr->type;
            continue;
          case DREF_PTR_LEFT :      // DE-REFERENCE PTR ON LEFT
          { if( ! NodeDerefPtr( &expr->u.subtree[0] ) )
                break;
            left = PTreeOpLeft( expr );
            type = left->type;
          } continue;
          case DREF_PTR_RIGHT :     // DE-REFERENCE PTR ON RIGHT
          { if( ! NodeDerefPtr( &expr->u.subtree[1] ) )
                break;
            right = PTreeOpRight( expr );
          } continue;
          case WARN_ADJ_COMPARES :
            if( !isRelationalOperator( expr ) )
                continue;
            if( !isRelationalOperator( left ) )
                continue;
            PTreeWarnExpr( expr, WARN_ADJACENT_RELN_OPS );
            continue;
          case WARN_OPEQ_INT_TRUNC :
            if( (PTreeOpFlags( expr ) & PTO_ASSIGN_SAME) == 0 )
                continue;
            /* fall through */
          case WARN_INT_TRUNC :
          { unsigned tgt_bits;        // bits in target
            TYPE  tgt_type;           // target type
            if( ! IntegralType( type ) )
                continue;
            if( ! IntegralType( right->type ) )
                continue;
            if( NodeIsUnaryOp( left, CO_BITFLD_CONVERT ) ) {
                tgt_type = PTreeOpLeft( left )->type;
                tgt_type = TypedefModifierRemoveOnly( tgt_type );
                tgt_bits = tgt_type->u.b.field_width;
            } else {
                tgt_type = type;
                tgt_bits = getConstBitsType( tgt_type );
            }
            warnIntTrunc( right, tgt_type, tgt_bits );
          } continue;
          case WARN_BOOL_ASSIGN_LEFT :
            warnBoolAssignment( left );
            continue;
          case WARN_BOOL_ASSIGN_RIGHT :
            warnBoolAssignment( right );
            continue;
          case WARN_CONST_COND_LEFT :
            warnBoolConst( left, expr );
            continue;
          case WARN_CONST_COND_RIGHT :
            warnBoolConst( right, expr );
            continue;
          case WARN_POINTER_ZERO :
            warnPointerZero( expr, left, right );
            continue;
          case WARN_USELESS_CMP :
            warnUselessCompare( expr, left, right );
            continue;
          case WARN_MINUS_UNSIGNED:
            if( NodeIsUnaryOp( expr, CO_UMINUS ) ) {
                if( left->op == PT_INT_CONSTANT && UnsignedIntType( type ) ) {
                    PTreeWarnExpr( expr, WARN_NEGATE_UNSIGNED );
                }
            }
            continue;
          case RELOAD_EXPR_BINARY :             // drops thru
            right = PTreeOpRight( expr );
            right->type = BindTemplateClass( right->type, &right->locn, FALSE );
          case RELOAD_EXPR_UNARY :
            left = PTreeOpLeft( expr );
            left->type = BindTemplateClass( left->type, &left->locn, FALSE );
          case RELOAD_EXPR_TYPE :               // drops thru
            if( expr->op == PT_ERROR )
                break;
            type = expr->type;
            type = BindTemplateClass( type, &expr->locn, FALSE );
            continue;
          case CONV_BASIC_TYPE :
            type = TypedefModifierRemove( type );
            continue;
#ifndef NDEBUG
          default :
            CFatal( "ANALYSE -- undefined action" );
#endif
        } // switches for actions
//
// - if we "break" out of switch, an error has been diagnosed
// - each case "continue's when there is no error
//
        if( temp_class != TEMP_TYPE_NONE ) {
            TemporaryClass( temp_class );
        }
        PTreeErrorNode( expr );
        type = NULL;
        break;
    } // for-loop
    expr->type = type;              // don't use NodeSetType (ref.assignment)
    switch( expr->op ) {
      case PT_UNARY :
        if( expr->u.subtree[0] != NULL ) {
            if( expr->cgop == CO_ADDR_OF ) {
                expr->flags &= ~PTF_LV_CHECKED;
            } else {
                expr = FoldUnary( expr );
                expr->flags |= PTF_LV_CHECKED;
            }
        }
        break;
      case PT_BINARY :
        if( expr->u.subtree[0] != NULL
         && expr->u.subtree[1] != NULL ) {
            expr = FoldBinary( expr );
        }
        // drops thru
      default :
        expr->flags |= PTF_LV_CHECKED;
        break;
    }
    if( templ != NULL ) {
        PTreeFreeSubtrees( templ );
    }
    return expr;
}


PTREE AnalyseNode(              // ANALYSE PTREE NODE FOR SEMANTICS
    PTREE expr )                // - current node
{
    if( expr->flags & PTF_ALREADY_ANALYSED ) {
        return expr;
    }
    CtxTokenLocn( &expr->locn );
    switch( expr->op ) {
      case PT_ID :
        expr->flags |= PTF_LVALUE;
        break;
      case PT_BINARY :
        if( ( expr->u.subtree[0] != NULL
           && expr->u.subtree[0]->op == PT_ERROR )
         || ( expr->u.subtree[1] != NULL
           && expr->u.subtree[1]->op == PT_ERROR ) ) {
            PTreePropogateError( expr );
        } else {
            switch( expr->cgop ) {
              case CO_COLON_COLON :
                ExtraRptTabIncr( ctrOps, CO_COLON_COLON, 0 );
                break;
              case CO_TEMPLATE :
                break;
              case CO_OFFSETOF :
                ExtraRptTabIncr( ctrOps, CO_OFFSETOF, 0 );
                expr = AnalyseOffsetOf( expr );
                break;
              case CO_DOT :
                ExtraRptTabIncr( ctrOps, CO_DOT, 0 );
                expr = AnalyseLvDot( expr );
                break;
              case CO_ARROW :
                ExtraRptTabIncr( ctrOps, CO_ARROW, 0 );
                expr = AnalyseLvArrow( expr );
                break;
              default :
                expr = AnalyseOperator( expr );
                break;
            }
        }
        break;
      case PT_UNARY :
        if( expr->u.subtree[0] == NULL
         || expr->u.subtree[0]->op != PT_ERROR ) {
            expr = AnalyseOperator( expr );
        } else {
            PTreePropogateError( expr );
        }
        break;
    }
    expr->flags |= PTF_ALREADY_ANALYSED;
    return expr;
}

static PTREE clearAnalysedFlag(
    PTREE expr )
{
    expr->flags &= ~PTF_ALREADY_ANALYSED;
    return expr;
}

static PTREE run_traversals(    // ANALYZE EXPRESSION VIA TRAVERSALS
    PTREE expr )
{
#ifndef NDEBUG
    expr = DbgCommaInsertion( expr );
    if( PragDbgToggle.dump_ptree ) {
        DumpCommentary( "Parse tree to be analyzed semantically" );
        DumpPTree( expr );
    }
    if( PragDbgToggle.print_ptree ) {
        DumpCommentary( "Parse tree to be analyzed semantically" );
        DbgPrintPTREE( expr );
    }
#endif
    if( expr == NULL ) {
        return PTreeErrorNode( NULL );
    }
    expr = PTreeTraversePostfix( expr, &AnalyseNode );
    if( expr->op != PT_ERROR) {
        expr = PTreeTraversePostfix( expr, &clearAnalysedFlag );
    }
    if( expr->op != PT_ERROR && (expr->flags & PTF_LV_CHECKED) == 0 ) {
//      AnalyseLvalue( PTreeRef( &expr ) );
        AnalyseLvalue( &expr );
    }
    return expr;
}


PTREE AnalyseStmtExpr(      // ANALYZE A STATEMENT EXPRESSION
    PTREE expr )
{
    if( expr != NULL ) {
        expr = run_traversals( expr );
        warnMeaningfulSideEffect( expr );
        expr = NodeDone( expr );
    }
    return expr;
}



PTREE AnalyseInitExpr(      // ANALYZE AN INITIALIZATION EXPRESSION
    PTREE expr,             // - expression
    bool is_static )        // - is target a static
{
    if( expr != NULL ) {
        if( NodeIsBinaryOp( expr, CO_INIT ) ) {
            expr->flags |= is_static ? PTF_STATIC_INIT : PTF_BLOCK_INIT;
        }
        expr = AnalyseStmtExpr( expr );
    }
    return expr;
}


PTREE AnalyseRawExpr(       // ANALYZE AN EXPRESSION
    PTREE expr )
{
    if( expr != NULL ) {
        expr = run_traversals( expr );
    }
    return expr;
}


PTREE AnalyseBoolExpr(      // ANALYZE A BOOLEAN EXPRESSION
    PTREE expr )
{
    if( expr != NULL ) {
        expr = run_traversals( expr );
        if( expr->op != PT_ERROR ) {
            if( 0 == ( expr->flags & PTF_BOOLEAN ) ) {
                warnBoolAssignment( expr );
            }
            expr = NodeConvertToBool( expr );
        }
    }
    return expr;
}


PTREE AnalyseValueExpr(     // ANALYZE AN EXPRESSION, MAKE IT A VALUE
    PTREE expr )
{
    if( expr != NULL ) {
        expr = run_traversals( expr );
        if( expr->op != PT_ERROR ) {
            expr = NodeRvalue( expr );
        }
    }
    return expr;
}


PTREE AnalyseIntegralExpr(  // ANALYZE AN EXPRESSION, MAKE IT AN INT. VALUE
    PTREE expr )
{
    PTREE old;              // - node to be freed

    if( expr != NULL ) {
        expr = AnalyseValueExpr( PTreeUnary( CO_CONVERT_INT, expr ) );
        if( expr->op != PT_ERROR
         && NodeIsUnaryOp( expr, CO_CONVERT_INT ) ) {
            old = expr;
            expr = old->u.subtree[0];
            PTreeFree( old );
        }
    }
    return expr;
}

PTREE AnalyseReturnExpr(    // ANALYSE A RETURN EXPRESSION
    SYMBOL func,            // - function to which the expression applies
    PTREE expr )            // - expression
{
    TYPE fun_type;          // - type of return
    PTREE right;            // - right operand of return expression

    if( expr != NULL ) {
        fun_type = SymFuncReturnType( func );
        right = expr;
        expr = NodeBinary( CO_RETURN, PTreeType( fun_type ), right );
        expr = PTreeCopySrcLocation( expr, right );
        expr = run_traversals( expr );
        expr = NodeDone( expr );
        if( FnRetnOpt() ) {
            expr->flags |= PTF_RETN_OPT;
        }
    }
    return expr;
}
