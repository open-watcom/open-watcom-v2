#include "fail.h"

struct U;
extern U x;

U &r = x;

void foo( U &r ) {
    if( &r != &x ) fail(__LINE__);
}

int main() {
    foo( x );
    _PASS;
}

/* pretend this is another module... */

struct U {
    U() : i(1) {
    }
    ~U() {
	if( i != 1 ) fail(__LINE__);
    }
    int i;
};

U x;
