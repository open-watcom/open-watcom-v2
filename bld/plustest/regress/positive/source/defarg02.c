#include "fail.h"

#define START_VAL (12)

// first test
static int val = START_VAL;
struct Q {
    int q;
    Q() { q = val;  val++; };
    void qr() const;
};

struct C : Q {
    int c;
    C(int in) { c = in; };
    C( Q const & q_in ) : Q(q_in) {};
    void xyz() const;
};

void func( C const & cr = C(Q()) );

void func( C const & cr )
{
    cr.xyz();
}

void C::xyz() const
{
    qr();
}

void Q::qr() const
{
    if( q != START_VAL ) fail(__LINE__);
}

// second test
static int x_val = START_VAL;
struct X : _CD {
    int x;
    X() { x = x_val; x_val++; };
};

struct Y : _CD {
    int y;
    Y() { y = x_val; x_val++; };
};


struct Z : X, Y {
    Z( X a, Y b ) : X(a), Y(b) {};
};

void temp( Z const & r = Z( X(), Y() ) ) {
    int num = r.x + r.y;
    if( num != START_VAL + START_VAL + 1 ) fail(__LINE__);
};

//third test
struct T {
    T(int i) { new T(); }
    T() { };
    void * operator new [] (size_t s);
};

foo( T *t = new T(1) ) {}

int main()
{
    func();
    temp();
    foo();
    _PASS;
}
