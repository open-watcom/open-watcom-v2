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


// 93/12/20 (jww) force re-compile

#include "cgstd.h"
#include "cg.h"
#include "bckdef.h"
#include "typclass.h"
#include "typedef.h"
#include "cfloat.h"
#include "cgaux.h"
#include "model.h"
#include "cgstub.h"
#define extern
#include "stubdata.h"
cg_type CGIntTypes[] = {
    TY_UINT_1,
    TY_INT_1,
    TY_UINT_2,
    TY_INT_2,
    TY_UINT_4,
    TY_INT_4,
    TY_LONG_POINTER,
    TY_HUGE_POINTER,
    TY_NEAR_POINTER,
    TY_INTEGER,
    TY_UNSIGNED,
    TY_POINTER,
    TY_LONG_CODE_PTR,
    TY_NEAR_CODE_PTR,
    TY_CODE_PTR,
    TY_SINGLE,
    TY_DOUBLE,
    TY_DEFAULT
    };
cg_type DGIntTypes[] = {
    TY_UINT_1,
    TY_INT_1,
    TY_UINT_2,
    TY_INT_2,
    TY_UINT_4,
    TY_INT_4,
    TY_LONG_POINTER,
    TY_HUGE_POINTER,
    TY_NEAR_POINTER,
    TY_INTEGER,
    TY_UNSIGNED,
    TY_POINTER,
    TY_LONG_CODE_PTR,
    TY_NEAR_CODE_PTR,
    TY_CODE_PTR,
    TY_DEFAULT
};
cg_type FloatTypes[] = {
    TY_SINGLE,
    TY_DOUBLE,
    TY_DEFAULT
};
cg_op ControlOps[] = {
    O_GOTO,
    O_LABEL,
    O_INVOKE_LABEL,
    O_LABEL_RETURN,
    O_IF_TRUE,
    O_IF_FALSE,
    O_NOP
};
cg_op CompareOps[] = {
    O_EQ,
    O_NE,
    O_LT,
    O_GT,
    O_LE,
    O_GE,
    O_NOP
};
cg_op FlowOps[] = {
    O_FLOW_AND,
    O_FLOW_OR,
    O_FLOW_NOT,
    O_NOP
};
cg_op UnaryOps[] = {
    O_UMINUS,
    O_COMPLEMENT,
    O_POINTS,
    O_CONVERT,
    O_ROUND,
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
    O_PARENTHESIS,
    O_PTR_TO_FOREIGN,
    O_PTR_TO_NATIVE,
    O_NOP
};
cg_op BinaryOps[] = {
    O_PLUS,
    O_MINUS,
    O_TIMES,
    O_DIV,
    O_MOD,
    O_AND,
    O_OR,
    O_XOR,
    O_LSHIFT,
    O_RSHIFT,
    O_COMMA,
    O_POW,
    O_ATAN2,
    O_FMOD,
    O_CONVERT,
    O_SIDE_EFFECT,
    O_NOP
};
cg_op FunkyOps[] = {
    O_POW,
    O_ATAN2,
    O_FMOD,
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
    O_RETURN,
    O_NOP
};
char * Classes[] = {
    "CG_FE",
    "CG_LBL",
    "CG_BACK",
    "CG_TBL",
    ""
};
char * Ops[] = {
        "",
        "+",
        "O_INTERNAL_01",
        "-",
        "O_INTERNAL_02",
        "*",
        "O_INTERNAL_03",
        "/",
        "%",
        "&",
        "|",
        "^",
        ">>",
        "<<",
        "**",
        "O_INTERNAL_16",
        "atan2",
        "fmod",
        "-",
        "~",
        "log",
        "cos",
        "sin",
        "tan",
        "sqrt",
        "fabs",
        "acos",
        "asin",
        "atan",
        "cosh",
        "exp",
        "log10",
        "sinh",
        "tanh",
        "PTR_TO_NATIVE",
        "PTR_TO_FOREIGN",
        "PARENTHESIS",
        "<c=",
        "<r=",
        "=",
        "O_INTERNAL_05",
        "O_INTERNAL_06",
        "O_INTERNAL_07",
        "O_INTERNAL_08",
        "O_INTERNAL_09",
        "O_INTERNAL_10",
        "O_INTERNAL_11",
        "O_INTERNAL_14",
        "O_INTERNAL_15",
        "==",
        "!=",
        ">",
        "<=",
        "<",
        ">=",
        "O_INTERNAL_12",
        "O_INTERNAL_17",
        "O_INTERNAL_18",
        "O_INTERNAL_19",
        "O_INTERNAL_20",
        "O_INTERNAL_21",
        "O_INTERNAL_22",
        "O_INTERNAL_23",
        "O_INTERNAL_24",
        "O_INTERNAL_25",
        "O_INTERNAL_26",
        "O_INTERNAL_27",
        "O_INTERNAL_28",
        "O_INTERNAL_29",
        "O_INTERNAL_30",
        "O_INTERNAL_31",
        "O_INTERNAL_32",
        "O_INTERNAL_33",
        "O_INTERNAL_34",
        "O_SLACK_29",
        "O_SLACK_30",
        "O_SLACK_31",
        "O_SLACK_32",
        "O_SLACK_33",
        "O_SLACK_34",
        "O_SLACK_35",
        "O_SLACK_36",
        "O_SLACK_37",
        "O_SLACK_38",
        "O_SLACK_39",
        "O_INTERNAL_13",
        "&&",
        "||",
        "O_FLOW_OUT",
        "!",
        "@",
        "O_GOTO",
        "O_BIG_GOTO",
        "O_IF_TRUE",
        "O_IF_FALSE",
        "O_INVOKE_LABEL",
        "O_LABEL",
        "O_BIG_LABEL",
        "O_LABEL_RETURN",
        "O_PROC",
        "O_PARM_DEF",
        "O_AUTO_DEF",
        "O_COMMA",
        "O_PASS_PROC_PARM",
        "O_DEFN_PROC_PARM",
        "O_CALL_PROC_PARM",
        "O_PRE_GETS",
        "O_POST_GETS",
        "O_SIDE_EFFECT",
        "",
        ",",
        "return",
        "O_LV_GETS",
        "O_LV_PRE_GETS",
        ""
};
char * Tipes[] = {
    "T_UINT_1",
    "T_INT_1",
    "T_UINT_2",
    "T_INT_2",
    "T_UINT_4",
    "T_INT_4",
    "T_UINT_8",
    "T_INT_8",
    "T_LONG_POINTER",
    "T_HUGE_POINTER",
    "T_NEAR_POINTER",
    "T_LONG_CODE_PTR",
    "T_NEAR_CODE_PTR",
    "T_SINGLE",
    "T_DOUBLE",
    "T_LONG_DOUBLE",
    "T_UNKNOWN",
    "T_DEFAULT",
    "T_INTEGER",
    "T_UNSIGNED",
    "T_POINTER",
    "T_CODE_PTR",
    "T_BOOLEAN",
    "T_PROC_PARM",
    ""
};
