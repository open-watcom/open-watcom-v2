#include "fail.h"

unsigned ctors;
struct S {
    S() { ++ctors; }
};

static S a[1000];

int main()
{
    if( ctors != 1000 ) fail(__LINE__);
    _PASS;
}
