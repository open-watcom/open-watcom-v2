#include "fail.h"

unsigned count;

struct D {
    unsigned sig;
    D(unsigned l) : sig(l) { if( l != ++count ) fail(__LINE__); }
    ~D(){ if( sig != count-- ) fail(__LINE__); }
};

struct C1 {
    static D m1;
};

struct C2 {
    static D m2;
};

D o1(1);
D o2(2);
D C1::m1(3);
D o3(4);
D C2::m2(5);
D o4(6);

int main()
{
    D o5(7);
    if( count != 7 ) fail(__LINE__);
    D o6(8);
    if( count != 8 ) fail(__LINE__);
    _PASS;
}
