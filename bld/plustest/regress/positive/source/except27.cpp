#include "fail.h"

unsigned count;
unsigned thrown;

void test( int i ) {
    ++count;
    if( i % 5 ) {
	++thrown;
	throw -i;
    }
}

int main() {
    int i;

    for( i = 0; i < 100; ++i ) {
	try {
	    test( i );
	} catch( int x ) {
	    if( x != -i ) fail(__LINE__);
	    if( x & 1 ) continue;
	}
	test( 0 );
    }
    if( count != 160 ) fail(__LINE__);
    if( thrown != 80 ) fail(__LINE__);
    _PASS;
}
