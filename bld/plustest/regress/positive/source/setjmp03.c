#include "fail.h"
#include <setjmp.h>

jmp_buf x;

int c,d;

struct CD {
    CD() {
	++c;
    }
    ~CD() {
	++d;
    }
};

void die() {
    longjmp( x, 23 );
}

void bar() {
    CD x,y,z;
    die();
}

void foo() {
    CD x,y,z;
    bar();
}

void noCD() {
    int s = setjmp( x );
    if( s == 0 ) {
	foo();
	fail(__LINE__);
    } else {
	if( c != 6 ) fail(__LINE__);
	if( d != 6 ) fail(__LINE__);
	if( s != 23 ) fail(__LINE__);
    }
}

void withCD() {
    CD v;
    int s = setjmp( x );
    if( s == 0 ) {
	foo();
	fail(__LINE__);
    } else {
	if( c != 6+1+6 ) fail(__LINE__);
	if( d != 6+6 ) fail(__LINE__);
	if( s != 23 ) fail(__LINE__);
    }
}

int main() {
    noCD();
    withCD();
    if( d != 6+6+1 ) fail(__LINE__);
    _PASS;
}
