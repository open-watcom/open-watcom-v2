#include "fail.h"
#include <string.h>

void does_throw() {
    throw "asdf";
}

void test() {
    try {
	try {
	    does_throw();
	} catch( char *p ) {
	    if( strcmp( p, "asdf" ) ) fail(__LINE__);
	    throw -101;
	} catch( ... ) {
	    fail(__LINE__);
	}
    } catch( int x ) {
	if( x != -101 ) fail(__LINE__);
    } catch( ... ) {
	fail(__LINE__);
    }
}

int main() {
    test();
    _PASS;
}
