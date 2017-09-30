#include "fail.h"

int locks;

struct L {
    L() {
	++locks;
    }
    ~L() {
	--locks;
    }
};

void foo2() {
    L lock3;
    throw 0;
}

void foo() {
    L lock2;
    foo2();
}

int caught;

int main()
{
    try {
	L lock1;
	foo();
    } catch( int x ) {
	if( x != 0 ) fail(__LINE__);
	++caught;
    } catch( ... ) {
	fail(__LINE__);
    }
    return( errors != 0 );
}

int exit_in_progress;

struct D {
    ~D() {
	if( locks != 0 ) fail(__LINE__);
	if( !caught ) fail(__LINE__);
	if( ! exit_in_progress ) {
	    ++exit_in_progress;
	    _PASS_EXIT;
	}
    }
};

D __dummy;
