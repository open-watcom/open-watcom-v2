#include "fail.h"

void operator delete( void * )
{
    fail(__LINE__);
}

int count;

struct S {
    S(){ ++count; }
    ~S(){ --count; if( count == 0 ) fail(__LINE__); }
};

int main() {
    {
	S &x1 = *new S;
	S &x2 = *new S;
	S &x3 = *new S;
	S v1;
    }
    if( count != 3 ) fail(__LINE__);
    _PASS;
}
