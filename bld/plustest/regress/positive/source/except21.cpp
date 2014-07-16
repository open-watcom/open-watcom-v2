#include "fail.h"

struct x {
    static int count;
    int sig;
    x(int v) : sig(v) {
	++count;
    }
    x( x const &s ) : sig(s.sig) {
	++count;
    }
    ~x() {
	--count;
	if( count < 0 ) fail(__LINE__);
	sig = -1;
    }
};
int x::count;
struct x1 : x { x1() : x(1) {} };
void throw_x1() {
    throw x1();
}

struct x2 : x { x2() : x(2) {} };
void throw_x2() {
    throw x2();
}

struct x3 : x { x3() : x(3) {} };
void throw_x3() {
    throw x3();
}

struct x4 : x { x4() : x(4) {} };
void throw_x4() {
    throw x4();
}

struct x5 : x { x5() : x(5) {} };
void throw_x5() {
    throw x5();
}

void re_throw() {
    throw;
}

void test1() {
    try {
	throw_x1();
    } catch( x1 const &r ) {
	if( r.sig != 1 ) fail(__LINE__);
    } catch( ... ) {
	fail(__LINE__);
    }
}

void test2a() {
    try {
	re_throw();
    } catch( x2 const &r ) {
	if( r.sig != 2 ) fail(__LINE__);
    } catch( ... ) {
	fail(__LINE__);
    }
}

void test2() {
    try {
	throw_x2();
    } catch( x2 const &r ) {
	if( r.sig != 2 ) fail(__LINE__);
	test2a();
	if( r.sig != 2 ) fail(__LINE__);
    } catch( ... ) {
	fail(__LINE__);
    }
}

void test3b() {
    try {
	re_throw();
    } catch( x3 const &r ) {
	if( r.sig != 3 ) fail(__LINE__);
    } catch( ... ) {
	fail(__LINE__);
    }
}

void test3a() {
    try {
	re_throw();
    } catch( x3 const &r ) {
	if( r.sig != 3 ) fail(__LINE__);
	test3b();
	if( r.sig != 3 ) fail(__LINE__);
    } catch( ... ) {
	fail(__LINE__);
    }
}

void test3() {
    try {
	throw_x3();
    } catch( x3 const &r ) {
	if( r.sig != 3 ) fail(__LINE__);
	test3a();
	if( r.sig != 3 ) fail(__LINE__);
    } catch( ... ) {
	fail(__LINE__);
    }
}

void test4b() {
    try {
	throw_x5();
    } catch( x5 const &r ) {
	if( r.sig != 5 ) fail(__LINE__);
    } catch( ... ) {
	fail(__LINE__);
    }
}

void test4a() {
    try {
	re_throw();
    } catch( x4 const &r ) {
	if( r.sig != 4 ) fail(__LINE__);
	test4b();
	if( r.sig != 4 ) fail(__LINE__);
    } catch( ... ) {
	fail(__LINE__);
    }
}

void test4() {
    try {
	throw_x4();
    } catch( x4 const &r ) {
	if( r.sig != 4 ) fail(__LINE__);
	test4a();
	if( r.sig != 4 ) fail(__LINE__);
    } catch( ... ) {
	fail(__LINE__);
    }
}

int main() {
    try {
	test1();
    } catch( ... ) {
	fail(__LINE__);
    }
    if( x::count != 0 ) fail(__LINE__);
    x::count = 0;
    try {
	test2();
    } catch( ... ) {
	fail(__LINE__);
    }
    if( x::count != 0 ) fail(__LINE__);
    x::count = 0;
    try {
	test3();
    } catch( ... ) {
	fail(__LINE__);
    }
    if( x::count != 0 ) fail(__LINE__);
    x::count = 0;
    try {
	test4();
    } catch( ... ) {
	fail(__LINE__);
    }
    if( x::count != 0 ) fail(__LINE__);
    _PASS;
}
