#include "fail.h"
// test for proper call graph support on boundary

#pragma inline_depth(3)

int ctored;

struct S {
    S() { ++ ctored; }
    S( S const & ) { ++ ctored; }
    ~S(){ -- ctored; }
};

inline
void inln_0()
{
    S sv;
}

inline
void inln_1()
{
    inln_0();
}

inline
void inln_2()
{
    inln_1();
}

int main()
{
    inln_2();
    if( ctored != 0 ) fail(__LINE__);
    _PASS;
}
