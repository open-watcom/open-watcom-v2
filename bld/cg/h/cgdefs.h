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


#ifndef _CG_DEFS_INCLUDED

/*  Start of Internal Code Generator Operators ... Must correspond to */
/*  code generator header file <opcodes.h> */


typedef enum {
        O_NOP,

/*  Binary Ops  -- call CGBinary() */

        O_PLUS,
        O_INTERNAL_01,
        O_MINUS,
        O_INTERNAL_02,
        O_TIMES,
        O_INTERNAL_03,
        O_DIV,
        O_MOD,
        O_AND,
        O_OR,
        O_XOR,
        O_RSHIFT,
        O_LSHIFT,
        O_POW,
        O_INTERNAL_16,
        O_ATAN2,
        O_FMOD,

/*  Unary Ops -- call CGUnary() */

        O_UMINUS,
        O_COMPLEMENT,
        O_LOG,
        O_COS,
        O_SIN,
        O_TAN,
        O_SQRT,
        O_FABS,
        O_ACOS,
        O_ASIN,
        O_ATAN,
        O_COSH,
        O_EXP,
        O_LOG10,
        O_SINH,
        O_TANH,

        O_PTR_TO_NATIVE, // oh! ouch! the 386 really hurts my head!
        O_PTR_TO_FORIEGN,
        O_PTR_TO_FOREIGN = O_PTR_TO_FORIEGN,
        O_PARENTHESIS,  /* optimization control - NOP for now */

        O_CONVERT,
        O_INTERNAL_05,  /* LA, CAREFUL_LA */
        O_INTERNAL_06,
        O_ROUND,
        O_GETS,         /* CGAssign(), CGLVAssign() */

/*  calling operators */

        O_INTERNAL_07,
        O_INTERNAL_08,
        O_INTERNAL_09,
        O_INTERNAL_10,  /* CGAddParm() */

/*  Internal */

        O_INTERNAL_11,

/*  Comparison operators -- call CGCompare() */

        O_INTERNAL_14,
        O_INTERNAL_15,
        O_EQ,
        O_NE,
        O_GT,
        O_LE,
        O_LT,
        O_GE,

/*  Call -- call CGCall() */

        O_INTERNAL_12,

/*  Slack */

        O_INTERNAL_17,
        O_INTERNAL_18,
        O_INTERNAL_19,
        O_INTERNAL_20,
        O_INTERNAL_21,
        O_INTERNAL_22,
        O_INTERNAL_23,
        O_INTERNAL_24,
        O_INTERNAL_25,
        O_INTERNAL_26,
        O_INTERNAL_27,
        O_INTERNAL_28,
        O_INTERNAL_29,
        O_INTERNAL_30,
        O_INTERNAL_31,
        O_INTERNAL_32,
        O_INTERNAL_33,
        O_INTERNAL_34,
        O_STACK_ALLOC,
        O_VA_START,
        O_SLACK_31,
        O_SLACK_32,
        O_SLACK_33,
        O_SLACK_34,
        O_SLACK_35,
        O_SLACK_36,
        O_SLACK_37,
        O_SLACK_38,
        O_SLACK_39,

/*  Internal */

        O_INTERNAL_13,

/*  End of Internal Code Generator operators (opcode_defs) */
/*  Flow operators -- call CGFlow() */

        O_FLOW_AND,
        O_FLOW_OR,
        O_FLOW_OUT,
        O_FLOW_NOT,

        O_POINTS,       /* CGUnary() */

/* % Control operators -- call CGControl(),CGBigGoto(),CGBigLabel() */

        O_GOTO,
        O_BIG_GOTO,
        O_IF_TRUE,
        O_IF_FALSE,
        O_INVOKE_LABEL,
        O_LABEL,
        O_BIG_LABEL,
        O_LABEL_RETURN,

        O_PROC,         /*  CGProcDecl */
        O_PARM_DEF,     /*  CGParmDecl */
        O_AUTO_DEF,     /*  CGAutoDecl */
        O_COMMA,

        O_PASS_PROC_PARM,       /* CGPassPP */
        O_DEFN_PROC_PARM,       /* CGDeclPP */
        O_CALL_PROC_PARM,       /* CGCallPP */

        O_PRE_GETS,     /*     CGPreGets, CGLVPreGets */
        O_POST_GETS,    /*     CGPostGets */

        O_SIDE_EFFECT,

/*  Beginning of FRONT END SPECIFIC Operators */
/* */
/*  - Bonus plan. Add your own ops here! */
/* */
/*  End       of FRONT END SPECIFIC Operators */

        MAX_OP

} cg_op;

typedef enum {

/*  The first part must correspond to <typclass> */

        T_UINT_1,       /*   0 */
        T_INT_1,
        T_UINT_2,
        T_INT_2,
        T_UINT_4,
        T_INT_4,
        T_UINT_8,
        T_INT_8,

        T_LONG_POINTER,
        T_HUGE_POINTER,
        T_NEAR_POINTER,
        T_LONG_CODE_PTR,
        T_NEAR_CODE_PTR,

        T_SINGLE,
#ifdef BY_C_FRONT_END
        TY_DOUBLE,
#else
        T_DOUBLE,
#endif
        T_LONG_DOUBLE,

        T_UNKNOWN,

#ifdef BY_C_FRONT_END
        TY_DEFAULT,     /*  11  Use defaults */
#else
        T_DEFAULT,      /*  11  Use defaults */
#endif

        T_INTEGER,      /*  Default integer */
#ifdef BY_C_FRONT_END
        TY_UNSIGNED,    /*  Default unsigned */
#else
        T_UNSIGNED,     /*  Default unsigned */
#endif
        T_POINTER,      /*  Default data pointer */
        T_CODE_PTR,     /*  Default code pointer */
        T_BOOLEAN,      /*  Resultant type for O_FLOW, comparison ops */

        T_PROC_PARM,    /*  For Pascal procedural parameters */
        T_VA_LIST,      /*  For RISC-based O_VA_START support */
        T_FIRST_FREE    /*  First user definable type */
} predefined_cg_types;

#define T_HUGE_CODE_PTR T_LONG_CODE_PTR  /* for now */

#define T_LAST_FREE     65530U

typedef enum {
        T_NEAR_INTEGER = T_LAST_FREE,
        T_LONG_INTEGER,
        T_HUGE_INTEGER
} more_cg_types;

#define MIN_OP          O_NOP
#define O_FIRST_COND    O_EQ
#define O_LAST_COND     O_GE
#define O_FIRST_FLOW    O_FLOW_AND
#define O_LAST_FLOW     O_FLOW_NOT

typedef unsigned short  cg_type;

#define _CG_DEFS_INCLUDED
#endif
