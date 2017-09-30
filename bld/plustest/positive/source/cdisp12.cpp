#include "fail.h"
#include <string.h>

void *operator new( unsigned, void *p )
{
    return p;
}

#pragma inline_depth(0)

struct B3;
B3 *B3_OK;
void set_B3( B3 *p )
{
    if( B3_OK ) {
	if( B3_OK != p ) fail(__LINE__);
    }
    B3_OK = p;
}

struct E9;
E9 *E9_OK;
void set_E9( E9 *p )
{
    if( E9_OK ) {
	if( E9_OK != p ) fail(__LINE__);
    }
    E9_OK = p;
}

struct A1 {
    int __u;
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
};
struct A2 {
    int __u;
    virtual void foo() {}
    virtual void bar() {}
};
struct B3 : A1, A2 {
    int __u;
    virtual void foo() { if( this != B3_OK ) fail(__LINE__); }
    B3() {
	B3 *p = this;
	set_B3( p );
	A2 *pA2 = this;
	pA2->foo();
    }
};
struct B4 {
    int __u;
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
};
struct C5 : B4, B3 {
    int __u;
    C5() {
	B3 *p = this;
	set_B3( p );
	A2 *pA2 = this;
	pA2->foo();
    }
};
struct C6 {
    int __u;
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
};
struct D7 : C6, C5 {
    int __u;
    D7() {
	B3 *p = this;
	set_B3( p );
	A2 *pA2 = this;
	pA2->foo();
    }
};

struct E8 {
    int __u;
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
};
void hook() {
    static int h;
    ++h;
}
struct E9 : virtual D7 {
    int __u;
    virtual void __u() {}
    virtual void foo() { if( this != E9_OK ) fail(__LINE__); }
    E9() {
	E9 *p = this;
	set_E9( p );
	A2 *pA2 = this;
	hook();
	pA2->foo();
	B3 *pB3 = this;
	pB3->foo();
    }
};
struct F10 {
    int __u;
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
};
struct F11 : E8, E9 {
    int __u;
    virtual void __u() {}
    F11() {
	E9 *p = this;
	set_E9( p );
	A2 *pA2 = this;
	pA2->foo();
    }
};
struct G12 {
    int __u;
    virtual void __u() {}
};
struct G13 : F10, F11 {
    G13() {
	E9 *p = this;
	set_E9( p );
	A2 *pA2 = this;
	pA2->foo();
    }
    int __u;
    virtual void __u() {}
    int __u;
    virtual void __u() {}
    virtual void __u() {}
    int __u;
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    int __u;
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
    virtual void __u() {}
};
struct H14 : virtual G13 {
    int __u;
    virtual void __u() {}
};
struct I15 : virtual H14 {
    int __u;
    virtual void __u() {}
    I15() {
	E9 *p = this;
	set_E9( p );
	A2 *pA2 = this;
	pA2->foo();
	B3 *pB3 = this;
	pB3->foo();
    }
};
struct I16 : virtual H14 {
    int __u;
    virtual void __u() {}
};
struct J17 : virtual I15, virtual I16 {
    int __u;
    virtual void __u() {}
    J17() {
	E9 *p = this;
	set_E9( p );
	A2 *pA2 = this;
	pA2->foo();
	B3 *pB3 = this;
	pB3->foo();
    }
};

int main() {
    char *p = new char[4096];
    memset( p, -1, 4096 );
    p += ((((unsigned)p)+0xff)&~0xff) - (unsigned)p;
    new (p) J17;
    _PASS;
}
