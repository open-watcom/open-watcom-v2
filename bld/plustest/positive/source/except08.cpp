#include "fail.h"
#include <stdio.h>

unsigned i_ctors;
unsigned s_ctors;

typedef int exception;

struct I {
    int x;
    I();
    ~I();
};

struct S {
    I x;
    S( const char * filename = 0 ) throw (exception);
    ~S();
};
  

int main()
{
    {
	S x;
	{
	    S x;
	    {
		S x;
	    }
	}
    }
    if( i_ctors != 0 || s_ctors != 0 ) fail(__LINE__);
    _PASS;
}

I::I() { x=0; ++i_ctors; }
I::~I() { --i_ctors; }

S::S ( const char *filename) throw (exception) {
    ++s_ctors;
}

S::~S() {
    --s_ctors;
}
