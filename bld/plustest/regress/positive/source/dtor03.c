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
    B() {
	if( sequence != 1 ) fail(__LINE__);
	++sequence;
    }
    ~B(){
	if( sequence != 3 ) fail(__LINE__);
	++sequence;
    }
};

struct D : B {
    D() {
	O x;
	throw 28;
    }
};

int main()
{
    if( sequence != 0 ) fail(__LINE__);
    ++sequence;
    try {
	D x;
    } catch( int x ) {
	if( x != 28 ) fail(__LINE__);
    } catch( ... ) {
	fail(__LINE__);
    }
    if( sequence != 4 ) fail(__LINE__);
    _PASS;
}
