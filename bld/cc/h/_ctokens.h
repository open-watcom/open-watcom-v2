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


pick( T_NULL,                   "end of line",          TC_XX,            OPR_NOP )
pick( T_EOF,                    "end of file",          TC_XX,            OPR_NOP )
pick( T_COMMA,                  ",",                    TC_COMMA,         OPR_COMMA )
pick( T_QUESTION,               "?",                    TC_QUESTION,      OPR_QUESTION )
pick( T_COLON,                  ":",                    TC_COLON,         OPR_COLON )
pick( T_SEMI_COLON,             ";",                    TC_XX,            OPR_NOP )
pick( T_LEFT_PAREN,             "(",                    TC_FUNC_CALL,     OPR_NOP )
pick( T_RIGHT_PAREN,            ")",                    TC_RIGHT_PAREN,   OPR_NOP )
pick( T_LEFT_BRACKET,           "[",                    TC_LEFT_BRACKET,  OPR_NOP )
pick( T_RIGHT_BRACKET,          "]",                    TC_RIGHT_BRACKET, OPR_NOP )
pick( T_LEFT_BRACE,             "{",                    TC_XX,            OPR_NOP )
pick( T_RIGHT_BRACE,            "}",                    TC_XX,            OPR_NOP )
pick( T_DOT,                    ".",                    TC_DOT,           OPR_NOP )
pick( T_TILDE,                  "~",                    TC_XX,            OPR_NOP )
pick( T_EQUAL,                  "=",                    TC_ASSIGN_OP,     OPR_EQUALS )
pick( T_EQ,                     "==",                   TC_EQ_NE,         OPR_NOP )
pick( T_EXCLAMATION,            "!",                    TC_XX,            OPR_NOT )
pick( T_NE,                     "!=",                   TC_EQ_NE,         OPR_NOP )
pick( T_OR,                     "|",                    TC_OR,            OPR_OR )
pick( T_OR_EQUAL,               "|=",                   TC_ASSIGN_OP,     OPR_OR_EQUAL )
pick( T_OR_OR,                  "||",                   TC_OR_OR,         OPR_OR_OR )
pick( T_XOR,                    "^",                    TC_XOR,           OPR_XOR )
pick( T_XOR_EQUAL,              "^=",                   TC_ASSIGN_OP,     OPR_XOR_EQUAL )
pick( T_AND,                    "&",                    TC_AND,           OPR_AND )
pick( T_AND_EQUAL,              "&=",                   TC_ASSIGN_OP,     OPR_AND_EQUAL )
pick( T_AND_AND,                "&&",                   TC_AND_AND,       OPR_AND_AND )
pick( T_GT,                     ">",                    TC_REL_OP,        OPR_NOP )
pick( T_GE,                     ">=",                   TC_REL_OP,        OPR_NOP )
pick( T_RSHIFT,                 ">>",                   TC_SHIFT_OP,      OPR_RSHIFT )
pick( T_RSHIFT_EQUAL,           ">>=",                  TC_ASSIGN_OP,     OPR_RSHIFT_EQUAL )
pick( T_LT,                     "<",                    TC_REL_OP,        OPR_NOP )
pick( T_LE,                     "<=",                   TC_REL_OP,        OPR_NOP )
pick( T_LSHIFT,                 "<<",                   TC_SHIFT_OP,      OPR_LSHIFT )
pick( T_LSHIFT_EQUAL,           "<<=",                  TC_ASSIGN_OP,     OPR_LSHIFT_EQUAL )
pick( T_PLUS,                   "+",                    TC_ADD_OP,        OPR_ADD )
pick( T_PLUS_EQUAL,             "+=",                   TC_ASSIGN_OP,     OPR_PLUS_EQUAL )
pick( T_PLUS_PLUS,              "++",                   TC_POSTINC,       OPR_POSTINC )
pick( T_MINUS,                  "-",                    TC_ADD_OP,        OPR_SUB )
pick( T_MINUS_EQUAL,            "-=",                   TC_ASSIGN_OP,     OPR_MINUS_EQUAL )
pick( T_MINUS_MINUS,            "--",                   TC_POSTINC,       OPR_POSTDEC )
pick( T_TIMES,                  "*",                    TC_MUL_OP,        OPR_MUL )
pick( T_TIMES_EQUAL,            "*=",                   TC_ASSIGN_OP,     OPR_TIMES_EQUAL )
pick( T_DIV,                    "/",                    TC_MUL_OP,        OPR_DIV )
pick( T_DIV_EQUAL,              "/=",                   TC_ASSIGN_OP,     OPR_DIV_EQUAL )
pick( T_PERCENT,                "%",                    TC_MUL_OP,        OPR_MOD )
pick( T_PERCENT_EQUAL,          "%=",                   TC_ASSIGN_OP,     OPR_MOD_EQUAL )

#ifndef OPERATORS_ONLY

pick( T_SHARP,                  "#",                    TC_XX,            OPR_NOP )
pick( T_DOT_DOT_DOT,            "...",                  TC_XX,            OPR_NOP )
pick( T_SHARP_SHARP,            "##",                   TC_XX,            OPR_NOP )
pick( T_ARROW,                  "->",                   TC_ARROW,         OPR_NOP )
pick( T_SEG_OP,                 ":>",                   TC_SEG_OP,        OPR_NOP )
pick( T_ID,                     "identifier",           TC_XX,            OPR_NOP )
pick( T_CONSTANT,               "constant",             TC_XX,            OPR_NOP )
pick( T_STRING,                 "string",               TC_XX,            OPR_NOP )

#define FIRST_KEYWORD   (T_STRING + 1)

#include "keywords.gh"

#define LAST_KEYWORD    (T_MACRO_PARM - 1)

pick( T_MACRO_PARM,             "macro parm",           TC_XX,            OPR_NOP )
pick( T_MACRO_VAR_PARM,         "macro var parm",       TC_XX,            OPR_NOP )
pick( T_MACRO_EMPTY_VAR_PARM,   "empty macro var parm", TC_XX,            OPR_NOP )
pick( T_MACRO,                  "macro",                TC_XX,            OPR_NOP )
pick( T_FUNC_DEFN,              "function",             TC_XX,            OPR_NOP )
pick( T_FUNC_END,               "fnend",                TC_XX,            OPR_NOP )
pick( T_JUMP,                   "jump",                 TC_XX,            OPR_NOP )
pick( T_JUMPTRUE,               "jumptrue",             TC_XX,            OPR_NOP )
pick( T_JUMPFALSE,              "jumpfalse",            TC_XX,            OPR_NOP )
pick( T_PARM_LIST,              "parm_list",            TC_XX,            OPR_NOP )
pick( T_PARM,                   "parm",                 TC_XX,            OPR_NOP )
pick( T_CALL,                   "call",                 TC_XX,            OPR_NOP )
pick( T_CALL_NO_PARM,           "call",                 TC_XX,            OPR_NOP )
pick( T_LABEL_DEFN,             "label",                TC_XX,            OPR_NOP )
pick( T_ADDR_OF,                "&",                    TC_XX,            OPR_NOP )
pick( T_END_FOR,                "endfor",               TC_XX,            OPR_NOP )
pick( T_FORWARD,                "?",                    TC_XX,            OPR_NOP )
pick( T_CONVERT,                "convert",              TC_XX,            OPR_NOP )
pick( T_WHITE_SPACE,            " ",                    TC_XX,            OPR_NOP )
pick( T_BAD_CHAR,               " ",                    TC_XX,            OPR_NOP )
pick( T_ASSIGN_LAST,            "=",                    TC_XX,            OPR_NOP )
pick( T_SAVED_ID,               "identifier",           TC_XX,            OPR_NOP )
pick( T_MACRO_SHARP_SHARP,      "##",                   TC_XX,            OPR_NOP )
pick( T_PPNUMBER,               "ppnumber",             TC_XX,            OPR_NOP )
pick( T_MACRO_SHARP,            "#",                    TC_XX,            OPR_NOP )
pick( T_BAD_TOKEN,              " ",                    TC_XX,            OPR_NOP )
pick( T_COMMA_DOT,              ",.",                   TC_XX,            OPR_NOP )
pick( T_ATSIGN,                 "@",                    TC_XX,            OPR_NOP )
pick( T_MATHFUNC,               "math",                 TC_XX,            OPR_NOP )
pick( T_LSTRING,                "string",               TC_XX,            OPR_NOP )
pick( T_EXCEPT_CODE,            "_exception_code",      TC_XX,            OPR_NOP )
pick( T_EXCEPT_INFO,            "_exception_info",      TC_XX,            OPR_NOP )
pick( T_END_FINALLY,            "_end_finally",         TC_XX,            OPR_NOP )
pick( T_UNROLL,                 "unroll",               TC_XX,            OPR_NOP )
pick( T_UNWIND,                 "unwind",               TC_XX,            OPR_NOP )
pick( T_VA_START,               "va_start",             TC_XX,            OPR_NOP )
pick( T_ALLOCA,                 "alloca",               TC_XX,            OPR_NOP )
pick( T_LAST_TOKEN,             " ",                    TC_XX,            OPR_NOP )

#endif
