#include "fail.h"

unsigned count;

void operator delete( void *p ) {
    free( p );
    ++count;
}

struct B { int i; virtual ~B() { } };
struct C { int j; virtual ~C() { } };
struct D : B, C { int k; virtual ~D() { } };

int main() {
    count = 0;
    B *pb = new D;
    C *pc = new D;
    delete pb;
    delete pc;
    if( count != 2 ) fail(__LINE__);
    _PASS;
}
