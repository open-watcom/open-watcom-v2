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


typedef enum {
    T_NULL,
    T_UNKNOWN,
    T_EOF,
    T_PERCENT,      // %
    T_VARIABLE,
    T_STRING,
    T_CONSTANT,
    T_PLUS,         // +
    T_MINUS,        // -
    T_TIMES,        // *
    T_DIV,          // /
    T_LEFT_PAREN,   // (
    T_RIGHT_PAREN,  // )
    T_EQ,           // ==
    T_NE,           // !=
    T_GE,           // <=
    T_LE,           // >=
    T_LT,           // <
    T_GT,           // >
    T_EXCLAMATION,  // !
    T_OR,           // |
    T_OR_OR,        // ||
    T_AND,          // &
    T_AND_AND,      // &&
    T_RSHIFT,       // >>
    T_LSHIFT,       // <<
    T_XOR,          // ^
    T_TILDE,        // ~
    T_COLON,        // :
    T_QUESTION,     // ?
    T_EXPONENT      // **
} token;

#define TBUFF_SIZE      256

extern long GetConstExpr( void );
void        StartExprParse( const char *data, jmp_buf abort_addr );
