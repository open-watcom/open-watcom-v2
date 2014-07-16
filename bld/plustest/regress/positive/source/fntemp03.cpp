#include "fail.h"
// type decay before argument binding
#include <stdio.h>

template <class T>
    void foo( char *p, void r(char *), T )
    {
	r(p);
    }

int got_here;

void dump( char *p )
{
    char *q = p;
    ++got_here;
}

int main()
{
    foo( "????", dump, 1 );
    if( ! got_here ) fail(__LINE__);
    _PASS;
}
