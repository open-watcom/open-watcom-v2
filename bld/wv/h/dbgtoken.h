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


typedef enum
 {
        /* one character command line delimiters */
    T_LT,
    T_GT,
    T_MUL,
    T_DIV,
    T_LEFT_PAREN,
    T_RIGHT_PAREN,
    T_COMMA,
    T_LEFT_BRACE,
    T_RIGHT_BRACE,
    T_EXCLAMATION,
    T_QUESTION,
    T_CMD_SEPARATOR,
    T_LEFT_BRACKET,
    T_RIGHT_BRACKET,
    T_TILDE,
    T_SHARP,
    T_LINE_SEPARATOR,
        /* numbers & names for expressions and command lines */
    T_INT_NUM,
    T_REAL_NUM,
    T_BAD_NUM,
    T_NAME,
    T_STRING,
    T_UNKNOWN,
 } tokens;

#define FIRST_CMDLN_DELIM  T_LT
#define LAST_CMDLN_DELIM   T_UNKNOWN

typedef struct {
    char *delims;
    char *keywords;
} token_table;
