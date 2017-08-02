#include "dump.h"

// confirm that a UDCF will be taken in the class to class case

struct C {
    int c;
    C(int int_in) { c = int_in; };
};
struct B {
    int b;
    operator C() { GOOD; return C(b); } ;
};

void f(C) GOOD
B b;

int main()
{
    f(b);
    CHECK_GOOD( 11+14 );
    return errors != 0;
}
