#include "fail.h"
#include <string.h>

struct B {
    void operator delete( void *pv, unsigned s );
    void *operator new( unsigned s );
    void operator delete []( void *pv, unsigned s );
    void *operator new []( unsigned s );
    virtual ~B();
};

void B::operator delete( void *pv, unsigned s )
{
    char *pc = (char*) pv;
    memset( pc, -__LINE__, s );
    delete [] pc;
}

void *B::operator new( unsigned s )
{
    return new char[s];
}

void B::operator delete []( void *pv, unsigned s )
{
    char *pc = (char*) pv;
    memset( pc, -__LINE__, s );
    delete [] pc;
}

void *B::operator new []( unsigned s )
{
    return new char[s];
}

virtual B::~B() {
}

unsigned dcount;

struct D : B {
    ~D();
};

D::~D() {
    ++dcount;
}

void test() {
    B *p = new D[10];
    delete [] p;
}

void zap() {
    static void *p;
    char buff[1000];
    memset( buff, 0xdd, sizeof( buff ) );
    p = buff;
}

int main() {
    {
	D a[10];
	D x,y,z;
	test();
	zap();
    }
    if( dcount != 10+1+1+1+10 ) fail(__LINE__);
    _PASS;
}
