#include "fail.h"

struct S {
    int a[16];
};

int k;

void foo() {
    ++k;
    throw S();
}

int main() {
    for( int i = 1; i < 10000; ++i ) {
	try {
	    foo();
	} catch( S const &r ) {
	    if( i != k ) fail(__LINE__);
	}
	if( i != k ) fail(__LINE__);
    }
    _PASS;
}
