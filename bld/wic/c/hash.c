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


#include <stdio.h>
#include <string.h>
#include "wic.h"

TokTab  tokenTable[]  =
{
    "\n",               Y_PRE_NEWLINE,    "\n",         "\n",
    "!",                Y_EXCLAMATION,    "0.EQ.",      "!",
    "!=",               Y_NE,             ".NE.",       "NE",
    "#",                Y_POUND,          "#",          "#",
    "##",               Y_POUND_POUND,    "##",         "##",
    "#define",          Y_PRE_DEFINE,     "*$DEFINE",   "",
    "#elif",            Y_PRE_ELIF,       "*$ELIF",     "ELSE IF",
    "#else",            Y_PRE_ELSE,       "*$ELSE",     "ELSE",
    "#endif",           Y_PRE_ENDIF,      "*$ENDIF",    "ENDIF",
    "#error",           Y_PRE_ERROR,      "*$DEFINE",   "ERROR",
    "#if",              Y_PRE_IF,         "*$IF",       "IF",
    "#ifdef",           Y_PRE_IFDEF,      "*$IFDEF",    "IFDEF",
    "#ifndef",          Y_PRE_IFNDEF,     "*$IFNDEF",   "IFNDEF",
    "#include",         Y_PRE_INCLUDE,    "*$INCLUDE",  "INCLUDE",
    "#line",            Y_PRE_LINE,       "*$LINE",     "LINE",
    "#pragma",          Y_PRE_PRAGMA,     "*$PRAGMA",   "PRAGMA",
    "#undef",           Y_PRE_UNDEF,      "*$UNDEFINE", "UNDEF",
    "%",                Y_PERCENT,        "MOD",        "MOD",
    "%=",               Y_PERCENT_EQUAL,  "%=",         "%=",
    "&",                Y_AND,            ".AND.",      "AND",
    "&&",               Y_AND_AND,        ".AND.",      "&&",
    "&&=",              Y_AND_EQUAL,      "&&=",        "&&=",
    "(",                Y_LEFT_PAREN,     "(",          "(",
    ")",                Y_RIGHT_PAREN,    ")",          ")",
    "*",                Y_TIMES,          "*",          "*",
    "*=",               Y_TIMES_EQUAL,    "*=",         "*=",
    "+",                Y_PLUS,           "+",          "+",
    "++",               Y_PLUS_PLUS,      "++",         "++",
    "+=",               Y_PLUS_EQUAL,     "+=",         "+=",
    ",",                Y_COMMA,          ",",          ",",
    "-",                Y_MINUS,          "-",          "-",
    "--",               Y_MINUS_MINUS,    "--",         "--",
    "-=",               Y_MINUS_EQUAL,    "-=",         "-=",
    "->",               Y_ARROW,          "->",         "->",
    ".",                Y_DOT,            "%",          ".",
    "...",              Y_DOT_DOT_DOT,    "...",        "...",
    "/",                Y_DIVIDE,         "/",          "/",
    "/=",               Y_DIVIDE_EQUAL,   "/=",         "/=",
    ":",                Y_COLON,          ":",          ":",
    ":>",               Y_SEG_OP,         ":>",         ":>",
    ";",                Y_SEMICOLON,      ";",          ";",
    "<",                Y_LT,             ".LT.",       "LT",
    "<<",               Y_LSHIFT,         "<<",         "SHL",
    "<<=",              Y_LSHIFT_EQUAL,   "<<=",        "<<=",
    "<=",               Y_LE,             ".LE.",       "LE",
    "=",                Y_EQUAL,          "=",          "=",
    "==",               Y_EQ,             ".EQ.",       "EQ",
    ">",                Y_GT,             ".GT.",       "GT",
    ">=",               Y_GE,             ".GE.",       "GE",
    ">>",               Y_RSHIFT,         "ISHFT",      "SHR",
    ">>=",              Y_RSHIFT_EQUAL,   ">>=",        ">>=",
    "?",                Y_QUESTION,       "?",          "?",
    "[",                Y_LEFT_BRACKET,   "(",          "[",
    "]",                Y_RIGHT_BRACKET,  ")",          "]",
    "^",                Y_XOR,            ".XOR.",      "XOR",
    "^=",               Y_XOR_EQUAL,      "^=",         "^=",
    "_Cdecl",           Y___CDECL,        "__cdecl",    "__cdecl",
    "_Far16",           Y___FAR16,        "__far16",    "__far16",
    "_Pascal",          Y___PASCAL,       "__pascal",   "__pascal",
    "_Seg16",           Y___FAR16,        "_Seg16",     "_Seg16",
    "_System",          Y__SYSCALL,       "_System",    "_System",
    "__based",          Y___BASED,        "__based",    "__based",
    "__cdecl",          Y___CDECL,        "__cdecl",    "__cdecl",
    "__export",         Y___EXPORT,       "__export",   "__export",
    "__far",            Y___FAR,          "__far",      "__far",
    "__far16",          Y___FAR16,        "__far16",    "__far16",
    "__fortran",        Y___FORTRAN,      "__fortran",  "__fortran",
    "__huge",           Y___HUGE,         "__huge",     "__huge",
    "__interrupt",      Y___INTERRUPT,    "__interrupt","__interrupt",
    "__loadds",         Y___LOADDS,       "__loadds",   "__loadds",
    "__near",           Y___NEAR,         "__near",     "__near",
    "__pascal",         Y___PASCAL,       "__pascal",   "__pascal",
    "__pragma",         Y___PRAGMA,       "__pragma",   "__pragma",
    "__saveregs",       Y___SAVEREGS,     "__saveregs", "__saveregs",
    "__segment",        Y___SEGMENT,      "__segment",  "__segment",
    "__segname",        Y___SEGNAME,      "__segname",  "__segname",
    "__self",           Y___SELF,         "__self",     "__self",
    "__stdcall",        Y___STDCALL,      "__stdcall",  "__stdcall",
    "_far",             Y___FAR,          "__far",      "__far",
    "_huge",            Y___HUGE,         "__huge",     "__huge",
    "_near",            Y___NEAR,         "__near",     "__near",
    "_packed",          Y__PACKED,        "_packed",    "_packed",
    "_syscall",         Y__SYSCALL,       "_syscall",   "_syscall",
    "auto",             Y_AUTO,           "auto",       "auto",
    "char",             Y_CHAR,           "char",       "char",
    "const",            Y_CONST,          "const",      "const",
    "double",           Y_DOUBLE,         "double",     "double",
    "defined",          Y_DEFINED,        "DEFINED",    "DEFINED",
    "else",             Y_ELSE,           "ELSE",       "else",
    "enum",             Y_ENUM,           "enum",       "enum",
    "extern",           Y_EXTERN,         "extern",     "extern",
    "far",              Y___FAR,          "far",        "far",
    "float",            Y_FLOAT,          "float",      "float",
    "int",              Y_INT,            "int",        "int",
    "long",             Y_LONG,           "long",       "long",
    "near",             Y___NEAR,         "near",       "near",
    "register",         Y_REGISTER,       "register",   "register",
    "short",            Y_SHORT,          "short",      "short",
    "signed",           Y_SIGNED,         "signed",     "signed",
    "sizeof",           Y_SIZEOF,         "sizeof",     "size",
    "static",           Y_STATIC,         "static",     "static",
    "struct",           Y_STRUCT,         "struct",     "struct",
    "typedef",          Y_TYPEDEF,        "typedef",    "typedef",
    "union",            Y_UNION,          "union",      "union",
    "unsigned",         Y_UNSIGNED,       "unsigned",   "unsigned",
    "void",             Y_VOID,           "void",       "void",
    "volatile",         Y_VOLATILE,       "volatile",   "volatile",
    "{",                Y_LEFT_BRACE,     "{",          "{",
    "|",                Y_OR,             ".OR.",       "OR",
    "|=",               Y_OR_EQUAL,       "|=",         "|=",
    "||",               Y_OR_OR,          ".OR.",       "||",
    "}",                Y_RIGHT_BRACE,    "}",          "}",
    "~",                Y_TILDE,          ".NOT.",      "NOT"
};

#define SIZE (sizeof tokenTable / sizeof tokenTable[0])

#define M       256

static pTokTab hash[M][2];
//  There are no more than 2 collisions per keyword.  If, when more
//  keywords added, more than 2 collisions per keyword occur, try to
//  modify the value of 'b' in 'computeKey'  or increase M.
//  M MUST be a in a form 2^x.

static int computeKey(char *s) {
    enum { b = 101 };
    unsigned long key = 0;
    int i;
    for (i = 0; s[i] != 0; i++) {
        key += s[i];
        key *= b;
    }
    key = key & (M-1);
    return key;
}

void initHashTable(void) {
    int i, key;
    for (i = 0; i < SIZE; i++) {
        key = computeKey(tokenTable[i].name);
        assert(key < M && key >= 0);
        if (hash[key][0] != NULL) {
            if (hash[key][1] != NULL) {
                printf("Fatal collision: %s %s %s\n",
                        tokenTable[i].name, hash[key][0], hash[key][1]);
                printf("I = %d, key = %d\n", i, key);
                assert(0);
                // See comment at the declaration of 'hash'
            } else {
                hash[key][1] = &(tokenTable[i]);
            }
        } else {
            hash[key][0] = &(tokenTable[i]);
        }
    }
}


pTokTab tabLookup(char *name)
{
    unsigned key;
    pTokTab retval = NULL;
    int i;

    key =  computeKey(name);

    for (i = 0; i < 2; i++) {
        if (hash[key][i] != NULL) {
            if (strcmp(name, hash[key][i]->name) == 0) {
                retval = hash[key][i];
                break;
            }
        }
    }
    return retval;
}
