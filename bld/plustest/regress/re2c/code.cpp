/* $Log:        CODE.C $
Revision 1.1  92/08/20  15:50:28  Anthony_Scian
.

// Revision 1.1  1992/08/20  17:14:07  peter
// Initial revision
//
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "useful.h"
#include "globals.h"
#include "dfa.h"

void genGoTo(ostream &o, State *to){
    o  << "\tgoto L" << to->label << ";\n";
}

void genIf(ostream &o, char *cmp, uint v){
    o << "\tif((CURSOR) " << cmp << " '";
    prtCh(o, v);
    o << "')";
}

void indent(ostream &o, uint i){
    while(i-- > 0)
        o << "\t";
}

void Match::emit(ostream &o){
    o << "\t{ADVANCE}\n";
    if(state->link)
        o << "\t{CHECK(" << state->depth << ")}\n";
}

void Enter::emit(ostream &o){
    o << "\t{ADVANCE}\n";
    o << (SubString) name << ":\n";
    if(state->link)
        o << "\t{CHECK(" << state->depth << ")}\n";
}

void Save::emit(ostream &o){
    o << "\t{ADVANCE}\n";
    o << "\t{MARK(" << selector << ")}\n";
    if(state->link)
        o << "\t{CHECK(" << state->depth << ")}\n";
}

Move::Move(State *s) : Action(s) {
    ;
}

void Move::emit(ostream &o){
    ;
}

Accept::Accept(State *x, uint n, uint *s, State **r)
    : Action(x), nRules(n), saves(s), rules(r){
    ;
}

void Accept::emit(ostream &o){
    bool first = true;
    for(uint i = 0; i < nRules; ++i)
        if(saves[i] != ~0){
            if(first){
                first = false;
                o << "\t{REVERT}\n";
                o << "\tswitch(MARKER){\n";
            }
            o << "\tcase " << saves[i] << ":";
            genGoTo(o, rules[i]);
        }
    if(!first)
        o << "\t}\n";
}

Rule::Rule(State *s, RuleOp *r) : Action(s), rule(r) {
    ;
}

void Rule::emit(ostream &o){
    uint back = rule->ctx->fixedLength();
    if(back != ~0 && back > 0)
        o << "\t{BACK(" << back << ")}";
    if(fileName)
        o << "\n#line " << rule->line;
    o << "\n\t" << rule->code << "\n";
}

void doLinear(ostream &o, uint i, Span *s, uint n, State *next){
    for(;;){
        State *bg = s[0].to;
        while(n >= 3 && s[2].to == bg && (s[1].ub - s[0].ub) == 1){
            if(s[1].to == next && n == 3){
                indent(o, i); genIf(o, "!=", s[0].ub); genGoTo(o, bg);
                return;
            } else {
                indent(o, i); genIf(o, "==", s[0].ub); genGoTo(o, s[1].to);
            }
            n -= 2; s += 2;
        }
        if(n == 1){
            if(bg != next){
                indent(o, i); genGoTo(o, s[0].to);
            }
            return;
        } else if(n == 2 && bg == next){
            indent(o, i); genIf(o, ">=", s[0].ub); genGoTo(o, s[1].to);
            return;
        } else {
            indent(o, i); genIf(o, "<=", s[0].ub - 1); genGoTo(o, bg);
            n -= 1; s += 1;
        }
    }
}

void State::genLinear(ostream &o){
    o << "\t{LOADCURSOR}\n";
    doLinear(o, 0, go.span, go.nSpans, next);
}

void genCases(ostream &o, uint lb, Span *s){
    for(uint i = lb; i < s->ub; ++i){
        o << "case '"; prtCh(o, i); o << "':\n";
    }
}

void State::genSwitch(ostream &o){
    if(go.nSpans <= 2){
        genLinear(o);
    } else {
        State *def = go.span[go.nSpans-1].to;
        Span **sP = new Span*[go.nSpans-1], **r, **s, **t;

        t = &sP[0];
        for(uint i = 0; i < go.nSpans; ++i)
            if(go.span[i].to != def)
                *(t++) = &go.span[i];

        o << "\t{LOADCURSOR}\n\tswitch((CURSOR)){\n";
        while(t != &sP[0]){
            r = s = &sP[0];
            o << "\t";
            if(*s == &go.span[0])
                genCases(o, 0, *s);
            else
                genCases(o, (*s)[-1].ub, *s);
            State *to = (*s)->to;
            while(++s < t){
                if((*s)->to == to)
                    genCases(o, (*s)[-1].ub, *s);
                else
                    *(r++) = *s;
            }
            o << "\n\t";
            genGoTo(o, to);
            t = r;
        }
        o << "\tdefault:\n\t";
        genGoTo(o, def);
        o << "\t}\n";

        delete sP;
    }
}

void doBinary(ostream &o, uint i, Span *s, uint n, State *next){
    if(n <= 4){
        doLinear(o, i, s, n, next);
    } else {
        uint h = n/2;
        indent(o, i); genIf(o, "<=", s[h-1].ub - 1); o << "{\n";
        doBinary(o, i+1, &s[0], h, next);
        indent(o, i); o << "\t} else {\n";
        doBinary(o, i+1, &s[h], n - h, next);
        indent(o, i); o << "\t}\n";
    }
}

void State::genBinary(ostream &o){
    o << "\t{LOADCURSOR}\n";
    doBinary(o, 0, go.span, go.nSpans, next);
}

void State::genGoto(ostream &o){
    if(go.nSpans == 0)
        return;
    if(sFlag){
        genSwitch(o);
        return;
    }
    if(go.nSpans > 8){
        Span *bot = &go.span[0], *top = &go.span[go.nSpans-1];
        uint util;
        if(bot[0].to == top[0].to){
            util = (top[-1].ub - bot[0].ub)/(go.nSpans - 2);
        } else {
            if(bot[0].ub > (top[0].ub - top[-1].ub)){
                util = (top[0].ub - bot[0].ub)/(go.nSpans - 1);
            } else {
                util = top[-1].ub/(go.nSpans - 1);
            }
        }
        if(util <= 2){
            genSwitch(o);
            return;
        }
    }
    if(go.nSpans > 5){
        genBinary(o);
    } else {
        genLinear(o);
    }
}

void State::emit(ostream &o){
    o << "L" << label << ":";
    action->emit(o);
}

uint merge(Span *x0, State *fg, State *bg){
    Span *x = x0, *f = fg->go.span, *b = bg->go.span;
    uint nf = fg->go.nSpans, nb = bg->go.nSpans;
    State *prev = NULL, *to;
    // NB: we assume both spans are for same range
    for(;;){
        if(f->ub == b->ub){
            to = f->to == b->to? bg : f->to;
            if(to == prev){
                --x;
            } else {
                x->to = prev = to;
            }
            x->ub = f->ub;
            ++x; ++f; --nf; ++b; --nb;
            if(nf == 0 && nb == 0)
                return x - x0;
        }
        while(f->ub < b->ub){
            to = f->to == b->to? bg : f->to;
            if(to == prev){
                --x;
            } else {
                x->to = prev = to;
            }
            x->ub = f->ub;
            ++x; ++f; --nf;
        }
        while(b->ub < f->ub){
            to = b->to == f->to? bg : f->to;
            if(to == prev){
                --x;
            } else {
                x->to = prev = to;
            }
            x->ub = b->ub;
            ++x; ++b; --nb;
        }
    }
}

const uint cInfinity = ~0;

class SCC {
public:
    State       **top, **stk;
public:
    SCC(uint);
    ~SCC();
    void traverse(State*);
};

SCC::SCC(uint size){
    top = stk = new State*[size];
}

SCC::~SCC(){
    delete stk;
}

void SCC::traverse(State *x){
    *top = x;
    uint k = ++top - stk;
    x->depth = k;
    for(uint i = 0; i < x->go.nSpans; ++i){
        State *y = x->go.span[i].to;
        if(y){
            if(y->depth == 0)
                traverse(y);
            if(y->depth < x->depth)
                x->depth = y->depth;
        }
    }
    if(x->depth == k)
        do {
            (*--top)->depth = cInfinity;
            (*top)->link = x;
        } while(*top != x);
}

uint maxDist(State *s){
    uint mm = 0;
    for(uint i = 0; i < s->go.nSpans; ++i){
        State *t = s->go.span[i].to;
        if(t){
            uint m = 1;
            if(!t->link)
                m += maxDist(t);
            if(m > mm)
                mm = m;
        }
    }
    return mm;
}

void calcDepth(State *head){
    State *t;
    for(State *s = head; s; s = s->next){
        if(s->link == s){
            for(uint i = 0; i < s->go.nSpans; ++i){
                t = s->go.span[i].to;
                if(t && t->link == s)
                    goto inSCC;
            }
            s->link = NULL;
        } else {
        inSCC:
            s->depth = maxDist(s);
        }
    }
}

void DFA::findSCCs(){
    SCC scc(nStates);
    State *s;

    for(s = head; s; s = s->next){
        s->depth = 0;
        s->link = NULL;
    }

    for(s = head; s; s = s->next)
        if(!s->depth)
            scc.traverse(s);

    calcDepth(head);
}

void DFA::split(State *s){
    State *move = new State;
    new Move(move);
    addState(&s->next, move);
    move->link = s->link;
    move->rule = s->rule;
    move->go = s->go;
    s->rule = NULL;
    s->go.nSpans = 1;
    s->go.span = new Span;
    s->go.span[0].ub = ubChar;
    s->go.span[0].to = move;
}

uint DFA::emit(SubString &name, uint label, ostream &o){
    State *s;
    uint i, j;

    findSCCs();
    head->link = head;
    head->depth = maxDist(head);

    uint nRules = 0;
    for(s = head; s; s = s->next)
        if(s->rule && s->rule->accept >= nRules)
                nRules = s->rule->accept + 1;

    uint nSaves = 0;
    uint *saves = new uint[nRules];
    memset(saves, ~0, (nRules)*sizeof(*saves));
    uint nMarks = 0;

    for(s = head; s; s = s->next){
        RuleOp *ignore = NULL;
        if(s->rule){
            for(i = 0; i < s->go.nSpans; ++i)
                if(s->go.span[i].to && !s->go.span[i].to->rule){
                    delete s->action;
                    if(saves[s->rule->accept] == ~0)
                        saves[s->rule->accept] = nSaves++;
                    new Save(s, saves[s->rule->accept]);
                    continue;
                }
            ignore = s->rule;
        }
    }

    State **rules = new State*[nRules];
    memset(rules, 0, (nRules)*sizeof(*rules));
    State *accept = NULL;
    for(s = head; s; s = s->next){
        State *ow;
        if(!s->rule){
            ow = accept;
        } else {
            if(!rules[s->rule->accept]){
                State *n = new State;
                new Rule(n, s->rule);
                rules[s->rule->accept] = n;
                addState(&s->next, n);
            }
            ow = rules[s->rule->accept];
        }
        for(i = 0; i < s->go.nSpans; ++i)
            if(!s->go.span[i].to){
                if(!ow){
                    ow = accept = new State;
                    new Accept(accept, nRules, saves, rules);
                    addState(&s->next, accept);
                }
                s->go.span[i].to = ow;
            }
    }

    for(s = head; s; s = s->next)
        if(s->link){
            split(s);
            s = s->next;
        }

    Span *span = new Span[ubChar - lbChar];
    for(s = head; s; s = s->next){
        if(!s->link){
            for(i = 0; i < s->go.nSpans; ++i){
                State *to = s->go.span[i].to;
                if(to && to->link && to->depth == 1){
                    to = to->go.span[0].to;
                    uint nSpans = merge(span, s, to);
                    if(nSpans < s->go.nSpans){
                        delete s->go.span;
                        s->go.nSpans = nSpans;
                        s->go.span = new Span[nSpans];
                        memcpy(s->go.span, span, nSpans*sizeof(Span));
                    }
                    break;
                }
            }
        }
    }
    delete span;

    delete head->action;
    new Enter(head, name);

    for(s = head; s; s = s->next)
        s->label = label++;

    o << "\tgoto " << name << ";\n";
    for(s = head; s; s = s->next){
        s->emit(o);
        s->genGoto(o);
    }

    delete saves;
    delete rules;

    return label;
}
