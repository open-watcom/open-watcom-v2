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


#ifndef _re_h
#define _re_h

#include <stdio.h>
#include "token.h"
#include "ins.h"

typedef struct extop {
    char    op;
    int     minsize;
    int     maxsize;
} ExtOp;

typedef struct CharPtn {
    uint            card;
    struct CharPtn  *fix;
    struct CharPtn  *nxt;
} CharPtn;

typedef struct CharSet {
    CharPtn     *fix;
    CharPtn     *freeHead;
    CharPtn     **freeTail;
    CharPtn     *rep[NCHARS];
    CharPtn     ptn[NCHARS];
} CharSet;

typedef struct Range {
    struct Range    *next;
    uint            lb, ub;     // [lb,ub)
} Range;

typedef enum {
        NULLOP = 1,
        MATCHOP,
        RULEOP,
        ALTOP,
        CATOP,
        CLOSEOP,
        CLOSEVOP
} RegExpType;

typedef struct RegExp {
    RegExpType  type;
    uint        size;
    union {
        struct {
            Range           *match;
        } MatchOp;
        struct {
            struct RegExp   *exp;
            struct RegExp   *ctx;
            Ins             *ins;
            uint            accept;
            Token           *code;
            uint            line;
        } RuleOp;
        struct {
            struct RegExp   *exp1, *exp2;
        } AltOp;
        struct {
            struct RegExp   *exp1, *exp2;
        } CatOp;
        struct {
            struct RegExp   *exp;
        } CloseOp;
        struct {
            struct RegExp   *exp;
            int             min;
            int             max;
        } CloseVOp;
    } u;
} RegExp;

extern uint     RegExp_fixedLength( RegExp * );
extern RegExp   *RegExp_new_NullOp( void );
extern RegExp   *RegExp_new_CatOp( RegExp *e1, RegExp *e2 );
extern RegExp   *RegExp_new_RuleOp( RegExp *, RegExp *, Token *, uint );
extern RegExp   *RegExp_new_CloseOp( RegExp *e );
extern void     genCode( FILE *, RegExp * );
extern RegExp   *mkAlt( RegExp *, RegExp * );
extern RegExp   *mkDiff( RegExp *, RegExp * );
extern RegExp   *ranToRE( SubStr );
extern RegExp   *strToRE( SubStr );

#endif
