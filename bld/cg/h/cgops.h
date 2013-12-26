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
* Description:  enumeration and data for cg operators
*
****************************************************************************/

                                                                                                                                                                                                                                        
/*     enum_ext          enum_int           dmp_sym  dmp_mac dmp_op npar attr */
/*  Start of Code Generator Operators ... */
PICK(  NOP,              NOP,               ,        STR2,  "nop  ", 0,  0                     )

/*  Binary Ops  -- call CGBinary() */

PICK(  PLUS,             ADD,               "+",     STR1,  "add  ", 2,  PUSHABLE             )
PICK(  INTERNAL_01,      EXT_ADD,           ,        STR2,  "adc  ", 2,  MOVEABLE             )
PICK(  MINUS,            SUB,               "-",     STR1,  "sub  ", 2,  PUSHABLE             )
PICK(  INTERNAL_02,      EXT_SUB,           ,        STR2,  "sbb  ", 2,  MOVEABLE             )
PICK(  TIMES,            MUL,               "*",     STR1,  "mul  ", 2,  MOVEABLE | SWAPABLE  )
PICK(  INTERNAL_03,      EXT_MUL,           ,        STR2,  "emul ", 2,  MOVEABLE | SWAPABLE  )
PICK(  DIV,              DIV,               "/",     STR1,  "div  ", 2,  MOVEABLE | SWAPABLE  )
PICK(  MOD,              MOD,               ,        STR2,  "mod  ", 2,  MOVEABLE | SWAPABLE  )
PICK(  AND,              AND,               "&",     STR1,  "and  ", 2,  MOVEABLE | SWAPABLE  )
PICK(  OR,               OR,                "|",     STR1,  "or   ", 2,  MOVEABLE | SWAPABLE  )
PICK(  XOR,              XOR,               "^",     STR1,  "xor  ", 2,  MOVEABLE | SWAPABLE  )
PICK(  RSHIFT,           RSHIFT,            ">>",    STR1,  "shr  ", 2,  MOVEABLE | SWAPABLE  )
PICK(  LSHIFT,           LSHIFT,            "<<",    STR1,  "shl  ", 2,  MOVEABLE | SWAPABLE  )
PICK(  POW,              POW,               ,        STR2,  "pow  ", 2,  MOVEABLE | SWAPABLE  )
PICK(  INTERNAL_04,      P5DIV,             ,        STR2,  "P5div", 2,  MOVEABLE | SWAPABLE  )
PICK(  ATAN2,            ATAN2,             ,        STR2,  "atan2", 2,  MOVEABLE | SWAPABLE  )
PICK(  FMOD,             FMOD,              ,        STR2,  "fmod ", 2,  MOVEABLE | SWAPABLE  )
                                                                     
/*  Unary Ops -- call CGUnary() */                                   
                                                                     
PICK(  UMINUS,           NEGATE,            ,        STR2,  "neg  ", 1,  MOVEABLE | SWAPABLE  )
PICK(  COMPLEMENT,       COMPLEMENT,        ,        STR2,  "not  ", 1,  MOVEABLE | SWAPABLE  )
PICK(  LOG,              LOG,               ,        STR2,  "log  ", 1,  MOVEABLE | SWAPABLE  )
PICK(  COS,              COS,               ,        STR2,  "cos  ", 1,  MOVEABLE | SWAPABLE  )
PICK(  SIN,              SIN,               ,        STR2,  "sin  ", 1,  MOVEABLE | SWAPABLE  )
PICK(  TAN,              TAN,               ,        STR2,  "tan  ", 1,  MOVEABLE | SWAPABLE  )
PICK(  SQRT,             SQRT,              ,        STR2,  "sqrt ", 1,  MOVEABLE | SWAPABLE  )
PICK(  FABS,             FABS,              ,        STR2,  "fabs ", 1,  MOVEABLE | SWAPABLE  )
PICK(  ACOS,             ACOS,              ,        STR2,  "acos ", 1,  MOVEABLE | SWAPABLE  )
PICK(  ASIN,             ASIN,              ,        STR2,  "asin ", 1,  MOVEABLE | SWAPABLE  )
PICK(  ATAN,             ATAN,              ,        STR2,  "atan ", 1,  MOVEABLE | SWAPABLE  )
PICK(  COSH,             COSH,              ,        STR2,  "cosh ", 1,  MOVEABLE | SWAPABLE  )
PICK(  EXP,              EXP,               ,        STR2,  "exp  ", 1,  MOVEABLE | SWAPABLE  )
PICK(  LOG10,            LOG10,             ,        STR2,  "log10", 1,  MOVEABLE | SWAPABLE  )
PICK(  SINH,             SINH,              ,        STR2,  "sinh ", 1,  MOVEABLE | SWAPABLE  )
PICK(  TANH,             TANH,              ,        STR2,  "tanh ", 1,  MOVEABLE | SWAPABLE  )
                                                                     
PICK(  PTR_TO_NATIVE,    PTR_TO_NATIVE,     ,        STR2,  "flatp", 1,  MOVEABLE | SWAPABLE  )   /* oh! ouch! the 386 really hurts my head! */
PICK(  PTR_TO_FOREIGN,   PTR_TO_FOREIGN,    ,        STR2,  "segp ", 1,  MOVEABLE | SWAPABLE  )
PICK(  PARENTHESIS,      SLACK_19,          ,        STR2,  "OOPS!", 0,  0                     )   /* optimization control - NOP for now */
                                                                     
PICK(  CONVERT,          CONVERT,           "<c=",   STR1,  "cnv  ", 1,  MOVEABLE | SWAPABLE  )
PICK(  INTERNAL_05,      LA,                ,        STR2,  "lea  ", 1,  MOVEABLE | SWAPABLE  )
PICK(  INTERNAL_06,      CAREFUL_LA,        ,        STR2,  "lea!!", 1,  MOVEABLE | SWAPABLE  )
PICK(  ROUND,            ROUND,             ,        STR2,  "rnd  ", 1,  MOVEABLE | SWAPABLE  )
PICK(  GETS,             MOV,               ,        STR2,  "mov  ", 1,  MOVEABLE | SWAPABLE  )   /* CGAssign(), CGLVAssign() */
                                                                     
/*  calling operators */                                             
                                                                     
PICK(  INTERNAL_07,      CALL_INDIRECT,     ,        STR2,  "calli", 3,  SIDE_EFFECT          )
PICK(  INTERNAL_08,      PUSH,              ,        STR2,  "push ", 1,  SWAPABLE             )
PICK(  INTERNAL_09,      POP,               ,        STR2,  "pop  ", 0,  SWAPABLE             )
PICK(  INTERNAL_10,      PARM_DEF,          ,        STR2,  "parm ", 0,  0                     )   /* CGAddParm() */
PICK(  INTERNAL_11,      SELECT,            ,        STR2,  "sel  ", 1,  0                     )   /* generalized jump */
                                                                     
/*  Comparison operators -- call CGCompare() */                      
                                                                     
PICK(  INTERNAL_12,      BIT_TEST_TRUE,     ,        STR2,  "tst<>", 2,  SIDE_EFFECT          )
PICK(  INTERNAL_13,      BIT_TEST_FALSE,    ,        STR2,  "tst==", 2,  SIDE_EFFECT          )
PICK(  EQ,               CMP_EQUAL,         "==",    STR1,  "if ==", 2,  SIDE_EFFECT          )
PICK(  NE,               CMP_NOT_EQUAL,     "!=",    STR1,  "if !=", 2,  SIDE_EFFECT          )
PICK(  GT,               CMP_GREATER,       ">",     STR1,  "if > ", 2,  SIDE_EFFECT          )
PICK(  LE,               CMP_LESS_EQUAL,    "<=",    STR1,  "if <=", 2,  SIDE_EFFECT          )
PICK(  LT,               CMP_LESS,          "<",     STR1,  "if<  ", 2,  SIDE_EFFECT          )
PICK(  GE,               CMP_GREATER_EQUAL, ">=",    STR1,  "if>= ", 2,  SIDE_EFFECT          )
                                                                     
/*  Call -- call CGCall() */                                         
                                                                     
PICK(  INTERNAL_14,      CALL,              ,        STR2,  "call ", 2,  SIDE_EFFECT          )
                                                                     
PICK(  INTERNAL_15,      SET_EQUAL,         ,        STR2,  "set==", 2,  SIDE_EFFECT          )
PICK(  INTERNAL_16,      SET_NOT_EQUAL,     ,        STR2,  "set!=", 2,  SIDE_EFFECT          )
PICK(  INTERNAL_17,      SET_GREATER,       ,        STR2,  "set> ", 2,  SIDE_EFFECT          )
PICK(  INTERNAL_18,      SET_LESS_EQUAL,    ,        STR2,  "set<=", 2,  SIDE_EFFECT          )
PICK(  INTERNAL_19,      SET_LESS,          ,        STR2,  "set< ", 2,  SIDE_EFFECT          )
PICK(  INTERNAL_20,      SET_GREATER_EQUAL, ,        STR2,  "set>=", 2,  SIDE_EFFECT          )
PICK(  INTERNAL_21,      DEBUG_INFO,        ,        STR2,  "dinfo", 0,  0                     )
PICK(  INTERNAL_22,      CHEAP_NOP,         ,        STR2,  "cnop ", 0,  0                     )
PICK(  INTERNAL_23,      LOAD_UNALIGNED,    ,        STR2,  "ldq_u", 1,  0                     )
PICK(  INTERNAL_24,      STORE_UNALIGNED,   ,        STR2,  "stq_u", 1,  0                     )
PICK(  INTERNAL_25,      EXTRACT_LOW,       ,        STR2,  "extl ", 2,  0                     )
PICK(  INTERNAL_26,      EXTRACT_HIGH,      ,        STR2,  "exth ", 2,  0                     )
PICK(  INTERNAL_27,      INSERT_LOW,        ,        STR2,  "insl ", 2,  0                     )
PICK(  INTERNAL_28,      INSERT_HIGH,       ,        STR2,  "insh ", 2,  0                     )
PICK(  INTERNAL_29,      MASK_LOW,          ,        STR2,  "mskl ", 2,  0                     )
PICK(  INTERNAL_30,      MASK_HIGH,         ,        STR2,  "mskh ", 2,  0                     )
PICK(  INTERNAL_31,      ZAP,               ,        STR2,  "zap  ", 2,  0                     )
PICK(  INTERNAL_32,      ZAP_NOT,           ,        STR2,  "zap! ", 2,  0                     )
PICK(  STACK_ALLOC,      STACK_ALLOC,       ,        STR2,  "alloc", 0,  0                     )
PICK(  VA_START,         VA_START,          ,        STR2,  "vastr", 1,  0                     )
                                                                     
/*  Slack */                                                         
                                                                     
PICK(  SLACK_31,         SLACK_31,          ,        STR2,  "OOPS!", 0,  0                     )
PICK(  SLACK_32,         SLACK_32,          ,        STR2,  "OOPS!", 0,  0                     )
PICK(  SLACK_33,         SLACK_33,          ,        STR2,  "OOPS!", 0,  0                     )
PICK(  SLACK_34,         SLACK_34,          ,        STR2,  "OOPS!", 0,  0                     )
PICK(  SLACK_35,         SLACK_35,          ,        STR2,  "OOPS!", 0,  0                     )
PICK(  SLACK_36,         SLACK_36,          ,        STR2,  "OOPS!", 0,  0                     )
PICK(  SLACK_37,         SLACK_37,          ,        STR2,  "OOPS!", 0,  0                     )
PICK(  SLACK_38,         SLACK_38,          ,        STR2,  "OOPS!", 0,  0                     )
PICK(  SLACK_39,         SLACK_39,          ,        STR2,  "OOPS!", 0,  0                     )

/*  Internal */

PICK(  INTERNAL_33,      BLOCK,             ,        STR2,  "BLOCK", 0,  0                     )

/*  End of Internal Code Generator operators (opcode_defs) */

#ifndef ONLY_INTERNAL_CGOPS

/*  Flow operators -- call CGFlow() */

PICK( FLOW_AND,          ,                  "&&",    STR1,         , 0,  0                     )
PICK( FLOW_OR,           ,                  "||",    STR1,         , 0,  0                     )
PICK( FLOW_OUT,          ,                  ,        STR2,         , 0,  0                     )
PICK( FLOW_NOT,          ,                  "!",     STR1,         , 0,  0                     )
                                                                                              
PICK( POINTS,            ,                  "@",     STR1,         , 0,  0                     )   /* CGUnary() */

/* % Control operators -- call CGControl(),CGBigGoto(),CGBigLabel() */

PICK( GOTO,              ,                  ,        STR2,         , 0,  0                     )
PICK( BIG_GOTO,          ,                  ,        STR2,         , 0,  0                     )
PICK( IF_TRUE,           ,                  ,        STR2,         , 0,  0                     )
PICK( IF_FALSE,          ,                  ,        STR2,         , 0,  0                     )
PICK( INVOKE_LABEL,      ,                  ,        STR2,         , 0,  0                     )
PICK( LABEL,             ,                  ,        STR2,         , 0,  0                     )
PICK( BIG_LABEL,         ,                  ,        STR2,         , 0,  0                     )
PICK( LABEL_RETURN,      ,                  ,        STR2,         , 0,  0                     )

PICK( PROC,              ,                  ,        STR2,         , 0,  0                     )   /* CGProcDecl */
PICK( PARM_DEF,          ,                  ,        STR2,         , 0,  0                     )   /* CGParmDecl */
PICK( AUTO_DEF,          ,                  ,        STR2,         , 0,  0                     )   /* CGAutoDecl */
PICK( COMMA,             ,                  ,        STR2,         , 0,  0                     )

PICK( PASS_PROC_PARM,    ,                  ,        STR2,         , 0,  0                     )   /* CGPassPP */
PICK( DEFN_PROC_PARM,    ,                  ,        STR2,         , 0,  0                     )   /* CGDeclPP */
PICK( CALL_PROC_PARM,    ,                  ,        STR2,         , 0,  0                     )   /* CGCallPP */

PICK( PRE_GETS,          ,                  ,        STR2,         , 0,  0                     )   /* CGPreGets, CGLVPreGets */
PICK( POST_GETS,         ,                  ,        STR2,         , 0,  0                     )   /* CGPostGets */

PICK( SIDE_EFFECT,       ,                  ,        STR2,         , 0,  0                     )

#endif

