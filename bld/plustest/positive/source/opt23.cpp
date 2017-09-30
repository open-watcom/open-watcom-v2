#include "fail.h"

struct S {
    S *next;
    S *prev;
    unsigned v;
};

struct I {
    I *n;
    S *c;
};

struct C {
    S *list;
    unsigned count;
};

void I_ctor( I *t, S *c ) {
    t->c = c;
}

inline I *prev( I *t ) {
    t->c = t->c->prev;
    return t;
}

void erase( I i ) {
    S *p = i.c;
    S *e = p->next;
    S *n = e->next;
    n->prev = p;
    p->next = n;
    e->next = (S*) -3;
    e->prev = (S*) -5;
    e->v = -7;
}

I end( C *t ) {
    I d;

    I_ctor( &d, t->list->prev );
    return d;
}

void pop( C *t ) {
    I e = end( t );
    I w = *(prev(&e));
    erase( w );
}

unsigned a[] = {
    1, 2, 0, -1
};

int main() {
    S e1;
    S e2;
    S e3;
    S e4;
    C x;
    e1.next = &e2;
    e2.next = &e3;
    e3.next = &e4;
    e4.next = &e1;
    e1.prev = &e4;
    e2.prev = &e1;
    e3.prev = &e2;
    e4.prev = &e3;
    e1.v = 0;
    e2.v = 1;
    e3.v = 2;
    e4.v = 3;
    x.count = 3;
    x.list = &e1;
    pop( &x );
    unsigned *check = a;
    for( S *s = x.list, *c = s->next; ; c = c->next ) {
	if( c->next->prev != c ) fail(__LINE__);
	if( c->prev->next != c ) fail(__LINE__);
	if( c->v != *check++ ) fail(__LINE__);
	if( c == s ) break;
    }
    if( *check != -1 ) fail(__LINE__);
    _PASS;
}
