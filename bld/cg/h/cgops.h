/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  enumeration and data for cg operators
*
****************************************************************************/


/*     enum_ext          enum_int           dmp_sym  dmp_mac dmp_op npar attr */
/*  Start of Code Generator Operators ... */
PICK(  O_NOP,              OP_NOP,               ,        STR2,  "nop  ", 0,  0                     )

/*  Binary Ops  -- call CGBinary() */

PICK(  O_PLUS,             OP_ADD,               "+",     STR1,  "add  ", 2,  PUSHABLE             )
PICK(  O_INTERNAL_01,      OP_EXT_ADD,           ,        STR2,  "adc  ", 2,  MOVEABLE             )
PICK(  O_MINUS,            OP_SUB,               "-",     STR1,  "sub  ", 2,  PUSHABLE             )
PICK(  O_INTERNAL_02,      OP_EXT_SUB,           ,        STR2,  "sbb  ", 2,  MOVEABLE             )
PICK(  O_TIMES,            OP_MUL,               "*",     STR1,  "mul  ", 2,  MOVEABLE | SWAPABLE  )
PICK(  O_INTERNAL_03,      OP_EXT_MUL,           ,        STR2,  "emul ", 2,  MOVEABLE | SWAPABLE  )
PICK(  O_DIV,              OP_DIV,               "/",     STR1,  "div  ", 2,  MOVEABLE | SWAPABLE  )
PICK(  O_MOD,              OP_MOD,               ,        STR2,  "mod  ", 2,  MOVEABLE | SWAPABLE  )
PICK(  O_AND,              OP_AND,               "&",     STR1,  "and  ", 2,  MOVEABLE | SWAPABLE  )
PICK(  O_OR,               OP_OR,                "|",     STR1,  "or   ", 2,  MOVEABLE | SWAPABLE  )
PICK(  O_XOR,              OP_XOR,               "^",     STR1,  "xor  ", 2,  MOVEABLE | SWAPABLE  )
PICK(  O_RSHIFT,           OP_RSHIFT,            ">>",    STR1,  "shr  ", 2,  MOVEABLE | SWAPABLE  )
PICK(  O_LSHIFT,           OP_LSHIFT,            "<<",    STR1,  "shl  ", 2,  MOVEABLE | SWAPABLE  )
PICK(  O_POW,              OP_POW,               ,        STR2,  "pow  ", 2,  MOVEABLE | SWAPABLE  )
PICK(  O_INTERNAL_04,      OP_P5DIV,             ,        STR2,  "P5div", 2,  MOVEABLE | SWAPABLE  )
PICK(  O_ATAN2,            OP_ATAN2,             ,        STR2,  "atan2", 2,  MOVEABLE | SWAPABLE  )
PICK(  O_FMOD,             OP_FMOD,              ,        STR2,  "fmod ", 2,  MOVEABLE | SWAPABLE  )

/*  Unary Ops -- call CGUnary() */

PICK(  O_UMINUS,           OP_NEGATE,            ,        STR2,  "neg  ", 1,  MOVEABLE | SWAPABLE  )
PICK(  O_COMPLEMENT,       OP_COMPLEMENT,        ,        STR2,  "not  ", 1,  MOVEABLE | SWAPABLE  )
PICK(  O_LOG,              OP_LOG,               ,        STR2,  "log  ", 1,  MOVEABLE | SWAPABLE  )
PICK(  O_COS,              OP_COS,               ,        STR2,  "cos  ", 1,  MOVEABLE | SWAPABLE  )
PICK(  O_SIN,              OP_SIN,               ,        STR2,  "sin  ", 1,  MOVEABLE | SWAPABLE  )
PICK(  O_TAN,              OP_TAN,               ,        STR2,  "tan  ", 1,  MOVEABLE | SWAPABLE  )
PICK(  O_SQRT,             OP_SQRT,              ,        STR2,  "sqrt ", 1,  MOVEABLE | SWAPABLE  )
PICK(  O_FABS,             OP_FABS,              ,        STR2,  "fabs ", 1,  MOVEABLE | SWAPABLE  )
PICK(  O_ACOS,             OP_ACOS,              ,        STR2,  "acos ", 1,  MOVEABLE | SWAPABLE  )
PICK(  O_ASIN,             OP_ASIN,              ,        STR2,  "asin ", 1,  MOVEABLE | SWAPABLE  )
PICK(  O_ATAN,             OP_ATAN,              ,        STR2,  "atan ", 1,  MOVEABLE | SWAPABLE  )
PICK(  O_COSH,             OP_COSH,              ,        STR2,  "cosh ", 1,  MOVEABLE | SWAPABLE  )
PICK(  O_EXP,              OP_EXP,               ,        STR2,  "exp  ", 1,  MOVEABLE | SWAPABLE  )
PICK(  O_LOG10,            OP_LOG10,             ,        STR2,  "log10", 1,  MOVEABLE | SWAPABLE  )
PICK(  O_SINH,             OP_SINH,              ,        STR2,  "sinh ", 1,  MOVEABLE | SWAPABLE  )
PICK(  O_TANH,             OP_TANH,              ,        STR2,  "tanh ", 1,  MOVEABLE | SWAPABLE  )

PICK(  O_PTR_TO_NATIVE,    OP_PTR_TO_NATIVE,     ,        STR2,  "flatp", 1,  MOVEABLE | SWAPABLE  )   /* oh! ouch! the 386 really hurts my head! */
PICK(  O_PTR_TO_FOREIGN,   OP_PTR_TO_FOREIGN,    ,        STR2,  "segp ", 1,  MOVEABLE | SWAPABLE  )
PICK(  O_PARENTHESIS,      OP_SLACK_19,          ,        STR2,  "OOPS!", 0,  0                     )   /* optimization control - NOP for now */

PICK(  O_CONVERT,          OP_CONVERT,           "<c=",   STR1,  "cnv  ", 1,  MOVEABLE | SWAPABLE  )
PICK(  O_INTERNAL_05,      OP_LA,                ,        STR2,  "lea  ", 1,  MOVEABLE | SWAPABLE  )
PICK(  O_INTERNAL_06,      OP_CAREFUL_LA,        ,        STR2,  "lea!!", 1,  MOVEABLE | SWAPABLE  )
PICK(  O_ROUND,            OP_ROUND,             ,        STR2,  "rnd  ", 1,  MOVEABLE | SWAPABLE  )
PICK(  O_GETS,             OP_MOV,               ,        STR2,  "mov  ", 1,  MOVEABLE | SWAPABLE  )   /* CGAssign(), CGLVAssign() */

/*  calling operators */

PICK(  O_INTERNAL_07,      OP_CALL_INDIRECT,     ,        STR2,  "calli", 3,  SIDE_EFFECT          )
PICK(  O_INTERNAL_08,      OP_PUSH,              ,        STR2,  "push ", 1,  SWAPABLE             )
PICK(  O_INTERNAL_09,      OP_POP,               ,        STR2,  "pop  ", 0,  SWAPABLE             )
PICK(  O_INTERNAL_10,      OP_PARM_DEF,          ,        STR2,  "parm ", 0,  0                     )   /* CGAddParm() */
PICK(  O_INTERNAL_11,      OP_SELECT,            ,        STR2,  "sel  ", 1,  0                     )   /* generalized jump */

/*  Comparison operators -- call CGCompare() */

PICK(  O_INTERNAL_12,      OP_BIT_TEST_TRUE,     ,        STR2,  "tst<>", 2,  SIDE_EFFECT          )
PICK(  O_INTERNAL_13,      OP_BIT_TEST_FALSE,    ,        STR2,  "tst==", 2,  SIDE_EFFECT          )
PICK(  O_EQ,               OP_CMP_EQUAL,         "==",    STR1,  "if ==", 2,  SIDE_EFFECT          )
PICK(  O_NE,               OP_CMP_NOT_EQUAL,     "!=",    STR1,  "if !=", 2,  SIDE_EFFECT          )
PICK(  O_GT,               OP_CMP_GREATER,       ">",     STR1,  "if > ", 2,  SIDE_EFFECT          )
PICK(  O_LE,               OP_CMP_LESS_EQUAL,    "<=",    STR1,  "if <=", 2,  SIDE_EFFECT          )
PICK(  O_LT,               OP_CMP_LESS,          "<",     STR1,  "if<  ", 2,  SIDE_EFFECT          )
PICK(  O_GE,               OP_CMP_GREATER_EQUAL, ">=",    STR1,  "if>= ", 2,  SIDE_EFFECT          )

/*  Call -- call CGCall() */

PICK(  O_INTERNAL_14,      OP_CALL,              ,        STR2,  "call ", 2,  SIDE_EFFECT          )

PICK(  O_INTERNAL_15,      OP_SET_EQUAL,         ,        STR2,  "set==", 2,  SIDE_EFFECT          )
PICK(  O_INTERNAL_16,      OP_SET_NOT_EQUAL,     ,        STR2,  "set!=", 2,  SIDE_EFFECT          )
PICK(  O_INTERNAL_17,      OP_SET_GREATER,       ,        STR2,  "set> ", 2,  SIDE_EFFECT          )
PICK(  O_INTERNAL_18,      OP_SET_LESS_EQUAL,    ,        STR2,  "set<=", 2,  SIDE_EFFECT          )
PICK(  O_INTERNAL_19,      OP_SET_LESS,          ,        STR2,  "set< ", 2,  SIDE_EFFECT          )
PICK(  O_INTERNAL_20,      OP_SET_GREATER_EQUAL, ,        STR2,  "set>=", 2,  SIDE_EFFECT          )
PICK(  O_INTERNAL_21,      OP_DEBUG_INFO,        ,        STR2,  "dinfo", 0,  0                     )
PICK(  O_INTERNAL_22,      OP_CHEAP_NOP,         ,        STR2,  "cnop ", 0,  0                     )
PICK(  O_INTERNAL_23,      OP_LOAD_UNALIGNED,    ,        STR2,  "ldq_u", 1,  0                     )
PICK(  O_INTERNAL_24,      OP_STORE_UNALIGNED,   ,        STR2,  "stq_u", 1,  0                     )
PICK(  O_INTERNAL_25,      OP_EXTRACT_LOW,       ,        STR2,  "extl ", 2,  0                     )
PICK(  O_INTERNAL_26,      OP_EXTRACT_HIGH,      ,        STR2,  "exth ", 2,  0                     )
PICK(  O_INTERNAL_27,      OP_INSERT_LOW,        ,        STR2,  "insl ", 2,  0                     )
PICK(  O_INTERNAL_28,      OP_INSERT_HIGH,       ,        STR2,  "insh ", 2,  0                     )
PICK(  O_INTERNAL_29,      OP_MASK_LOW,          ,        STR2,  "mskl ", 2,  0                     )
PICK(  O_INTERNAL_30,      OP_MASK_HIGH,         ,        STR2,  "mskh ", 2,  0                     )
PICK(  O_INTERNAL_31,      OP_ZAP,               ,        STR2,  "zap  ", 2,  0                     )
PICK(  O_INTERNAL_32,      OP_ZAP_NOT,           ,        STR2,  "zap! ", 2,  0                     )
PICK(  O_STACK_ALLOC,      OP_STACK_ALLOC,       ,        STR2,  "alloc", 0,  0                     )
PICK(  O_VA_START,         OP_VA_START,          ,        STR2,  "vastr", 1,  0                     )

/*  Slack */

PICK(  O_SLACK_31,         OP_SLACK_31,          ,        STR2,  "OOPS!", 0,  0                     )
PICK(  O_SLACK_32,         OP_SLACK_32,          ,        STR2,  "OOPS!", 0,  0                     )
PICK(  O_SLACK_33,         OP_SLACK_33,          ,        STR2,  "OOPS!", 0,  0                     )
PICK(  O_SLACK_34,         OP_SLACK_34,          ,        STR2,  "OOPS!", 0,  0                     )
PICK(  O_SLACK_35,         OP_SLACK_35,          ,        STR2,  "OOPS!", 0,  0                     )
PICK(  O_SLACK_36,         OP_SLACK_36,          ,        STR2,  "OOPS!", 0,  0                     )
PICK(  O_SLACK_37,         OP_SLACK_37,          ,        STR2,  "OOPS!", 0,  0                     )
PICK(  O_SLACK_38,         OP_SLACK_38,          ,        STR2,  "OOPS!", 0,  0                     )
PICK(  O_SLACK_39,         OP_SLACK_39,          ,        STR2,  "OOPS!", 0,  0                     )

/*  Internal */

PICK(  O_INTERNAL_33,      OP_BLOCK,             ,        STR2,  "BLOCK", 0,  0                     )

/*  End of Internal Code Generator operators (opcode_defs) */

#ifndef ONLY_INTERNAL_CGOPS

/*  Flow operators -- call CGFlow() */

PICK(  O_FLOW_AND,            ,                  "&&",    STR1,         , 0,  0                     )
PICK(  O_FLOW_OR,             ,                  "||",    STR1,         , 0,  0                     )
PICK(  O_FLOW_OUT,            ,                  ,        STR2,         , 0,  0                     )
PICK(  O_FLOW_NOT,            ,                  "!",     STR1,         , 0,  0                     )

PICK(  O_POINTS,              ,                  "@",     STR1,         , 0,  0                     )   /* CGUnary() */

/* % Control operators -- call CGControl(),CGBigGoto(),CGBigLabel() */

PICK(  O_GOTO,                ,                  ,        STR2,         , 0,  0                     )
PICK(  O_BIG_GOTO,            ,                  ,        STR2,         , 0,  0                     )
PICK(  O_IF_TRUE,             ,                  ,        STR2,         , 0,  0                     )
PICK(  O_IF_FALSE,            ,                  ,        STR2,         , 0,  0                     )
PICK(  O_INVOKE_LABEL,        ,                  ,        STR2,         , 0,  0                     )
PICK(  O_LABEL,               ,                  ,        STR2,         , 0,  0                     )
PICK(  O_BIG_LABEL,           ,                  ,        STR2,         , 0,  0                     )
PICK(  O_LABEL_RETURN,        ,                  ,        STR2,         , 0,  0                     )

PICK(  O_PROC,                ,                  ,        STR2,         , 0,  0                     )   /* CGProcDecl */
PICK(  O_PARM_DEF,            ,                  ,        STR2,         , 0,  0                     )   /* CGParmDecl */
PICK(  O_AUTO_DEF,            ,                  ,        STR2,         , 0,  0                     )   /* CGAutoDecl */
PICK(  O_COMMA,               ,                  ,        STR2,         , 0,  0                     )

PICK(  O_PASS_PROC_PARM,      ,                  ,        STR2,         , 0,  0                     )   /* CGPassPP */
PICK(  O_DEFN_PROC_PARM,      ,                  ,        STR2,         , 0,  0                     )   /* CGDeclPP */
PICK(  O_CALL_PROC_PARM,      ,                  ,        STR2,         , 0,  0                     )   /* CGCallPP */

PICK(  O_PRE_GETS,            ,                  ,        STR2,         , 0,  0                     )   /* CGPreGets, CGLVPreGets */
PICK(  O_POST_GETS,           ,                  ,        STR2,         , 0,  0                     )   /* CGPostGets */

PICK(  O_SIDE_EFFECT,         ,                  ,        STR2,         , 0,  0                     )

#endif

