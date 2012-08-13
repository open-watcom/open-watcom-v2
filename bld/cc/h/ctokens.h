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


#include "weights.gh"

/* the following table is in order of priority of operators */

typedef enum token_class {
        TC_MODIFIER = 1,
        TC_BASED,
        TC_SEGMENT,
        TC_SEGNAME,
        TC_SELF,
        TC_KEYWORD,             /* C keyword */
        TC_QUALIFIER,           /* const, volatile */
        TC_STG_CLASS,           /* auto, register, static, extern, typedef */
        TC_SEG16,
        TC_DECLSPEC,
        TC_XX,
        TC_RIGHT_BRACKET,       /* ] */
        TC_INDEX,               /* [ */
        TC_RIGHT_PAREN,         /* ) */
        TC_PARM_LIST,           /* ( of func call */
        TC_LEFT_PAREN,          /* ( */
        TC_START,               /* start of expression */
        TC_TERNARY,             /* ?: */
        TC_COLON,               /* :  */
        TC_COMMA,               /* , */
        TC_START1,              /* expressions without commas */
        TC_ASSIGNMENT,          /* assignment operation */
        TC_ASSIGN_OP,           /* assignment operator */
        TC_TERNARY_DONE,        /* ?: done */
        TC_START2,              /* expressions without assignments */
        TC_QUESTION,            /* ?  */
        TC_OR_OR,               /* || */
        TC_AND_AND,             /* && */
        TC_OR,                  /* |  */
        TC_XOR,                 /* ^  */
        TC_AND,                 /* &  */
        TC_EQ_NE,               /* == or != */
        TC_REL_OP,              /* relational operator */
        TC_SHIFT_OP,            /* << or >> */
        TC_ADD_OP,              /* + or - */
        TC_MUL_OP,              /* *, /, % */

/* following operators are unary and all have the same precedence */

        TC_START_UNARY,         /* start of unary expression */
        TC_PREINC,              /* ++, -- */
        TC_ADDR,                /* & */
        TC_EXCLAMATION,         /* ! */
        TC_PLUS,                /* + */
        TC_MINUS,               /* - */
        TC_TILDE,               /* ~ */
        TC_SIZEOF,              /* sizeof */
        TC_INDIRECTION,         /* * */
        TC_CAST,                /* (type) */

        TC_SAVED_PLIST,         /* saved parameter list info */
        TC_POSTINC,             /* ++, -- */
        TC_LEFT_BRACKET,        /* [ */
        TC_SEG_OP,              /* :> */
        TC_FUNC_CALL,           /* ( */
        TC_DOT,                 /* . */
        TC_ARROW                /* -> */
} token_class;

typedef enum TOKEN {
    #define pick(token,string,class) token,
    #include "_ctokens.h"
    #undef pick
} TOKEN;

extern  char        *Tokens[];
extern  token_class TokenClass[];

