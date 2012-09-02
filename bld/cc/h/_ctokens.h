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
* Description:  Token constant definitions
*
****************************************************************************/


pick( T_NULL,                   "end of line",          TC_XX )
pick( T_EOF,                    "end of file",          TC_XX )
pick( T_COMMA,                  ",",                    TC_COMMA )
pick( T_QUESTION,               "?",                    TC_QUESTION )
pick( T_COLON,                  ":",                    TC_COLON )
pick( T_SEMI_COLON,             ";",                    TC_XX )
pick( T_LEFT_PAREN,             "(",                    TC_FUNC_CALL )
pick( T_RIGHT_PAREN,            ")",                    TC_RIGHT_PAREN )
pick( T_LEFT_BRACKET,           "[",                    TC_LEFT_BRACKET )
pick( T_RIGHT_BRACKET,          "]",                    TC_RIGHT_BRACKET )
pick( T_LEFT_BRACE,             "{",                    TC_XX )
pick( T_RIGHT_BRACE,            "}",                    TC_XX )
pick( T_DOT,                    ".",                    TC_DOT )
pick( T_TILDE,                  "~",                    TC_XX  )
pick( T_EQUAL,                  "=",                    TC_ASSIGN_OP )
pick( T_EQ,                     "==",                   TC_EQ_NE )
pick( T_EXCLAMATION,            "!",                    TC_XX )
pick( T_NE,                     "!=",                   TC_EQ_NE )
pick( T_OR,                     "|",                    TC_OR )
pick( T_OR_EQUAL,               "|=",                   TC_ASSIGN_OP )
pick( T_OR_OR,                  "||",                   TC_OR_OR )
pick( T_XOR,                    "^",                    TC_XOR )
pick( T_XOR_EQUAL,              "^=",                   TC_ASSIGN_OP )
pick( T_AND,                    "&",                    TC_AND )
pick( T_AND_EQUAL,              "&=",                   TC_ASSIGN_OP )
pick( T_AND_AND,                "&&",                   TC_AND_AND )
pick( T_GT,                     ">",                    TC_REL_OP )
pick( T_GE,                     ">=",                   TC_REL_OP )
pick( T_RSHIFT,                 ">>",                   TC_SHIFT_OP )
pick( T_RSHIFT_EQUAL,           ">>=",                  TC_ASSIGN_OP )
pick( T_LT,                     "<",                    TC_REL_OP )
pick( T_LE,                     "<=",                   TC_REL_OP )
pick( T_LSHIFT,                 "<<",                   TC_SHIFT_OP )
pick( T_LSHIFT_EQUAL,           "<<=",                  TC_ASSIGN_OP )
pick( T_PLUS,                   "+",                    TC_ADD_OP )
pick( T_PLUS_EQUAL,             "+=",                   TC_ASSIGN_OP )
pick( T_PLUS_PLUS,              "++",                   TC_POSTINC )
pick( T_MINUS,                  "-",                    TC_ADD_OP )
pick( T_MINUS_EQUAL,            "-=",                   TC_ASSIGN_OP )
pick( T_MINUS_MINUS,            "--",                   TC_POSTINC )
pick( T_TIMES,                  "*",                    TC_MUL_OP )
pick( T_TIMES_EQUAL,            "*=",                   TC_ASSIGN_OP )
pick( T_DIVIDE,                 "/",                    TC_MUL_OP )
pick( T_DIVIDE_EQUAL,           "/=",                   TC_ASSIGN_OP )
pick( T_PERCENT,                "%",                    TC_MUL_OP )
pick( T_PERCENT_EQUAL,          "%=",                   TC_ASSIGN_OP )
pick( T_SHARP,                  "#",                    TC_XX )
pick( T_DOT_DOT_DOT,            "...",                  TC_XX )
pick( T_SHARP_SHARP,            "##",                   TC_XX )
pick( T_ARROW,                  "->",                   TC_ARROW )
pick( T_SEG_OP,                 ":>",                   TC_SEG_OP )
pick( T_ID,                     "identifier",           TC_XX )
pick( T_CONSTANT,               "constant",             TC_XX )
pick( T_STRING,                 "string",               TC_XX )

#define FIRST_KEYWORD   (T_STRING + 1)

#include "keywords.gh"

#define LAST_KEYWORD    (T_MACRO_PARM - 1)

pick( T_MACRO_PARM,             "macro parm",           TC_XX )
pick( T_MACRO_VAR_PARM,         "macro var parm",       TC_XX )
pick( T_MACRO_EMPTY_VAR_PARM,   "empty macro var parm", TC_XX )
pick( T_MACRO,                  "macro",                TC_XX )
pick( T_FUNC_DEFN,              "function",             TC_XX )
pick( T_FUNC_END,               "fnend",                TC_XX )
pick( T_JUMP,                   "jump",                 TC_XX )
pick( T_JUMPTRUE,               "jumptrue",             TC_XX )
pick( T_JUMPFALSE,              "jumpfalse",            TC_XX )
pick( T_PARM_LIST,              "parm_list",            TC_XX )
pick( T_PARM,                   "parm",                 TC_XX )
pick( T_CALL,                   "call",                 TC_XX )
pick( T_CALL_NO_PARM,           "call",                 TC_XX )
pick( T_LABEL_DEFN,             "label",                TC_XX )
pick( T_ADDR_OF,                "&",                    TC_XX )
pick( T_END_FOR,                "endfor",               TC_XX )
pick( T_FORWARD,                "?",                    TC_XX )
pick( T_CONVERT,                "convert",              TC_XX )
pick( T_WHITE_SPACE,            " ",                    TC_XX )
pick( T_BAD_CHAR,               " ",                    TC_XX )
pick( T_ASSIGN_LAST,            "=",                    TC_XX )
pick( T_SAVED_ID,               "identifier",           TC_XX )
pick( T_MACRO_SHARP_SHARP,      "##",                   TC_XX )
pick( T_PPNUMBER,               "ppnumber",             TC_XX )
pick( T_MACRO_SHARP,            "#",                    TC_XX )
pick( T_BAD_TOKEN,              " ",                    TC_XX )
pick( T_COMMA_DOT,              ",.",                   TC_XX )
pick( T_ATSIGN,                 "@",                    TC_XX )
pick( T_MATHFUNC,               "math",                 TC_XX )
pick( T_LSTRING,                "string",               TC_XX )
pick( T_EXCEPT_CODE,            "_exception_code",      TC_XX )
pick( T_EXCEPT_INFO,            "_exception_info",      TC_XX )
pick( T_END_FINALLY,            "_end_finally",         TC_XX )
pick( T_UNROLL,                 "unroll",               TC_XX )
pick( T_UNWIND,                 "unwind",               TC_XX )
pick( T_VA_START,               "va_start",             TC_XX )
pick( T_ALLOCA,                 "alloca",               TC_XX )
pick( T_LAST_TOKEN,             " ",                    TC_XX )

