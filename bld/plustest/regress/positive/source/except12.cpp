#include "fail.h"
#include <stdio.h>


struct except {
    int error;
    int kind;
    static int count;
    except( int k = 0 ) : kind(k), error(0) { ++count; }
    except( except const &s ) : kind(s.kind), error(0) { ++count; }
    ~except() { --count; if( !error && count < 0 ) (count=0),fail(__LINE__); }
};
struct except1 : except {
    except1() : except(1) {}
    ~except1() { if( count <= 0 ) ++error,fail(__LINE__); }
};
struct except2 : except {
    except2() : except(2) {}
    ~except2() { if( count <= 0 ) ++error,fail(__LINE__); }
};
struct except3 : except {
    except3() : except(3) {}
    ~except3() { if( count <= 0 ) ++error,fail(__LINE__); }
};
int except::count;

unsigned handles = 3;

struct scarce {
    int h;
    int open;
    scarce() : h(handles), open(0) {
	if( handles == 0 ) {
	    throw except();
	} else {
	    --handles;
	}
    }
    ~scarce() {
	if( h != ( handles + 1 ) ) {
	    fail(__LINE__);
	} else {
	    ++handles;
	}
    }
    void use() {
	++open;
    }
    void validate() {
	if( !open || !h ) fail(__LINE__);
	switch( h ) {
	case 1:
	    throw except1();
	    break;
	case 2:
	    throw except2();
	    break;
	case 3:
	    throw except3();
	    break;
	default:
	    fail(__LINE__);
	}
    }
};

void test( int expecting ) {
    scarce *p = NULL;

    try {
	p = new scarce();
	p->use();
	p->validate();
    } catch( except1 const &r ) {
	if( expecting != 1 ) fail(__LINE__);
	delete p;
	throw except1();
    } catch( except2 const &r ) {
	if( expecting != 2 ) fail(__LINE__);
	delete p;
	throw except2();
    } catch( except3 const &r ) {
	if( expecting != 3 ) fail(__LINE__);
	delete p;
	throw except3();
    } catch( except const &r ) {
	if( expecting != 0 ) fail(__LINE__);
	delete p;
	throw except();
    } catch( ... ) {
	fail(__LINE__);
	delete p;
	throw __LINE__;
    }
}

void foo() {
    int expecting = 3;
    scarce *u3 = NULL;
    scarce *u2 = NULL;
    scarce *u1 = NULL;
    try {
	test(3);
    } catch( except1 const &r ) {
	if( expecting != 1 ) fail(__LINE__);
    } catch( except2 const &r ) {
	if( expecting != 2 ) fail(__LINE__);
    } catch( except3 const &r ) {
	if( expecting != 3 ) fail(__LINE__);
    } catch( except const &r ) {
	if( expecting != 0 ) fail(__LINE__);
    } catch( ... ) {
	fail(__LINE__);
    }
    try {
	u3 = new scarce;
	--expecting;
	test(2);
    } catch( except1 const &r ) {
	if( expecting != 1 ) fail(__LINE__);
    } catch( except2 const &r ) {
	if( expecting != 2 ) fail(__LINE__);
    } catch( except3 const &r ) {
	if( expecting != 3 ) fail(__LINE__);
    } catch( except const &r ) {
	if( expecting != 0 ) fail(__LINE__);
    } catch( ... ) {
	fail(__LINE__);
    }
    try {
	u2 = new scarce;
	--expecting;
	test(1);
    } catch( except1 const &r ) {
	if( expecting != 1 ) fail(__LINE__);
    } catch( except2 const &r ) {
	if( expecting != 2 ) fail(__LINE__);
    } catch( except3 const &r ) {
	if( expecting != 3 ) fail(__LINE__);
    } catch( except const &r ) {
	if( expecting != 0 ) fail(__LINE__);
    } catch( ... ) {
	fail(__LINE__);
    }
    try {
	u1 = new scarce;
	--expecting;
	test(0);
    } catch( except1 const &r ) {
	if( expecting != 1 ) fail(__LINE__);
    } catch( except2 const &r ) {
	if( expecting != 2 ) fail(__LINE__);
    } catch( except3 const &r ) {
	if( expecting != 3 ) fail(__LINE__);
    } catch( except const &r ) {
	if( expecting != 0 ) fail(__LINE__);
    } catch( ... ) {
	fail(__LINE__);
    }
    delete u1;
    delete u2;
    delete u3;
}

int main()
{
    foo();
    _PASS;
}
