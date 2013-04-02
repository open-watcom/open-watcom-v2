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
* Description:  SSL structures and enums.
*
****************************************************************************/


#include "sslops.h"

#include "pushpck1.h"

#define MAX_TOKEN_LEN   256

typedef enum {
        /* single character delimiters */
        T_SEMI,
        T_COLON,
        T_QUESTION,
        T_DOT,
        T_POUND,
        T_LEFT_BRACE,
        T_RITE_BRACE,
        T_LEFT_BRACKET,
        T_RITE_BRACKET,
        T_LEFT_PAREN,
        T_RITE_PAREN,
        T_GT,
        T_OR,
        T_STAR,
        T_AT,
        T_COMMA,
        T_EQUALS,
        /* two character delimiters */
        T_GT_GT,
        /* keywords */
        T_INPUT,
        T_OUTPUT,
        T_ERROR,
        T_TYPE,
        T_MECH,
        T_RULES,
        /* other things */
        T_LITERAL,
        T_NAME,
        T_BAD_CHAR,
        T_EOF,
} token;

typedef enum { CLASS_INPUT,
               CLASS_OUTPUT,
               CLASS_INOUT,
               CLASS_ERROR,
               CLASS_TYPE,
               CLASS_SEM,
               CLASS_RULE,
               CLASS_ENUMS,
               CLASS_ANY,
} class;


#define NO_LOCATION ((unsigned short)-1)

typedef struct instruction {
        struct instruction      *flink, *blink;
        unsigned short          location;
        unsigned short          operand;
        void                    *ptr;
        op_code                 ins;
} instruction;

typedef struct choice_entry {
        struct choice_entry     *link;
        unsigned short          value;
        instruction             *lbl;
} choice_entry;

typedef union {
        unsigned        token;          /* for tokens */
        struct {                        /* for semantic actions */
            unsigned short value;
            struct symbol  *ret;
            struct symbol  *parm;
        }               sem;
        struct {                        /* for rules */
            instruction *lbl;
            struct symbol    *ret;
            unsigned short   exported        : 1;
            unsigned short   defined         : 1;
        }               rule;
        struct {                        /* for type values */
            struct symbol   *type;
            unsigned short  value;
        }               enums;
} values;

typedef struct symbol {
        struct  symbol  *link;
        char            *name;
        char            *alias;
        values          v;
        class           typ;
} symbol;

#include "poppck.h"
