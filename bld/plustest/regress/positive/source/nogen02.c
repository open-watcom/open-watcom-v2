#include "fail.h"
#include <stdio.h>


class A {
    void operator=(const A &);
public:
    int a;
    friend void verify_x( int );
};

struct B : A {
    void operator=(int i) { a = i; b = i; }
    friend void verify_x( int );
private:
    int b;
};

B x;
int i;

void set_x( int v )
{
    x = v;
    i += v;
}

void verify_x( int v )
{
    if( x.a != v || x.b != v ) fail(__LINE__);
}

int main()
{
    i = 1;
    set_x( 6 );
    verify_x( 6 );
    x = i;
    verify_x( 7 );
    set_x( 28 );
    verify_x( 28 );
    x = i;
    verify_x( 35 );
    _PASS;
}
