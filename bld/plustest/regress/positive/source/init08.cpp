#include "fail.h"

unsigned name;
unsigned count;

struct S {
    int n;
    S() : n(++name) { ++count; }
    S( S const &s ) : n(s.n) { ++count; }
    ~S() { --count; }
};

int main()
{
    {
	S &r1 = S();
	S const &r2 = S();
	if( count != 2 ) fail(__LINE__);
	{
	    if( count != 2 ) fail(__LINE__);
	    {
		S x;
		if( count != 3 ) fail(__LINE__);
	    }
	    if( count != 2 ) fail(__LINE__);
	}
	if( count != 2 ) fail(__LINE__);
    }
    if( count != 0 ) fail(__LINE__);
    _PASS;
}
