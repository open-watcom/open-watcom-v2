#include "fail.h"

int ctor;

struct S {
    S() { ++ctor; }
    ~S() { fail(__LINE__); }
};

void main()
{
    S x;	// dtor should not be called!
    if( ctor != 1 ) fail(__LINE__);
    _PASS_EXIT;
}
