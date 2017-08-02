// CTOR21 -- a tricky conversion example
#include "fail.h"

class X {
public:
  operator int() { return x; }
  int x;
  X( int x ) : x(x) {}
};

class Y {
public:
  int y;
  operator X();
  Y( int y ) :y(y) {}
};

Y::operator X()
{
    return X(y);
}

Y a(463);
int c = X(a);   // ok: a.operator X().operator int()


int main() {
    if( c != 463 ) fail(__LINE__);
    _PASS;
}
