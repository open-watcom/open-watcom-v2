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


#ifndef _dfa_h
#define _dfa_h

#include <iostream>
#include "re.h"

extern void prtCh(std::ostream&, uchar);
extern void printSpan(std::ostream&, uint, uint);

class DFA;
class State;

class Action {
public:
    State               *state;
public:
    Action(State*);
    virtual void emit(std::ostream&) = 0;
};

class Match: public Action {
public:
    Match(State*);
    void emit(std::ostream&);
};

class Enter: public Action {
public:
    uint                label;
public:
    Enter(State*, uint);
    void emit(std::ostream&);
};

class Save: public Match {
public:
    uint                selector;
public:
    Save(State*, uint);
    void emit(std::ostream&);
};

class Move: public Action {
public:
    Move(State*);
    void emit(std::ostream&);
};

class Accept: public Action {
public:
    uint                nRules;
    uint                *saves;
    State               **rules;
public:
    Accept(State*, uint, uint*, State**);
    void emit(std::ostream&);
};

class Rule: public Action {
public:
    RuleOp              *rule;
public:
    Rule(State*, RuleOp*);
    void emit(std::ostream&);
};

class Span {
public:
    uint                ub;
    State               *to;
public:
    uint show(std::ostream&, uint);
};

class Go {
public:
    uint                nSpans;
    Span                *span;
public:
    void genGoto(std::ostream&, State*);
    void genBase(std::ostream&, State*);
    void genLinear(std::ostream&, State*);
    void genBinary(std::ostream&, State*);
    void genSwitch(std::ostream&, State*);
    void compact();
    void unmap(Go*, State*);
};

class State {
public:
    uint                label;
    RuleOp              *rule;
    State               *next;
    State               *link;
    uint                depth;          // for finding SCCs
    uint                kCount;
    Ins                 **kernel;
    bool                isBase:1;
    Go                  go;
    Action              *action;
public:
    State();
    ~State();
    void emit(std::ostream&);
    friend std::ostream& operator<<(std::ostream&, const State&);
    friend std::ostream& operator<<(std::ostream&, const State*);
};

class DFA {
public:
    uint                lbChar;
    uint                ubChar;
    uint                nStates;
    State               *head;
    State               **tail;
    State               *toDo;
public:
    DFA(Ins*, uint, uint, uint, Char*);
    ~DFA();
    void addState(State**, State*);
    State *findState(Ins**, uint);
    void split(State*);

    void findSCCs();
    void emit(std::ostream&);

    friend std::ostream& operator<<(std::ostream&, const DFA&);
    friend std::ostream& operator<<(std::ostream&, const DFA*);
};

inline Action::Action(State *s) : state(s) {
    s->action = this;
}

inline Match::Match(State *s) : Action(s)
    { }

inline Enter::Enter(State *s, uint l) : Action(s), label(l)
    { }

inline Save::Save(State *s, uint i) : Match(s), selector(i)
    { }

inline std::ostream& operator<<(std::ostream &o, const State *s)
    { return o << *s; }

inline std::ostream& operator<<(std::ostream &o, const DFA *dfa)
    { return o << *dfa; }

#endif
