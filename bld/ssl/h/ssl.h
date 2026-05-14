/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2026      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  SSL structures and enums.
*
****************************************************************************/


#include <stdio.h>
#include "bool.h"
#include "sslops.h"
#include "ppmem.h"


#define MAX_TOKEN_LEN   256

#define KEYWORDS_DEFS \
    KEYWORDS_DEF( "input",     T_INPUT,  CLASS_INPUT ) \
    KEYWORDS_DEF( "output",    T_OUTPUT, CLASS_OUTPUT ) \
    KEYWORDS_DEF( "error",     T_ERROR,  CLASS_ERROR ) \
    KEYWORDS_DEF( "type",      T_TYPE,   CLASS_TYPE ) \
    KEYWORDS_DEF( "mechanism", T_MECH,   CLASS_SEM ) \
    KEYWORDS_DEF( "rules",     T_RULES,  CLASS_RULE )

#define DELIMS_DEFS \
    DELIMS_DEF( ';', T_SEMI ) \
    DELIMS_DEF( ':', T_COLON ) \
    DELIMS_DEF( '?', T_QUESTION ) \
    DELIMS_DEF( '.', T_DOT ) \
    DELIMS_DEF( '#', T_POUND ) \
    DELIMS_DEF( '{', T_LEFT_BRACE ) \
    DELIMS_DEF( '}', T_RITE_BRACE ) \
    DELIMS_DEF( '[', T_LEFT_BRACKET ) \
    DELIMS_DEF( ']', T_RITE_BRACKET ) \
    DELIMS_DEF( '(', T_LEFT_PAREN ) \
    DELIMS_DEF( ')', T_RITE_PAREN ) \
    DELIMS_DEF( '>', T_GT ) \
    DELIMS_DEF( '|', T_OR ) \
    DELIMS_DEF( '*', T_STAR ) \
    DELIMS_DEF( '@', T_AT ) \
    DELIMS_DEF( ',', T_COMMA ) \
    DELIMS_DEF( '=', T_EQUALS )

#define NO_LOCATION ((unsigned)-1)

typedef enum {
    /* single character delimiters */
    #define DELIMS_DEF(a,b) b,
        DELIMS_DEFS
    #undef DELIMS_DEF
    /* two character delimiters */
    T_GT_GT,
    /* keywords */
    #define KEYWORDS_DEF(a,b,c) b,
        KEYWORDS_DEFS
    #undef KEYWORDS_DEF
    /* other things */
    T_LITERAL,
    T_NAME,
    T_BAD_CHAR,
    T_EOF,
} ssl_token;

typedef enum {
    #define KEYWORDS_DEF(a,b,c) c,
        KEYWORDS_DEFS
    #undef KEYWORDS_DEF
    CLASS_INOUT,
    CLASS_ENUMS,
    CLASS_ANY,
} ssl_class;

typedef struct instruction {
    struct instruction      *flink;
    struct instruction      *blink;
    union {
        struct instruction  *lbl;
        struct choice_entry *choice;
    } u;
    union {
        int                 operand;
        int                 reference;
    } u1;
    unsigned                location;
    op_code                 opcode;
    bool                    is_long;
} instruction;

typedef struct choice_entry {
    struct choice_entry     *link;
    instruction             *lbl;
    int                     value;
} choice_entry;

typedef union {
    int             token;          /* for tokens */
    struct {                        /* for semantic actions */
        struct symbol   *ret;
        struct symbol   *parm;
        int             value;
    }               sem;
    struct {                        /* for rules */
        instruction     *lbl;
        struct symbol   *ret;
        unsigned        exported    : 1;
        unsigned        defined     : 1;
    }               rule;
    struct {                        /* for type values */
        struct symbol   *type;
        int             value;
    }               enums;
} values;

typedef struct symbol {
    struct symbol   *link;
    char            *name;
    char            *alias;
    values          v;
    ssl_class       typ;
} symbol;

extern ssl_token       CurrToken;
extern char            TokenBuff[MAX_TOKEN_LEN];
extern unsigned        TokenLen;
extern FILE            *PrsFile;
