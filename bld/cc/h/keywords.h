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


#define FIRST_KEYWORD   T___CDECL
pick( T___CDECL,       "__cdecl",      TC_CDECL ),
pick( T___PASCAL,      "__pascal",     TC_PASCAL ),
pick( T___SYSCALL,     "__syscall",    TC_SYSCALL ),
pick( T_AUTO,          "auto",         TC_STG_CLASS ),
pick( T_SWITCH,        "switch",       TC_KEYWORD ),
pick( T_IF,            "if",           TC_KEYWORD ),
pick( T_ENUM,          "enum",         TC_KEYWORD ),
pick( T_VOLATILE,      "volatile",     TC_QUALIFIER ),
pick( T_WHILE,         "while",        TC_KEYWORD ),
pick( T_RETURN,        "return",       TC_KEYWORD ),
pick( T_GOTO,          "goto",         TC_KEYWORD ),
pick( T_BREAK,         "break",        TC_KEYWORD ),
pick( T___HUGE,        "__huge",       TC_HUGE ),
pick( T_VOID,          "void",         TC_KEYWORD ),
pick( T_ELSE,          "else",         TC_KEYWORD ),
pick( T___SEGNAME,     "__segname",    TC_SEGNAME ),
pick( T___EXPORT,      "__export",     TC_EXPORT ),
pick( T___SEGMENT,     "__segment",    TC_SEGMENT ),
pick( T___LOADDS,      "__loadds",     TC_LOADDS ),
pick( T___INTERRUPT,   "__interrupt",  TC_INTERRUPT ),
pick( T___SAVEREGS,    "__saveregs",   TC_SAVEREGS ),
pick( T_SHORT,         "short",        TC_KEYWORD ),
pick( T_STRUCT,        "struct",       TC_KEYWORD ),
pick( T___BASED,       "__based",      TC_BASED ),
pick( T___FAR,         "__far",        TC_FAR ),
pick( T___NEAR,        "__near",       TC_NEAR ),
pick( T_DOUBLE,        "double",       TC_KEYWORD ),
pick( T___SELF,        "__self",       TC_SELF ),
pick( T_DO,            "do",           TC_KEYWORD ),
pick( T_DEFAULT,       "default",      TC_KEYWORD ),
pick( T_SIGNED,        "signed",       TC_KEYWORD ),
pick( T_UNSIGNED,      "unsigned",     TC_KEYWORD ),
pick( T_FLOAT,         "float",        TC_KEYWORD ),
pick( T_CASE,          "case",         TC_KEYWORD ),
pick( T_TYPEDEF,       "typedef",      TC_STG_CLASS ),
pick( T_SIZEOF,        "sizeof",       TC_KEYWORD ),
pick( T_CONST,         "const",        TC_QUALIFIER ),
pick( T_CONTINUE,      "continue",     TC_KEYWORD ),
pick( T_LONG,          "long",         TC_KEYWORD ),
pick( T_STATIC,        "static",       TC_STG_CLASS ),
pick( T_INT,           "int",          TC_KEYWORD ),
pick( T_FOR,           "for",          TC_KEYWORD ),
pick( T___FORTRAN,     "__fortran",    TC_FORTRAN ),
pick( T_EXTERN,        "extern",       TC_STG_CLASS ),
pick( T_REGISTER,      "register",     TC_STG_CLASS ),
pick( T_UNION,         "union",        TC_KEYWORD ),
pick( T_CHAR,          "char",         TC_KEYWORD ),
