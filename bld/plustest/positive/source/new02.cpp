#include "fail.h"
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <new.h>

struct T {
    int a;
    virtual ~T() {}
};
struct P {
    int a;
    virtual ~P() {}
};
struct S : P, T {
    int a[100];
    void operator delete( void *p, size_t size )
    {
	if( size < sizeof( S ) ) {
	    fail(__LINE__);
	}
	::delete p;
    }
};

int main()
{
    P *t1 = new S;
    P *t2 = new S;
    T *t3 = new S[10];
    T *t4 = new S[10];

    delete t1;
    delete t2;
    delete [] t3;
    delete [] t4;
    _PASS;
}
