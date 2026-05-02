/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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


#ifndef _dfa_h
#define _dfa_h

#include <stdio.h>
#include "re.h"

struct DFA;
struct State;

typedef enum {
    MATCHACT = 1,
    ENTERACT,
    SAVEMATCHACT,
    MOVEACT,
    ACCEPTACT,
    RULEACT
} ActionType;

typedef struct Action {
    struct State    *state;
    ActionType      type;
    union {
        struct {
            uint    label;
        } Enter;
        struct {
            uint    selector;
        } SaveMatch;
        struct {
            uint    nRules;
            uint    *saves;
            struct State    **rules;
        } Accept;
        struct {
            RegExp  *rule;  /* RuleOp */
        } Rule;
    } u;
} Action;

typedef struct Span {
    Char            ub;
    struct State    *to;
} Span;

typedef struct Go {
    uint            nSpans;
    Span            *span;
} Go;

typedef struct State {
    uint            label;
    RegExp          *rule;  /* RuleOp */
    struct State    *next;
    struct State    *link;
    uint            depth;          /* for finding SCCs */
    uint            kCount;
    Ins             **kernel;
    uint            isBase:1;
    uint            referenced:1;
    Go              go;
    Action          *action;
} State;

typedef struct DFA {
    Char            lbChar;
    Char            ubChar;
    uint            nStates;
    State           *head;
    State           **tail;
    State           *toDo;
} DFA;

extern DFA      *DFA_new( Ins *, uint, Char, Char, Char *, uint );
extern void     DFA_delete( DFA *d );
extern void     DFA_addState( DFA *d, State **st_before, State *st );
extern void     DFA_emit( DFA *d, FILE *o );
extern State    *State_new( void );

extern Action   *Action_new_Match( State *st );
extern Action   *Action_new_Enter( State *st );
extern Action   *Action_new_Save( State *st, uint i );
extern Action   *Action_new_Move( State *st );
extern Action   *Action_new_Rule( State *st, RegExp *r );
extern Action   *Action_new_Accept( State *st, uint n, uint *sv, State **rules );
extern void     Action_delete( State *st );

#endif
