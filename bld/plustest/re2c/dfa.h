#ifndef _dfa_h
#define _dfa_h

/* $Log:        DFA.H $
Revision 1.1  92/08/20  15:50:30  Anthony_Scian
.

 * Revision 1.1  1992/08/20  17:14:07  peter
 * Initial revision
 *
 */

#include <iostream.h>
#include "re.h"

extern void prtCh(ostream&, uchar);

class DFA;
class State;

class Action {
public:
    State               *state;
public:
    Action(State*);
    virtual void emit(ostream&) = 0;
};

class Match: public Action {
public:
    Match(State*);
    void emit(ostream&);
};

class Enter: public Action {
public:
    SubString   &name;
public:
    Enter(State*, SubString&);
    void emit(ostream&);
};

class Save: public Match {
public:
    uint                selector;
public:
    Save(State*, uint);
    void emit(ostream&);
};

class Move: public Action {
public:
    Move(State*);
    void emit(ostream&);
};

class Accept: public Action {
public:
    uint                nRules;
    uint                *saves;
    State               **rules;
public:
    Accept(State*, uint, uint*, State**);
    void emit(ostream&);
};

class Rule: public Action {
public:
    RuleOp              *rule;
public:
    Rule(State*, RuleOp*);
    void emit(ostream&);
};

class Span {
public:
    uint                ub;
    State               *to;
public:
    uint show(ostream&, uint);
};

struct Go {
    uint                nSpans;
    Span                *span;
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
    Go                  go;
    Action              *action;
public:
    State();
    ~State();
    void emit(ostream&);
    void genGoto(ostream&);
    void genLinear(ostream&);
    void genBinary(ostream&);
    void genSwitch(ostream&);
    friend ostream& operator<<(ostream&, const State&);
    friend ostream& operator<<(ostream&, const State*);
};

class DFA {
public:
    uint                lbChar;
    uint                ubChar;
    uint                nStates;
    State               *head, **tail;
    State               *toDo;
public:
    DFA(Ins*, uint, uint, uint, Char*);
    ~DFA();
    void addState(State**, State*);
    State *findState(Ins**, uint);
    void split(State*);

    void findSCCs();
    uint emit(SubString&, uint, ostream&);

    friend ostream& operator<<(ostream&, const DFA&);
    friend ostream& operator<<(ostream&, const DFA*);
};

inline Action::Action(State *s) : state(s) {
    s->action = this;
}

inline Match::Match(State *s) : Action(s)
    { }

inline Enter::Enter(State *s, SubString &n) : Action(s), name(n)
    { }

inline Save::Save(State *s, uint i) : Match(s), selector(i)
    { }

inline ostream& operator<<(ostream &o, const State *s)
    { return o << *s; }

inline ostream& operator<<(ostream &o, const DFA *dfa)
    { return o << *dfa; }

#endif
