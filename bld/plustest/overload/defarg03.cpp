#include "dump.h"

// order shouldn't matter!
// 1 2
struct B {
    void operator() (int i1, int i2); // (1)
    // add (int,int)
    void operator() (int i1, int i2, int i3 = 6); // (2)
    // add (int,int,int) and (int, int, [int] )
};
void B::operator() (int i1, int i2) GOOD;
void (B::*pmfb)(int, int) = &B::operator();

// 2 1
struct C {
    void operator() (int i1, int i2, int i3 = 6); // (2)
    void operator() (int i1, int i2); // (1) 
};
void C::operator() (int i1, int i2) GOOD;
void (C::*pmfc)(int, int) = &C::operator();


int main()
{
    B b;
    C c;

    (b.*pmfb)(3,4);
    (c.*pmfc)(3,4);
    CHECK_GOOD(30);
    return errors != 0;
}

