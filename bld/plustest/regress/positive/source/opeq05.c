#include "fail.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned v_assignments;

struct V {
    V & operator =( V const & )
    {
	++v_assignments;
	return *this;
    }
};
struct S : virtual V {
};
struct P : virtual V {
};
struct T : P, S {
};
struct X : T {
};

void foo( T &x, T &y )
{
    x = y;
}

int main()
{
    X d;
    X s;

    foo( d, s );
    if( v_assignments != 1 ) fail(__LINE__);
    _PASS;
}
