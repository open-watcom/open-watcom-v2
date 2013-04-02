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


#define NO_PREC     (-1)
#define PREC_UNARY  15
#define MAX_PREC    15

/*    enum                              text                     class  precedence  */
pick( T_NULL,                           "end of line",           TC_A0, -1 )
pick( T_EOF,                            "end of file",           TC_A0, -1 )
pick( T_START,                          "",                      TC_A3, 0  )
pick( T_COMMA,                          ",",                     TC_A2, -1 )
pick( T_QUESTION,                       "?",                     TC_A2, 4  )
pick( T_COLON,                          ":",                     TC_A2, 3  )
pick( T_SEMI_COLON,                     ";",                     TC_A2, -1 )
pick( T_COLON_COLON,                    "::",                    TC_A1, -1 )
pick( T_LEFT_PAREN,                     "(",                     TC_A2, 2  )
pick( T_RIGHT_PAREN,                    ")",                     TC_A2, 1  )
pick( T_LEFT_BRACKET,                   "[",                     TC_A2, -1 )
pick( T_RIGHT_BRACKET,                  "]",                     TC_A2, -1 )
pick( T_LEFT_BRACE,                     "{",                     TC_A2, -1 )
pick( T_RIGHT_BRACE,                    "}",                     TC_A2, -1 )
pick( T_DOT,                            ".",                     TC_A2, -1 )
pick( T_TILDE,                          "~",                     TC_A2, 15 )
pick( T_EQUAL,                          "=",                     TC_A2, -1 )
pick( T_EQ,                             "==",                    TC_A1, 10 )
pick( T_EXCLAMATION,                    "!",                     TC_A2, 15 )
pick( T_NE,                             "!=",                    TC_A1, 10 )
pick( T_OR,                             "|",                     TC_A2, 7  )
pick( T_OR_EQUAL,                       "|=",                    TC_A1, -1 )
pick( T_OR_OR,                          "||",                    TC_A1, 5  )
pick( T_XOR,                            "^",                     TC_A2, 8  )
pick( T_XOR_EQUAL,                      "^=",                    TC_A1, -1 )
pick( T_AND,                            "&",                     TC_A2, 9  )
pick( T_AND_EQUAL,                      "&=",                    TC_A1, -1 )
pick( T_AND_AND,                        "&&",                    TC_A1, 6  )
pick( T_GT,                             ">",                     TC_A2, 11 )
pick( T_GE,                             ">=",                    TC_A1, 11 )
pick( T_RSHIFT,                         ">>",                    TC_A1, 12 )
pick( T_RSHIFT_EQUAL,                   ">>=",                   TC_A0, -1 )
pick( T_LT,                             "<",                     TC_A2, 11 )
pick( T_LE,                             "<=",                    TC_A1, 11 )
pick( T_LSHIFT,                         "<<",                    TC_A1, 12 )
pick( T_LSHIFT_EQUAL,                   "<<=",                   TC_A0, -1 )
pick( T_PLUS,                           "+",                     TC_A2, 13 )
pick( T_PLUS_EQUAL,                     "+=",                    TC_A1, -1 )
pick( T_PLUS_PLUS,                      "++",                    TC_A1, -1 )
pick( T_MINUS,                          "-",                     TC_A2, 13 )
pick( T_MINUS_EQUAL,                    "-=",                    TC_A1, -1 )
pick( T_MINUS_MINUS,                    "--",                    TC_A1, -1 )
pick( T_TIMES,                          "*",                     TC_A2, 14 )
pick( T_TIMES_EQUAL,                    "*=",                    TC_A1, -1 )
pick( T_DIVIDE,                         "/",                     TC_A2, 14 )
pick( T_DIVIDE_EQUAL,                   "/=",                    TC_A1, -1 )
pick( T_PERCENT,                        "%",                     TC_A2, 14 )
pick( T_PERCENT_EQUAL,                  "%=",                    TC_A1, -1 )
pick( T_SHARP,                          "#",                     TC_A2, -1 )
pick( T_DOT_DOT_DOT,                    "...",                   TC_A0, -1 )
pick( T_SHARP_SHARP,                    "##",                    TC_A1, -1 )
pick( T_ARROW,                          "->",                    TC_A1, -1 )
pick( T_SEG_OP,                         ":>",                    TC_A1, -1 )
pick( T_ARROW_STAR,                     "->*",                   TC_A0, -1 )
pick( T_DOT_STAR,                       ".*",                    TC_A1, -1 )
pick( T_ID,                             "identifier",            TC_A1, -1 )
pick( T_UNKNOWN_ID,                     "identifier",            TC_A1, -1 )
pick( T_TEMPLATE_ID,                    "identifier",            TC_A1, -1 )
pick( T_CONSTANT,                       "constant",              TC_A3, -1 )
pick( T_PPNUMBER,                       "pp-number",             TC_A2, -1 )
pick( T_STRING,                         "string",                TC_A1, -1 )
pick( T_LSTRING,                        "wide string",           TC_A0, -1 )
pick( T_WHITE_SPACE,                    " ",                     TC_A2, -1 )
pick( T_ATSIGN,                         "@",                     TC_A2, -1 )
pick( T_BAD_CHAR,                       "bad character",         TC_A2, -1 )
pick( T_SAVED_ID,                       "identifier",            TC_A1, -1 )
pick( T_BAD_TOKEN,                      "bad token",             TC_A2, -1 )
pick( T_MACRO_PARM,                     "macro parm",            TC_A1, -1 )
pick( T_MACRO,                          "macro",                 TC_A2, -1 )
pick( T_UNARY_PLUS,                     "+",                     TC_A2, 15 )
pick( T_UNARY_MINUS,                    "-",                     TC_A2, 15 )
pick( T_MACRO_SHARP_SHARP,              "##",                    TC_A1, -1 )
pick( T_MACRO_SHARP,                    "#",                     TC_A2, -1 )
pick( T_GLOBAL_ID,                      "::<id>",                TC_A1, -1 )
pick( T_GLOBAL_UNKNOWN_ID,              "::<id>",                TC_A1, -1 )
pick( T_GLOBAL_TEMPLATE_ID,             "::<id>",                TC_A1, -1 )
pick( T_GLOBAL_TYPE_NAME,               "::<type-name>",         TC_A2, -1 )
pick( T_GLOBAL_TEMPLATE_NAME,           "::<template-name>",     TC_A2, -1 )
pick( T_GLOBAL_NAMESPACE_NAME,          "::<namespace-name>",    TC_A1, -1 )
pick( T_GLOBAL_OPERATOR,                "::operator",            TC_A1, -1 )
pick( T_GLOBAL_TILDE,                   "::~",                   TC_A0, -1 )
pick( T_GLOBAL_NEW,                     "::new",                 TC_A2, -1 )
pick( T_GLOBAL_DELETE,                  "::delete",              TC_A3, -1 )
pick( T_SCOPED_ID,                      "C::<id>",               TC_A0, -1 )
pick( T_SCOPED_UNKNOWN_ID,              "C::<id>",               TC_A0, -1 )
pick( T_SCOPED_TEMPLATE_ID,             "C::<id>",               TC_A0, -1 )
pick( T_SCOPED_TYPE_NAME,               "C::<type-name>",        TC_A1, -1 )
pick( T_SCOPED_TEMPLATE_NAME,           "C::<template-name>",    TC_A1, -1 )
pick( T_SCOPED_NAMESPACE_NAME,          "C::<namespace-name>",   TC_A0, -1 )
pick( T_SCOPED_OPERATOR,                "C::operator",           TC_A0, -1 )
pick( T_SCOPED_TILDE,                   "C::~",                  TC_A3, -1 )
pick( T_SCOPED_TIMES,                   "C::*",                  TC_A3, -1 )
pick( T_TEMPLATE_SCOPED_ID,             "T<>::<id>",             TC_A2, -1 )
pick( T_TEMPLATE_SCOPED_UNKNOWN_ID,     "T<>::<id>",             TC_A2, -1 )
pick( T_TEMPLATE_SCOPED_TEMPLATE_ID,    "T<>::<id>",             TC_A2, -1 )
pick( T_TEMPLATE_SCOPED_TYPE_NAME,      "T<>::<type-name>",      TC_A3, -1 )
pick( T_TEMPLATE_SCOPED_TEMPLATE_NAME,  "T<>::<template-name>",  TC_A3, -1 )
pick( T_TEMPLATE_SCOPED_NAMESPACE_NAME, "T<>::<namespace-name>", TC_A2, -1 )
pick( T_TEMPLATE_SCOPED_OPERATOR,       "T<>::operator",         TC_A2, -1 )
pick( T_TEMPLATE_SCOPED_TILDE,          "T<>::~",                TC_A1, -1 )
pick( T_TEMPLATE_SCOPED_TIMES,          "T<>::*",                TC_A1, -1 )
pick( T_IMPOSSIBLE,                     "???",                   TC_A0, -1 )
pick( T_DEFARG_END,                     "<end-defarg>",          TC_A3, -1 )
pick( T_FILLER0,                        "",                      TC_A3, -1 )
pick( T_FILLER1,                        "",                      TC_A3, -1 )
pick( T_FILLER2,                        "",                      TC_A3, -1 )
pick( T_FILLER3,                        "",                      TC_A3, -1 )
pick( T_FILLER4,                        "",                      TC_A3, -1 )
pick( T_FILLER5,                        "",                      TC_A3, -1 )
pick( T_FILLER6,                        "",                      TC_A3, -1 )
pick( T_FILLER7,                        "",                      TC_A3, -1 )
pick( T_FILLER8,                        "",                      TC_A3, -1 )
pick( T_FILLER9,                        "",                      TC_A3, -1 )

/* digraphs */
pick( T_ALT_LEFT_BRACKET,               "<:",                    TC_A1, -1 )
pick( T_ALT_RIGHT_BRACKET,              ":>",                    TC_A1, -1 )
pick( T_ALT_LEFT_BRACE,                 "<%",                    TC_A1, -1 )
pick( T_ALT_RIGHT_BRACE,                "%>",                    TC_A1, -1 )
pick( T_ALT_SHARP,                      "%:",                    TC_A1, -1 )
pick( T_ALT_SHARP_SHARP,                "%:%:",                  TC_A3, -1 )

/* alternative tokens */
pick( T_ALT_AND,                        "bitand",                TC_A1, 9  )
pick( T_ALT_AND_EQUAL,                  "andeq",                 TC_A2, -1 )
pick( T_ALT_AND_AND,                    "and",                   TC_A0, 6  )
pick( T_ALT_OR,                         "bitor",                 TC_A2, 7  )
pick( T_ALT_OR_EQUAL,                   "or_eq",                 TC_A2, -1 )
pick( T_ALT_OR_OR,                      "or",                    TC_A1, 5  )
pick( T_ALT_XOR,                        "xor",                   TC_A0, 8  )
pick( T_ALT_XOR_EQUAL,                  "xor_eq",                TC_A1, -1 )
pick( T_ALT_EXCLAMATION,                "not",                   TC_A0, 15 )
pick( T_ALT_NE,                         "not_eq",                TC_A1, 10 )
pick( T_ALT_TILDE,                      "compl",                 TC_A2, 15 )

#ifndef define_precedence

#define FIRST_KEYWORD   (T_ALT_TILDE + 1)

/* start of keywords */
#include "keywords.gh"
                                                                 
#define LAST_KEYWORD    (T_MACRO_VAR_PARM - 1)

pick( T_MACRO_VAR_PARM,                 "macro var parm",        TC_A1, -1 )
pick( T_MACRO_EMPTY_VAR_PARM,           "macro var empty parm",  TC_A1, -1 )

#endif
