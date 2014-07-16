#include "fail.h"
#include <stdio.h>

int dtors;

struct S {
    S() {--dtors;}
    ~S(){ ++dtors; }
};

struct T {
    struct {
	S x;
	S y;
    } _x;
    struct {
	S x;
	S y;
    } _y;
};

void foo()
{
    T x,y;
}

int main()
{
    if( dtors != 0 ) fail(__LINE__);
    foo();
    if( dtors != 0 ) fail(__LINE__);
    _PASS;
}
