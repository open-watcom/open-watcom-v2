#include "fail.h"
#include <stdio.h>

unsigned dtors;

struct S {
    ~S() { ++dtors; }
};
template <class T>
    struct C {
	void free() { val->~T(); }
	T *val;
    };

C<S> x;
C<int> y;
C<void *> z;
C<void> w;

int main()
{
    x.free();
    y.free();
    z.free();
    w.free();
    if( dtors != 1 ) fail(__LINE__);
    _PASS;
}
