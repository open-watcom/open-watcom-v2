// cast to void using function-like cast syntax doesn't work
int foo(int);

void bar()
{
    void(foo(1));
}

template <class T>
    struct SS {
	T a;
    };

template <class X>
    void foo( X );

template <class Q>
    char foo( SS<Q> );

template <class Q>
    char bar( SS<Q> );

template <class X>
    void bar( X );

void foo()
{
    SS<int> zz;

    foo( zz );
    bar( zz );
}

#include "diag0140.h"

int *same_as_struct;
int *same_as_enum;

class same_as_fn {
};
class same_as_var {
};
