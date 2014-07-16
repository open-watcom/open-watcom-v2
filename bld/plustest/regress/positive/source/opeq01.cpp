#include "fail.h"
#include <stdio.h>

unsigned called;


struct B {
    B& operator=(const B&)
    {
	++called;
	return *this;
    }
};

struct D : public B {
    D& operator=(const char*)
    {
	fail( __LINE__ );
	return *this;
    }
};

int main()
{
   D d1, d2;
   d1 = d2;
   if( !called ) fail(__LINE__);
    _PASS;
}
