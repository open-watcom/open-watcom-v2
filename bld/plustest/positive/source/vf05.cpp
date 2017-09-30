#include "fail.h"

enum {
    V_F,
    V_G,
    V_H,
    S_F,
    S_G,
    S_H,
    CALLED_COUNT
};

unsigned called[CALLED_COUNT];

struct V {
    virtual void f() { ++called[V_F]; }
    virtual void g() { ++called[V_G]; }
    virtual void h() { ++called[V_H]; }
    V() {
	h();
	h();
    }
    ~V() {
	g();
    }
};

struct S : virtual V {
    virtual void f() { ++called[S_F]; }
    virtual void g() { ++called[S_G]; }
    virtual void h() { ++called[S_H]; }
    S() {
	h();
	h();
    }
    ~S() {
	g();
    }
};

int main()
{
    {
	S x;
    }
    if( called[V_F] != 0 ) fail(__LINE__);
    if( called[V_G] != 1 ) fail(__LINE__);
    if( called[V_H] != 2 ) fail(__LINE__);
    if( called[S_F] != 0 ) fail(__LINE__);
    if( called[S_G] != 1 ) fail(__LINE__);
    if( called[S_H] != 2 ) fail(__LINE__);
    _PASS;
}
