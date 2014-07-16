#include "fail.h"

unsigned ctors;

struct X {
    X(int v) : a(v) { ++ctors; }
    X(X const &s) : a(s.a) { ++ctors; }
    ~X() { --ctors; }
    int a;
    int x() { return a; }
};

X func()
{
    return X(3);
}


int main()
{
    for(int i = 0; i < func().x(); i++) {
	// check handles both scope and expr lifetime temps
	if( ctors > 1 ) fail(__LINE__);
    }
    if( ctors != 0 ) fail(__LINE__);
    _PASS;
}
