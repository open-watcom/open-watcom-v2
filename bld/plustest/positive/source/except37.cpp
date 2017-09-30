#include "fail.h"

static int except_called = 0;
static int except_caught = 0;

struct S {
    S() { except_called++; };
};
static int I = 0;
static int FuncCalled = 0;

int foo()
{
    if( I==0 ) {
	throw S();
    }
    return 1;
}

int func( int i = 2,  int j = (I==0 ? throw S() : I) )
{
    FuncCalled++;
    return 1;
}

int funcfoo( int i = 2,  int j = foo() )
{
    FuncCalled++;
    return 1;
}

int main()
{
    I = 0;
    try {
        func(1);

    } catch(S) {
	except_caught++;
    }
    if( except_called != 1 || except_caught != 1 || FuncCalled != 0 ) {
	fail(__LINE__);
    }
    try {
        funcfoo(1);

    } catch(S) {
	except_caught++;
    }
    if( except_called != 2 || except_caught != 2 || FuncCalled != 0) {
	fail(__LINE__);
    }
    I = 1;
    try {
        func();
	funcfoo();
    } catch(S) {
	except_caught++;
    }
    if( except_called != 2 || except_caught != 2 || FuncCalled != 2) {
	fail(__LINE__);
    }
    try {
	func(1,2);
	funcfoo(1,2);
    } catch(S) {
	except_caught++;
    }
    if( except_called != 2 || except_caught != 2 || FuncCalled != 4) {
	fail(__LINE__);
    }
    _PASS;
}
