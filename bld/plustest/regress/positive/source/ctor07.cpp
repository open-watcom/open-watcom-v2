#include "fail.h"

unsigned dtors;
unsigned ctors;

struct S {
    static S *a[100];
    static int n;
    S(int x = 0) {
	a[n++] = this;
	++ctors;
    }
    ~S() {
	++dtors;
	if( a[--n] != this ) fail(__LINE__);
    }
};

S * S::a[];
int S::n;

struct F {
    ~F() {
	if( !( dtors == 3 && errors == 0 ) ) fail(__LINE__);
    }
} final;

S d3[] = { 1, 2, 3 };

int main()
{
    if( ctors != 3 ) fail(__LINE__);
    _PASS;
}
