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

#include <iostream>
#include "token.h"
#include "ins.h"

struct CharPtn {
    uint        card;
    CharPtn     *fix;
    CharPtn     *nxt;
};

struct CharSet {
    CharPtn     *fix;
    CharPtn     *freeHead;
    CharPtn     **freeTail;
    CharPtn     *rep[nChars];
    CharPtn     ptn[nChars];
};

class Range {
public:
    Range       *next;
    uint        lb, ub;         // [lb,ub)
public:
    Range(uint l, uint u) : next(NULL), lb(l), ub(u)
        { }
    Range(Range &r) : next(NULL), lb(r.lb), ub(r.ub)
        { }
    friend std::ostream& operator<<(std::ostream&, const Range&);
    friend std::ostream& operator<<(std::ostream&, const Range*);
};

inline std::ostream& operator<<(std::ostream &o, const Range *r){
        return r? o << *r : o;
}

class RegExp {
public:
    uint        size;
public:
    virtual char *typeOf() = 0;
    RegExp *isA(char *t)
        { return typeOf() == t? this : NULL; }
    virtual void split(CharSet&) = 0;
    virtual void calcSize(Char*) = 0;
    virtual uint fixedLength();
    virtual void compile(Char*, Ins*) = 0;
    virtual void display(std::ostream&) const = 0;
    friend std::ostream& operator<<(std::ostream&, const RegExp&);
    friend std::ostream& operator<<(std::ostream&, const RegExp*);
};

inline std::ostream& operator<<(std::ostream &o, const RegExp &re){
    re.display(o);
    return o;
}

inline std::ostream& operator<<(std::ostream &o, const RegExp *re){
    return o << *re;
}

class NullOp: public RegExp {
public:
    static char *type;
public:
    char *typeOf()
        { return type; }
    void split(CharSet&);
    void calcSize(Char*);
    uint fixedLength();
    void compile(Char*, Ins*);
    void display(std::ostream &o) const {
        o << "_";
    }
};

class MatchOp: public RegExp {
public:
    static char *type;
    Range       *match;
public:
    MatchOp(Range *m) : match(m)
        { }
    char *typeOf()
        { return type; }
    void split(CharSet&);
    void calcSize(Char*);
    uint fixedLength();
    void compile(Char*, Ins*);
    void display(std::ostream&) const;
};

class RuleOp: public RegExp {
private:
    RegExp      *exp;
public:
    RegExp      *ctx;
    static char *type;
    Ins         *ins;
    uint        accept;
    Token       *code;
    uint        line;
public:
    RuleOp(RegExp*, RegExp*, Token*, uint);
    char *typeOf()
        { return type; }
    void split(CharSet&);
    void calcSize(Char*);
    void compile(Char*, Ins*);
    void display(std::ostream &o) const {
        o << exp << "/" << ctx << ";";
    }
};


RegExp *mkAlt(RegExp*, RegExp*);

class AltOp: public RegExp {
private:
    RegExp      *exp1, *exp2;
public:
    static char *type;
public:
    AltOp(RegExp *e1, RegExp *e2)
        { exp1 = e1;  exp2 = e2; }
    char *typeOf()
        { return type; }
    void split(CharSet&);
    void calcSize(Char*);
    uint fixedLength();
    void compile(Char*, Ins*);
    void display(std::ostream &o) const {
        o << exp1 << "|" << exp2;
    }
    friend RegExp *mkAlt(RegExp*, RegExp*);
};

class CatOp: public RegExp {
private:
    RegExp      *exp1, *exp2;
public:
    static char *type;
public:
    CatOp(RegExp *e1, RegExp *e2)
        { exp1 = e1;  exp2 = e2; }
    char *typeOf()
        { return type; }
    void split(CharSet&);
    void calcSize(Char*);
    uint fixedLength();
    void compile(Char*, Ins*);
    void display(std::ostream &o) const {
        o << exp1 << exp2;
    }
};

class CloseOp: public RegExp {
private:
    RegExp      *exp;
public:
    static char *type;
public:
    CloseOp(RegExp *e)
        { exp = e; }
    char *typeOf()
        { return type; }
    void split(CharSet&);
    void calcSize(Char*);
    void compile(Char*, Ins*);
    void display(std::ostream &o) const {
        o << exp << "+";
    }
};

extern void genCode(std::ostream&, RegExp*);
extern RegExp *mkDiff(RegExp*, RegExp*);
extern RegExp *strToRE(SubStr);
extern RegExp *ranToRE(SubStr);

#endif
