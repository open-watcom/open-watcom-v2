#include "fail.h"
#include <stdio.h>


int expected;

struct S {
    void foo(int s) { if( expected != 1 || s != 1 ) fail(__LINE__); }
    void bar(int s) { if( expected != 0 || s != 0 ) fail(__LINE__); }
    static void _foo(int s) { if( expected != 1 || s != 1 ) fail(__LINE__); }
    static void _bar(int s) { if( expected != 0 || s != 0 ) fail(__LINE__); }
    void test(int s)
    {
	(this->*(s ? &S::foo : &S::bar))(s);
	( s ? _foo : _bar)(s);
    }
};

int main()
{
    S s;
    expected = 1;
    s.test(1);
    expected = 0;
    s.test(0);
    _PASS;
}
