#include "fail.h"
#include <stdio.h>

unsigned sequence;

struct O {
    ~O(){
	if( sequence != 2 ) fail(__LINE__);
	++sequence;
    }
};

struct B {
    ~B(){
	if( sequence != 3 ) fail(__LINE__);
	++sequence;
    }
};

struct D : B {
    ~D(){
	if( sequence != 1 ) fail(__LINE__);
	++sequence;
	O x;
    }
};

int main()
{
    if( sequence != 0 ) fail(__LINE__);
    ++sequence;
    {
	D x;
    }
    if( sequence != 4 ) fail(__LINE__);
    _PASS;
}
